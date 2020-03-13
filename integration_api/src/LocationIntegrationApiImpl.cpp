/* Copyright (c) 2019-2020 The Linux Foundation. All rights reserved.
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

#define LOG_TAG "LocSvc_LocationIntegrationApi"

#include <loc_cfg.h>
#include <LocationIntegrationApiImpl.h>
#include <log_util.h>
#include <gps_extended_c.h>

namespace location_integration {

/******************************************************************************
Utilities
******************************************************************************/

static LocConfigTypeEnum getLocConfigTypeFromMsgId(ELocMsgID  msgId) {
    LocConfigTypeEnum configType = (LocConfigTypeEnum) 0;
    switch (msgId) {
    case E_INTAPI_CONFIG_CONSTRAINTED_TUNC_MSG_ID:
        configType = CONFIG_CONSTRAINED_TIME_UNCERTAINTY;
        break;
    case E_INTAPI_CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR_MSG_ID:
        configType = CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR;
        break;
    case E_INTAPI_CONFIG_SV_CONSTELLATION_MSG_ID:
        configType = CONFIG_CONSTELLATIONS;
        break;
    case E_INTAPI_CONFIG_AIDING_DATA_DELETION_MSG_ID:
        configType = CONFIG_AIDING_DATA_DELETION;
        break;
    case E_INTAPI_CONFIG_LEVER_ARM_MSG_ID:
        configType = CONFIG_LEVER_ARM;
        break;
    case E_INTAPI_CONFIG_ROBUST_LOCATION_MSG_ID:
        configType = CONFIG_ROBUST_LOCATION;
        break;
    case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID:
    case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_RESP_MSG_ID:
        configType = GET_ROBUST_LOCATION_CONFIG;
        break;
    default:
        break;
    }
    return configType;
}

static LocIntegrationResponse getLocIntegrationResponse(::LocationError error) {
    LocIntegrationResponse response;

    switch (error) {
        case LOCATION_ERROR_SUCCESS:
            response = LOC_INT_RESPONSE_SUCCESS;
            break;
        case LOCATION_ERROR_NOT_SUPPORTED:
            response = LOC_INT_RESPONSE_NOT_SUPPORTED;
            break;
        case LOCATION_ERROR_INVALID_PARAMETER:
            response = LOC_INT_RESPONSE_PARAM_INVALID;
            break;
        default:
            response = LOC_INT_RESPONSE_FAILURE;
            break;
    }

    return response;
}

/******************************************************************************
ILocIpcListener override
******************************************************************************/
class IpcListener : public ILocIpcListener {
    MsgTask& mMsgTask;
    LocationIntegrationApiImpl& mApiImpl;
public:
    inline IpcListener(LocationIntegrationApiImpl& apiImpl, MsgTask& msgTask) :
            mMsgTask(msgTask), mApiImpl(apiImpl) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data, uint32_t length,
                           const LocIpcRecver* recver) override;
};

/******************************************************************************
LocationIntegrationApiImpl
******************************************************************************/
mutex LocationIntegrationApiImpl::mMutex;
bool LocationIntegrationApiImpl::mClientRunning = false;

/******************************************************************************
LocationIntegrationApiImpl - constructors
******************************************************************************/
LocationIntegrationApiImpl::LocationIntegrationApiImpl(LocIntegrationCbs& integrationCbs) :
        mHalRegistered(false),
        mIntegrationCbs(integrationCbs),
        mTuncConfigInfo{},
        mPaceConfigInfo{},
        mSVConfigInfo{},
        mLeverArmConfigInfo{} {
    if (integrationClientAllowed() == false) {
        return;
    }

    mMsgTask = new MsgTask("IntegrationApiImpl", false);
    // get pid to generate sokect name
    uint32_t pid = (uint32_t)getpid();

#ifdef FEATURE_EXTERNAL_AP
    SockNodeEap sock(LOCATION_CLIENT_API_QSOCKET_CLIENT_SERVICE_ID,
                     pid * 100);
    strlcpy(mSocketName, sock.getNodePathname().c_str(), sizeof(mSocketName));
    unique_ptr<LocIpcRecver> recver = LocIpc::getLocIpcQrtrRecver(
            make_shared<IpcListener>(*this, *mMsgTask), sock.getId1(), sock.getId2());

    // establish an ipc sender to the hal daemon
    mIpcSender = LocIpc::getLocIpcQrtrSender(LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID,
                                             LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID);
    if (mIpcSender == nullptr) {
        LOC_LOGe("create sender socket failed for service id: %d instance id: %d",
                 LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID,
                 LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID);
        return;
    }
#else
    SockNodeLocal sock(LOCATION_INTEGRATION_API, pid, 0);

    strlcpy(mSocketName, sock.getNodePathname().c_str(), sizeof(mSocketName));
    unique_ptr<LocIpcRecver> recver = LocIpc::getLocIpcLocalRecver(
            make_shared<IpcListener>(*this, *mMsgTask), mSocketName);

    LOC_LOGd("create sender socket: %s", mSocketName);
    // establish an ipc sender to the hal daemon
    mIpcSender = LocIpc::getLocIpcLocalSender(SOCKET_TO_LOCATION_HAL_DAEMON);
    if (mIpcSender == nullptr) {
        LOC_LOGe("create sender socket failed %s", SOCKET_TO_LOCATION_HAL_DAEMON);
        return;
    }
#endif //  FEATURE_EXTERNAL_AP

    LOC_LOGd("listen on socket: %s", mSocketName);
    mIpc.startNonBlockingListening(recver);
}

LocationIntegrationApiImpl::~LocationIntegrationApiImpl() {
}

void LocationIntegrationApiImpl::destroy() {

    struct DestroyReq : public LocMsg {
        DestroyReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~DestroyReq() {}
        void proc() const {
            // deregister
            if (mApiImpl->mHalRegistered && (nullptr != mApiImpl->mIpcSender)) {
                LocAPIClientDeregisterReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
                LOC_LOGd(">>> DeregisterReq rc=%d", rc);
                mApiImpl->mIpcSender = nullptr;
            }

            if (mApiImpl->mMsgTask) {
                mApiImpl->mMsgTask->destroy();
            }

            // delete the integration client, so another integration client can be set
            {
                lock_guard<mutex> lock(mMutex);
                mApiImpl->mClientRunning = false;
            }
            delete mApiImpl;
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    mMsgTask->sendMsg(new (nothrow) DestroyReq(this));
}

bool LocationIntegrationApiImpl::integrationClientAllowed() {
    lock_guard<mutex> lock(mMutex);

    // allow only one instance of integration client running per process
    if (!mClientRunning) {
        mClientRunning = true;
        return true;
    } else {
         LOC_LOGe("there is already a running location integration api client in the process");
         return false;
    }
}

/******************************************************************************
LocationIntegrationApiImpl -ILocIpcListener
******************************************************************************/
void IpcListener::onListenerReady() {
    struct ClientRegisterReq : public LocMsg {
        ClientRegisterReq(LocationIntegrationApiImpl& apiImpl) : mApiImpl(apiImpl) {}
        void proc() const {
            mApiImpl.sendClientRegMsgToHalDaemon();
        }
        LocationIntegrationApiImpl& mApiImpl;
    };
    mMsgTask.sendMsg(new (nothrow) ClientRegisterReq(mApiImpl));
}

void IpcListener::onReceive(const char* data, uint32_t length,
                            const LocIpcRecver* recver) {
    struct OnReceiveHandler : public LocMsg {
        OnReceiveHandler(LocationIntegrationApiImpl& apiImpl, IpcListener& listener,
                         const char* data, uint32_t length) :
                mApiImpl(apiImpl), mListener(listener), mMsgData(data, length) {}

        virtual ~OnReceiveHandler() {}
        void proc() const {
            LocAPIMsgHeader *pMsg = (LocAPIMsgHeader *)(mMsgData.data());
            switch (pMsg->msgId) {
            case E_LOCAPI_HAL_READY_MSG_ID:
                LOC_LOGd("<<< HAL ready");
                if (sizeof(LocAPIHalReadyIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                // location hal daemon crashed and re-started
                mApiImpl.processHalReadyMsg();
                break;

            case E_INTAPI_CONFIG_CONSTRAINTED_TUNC_MSG_ID:
            case E_INTAPI_CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR_MSG_ID:
            case E_INTAPI_CONFIG_SV_CONSTELLATION_MSG_ID:
            case E_INTAPI_CONFIG_AIDING_DATA_DELETION_MSG_ID:
            case E_INTAPI_CONFIG_LEVER_ARM_MSG_ID:
            case E_INTAPI_CONFIG_ROBUST_LOCATION_MSG_ID:
            case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID:
            {
                if (sizeof(LocAPIGenericRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.processConfigRespCb((LocAPIGenericRespMsg*)pMsg);
                break;
            }

            case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_RESP_MSG_ID:
            {
                if (sizeof(LocConfigGetRobustLocationConfigRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.processGetRobustLocationConfigRespCb(
                        (LocConfigGetRobustLocationConfigRespMsg*)pMsg);
                break;
            }

            default:
                LOC_LOGw("<<< unknown message %d", pMsg->msgId);
                break;
            }
        }
        LocationIntegrationApiImpl& mApiImpl;
        IpcListener& mListener;
        const string mMsgData;
    };

    mMsgTask.sendMsg(new (nothrow) OnReceiveHandler(mApiImpl, *this, data, length));
}

/******************************************************************************
LocationIntegrationApi - integration API implementation
******************************************************************************/
uint32_t LocationIntegrationApiImpl::resetConstellationConfig() {
    struct ResetConstellationConfigReq : public LocMsg {
        ResetConstellationConfigReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~ResetConstellationConfigReq() {}
        void proc() const {
            GnssSvTypeConfig svTypeConfig = {};
            GnssSvIdConfig   svIdConfig = {};
            LocConfigSvConstellationReqMsg msg(mApiImpl->mSocketName,
                                               true, // resetToDefault: true
                                               svTypeConfig,
                                               svIdConfig);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_CONSTELLATIONS,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
           // cache the last config to be used when hal daemon restarts
            mApiImpl->mSVConfigInfo.isValid = true;
            mApiImpl->mSVConfigInfo.resetToDeFault = true;
            mApiImpl->mSVConfigInfo.svTypeConfig = {};
            mApiImpl->mSVConfigInfo.svIdConfig = {};
        }
        LocationIntegrationApiImpl* mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) ResetConstellationConfigReq(this));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configConstellations(
        const GnssSvTypeConfig& svTypeConfig,
        const GnssSvIdConfig&   svIdConfig) {

    struct ConfigConstellationsReq : public LocMsg {
        ConfigConstellationsReq(LocationIntegrationApiImpl* apiImpl,
                                const GnssSvTypeConfig& svTypeConfig,
                                const GnssSvIdConfig& svIdConfig) :
                mApiImpl(apiImpl),
                mSvTypeConfig(svTypeConfig),
                mSvIdConfig(svIdConfig) {}
        virtual ~ConfigConstellationsReq() {}
        void proc() const {
            LocConfigSvConstellationReqMsg msg(mApiImpl->mSocketName,
                                               false, // resetToDefault: false
                                               mSvTypeConfig,
                                               mSvIdConfig);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_CONSTELLATIONS,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
           // cache the last config to be used when hal daemon restarts
            mApiImpl->mSVConfigInfo.isValid = true;
            mApiImpl->mSVConfigInfo.resetToDeFault = false;
            mApiImpl->mSVConfigInfo.svTypeConfig = mSvTypeConfig;
            mApiImpl->mSVConfigInfo.svIdConfig = mSvIdConfig;
        }

        LocationIntegrationApiImpl* mApiImpl;
        GnssSvTypeConfig mSvTypeConfig;
        GnssSvIdConfig mSvIdConfig;
    };
    mMsgTask->sendMsg(new (nothrow) ConfigConstellationsReq(
            this, svTypeConfig, svIdConfig));
    return 0;
}

uint32_t LocationIntegrationApiImpl::configConstrainedTimeUncertainty(
        bool enable, float tuncThreshold, uint32_t energyBudget) {

    struct ConfigConstrainedTuncReq : public LocMsg {
        ConfigConstrainedTuncReq(LocationIntegrationApiImpl* apiImpl,
                                 bool enable,
                                 float tuncThreshold,
                                 uint32_t energyBudget) :
                mApiImpl(apiImpl),
                mEnable(enable),
                mTuncThreshold(tuncThreshold),
                mEnergyBudget(energyBudget) {}
        virtual ~ConfigConstrainedTuncReq() {}
        void proc() const {
            LocConfigConstrainedTuncReqMsg msg(mApiImpl->mSocketName,
                                               mEnable, mTuncThreshold, mEnergyBudget);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_CONSTRAINED_TIME_UNCERTAINTY,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
           // cache the last config to be used when hal daemon restarts
            mApiImpl->mTuncConfigInfo.isValid = true;
            mApiImpl->mTuncConfigInfo.enable = mEnable;
            mApiImpl->mTuncConfigInfo.tuncThresholdMs = mTuncThreshold;
            mApiImpl->mTuncConfigInfo.energyBudget = mEnergyBudget;
        }
        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
        float mTuncThreshold;
        uint32_t mEnergyBudget;
    };
    mMsgTask->sendMsg(new (nothrow)ConfigConstrainedTuncReq(
            this, enable, tuncThreshold, energyBudget));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configPositionAssistedClockEstimator(bool enable) {

    struct ConfigPositionAssistedClockEstimatorReq : public LocMsg {
        ConfigPositionAssistedClockEstimatorReq(LocationIntegrationApiImpl* apiImpl,
                                                bool enable) :
                mApiImpl(apiImpl),
                mEnable(enable) {}
        virtual ~ConfigPositionAssistedClockEstimatorReq() {}
        void proc() const {
            LocConfigPositionAssistedClockEstimatorReqMsg msg(mApiImpl->mSocketName,
                                                              mEnable);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR,
                                         reinterpret_cast<uint8_t*>(&msg),
                                         sizeof(msg));
           // cache the last config to be used when hal daemon restarts
            mApiImpl->mPaceConfigInfo.isValid = true;
            mApiImpl->mPaceConfigInfo.enable = mEnable;
        }
        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
    };
    mMsgTask->sendMsg(new (nothrow)
            ConfigPositionAssistedClockEstimatorReq(this, enable));

    return 0;
}

uint32_t LocationIntegrationApiImpl::gnssDeleteAidingData(
        GnssAidingData& aidingData) {
    struct DeleteAidingDataReq : public LocMsg {
        DeleteAidingDataReq(LocationIntegrationApiImpl* apiImpl,
                            GnssAidingData& aidingData) :
                mApiImpl(apiImpl),
                mAidingData(aidingData) {}
        virtual ~DeleteAidingDataReq() {}
        void proc() const {
            LocConfigAidingDataDeletionReqMsg msg(mApiImpl->mSocketName,
                                                  const_cast<GnssAidingData&>(mAidingData));
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_AIDING_DATA_DELETION,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
        GnssAidingData mAidingData;
    };
    mMsgTask->sendMsg(new (nothrow) DeleteAidingDataReq(this, aidingData));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configLeverArm(
        const LeverArmConfigInfo& configInfo) {
    struct ConfigLeverArmReq : public LocMsg {
        ConfigLeverArmReq(LocationIntegrationApiImpl* apiImpl,
                          const LeverArmConfigInfo& configInfo) :
                mApiImpl(apiImpl),
                mConfigInfo(configInfo) {}
        virtual ~ConfigLeverArmReq() {}
        void proc() const {
            LocConfigLeverArmReqMsg msg(mApiImpl->mSocketName, mConfigInfo);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_LEVER_ARM,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
            // cache the last config to be used when hal daemon restarts
            mApiImpl->mLeverArmConfigInfo = mConfigInfo;
        }
        LocationIntegrationApiImpl* mApiImpl;
        LeverArmConfigInfo mConfigInfo;
    };

    if (configInfo.leverArmValidMask) {
        mMsgTask->sendMsg(new (nothrow) ConfigLeverArmReq(this, configInfo));
    }

    return 0;
}


uint32_t LocationIntegrationApiImpl::configRobustLocation(
        bool enable, bool enableForE911) {
    struct ConfigRobustLocationReq : public LocMsg {
        ConfigRobustLocationReq(LocationIntegrationApiImpl* apiImpl,
                                bool enable,
                                bool enableForE911) :
                mApiImpl(apiImpl),
                mEnable(enable),
                mEnableForE911(enableForE911){}
        virtual ~ConfigRobustLocationReq() {}
        void proc() const {
            LocConfigRobustLocationReqMsg msg(mApiImpl->mSocketName, mEnable, mEnableForE911);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_ROBUST_LOCATION,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
            mApiImpl->mRobustLocationConfigInfo.isValid = true;
            mApiImpl->mRobustLocationConfigInfo.enable = mEnable;
            mApiImpl->mRobustLocationConfigInfo.enableForE911 = mEnableForE911;
        }
        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
        bool mEnableForE911;
    };

    mMsgTask->sendMsg(new (nothrow)
                      ConfigRobustLocationReq(this, enable, enableForE911));

    return 0;
}

uint32_t LocationIntegrationApiImpl::getRobustLocationConfig() {

    struct GetRobustLocationConfigReq : public LocMsg {
        GetRobustLocationConfigReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~GetRobustLocationConfigReq() {}
        void proc() const {
            LocConfigGetRobustLocationConfigReqMsg msg(mApiImpl->mSocketName);
            mApiImpl->sendConfigMsgToHalDaemon(GET_ROBUST_LOCATION_CONFIG,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    if (mIntegrationCbs.getRobustLocationConfigCb == nullptr) {
        LOC_LOGe("no callback passed in constructor to receive robust location config");
        // return 1 to signal error
        return 1;
    }
    mMsgTask->sendMsg(new (nothrow) GetRobustLocationConfigReq(this));

    return 0;
}

void LocationIntegrationApiImpl::sendConfigMsgToHalDaemon(
        LocConfigTypeEnum configType, uint8_t* pMsg,
        size_t msgSize, bool invokeResponseCb) {

    bool messageSentToHal = false;
    if (mHalRegistered) {
        bool rc = sendMessage(pMsg, msgSize);
        LOC_LOGd(">>> sendConfigMsgToHalDaemon, msg type=%d, rc=%d", configType, rc);
        if (true == rc) {
            messageSentToHal = true;
        } else {
            LOC_LOGe(">>> sendConfigMsgToHalDaemon, msg type=%d, rc=%d", configType, rc);
        }
    }

    if (invokeResponseCb && mIntegrationCbs.configCb) {
        if (true == messageSentToHal) {
            addConfigReq(configType);
        } else {
            mIntegrationCbs.configCb(configType, LOC_INT_RESPONSE_FAILURE);
        }
    }
}

void LocationIntegrationApiImpl::sendClientRegMsgToHalDaemon(){

    LocAPIClientRegisterReqMsg msg(mSocketName, LOCATION_INTEGRATION_API);
    bool rc = sendMessage(reinterpret_cast<uint8_t *>(&msg), sizeof(msg));
    LOC_LOGd(">>> onListenerReady::ClientRegisterReqMsg rc=%d", rc);
    if (true == rc) {
        mHalRegistered = true;
    }
}

void LocationIntegrationApiImpl::processHalReadyMsg() {
    // when location hal daemon crashes and restarts,
    // we flush out all pending requests and notify each client
    // that the request has failed.
    flushConfigReqs();

    // when hal daemon crashes and then restarted,
    // we need to find the new node/port when remote socket api is used,
    //
    // this code can not be moved to inside of onListenerReady as
    // onListenerReady can be invoked from other places
    if (mIpcSender != nullptr) {
        mIpcSender->informRecverRestarted();
    }

    // register with hal daemon
    sendClientRegMsgToHalDaemon();

    // send cached configuration to hal daemon
    if (mSVConfigInfo.isValid) {
        LocConfigSvConstellationReqMsg msg(mSocketName,
                                           mSVConfigInfo.resetToDeFault,
                                           mSVConfigInfo.svTypeConfig,
                                           mSVConfigInfo.svIdConfig);
        sendConfigMsgToHalDaemon(CONFIG_CONSTELLATIONS,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg), false);
    }
    if (mTuncConfigInfo.isValid) {
        LocConfigConstrainedTuncReqMsg msg(mSocketName,
                                           mTuncConfigInfo.enable,
                                           mTuncConfigInfo.tuncThresholdMs,
                                           mTuncConfigInfo.energyBudget);
        sendConfigMsgToHalDaemon(CONFIG_CONSTRAINED_TIME_UNCERTAINTY,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg), false);
    }
    if (mPaceConfigInfo.isValid) {
        LocConfigPositionAssistedClockEstimatorReqMsg msg(mSocketName,
                                                          mPaceConfigInfo.enable);
        sendConfigMsgToHalDaemon(CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg), false);
    }
    if (mLeverArmConfigInfo.leverArmValidMask) {
        LocConfigLeverArmReqMsg msg(mSocketName, mLeverArmConfigInfo);
        sendConfigMsgToHalDaemon(CONFIG_LEVER_ARM,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg));
    }
    if (mRobustLocationConfigInfo.isValid) {
        LocConfigRobustLocationReqMsg msg(mSocketName,
                                          mRobustLocationConfigInfo.enable,
                                          mRobustLocationConfigInfo.enableForE911);
        sendConfigMsgToHalDaemon(CONFIG_ROBUST_LOCATION,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg));
    }
}

void LocationIntegrationApiImpl::addConfigReq(LocConfigTypeEnum configType) {
    LOC_LOGv("add configType %d", configType);

    auto configReqData = mConfigReqCntMap.find(configType);
    if (configReqData == std::end(mConfigReqCntMap)) {
        mConfigReqCntMap.emplace(configType, 1);
    } else{
        int newCnt = configReqData->second+1;
        mConfigReqCntMap.erase(configReqData);
        mConfigReqCntMap.emplace(configType, newCnt);
    }
}

void LocationIntegrationApiImpl::processConfigRespCb(const LocAPIGenericRespMsg* pRespMsg) {
    LocConfigTypeEnum configType = getLocConfigTypeFromMsgId(pRespMsg->msgId);
    LocIntegrationResponse intResponse = getLocIntegrationResponse(pRespMsg->err);
    LOC_LOGd("<<< response message id: %d, msg err: %d, "
             "config type: %d, int response %d",
             pRespMsg->msgId, pRespMsg->err, configType, intResponse);

    if (mIntegrationCbs.configCb) {
        auto configReqData = mConfigReqCntMap.find(configType);
        if (configReqData != std::end(mConfigReqCntMap)) {
            int reqCnt = configReqData->second;
            if (reqCnt > 0) {
                mIntegrationCbs.configCb(configType, intResponse);
            }
            mConfigReqCntMap.erase(configReqData);
            // there are still some request pending for this config type
            // need to store it in the map
            if (--reqCnt > 0) {
                mConfigReqCntMap.emplace(configType, reqCnt);
            }
        }
    }
}

// flush all the pending config request if location hal daemon has crashed
// and restarted
void LocationIntegrationApiImpl::flushConfigReqs() {
    for (auto itr=mConfigReqCntMap.begin(); itr!=mConfigReqCntMap.end(); ++itr) {
         int reqCnt = itr->second;
         while (reqCnt-- > 0) {
             mIntegrationCbs.configCb(itr->first, LOC_INT_RESPONSE_FAILURE);
         }
    }
    mConfigReqCntMap.clear();
}

void LocationIntegrationApiImpl::processGetRobustLocationConfigRespCb(
        const LocConfigGetRobustLocationConfigRespMsg* pRespMsg) {

    LOC_LOGd("<<< response message id: %d, mask 0x%x, enabled: %d, enabledFor911: %d",
             pRespMsg->msgId,
             pRespMsg->mRobustLoationConfig.validMask,
             pRespMsg->mRobustLoationConfig.enabled,
             pRespMsg->mRobustLoationConfig.enabledForE911);

    if (mIntegrationCbs.getRobustLocationConfigCb) {
        // conversion between the enums
        RobustLocationConfig robustConfig = {};
        uint32_t validMask = 0;;
        if (pRespMsg->mRobustLoationConfig.validMask &
                GNSS_CONFIG_ROBUST_LOCATION_ENABLED_VALID_BIT) {
            validMask |= ROBUST_LOCATION_CONFIG_VALID_ENABLED;
            robustConfig.enabled = pRespMsg->mRobustLoationConfig.enabled;
        }
        if (pRespMsg->mRobustLoationConfig.validMask &
                GNSS_CONFIG_ROBUST_LOCATION_ENABLED_FOR_E911_VALID_BIT) {
            validMask |= ROBUST_LOCATION_CONFIG_VALID_ENABLED_FOR_E911;
            robustConfig.enabledForE911 = pRespMsg->mRobustLoationConfig.enabledForE911;
        }

        robustConfig.validMask = (RobustLocationConfigValidMask) validMask;
        mIntegrationCbs.getRobustLocationConfigCb(robustConfig);
    }
}

/******************************************************************************
LocationIntegrationApiImpl - Not implemented ILocationControlAPI functions
******************************************************************************/
uint32_t* LocationIntegrationApiImpl::gnssUpdateConfig(GnssConfig config) {
    return nullptr;
}

} // namespace location_integration
