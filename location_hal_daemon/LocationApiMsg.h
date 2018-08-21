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

#ifndef LOCATIONAPIMSG_H
#define LOCATIONAPIMSG_H

#include <loc_pla.h> // for strlcpy
#include <LocationDataTypes.h>

/******************************************************************************
Constants
******************************************************************************/
#define LOCATION_REMOTE_API_MSG_VERSION (1)

// Maximum fully qualified path(including the file name)
// for the location remote API service and client socket name
#define MAX_SOCKET_PATHNAME_LENGTH (128)

#define LOCATION_CLIENT_SESSION_ID_INVALID (0)

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

    // tacking session
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
    E_LOCAPI_DATA_MSG_ID = 16
};

typedef uint32_t LocationCallbacksMask;
enum ELocationCallbacksOption {
    E_LOC_CB_TRACKING_BIT               = (1<<0), /**< Register for Location */
    E_LOC_CB_GNSS_LOCATION_INFO_BIT     = (1<<1), /**< Register for GNSS Location */
    E_LOC_CB_GNSS_SV_BIT                = (1<<2), /**< Register for GNSS SV */
    E_LOC_CB_GNSS_NMEA_BIT              = (1<<3), /**< Register for GNSS NMEA */
    E_LOC_CB_GNSS_DATA_BIT              = (1<<4)  /**< Register for GNSS DATA */
};

/******************************************************************************
Common data structure
******************************************************************************/
struct LocAPINmeaSerializedPayload {
    size_t size;
    uint64_t timestamp;
    size_t length;
    char nmea[1];
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
            strlcpy(mSocketName, name, MAX_SOCKET_PATHNAME_LENGTH);
        }
};

/******************************************************************************
IPC message structure - client registration
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_CLIENT_REGISTER_MSG_ID
struct LocAPIClientRegisterReqMsg: LocAPIMsgHeader
{
    inline LocAPIClientRegisterReqMsg(const char* name) :
        LocAPIMsgHeader(name, E_LOCAPI_CLIENT_REGISTER_MSG_ID) { }
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

/******************************************************************************
IPC message structure - tracking
******************************************************************************/
// defintion for message with msg id of E_LOCAPI_START_TRACKING_MSG_ID
struct LocAPIStartTrackingReqMsg: LocAPIMsgHeader
{
    uint32_t intervalInMs;
    uint32_t distanceInMeters;

    inline LocAPIStartTrackingReqMsg(const char* name,
                                     uint32_t sessionInterval,
                                     uint32_t sessionDistance):
        LocAPIMsgHeader(name, E_LOCAPI_START_TRACKING_MSG_ID),
        intervalInMs(sessionInterval),
        distanceInMeters(sessionDistance) { }
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
    uint32_t intervalInMs;
    uint32_t distanceInMeters;

    inline LocAPIUpdateTrackingOptionsReqMsg(const char* name,
                                     uint32_t sessionInterval,
                                     uint32_t sessionDistance):
        LocAPIMsgHeader(name, E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID),
        intervalInMs(sessionInterval),
        distanceInMeters(sessionDistance) { }
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

// defintion for message with msg id of E_LOCAPI_LOCATION_INFO_MSG_ID
struct LocAPILocationInfoIndMsg: LocAPIMsgHeader
{
    GnssLocationInfoNotification gnssLocationInfoNotification;

    inline LocAPILocationInfoIndMsg(const char* name,
        GnssLocationInfoNotification& locationInfo) :
        LocAPIMsgHeader(name, E_LOCAPI_LOCATION_INFO_MSG_ID),
        gnssLocationInfoNotification(locationInfo) { }
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

#endif /* LOCATIONAPIMSG_H */
