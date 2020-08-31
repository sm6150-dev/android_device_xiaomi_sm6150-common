/* Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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

#include <cinttypes>
#include <gps_extended_c.h>
#include <LocationApiMsg.h>
#include <LocHalDaemonClientHandler.h>
#include <LocationApiService.h>

shared_ptr<LocIpcSender> LocHalDaemonClientHandler::createSender(const string socket) {
    SockNode sockNode(SockNode::create(socket));
    return sockNode.createSender(true);
}

/******************************************************************************
LocHalDaemonClientHandler - updateSubscriptionMask
******************************************************************************/
void LocHalDaemonClientHandler::updateSubscription(uint32_t mask) {

    // update my subscription mask
    mSubscriptionMask = mask;

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

    if (mSubscriptionMask & E_LOC_CB_DISTANCE_BASED_TRACKING_BIT) {
        mCallbacks.trackingCb = [this](Location location) {
            onTrackingCb(location);
        };
    }

    // batching
    if (mSubscriptionMask & E_LOC_CB_BATCHING_BIT) {
        mCallbacks.batchingCb = [this](size_t count, Location* location,
                BatchingOptions batchingOptions) {
            onBatchingCb(count, location, batchingOptions);
        };
    } else {
        mCallbacks.batchingCb = nullptr;
    }
    // batchingStatus
    if (mSubscriptionMask & E_LOC_CB_BATCHING_STATUS_BIT) {
        mCallbacks.batchingStatusCb = [this](BatchingStatusInfo batchingStatus,
                std::list<uint32_t>& listOfCompletedTrips) {
            onBatchingStatusCb(batchingStatus, listOfCompletedTrips);
        };
    } else {
        mCallbacks.batchingStatusCb = nullptr;
    }
    //Geofence Breach
    if (mSubscriptionMask & E_LOC_CB_GEOFENCE_BREACH_BIT) {
        mCallbacks.geofenceBreachCb = [this](GeofenceBreachNotification geofenceBreachNotif) {
            onGeofenceBreachCb(geofenceBreachNotif);
        };
    } else {
        mCallbacks.geofenceBreachCb = nullptr;
    }

    // location info
    if (mSubscriptionMask & (E_LOC_CB_GNSS_LOCATION_INFO_BIT | E_LOC_CB_SIMPLE_LOCATION_INFO_BIT)) {
        mCallbacks.gnssLocationInfoCb = [this](GnssLocationInfoNotification notification) {
            onGnssLocationInfoCb(notification);
        };
    } else {
        mCallbacks.gnssLocationInfoCb = nullptr;
    }

    // engine locations info
    if (mSubscriptionMask & E_LOC_CB_ENGINE_LOCATIONS_INFO_BIT) {
        mCallbacks.engineLocationsInfoCb =
                [this](uint32_t count, GnssLocationInfoNotification* notificationArr) {
            onEngLocationsInfoCb(count, notificationArr);
        };
    } else {
        mCallbacks.engineLocationsInfoCb = nullptr;
    }

    // sv info
    if (mSubscriptionMask & E_LOC_CB_GNSS_SV_BIT) {
        mCallbacks.gnssSvCb = [this](GnssSvNotification notification) {
            onGnssSvCb(notification);
        };
    } else {
        mCallbacks.gnssSvCb = nullptr;
    }

    // nmea
    if (mSubscriptionMask & E_LOC_CB_GNSS_NMEA_BIT) {
        mCallbacks.gnssNmeaCb = [this](GnssNmeaNotification notification) {
            onGnssNmeaCb(notification);
        };
    } else {
        mCallbacks.gnssNmeaCb = nullptr;
    }

    // data
    if (mSubscriptionMask & E_LOC_CB_GNSS_DATA_BIT) {
        mCallbacks.gnssDataCb = [this](GnssDataNotification notification) {
            onGnssDataCb(notification);
        };
    } else {
        mCallbacks.gnssDataCb = nullptr;
    }

    // measurements
    if (mSubscriptionMask & E_LOC_CB_GNSS_MEAS_BIT) {
        mCallbacks.gnssMeasurementsCb = [this](GnssMeasurementsNotification notification) {
            onGnssMeasurementsCb(notification);
        };
    } else {
        mCallbacks.gnssMeasurementsCb = nullptr;
    }

    // system info
    if (mSubscriptionMask & E_LOC_CB_SYSTEM_INFO_BIT) {
        mCallbacks.locationSystemInfoCb = [this](LocationSystemInfo notification) {
            onLocationSystemInfoCb(notification);
        };
    } else {
        mCallbacks.locationSystemInfoCb = nullptr;
    }

    // following callbacks are not supported
    mCallbacks.gnssNiCb = nullptr;
    mCallbacks.geofenceStatusCb = nullptr;

    // call location API if already created
    if (mLocationApi) {
        LOC_LOGd("--> updateCallbacks mask=0x%x", mask);
        mLocationApi->updateCallbacks(mCallbacks);
    }
}

uint32_t LocHalDaemonClientHandler::startTracking() {
    LOC_LOGd("distance %d, internal %d, req mask %x",
             mOptions.minDistance, mOptions.minInterval,
             mOptions.locReqEngTypeMask);
    if (mSessionId == 0 && mLocationApi) {
        mSessionId = mLocationApi->startTracking(mOptions);
    }
    return mSessionId;
}

// Round input TBF to 100ms, 200ms, 500ms, and integer senconds
// input tbf < 200 msec, round to 100 msec, else
// input tbf < 500 msec, round to 200 msec, else
// input tbf < 1000 msec, round to 500 msec, else
// round up input tbf to the closet integer seconds
uint32_t LocHalDaemonClientHandler::startTracking(LocationOptions & locOptions) {
    LOC_LOGd("distance %d, internal %d, req mask %x",
          locOptions.minDistance, locOptions.minInterval,
             locOptions.locReqEngTypeMask);
    if (mSessionId == 0 && mLocationApi) {
        // update option
        mOptions = locOptions;
        // set interval to engine supported interval
        mOptions.minInterval = getSupportedTbf(mOptions.minInterval);
        mSessionId = mLocationApi->startTracking(mOptions);
    }

    return mSessionId;
}

void LocHalDaemonClientHandler::unsubscribeLocationSessionCb() {

    uint32_t subscriptionMask = mSubscriptionMask;

    subscriptionMask &= ~LOCATION_SESSON_ALL_INFO_MASK;
    updateSubscription(subscriptionMask);
}

void LocHalDaemonClientHandler::stopTracking() {
    if (mSessionId != 0 && mLocationApi) {
        mLocationApi->stopTracking(mSessionId);
        mSessionId = 0;
    }
}

void LocHalDaemonClientHandler::updateTrackingOptions(LocationOptions & locOptions) {
    if (mSessionId != 0 && mLocationApi) {
        LOC_LOGe("distance %d, internal %d, req mask %x",
             locOptions.minDistance, locOptions.minInterval,
             locOptions.locReqEngTypeMask);

        TrackingOptions trackingOption;
        trackingOption.setLocationOptions(locOptions);
        // set tbf to device supported tbf
        trackingOption.minInterval = getSupportedTbf(trackingOption.minInterval);
        mLocationApi->updateTrackingOptions(mSessionId, trackingOption);

        // save other info: eng req type that will be used in filtering
        mOptions = locOptions;
    }
}

uint32_t LocHalDaemonClientHandler::startBatching(uint32_t minInterval, uint32_t minDistance,
        BatchingMode batchMode) {
    if (mBatchingId == 0 && mLocationApi) {
        // update option
        LocationOptions locOption = {};
        locOption.size = sizeof(locOption);
        locOption.minInterval = minInterval;
        locOption.minDistance = minDistance;
        locOption.mode = GNSS_SUPL_MODE_STANDALONE;
        mBatchOptions.size = sizeof(mBatchOptions);
        mBatchOptions.batchingMode = batchMode;
        mBatchOptions.setLocationOptions(locOption);

        mBatchingId = mLocationApi->startBatching(mBatchOptions);
    }
    return mBatchingId;

}

void LocHalDaemonClientHandler::stopBatching() {
    if (mBatchingId != 0 && mLocationApi) {
        mLocationApi->stopBatching(mBatchingId);
        mBatchingId = 0;
    }
}

void LocHalDaemonClientHandler::updateBatchingOptions(uint32_t minInterval, uint32_t minDistance,
        BatchingMode batchMode) {
    if (mBatchingId != 0 && mLocationApi) {
        // update option
        LocationOptions locOption = {};
        locOption.size = sizeof(locOption);
        locOption.minInterval = minInterval;
        locOption.minDistance = minDistance;
        locOption.mode = GNSS_SUPL_MODE_STANDALONE;
        mBatchOptions.size = sizeof(mBatchOptions);
        mBatchOptions.batchingMode = batchMode;
        mBatchOptions.setLocationOptions(locOption);

        mLocationApi->updateBatchingOptions(mBatchingId, mBatchOptions);
    }
}
void LocHalDaemonClientHandler::setGeofenceIds(size_t count, uint32_t* clientIds,
        uint32_t* sessionIds) {
    for (int i=0; i<count; ++i) {
        mGfIdsMap[clientIds[i]] = sessionIds[i];
    }
}

void LocHalDaemonClientHandler::eraseGeofenceIds(size_t count, uint32_t* clientIds) {
    for (int i=0; i<count; ++i) {
        mGfIdsMap.erase(clientIds[i]);
    }
}
uint32_t* LocHalDaemonClientHandler::getSessionIds(size_t count, uint32_t* clientIds) {
    uint32_t* sessionIds = nullptr;
    if (count > 0) {
        sessionIds = (uint32_t*)malloc(sizeof(uint32_t) * count);
        if (nullptr == sessionIds) {
            return nullptr;
        }
        memset(sessionIds, 0, sizeof(uint32_t) * count);
        for (int i=0; i<count; ++i) {
            sessionIds[i] = mGfIdsMap[clientIds[i]];
        }
    }
    return sessionIds;
}

uint32_t* LocHalDaemonClientHandler::getClientIds(size_t count, uint32_t* sessionIds) {
    uint32_t* clientIds = nullptr;
    if (count > 0) {
        clientIds = (uint32_t*)malloc(sizeof(uint32_t) * count);
        if (nullptr == clientIds) {
            return nullptr;
        }
        memset(clientIds, 0, sizeof(uint32_t) * count);
        for (int i=0; i<count; ++i) {
            for(auto itor = mGfIdsMap.begin(); itor != mGfIdsMap.end(); itor++) {
                if (itor->second == sessionIds[i]) {
                    clientIds[i] = itor->first;
                }
            }
        }
    }
    return clientIds;
}

uint32_t* LocHalDaemonClientHandler::addGeofences(size_t count, GeofenceOption* options,
        GeofenceInfo* info) {
    if (count > 0 && mLocationApi) {
        mGeofenceIds = mLocationApi->addGeofences(count, options, info);
        if (mGeofenceIds) {
            LOC_LOGi("start new geofence sessions: %p", mGeofenceIds);
        }
    }
    return mGeofenceIds;
}

void LocHalDaemonClientHandler::removeGeofences(size_t count, uint32_t* ids) {
    if (count > 0 && mLocationApi) {
        mLocationApi->removeGeofences(count, ids);
    }
}

void LocHalDaemonClientHandler::modifyGeofences(size_t count, uint32_t* ids,
        GeofenceOption* options) {
    if (count >0 && mLocationApi) {
        mLocationApi->modifyGeofences(count, ids, options);
    }
}
void LocHalDaemonClientHandler::pauseGeofences(size_t count, uint32_t* ids) {
    if (count > 0 && mLocationApi) {
        mLocationApi->pauseGeofences(count, ids);
    }
}
void LocHalDaemonClientHandler::resumeGeofences(size_t count, uint32_t* ids) {
    if (count > 0 && mLocationApi) {
        mLocationApi->resumeGeofences(count, ids);
    }
}

void LocHalDaemonClientHandler::pingTest() {

    if (nullptr != mIpcSender) {
        LocAPIPingTestIndMsg msg(SERVICE_NAME);
        int rc = sendMessage(msg);

        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::cleanup() {
    // please do not attempt to hold the lock, as the caller of this function
    // already holds the lock

    // set the ptr to null to prevent further sending out message to the
    // remote client that is no longer reachable
    mIpcSender = nullptr;

    // check whether this is client from external AP,
    // mName for client on external ap is fully-qualified path name ending with
    // "serviceid.instanceid"
    if (strncmp(mName.c_str(), EAP_LOC_CLIENT_DIR,
                sizeof(EAP_LOC_CLIENT_DIR)-1) == 0 ) {
        LOC_LOGv("removed file %s", mName.c_str());
        if (0 != remove(mName.c_str())) {
            LOC_LOGe("<-- failed to remove file %s", mName.c_str());
        }
    }

    if (mLocationApi) {
        mLocationApi->destroy([this]() {onLocationApiDestroyCompleteCb();});
        mLocationApi = nullptr;
    } else {
        // For location integration api client handler, it does not
        // instantiate LocationApi interface and can be freed right away
        LOC_LOGe("delete LocHalDaemonClientHandler");
        delete this;
    }
}

/******************************************************************************
LocHalDaemonClientHandler - Location API response callback functions
******************************************************************************/
void LocHalDaemonClientHandler::onResponseCb(LocationError err, uint32_t id) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);

    if (nullptr != mIpcSender) {
        LOC_LOGd("--< onResponseCb err=%u id=%u", err, id);

        ELocMsgID pendingMsgId = E_LOCAPI_UNDEFINED_MSG_ID;
        if (!mPendingMessages.empty()) {
            pendingMsgId = mPendingMessages.front();
            mPendingMessages.pop();
        }

        int rc = 0;
        // send corresponding indication message if pending
        switch (pendingMsgId) {
            case E_LOCAPI_START_TRACKING_MSG_ID: {
                LOC_LOGd("<-- start resp err=%u id=%u pending=%u", err, id, pendingMsgId);
                LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_START_TRACKING_MSG_ID, err);
                rc = sendMessage(msg);
                break;
            }
        case E_LOCAPI_STOP_TRACKING_MSG_ID: {
            LOC_LOGd("<-- stop resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_STOP_TRACKING_MSG_ID, err);
            rc = sendMessage(msg);
            break;
        }
        case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID: {
            LOC_LOGd("<-- update resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID, err);
            rc = sendMessage(msg);
            break;
        }
        case E_LOCAPI_START_BATCHING_MSG_ID : {
            LOC_LOGd("<-- start batching resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_START_BATCHING_MSG_ID , err);
            rc = sendMessage(msg);
            break;
        }
        case E_LOCAPI_STOP_BATCHING_MSG_ID: {
            LOC_LOGd("<-- stop batching resp err=%u id=%u pending=%u", err, id, pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_STOP_BATCHING_MSG_ID, err);
            rc = sendMessage(msg);
            break;
        }
        case E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID: {
            LOC_LOGd("<-- update batching options resp err=%u id=%u pending=%u", err, id,
                    pendingMsgId);
            LocAPIGenericRespMsg msg(SERVICE_NAME, E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID, err);
            rc = sendMessage(msg);
            break;
        }
        default: {
            LOC_LOGe("no pending message for %s", mName.c_str());
            return;
            }
        }

        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onCollectiveResponseCallback(
        size_t count, LocationError *errs, uint32_t *ids) {
    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onCollectiveResponseCallback");

    if (nullptr == mIpcSender) {
        return;
    }

    ELocMsgID pendingMsgId = E_LOCAPI_UNDEFINED_MSG_ID;
    if (!mGfPendingMessages.empty()) {
        pendingMsgId = mGfPendingMessages.front();
        mGfPendingMessages.pop();
    }
    if (0 == count) {
        return;
    }
    // serialize LocationError and ids into ipc message payload
    size_t msglen = sizeof(LocAPICollectiveRespMsg) + sizeof(GeofenceResponse) * (count - 1);
    uint8_t *msg = new(std::nothrow) uint8_t[msglen];
    if (nullptr == msg) {
        return;
    }
    memset(msg, 0, msglen);
    LocAPICollectiveRespMsg *pmsg = reinterpret_cast<LocAPICollectiveRespMsg*>(msg);
    strlcpy(pmsg->mSocketName, SERVICE_NAME, MAX_SOCKET_PATHNAME_LENGTH);
    pmsg->collectiveRes.size = msglen;
    pmsg->collectiveRes.count = count;

    int rc = 0;
    uint32_t* clientIds = getClientIds(count, ids);
    if (nullptr == clientIds) {
        delete[] msg;
        return;
    }
    GeofenceResponse gfResponse = {};
    for (int i=0; i<count; ++i) {
        gfResponse.clientId = clientIds[i];
        gfResponse.error = errs[i];
        *(pmsg->collectiveRes.resp+i) = gfResponse;
        if (errs[i] != LOCATION_ERROR_SUCCESS) {
            eraseGeofenceIds(1, &clientIds[i]);
        }
    }

    // send corresponding indication message if pending
    switch (pendingMsgId) {
        case E_LOCAPI_ADD_GEOFENCES_MSG_ID: {
            LOC_LOGd("<-- addGeofence resp pending=%u", pendingMsgId);
            pmsg->msgId = E_LOCAPI_ADD_GEOFENCES_MSG_ID;
            rc = sendMessage(msg, msglen);
            break;
        }
        case E_LOCAPI_REMOVE_GEOFENCES_MSG_ID: {
            LOC_LOGd("<-- removeGeofence resp pending=%u", pendingMsgId);
            pmsg->msgId = E_LOCAPI_REMOVE_GEOFENCES_MSG_ID;
            eraseGeofenceIds(count, clientIds);
            rc = sendMessage(msg, msglen);
            break;
        }
        case E_LOCAPI_MODIFY_GEOFENCES_MSG_ID: {
            LOC_LOGd("<-- modifyGeofence resp pending=%u", pendingMsgId);
            pmsg->msgId = E_LOCAPI_MODIFY_GEOFENCES_MSG_ID;
            rc = sendMessage(msg, msglen);
            break;
        }
        case E_LOCAPI_PAUSE_GEOFENCES_MSG_ID: {
            LOC_LOGd("<-- pauseGeofence resp pending=%u", pendingMsgId);
            pmsg->msgId = E_LOCAPI_PAUSE_GEOFENCES_MSG_ID;
            rc = sendMessage(msg, msglen);
            break;
        }
        case E_LOCAPI_RESUME_GEOFENCES_MSG_ID: {
            LOC_LOGd("<-- resumeGeofence resp pending=%u", pendingMsgId);
            pmsg->msgId = E_LOCAPI_RESUME_GEOFENCES_MSG_ID;
            rc = sendMessage(msg, msglen);
            break;
        }
        default: {
            LOC_LOGe("no pending geofence message for %s", mName.c_str());
            free(clientIds);
            return;
        }
    }

    // purge this client if failed
    if (!rc) {
        LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
        mService->deleteClientbyName(mName);
    }
    delete[] msg;
    free(clientIds);
}


/******************************************************************************
LocHalDaemonClientHandler - Location Control API response callback functions
******************************************************************************/
void LocHalDaemonClientHandler::onControlResponseCb(LocationError err, ELocMsgID msgId) {
    // no need to hold the lock, as lock is already held at the caller
    if (nullptr != mIpcSender) {
        LOC_LOGd("--< onControlResponseCb err=%u msgId=%u", err, msgId);
        LocAPIGenericRespMsg msg(SERVICE_NAME, msgId, err);
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onGnssConfigCb(ELocMsgID configMsgId,
                                               const GnssConfig & gnssConfig) {
    uint8_t* msg = nullptr;
    size_t msgLen = 0;

    switch (configMsgId) {
    case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID:
        if (gnssConfig.flags & GNSS_CONFIG_FLAGS_ROBUST_LOCATION_BIT) {
            msg = (uint8_t*) new LocConfigGetRobustLocationConfigRespMsg(
                    SERVICE_NAME, gnssConfig.robustLocationConfig);
            msgLen = sizeof(LocConfigGetRobustLocationConfigRespMsg);
        }
        break;
    default:
        break;
    }

    if ((nullptr != mIpcSender) && (nullptr != msg)) {
        int rc = sendMessage(msg, msgLen);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }

    // cleanup
    if (nullptr != msg) {
        delete msg;
        msg = nullptr;
    }
}

/******************************************************************************
LocHalDaemonClientHandler - Location API callback functions
******************************************************************************/
void LocHalDaemonClientHandler::onCapabilitiesCallback(LocationCapabilitiesMask mask) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onCapabilitiesCallback=0x%x", mask);

    if (nullptr != mIpcSender) {
        // broadcast
        LocAPICapabilitiesIndMsg msg(SERVICE_NAME, mask);
        if (mask != mCapabilityMask) {
            LOC_LOGd("mask old=0x%x new=0x%x", mCapabilityMask, mask);
            mCapabilityMask = mask;
            int rc = sendMessage(msg);
            // purge this client if failed
            if (!rc) {
                LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
                mService->deleteClientbyName(mName);
            }
        }
    }
}

void LocHalDaemonClientHandler::onTrackingCb(Location location) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onTrackingCb");

    if ((nullptr != mIpcSender) &&
            (mSubscriptionMask & E_LOC_CB_DISTANCE_BASED_TRACKING_BIT)) {
        // broadcast
        LocAPILocationIndMsg msg(SERVICE_NAME, location);
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onBatchingCb(size_t count, Location* location,
        BatchingOptions batchOptions) {
    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onBatchingCb");

    if ((nullptr != mIpcSender) && (mSubscriptionMask & E_LOC_CB_BATCHING_BIT)) {
        if (0 == count) {
            return;
        }

        // serialize locations in batch into ipc message payload
        size_t msglen = sizeof(LocAPIBatchingIndMsg) + sizeof(Location) * (count - 1);
        uint8_t *msg = new(std::nothrow) uint8_t[msglen];
        if (nullptr == msg) {
            return;
        }
        memset(msg, 0, msglen);
        LocAPIBatchingIndMsg *pmsg = reinterpret_cast<LocAPIBatchingIndMsg*>(msg);
        strlcpy(pmsg->mSocketName, SERVICE_NAME, MAX_SOCKET_PATHNAME_LENGTH);
        pmsg->msgId = E_LOCAPI_BATCHING_MSG_ID;
        pmsg->batchNotification.size = msglen;
        pmsg->batchNotification.count = count;
        pmsg->batchNotification.status = BATCHING_STATUS_POSITION_AVAILABE;
        memcpy(&(pmsg->batchNotification.location[0]), location, count * sizeof(Location));

        int rc = sendMessage(msg, msglen);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }

        delete[] msg;
    }
}

void LocHalDaemonClientHandler::onBatchingStatusCb(BatchingStatusInfo batchingStatus,
                std::list<uint32_t>& listOfCompletedTrips) {
    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onBatchingStatusCb");
    if ((nullptr != mIpcSender) && (mSubscriptionMask & E_LOC_CB_BATCHING_STATUS_BIT) &&
                (BATCHING_MODE_TRIP == mBatchingMode) &&
                (BATCHING_STATUS_TRIP_COMPLETED == batchingStatus.batchingStatus)) {
        // For trip batching, notify client to stop session when BATCHING_STATUS_TRIP_COMPLETED
        LocAPIBatchNotification batchNotif = {};
        batchNotif.status = BATCHING_STATUS_TRIP_COMPLETED;
        LocAPIBatchingIndMsg msg(SERVICE_NAME, batchNotif);
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onGeofenceBreachCb(GeofenceBreachNotification gfBreachNotif) {
    LOC_LOGd("--< onGeofenceBreachCallback");
    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);

    if ((nullptr != mIpcSender) &&
            (mSubscriptionMask & E_LOCAPI_GEOFENCE_BREACH_MSG_ID)) {

        uint32_t* clientIds = getClientIds(gfBreachNotif.count, gfBreachNotif.ids);
        if (nullptr == clientIds) {
            LOC_LOGe("Failed to alloc %zu bytes",
                     sizeof(uint32_t) * gfBreachNotif.count);
            return;
        }
        // serialize GeofenceBreachNotification into ipc message payload
        size_t msglen = sizeof(LocAPIGeofenceBreachIndMsg) +
                sizeof(uint32_t) * (gfBreachNotif.count - 1);
        uint8_t *msg = new(std::nothrow) uint8_t[msglen];
        if (nullptr == msg) {
            free(clientIds);
            return;
        }
        memset(msg, 0, msglen);
        LocAPIGeofenceBreachIndMsg *pmsg = reinterpret_cast<LocAPIGeofenceBreachIndMsg*>(msg);
        strlcpy(pmsg->mSocketName, SERVICE_NAME, MAX_SOCKET_PATHNAME_LENGTH);
        pmsg->msgId = E_LOCAPI_GEOFENCE_BREACH_MSG_ID;
        pmsg->gfBreachNotification.size = msglen;
        pmsg->gfBreachNotification.count = gfBreachNotif.count;
        pmsg->gfBreachNotification.timestamp = gfBreachNotif.timestamp;
        pmsg->gfBreachNotification.location = gfBreachNotif.location;
        pmsg->gfBreachNotification.type = gfBreachNotif.type;
        memcpy(&(pmsg->gfBreachNotification.id[0]), clientIds, sizeof(uint32_t)*gfBreachNotif.count);

        int rc = sendMessage(msg, msglen);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }

        delete[] msg;
        free(clientIds);
    }
}

void LocHalDaemonClientHandler::onGnssLocationInfoCb(GnssLocationInfoNotification notification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onGnssLocationInfoCb");

    if ((nullptr != mIpcSender) && (mSubscriptionMask &
            (E_LOC_CB_GNSS_LOCATION_INFO_BIT | E_LOC_CB_SIMPLE_LOCATION_INFO_BIT))) {
        int rc = 0;
        if (mSubscriptionMask & E_LOC_CB_GNSS_LOCATION_INFO_BIT) {
            LocAPILocationInfoIndMsg msg(SERVICE_NAME, notification);
            rc = sendMessage(msg);
        } else {
            LocAPILocationIndMsg msg(SERVICE_NAME, notification.location);
            rc = sendMessage(msg);
        }
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onEngLocationsInfoCb(
        uint32_t count,
        GnssLocationInfoNotification* engLocationsInfoNotification
) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onEngLocationInfoCb count: %d, locReqEngTypeMask 0x%x",
             count, mOptions.locReqEngTypeMask);

    if ((nullptr != mIpcSender) &&
        (mSubscriptionMask & E_LOC_CB_ENGINE_LOCATIONS_INFO_BIT)) {

        int reportCount = 0;
        GnssLocationInfoNotification engineLocationInfoNotification[LOC_OUTPUT_ENGINE_COUNT];
        for (int i = 0; i < count; i++) {
            GnssLocationInfoNotification* locPtr = engLocationsInfoNotification+i;

            LOC_LOGv("--< onEngLocationInfoCb i %d, type %d", i, locPtr->locOutputEngType);
            if (((locPtr->locOutputEngType == LOC_OUTPUT_ENGINE_FUSED) &&
                 (mOptions.locReqEngTypeMask & LOC_REQ_ENGINE_FUSED_BIT)) ||
                ((locPtr->locOutputEngType == LOC_OUTPUT_ENGINE_SPE) &&
                 (mOptions.locReqEngTypeMask & LOC_REQ_ENGINE_SPE_BIT)) ||
                ((locPtr->locOutputEngType == LOC_OUTPUT_ENGINE_PPE) &&
                 (mOptions.locReqEngTypeMask & LOC_REQ_ENGINE_PPE_BIT ))) {
                engineLocationInfoNotification[reportCount++] = *locPtr;
            }
        }

        if (reportCount > 0 ) {
            LocAPIEngineLocationsInfoIndMsg msg(SERVICE_NAME, reportCount,
                                                engineLocationInfoNotification);
            int rc = sendMessage((const uint8_t*)&msg, msg.getMsgSize());
            // purge this client if failed
            if (!rc) {
                LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
                mService->deleteClientbyName(mName);
            }
        }
    }
}

void LocHalDaemonClientHandler::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onGnssNiCb");
}

void LocHalDaemonClientHandler::onGnssSvCb(GnssSvNotification notification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onGnssSvCb");

    if ((nullptr != mIpcSender) &&
            (mSubscriptionMask & E_LOC_CB_GNSS_SV_BIT)) {
        // broadcast
        LocAPISatelliteVehicleIndMsg msg(SERVICE_NAME, notification);
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onGnssNmeaCb(GnssNmeaNotification notification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    if ((nullptr != mIpcSender) && (mSubscriptionMask & E_LOC_CB_GNSS_NMEA_BIT)) {
        LOC_LOGd("--< onGnssNmeaCb[%s] t=%" PRIu64" l=%zu nmea=%s",
                mName.c_str(),
                notification.timestamp,
                notification.length,
                notification.nmea);

        // serialize nmea string into ipc message payload
        size_t msglen = sizeof(LocAPINmeaIndMsg) + notification.length;
        uint8_t *msg = new(std::nothrow) uint8_t[msglen];
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

        int rc = sendMessage(msg, msglen);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }

        delete[] msg;
    }
}

void LocHalDaemonClientHandler::onGnssDataCb(GnssDataNotification notification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onGnssDataCb");

    if ((nullptr != mIpcSender) &&
            (mSubscriptionMask & E_LOC_CB_GNSS_DATA_BIT)) {
        for (int sig = 0; sig < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
            if (GNSS_LOC_DATA_JAMMER_IND_BIT ==
                (notification.gnssDataMask[sig] & GNSS_LOC_DATA_JAMMER_IND_BIT)) {
                LOC_LOGv("jammerInd[%d]=%f", sig, notification.jammerInd[sig]);
            }
            if (GNSS_LOC_DATA_AGC_BIT ==
                (notification.gnssDataMask[sig] & GNSS_LOC_DATA_AGC_BIT)) {
                LOC_LOGv("agc[%d]=%f", sig, notification.agc[sig]);
            }
        }

        LocAPIDataIndMsg msg(SERVICE_NAME, notification);
        LOC_LOGv("Sending data message");
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onGnssMeasurementsCb(GnssMeasurementsNotification notification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onGnssMeasurementsCb");

    if ((nullptr != mIpcSender) && (mSubscriptionMask & E_LOC_CB_GNSS_MEAS_BIT)) {
        LocAPIMeasIndMsg msg(SERVICE_NAME, notification);
        LOC_LOGv("Sending meas message");
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onLocationSystemInfoCb(LocationSystemInfo notification) {

    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);
    LOC_LOGd("--< onLocationSystemInfoCb");

    if ((nullptr != mIpcSender) &&
            (mSubscriptionMask & E_LOC_CB_SYSTEM_INFO_BIT)) {

        LocAPILocationSystemInfoIndMsg msg(SERVICE_NAME, notification);
        LOC_LOGv("Sending location system info message");
        int rc = sendMessage(msg);
        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

void LocHalDaemonClientHandler::onLocationApiDestroyCompleteCb() {
    std::lock_guard<std::mutex> lock(LocationApiService::mMutex);

    LOC_LOGe("delete LocHalDaemonClientHandler");
    delete this;
    // PLEASE NOTE: no more code after this, including print for class variable
}

/******************************************************************************
LocHalDaemonClientHandler - Engine info related functionality
******************************************************************************/
// called to deliver GNSS energy consumed info to the requesting client
// as this is single shot request, the corresponding mask will be cleared
// as well
void LocHalDaemonClientHandler::onGnssEnergyConsumedInfoAvailable(
   LocAPIGnssEnergyConsumedIndMsg &msg) {

   if ((nullptr != mIpcSender) &&
            (mEngineInfoRequestMask & E_ENGINE_INFO_CB_GNSS_ENERGY_CONSUMED_BIT)) {

        int rc = sendMessage(msg);
        mEngineInfoRequestMask &= ~E_ENGINE_INFO_CB_GNSS_ENERGY_CONSUMED_BIT;

        // purge this client if failed
        if (!rc) {
            LOC_LOGe("failed rc=%d purging client=%s", rc, mName.c_str());
            mService->deleteClientbyName(mName);
        }
    }
}

// return true if the client has pending request to retrieve
// GNSS energy consumed
bool LocHalDaemonClientHandler::hasPendingEngineInfoRequest(uint32_t mask) {
    if (mEngineInfoRequestMask & E_ENGINE_INFO_CB_GNSS_ENERGY_CONSUMED_BIT) {
        return true;
    } else {
        return false;
    }
}

// set up the bit to indicating the engine info request
// is pending.
void LocHalDaemonClientHandler::addEngineInfoRequst(uint32_t mask) {
    mEngineInfoRequestMask |= E_ENGINE_INFO_CB_GNSS_ENERGY_CONSUMED_BIT;
}

// Round input TBF to 100ms, 200ms, 500ms, and integer senconds that engine supports
// input tbf < 200 msec, round to 100 msec, else
// input tbf < 500 msec, round to 200 msec, else
// input tbf < 1000 msec, round to 500 msec, else
// round up input tbf to the closet integer seconds
uint32_t LocHalDaemonClientHandler::getSupportedTbf(uint32_t tbfMsec) {
    uint32_t supportedTbfMsec = 0;

    if (tbfMsec < 200) {
        supportedTbfMsec = 100;
    } else if (tbfMsec < 500) {
        supportedTbfMsec = 200;
    } else if (tbfMsec < 1000) {
        supportedTbfMsec = 500;
    } else {
        if (tbfMsec > (UINT32_MAX - 999)) {
            supportedTbfMsec = UINT32_MAX / 1000 * 1000;
        } else {
            // round up to the next integer second
            supportedTbfMsec = (tbfMsec+999) / 1000 * 1000;
        }
    }

    return supportedTbfMsec;
}
