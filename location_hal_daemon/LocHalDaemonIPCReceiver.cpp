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

#include <LocHalDaemonIPCReceiver.h>
#include <LocationApiService.h>

/******************************************************************************
LocHalDaemonIPCReceiver - LocIpc overrids
******************************************************************************/
void LocHalDaemonIPCReceiver::onReceive(const std::string& data) {

    // parse received message
    LocAPIMsgHeader* pMsg = (LocAPIMsgHeader*)(data.data());
    LOC_LOGd(">-- onReceive len=%u remote=%s msgId=%u",
            data.length(), pMsg->mSocketName, pMsg->msgId);

    switch (pMsg->msgId) {
        case E_LOCAPI_CLIENT_REGISTER_MSG_ID: {
            // new client
            if (sizeof(LocAPIClientRegisterReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->newClient(reinterpret_cast
                    <LocAPIClientRegisterReqMsg*>(pMsg));
            break;
        }
        case E_LOCAPI_CLIENT_DEREGISTER_MSG_ID: {
            // delete client
            if (sizeof(LocAPIClientDeregisterReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->deleteClient(reinterpret_cast
                    <LocAPIClientDeregisterReqMsg*>(pMsg));
            break;
        }

        case E_LOCAPI_START_TRACKING_MSG_ID: {
            // start
            if (sizeof(LocAPIStartTrackingReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->startTracking(reinterpret_cast
                    <LocAPIStartTrackingReqMsg*>(pMsg));
            break;
        }
        case E_LOCAPI_STOP_TRACKING_MSG_ID: {
            // stop
            if (sizeof(LocAPIStopTrackingReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->stopTracking(reinterpret_cast
                    <LocAPIStopTrackingReqMsg*>(pMsg));
            break;
        }
        case E_LOCAPI_UPDATE_CALLBACKS_MSG_ID: {
            // update subscription
            if (sizeof(LocAPIUpdateCallbacksReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->updateSubscription(reinterpret_cast
                    <LocAPIUpdateCallbacksReqMsg*>(pMsg));
            break;
        }
        case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID: {
            if (sizeof(LocAPIUpdateTrackingOptionsReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->updateTrackingOptions(reinterpret_cast
                    <LocAPIUpdateTrackingOptionsReqMsg*>(pMsg));
            break;
        }

        case E_LOCAPI_CONTROL_UPDATE_CONFIG_MSG_ID: {
            if (sizeof(LocAPIUpdateConfigReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->gnssUpdateConfig(reinterpret_cast<
                    LocAPIUpdateConfigReqMsg*>(pMsg)->gnssConfig);
            break;
        }
        case E_LOCAPI_CONTROL_DELETE_AIDING_DATA_MSG_ID: {
            if (sizeof(LocAPIDeleteAidingDataReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->gnssDeleteAidingData(reinterpret_cast
                    <LocAPIDeleteAidingDataReqMsg*>(pMsg)->gnssAidingData);
            break;
        }
        case E_LOCAPI_CONTROL_UPDATE_NETWORK_AVAILABILITY_MSG_ID: {
            if (sizeof(LocAPIUpdateNetworkAvailabilityReqMsg) != data.length()) {
                LOC_LOGe("invalid message");
                break;
            }
            mService->updateNetworkAvailability(reinterpret_cast
                    <LocAPIUpdateNetworkAvailabilityReqMsg*>(pMsg)->mAvailability);
            break;
        }
        default: {
            LOC_LOGe("Unknown message");
            break;
        }
    }
}

void LocHalDaemonIPCReceiver::onListenerReady() {
    if (0 != chown(SOCKET_TO_LOCATION_HAL_DAEMON, UID_GPS, GID_LOCCLIENT)) {
        LOC_LOGe("chown to group locclient failed %s", strerror(errno));
    }

    mService->onListenerReady();
}

