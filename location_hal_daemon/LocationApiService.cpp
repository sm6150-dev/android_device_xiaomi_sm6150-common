/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include <SystemStatus.h>
#include <LocationApiMsg.h>
#include <gps_extended_c.h>

#include <LocHalDaemonIPCReceiver.h>
#include <LocHalDaemonIPCSender.h>
#include <LocHalDaemonClientHandler.h>
#include <LocationApiService.h>

#define SERVICE_NAME "locapiservice"

typedef void* (getLocationInterface)();

/******************************************************************************
LocHalDaemonClientHandler - static members
******************************************************************************/
LocHalDaemonIPCSender* LocHalDaemonClientHandler::mLocIpcSender = nullptr;

/******************************************************************************
LocationApiService - static members
******************************************************************************/
LocationApiService* LocationApiService::mInstance = nullptr;
std::mutex LocationApiService::mMutex;

/******************************************************************************
LocationApiService - constructors
******************************************************************************/
LocationApiService::LocationApiService(uint32_t autostart, uint32_t sessiontbfms) :

    mLocationControlId(0),
    mAutoStartGnss(autostart),
    mGnssSessionTbfMs(sessiontbfms) {

    LOC_LOGd("mAutoStartGnss=%u", mAutoStartGnss);
    LOC_LOGd("mGnssSessionTbfMs=%u", mGnssSessionTbfMs);

    // set callback functions for Location API
    mCallbacks.size = sizeof(mCallbacks);
    // mandatory callback
    mCallbacks.capabilitiesCb = [this](LocationCapabilitiesMask mask) {
        onCapabilitiesCallback(mask);
    };
    mCallbacks.responseCb = [this](LocationError err, uint32_t id) {
        onResponseCb(err, id);
    };

    mCallbacks.collectiveResponseCb =
            [this](size_t count, LocationError* errs, uint32_t* ids) {
        onCollectiveResponseCallback(count, errs, ids);
    };

    // optional
    mCallbacks.trackingCb = [this](Location location) {
        onTrackingCb(location);
    };
    mCallbacks.gnssLocationInfoCb = [this](GnssLocationInfoNotification notification) {
        onGnssLocationInfoCb(notification);
    };
    mCallbacks.gnssSvCb = [this](GnssSvNotification notification) {
        onGnssSvCb(notification);
    };
    mCallbacks.gnssNmeaCb = [this](GnssNmeaNotification notification) {
        onGnssNmeaCb(notification);
    };
    mCallbacks.gnssMeasurementsCb = nullptr;
    mCallbacks.gnssNiCb = nullptr;

    mCallbacks.geofenceBreachCb = nullptr;
    mCallbacks.geofenceStatusCb = nullptr;
    mCallbacks.batchingCb = nullptr;
    mCallbacks.batchingStatusCb = nullptr;

    // create Location control API
    mControlCallabcks.size = sizeof(mControlCallabcks);
    mControlCallabcks.responseCb = [this](LocationError err, uint32_t id) {
        onControlResponseCallback(err, id);
    };
    mControlCallabcks.collectiveResponseCb =
            [this](size_t count, LocationError *errs, uint32_t *ids) {
        onControlCollectiveResponseCallback(count, errs, ids);
    };
    mLocationControlApi = LocationControlAPI::createInstance(mControlCallabcks);
    if (nullptr == mLocationControlApi) {
        LOC_LOGd("Failed to create LocationControlAPI");
        return;
    }

    // enable
    mLocationControlId = mLocationControlApi->enable(LOCATION_TECHNOLOGY_TYPE_GNSS);
    LOC_LOGd("-->enable=%u", mLocationControlId);
    // this is a unique id assigned to this daemon - will be used when disable

    // create IPC receiver
    mIpcReceiver = new LocHalDaemonIPCReceiver(this);
    if (nullptr == mIpcReceiver) {
        LOC_LOGd("Failed to create LocHalDaemonIPCReceiver");
        return;
    }

    // create a default client if enabled by config
    if (mAutoStartGnss) {
        LOC_LOGd("--> Starting a default client...");
        LocHalDaemonClientHandler* pClient = new LocHalDaemonClientHandler("default", mCallbacks);
        mClients.emplace("default", pClient);

        LocationOptions option = {0};
        option.size = sizeof(option);
        option.minDistance = 0;
        option.minInterval = mGnssSessionTbfMs;
        option.mode = GNSS_SUPL_MODE_STANDALONE;
        uint32_t sessionid = pClient->mLocationApi->startTracking(option);
        LOC_LOGd("--> Starting a default client...sessionid=%d", sessionid);

        pClient->mSessionId = sessionid;
        pClient->mSubscriptionMask = 0;
        pClient->mPendingMessages.push(E_LOCAPI_START_TRACKING_MSG_ID);
    }

    // start receiver - never return
    LOC_LOGd("Ready");
    mIpcReceiver->start();
}

LocationApiService::~LocationApiService() {

    // stop ipc receiver thread
    if (nullptr != mIpcReceiver) {
        mIpcReceiver->stop();
        delete mIpcReceiver;
    }

    // delete client
    for (auto each : mClients) {
        LOC_LOGd(">-- deleted client [%s]", each.first.c_str());
        delete each.second;
    }

    // delete location contorol API handle
    mLocationControlApi->disable(mLocationControlId);
    mLocationControlApi->destroy();
}

void LocationApiService::onListenerReady() {

    // traverse client sockets directory - then broadcast READY message
    LOC_LOGd(">-- onListenerReady Finding client sockets...");

    DIR *dirp = opendir(SOCKET_DIR_TO_CLIENT);
    if (!dirp) {
        return;
    }

    struct dirent *dp = nullptr;
    struct stat sbuf = {0};
    while (nullptr != (dp = readdir(dirp))) {
        std::string fname = SOCKET_DIR_TO_CLIENT;
        fname += dp->d_name;
        if (-1 == lstat(fname.c_str(), &sbuf)) {
            continue;
        }
        if ('.' == (dp->d_name[0])) {
            continue;
        }
        if (S_ISSOCK((mode_t)sbuf.st_mode)) {
            LOC_LOGd("<-- Sending ready to socket: %s", fname.c_str());
            LocHalDaemonIPCSender* pIpcSender = new LocHalDaemonIPCSender(fname.c_str());
            LocAPIHalReadyIndMsg msg(SERVICE_NAME);
            pIpcSender->send(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
            delete pIpcSender;
        }
    }
    closedir(dirp);
}

/******************************************************************************
LocationApiService - implementation - registration
******************************************************************************/
void LocationApiService::newClient(LocAPIClientRegisterReqMsg *pMsg) {

    std::lock_guard<std::mutex> lock(mMutex);
    std::string clientname(pMsg->mSocketName);

    // if this name is already used return error
    if (mClients.find(clientname) != mClients.end()) {
        LOC_LOGe("invalid client=%s already existing", clientname.c_str());
        return;
    }

    // store it in client property database
    LocHalDaemonClientHandler *pClient = new LocHalDaemonClientHandler(clientname, mCallbacks);
    mClients.emplace(clientname, pClient);
    LOC_LOGi(">-- registered new client=%s", clientname.c_str());
}

void LocationApiService::deleteClient(LocAPIClientDeregisterReqMsg *pMsg) {

    std::lock_guard<std::mutex> lock(mMutex);
    std::string clientname(pMsg->mSocketName);
    LocHalDaemonClientHandler* pClient = getClient(pMsg->mSocketName);

    // delete this client from property db
    mClients.erase(clientname);
    if (!pClient) {
        LOC_LOGe(">-- deleteClient invlalid client=%s", pMsg->mSocketName);
        return;
    }
    delete pClient;

    LOC_LOGi(">-- deleteClient client=%s", clientname.c_str());
}

/******************************************************************************
LocationApiService - implementation - tracking
******************************************************************************/
uint32_t LocationApiService::startTracking(LocAPIStartTrackingReqMsg *pMsg) {

    std::lock_guard<std::mutex> lock(mMutex);
    LocHalDaemonClientHandler* pClient = getClient(pMsg->mSocketName);
    if (!pClient) {
        LOC_LOGe(">-- start invlalid client=%s", pMsg->mSocketName);
        return 0;
    }

    // create option
    LocationOptions option = { 0 };
    option.size = sizeof(option);
    option.minDistance = pMsg->distanceInMeters;
    option.minInterval = pMsg->intervalInMs;
    option.mode = GNSS_SUPL_MODE_STANDALONE;

    uint32_t sessionid = pClient->mLocationApi->startTracking(option);
    if (!sessionid) {
        LOC_LOGe("Failed to start session");
        return 0;
    }
    // success
    pClient->mSessionId = sessionid;
    pClient->mPendingMessages.push(E_LOCAPI_START_TRACKING_MSG_ID);

    LOC_LOGi(">-- start started session id=%u", sessionid);
    return sessionid;
}

void LocationApiService::stopTracking(LocAPIStopTrackingReqMsg *pMsg) {

    std::lock_guard<std::mutex> lock(mMutex);
    LocHalDaemonClientHandler* pClient = getClient(pMsg->mSocketName);
    if (!pClient) {
        LOC_LOGe(">-- start invlalid client=%s", pMsg->mSocketName);
        return;
    }

    pClient->mSubscriptionMask = 0;
    pClient->mLocationApi->stopTracking(pClient->mSessionId);
    pClient->mPendingMessages.push(E_LOCAPI_STOP_TRACKING_MSG_ID);

    LOC_LOGi(">-- stop sessionid=%u", pClient->mSessionId);
}

void LocationApiService::updateSubscription(LocAPIUpdateCallbacksReqMsg *pMsg) {

    std::lock_guard<std::mutex> lock(mMutex);
    LocHalDaemonClientHandler* pClient = getClient(pMsg->mSocketName);
    if (!pClient) {
        LOC_LOGe(">-- updateSubscription invlalid client=%s", pMsg->mSocketName);
        return;
    }

    pClient->mSubscriptionMask = pMsg->locationCallbacks;

    // update callback
    mCallbacks.trackingCb = [this](Location location) {
        onTrackingCb(location);
    };
    if (pClient->mSubscriptionMask & E_LOC_CB_GNSS_LOCATION_INFO_BIT) {
        mCallbacks.gnssLocationInfoCb = [this](GnssLocationInfoNotification notification) {
            onGnssLocationInfoCb(notification);
        };
    } else {
        mCallbacks.gnssLocationInfoCb = nullptr;
    }
    if (pClient->mSubscriptionMask & E_LOC_CB_GNSS_SV_BIT) {
        mCallbacks.gnssSvCb = [this](GnssSvNotification notification) {
            onGnssSvCb(notification);
        };
    } else {
        mCallbacks.gnssSvCb = nullptr;
    }
    if (pClient->mSubscriptionMask & E_LOC_CB_GNSS_NMEA_BIT) {
        mCallbacks.gnssNmeaCb = [this](GnssNmeaNotification notification) {
            onGnssNmeaCb(notification);
        };
    } else {
        mCallbacks.gnssNmeaCb = nullptr;
    }

    pClient->mLocationApi->updateCallbacks(mCallbacks);

    LOC_LOGi(">-- update subscription client=%s mask=0x%x",
            pMsg->mSocketName, pMsg->locationCallbacks);
}

void LocationApiService::updateTrackingOptions(LocAPIUpdateTrackingOptionsReqMsg *pMsg) {

    std::lock_guard<std::mutex> lock(mMutex);

    // create option
    LocationOptions option = { 0 };
    option.size = sizeof(option);
    option.minDistance = pMsg->distanceInMeters;
    option.minInterval = pMsg->intervalInMs;
    option.mode = GNSS_SUPL_MODE_STANDALONE;

    LocHalDaemonClientHandler* pClient = getClient(pMsg->mSocketName);
    if (pClient) {
        pClient->mLocationApi->updateTrackingOptions(pClient->mSessionId, option);
        pClient->mPendingMessages.push(E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID);
    }

    LOC_LOGi(">-- update tracking options");
}

void LocationApiService::updateNetworkAvailability(bool availability) {

    LOC_LOGi(">-- updateNetworkAvailability=%u", availability);
    GnssInterface* gnssInterface = getGnssInterface();
    if (gnssInterface) {
        gnssInterface->updateConnectionStatus(
                availability, loc_core::NetworkInfoDataItemBase::TYPE_UNKNOWN);
    }
}

/******************************************************************************
LocationApiService - Location Control API callback functions
******************************************************************************/
void LocationApiService::onControlResponseCallback(LocationError err, uint32_t id) {
    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onControlResponseCallback err=%u id=%u", err, id);
}

void LocationApiService::onControlCollectiveResponseCallback(
    size_t count, LocationError *errs, uint32_t *ids) {
    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onControlCollectiveResponseCallback");
}

/******************************************************************************
LocationApiService - Location API response callback functions
******************************************************************************/
void LocationApiService::onResponseCb(LocationError err, uint32_t id) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onResponseCb err=%u id=%u", err, id);

    // find matching client by session id
    ELocMsgID pendingMsgId = E_LOCAPI_UNDEFINED_MSG_ID;
    LocHalDaemonClientHandler *client = nullptr;
    std::string clientname = "";
    for (auto each : mClients) {
        if (id == each.second->mSessionId) {
            clientname = each.first;
            client = each.second;
            if (!each.second->mPendingMessages.empty()) {
                pendingMsgId = each.second->mPendingMessages.front();
                each.second->mPendingMessages.pop();
            }
            break;
        }
    }
    // exit if matching client is default or invalid
    if (!client || clientname == "default") {
        return;
    }

    int rc = 0;
    // send corresponding indication message if pending
    switch (pendingMsgId) {
        case E_LOCAPI_START_TRACKING_MSG_ID: {
            LOC_LOGd("<-- start resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_START_TRACKING_MSG_ID, err);
            rc = client->sendMessage(msg);
            break;
        }
        case E_LOCAPI_STOP_TRACKING_MSG_ID: {
            LOC_LOGd("<-- stop resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_STOP_TRACKING_MSG_ID, err);
            rc = client->sendMessage(msg);
            client->mSessionId = 0;
            break;
        }
        case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID: {
            LOC_LOGd("<-- update resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID, err);
            rc = client->sendMessage(msg);
            break;
        }
        default: {
            LOC_LOGe("no pending message for %s", clientname.c_str());
            return;
        }
    }

    // purge this client if failed
    if (!rc) {
        LOC_LOGe("failed rc=%d purging client=%s", rc, clientname.c_str());
        mClients.erase(clientname);
        delete client;
    }
}

void LocationApiService::onCollectiveResponseCallback(
        size_t count, LocationError *errs, uint32_t *ids) {
    LOC_LOGd("--< onCollectiveResponseCallback");
}

/******************************************************************************
LocationApiService - Location API callback functions
******************************************************************************/
void LocationApiService::onCapabilitiesCallback(LocationCapabilitiesMask mask) {

    std::lock_guard<std::mutex> lock(mMutex);
    //LOC_LOGd("--< onCapabilitiesCallback=0x%" PRIx64" controlId=%u", mask, mLocationControlId);
    LOC_LOGd("--< onCapabilitiesCallback=0x%x controlId=%u", mask, mLocationControlId);

    // broadcast
    LocAPICapabilitiesIndMsg msg(SERVICE_NAME, mask);
    for (auto each : mClients) {
        if ((nullptr != each.second) && (mask != each.second->mCapabilityMask)) {
            LOC_LOGd("[%s] : mask old=0x%x new=0x%x",
                    each.first.c_str(), each.second->mCapabilityMask, mask);
            each.second->mCapabilityMask = mask;
            if (each.first != "default") {
                int rc = each.second->sendMessage(msg);
                // purge this client if failed
                if (!rc) {
                    LOC_LOGe("failed rc=%d purging client=%s", rc, each.first.c_str());
                    mClients.erase(each.first);
                    delete each.second;
                }
            }
        }
    }
}

void LocationApiService::onTrackingCb(Location location) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onTrackingCb");

    // broadcast
    LocAPILocationIndMsg msg(SERVICE_NAME, location);
    for (auto each : mClients) {
        if ((each.first != "default") &&
                (each.second->mSubscriptionMask & E_LOC_CB_TRACKING_BIT)) {
            int rc = each.second->sendMessage(msg);
            // purge this client if failed
            if (!rc) {
                LOC_LOGe("failed rc=%d purging client=%s", rc, each.first.c_str());
                mClients.erase(each.first);
                delete each.second;
            }
        }
    }
}

void LocationApiService::onGnssLocationInfoCb(GnssLocationInfoNotification notification) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onGnssLocationInfoCb");

    // broadcast
    LocAPILocationInfoIndMsg msg(SERVICE_NAME, notification);
    for (auto each : mClients) {
        if ((each.first != "default") &&
                (each.second->mSubscriptionMask & E_LOC_CB_GNSS_LOCATION_INFO_BIT)) {
            int rc = each.second->sendMessage(msg);
            // purge this client if failed
            if (!rc) {
                LOC_LOGe("failed rc=%d purging client=%s", rc, each.first.c_str());
                mClients.erase(each.first);
                delete each.second;
            }
        }
    }
}

void LocationApiService::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onGnssNiCb");
}

void LocationApiService::onGnssSvCb(GnssSvNotification notification) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onGnssSvCb");

    // broadcast
    LocAPISatelliteVehicleIndMsg msg(SERVICE_NAME, notification);
    for (auto each : mClients) {
        if ((each.first != "default") &&
                (each.second->mSubscriptionMask & E_LOC_CB_GNSS_SV_BIT)) {
            int rc = each.second->sendMessage(msg);
            // purge this client if failed
            if (!rc) {
                LOC_LOGe("failed rc=%d purging client=%s", rc, each.first.c_str());
                mClients.erase(each.first);
                delete each.second;
            }
        }
    }
}

void LocationApiService::onGnssNmeaCb(GnssNmeaNotification notification) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onGnssNmeaCb time=%" PRIu64" len=%zu nmea=%s",
            notification.timestamp,
            notification.length,
            notification.nmea);

    // serialize nmea string into ipc message payload
    size_t msglen = sizeof(LocAPINmeaIndMsg) + notification.length;
    uint8_t *msg = new(nothrow) uint8_t[msglen];
    if (nullptr == msg) {
        return;
    }
    memset(msg, 0, msglen);
    LocAPINmeaIndMsg *pmsg = reinterpret_cast<LocAPINmeaIndMsg*>(msg);
    strlcpy(pmsg->mSocketName, SERVICE_NAME, MAX_SOCKET_PATHNAME_LENGTH);
    pmsg->msgId = E_LOCAPI_NMEA_MSG_ID;
    pmsg->gnssNmeaNotification.size = msglen;
    pmsg->gnssNmeaNotification.timestamp = notification.timestamp;
    pmsg->gnssNmeaNotification.length = notification.length;
    memcpy(&(pmsg->gnssNmeaNotification.nmea[0]), notification.nmea, notification.length);

    // broadcast
    for (auto each : mClients) {
        if ((each.first != "default") &&
                (each.second->mSubscriptionMask & E_LOC_CB_GNSS_NMEA_BIT)) {
            int rc = each.second->sendMessage(msg, msglen);
            // purge this client if failed
            if (!rc) {
                LOC_LOGe("failed rc=%d purging client=%s", rc, each.first.c_str());
                mClients.erase(each.first);
                delete each.second;
            }
        }
    }

    delete[] msg;
}

void LocationApiService::onGnssMeasurementsCb(GnssMeasurementsNotification notification) {

    std::lock_guard<std::mutex> lock(mMutex);
    LOC_LOGd("--< onGnssMeasurementsCb");
}

/******************************************************************************
LocationApiService - other utilities
******************************************************************************/
GnssInterface* LocationApiService::getGnssInterface() {

    static bool getGnssInterfaceFailed = false;
    static GnssInterface* gnssInterface = nullptr;

    if (nullptr == gnssInterface && !getGnssInterfaceFailed) {
        LOC_LOGd("Loading libgnss.so::getGnssInterface ...");
        getLocationInterface* getter = NULL;
        const char *error = NULL;
        dlerror();
        void *handle = dlopen("libgnss.so", RTLD_NOW);
        if (nullptr == handle) {
            LOC_LOGe("dlopen for libgnss.so failed");
        } else if (nullptr != (error = dlerror()))  {
            LOC_LOGe("dlopen for libgnss.so failed, error = %s", error);
        } else {
            getter = (getLocationInterface*)dlsym(handle, "getGnssInterface");
            if ((error = dlerror()) != NULL)  {
                LOC_LOGe("dlsym for getGnssInterface failed, error = %s", error);
                getter = NULL;
            }
        }

        if (nullptr == getter) {
            getGnssInterfaceFailed = true;
        } else {
            gnssInterface = (GnssInterface*)(*getter)();
        }
    }
    return gnssInterface;
}

