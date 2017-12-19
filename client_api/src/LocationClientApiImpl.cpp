/* Copyright (c) 2018 The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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

#define LOG_TAG "LocSvc_LocationClientApi"

#include <LocationClientApiImpl.h>
#include <log_util.h>
#include <gps_extended_c.h>
#include <unistd.h>

using namespace std;

namespace location_client {

/******************************************************************************
LocationClientApiImpl
******************************************************************************/

uint32_t  LocationClientApiImpl::mClientIdGenerator = LOCATION_CLIENT_SESSION_ID_INVALID;

mutex LocationClientApiImpl::mMutex;

/******************************************************************************
LocationClientApiImpl - constructors
******************************************************************************/
LocationClientApiImpl::LocationClientApiImpl(LocationClientApi* locationClientApi) :
    mLocationClient(locationClientApi), mHalRegistered(false),
    mCallbacksMask(0), mLocationOptions({0}) {

    mMsgTask = new MsgTask("ClientApiImpl", false);

    // create socket to send
    mIpcSender = new LocIpcSender(SOCKET_TO_LOCATION_HAL_DAEMON);
    if (nullptr == mIpcSender) {
        LOC_LOGe("create mIpcSender failed %s\n", SOCKET_TO_LOCATION_HAL_DAEMON);
    }

    // get clientId
    lock_guard<mutex> lock(mMutex);
    mClientId = ++mClientIdGenerator;

    // get pid to generate sokect name
    uint32_t pid = (uint32_t)getpid();
    int strCopied = strlcpy(mSocketName,SOCKET_TO_LOCATION_CLIENT_BASE,
                           MAX_SOCKET_PATHNAME_LENGTH);
    if (strCopied>0 && strCopied< MAX_SOCKET_PATHNAME_LENGTH) {
        snprintf(mSocketName+strCopied,
                 MAX_SOCKET_PATHNAME_LENGTH-strCopied,
                 ".%u.%u", pid, mClientId);
        LOC_LOGd("scoketname=%s\n", mSocketName);
        startListeningNonBlocking(mSocketName);
    } else {
        LOC_LOGe("strlcpy failed %d\n", strCopied);
    }
}

LocationClientApiImpl::~LocationClientApiImpl() {
    bool rc = false;

    // deregister
    if (mHalRegistered && (nullptr != mIpcSender)) {
        LocAPIClientDeregisterReqMsg msg(mSocketName);
        rc = mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                   sizeof(msg));
        LOC_LOGd(">>> DeregisterReq rc=%d\n", rc);
        delete mIpcSender;
    }

    if (mMsgTask) {
        mMsgTask->destroy();
    }
}

/******************************************************************************
LocationClientApiImpl - implementation
******************************************************************************/
void LocationClientApiImpl::updateCallbacks(LocationCallbacks& callbacks) {
    LocationCallbacksMask callBacksMask = 0;

    //convert callbacks to callBacksMask
    if (callbacks.trackingCb) {
        callBacksMask |= E_LOC_CB_TRACKING_BIT;
    }

    if (callbacks.gnssLocationInfoCb) {
        callBacksMask |= E_LOC_CB_GNSS_LOCATION_INFO_BIT;
    }

    if (callbacks.gnssSvCb) {
        callBacksMask |= E_LOC_CB_GNSS_SV_BIT;
    }

    if (callbacks.gnssNmeaCb) {
        callBacksMask |= E_LOC_CB_GNSS_NMEA_BIT;
    }

    if (mCallbacksMask != callBacksMask) {
        if (mHalRegistered) {
            struct UpdateCallbacksReq : public LocMsg {
                UpdateCallbacksReq(LocationClientApiImpl *apiImpl) :
                    mApiImpl(apiImpl) {}
                virtual ~UpdateCallbacksReq() {}
                void proc() const {
                    LocAPIUpdateCallbacksReqMsg msg(mApiImpl->mSocketName,
                                                    mApiImpl->mCallbacksMask);
                    bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                         sizeof(msg));
                    LOC_LOGd(">>> UpdateCallbacksReq callBacksMask=0x%x rc=%d\n",
                             mApiImpl->mCallbacksMask, rc);
                }
                LocationClientApiImpl *mApiImpl;
            };
            mCallbacksMask = callBacksMask;
            mMsgTask->sendMsg(new (nothrow) UpdateCallbacksReq(this));
        } else {
            LOC_LOGd("Not registered yet \n");
        }
    } else {
        LOC_LOGd("No updateCallbacks because same callBacksMask 0x%x\n",
                  callBacksMask);
    }
}

uint32_t LocationClientApiImpl::startTracking(LocationOptions& option) {
    mLocationOptions = option;

    if (mHalRegistered) {
        struct StartTrackingReq : public LocMsg {
            StartTrackingReq(LocationClientApiImpl *apiImpl) :
                mApiImpl(apiImpl) {}
            virtual ~StartTrackingReq() {}
            void proc() const {
                LocAPIStartTrackingReqMsg msg(mApiImpl->mSocketName,
                                              mApiImpl->mLocationOptions.minInterval,
                                              mApiImpl->mLocationOptions.minDistance);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                     sizeof(msg));
                LOC_LOGd(">>> StartTrackingReq Interval=%d Distance=%d\n",
                         mApiImpl->mLocationOptions.minInterval,
                         mApiImpl->mLocationOptions.minDistance);
            }
            LocationClientApiImpl *mApiImpl;
        };
        mMsgTask->sendMsg(new (nothrow) StartTrackingReq(this));
    } else {
        LOC_LOGd("Not registered yet \n");
    }

    return mClientId;
}

void LocationClientApiImpl::stopTracking(uint32_t sessionId) {

    if (sessionId == mClientId) {
        mLocationOptions.minInterval = 0;
        mLocationOptions.minDistance = 0;
        mCallbacksMask = 0;
        struct StopTrackingReq : public LocMsg {
            StopTrackingReq(LocationClientApiImpl *apiImpl) :
                mApiImpl(apiImpl) {}
            virtual ~StopTrackingReq() {}
            void proc() const {
                LocAPIStopTrackingReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                    sizeof(msg));
                LOC_LOGd(">>> StopTrackingReq rc=%d\n", rc);
            }
            LocationClientApiImpl *mApiImpl;
        };
        mMsgTask->sendMsg(new (nothrow) StopTrackingReq(this));
    } else {
        LOC_LOGe ("Failed, no active session\n");
    }
}

void LocationClientApiImpl::updateTrackingOptions(uint32_t, LocationOptions& option) {

    if ((mLocationOptions.minInterval != option.minInterval) ||
        (mLocationOptions.minDistance != option.minDistance)) {
        struct UpdateTrackingOptionsReq : public LocMsg {
            UpdateTrackingOptionsReq(LocationClientApiImpl *apiImpl) :
                mApiImpl(apiImpl) {}
            virtual ~UpdateTrackingOptionsReq() {}
            void proc() const {
                LocAPIUpdateTrackingOptionsReqMsg msg(mApiImpl->mSocketName,
                                              mApiImpl->mLocationOptions.minInterval,
                                              mApiImpl->mLocationOptions.minDistance);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                     sizeof(msg));
                LOC_LOGd(">>> UpdateTrackingOptionsReq Interval=%d Distance=%d\n",
                         mApiImpl->mLocationOptions.minInterval,
                         mApiImpl->mLocationOptions.minDistance);
            }
            LocationClientApiImpl *mApiImpl;
        };
        mLocationOptions = option;
        mMsgTask->sendMsg(new (nothrow) UpdateTrackingOptionsReq(this));
    } else {
        LOC_LOGd("No UpdateTrackingOptions because same Interval=%d Distance=%d\n",
                  option.minInterval, option.minDistance);
    }
}

uint32_t* LocationClientApiImpl::gnssUpdateConfig(GnssConfig config) {

    struct UpdateConfigReq : public LocMsg {
        UpdateConfigReq(LocationClientApiImpl *apiImpl,
                        GnssConfig *config) :
            mApiImpl(apiImpl), mConfig(config) {}
        virtual ~UpdateConfigReq() {}
        void proc() const {
            LocAPIUpdateConfigReqMsg msg(mApiImpl->mSocketName, *mConfig);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
            LOC_LOGd(">>> UpdateConfigReq rc=%d\n", rc);
        }
        LocationClientApiImpl *mApiImpl;
        GnssConfig *mConfig;
    };
    mMsgTask->sendMsg(new (nothrow) UpdateConfigReq(this, &config));
    return nullptr;
}

uint32_t LocationClientApiImpl::gnssDeleteAidingData(GnssAidingData& data) {

    struct DeleteAidingDataReq : public LocMsg {
        DeleteAidingDataReq(LocationClientApiImpl *apiImpl,
                            GnssAidingData *data) :
            mApiImpl(apiImpl), mAidingData(data) {}
        virtual ~DeleteAidingDataReq() {}
        void proc() const {
            LocAPIDeleteAidingDataReqMsg msg(mApiImpl->mSocketName, *mAidingData);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
            LOC_LOGd(">>> DeleteAidingDataReq rc=%d\n", rc);
        }
        LocationClientApiImpl *mApiImpl;
        GnssAidingData *mAidingData;
    };
    mMsgTask->sendMsg(new (nothrow) DeleteAidingDataReq(this, &data));
    return 0;
}

void LocationClientApiImpl::updateNetworkAvailability(bool available) {

    struct UpdateNetworkAvailabilityReq : public LocMsg {
        UpdateNetworkAvailabilityReq(LocationClientApiImpl *apiImpl,
                                     bool available) :
            mApiImpl(apiImpl), mAvailable(available) {}
        virtual ~UpdateNetworkAvailabilityReq() {}
        void proc() const {
            LocAPIUpdateNetworkAvailabilityReqMsg msg(mApiImpl->mSocketName,
                                                      mAvailable);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
        }
        LocationClientApiImpl *mApiImpl;
        bool mAvailable;
    };
    LOC_LOGd(">>> UpdateNetworkAvailabilityReq available=%d \n",available);
    mMsgTask->sendMsg(new (nothrow) UpdateNetworkAvailabilityReq(this, available));
}

void LocationClientApiImpl::capabilitesCallback(ELocMsgID  msgId, const void* msgData) {

    if (nullptr != mLocationClient) {
        mHalRegistered = true;
        mLocationClient->locationClientApiImplCb(msgId, (void*)msgData);

        if (0 != mCallbacksMask) {
                LocAPIUpdateCallbacksReqMsg msg(mSocketName, mCallbacksMask);
                bool rc = mIpcSender->send(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
                LOC_LOGd(">>> UpdateCallbacksReq callBacksMask=0x%x rc=%d\n", mCallbacksMask, rc);
        }

        if (0 != mLocationOptions.minInterval) {
            (void)startTracking(mLocationOptions);
        }
    } else {
        LOC_LOGe("NULL mLocationClient\n");
    }


    return;
}

/******************************************************************************
LocationClientApiImpl - LocIpc overrides
******************************************************************************/
void LocationClientApiImpl::onListenerReady() {

    struct ClientRegisterReq : public LocMsg {
        ClientRegisterReq(LocationClientApiImpl *apiImpl) :
            mApiImpl(apiImpl) {}
        virtual ~ClientRegisterReq() {}
        void proc() const {
            LocAPIClientRegisterReqMsg msg(mApiImpl->mSocketName);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t *>(&msg), sizeof(msg));
            LOC_LOGd(">>> onListenerReady::ClientRegisterReqMsg rc=%d\n", rc);
        }
        LocationClientApiImpl *mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) ClientRegisterReq(this));

}

void LocationClientApiImpl::onReceive(const string& data) {

    struct OnReceiveHandler : public LocMsg {
        OnReceiveHandler(LocationClientApiImpl *apiImpl,
                         const string& data) :
            mApiImpl(apiImpl), mMsgData(data) {}
        virtual ~OnReceiveHandler() {}
        void proc() const {
            LocAPIMsgHeader *pMsg = (LocAPIMsgHeader *)(mMsgData.data());

            switch (pMsg->msgId) {
                case E_LOCAPI_CAPABILILTIES_MSG_ID:
                    {
                        LOC_LOGd("<<< capabilities indication \n");
                        mApiImpl->capabilitesCallback(
                           pMsg->msgId, (void *)pMsg);
                        break;
                    }

                case E_LOCAPI_HAL_READY_MSG_ID:
                    {
                        LOC_LOGd("<<< HAL ready \n");
                        mApiImpl->onListenerReady();
                        break;
                    }

                case E_LOCAPI_START_TRACKING_MSG_ID:
                case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID:
                case E_LOCAPI_STOP_TRACKING_MSG_ID:
                    {
                        LOC_LOGd("<<< response message %d\n",
                                 pMsg->msgId);
                        if (nullptr != mApiImpl->mLocationClient) {
                            mApiImpl->mLocationClient->locationClientApiImplCb(
                                pMsg->msgId, (void *)pMsg);
                        } else {
                            LOC_LOGe("NULL mLocationClient\n");
                        }
                        break;
                    }

                    // async indication messages
                case E_LOCAPI_LOCATION_MSG_ID:
                    {
                        LOC_LOGd("<<< message = location\n");
                        if (nullptr != mApiImpl->mLocationClient) {
                            if (mApiImpl->mCallbacksMask & E_LOC_CB_TRACKING_BIT) {
                                mApiImpl->mLocationClient->locationClientApiImplCb(
                                        pMsg->msgId, (void *)pMsg);
                            } else {
                                LOC_LOGe("Location Ind message not registered\n");
                            }
                        } else {
                            LOC_LOGe("NULL mLocationClient\n");
                        }
                        break;
                    }

                case E_LOCAPI_LOCATION_INFO_MSG_ID:
                    {
                        LOC_LOGd("<<< message = location info\n");
                        if (nullptr != mApiImpl->mLocationClient) {
                            if (mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_LOCATION_INFO_BIT) {
                                mApiImpl->mLocationClient->locationClientApiImplCb(
                                        pMsg->msgId, (void *)pMsg);
                            } else {
                                LOC_LOGe("LocationInfoInd message not registered\n");
                            }
                        } else {
                            LOC_LOGe("NULL mLocationClient\n");
                        }
                        break;
                    }

                case E_LOCAPI_SATELLITE_VEHICLE_MSG_ID:
                    {
                        LOC_LOGd("<<< message = sv\n");
                        if (nullptr != mApiImpl->mLocationClient) {
                            if (mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_SV_BIT) {
                                mApiImpl->mLocationClient->locationClientApiImplCb(
                                        pMsg->msgId, (void *)pMsg);
                            } else {
                                LOC_LOGe("SV Ind message not registered\n");
                            }
                        } else {
                            LOC_LOGe("NULL mLocationClient\n");
                        }
                        break;
                    }

                case E_LOCAPI_NMEA_MSG_ID:
                    {
                        LOC_LOGd("<<< message = nmea\n");
                        if (nullptr != mApiImpl->mLocationClient) {
                            if (mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_NMEA_BIT) {
                                mApiImpl->mLocationClient->locationClientApiImplCb(
                                        pMsg->msgId, (void *)pMsg);
                            } else {
                                LOC_LOGe("NMEA Ind message not registered\n");
                            }
                        } else {
                            LOC_LOGe("NULL mLocationClient\n");
                        }
                        break;
                    }

                default:
                    {
                        LOC_LOGe("<<< unknown message %d\n", pMsg->msgId);
                        break;
                    }
            }
        }
        LocationClientApiImpl *mApiImpl;
        string mMsgData;
    };
    mMsgTask->sendMsg(new (nothrow) OnReceiveHandler(this, data));

}

/******************************************************************************
LocationClientApiImpl - Not implemented overrides
******************************************************************************/

uint32_t LocationClientApiImpl::startBatching(LocationOptions&, BatchingOptions&) {
    return 0;
}

void LocationClientApiImpl::stopBatching(uint32_t) {
}

void LocationClientApiImpl::updateBatchingOptions(
        uint32_t id, LocationOptions&, BatchingOptions&) {
}

void LocationClientApiImpl::getBatchedLocations(uint32_t id, size_t count) {
}

uint32_t* LocationClientApiImpl::addGeofences(size_t count,
                                              GeofenceOption*, GeofenceInfo*) {
    return nullptr;
}

void LocationClientApiImpl::removeGeofences(size_t count, uint32_t* ids) {
}

void LocationClientApiImpl::modifyGeofences(
        size_t count, uint32_t* ids, GeofenceOption* options) {
}

void LocationClientApiImpl::pauseGeofences(size_t count, uint32_t* ids) {
}

void LocationClientApiImpl::resumeGeofences(size_t count, uint32_t* ids) {
}

void LocationClientApiImpl::gnssNiResponse(uint32_t id, GnssNiResponse response) {
}

} // namespace location_client

