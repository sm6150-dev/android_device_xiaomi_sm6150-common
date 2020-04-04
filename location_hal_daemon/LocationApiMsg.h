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

#ifndef LOCATIONAPIMSG_H
#define LOCATIONAPIMSG_H

#include <string>
#include <memory>
#include <algorithm>
#include <loc_pla.h> // for strlcpy
#include <gps_extended_c.h>
#include <log_util.h>
#include <LocIpc.h>
#include <LocationDataTypes.h>

/******************************************************************************
Constants
******************************************************************************/
#define LOCATION_REMOTE_API_MSG_VERSION (1)

// Maximum fully qualified path(including the file name)
// for the location remote API service and client socket name
#define MAX_SOCKET_PATHNAME_LENGTH (128)

#define LOCATION_CLIENT_SESSION_ID_INVALID (0)

#define LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID    (5001)
#define LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID   (1)
#define LOCATION_CLIENT_API_QSOCKET_CLIENT_SERVICE_ID       (5002)

#define s_CLIENTAPI_LOCAL SOCKET_LOC_CLIENT_DIR  LOC_CLIENT_NAME_PREFIX
#define s_INTAPI_LOCAL    SOCKET_LOC_CLIENT_DIR  LOC_INTAPI_NAME_PREFIX
#define sEAP EAP_LOC_CLIENT_DIR LOC_CLIENT_NAME_PREFIX

using namespace std;
using namespace loc_util;

class SockNode {
    const int32_t mId1;
    const int32_t mId2;
    const string mNodePathnamePrefix;

public:
    enum Type { LOCAL, EAP, OTHER };
    SockNode(int32_t id1, int32_t mId2, const string&& prefix) :
            mId1(id1), mId2(mId2), mNodePathnamePrefix(prefix) {
    }
    SockNode(SockNode&& node) :
            SockNode(node.mId1, node.mId2, move(node.mNodePathnamePrefix)) {
    }
    static SockNode create(const string fullPathName) {
        return create(fullPathName.c_str(), fullPathName.size());
    }
    static SockNode create(const char* fullPathName, int32_t length = -1) {
        uint32_t count = 0;
        int32_t indx = 0, id1 = -1, id2 = -1;

        if (nullptr == fullPathName) {
            fullPathName = "";
        }
        indx = (length < 0) ? (strlen(fullPathName) - 1) : length;

        while (count < 2 && indx >= 0) {
            if ('.' == fullPathName[indx]) {
                count++;
            }
            indx--;
        }
        if (count == 2) {
            indx++;
            sscanf(fullPathName+indx+1, "%d.%d", &id1, &id2);
        } else {
            indx = 0;
        }

        return SockNode(id1, id2, string(fullPathName, indx));
    }
    inline int getId1() const { return mId1; }
    inline int getId2() const { return mId2; }
    inline const string& getNodePathnamePrefix() const { return mNodePathnamePrefix; }
    inline string getNodePathname() const {
        return string(mNodePathnamePrefix).append(1, '.').append(to_string(mId1))
            .append(1, '.').append(to_string(mId2));
    }
    inline Type getNodeType() const {
        Type type = OTHER;
        if (mNodePathnamePrefix.compare(0, sizeof(SOCKET_LOC_CLIENT_DIR)-1,
                                        SOCKET_LOC_CLIENT_DIR) == 0) {
            type = LOCAL;
        } else if (mNodePathnamePrefix.compare(0, sizeof(sEAP)-1, sEAP) == 0) {
            type = EAP;
        }
        return type;
    }
    inline shared_ptr<LocIpcSender> createSender(bool createFsNode = false) {
        const string socket = getNodePathname();
        const char* sock = socket.c_str();
        switch (getNodeType()) {
        case SockNode::LOCAL:
            return LocIpc::getLocIpcLocalSender(sock);
        case SockNode::EAP:
            if (createFsNode) {
                if (nullptr == fopen(sock, "w")) {
                    LOC_LOGe("<-- failed to open file %s", sock);
                }
            }
            return LocIpc::getLocIpcQrtrSender(getId1(), getId2());
        default:
            return nullptr;
        }
    }
};

enum ClientType {
    LOCATION_CLIENT_API = 1,
    LOCATION_INTEGRATION_API = 2,
};

class SockNodeLocal : public SockNode {
public:
    SockNodeLocal(ClientType type, int32_t pid, int32_t tid) :
        SockNode(pid, tid, (LOCATION_CLIENT_API == type) ? s_CLIENTAPI_LOCAL : s_INTAPI_LOCAL) {}
};

class SockNodeEap : public SockNode {
public:
    SockNodeEap(int32_t service, int32_t instance) :
        SockNode(service, instance, sEAP) {}
};

/******************************************************************************
List of message IDs supported by Location Remote API
******************************************************************************/
enum ELocMsgID {
    E_LOCAPI_UNDEFINED_MSG_ID = 0,

    // registration
    E_LOCAPI_CLIENT_REGISTER_MSG_ID = 1,
    E_LOCAPI_CLIENT_DEREGISTER_MSG_ID = 2,
    E_LOCAPI_CAPABILILTIES_MSG_ID = 3,
    E_LOCAPI_HAL_READY_MSG_ID = 4,

    // tracking session
    E_LOCAPI_START_TRACKING_MSG_ID = 5,
    E_LOCAPI_STOP_TRACKING_MSG_ID = 6,
    E_LOCAPI_UPDATE_CALLBACKS_MSG_ID = 7,
    E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID = 8,

    // control
    E_LOCAPI_CONTROL_UPDATE_CONFIG_MSG_ID = 9,
    E_LOCAPI_CONTROL_DELETE_AIDING_DATA_MSG_ID = 10,
    E_LOCAPI_CONTROL_UPDATE_NETWORK_AVAILABILITY_MSG_ID = 11,

    // Position reports
    E_LOCAPI_LOCATION_MSG_ID = 12,
    E_LOCAPI_LOCATION_INFO_MSG_ID = 13,
    E_LOCAPI_SATELLITE_VEHICLE_MSG_ID = 14,
    E_LOCAPI_NMEA_MSG_ID = 15,
    E_LOCAPI_DATA_MSG_ID = 16,

    // Get API to retrieve info from GNSS engine
    E_LOCAPI_GET_GNSS_ENGERY_CONSUMED_MSG_ID = 17,

    E_LOCAPI_LOCATION_SYSTEM_INFO_MSG_ID = 18,

    // engine position report
    E_LOCAPI_ENGINE_LOCATIONS_INFO_MSG_ID = 19,

    // batching session
    E_LOCAPI_START_BATCHING_MSG_ID = 20,
    E_LOCAPI_STOP_BATCHING_MSG_ID = 21,
    E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID = 22,

    //batching reports
    E_LOCAPI_BATCHING_MSG_ID = 23,

    // geofence session
    E_LOCAPI_ADD_GEOFENCES_MSG_ID = 24,
    E_LOCAPI_REMOVE_GEOFENCES_MSG_ID = 25,
    E_LOCAPI_MODIFY_GEOFENCES_MSG_ID = 26,
    E_LOCAPI_PAUSE_GEOFENCES_MSG_ID = 27,
    E_LOCAPI_RESUME_GEOFENCES_MSG_ID = 28,

    //geofence breach
    E_LOCAPI_GEOFENCE_BREACH_MSG_ID = 29,

    // Measurement reports
    E_LOCAPI_MEAS_MSG_ID = 30,

    // ping
    E_LOCAPI_PINGTEST_MSG_ID = 99,

    // integration API config request
    E_INTAPI_CONFIG_CONSTRAINTED_TUNC_MSG_ID = 200,
    E_INTAPI_CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR_MSG_ID = 201,
    E_INTAPI_CONFIG_SV_CONSTELLATION_MSG_ID  = 202,
    E_INTAPI_CONFIG_AIDING_DATA_DELETION_MSG_ID  = 203,
    E_INTAPI_CONFIG_LEVER_ARM_MSG_ID  = 204,
    E_INTAPI_CONFIG_ROBUST_LOCATION_MSG_ID  = 205,

    // integration API config retrieval request/response
    E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID  = 300,
    E_INTAPI_GET_ROBUST_LOCATION_CONFIG_RESP_MSG_ID  = 301,
};

typedef uint32_t LocationCallbacksMask;
enum ELocationCallbacksOption {
    E_LOC_CB_DISTANCE_BASED_TRACKING_BIT= (1<<0), /**< Register for DBT location report */
    E_LOC_CB_GNSS_LOCATION_INFO_BIT     = (1<<1), /**< Register for GNSS Location */
    E_LOC_CB_GNSS_SV_BIT                = (1<<2), /**< Register for GNSS SV */
    E_LOC_CB_GNSS_NMEA_BIT              = (1<<3), /**< Register for GNSS NMEA */
    E_LOC_CB_GNSS_DATA_BIT              = (1<<4), /**< Register for GNSS DATA */
    E_LOC_CB_SYSTEM_INFO_BIT            = (1<<5),  /**< Register for Location system info */
    E_LOC_CB_BATCHING_BIT               = (1<<6), /**< Register for Batching */
    E_LOC_CB_BATCHING_STATUS_BIT        = (1<<7), /**< Register for Batching  Status*/
    E_LOC_CB_GEOFENCE_BREACH_BIT        = (1<<8), /**< Register for Geofence Breach */
    E_LOC_CB_ENGINE_LOCATIONS_INFO_BIT  = (1<<9), /**< Register for multiple engine reports */
    E_LOC_CB_SIMPLE_LOCATION_INFO_BIT   = (1<<10), /**< Register for simple location */
    E_LOC_CB_GNSS_MEAS_BIT              = (1<<11), /**< Register for GNSS Measurements */
};

// Mask related to all info that are tied with a position session and need to be unsubscribed
// when session is stopped
#define LOCATION_SESSON_ALL_INFO_MASK (E_LOC_CB_DISTANCE_BASED_TRACKING_BIT|\
                                       E_LOC_CB_GNSS_LOCATION_INFO_BIT|\
                                       E_LOC_CB_GNSS_SV_BIT|E_LOC_CB_GNSS_NMEA_BIT|\
                                       E_LOC_CB_GNSS_DATA_BIT|E_LOC_CB_GNSS_MEAS_BIT|\
                                       E_LOC_CB_ENGINE_LOCATIONS_INFO_BIT|\
                                       E_LOC_CB_SIMPLE_LOCATION_INFO_BIT)

typedef uint32_t EngineInfoCallbacksMask;
enum EEngineInfoCallbacksMask {
    // gnss energy consumed, once the info is delivered,
    // this bit will be cleared
    E_ENGINE_INFO_CB_GNSS_ENERGY_CONSUMED_BIT = (1<<0) /**< GNSS energy consumed */
};

/******************************************************************************
Common data structure
******************************************************************************/
struct LocAPINmeaSerializedPayload {
    // do not use size_t as data type for size_t is architecture dependent
    uint32_t size;
    uint64_t timestamp;
    // do not use size_t as data type for size_t is architecture dependent
    uint32_t length;
    char nmea[1];
};

struct LocAPIBatchNotification {
    // do not use size_t as data type for size_t is architecture dependent
    uint32_t size;
    uint32_t count;
    BatchingStatus status;
    Location location[1];
};

struct LocAPIGeofenceBreachNotification {
    // do not use size_t as data type for size_t is architecture dependent
    uint32_t size;
    uint32_t count;
    uint64_t timestamp;
    GeofenceBreachTypeMask type; //type of breach
    Location location;   //location associated with breach
    uint32_t id[1];
};

struct GeofencePayload {
    uint32_t gfClientId;
    GeofenceOption gfOption;
    GeofenceInfo gfInfo;
};

struct GeofencesAddedReqPayload {
    uint32_t count;
    GeofencePayload gfPayload[1];
};

struct GeofencesReqClientIdPayload {
    uint32_t count;
    uint32_t gfIds[1];
};

struct GeofenceResponse {
    uint32_t clientId;
    LocationError error;
};

struct CollectiveResPayload {
    // do not use size_t as data type for size_t is architecture dependent
    uint32_t size;
    uint32_t count;
    GeofenceResponse resp[1];
};
/******************************************************************************
IPC message header structure
******************************************************************************/
struct LocAPIMsgHeader
{
    char       mSocketName[MAX_SOCKET_PATHNAME_LENGTH]; /**< Processor string */
    ELocMsgID  msgId;               /**< LocationMsgID */
    uint32_t   msgVersion;          /**< Location remote API message version */

    inline LocAPIMsgHeader(const char* name, ELocMsgID msgId):
        msgId(msgId),
        msgVersion(LOCATION_REMOTE_API_MSG_VERSION) {
            memset(mSocketName, 0, MAX_SOCKET_PATHNAME_LENGTH);
            strlcpy(mSocketName, name, MAX_SOCKET_PATHNAME_LENGTH);
        }
};

/******************************************************************************
IPC message structure - client registration
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_CLIENT_REGISTER_MSG_ID
struct LocAPIClientRegisterReqMsg: LocAPIMsgHeader
{
    ClientType mClientType;

    inline LocAPIClientRegisterReqMsg(const char* name, ClientType clientType) :
        LocAPIMsgHeader(name, E_LOCAPI_CLIENT_REGISTER_MSG_ID),
        mClientType(clientType) { }
};

// defintion for message with msg id of E_LOCAPI_CLIENT_DEREGISTER_MSG_ID
struct LocAPIClientDeregisterReqMsg: LocAPIMsgHeader
{
    inline LocAPIClientDeregisterReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_CLIENT_DEREGISTER_MSG_ID) { }
};

// defintion for message with msg id of E_LOCAPI_CAPABILILTIES_MSG_ID
struct LocAPICapabilitiesIndMsg: LocAPIMsgHeader
{
    LocationCapabilitiesMask capabilitiesMask;

    inline LocAPICapabilitiesIndMsg(const char* name,
        LocationCapabilitiesMask capabilitiesMask) :
        LocAPIMsgHeader(name, E_LOCAPI_CAPABILILTIES_MSG_ID),
        capabilitiesMask(capabilitiesMask) { }
};

// defintion for message with msg id of E_LOCAPI_HAL_READY_MSG_ID
struct LocAPIHalReadyIndMsg: LocAPIMsgHeader
{
    inline LocAPIHalReadyIndMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_HAL_READY_MSG_ID) { }
};

/******************************************************************************
IPC message structure - generic response
******************************************************************************/
struct LocAPIGenericRespMsg: LocAPIMsgHeader
{
    LocationError err;

    inline LocAPIGenericRespMsg(const char* name, ELocMsgID msgId, LocationError err) :
        LocAPIMsgHeader(name, msgId),
        err(err) { }
};
struct LocAPICollectiveRespMsg: LocAPIMsgHeader
{
    CollectiveResPayload collectiveRes;

    inline LocAPICollectiveRespMsg(const char* name, ELocMsgID msgId,
            CollectiveResPayload& response) :
        LocAPIMsgHeader(name, msgId),
        collectiveRes(response) { }
};


/******************************************************************************
IPC message structure - tracking
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_START_TRACKING_MSG_ID
struct LocAPIStartTrackingReqMsg: LocAPIMsgHeader
{
    LocationOptions locOptions;

    inline LocAPIStartTrackingReqMsg(const char* name,
                                     const LocationOptions & locSessionOptions):
        LocAPIMsgHeader(name, E_LOCAPI_START_TRACKING_MSG_ID),
        locOptions(locSessionOptions) { }
};

// defintion for message with msg id of E_LOCAPI_STOP_TRACKING_MSG_ID
struct LocAPIStopTrackingReqMsg: LocAPIMsgHeader
{
    inline LocAPIStopTrackingReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_STOP_TRACKING_MSG_ID) { }
};

// defintion for message with msg id of E_LOCAPI_UPDATE_CALLBACKS_MSG_ID
struct LocAPIUpdateCallbacksReqMsg: LocAPIMsgHeader
{
    LocationCallbacksMask    locationCallbacks;

    inline LocAPIUpdateCallbacksReqMsg(const char* name,
                                       LocationCallbacksMask callBacksMask):
        LocAPIMsgHeader(name, E_LOCAPI_UPDATE_CALLBACKS_MSG_ID),
        locationCallbacks(callBacksMask) { }
};

// defintion for message with msg id of E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID
struct LocAPIUpdateTrackingOptionsReqMsg: LocAPIMsgHeader
{
    LocationOptions locOptions;

    inline LocAPIUpdateTrackingOptionsReqMsg(const char* name,
                                             const LocationOptions & locSessionOptions):
        LocAPIMsgHeader(name, E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID),
        locOptions(locSessionOptions) { }
};

/******************************************************************************
IPC message structure - batching
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_START_BATCHING_MSG_ID
struct LocAPIStartBatchingReqMsg: LocAPIMsgHeader
{
    uint32_t intervalInMs;
    uint32_t distanceInMeters;
    BatchingMode batchingMode;

    inline LocAPIStartBatchingReqMsg(const char* name,
                                     uint32_t minInterval,
                                     uint32_t minDistance,
                                     BatchingMode batchMode):
        LocAPIMsgHeader(name, E_LOCAPI_START_BATCHING_MSG_ID),
        intervalInMs(minInterval),
        distanceInMeters(minDistance),
        batchingMode(batchMode) { }
};

// defintion for message with msg id of E_LOCAPI_STOP_BATCHING_MSG_ID
struct LocAPIStopBatchingReqMsg: LocAPIMsgHeader
{
    inline LocAPIStopBatchingReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_STOP_BATCHING_MSG_ID) { }
};

// defintion for message with msg id of E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID
struct LocAPIUpdateBatchingOptionsReqMsg: LocAPIMsgHeader
{
    uint32_t intervalInMs;
    uint32_t distanceInMeters;
    BatchingMode batchingMode;

    inline LocAPIUpdateBatchingOptionsReqMsg(const char* name,
                                             uint32_t sessionInterval,
                                             uint32_t sessionDistance,
                                             BatchingMode batchMode):
        LocAPIMsgHeader(name, E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID),
        intervalInMs(sessionInterval),
        distanceInMeters(sessionDistance),
        batchingMode(batchMode) { }
};

/******************************************************************************
IPC message structure - geofence
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_ADD_GEOFENCES_MSG_ID
struct LocAPIAddGeofencesReqMsg: LocAPIMsgHeader
{
    GeofencesAddedReqPayload geofences;
    inline LocAPIAddGeofencesReqMsg(const char* name, GeofencesAddedReqPayload& geofencesAdded):
        LocAPIMsgHeader(name, E_LOCAPI_ADD_GEOFENCES_MSG_ID),
        geofences(geofencesAdded) { }
};
// defintion for message with msg id of E_LOCAPI_REMOVE_GEOFENCES_MSG_ID
struct LocAPIRemoveGeofencesReqMsg: LocAPIMsgHeader
{
    GeofencesReqClientIdPayload gfClientIds;
    inline LocAPIRemoveGeofencesReqMsg(const char* name, GeofencesReqClientIdPayload& ids):
        LocAPIMsgHeader(name, E_LOCAPI_REMOVE_GEOFENCES_MSG_ID), gfClientIds(ids) { }
};
// defintion for message with msg id of E_LOCAPI_MODIFY_GEOFENCES_MSG_ID
struct LocAPIModifyGeofencesReqMsg: LocAPIMsgHeader
{
    GeofencesAddedReqPayload geofences;

    inline LocAPIModifyGeofencesReqMsg(const char* name,
                                       GeofencesAddedReqPayload& geofencesModified):
        LocAPIMsgHeader(name, E_LOCAPI_MODIFY_GEOFENCES_MSG_ID),
        geofences(geofencesModified) { }
};
// defintion for message with msg id of E_LOCAPI_PAUSE_GEOFENCES_MSG_ID
struct LocAPIPauseGeofencesReqMsg: LocAPIMsgHeader
{
    GeofencesReqClientIdPayload gfClientIds;
    inline LocAPIPauseGeofencesReqMsg(const char* name,
                                      GeofencesReqClientIdPayload& ids):
        LocAPIMsgHeader(name, E_LOCAPI_PAUSE_GEOFENCES_MSG_ID),
        gfClientIds(ids) { }
};
// defintion for message with msg id of E_LOCAPI_RESUME_GEOFENCES_MSG_ID
struct LocAPIResumeGeofencesReqMsg: LocAPIMsgHeader
{
    GeofencesReqClientIdPayload gfClientIds;
    inline LocAPIResumeGeofencesReqMsg(const char* name,
                                      GeofencesReqClientIdPayload& ids):
        LocAPIMsgHeader(name, E_LOCAPI_RESUME_GEOFENCES_MSG_ID),
        gfClientIds(ids) { }
};

/******************************************************************************
IPC message structure - control
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_CONTROL_UPDATE_CONFIG_MSG_ID
struct LocAPIUpdateConfigReqMsg: LocAPIMsgHeader
{
    GnssConfig gnssConfig;

    inline LocAPIUpdateConfigReqMsg(const char* name, GnssConfig& config) :
        LocAPIMsgHeader(name, E_LOCAPI_CONTROL_UPDATE_CONFIG_MSG_ID),
        gnssConfig(config) { }
};

// defintion for message with msg id of E_LOCAPI_CONTROL_DELETE_AIDING_DATA_MSG_ID
struct LocAPIDeleteAidingDataReqMsg: LocAPIMsgHeader
{
    GnssAidingData gnssAidingData;

    inline LocAPIDeleteAidingDataReqMsg(const char* name, GnssAidingData& data) :
        LocAPIMsgHeader(name, E_LOCAPI_CONTROL_DELETE_AIDING_DATA_MSG_ID),
        gnssAidingData(data) { }
};

struct LocAPIUpdateNetworkAvailabilityReqMsg: LocAPIMsgHeader
{
    bool mAvailability;

    inline LocAPIUpdateNetworkAvailabilityReqMsg(const char* name, bool availability) :
        LocAPIMsgHeader(name, E_LOCAPI_CONTROL_UPDATE_NETWORK_AVAILABILITY_MSG_ID),
        mAvailability(availability) { }
};

struct LocAPIGetGnssEnergyConsumedReqMsg: LocAPIMsgHeader
{
    inline LocAPIGetGnssEnergyConsumedReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_GET_GNSS_ENGERY_CONSUMED_MSG_ID) { }
};

/******************************************************************************
IPC message structure - indications
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_LOCATION_MSG_ID
struct LocAPILocationIndMsg: LocAPIMsgHeader
{
    Location locationNotification;

    inline LocAPILocationIndMsg(const char* name,
        Location& location) :
        LocAPIMsgHeader(name, E_LOCAPI_LOCATION_MSG_ID),
        locationNotification(location) { }
};

// defintion for message with msg id of E_LOCAPI_BATCHING_MSG_ID
struct LocAPIBatchingIndMsg: LocAPIMsgHeader
{
    LocAPIBatchNotification batchNotification;

    inline LocAPIBatchingIndMsg(const char* name, LocAPIBatchNotification& batchNotif) :
        LocAPIMsgHeader(name, E_LOCAPI_BATCHING_MSG_ID),
        batchNotification(batchNotif) { }
};

// defintion for message with msg id of E_LOCAPI_GEOFENCE_BREACH_MSG_ID
struct LocAPIGeofenceBreachIndMsg: LocAPIMsgHeader
{
    LocAPIGeofenceBreachNotification gfBreachNotification;

    inline LocAPIGeofenceBreachIndMsg(const char* name,
            LocAPIGeofenceBreachNotification& gfBreachNotif) :
        LocAPIMsgHeader(name, E_LOCAPI_GEOFENCE_BREACH_MSG_ID),
        gfBreachNotification(gfBreachNotif) { }
};

// defintion for message with msg id of E_LOCAPI_LOCATION_INFO_MSG_ID
struct LocAPILocationInfoIndMsg: LocAPIMsgHeader
{
    GnssLocationInfoNotification gnssLocationInfoNotification;

    inline LocAPILocationInfoIndMsg(const char* name,
        GnssLocationInfoNotification& locationInfo) :
        LocAPIMsgHeader(name, E_LOCAPI_LOCATION_INFO_MSG_ID),
        gnssLocationInfoNotification(locationInfo) { }
};

// defintion for message with msg id of E_LOCAPI_ENGINE_LOCATIONS_INFO_MSG_ID
struct LocAPIEngineLocationsInfoIndMsg: LocAPIMsgHeader
{
    uint32_t count;
    GnssLocationInfoNotification engineLocationsInfo[LOC_OUTPUT_ENGINE_COUNT];

    inline LocAPIEngineLocationsInfoIndMsg(
            const char* name,
            int cnt,
            GnssLocationInfoNotification* locationInfo) :
            LocAPIMsgHeader(name, E_LOCAPI_ENGINE_LOCATIONS_INFO_MSG_ID),
            count(cnt) {

        if (count > LOC_OUTPUT_ENGINE_COUNT) {
            count = LOC_OUTPUT_ENGINE_COUNT;
        }
        if (count > 0) {
            memcpy(engineLocationsInfo, locationInfo,
                   sizeof(GnssLocationInfoNotification) * count);
        }
    }

    inline uint32_t getMsgSize() const {
        return (sizeof(LocAPIEngineLocationsInfoIndMsg) -
                (LOC_OUTPUT_ENGINE_COUNT - count) * sizeof(GnssLocationInfoNotification));
    }
};

// defintion for message with msg id of E_LOCAPI_SATELLITE_VEHICLE_MSG_ID
struct LocAPISatelliteVehicleIndMsg: LocAPIMsgHeader
{
    GnssSvNotification gnssSvNotification;

    inline LocAPISatelliteVehicleIndMsg(const char* name,
        GnssSvNotification& svNotification) :
        LocAPIMsgHeader(name, E_LOCAPI_SATELLITE_VEHICLE_MSG_ID),
        gnssSvNotification(svNotification) { }
};

// defintion for message with msg id of E_LOCAPI_NMEA_MSG_ID
struct LocAPINmeaIndMsg: LocAPIMsgHeader
{
    LocAPINmeaSerializedPayload gnssNmeaNotification;

    inline LocAPINmeaIndMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_NMEA_MSG_ID) { }
};

// defintion for message with msg id of E_LOCAPI_DATA_MSG_ID
struct LocAPIDataIndMsg : LocAPIMsgHeader
{
    GnssDataNotification gnssDataNotification;

    inline LocAPIDataIndMsg(const char* name,
        GnssDataNotification& dataNotification) :
        LocAPIMsgHeader(name, E_LOCAPI_DATA_MSG_ID),
        gnssDataNotification(dataNotification) { }
};

// defintion for message with msg id of E_LOCAPI_MEAS_MSG_ID
struct LocAPIMeasIndMsg : LocAPIMsgHeader
{
    GnssMeasurementsNotification gnssMeasurementsNotification;

    inline LocAPIMeasIndMsg(const char* name,
        GnssMeasurementsNotification& measurementsNotification) :
        LocAPIMsgHeader(name, E_LOCAPI_MEAS_MSG_ID),
        gnssMeasurementsNotification(measurementsNotification) { }
};

// defintion for message with msg id of E_LOCAPI_GET_TOTAL_ENGERY_CONSUMED_BY_GPS_ENGINE_MSG_ID
struct LocAPIGnssEnergyConsumedIndMsg: LocAPIMsgHeader
{
    uint64_t totalGnssEnergyConsumedSinceFirstBoot;

    inline LocAPIGnssEnergyConsumedIndMsg(const char* name, uint64_t energyConsumed) :
        LocAPIMsgHeader(name, E_LOCAPI_GET_GNSS_ENGERY_CONSUMED_MSG_ID),
        totalGnssEnergyConsumedSinceFirstBoot(energyConsumed) { }
};

// defintion for message with msg id of E_LOCAPI_LOCATION_SYSTEM_INFO_MSG_ID
struct LocAPILocationSystemInfoIndMsg: LocAPIMsgHeader
{
    LocationSystemInfo locationSystemInfo;

    inline LocAPILocationSystemInfoIndMsg(const char* name, const LocationSystemInfo & systemInfo) :
        LocAPIMsgHeader(name, E_LOCAPI_LOCATION_SYSTEM_INFO_MSG_ID),
        locationSystemInfo(systemInfo) { }
};

/******************************************************************************
IPC message structure - Location Integration API Configure Request
******************************************************************************/
struct LocConfigConstrainedTuncReqMsg: LocAPIMsgHeader
{
    bool     mEnable;
    float    mTuncConstraint;
    uint32_t mEnergyBudget;

    inline LocConfigConstrainedTuncReqMsg(const char* name,
                                          bool enable,
                                          float tuncConstraint,
                                          uint32_t energyBudget) :
            LocAPIMsgHeader(name, E_INTAPI_CONFIG_CONSTRAINTED_TUNC_MSG_ID),
            mEnable(enable),
            mTuncConstraint(tuncConstraint),
            mEnergyBudget(energyBudget) { }
};

struct LocConfigPositionAssistedClockEstimatorReqMsg: LocAPIMsgHeader
{
    bool     mEnable;
    inline LocConfigPositionAssistedClockEstimatorReqMsg(const char* name,
                                                         bool enable) :
            LocAPIMsgHeader(name,
                            E_INTAPI_CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR_MSG_ID),
            mEnable(enable) { }
};

struct LocConfigSvConstellationReqMsg: LocAPIMsgHeader
{
    bool mResetToDefault;
    GnssSvTypeConfig mSvTypeConfig;
    GnssSvIdConfig   mSvIdConfig;

    inline LocConfigSvConstellationReqMsg(const char* name,
                                          bool resetToDefault,
                                          const GnssSvTypeConfig& svTypeConfig,
                                          const GnssSvIdConfig& svIdConfig) :
            LocAPIMsgHeader(name, E_INTAPI_CONFIG_SV_CONSTELLATION_MSG_ID),
            mResetToDefault(resetToDefault),
            mSvTypeConfig(svTypeConfig),
            mSvIdConfig(svIdConfig){ }
};

// defintion for message with msg id of E_LOCAPI_CONTROL_DELETE_AIDING_DATA_MSG_ID
struct LocConfigAidingDataDeletionReqMsg: LocAPIMsgHeader
{
    GnssAidingData mAidingData;

    inline LocConfigAidingDataDeletionReqMsg(const char* name, GnssAidingData& aidingData) :
        LocAPIMsgHeader(name, E_INTAPI_CONFIG_AIDING_DATA_DELETION_MSG_ID),
        mAidingData(aidingData) { }
};

struct LocConfigLeverArmReqMsg: LocAPIMsgHeader
{
    LeverArmConfigInfo mLeverArmConfigInfo;

    inline LocConfigLeverArmReqMsg(const char* name,
                                   const LeverArmConfigInfo & configInfo) :
        LocAPIMsgHeader(name, E_INTAPI_CONFIG_LEVER_ARM_MSG_ID),
        mLeverArmConfigInfo(configInfo) { }
};

struct LocConfigRobustLocationReqMsg: LocAPIMsgHeader
{
    bool mEnable;
    bool mEnableForE911;

    inline LocConfigRobustLocationReqMsg(const char* name,
                                         bool enable,
                                         bool enableForE911) :
        LocAPIMsgHeader(name, E_INTAPI_CONFIG_ROBUST_LOCATION_MSG_ID),
        mEnable(enable),
        mEnableForE911(enableForE911) { }
};

/******************************************************************************
IPC message structure - Location Integration API Get request/response message
******************************************************************************/
struct LocConfigGetRobustLocationConfigReqMsg: LocAPIMsgHeader
{
    inline LocConfigGetRobustLocationConfigReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID) { }
};

struct LocConfigGetRobustLocationConfigRespMsg: LocAPIMsgHeader
{
    GnssConfigRobustLocation mRobustLoationConfig;

    inline LocConfigGetRobustLocationConfigRespMsg(
            const char* name,
            GnssConfigRobustLocation robustLoationConfig) :
        LocAPIMsgHeader(name, E_INTAPI_GET_ROBUST_LOCATION_CONFIG_RESP_MSG_ID),
        mRobustLoationConfig(robustLoationConfig) { }
};

/******************************************************************************
IPC message structure - ping
******************************************************************************/
#define LOCATION_REMOTE_API_PINGTEST_SIZE (1024)

struct LocAPIPingTestReqMsg: LocAPIMsgHeader
{
    uint8_t data[LOCATION_REMOTE_API_PINGTEST_SIZE];

    inline LocAPIPingTestReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_PINGTEST_MSG_ID) { }
};

struct LocAPIPingTestIndMsg: LocAPIMsgHeader
{
    uint8_t data[LOCATION_REMOTE_API_PINGTEST_SIZE];

    inline LocAPIPingTestIndMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_PINGTEST_MSG_ID) { }
};

#endif /* LOCATIONAPIMSG_H */
