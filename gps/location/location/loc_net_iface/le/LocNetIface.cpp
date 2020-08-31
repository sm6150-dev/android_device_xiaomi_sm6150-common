/* Copyright (c) 2017, 2020 The Linux Foundation. All rights reserved.
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
#define LOG_TAG "LocSvc_LocNetIfaceLE"

#include "LocNetIface.h"
#include <QCMAP_Client.h>
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include <loc_pla.h>
#include "DataItemConcreteTypes.h"
#include <loc_cfg.h>
#include <log_util.h>
#include <unistd.h>

using namespace izat_manager;

/* LocNetIface singleton instance
 * Used for QCMAP registration */
LocNetIface* LocNetIface::sLocNetIfaceInstance = NULL;

void LocNetIface::subscribe(
        const std::list<DataItemId>& itemListToSubscribe) {

    ENTRY_LOG();

    /* Add items to subscribed list */
    bool anyUpdatesToSubscriptionList =
            updateSubscribedItemList(itemListToSubscribe, true);

    /* If either of network info items is in subscription list,
     * subscribe with QCMAP */
    if (anyUpdatesToSubscriptionList) {
        if (isItemSubscribed(NETWORKINFO_DATA_ITEM_ID)) {
            subscribeWithQcmap();
            notifyCurrentNetworkInfo(true);
        }
        if (isItemSubscribed(WIFIHARDWARESTATE_DATA_ITEM_ID)) {
            subscribeWithQcmap();
            notifyCurrentWifiHardwareState(true);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void LocNetIface::unsubscribe(
        const std::list<DataItemId>& itemListToUnsubscribe) {

    ENTRY_LOG();

    /* Remove items from subscribed list */
    bool anyUpdatesToSubscriptionList =
            updateSubscribedItemList(itemListToUnsubscribe, false);

    /* If neither of below two items left in subscription, we can unsubscribe
     * from QCMAP */
    if (anyUpdatesToSubscriptionList &&
            !isItemSubscribed(NETWORKINFO_DATA_ITEM_ID) &&
            !isItemSubscribed(WIFIHARDWARESTATE_DATA_ITEM_ID)) {

        unsubscribeWithQcmap();
    }
}

void LocNetIface::unsubscribeAll() {

    ENTRY_LOG();

    /* Check about network items */
    if (isItemSubscribed(NETWORKINFO_DATA_ITEM_ID) ||
            isItemSubscribed(WIFIHARDWARESTATE_DATA_ITEM_ID)) {

        unsubscribeWithQcmap();
    }

    /* Clear subscription list */
    mSubscribedItemList.clear();
}

void LocNetIface::requestData(
        const std::list<DataItemId>& itemListToRequestData) {

    ENTRY_LOG();

    /* NO-OP for LE platform
     * We don't support any data item to fetch data for */
}

void LocNetIface::subscribeWithQcmap() {

    ENTRY_LOG();

    qmi_error_type_v01 qcmapErr = QMI_ERR_NONE_V01;

    /* We handle qcmap subscription from an exclusive instance */
    if (LocNetIface::sLocNetIfaceInstance != NULL) {

        LOC_LOGI("QCMAP registration already done !");
        return;
    }

    /* First time registration */
    if (LocNetIface::sLocNetIfaceInstance == NULL) {
        LocNetIface::sLocNetIfaceInstance = this;
    }

    /* Are we already subscribed */
    if (mQcmapClientPtr != NULL) {
        LOC_LOGW("Already subscribed !");
        return;
    }

    /* Create a QCMAP Client instance */
    mQcmapClientPtr = new QCMAP_Client(qcmapClientCallback);
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("Failed to allocate QCMAP instance !");
        return;
    }
    LOC_LOGD("Created QCMAP_Client instance %p", mQcmapClientPtr);

#ifdef FEATURE_MOBILEAP_INDICATION
    // We need to Enable/Disable mobile AP only for backhaul connection only if the feature
    // FEATURE_MOBILEAP_INDICATION is available, since RegisterForIndications will give us
    // network notification and we don't need to keep mobileap enabled for the same.
    /* Need to RegisterForIndications to get station mode status indications */
    uint64_t reg_mask = WWAN_ROAMING_STATUS_IND|BACKHAUL_STATUS_IND|WWAN_STATUS_IND| \
            MOBILE_AP_STATUS_IND|STATION_MODE_STATUS_IND|CRADLE_MODE_STATUS_IND| \
            ETHERNET_MODE_STATUS_IND|BT_TETHERING_STATUS_IND|BT_TETHERING_WAN_IND| \
            WLAN_STATUS_IND|PACKET_STATS_STATUS_IND;
    bool ret  = false;
    //Register with QCMAP for any BACKHAUL/network availability
    ret = mQcmapClientPtr->RegisterForIndications(&qcmapErr, reg_mask);
    LOC_LOGI("RegisterForIndications - qmi_error %d status %d\n", qcmapErr, ret);
    if (QMI_ERR_NONE_V01 != qcmapErr)
    {
        LOC_LOGE("Backhaul registration failed error value: %d",qcmapErr);
    }
#else
    /* Need to enable MobileAP to get station mode status indications */
    bool ret = mQcmapClientPtr->EnableMobileAP(&qcmapErr);
    if (ret == false || qcmapErr != 0) {
        LOC_LOGE("Failed to enable mobileap, qcmapErr %d", qcmapErr);
    }
    /* Invoke WLAN status registration
     * WWAN is by default registered */
    ret = mQcmapClientPtr->RegisterForWLANStatusIND(&qcmapErr, true);
    if (ret == false || qcmapErr != 0) {
        LOC_LOGE("RegisterForWLANStatusIND failed, qcmapErr %d", qcmapErr);
    }
#endif
}

void LocNetIface::unsubscribeWithQcmap() {

    ENTRY_LOG();

    // Simply deleting the qcmap client instance is enough
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance to unsubscribe from");
        return;
    }

    delete mQcmapClientPtr;
    mQcmapClientPtr = NULL;
}

void LocNetIface::qcmapClientCallback(
        qmi_client_type user_handle, /**< QMI user handle. */
        unsigned int msg_id, /**< Indicator message ID. */
        void *ind_buf, /**< Raw indication data. */
        unsigned int ind_buf_len, /**< Raw data length. */
        void *ind_cb_data /**< User callback handle. */ ) {

    ENTRY_LOG();
    lock_guard<recursive_mutex> guard(
            LocNetIface::sLocNetIfaceInstance->getMutex());

    qmi_client_error_type qmi_error;

    // Check the message type
    // msg_id  = QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01
    // ind_buf = qcmap_msgr_wlan_status_ind_msg_v01
    switch (msg_id) {

    case QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01: {
        LOC_LOGD("Received QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01");

        qcmap_msgr_wlan_status_ind_msg_v01 wlanStatusIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &wlanStatusIndData,
                sizeof(qcmap_msgr_wlan_status_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        LocNetIface::sLocNetIfaceInstance->handleQcmapCallback(wlanStatusIndData);
        break;
    }

    case QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01: {
        LOC_LOGD("Received QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01");

        qcmap_msgr_bring_up_wwan_ind_msg_v01 bringUpWwanIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &bringUpWwanIndData,
                sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        LocNetIface::sLocNetIfaceInstance->handleQcmapCallback(bringUpWwanIndData);
        break;
    }

    case QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01: {
        LOC_LOGD("Received QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01");

        qcmap_msgr_tear_down_wwan_ind_msg_v01 teardownWwanIndData;

        /* Parse the indication */
        qmi_error = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
                msg_id, ind_buf, ind_buf_len, &teardownWwanIndData,
                sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01));

        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        LocNetIface::sLocNetIfaceInstance->handleQcmapCallback(teardownWwanIndData);
        break;
    }

#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
    case QMI_QCMAP_MSGR_BACKHAUL_STATUS_IND_V01:
    {
        qcmap_msgr_backhaul_status_ind_msg_v01 backhaulStatusData;

        qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &backhaulStatusData,
                           sizeof(qcmap_msgr_backhaul_status_ind_msg_v01));
        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        LocNetIface::sLocNetIfaceInstance->handleQcmapCallback(backhaulStatusData);
        break;
     }

    case QMI_QCMAP_MSGR_WWAN_ROAMING_STATUS_IND_V01:
    {
        qcmap_msgr_wwan_roaming_status_ind_msg_v01 roamingStatusData;

        qmi_error = qmi_client_message_decode(user_handle,
                           QMI_IDL_INDICATION,
                           msg_id,
                           ind_buf,
                           ind_buf_len,
                           &roamingStatusData,
                           sizeof(qcmap_msgr_wwan_roaming_status_ind_msg_v01));
        if (qmi_error != QMI_NO_ERR) {
            LOC_LOGE("qmi_client_message_decode error %d", qmi_error);
            return;
        }

        LocNetIface::sLocNetIfaceInstance->handleQcmapCallback(roamingStatusData);
        break;
    }
#endif

    default:
        LOC_LOGE("Ignoring QCMAP indication: %d", msg_id);
    }
}

void LocNetIface::handleQcmapCallback(
        qcmap_msgr_wlan_status_ind_msg_v01 &wlanStatusIndData) {

    ENTRY_LOG();

    LOC_LOGD("WLAN Status (enabled=1, disabled=2): %d",
            wlanStatusIndData.wlan_status);

    LOC_LOGD("WLAN Mode (AP=1, ... STA=6): %d",
            wlanStatusIndData.wlan_mode);

    /* Notify observers */
    if (wlanStatusIndData.wlan_status == QCMAP_MSGR_WLAN_ENABLED_V01) {
        mLocNetWlanState =LOC_NET_CONN_STATE_ENABLED;
        notifyObserverForWlanStatus(true);
    } else if (wlanStatusIndData.wlan_status == QCMAP_MSGR_WLAN_DISABLED_V01) {
        mLocNetWlanState = LOC_NET_CONN_STATE_DISABLED;
        notifyObserverForWlanStatus(false);
    } else {
        LOC_LOGE("Invalid wlan status %d", wlanStatusIndData.wlan_status);
    }
}

#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
void LocNetIface::handleQcmapCallback(
            qcmap_msgr_backhaul_status_ind_msg_v01 &backhaulStatusIndData){
    ENTRY_LOG();

    if (true == backhaulStatusIndData.backhaul_type_valid)
    {
        boolean isIpv4Avail = ((backhaulStatusIndData.backhaul_v4_status_valid
                    && backhaulStatusIndData.backhaul_v4_status));
        boolean isIpv6Avail = ((backhaulStatusIndData.backhaul_v6_status_valid
                    && backhaulStatusIndData.backhaul_v6_status));
        setCurrentBackHaulStatus(backhaulStatusIndData.backhaul_type,
                                 isIpv4Avail, isIpv6Avail);
        notifyCurrentNetworkInfo(false);
    }
    else {
        LOC_LOGE("Backhaul type is not valid : %d", backhaulStatusIndData.backhaul_type_valid);
        mLocNetBackHaulState = LOC_NET_CONN_STATE_INVALID;
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_INVALID;
    }
}
#endif

void LocNetIface::handleQcmapCallback (
        qcmap_msgr_bring_up_wwan_ind_msg_v01 &bringUpWwanIndData) {

    ENTRY_LOG();

    LOC_LOGD("WWAN Bring up status (Connected_v4,v6=3,9, connecting fail_v4,v6=2,8): %d",
            bringUpWwanIndData.conn_status);

    /* Notify observers */
    if (bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 ||
            bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01) {
        //We update state and type in backhaul status CB only
        if (mIsConnectBackhaulPending &&
                mWwanCallStatusCb != NULL){
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }
        mIsConnectBackhaulPending = false;

      } else if (bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 ||
               bringUpWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01) {

        if (mIsConnectBackhaulPending &&
                mWwanCallStatusCb != NULL){
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_OPEN_FAILED");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_OPEN_FAILED, NULL,
                    LOC_NET_CONN_IP_TYPE_INVALID);
        }
        mIsConnectBackhaulPending = false;

    } else {
        LOC_LOGW("Unsupported wwan status %d",
                bringUpWwanIndData.conn_status);
    }
}

void LocNetIface::handleQcmapCallback(
        qcmap_msgr_tear_down_wwan_ind_msg_v01 &teardownWwanIndData) {

    ENTRY_LOG();

    LOC_LOGD("WWAN teardown status (Disconnected_v4,v6=6,12) (Disconnecting fail_v4,v6=5,11): %d",
            teardownWwanIndData.conn_status);

    /* Notify observers */
    if (teardownWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 ||
        teardownWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01) {
        //We update state and type in backhaul status CB only
        if (mIsDisconnectBackhaulPending &&
                mWwanCallStatusCb != NULL) {
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }
        mIsDisconnectBackhaulPending = false;

    } else if (teardownWwanIndData.conn_status == QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01 ||
                    teardownWwanIndData.conn_status ==
                        QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01) {

        if (mIsDisconnectBackhaulPending &&
                mWwanCallStatusCb != NULL){
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_CLOSE_FAILED, NULL,
                    LOC_NET_CONN_IP_TYPE_INVALID);
        }
        mIsDisconnectBackhaulPending = false;

    } else {
        LOC_LOGW("Unsupported wwan status %d",
                teardownWwanIndData.conn_status);
    }
}

#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
void LocNetIface::handleQcmapCallback(
        qcmap_msgr_wwan_roaming_status_ind_msg_v01 &roamingStatusIndData) {

    ENTRY_LOG();

    mIsRoaming = (roamingStatusIndData.wwan_roaming_status != 0);
    LOC_LOGD("Roaming status(OFF:0x00, ON:0x01-0x0C): %x, Roaming: %d",
                roamingStatusIndData.wwan_roaming_status, mIsRoaming);
}
#endif

void LocNetIface::notifyCurrentNetworkInfo(bool queryQcmap, LocNetConnType connType) {

    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance !");
        return;
    }

    /* Check saved state if queryQcmap disabled */
    if (!queryQcmap) {
        if (LOC_NET_CONN_TYPE_INVALID != mLocNetBackHaulType) {
            notifyObserverForNetworkInfo(
                    (LOC_NET_CONN_STATE_CONNECTED == mLocNetBackHaulState),
                    mLocNetBackHaulType);
        }
        else {
            LOC_LOGE("Invalid connection type:%d , State:%d",
                    mLocNetBackHaulType, mLocNetBackHaulState);
        }
        return;
    }

    /* Fetch connectivity status from qcmap and notify observers */
    /* Check if any network interface backhaul is connected */
    isAnyBackHaulConnected();
    if (LOC_NET_CONN_TYPE_WWAN_INTERNET == mLocNetBackHaulType) {
        /* Check the roaming status if backhaul type is WWAN */
        mIsRoaming = isWwanRoaming();
    }
    if (LOC_NET_CONN_TYPE_INVALID != mLocNetBackHaulType) {
        notifyObserverForNetworkInfo(
                (LOC_NET_CONN_STATE_CONNECTED == mLocNetBackHaulState),
                mLocNetBackHaulType);
    }
}

void LocNetIface::notifyCurrentWifiHardwareState(bool queryQcmap) {

    ENTRY_LOG();

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance !");
        return;
    }

    /* Check saved state if queryQcmap disabled */
    if (!queryQcmap) {
        notifyObserverForWlanStatus((LOC_NET_CONN_STATE_ENABLED == mLocNetWlanState));
        return;
    }

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        LocNetIface::sLocNetIfaceInstance->notifyCurrentWifiHardwareState(queryQcmap);
    }

    /* Fetch WLAN status */
    qcmap_msgr_wlan_mode_enum_v01 wlan_mode =
            QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;

    if (!mQcmapClientPtr->GetWLANStatus(&wlan_mode, &qmi_err_num)) {
        LOC_LOGE("Failed to fetch wlan status, err %d", qmi_err_num);
        return;
    }

    if (wlan_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01) {
        mLocNetWlanState = LOC_NET_CONN_STATE_DISABLED;
        notifyObserverForWlanStatus(false);
    } else if (wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_V01 ||
            wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
            wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_V01 ||
            wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01 ||
            wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_STA_BRIDGE_V01 ||
            wlan_mode == QCMAP_MSGR_WLAN_MODE_STA_ONLY_BRIDGE_V01) {
        mLocNetWlanState =LOC_NET_CONN_STATE_ENABLED;
        notifyObserverForWlanStatus(true);
    }
}

void LocNetIface::notifyObserverForWlanStatus(bool isWlanEnabled) {

    ENTRY_LOG();

    /* Validate subscription object */
    if (LocNetIfaceBase::sNotifyCb == NULL){
        LOC_LOGE("Notify callback NULL !");
        return;
    }

    /* Create a wifi hardware status item */
    WifiHardwareStateDataItem wifiStateDataItem;
    IDataItemCore *dataItem = NULL;

    wifiStateDataItem.mEnabled = isWlanEnabled;
    dataItem = &wifiStateDataItem;

    // Create a list and push data item, since that's what observer expects
    std::list<IDataItemCore *> dataItemList;
    dataItemList.push_back(dataItem);

    /* Notify back to client */
    LocNetIfaceBase::sNotifyCb(
            LocNetIfaceBase::sNotifyCbUserDataPtr, dataItemList);
}

void LocNetIface::notifyObserverForNetworkInfo(
        boolean isConnected, LocNetConnType connType){

    ENTRY_LOG();

    // Check if observer is registered
    if (LocNetIfaceBase::sNotifyCb == NULL) {
        LOC_LOGE("Notify callback NULL !");
        return;
    }

    // Create a network data item
    NetworkInfoDataItem networkInfoDataItem;
    IDataItemCore *dataItem = NULL;

    networkInfoDataItem.mType = (int32)connType;
    networkInfoDataItem.mAvailable = isConnected;
    networkInfoDataItem.mConnected = isConnected;
    networkInfoDataItem.mRoaming = mIsRoaming;

    dataItem = &networkInfoDataItem;

    // Create a list and push data item, since that's what observer expects
    std::list<IDataItemCore *> dataItemList;
    dataItemList.push_back(dataItem);

    /* Notify back to client */
    LocNetIfaceBase::sNotifyCb(
            LocNetIfaceBase::sNotifyCbUserDataPtr, dataItemList);
}

bool LocNetIface::setupWwanCall() {

    ENTRY_LOG();

    /* Validate call type requested */
    if (mLocNetConnType != LOC_NET_CONN_TYPE_WWAN_SUPL) {
        LOC_LOGE("Unsupported call type configured: %d", mLocNetConnType);
        return false;
    }

    /* Check for ongoing start/stop attempts */
    if (mIsDsiStartCallPending) {
        LOC_LOGW("Already start pending, returning as no-op");
        return true;
    }
    if (mIsDsiStopCallPending) {
        LOC_LOGE("Stop attempt pending, can't start now !");
        /* When stop completes and DS callback is received, we will
         * notify the client. So no need to notify now. */
        return false;
    }
    if (mIsDsiCallUp) {
        LOC_LOGW("Already ongoing data call");
        if (mWwanCallStatusCb != NULL) {
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }
        return true;
    }

    /* Initialize DSI library */
    int ret = -1;
    if (!mIsDsiInitDone) {

        if ((ret = dsi_init(DSI_MODE_GENERAL)) == DSI_SUCCESS) {
            LOC_LOGI("dsi_init success !");
        } else if (ret == DSI_EINITED) {
            LOC_LOGI("dsi_init already done !");
        } else {
            LOC_LOGE("dsi_init failed, err %d", ret);
        }
        mIsDsiInitDone = true;

        /* Sleep 100 ms for dsi_init() to complete */
        LOC_LOGV("Sleeping for 100 ms");
        usleep(100 * 1000);
    }

    /* Get DSI service handle */
    if (mDsiHandle == NULL) {
        mDsiHandle = dsi_get_data_srvc_hndl(
                LocNetIface::dsiNetEventCallback, this);
        if (mDsiHandle == NULL) {
            LOC_LOGE("NULL DSI Handle");
            return false;
        }
    }
    LOC_LOGD("DSI Handle for call %p", mDsiHandle);

    /* Set call parameters */
    dsi_call_param_value_t callParams;

    /* No Radio tech preference */
    callParams.buf_val = NULL;
    callParams.num_val = DSI_RADIO_TECH_UNKNOWN;
    LOC_LOGD("DSI_CALL_INFO_TECH_PREF = DSI_RADIO_TECH_UNKNOWN");
    dsi_set_data_call_param(mDsiHandle, DSI_CALL_INFO_TECH_PREF, &callParams);

    /* APN from gps.conf
      As this is read using loc cfg routine, the buffer size
      max is LOC_MAX_PARAM_STRING. */
    char* apnName = getApnNameFromConfig();
    int apnNameLen = strnlen(apnName, LOC_MAX_PARAM_STRING);
    if (apnName != NULL &&  apnNameLen > 0) {
        callParams.buf_val = apnName;
        callParams.num_val = apnNameLen;
        LOC_LOGD("DSI_CALL_INFO_APN_NAME = %s", apnName);
        dsi_set_data_call_param(mDsiHandle, DSI_CALL_INFO_APN_NAME, &callParams);
    } else{
        LOC_LOGE("Failed to fetch APN for data call setup");
        return false;
    }

    /* IP type from gps.conf */
    LocNetConnIpType ipType = getIpTypeFromConfig();
    callParams.buf_val = NULL;
    if (ipType == LOC_NET_CONN_IP_TYPE_V4) {
        callParams.num_val = DSI_IP_VERSION_4;
    } else if (ipType == LOC_NET_CONN_IP_TYPE_V6) {
        callParams.num_val = DSI_IP_VERSION_6;
    } else if (ipType == LOC_NET_CONN_IP_TYPE_V4V6) {
        callParams.num_val = DSI_IP_VERSION_4_6;
    } else {
        LOC_LOGE("No IP Type in gps.conf, using default v4");
        callParams.num_val = DSI_IP_VERSION_4;
    }
    dsi_set_data_call_param(
            mDsiHandle, DSI_CALL_INFO_IP_VERSION, &callParams);

    /* Send the call setup request */
    ret = dsi_start_data_call(mDsiHandle);
    if (ret != DSI_SUCCESS) {

        LOC_LOGE("DSI_START_DATA_CALL FAILED, err %d", ret);
        return false;
    }

    mIsDsiStartCallPending = true;
    LOC_LOGI("Data call START request sent successfully to DSI");
    return true;
}

bool LocNetIface::stopWwanCall() {

    ENTRY_LOG();

    /* Check for ongoing start/stop attempts */
    if (mIsDsiStopCallPending) {
        LOC_LOGW("Already stop pending, no-op");
        return true;
    }
    if (mIsDsiStartCallPending) {
        LOC_LOGE("Start attempt pending, can't stop now !");
        /* When start completes and DS callback is received, we will
         * notify the client. So no need to notify now. */
        return false;
    }
    if (!mIsDsiCallUp) {
        LOC_LOGE("No ongoing data call to stop");
        if (mWwanCallStatusCb != NULL) {
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }
        return true;
    }

    /* Stop the call */
    LOC_LOGD("Stopping data call with handle %p", mDsiHandle);

    int ret = dsi_stop_data_call(mDsiHandle);
    if (ret != DSI_SUCCESS) {

        LOC_LOGE("dsi_stop_data_call() returned err %d", ret);
        return false;
    }

    mIsDsiStopCallPending = true;
    LOC_LOGI("Data call STOP request sent to DS");
    return true;
}

/* Static callback method */
void LocNetIface::dsiNetEventCallback(
        dsi_hndl_t dsiHandle, void* userDataPtr, dsi_net_evt_t event,
        dsi_evt_payload_t* eventPayloadPtr){

    ENTRY_LOG();

    /* Analyze event payload */
    LocNetIface* locNetIface = static_cast<LocNetIface*>(userDataPtr);
    if (locNetIface == NULL){
        LOC_LOGE("Null user data !");
        return;
    }

    if (event == DSI_EVT_NET_IS_CONN){
        LOC_LOGI("DSI_EVT_NET_IS_CONN");
        locNetIface->handleDSCallback(dsiHandle, true);
    } else if (event == DSI_EVT_NET_NO_NET){
        LOC_LOGI("DSI_EVT_NET_NO_NET");
        locNetIface->handleDSCallback(dsiHandle, false);
    } else {
        LOC_LOGW("Unsupported event %d", event);
    }
}

void LocNetIface::handleDSCallback(
        dsi_hndl_t dsiHandle, bool isNetConnected){

    ENTRY_LOG();
    LOC_LOGV("dsiHandle %p, isCallUp %d, stopPending %d, startPending %d",
              dsiHandle, mIsDsiCallUp, mIsDsiStopCallPending,
              mIsDsiStartCallPending);

    /* Validate handle */
    if (mDsiHandle != dsiHandle){
        LOC_LOGE("DS Handle mismatch: %p vs %p", mDsiHandle, dsiHandle);
        return;
    }

    /* Process event */
    if (isNetConnected){

        /* Invoke client callback if registered*/
        if (mIsDsiStartCallPending &&
                mWwanCallStatusCb != NULL){
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }

        /* Start call complete */
        mIsDsiCallUp = true;
        mIsDsiStartCallPending = false;

    } else {

        /* Invoke client callback if registered */
        if (mIsDsiStopCallPending &&
                mWwanCallStatusCb != NULL) {
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        } else if (mIsDsiStartCallPending &&
                mWwanCallStatusCb != NULL){
            LOC_LOGV("LOC_NET_WWAN_CALL_EVT_OPEN_FAILED");
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_OPEN_FAILED, NULL,
                    LOC_NET_CONN_IP_TYPE_INVALID);
        }

        /* Stop call complete */
        mIsDsiCallUp = false;
        mIsDsiStopCallPending = false;
    }
}

bool LocNetIface::isNonMeteredBackHaulTypeConnected() {
    ENTRY_LOG();

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        return LocNetIface::sLocNetIfaceInstance->isNonMeteredBackHaulTypeConnected();
    }

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance !");
        return false;
    }

    /* Update backhaul status */
    isAnyBackHaulConnected();
    /* if Current backhaul - Is not WWAN && Is not an Invalid type*/
    return ((LOC_NET_CONN_TYPE_WWAN_INTERNET != mLocNetBackHaulType) &&
                (LOC_NET_CONN_TYPE_INVALID != mLocNetBackHaulType));
}

bool LocNetIface::isWwanRoaming() {
    ENTRY_LOG();

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        return LocNetIface::sLocNetIfaceInstance->isWwanRoaming();
    }

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance !");
        return false;
    }

    /* fetch roaming status */
    uint8_t roamStatus = 0;
#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;
    if (!mQcmapClientPtr->GetWWANRoamStatus(&roamStatus, &qmi_err_num)) {
        LOC_LOGE("Failed to fetch roaming status, err %d", qmi_err_num);
        return false;
    }
#endif
    // update internal roaming variable
    LOC_LOGD("Roaming status(OFF:0x00, ON:0x01-0x0C): %x", roamStatus);
    return (roamStatus != 0);
}

bool LocNetIface::isAnyBackHaulConnected() {

    ENTRY_LOG();

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        return LocNetIface::sLocNetIfaceInstance->isAnyBackHaulConnected();
    }

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance !");
        return false;
    }

#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
    /* Fetch backhaul status */
    qcmap_backhaul_status_info_type backhaulStatus =
            {false, false, QCMAP_MSGR_BACKHAUL_TYPE_ENUM_MIN_ENUM_VAL_V01};
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;

    if (!mQcmapClientPtr->GetBackhaulStatus(&backhaulStatus, &qmi_err_num)) {
        LOC_LOGE("Failed to fetch backhaul status, err %d", qmi_err_num);
        return false;
    }
    setCurrentBackHaulStatus(backhaulStatus.backhaul_type,
                backhaulStatus.backhaul_v4_available,
                backhaulStatus.backhaul_v6_available);
#endif
    return (LOC_NET_CONN_STATE_CONNECTED == mLocNetBackHaulState);
}

#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
void LocNetIface::setCurrentBackHaulStatus(
                qcmap_msgr_backhaul_type_enum_v01 backhaulType,
                boolean backhaulIPv4Available,
                boolean backhaulIPv6Available) {
    LOC_LOGI("Type:  1-WWAN, 2-USB Cradle, 3-WLAN , 4-Ethernet, 5-BT");
    LOC_LOGI("BackhaulStatus Type: %d, IPv4 avail:%d, IPv6 avail:%d",
                backhaulType, backhaulIPv4Available, backhaulIPv6Available);
    switch (backhaulType)
    {
      case QCMAP_MSGR_WWAN_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_WWAN_INTERNET;
        break;
      case QCMAP_MSGR_USB_CRADLE_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_USB_CRADLE;
        break;
      case QCMAP_MSGR_WLAN_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_WLAN;
        break;
      case QCMAP_MSGR_ETHERNET_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_ETHERNET;
        break;
      case QCMAP_MSGR_BT_BACKHAUL_V01:
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_BLUETOOTH;
        break;
      default:
        LOC_LOGE("Invalid backhaul type : %d", backhaulType);
        mLocNetBackHaulType = LOC_NET_CONN_TYPE_INVALID;
        break;
    }
    if (backhaulType != QCMAP_MSGR_WWAN_BACKHAUL_V01) {
        // set this to false for backhaul type other than wwan
        mIsRoaming = false;
    }
    if ((false == backhaulIPv4Available) && (false == backhaulIPv6Available)) {
        mLocNetBackHaulState = LOC_NET_CONN_STATE_DISCONNECTED;
    }
    else {
        mLocNetBackHaulState = LOC_NET_CONN_STATE_CONNECTED;
    }
}
#endif

/* isWwanConnected is used mainly from external clients (eg:XtraClient) */
bool LocNetIface::isWwanConnected() {

    ENTRY_LOG();

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        return LocNetIface::sLocNetIfaceInstance->isWwanConnected();
    }

    /* Validate QCMAP Client instance */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance !");
        return false;
    }

#ifdef FEATURE_ROAMING_BACKHAUL_STATUS_INDICATION
    /* Fetch backhaul status */
    qcmap_backhaul_status_info_type backhaulStatus =
            {false, false, QCMAP_MSGR_BACKHAUL_TYPE_ENUM_MIN_ENUM_VAL_V01};
    qmi_error_type_v01 qmi_err_num = QMI_ERROR_TYPE_MIN_ENUM_VAL_V01;

    if (!mQcmapClientPtr->GetBackhaulStatus(&backhaulStatus, &qmi_err_num)) {
        LOC_LOGE("Failed to fetch backhaul status, err %d", qmi_err_num);
        return false;
    }

    if ((QCMAP_MSGR_WWAN_BACKHAUL_V01 == backhaulStatus.backhaul_type) &&
            (backhaulStatus.backhaul_v4_available || backhaulStatus.backhaul_v6_available)) {
        // If WWAN is current backhaul type and either IPv4 or IPv6 connection available ?
        LOC_LOGV("WWAN is connected.");
        return true;
    } else {
        LOC_LOGV("WWAN is disconnected.");
        return false;
    }
#endif

    return false;
}

bool LocNetIface::connectBackhaul() {

    ENTRY_LOG();
    lock_guard<recursive_mutex> guard(mMutex);

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        LOC_LOGV("Invoke from static LocNetIface instance..");
        if (mWwanCallStatusCb != NULL) {
            LocNetIface::sLocNetIfaceInstance->
            registerWwanCallStatusCallback(
                    mWwanCallStatusCb, mWwanCbUserDataPtr);
        }
        return LocNetIface::sLocNetIfaceInstance->connectBackhaul();
    }

    /* QCMAP client instance must have been created.
     * Happens when some client subscribes. */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance");
        return false;
    }

    /* Check if backhaul is already connected */
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    qcmap_msgr_wwan_status_enum_v01 v4_status, v6_status;
    if (mQcmapClientPtr->GetWWANStatus(
            &v4_status, &v6_status, &qmi_err_num) == false) {
        LOC_LOGE("Failed to get wwan status, err 0x%x", qmi_err_num);
        return false;
    }
    if (v4_status == QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01 ||
        v6_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01) {
        LOC_LOGI("Ongoing connection attempt, ignoring connect.");
        return true;
    }
    if (v4_status == QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 ||
        v6_status == QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01) {
        LOC_LOGV("Backhaul already connected, ignoring connect.");
        if (mWwanCallStatusCb != NULL) {
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }
        /* Track each connection attempt,
         * by increasing connect request recvd counter before returning
         * success. */
        mConnectReqRecvCount++;
        return true;
    }

    /* Check if we've already sent the request */
    if (mIsConnectBackhaulPending || mIsConnectReqSent) {
        LOC_LOGI("Ignoring connect, connect pending %d, wwan state %d "
                "req sent %d", mIsConnectBackhaulPending, mLocNetBackHaulState,
                mIsConnectReqSent);
        mConnectReqRecvCount++;
        return true;
    }

#ifdef FEATURE_MOBILEAP_INDICATION
    qmi_err_num = QMI_ERR_NONE_V01;
     /* Need to enable MobileAP to invoke backhaul functions */
    bool ret = mQcmapClientPtr->EnableMobileAP(&qmi_err_num);
    if (false == ret ||  0 != qmi_err_num) {
        LOC_LOGE("Failed to enable mobileap, qcmapErr %d", qmi_err_num);
        return false;
    }
#endif

    /* Send connect request to QCMAP */
    qmi_err_num = QMI_ERR_NONE_V01;
    qcmap_msgr_wwan_call_type_v01 wwan_call_type = getWwanCallType();
    LOC_LOGV("Sending ConnectBackhaul request..");
    if (mQcmapClientPtr->ConnectBackHaul(
            wwan_call_type, &qmi_err_num) == false) {
        LOC_LOGE("Connect backhaul failed, err 0x%x", qmi_err_num);
#ifdef FEATURE_MOBILEAP_INDICATION
        qmi_err_num = QMI_ERR_NONE_V01;
        //Disabling mobile AP here if connect backhaul fails
        bool ret = mQcmapClientPtr->DisableMobileAP(&qmi_err_num);
        if (false == ret || 0 != qmi_err_num ) {
            LOC_LOGE("Failed to disable mobileap, qcmapErr %d", qmi_err_num);
        }
#endif
        return false;
    }

    /* Set the flag to track */
    mIsConnectReqSent = true;
    mIsConnectBackhaulPending = true;
    mConnectReqRecvCount++;
    return true;
}


qcmap_msgr_wwan_call_type_v01 LocNetIface::getWwanCallType() {
    return (getIpTypeFromConfig() == LOC_NET_CONN_IP_TYPE_V6) ?
        QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 :
        QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
}


bool LocNetIface::disconnectBackhaul() {

    ENTRY_LOG();
    lock_guard<recursive_mutex> guard(mMutex);

    /* Access QCMAP instance only from the static instance */
    if (this != LocNetIface::sLocNetIfaceInstance &&
            LocNetIface::sLocNetIfaceInstance != NULL) {
        LOC_LOGV("Invoke from static LocNetIface instance..");
        return LocNetIface::sLocNetIfaceInstance->disconnectBackhaul();
    }

    /* QCMAP client instance must have been created.
     * Happens when some client subscribes. */
    if (mQcmapClientPtr == NULL) {
        LOC_LOGE("No QCMAP instance");
        return false;
    }

    /* Track connect requests recvd to multiplexing */
    if (mConnectReqRecvCount <= 0) {
        LOC_LOGE("Invalid connect req recv count %d", mConnectReqRecvCount);
        return false;
    }
    mConnectReqRecvCount--;

    /* Check if we've sent the request.
     * If we didn't send the connect request, no need to disconnect.
     * If we still have surplus connect request count, don't disconnect */
    if (!mIsConnectReqSent || mConnectReqRecvCount > 0) {
        LOC_LOGI("Conn req sent %d, conn req rcvd cnt %d, "
                "ignoring disconnect", mIsConnectReqSent,
                mConnectReqRecvCount);
        if (mWwanCallStatusCb != NULL) {
            mWwanCallStatusCb(
                    mWwanCbUserDataPtr, LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS,
                    getApnNameFromConfig(), getIpTypeFromConfig());
        }
        return true;
    }

    /* Send disconnect request to QCMAP */
    qmi_error_type_v01 qmi_err_num = QMI_ERR_NONE_V01;
    qcmap_msgr_wwan_call_type_v01 wwan_call_type = getWwanCallType();
    LOC_LOGV("Sending DisconnectBackhaul..");
    if (mQcmapClientPtr->DisconnectBackHaul(
            wwan_call_type, &qmi_err_num) == false) {
        LOC_LOGE("Disconnect backhaul failed, err 0x%x", qmi_err_num);
#ifdef FEATURE_MOBILEAP_INDICATION
        // Even if DisconnectBackHaul fails, do not return, we need to
        // DisableMobileAP in any case.
    }
    qmi_err_num = QMI_ERR_NONE_V01;
    bool ret = mQcmapClientPtr->DisableMobileAP(&qmi_err_num);
    if ( false == ret || 0 != qmi_err_num) {
        LOC_LOGE("Failed to disable mobileap, qcmapErr %d", qmi_err_num);
#endif
        return false;
    }

    /* Set the flag to track */
    mIsConnectReqSent = false;
    mIsDisconnectBackhaulPending = true;
    return true;
}
