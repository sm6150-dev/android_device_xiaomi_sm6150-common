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
#include <sstream>

namespace location_client {

static uint32_t gfIdGenerator = LOCATION_CLIENT_SESSION_ID_INVALID;
mutex GeofenceImpl::mGfMutex;
uint32_t GeofenceImpl::nextId() {
    lock_guard<mutex> lock(mGfMutex);
    uint32_t id = ++gfIdGenerator;
    if (0xFFFFFFFF == id) {
        id = 0;
        gfIdGenerator = 0;
    }
    return id;
}

/******************************************************************************
Utilities
******************************************************************************/
static LocationCapabilitiesMask parseCapabilitiesMask(::LocationCapabilitiesMask mask) {
    uint64_t capsMask = 0;

    LOC_LOGd ("LocationCapabilitiesMask =0x%x ", mask);

    if (LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT & mask) {
        capsMask |= LOCATION_CAPS_TIME_BASED_TRACKING_BIT;
    }
    if (LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_TIME_BASED_BATCHING_BIT;
    }
    if (LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT;
    }
    if (LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT;
    }
    if (LOCATION_CAPABILITIES_GEOFENCE_BIT & mask) {
        capsMask |=  LOCATION_CAPS_GEOFENCE_BIT;
    }
    if (LOCATION_CAPABILITIES_OUTDOOR_TRIP_BATCHING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT;
    }

    return static_cast<LocationCapabilitiesMask>(capsMask);
}

static Location parseLocation(const ::Location &halLocation) {
    Location location;
    uint32_t flags = 0;

    location.timestamp = halLocation.timestamp;
    location.latitude = halLocation.latitude;
    location.longitude = halLocation.longitude;
    location.altitude = halLocation.altitude;
    location.speed = halLocation.speed;
    location.bearing = halLocation.bearing;
    location.horizontalAccuracy = halLocation.accuracy;
    location.verticalAccuracy = halLocation.verticalAccuracy;
    location.speedAccuracy = halLocation.speedAccuracy;
    location.bearingAccuracy = halLocation.bearingAccuracy;

    if (0 != halLocation.timestamp) {
        flags |= LOCATION_HAS_TIMESTAMP_BIT;
    }
    if (LOCATION_HAS_LAT_LONG_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_LAT_LONG_BIT;
    }
    if (LOCATION_HAS_ALTITUDE_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_ALTITUDE_BIT;
    }
    if (LOCATION_HAS_SPEED_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_SPEED_BIT;
    }
    if (LOCATION_HAS_BEARING_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_BEARING_BIT;
    }
    if (LOCATION_HAS_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_ACCURACY_BIT;
    }
    if (LOCATION_HAS_VERTICAL_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (LOCATION_HAS_SPEED_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (LOCATION_HAS_BEARING_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
    location.flags = (LocationFlagsMask)flags;

    flags = 0;
    if (LOCATION_TECHNOLOGY_GNSS_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_GNSS_BIT;
    }
    if (LOCATION_TECHNOLOGY_CELL_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_CELL_BIT;
    }
    if (LOCATION_TECHNOLOGY_WIFI_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_WIFI_BIT;
    }
    if (LOCATION_TECHNOLOGY_SENSORS_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_SENSORS_BIT;
    }
    location.techMask = (LocationTechnologyMask)flags;

    return location;
}

static GnssLocationSvUsedInPosition parseLocationSvUsedInPosition(
        const ::GnssLocationSvUsedInPosition &halSv) {

    GnssLocationSvUsedInPosition clientSv;
    clientSv.gpsSvUsedIdsMask = halSv.gpsSvUsedIdsMask;
    clientSv.gloSvUsedIdsMask = halSv.gloSvUsedIdsMask;
    clientSv.galSvUsedIdsMask = halSv.galSvUsedIdsMask;
    clientSv.bdsSvUsedIdsMask = halSv.bdsSvUsedIdsMask;
    clientSv.qzssSvUsedIdsMask = halSv.qzssSvUsedIdsMask;

    return clientSv;
}

static void parseGnssMeasUsageInfo(const ::GnssLocationInfoNotification &halLocationInfo,
        std::vector<GnssMeasUsageInfo>& clientMeasUsageInfo) {

    if (halLocationInfo.numOfMeasReceived) {

        for (int idx = 0; idx < halLocationInfo.numOfMeasReceived; idx++) {
            GnssMeasUsageInfo measUsageInfo;

            measUsageInfo.gnssSignalType = (GnssSignalTypeMask)
                    halLocationInfo.measUsageInfo[idx].gnssSignalType;
            measUsageInfo.gnssConstellation = (Gnss_LocSvSystemEnumType)
                    halLocationInfo.measUsageInfo[idx].gnssConstellation;
            measUsageInfo.gnssSvId = halLocationInfo.measUsageInfo[idx].gnssSvId;
            clientMeasUsageInfo.push_back(measUsageInfo);
        }
    }
}

static GnssLocationPositionDynamics parseLocationPositionDynamics(
        const ::GnssLocationPositionDynamics &halPositionDynamics) {
    GnssLocationPositionDynamics positionDynamics = {};
    uint32_t bodyFrameDataMask = 0;

    if (LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
        positionDynamics.longAccel = halPositionDynamics.longAccel;
    }
    if (LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
        positionDynamics.latAccel = halPositionDynamics.latAccel;
    }
    if (LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
        positionDynamics.vertAccel = halPositionDynamics.vertAccel;
    }
    if (LOCATION_NAV_DATA_HAS_YAW_RATE_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
        positionDynamics.yawRate = halPositionDynamics.yawRate;
    }
    if (LOCATION_NAV_DATA_HAS_PITCH_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_BIT;
        positionDynamics.pitch = halPositionDynamics.pitch;
    }

    if (LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
        positionDynamics.longAccelUnc = halPositionDynamics.longAccelUnc;
    }
    if (LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
        positionDynamics.latAccelUnc = halPositionDynamics.latAccelUnc;
    }
    if (LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
        positionDynamics.vertAccelUnc = halPositionDynamics.vertAccelUnc;
    }
    if (LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
        positionDynamics.yawRateUnc = halPositionDynamics.yawRateUnc;
    }
    if (LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
        positionDynamics.pitchUnc = halPositionDynamics.pitchUnc;
    }

    positionDynamics.bodyFrameDataMask = (GnssLocationPosDataMask)bodyFrameDataMask;

    return positionDynamics;
}

static LocationReliability parseLocationReliability(const ::LocationReliability &halReliability) {

    LocationReliability reliability;
    switch (halReliability) {
        case LOCATION_RELIABILITY_VERY_LOW:
            reliability = LOCATION_RELIABILITY_VERY_LOW;
            break;

        case LOCATION_RELIABILITY_LOW:
            reliability = LOCATION_RELIABILITY_LOW;
            break;

        case LOCATION_RELIABILITY_MEDIUM:
            reliability = LOCATION_RELIABILITY_MEDIUM;
            break;

        case LOCATION_RELIABILITY_HIGH:
            reliability = LOCATION_RELIABILITY_HIGH;
            break;

        default:
            reliability = LOCATION_RELIABILITY_NOT_SET;
            break;
    }

    return reliability;
}

static GnssSystemTimeStructType parseGnssTime(const ::GnssSystemTimeStructType &halGnssTime) {

    GnssSystemTimeStructType   gnssTime;
    uint32_t gnssTimeFlags = 0;

    if (GNSS_SYSTEM_TIME_WEEK_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_VALID;
    }
    if (GNSS_SYSTEM_TIME_WEEK_MS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
    }
    if (GNSS_SYSTEM_REF_FCOUNT_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_REF_FCOUNT_VALID;
    }
    if (GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
    }

    gnssTime.validityMask = (GnssSystemTimeStructTypeFlags)gnssTimeFlags;
    gnssTime.systemWeek = halGnssTime.systemWeek;
    gnssTime.systemMsec = halGnssTime.systemMsec;
    gnssTime.systemClkTimeBias = halGnssTime.systemClkTimeBias;
    gnssTime.systemClkTimeUncMs = halGnssTime.systemClkTimeUncMs;
    gnssTime.refFCount = halGnssTime.refFCount;
    gnssTime.numClockResets = halGnssTime.numClockResets;

    return gnssTime;
}

static GnssGloTimeStructType parseGloTime(const ::GnssGloTimeStructType &halGloTime) {

    GnssGloTimeStructType   gloTime;
    uint32_t gloTimeFlags = 0;

    if (GNSS_CLO_DAYS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_CLO_DAYS_VALID;
    }
    if (GNSS_GLO_MSEC_VALID  & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_MSEC_VALID ;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_VALID;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_UNC_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
    }
    if (GNSS_GLO_REF_FCOUNT_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_REF_FCOUNT_VALID;
    }
    if (GNSS_GLO_NUM_CLOCK_RESETS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
    }
    if (GNSS_GLO_FOUR_YEAR_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_FOUR_YEAR_VALID;
    }

    gloTime.validityMask = (GnssGloTimeStructTypeFlags)gloTimeFlags;
    gloTime.gloDays = halGloTime.gloDays;
    gloTime.gloMsec = halGloTime.gloMsec;
    gloTime.gloClkTimeBias = halGloTime.gloClkTimeBias;
    gloTime.gloClkTimeUncMs = halGloTime.gloClkTimeUncMs;
    gloTime.refFCount = halGloTime.refFCount;
    gloTime.numClockResets = halGloTime.numClockResets;
    gloTime.gloFourYear = halGloTime.gloFourYear;

    return gloTime;
}

static GnssSystemTime parseSystemTime(const ::GnssSystemTime &halSystemTime) {

    GnssSystemTime systemTime;
    memset(&systemTime, 0x0, sizeof(GnssSystemTime));

    switch (halSystemTime.gnssSystemTimeSrc) {
        case GNSS_LOC_SV_SYSTEM_GPS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GPS;
           systemTime.u.gpsSystemTime = parseGnssTime(halSystemTime.u.gpsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GALILEO;
           systemTime.u.galSystemTime = parseGnssTime(halSystemTime.u.galSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_SBAS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_SBAS;
           break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GLONASS;
           systemTime.u.gloSystemTime = parseGloTime(halSystemTime.u.gloSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_BDS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_BDS;
           systemTime.u.bdsSystemTime = parseGnssTime(halSystemTime.u.bdsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_QZSS;
           systemTime.u.qzssSystemTime = parseGnssTime(halSystemTime.u.qzssSystemTime);
           break;
    }

    return systemTime;
}

static GnssLocation parseLocationInfo(const ::GnssLocationInfoNotification &halLocationInfo) {

    GnssLocation locationInfo;
    ::Location halLocation = halLocationInfo.location;
    uint32_t flags = 0;

    locationInfo.timestamp = halLocation.timestamp;
    locationInfo.latitude = halLocation.latitude;
    locationInfo.longitude = halLocation.longitude;
    locationInfo.altitude = halLocation.altitude;
    locationInfo.speed = halLocation.speed;
    locationInfo.bearing = halLocation.bearing;
    locationInfo.horizontalAccuracy = halLocation.accuracy;
    locationInfo.verticalAccuracy = halLocation.verticalAccuracy;
    locationInfo.speedAccuracy = halLocation.speedAccuracy;
    locationInfo.bearingAccuracy = halLocation.bearingAccuracy;

    if (0 != halLocation.timestamp) {
        flags |= LOCATION_HAS_TIMESTAMP_BIT;
    }
    if (LOCATION_HAS_LAT_LONG_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_LAT_LONG_BIT;
    }
    if (LOCATION_HAS_ALTITUDE_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_ALTITUDE_BIT;
    }
    if (LOCATION_HAS_SPEED_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_SPEED_BIT;
    }
    if (LOCATION_HAS_BEARING_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_BEARING_BIT;
    }
    if (LOCATION_HAS_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_ACCURACY_BIT;
    }
    if (LOCATION_HAS_VERTICAL_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (LOCATION_HAS_SPEED_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (LOCATION_HAS_BEARING_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
    locationInfo.flags = (LocationFlagsMask)flags;

    flags = 0;
    if (LOCATION_TECHNOLOGY_GNSS_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_GNSS_BIT;
    }
    if (LOCATION_TECHNOLOGY_CELL_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_CELL_BIT;
    }
    if (LOCATION_TECHNOLOGY_WIFI_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_WIFI_BIT;
    }
    if (LOCATION_TECHNOLOGY_SENSORS_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_SENSORS_BIT;
    }
    locationInfo.techMask = (LocationTechnologyMask)flags;

    flags = 0;
    if (GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
    }
    if (GNSS_LOCATION_INFO_DOP_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_DOP_BIT;
    }
    if (GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT;
    }
    if (GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT;
    }
    if (GNSS_LOCATION_INFO_VER_RELIABILITY_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_VER_RELIABILITY_BIT;
    }
    if (GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT;
    }
    if (GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT;
    }
    if (GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT;
    }
    if (GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT;
    }
    if (GNSS_LOCATION_INFO_POS_TECH_MASK_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_POS_TECH_MASK_BIT;
    }
    if (GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT;
    }
    if (GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT;
    }
    if (GNSS_LOCATION_INFO_EXT_DOP_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EXT_DOP_BIT;
    }
    if (GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
    }
    if (GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT;
    }
    if (GNSS_LOCATION_INFO_EAST_STD_DEV_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EAST_STD_DEV_BIT;
    }
    if (GNSS_LOCATION_INFO_NORTH_VEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NORTH_VEL_BIT;
    }
    if (GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT;
    }
    if (GNSS_LOCATION_INFO_EAST_VEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EAST_VEL_BIT;
    }
    if (GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT;
    }
    if (GNSS_LOCATION_INFO_UP_VEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_UP_VEL_BIT;
    }
    if (GNSS_LOCATION_INFO_UP_VEL_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_UP_VEL_UNC_BIT;
    }
    if (GNSS_LOCATION_INFO_LEAP_SECONDS_BIT & halLocationInfo.flags) {
       flags |= GNSS_LOCATION_INFO_LEAP_SECONDS_BIT;
    }
    if (GNSS_LOCATION_INFO_TIME_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_TIME_UNC_BIT;
    }

    locationInfo.gnssInfoFlags = (GnssLocationInfoFlagMask)flags;
    locationInfo.altitudeMeanSeaLevel = halLocationInfo.altitudeMeanSeaLevel;
    locationInfo.pdop = halLocationInfo.pdop;
    locationInfo.hdop = halLocationInfo.hdop;
    locationInfo.vdop = halLocationInfo.vdop;
    locationInfo.gdop = halLocationInfo.gdop;
    locationInfo.tdop = halLocationInfo.tdop;
    locationInfo.magneticDeviation = halLocationInfo.magneticDeviation;
    locationInfo.horReliability = parseLocationReliability(halLocationInfo.horReliability);
    locationInfo.verReliability = parseLocationReliability(halLocationInfo.verReliability);
    locationInfo.horUncEllipseSemiMajor = halLocationInfo.horUncEllipseSemiMajor;
    locationInfo.horUncEllipseSemiMinor = halLocationInfo.horUncEllipseSemiMinor;
    locationInfo.horUncEllipseOrientAzimuth = halLocationInfo.horUncEllipseOrientAzimuth;
    locationInfo.northStdDeviation = halLocationInfo.northStdDeviation;
    locationInfo.eastStdDeviation = halLocationInfo.eastStdDeviation;
    locationInfo.northVelocity = halLocationInfo.northVelocity;
    locationInfo.eastVelocity = halLocationInfo.eastVelocity;
    locationInfo.upVelocity = halLocationInfo.upVelocity;
    locationInfo.northVelocityStdDeviation = halLocationInfo.northVelocityStdDeviation;
    locationInfo.eastVelocityStdDeviation = halLocationInfo.eastVelocityStdDeviation;
    locationInfo.upVelocityStdDeviation = halLocationInfo.upVelocityStdDeviation;
    locationInfo.svUsedInPosition =
            parseLocationSvUsedInPosition(halLocationInfo.svUsedInPosition);
    parseGnssMeasUsageInfo(halLocationInfo, locationInfo.measUsageInfo);

    flags = 0;
    if (LOCATION_SBAS_CORRECTION_IONO_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_CORRECTION_IONO_BIT;
    }
    if (LOCATION_SBAS_CORRECTION_FAST_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_CORRECTION_FAST_BIT;
    }
    if (LOCATION_SBAS_CORRECTION_LONG_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_CORRECTION_LONG_BIT;
    }
    if (LOCATION_SBAS_INTEGRITY_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_INTEGRITY_BIT;
    }
    if (LOCATION_NAV_CORRECTION_DGNSS_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_DGNSS_BIT;
    }
    if (LOCATION_NAV_CORRECTION_RTK_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_RTK_BIT;
    }
    if (LOCATION_NAV_CORRECTION_PPP_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_PPP_BIT;
    }
    locationInfo.navSolutionMask = (GnssLocationNavSolutionMask)flags;

    flags = 0;
    if (LOCATION_POS_TECH_DEFAULT_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_DEFAULT_BIT;
    }
    if (LOCATION_POS_TECH_SATELLITE_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_SATELLITE_BIT;
    }
    if (LOCATION_POS_TECH_CELLID_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_CELLID_BIT;
    }
    if (LOCATION_POS_TECH_WIFI_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_WIFI_BIT;
    }
    if (LOCATION_POS_TECH_SENSORS_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_SENSORS_BIT;
    }
    if (LOCATION_POS_TECH_REFERENCE_LOCATION_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_REFERENCE_LOCATION_BIT;
    }
    if (LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT;
    }
    if (LOCATION_POS_TECH_AFLT_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_AFLT_BIT;
    }
    if (LOCATION_POS_TECH_HYBRID_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_HYBRID_BIT;
    }
    if (LOCATION_POS_TECH_PPE_BIT & halLocationInfo.posTechMask) {
        flags |= LOCATION_POS_TECH_PPE_BIT;
    }
    locationInfo.posTechMask = (GnssLocationPosTechMask)flags;
    locationInfo.bodyFrameData = parseLocationPositionDynamics(halLocationInfo.bodyFrameData);
    locationInfo.gnssSystemTime = parseSystemTime(halLocationInfo.gnssSystemTime);
    locationInfo.leapSeconds = halLocationInfo.leapSeconds;
    locationInfo.timeUncMs = halLocationInfo.timeUncMs;

    return locationInfo;
}

static GnssSv parseGnssSv(const ::GnssSv &halGnssSv) {
    GnssSv gnssSv;

    gnssSv.svId = halGnssSv.svId;
    switch (halGnssSv.type) {
        case GNSS_SV_TYPE_GPS:
            gnssSv.type = GNSS_SV_TYPE_GPS;
            break;

        case GNSS_SV_TYPE_SBAS:
            gnssSv.type = GNSS_SV_TYPE_SBAS;
            break;

        case GNSS_SV_TYPE_GLONASS:
            gnssSv.type = GNSS_SV_TYPE_GLONASS;
            break;

        case GNSS_SV_TYPE_QZSS:
            gnssSv.type = GNSS_SV_TYPE_QZSS;
            break;

        case GNSS_SV_TYPE_BEIDOU:
            gnssSv.type = GNSS_SV_TYPE_BEIDOU;
            break;

        case GNSS_SV_TYPE_GALILEO:
            gnssSv.type = GNSS_SV_TYPE_GALILEO;
            break;

        default:
            gnssSv.type = GNSS_SV_TYPE_UNKNOWN;
            break;
    }
    gnssSv.cN0Dbhz = halGnssSv.cN0Dbhz;
    gnssSv.elevation = halGnssSv.elevation;
    gnssSv.azimuth = halGnssSv.azimuth;

    uint32_t gnssSvOptionsMask = 0;
    if (GNSS_SV_OPTIONS_HAS_EPHEMER_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask &= GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
    }
    if (GNSS_SV_OPTIONS_HAS_ALMANAC_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask &= GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
    }
    if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask &= GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }
    gnssSv.gnssSvOptionsMask = (GnssSvOptionsMask)gnssSvOptionsMask;

    return gnssSv;
}

static GnssData parseGnssData(const ::GnssDataNotification &halGnssData) {

    GnssData gnssData;

    for (int sig = GNSS_LOC_SIGNAL_TYPE_GPS_L1CA;
         sig < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
        gnssData.gnssDataMask[sig] = halGnssData.gnssDataMask[sig];
        gnssData.jammerInd[sig] = halGnssData.jammerInd[sig];
        gnssData.agc[sig] = halGnssData.agc[sig];
        if (0 != gnssData.gnssDataMask[sig]) {
            LOC_LOGv("gnssDataMask[%d]=0x%X", sig, gnssData.gnssDataMask[sig]);
            LOC_LOGv("jammerInd[%d]=%f", sig, gnssData.jammerInd[sig]);
            LOC_LOGv("agc[%d]=%f", sig, gnssData.agc[sig]);
        }
    }
    return gnssData;
}

static LocationResponse parseLocationError(::LocationError error) {
    LocationResponse response;

    switch (error) {
        case LOCATION_ERROR_SUCCESS:
            response = LOCATION_RESPONSE_SUCCESS;
            break;
        case LOCATION_ERROR_NOT_SUPPORTED:
            response = LOCATION_RESPONSE_NOT_SUPPORTED;
            break;
        default:
            response = LOCATION_RESPONSE_UNKOWN_FAILURE;
            break;
    }

    return response;
}

static LocationSystemInfo parseLocationSystemInfo(
        const::LocationSystemInfo &halSystemInfo) {
    LocationSystemInfo systemInfo = {0};

    if (halSystemInfo.systemInfoMask & LOCATION_SYS_INFO_LEAP_SECOND) {
        systemInfo.systemInfoMask |= LOCATION_SYS_INFO_LEAP_SECOND;

        if (halSystemInfo.leapSecondSysInfo.leapSecondInfoMask &
                LEAP_SECOND_SYS_INFO_LEAP_SECOND_CHANGE_BIT) {
            systemInfo.leapSecondSysInfo.leapSecondInfoMask |=
                    LEAP_SECOND_SYS_INFO_LEAP_SECOND_CHANGE_BIT;

            LeapSecondChangeInfo &clientInfo =
                    systemInfo.leapSecondSysInfo.leapSecondChangeInfo;
            const::LeapSecondChangeInfo &halInfo =
                    halSystemInfo.leapSecondSysInfo.leapSecondChangeInfo;

            clientInfo.gpsTimestampLsChange = parseGnssTime(halInfo.gpsTimestampLsChange);
            clientInfo.leapSecondsBeforeChange = halInfo.leapSecondsBeforeChange;
            clientInfo.leapSecondsAfterChange = halInfo.leapSecondsAfterChange;
        }

        if (halSystemInfo.leapSecondSysInfo.leapSecondInfoMask &
            LEAP_SECOND_SYS_INFO_CURRENT_LEAP_SECONDS_BIT) {
            systemInfo.leapSecondSysInfo.leapSecondInfoMask |=
                    LEAP_SECOND_SYS_INFO_CURRENT_LEAP_SECONDS_BIT;
            systemInfo.leapSecondSysInfo.leapSecondCurrent =
                    halSystemInfo.leapSecondSysInfo.leapSecondCurrent;

        }
    }

    return systemInfo;
}

/******************************************************************************
LocationClientApiImpl
******************************************************************************/

uint32_t  LocationClientApiImpl::mClientIdGenerator = LOCATION_CLIENT_SESSION_ID_INVALID;

mutex LocationClientApiImpl::mMutex;

/******************************************************************************
LocationClientApiImpl - constructors
******************************************************************************/
LocationClientApiImpl::LocationClientApiImpl(CapabilitiesCb capabitiescb) :
        mSessionId(LOCATION_CLIENT_SESSION_ID_INVALID),
        mBatchingId(LOCATION_CLIENT_SESSION_ID_INVALID),
        mCapabilitiesCb(capabitiescb), mLastAddedClientIds({}),
        mHalRegistered(false), mLocationCb(nullptr), mBatchingCb(nullptr),
        mCallbacksMask(0), mGnssEnergyConsumedInfoCb(nullptr),
        mGnssEnergyConsumedResponseCb(nullptr),
        mLocationSysInfoCb(nullptr),
        mLocationSysInfoResponseCb(nullptr) {

    mMsgTask = new MsgTask("ClientApiImpl", false);

    // create socket to send
    mIpcSender = new LocIpcSender(SOCKET_TO_LOCATION_HAL_DAEMON);
    if (nullptr == mIpcSender) {
        LOC_LOGe("create mIpcSender failed %s", SOCKET_TO_LOCATION_HAL_DAEMON);
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
        LOC_LOGd("scoketname=%s", mSocketName);
        startListeningNonBlocking(mSocketName);
    } else {
        LOC_LOGe("strlcpy failed %d", strCopied);
    }
}

LocationClientApiImpl::~LocationClientApiImpl() {
    bool rc = false;

    // deregister
    if (mHalRegistered && (nullptr != mIpcSender)) {
        LocAPIClientDeregisterReqMsg msg(mSocketName);
        rc = mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                   sizeof(msg));
        LOC_LOGd(">>> DeregisterReq rc=%d", rc);
        delete mIpcSender;
    }

    if (mMsgTask) {
        mMsgTask->destroy();
    }
}

/******************************************************************************
LocationClientApiImpl - implementation
******************************************************************************/
void LocationClientApiImpl::updateCallbackFunctions(const ClientCallbacks& cbs) {

    struct UpdateCallbackFunctionsReq : public LocMsg {
        UpdateCallbackFunctionsReq(LocationClientApiImpl* apiImpl, const ClientCallbacks& cbs) :
                mApiImpl(apiImpl), mCbs(cbs) {}
        virtual ~UpdateCallbackFunctionsReq() {}
        void proc() const {
            // update callback functions
            mApiImpl->mResponseCb = mCbs.responsecb;
            mApiImpl->mCollectiveResCb = mCbs.collectivecb;
            mApiImpl->mLocationCb = mCbs.locationcb;
            mApiImpl->mBatchingCb = mCbs.batchingcb;
            mApiImpl->mGfBreachCb = mCbs.gfbreachcb;
            mApiImpl->mGnssReportCbs = mCbs.gnssreportcbs;
        }
        LocationClientApiImpl* mApiImpl;
        const ClientCallbacks mCbs;
    };
    mMsgTask->sendMsg(new (nothrow) UpdateCallbackFunctionsReq(this, cbs));
}

void LocationClientApiImpl::updateCallbacks(LocationCallbacks& callbacks) {

    struct UpdateCallbacksReq : public LocMsg {
        UpdateCallbacksReq(LocationClientApiImpl* apiImpl, const LocationCallbacks& callbacks) :
                mApiImpl(apiImpl), mCallBacks(callbacks) {}
        virtual ~UpdateCallbacksReq() {}
        void proc() const {

            //convert callbacks to callBacksMask
            LocationCallbacksMask callBacksMask = 0;
            if (mCallBacks.trackingCb) {
                callBacksMask |= E_LOC_CB_TRACKING_BIT;
            }
            if (mCallBacks.gnssLocationInfoCb) {
                callBacksMask |= E_LOC_CB_GNSS_LOCATION_INFO_BIT;
            }
            if (mCallBacks.gnssSvCb) {
                callBacksMask |= E_LOC_CB_GNSS_SV_BIT;
            }
            if (mCallBacks.gnssNmeaCb) {
                callBacksMask |= E_LOC_CB_GNSS_NMEA_BIT;
            }
            if (mCallBacks.gnssDataCb) {
                callBacksMask |= E_LOC_CB_GNSS_DATA_BIT;
            }
            // handle callbacks that are not related to a fix session
            if (mApiImpl->mLocationSysInfoCb) {
                callBacksMask |= E_LOC_CB_SYSTEM_INFO_BIT;
            }
            if (mCallBacks.batchingCb) {
                callBacksMask |= E_LOC_CB_BATCHING_BIT;
            }
            if (mCallBacks.batchingStatusCb) {
                callBacksMask |= E_LOC_CB_BATCHING_STATUS_BIT;
            }
            if (mCallBacks.geofenceBreachCb) {
                callBacksMask |= E_LOC_CB_GEOFENCE_BREACH_BIT;
            }

            // update callback only when changed
            if (mApiImpl->mCallbacksMask != callBacksMask) {
                mApiImpl->mCallbacksMask = callBacksMask;
                if (mApiImpl->mHalRegistered) {
                    LocAPIUpdateCallbacksReqMsg msg(mApiImpl->mSocketName,
                                                    mApiImpl->mCallbacksMask);
                    bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                         sizeof(msg));
                    LOC_LOGd(">>> UpdateCallbacksReq callBacksMask=0x%x rc=%d",
                             mApiImpl->mCallbacksMask, rc);
                }
            } else {
                LOC_LOGd("No updateCallbacks because same callBacksMask 0x%x", callBacksMask);
            }
        }
        LocationClientApiImpl* mApiImpl;
        const LocationCallbacks mCallBacks;
    };
    mMsgTask->sendMsg(new (nothrow) UpdateCallbacksReq(this, callbacks));
}

uint32_t LocationClientApiImpl::startTracking(TrackingOptions& option) {

    struct StartTrackingReq : public LocMsg {
        StartTrackingReq(LocationClientApiImpl* apiImpl, TrackingOptions& option) :
                mApiImpl(apiImpl), mOption(option) {}
        virtual ~StartTrackingReq() {}
        void proc() const {
            // check if option is updated
            bool isOptionUpdated = false;
            if ((mApiImpl->mLocationOptions.minInterval != mOption.minInterval) ||
                (mApiImpl->mLocationOptions.minDistance != mOption.minDistance)) {
                isOptionUpdated = true;
            }
            mApiImpl->mLocationOptions = mOption;
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> startTracking - Not registered yet");
                return;
            }

            if (LOCATION_CLIENT_SESSION_ID_INVALID == mApiImpl->mSessionId) {
                //start a new tracking session
                mApiImpl->mSessionId = mApiImpl->mClientId;
                LocAPIStartTrackingReqMsg msg(mApiImpl->mSocketName,
                                              mApiImpl->mLocationOptions.minInterval,
                                              mApiImpl->mLocationOptions.minDistance);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                     sizeof(msg));
                LOC_LOGd(">>> StartTrackingReq Interval=%d Distance=%d",
                         mApiImpl->mLocationOptions.minInterval,
                         mApiImpl->mLocationOptions.minDistance);
            } else if (isOptionUpdated) {
                //update a tracking session
                mApiImpl->updateTrackingOptionsSync(
                        mApiImpl, const_cast<TrackingOptions&>(mOption));
            } else {
                LOC_LOGd(">>> StartTrackingReq - no change in option");
            }
        }
        LocationClientApiImpl* mApiImpl;
        TrackingOptions mOption;
    };
    mMsgTask->sendMsg(new (nothrow) StartTrackingReq(this, option));
    return 0;
}

void LocationClientApiImpl::stopTracking(uint32_t) {

    struct StopTrackingReq : public LocMsg {
        StopTrackingReq(LocationClientApiImpl* apiImpl) : mApiImpl(apiImpl) {}
        virtual ~StopTrackingReq() {}
        void proc() const {
            if (mApiImpl->mSessionId == mApiImpl->mClientId) {
                mApiImpl->mLocationOptions.minInterval = 0;
                mApiImpl->mLocationOptions.minDistance = 0;
                mApiImpl->mCallbacksMask = 0;
                // handle callback that are not tied with fix session
                if (mApiImpl->mLocationSysInfoCb) {
                    mApiImpl->mCallbacksMask |= E_LOC_CB_SYSTEM_INFO_BIT;
                }
                LocAPIStopTrackingReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                    sizeof(msg));
                LOC_LOGd(">>> StopTrackingReq rc=%d", rc);
            }
            mApiImpl->mSessionId = LOCATION_CLIENT_SESSION_ID_INVALID;
        }
        LocationClientApiImpl* mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) StopTrackingReq(this));
}

void LocationClientApiImpl::updateTrackingOptionsSync(
        LocationClientApiImpl* pImpl, TrackingOptions& option) {
        LocAPIUpdateTrackingOptionsReqMsg
                msg(pImpl->mSocketName, option.minInterval, option.minDistance);
        bool rc = pImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
        LOC_LOGd(">>> updateTrackingOptionsSync Interval=%d Distance=%d",
                option.minInterval, option.minDistance);
        pImpl->mLocationOptions = option;
}

void LocationClientApiImpl::updateTrackingOptions(uint32_t, TrackingOptions& option) {

    struct UpdateTrackingOptionsReq : public LocMsg {
        UpdateTrackingOptionsReq(LocationClientApiImpl* apiImpl, TrackingOptions& option) :
                mApiImpl(apiImpl), mOption(option) {}
        virtual ~UpdateTrackingOptionsReq() {}
        void proc() const {
            if ((mApiImpl->mLocationOptions.minInterval != mOption.minInterval) ||
                (mApiImpl->mLocationOptions.minDistance != mOption.minDistance)) {
                mApiImpl->updateTrackingOptionsSync(
                        mApiImpl, const_cast<TrackingOptions&>(mOption));
            } else {
                LOC_LOGd("No UpdateTrackingOptions because same Interval=%d Distance=%d",
                        mOption.minInterval, mOption.minDistance);
            }
        }
        LocationClientApiImpl* mApiImpl;
        TrackingOptions mOption;
    };
    mMsgTask->sendMsg(new (nothrow) UpdateTrackingOptionsReq(this, option));
}

//Batching
uint32_t LocationClientApiImpl::startBatching(BatchingOptions& batchOptions) {
    struct StartBatchingReq : public LocMsg {
        StartBatchingReq(LocationClientApiImpl *apiImpl, BatchingOptions& batchOptions) :
            mApiImpl(apiImpl), mBatchOptions(batchOptions) {}
        virtual ~StartBatchingReq() {}
        void proc() const {
            mApiImpl->mBatchingOptions = mBatchOptions;
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> startBatching - Not registered yet");
                return;
            }
            if (LOCATION_CLIENT_SESSION_ID_INVALID == mApiImpl->mSessionId) {
                //start a new batching session
                mApiImpl->mBatchingId = mApiImpl->mClientId;
                LocAPIStartBatchingReqMsg msg(mApiImpl->mSocketName,
                        mApiImpl->mBatchingOptions.minInterval,
                        mApiImpl->mBatchingOptions.minDistance,
                        mApiImpl->mBatchingOptions.batchingMode);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                        sizeof(msg));
                LOC_LOGd(">>> StartBatchingReq Interval=%d Distance=%d BatchingMode=%d",
                        mApiImpl->mBatchingOptions.minInterval,
                        mApiImpl->mBatchingOptions.minDistance,
                        mApiImpl->mBatchingOptions.batchingMode);
            } else {
                mApiImpl->updateBatchingOptions(mApiImpl->mBatchingId,
                        const_cast<BatchingOptions&>(mBatchOptions));
            }
        }
        LocationClientApiImpl *mApiImpl;
        BatchingOptions mBatchOptions;
    };
    mMsgTask->sendMsg(new (nothrow) StartBatchingReq(this, batchOptions));
    return 0;
}

void LocationClientApiImpl::stopBatching(uint32_t id) {
    struct StopBatchingReq : public LocMsg {
        StopBatchingReq(LocationClientApiImpl *apiImpl) :
            mApiImpl(apiImpl) {}
        virtual ~StopBatchingReq() {}
        void proc() const {
            if (mApiImpl->mBatchingId == mApiImpl->mClientId) {
                mApiImpl->mBatchingOptions = {};
                LocAPIStopBatchingReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                        sizeof(msg));
                LOC_LOGd(">>> StopBatchingReq rc=%d", rc);
            }
            mApiImpl->mBatchingId = LOCATION_CLIENT_SESSION_ID_INVALID;
        }
        LocationClientApiImpl *mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) StopBatchingReq(this));
}

void LocationClientApiImpl::updateBatchingOptions(uint32_t id, BatchingOptions& batchOptions) {
    struct UpdateBatchingOptionsReq : public LocMsg {
        UpdateBatchingOptionsReq(LocationClientApiImpl* apiImpl, BatchingOptions& batchOptions) :
            mApiImpl(apiImpl), mBatchOptions(batchOptions) {}
        virtual ~UpdateBatchingOptionsReq() {}
        void proc() const {
            mApiImpl->mBatchingOptions = mBatchOptions;
            LocAPIUpdateBatchingOptionsReqMsg msg(mApiImpl->mSocketName,
                    mApiImpl->mBatchingOptions.minInterval,
                    mApiImpl->mBatchingOptions.minDistance,
                    mApiImpl->mBatchingOptions.batchingMode);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                    sizeof(msg));
            LOC_LOGd(">>> StartBatchingReq Interval=%d Distance=%d BatchingMode=%d",
                    mApiImpl->mBatchingOptions.minInterval,
                    mApiImpl->mBatchingOptions.minDistance,
                    mApiImpl->mBatchingOptions.batchingMode);
        }
        LocationClientApiImpl *mApiImpl;
        BatchingOptions mBatchOptions;
    };

    if ((mBatchingOptions.minInterval != batchOptions.minInterval) ||
            (mBatchingOptions.minDistance != batchOptions.minDistance) ||
            mBatchingOptions.batchingMode != batchOptions.batchingMode) {
        mMsgTask->sendMsg(new (nothrow) UpdateBatchingOptionsReq(this, batchOptions));
    } else {
        LOC_LOGd("No UpdateBatchingOptions because same Interval=%d Distance=%d, BatchingMode=%d",
                batchOptions.minInterval, batchOptions.minDistance, batchOptions.batchingMode);
    }
}

void LocationClientApiImpl::getBatchedLocations(uint32_t id, size_t count) {}

bool LocationClientApiImpl::checkGeofenceMap(size_t count, uint32_t* ids) {
    for (int i=0; i<count; ++i) {
        auto got = mGeofenceMap.find(ids[i]);
        if (got == mGeofenceMap.end()) {
            return false;
        }
    }
    return true;
}
void LocationClientApiImpl::addGeofenceMap(uint32_t id, Geofence& geofence) {
    lock_guard<mutex> lock(mMutex);
    mGeofenceMap.insert(make_pair(id, geofence));
}
void LocationClientApiImpl::eraseGeofenceMap(size_t count, uint32_t* ids) {
    lock_guard<mutex> lock(mMutex);
    for (int i=0; i<count; ++i) {
        mGeofenceMap.erase(ids[i]);
    }
}

uint32_t* LocationClientApiImpl::addGeofences(size_t count, GeofenceOption* options,
        GeofenceInfo* infos) {
    struct AddGeofencesReq : public LocMsg {
        AddGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, GeofenceOption* gfOptions,
                GeofenceInfo* gfInfos, std::vector<uint32_t> clientIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfOptions(gfOptions), mGfInfos(gfInfos),
                mClientIds(clientIds) {}
        virtual ~AddGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> addGeofences - Not registered yet");
                return;
            }

            if (mGfCount > 0) {
                //Add geofences, serialize geofence msg payload into ipc message payload
                size_t msglen = sizeof(LocAPIAddGeofencesReqMsg) +
                        sizeof(GeofencePayload) * (mGfCount - 1);
                uint8_t *msg = new(std::nothrow) uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                memset(msg, 0, msglen);
                LocAPIAddGeofencesReqMsg *pMsg = reinterpret_cast<LocAPIAddGeofencesReqMsg*>(msg);
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                pMsg->msgId = E_LOCAPI_ADD_GEOFENCES_MSG_ID;
                pMsg->geofences.count = mGfCount;
                for (int i=0; i<mGfCount; ++i) {
                    (*(pMsg->geofences.gfPayload + i)).gfClientId = mClientIds[i];
                    (*(pMsg->geofences.gfPayload + i)).gfOption = mGfOptions[i];
                    (*(pMsg->geofences.gfPayload + i)).gfInfo = mGfInfos[i];
                }
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> AddGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfOptions);
                free(mGfInfos);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        GeofenceOption* mGfOptions;
        GeofenceInfo* mGfInfos;
        std::vector<uint32_t>  mClientIds;
    };
    mMsgTask->sendMsg(new (nothrow) AddGeofencesReq(this, count, options, infos,
            mLastAddedClientIds));
    return nullptr;
}

void LocationClientApiImpl::removeGeofences(size_t count, uint32_t* ids) {
    struct RemoveGeofencesReq : public LocMsg {
        RemoveGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds) {}
        virtual ~RemoveGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> removeGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Remove geofences
                size_t msglen = sizeof(LocAPIRemoveGeofencesReqMsg) +
                        sizeof(uint32_t) * (mGfCount - 1);
                uint8_t* msg = new uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                LocAPIRemoveGeofencesReqMsg* pMsg = (LocAPIRemoveGeofencesReqMsg*)msg;
                pMsg->gfClientIds.count = mGfCount;
                memcpy(pMsg->gfClientIds.gfIds, mGfIds, sizeof(uint32_t) * mGfCount);
                pMsg->msgId = E_LOCAPI_REMOVE_GEOFENCES_MSG_ID;
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> RemoveGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
    };
    mMsgTask->sendMsg(new (nothrow) RemoveGeofencesReq(this, count, ids));
}

void LocationClientApiImpl::modifyGeofences(
        size_t count, uint32_t* ids, GeofenceOption* options) {
    struct ModifyGeofencesReq : public LocMsg {
        ModifyGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds,
                GeofenceOption* gfOptions) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds), mGfOptions(gfOptions) {}
        virtual ~ModifyGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> modifyGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Modify geofences
                size_t msglen = sizeof(LocAPIModifyGeofencesReqMsg) +
                        sizeof(GeofencePayload) * (mGfCount-1);
                uint8_t *msg = new(std::nothrow) uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                memset(msg, 0, msglen);
                LocAPIModifyGeofencesReqMsg *pMsg =
                        reinterpret_cast<LocAPIModifyGeofencesReqMsg*>(msg);
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                pMsg->msgId = E_LOCAPI_MODIFY_GEOFENCES_MSG_ID;
                pMsg->geofences.count = mGfCount;
                for (int i=0; i<mGfCount; ++i) {
                    (*(pMsg->geofences.gfPayload + i)).gfClientId = mGfIds[i];
                    (*(pMsg->geofences.gfPayload + i)).gfOption = mGfOptions[i];
                }
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> ModifyGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
                free(mGfOptions);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
        GeofenceOption* mGfOptions;
    };
    mMsgTask->sendMsg(new (nothrow) ModifyGeofencesReq(this, count, ids, options));
}

void LocationClientApiImpl::pauseGeofences(size_t count, uint32_t* ids) {
    struct PauseGeofencesReq : public LocMsg {
        PauseGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds) {}
        virtual ~PauseGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> pauseGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Pause geofences
                size_t msglen = sizeof(LocAPIPauseGeofencesReqMsg) +
                        sizeof(uint32_t) * (mGfCount - 1);
                uint8_t* msg = new uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                LocAPIPauseGeofencesReqMsg* pMsg = (LocAPIPauseGeofencesReqMsg*)msg;
                pMsg->gfClientIds.count = mGfCount;
                memcpy(&(pMsg->gfClientIds.gfIds[0]), mGfIds, sizeof(uint32_t) * mGfCount);
                pMsg->msgId = E_LOCAPI_PAUSE_GEOFENCES_MSG_ID;
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> PauseGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
    };
    mMsgTask->sendMsg(new (nothrow) PauseGeofencesReq(this, count, ids));
}

void LocationClientApiImpl::resumeGeofences(size_t count, uint32_t* ids) {
    struct ResumeGeofencesReq : public LocMsg {
        ResumeGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds) {}
        virtual ~ResumeGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> resumeGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Resume geofences
                size_t msglen = sizeof(LocAPIResumeGeofencesReqMsg) +
                    sizeof(uint32_t) * (mGfCount - 1);
                uint8_t* msg = new uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                LocAPIResumeGeofencesReqMsg* pMsg = (LocAPIResumeGeofencesReqMsg*)msg;
                pMsg->gfClientIds.count = mGfCount;
                memcpy(pMsg->gfClientIds.gfIds, mGfIds, sizeof(uint32_t) * mGfCount);
                pMsg->msgId = E_LOCAPI_RESUME_GEOFENCES_MSG_ID;
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> ResumeGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
    };
    mMsgTask->sendMsg(new (nothrow) ResumeGeofencesReq(this, count, ids));
}

uint32_t* LocationClientApiImpl::gnssUpdateConfig(GnssConfig config) {

    struct UpdateConfigReq : public LocMsg {
        UpdateConfigReq(LocationClientApiImpl* apiImpl, GnssConfig& config) :
                mApiImpl(apiImpl), mConfig(config) {}
        virtual ~UpdateConfigReq() {}
        void proc() const {
            LocAPIUpdateConfigReqMsg msg(mApiImpl->mSocketName, const_cast<GnssConfig&>(mConfig));
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
        }
        LocationClientApiImpl* mApiImpl;
        GnssConfig mConfig;
    };
    mMsgTask->sendMsg(new (nothrow) UpdateConfigReq(this, config));
    return nullptr;
}

uint32_t LocationClientApiImpl::gnssDeleteAidingData(GnssAidingData& data) {

    struct DeleteAidingDataReq : public LocMsg {
        DeleteAidingDataReq(const LocationClientApiImpl* apiImpl, GnssAidingData& data) :
                mApiImpl(apiImpl), mAidingData(data) {}
        virtual ~DeleteAidingDataReq() {}
        void proc() const {
            LocAPIDeleteAidingDataReqMsg msg(mApiImpl->mSocketName,
                                             const_cast<GnssAidingData&>(mAidingData));
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
            LOC_LOGd(">>> DeleteAidingDataReq rc=%d", rc);
        }
        const LocationClientApiImpl* mApiImpl;
        GnssAidingData mAidingData;
    };
    mMsgTask->sendMsg(new (nothrow) DeleteAidingDataReq(this, data));
    return 0;
}

void LocationClientApiImpl::updateNetworkAvailability(bool available) {

    struct UpdateNetworkAvailabilityReq : public LocMsg {
        UpdateNetworkAvailabilityReq(const LocationClientApiImpl* apiImpl, bool available) :
                mApiImpl(apiImpl), mAvailable(available) {}
        virtual ~UpdateNetworkAvailabilityReq() {}
        void proc() const {
            LocAPIUpdateNetworkAvailabilityReqMsg msg(mApiImpl->mSocketName,
                                                      mAvailable);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
            LOC_LOGd(">>> UpdateNetworkAvailabilityReq available=%d ", mAvailable);
        }
        const LocationClientApiImpl* mApiImpl;
        const bool mAvailable;
    };
    mMsgTask->sendMsg(new (nothrow) UpdateNetworkAvailabilityReq(this, available));
}

void LocationClientApiImpl::getGnssEnergyConsumed(
        GnssEnergyConsumedCb gnssEnergyConsumedCallback,
        ResponseCb responseCallback) {

    struct GetGnssEnergyConsumedReq : public LocMsg {
        GetGnssEnergyConsumedReq(LocationClientApiImpl *apiImpl,
                                 GnssEnergyConsumedCb gnssEnergyConsumedCb,
                                 ResponseCb responseCb) :
        mApiImpl(apiImpl),
        mGnssEnergyConsumedCb(gnssEnergyConsumedCb),
        mResponseCb(responseCb) {}

        virtual ~GetGnssEnergyConsumedReq() {}
        void proc() const {
            mApiImpl->mGnssEnergyConsumedInfoCb = mGnssEnergyConsumedCb;
            mApiImpl->mGnssEnergyConsumedResponseCb = mResponseCb;

            // send msg to the hal daemon
            if (nullptr != mApiImpl->mGnssEnergyConsumedInfoCb) {
                LocAPIGetGnssEnergyConsumedReqMsg msg(mApiImpl->mSocketName);
                mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                           sizeof(msg));
            }
        }

        LocationClientApiImpl *mApiImpl;
        GnssEnergyConsumedCb   mGnssEnergyConsumedCb;
        ResponseCb             mResponseCb;
    };

    LOC_LOGd(">>> getGnssEnergyConsumed ");
    mMsgTask->sendMsg(new (nothrow)GetGnssEnergyConsumedReq(
            this, gnssEnergyConsumedCallback, responseCallback));
}

void LocationClientApiImpl::updateLocationSystemInfoListener(
    LocationSystemInfoCb locSystemInfoCallback,
    ResponseCb responseCallback) {

    struct UpdateLocationSystemInfoListenerReq : public LocMsg {
        UpdateLocationSystemInfoListenerReq(LocationClientApiImpl *apiImpl,
                                       LocationSystemInfoCb sysInfoCb,
                                       ResponseCb responseCb) :
        mApiImpl(apiImpl),
        mLocSysInfoCb(sysInfoCb),
        mResponseCb(responseCb) {}

        virtual ~UpdateLocationSystemInfoListenerReq() {}
        void proc() const {
            bool needIpc = false;
            LocationCallbacksMask callbackMaskCopy = mApiImpl->mCallbacksMask;
            // send msg to the hal daemon if the registration changes
            if ((nullptr != mLocSysInfoCb) &&
                (nullptr == mApiImpl->mLocationSysInfoCb)) {
                // client registers for system info, set up the bit
                mApiImpl->mCallbacksMask |= E_LOC_CB_SYSTEM_INFO_BIT;
                needIpc = true;
            } else if ((nullptr == mLocSysInfoCb) &&
                       (nullptr != mApiImpl->mLocationSysInfoCb)) {
                // system info is no longer needed, clear the bit
                mApiImpl->mCallbacksMask &= ~E_LOC_CB_SYSTEM_INFO_BIT;
                needIpc = true;
            }

            // save the new callback
            mApiImpl->mLocationSysInfoCb = mLocSysInfoCb;
            mApiImpl->mLocationSysInfoResponseCb = mResponseCb;

            // inform hal daemon of updated callback only when changed
            if (needIpc == true) {
                if (mApiImpl->mHalRegistered) {
                    LocAPIUpdateCallbacksReqMsg msg(mApiImpl->mSocketName,
                                                    mApiImpl->mCallbacksMask);
                    bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg),
                                                         sizeof(msg));
                    LOC_LOGd(">>> UpdateCallbacksReq new callBacksMask=0x%x, "
                             "old mask =0x%x, rc=%d",
                             mApiImpl->mCallbacksMask, callbackMaskCopy, rc);
                }
            } else {
                LOC_LOGd("No updateCallbacks because same callback");
            }
        }

        LocationClientApiImpl *mApiImpl;
        LocationSystemInfoCb   mLocSysInfoCb;
        ResponseCb             mResponseCb;
    };

    LOC_LOGd(">>> updateLocationSystemInfoListener ");
    mMsgTask->sendMsg(new (nothrow)UpdateLocationSystemInfoListenerReq(
            this, locSystemInfoCallback, responseCallback));
}

/******************************************************************************
LocationClientApiImpl - LocIpc onReceive handler
******************************************************************************/
void LocationClientApiImpl::capabilitesCallback(ELocMsgID msgId, const void* msgData) {

    mHalRegistered = true;
    const LocAPICapabilitiesIndMsg* pCapabilitiesIndMsg =
            (LocAPICapabilitiesIndMsg*)(msgData);
    LocationCapabilitiesMask capsMask = parseCapabilitiesMask(pCapabilitiesIndMsg->capabilitiesMask);
    if (mCapabilitiesCb) {
        mCapabilitiesCb(capsMask);
    }

    // send updatecallback request
    if (0 != mCallbacksMask) {
        LocAPIUpdateCallbacksReqMsg msg(mSocketName, mCallbacksMask);
        bool rc = mIpcSender->send(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
        LOC_LOGd(">>> UpdateCallbacksReq callBacksMask=0x%x rc=%d", mCallbacksMask, rc);
    }

    if (0 != mLocationOptions.minInterval) {
        TrackingOptions trackOption;
        trackOption.setLocationOptions(mLocationOptions);
        (void)startTracking(trackOption);
    }
}

void LocationClientApiImpl::pingTest(PingTestCb pingTestCallback) {

    mPingTestCb = pingTestCallback;

    struct PingTestReq : public LocMsg {
        PingTestReq(LocationClientApiImpl *apiImpl) : mApiImpl(apiImpl){}
        virtual ~PingTestReq() {}
        void proc() const {
            LocAPIPingTestReqMsg msg(mApiImpl->mSocketName);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
        }
        LocationClientApiImpl *mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) PingTestReq(this));
    return;
}

/******************************************************************************
LocationClientApiImpl - LocIpc overrides
******************************************************************************/
void LocationClientApiImpl::onListenerReady() {

    struct ClientRegisterReq : public LocMsg {
        ClientRegisterReq(LocationClientApiImpl* apiImpl) : mApiImpl(apiImpl) {}
        virtual ~ClientRegisterReq() {}
        void proc() const {
            LocAPIClientRegisterReqMsg msg(mApiImpl->mSocketName);
            bool rc = mApiImpl->mIpcSender->send(reinterpret_cast<uint8_t *>(&msg), sizeof(msg));
            LOC_LOGd(">>> onListenerReady::ClientRegisterReqMsg rc=%d", rc);
        }
        LocationClientApiImpl *mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) ClientRegisterReq(this));
}

void LocationClientApiImpl::onReceive(const string& data) {

    struct OnReceiveHandler : public LocMsg {
        OnReceiveHandler(LocationClientApiImpl* apiImpl, const string& data) :
                mApiImpl(apiImpl), mMsgData(data) {}
        virtual ~OnReceiveHandler() {}
        void proc() const {
            LocAPIMsgHeader *pMsg = (LocAPIMsgHeader *)(mMsgData.data());

            switch (pMsg->msgId) {
                case E_LOCAPI_CAPABILILTIES_MSG_ID:
                    {
                        LOC_LOGd("<<< capabilities indication");
                        mApiImpl->capabilitesCallback(pMsg->msgId, (void*)pMsg);
                        break;
                    }

                case E_LOCAPI_HAL_READY_MSG_ID:
                    {
                        LOC_LOGd("<<< HAL ready ");
                        mApiImpl->onListenerReady();
                        break;
                    }

                case E_LOCAPI_START_TRACKING_MSG_ID:
                case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID:
                case E_LOCAPI_STOP_TRACKING_MSG_ID:
                case E_LOCAPI_START_BATCHING_MSG_ID:
                case E_LOCAPI_STOP_BATCHING_MSG_ID:
                case E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID:
                    {
                        LOC_LOGd("<<< response message, msgId = %d", pMsg->msgId);
                        const LocAPIGenericRespMsg* pRespMsg = (LocAPIGenericRespMsg*)(pMsg);
                        LocationResponse response = parseLocationError(pRespMsg->err);
                        if (mApiImpl->mResponseCb) {
                            mApiImpl->mResponseCb(response);
                        }
                        break;
                    }
                case E_LOCAPI_ADD_GEOFENCES_MSG_ID:
                case E_LOCAPI_REMOVE_GEOFENCES_MSG_ID:
                case E_LOCAPI_MODIFY_GEOFENCES_MSG_ID:
                case E_LOCAPI_PAUSE_GEOFENCES_MSG_ID:
                case E_LOCAPI_RESUME_GEOFENCES_MSG_ID:
                    {
                        LOC_LOGd("<<< collective response message, msgId = %d", pMsg->msgId);
                        const LocAPICollectiveRespMsg* pRespMsg = (LocAPICollectiveRespMsg*)(pMsg);
                        std::vector<pair<Geofence, LocationResponse>> responses{};
                        for (int i=0; i<pRespMsg->collectiveRes.count; ++i) {
                            responses.push_back(make_pair(
                                    mApiImpl->mGeofenceMap.at(
                                    (*(pRespMsg->collectiveRes.resp + i)).clientId),
                                    parseLocationError(
                                    (*(pRespMsg->collectiveRes.resp + i)).error)));
                            if (LOCATION_ERROR_SUCCESS !=
                                    (*(pRespMsg->collectiveRes.resp + i)).error ||
                                    E_LOCAPI_REMOVE_GEOFENCES_MSG_ID == pMsg->msgId) {
                                mApiImpl->eraseGeofenceMap(1, const_cast<uint32_t*>(
                                        &((*(pRespMsg->collectiveRes.resp + i)).clientId)));
                            }
                        }
                        if (mApiImpl->mCollectiveResCb) {
                            mApiImpl->mCollectiveResCb(responses);
                        }
                        break;
                    }

                // async indication messages
                case E_LOCAPI_LOCATION_MSG_ID:
                    {
                        LOC_LOGd("<<< message = location");
                        if (mApiImpl->mCallbacksMask & E_LOC_CB_TRACKING_BIT) {
                            const LocAPILocationIndMsg* pLocationIndMsg = (LocAPILocationIndMsg*)(pMsg);
                            Location location = parseLocation(pLocationIndMsg->locationNotification);
                            if (mApiImpl->mLocationCb) {
                                mApiImpl->mLocationCb(location);
                            }
                        }
                        break;
                    }
                case E_LOCAPI_BATCHING_MSG_ID:
                    {
                        LOC_LOGd("<<< message = batching");
                        if (mApiImpl->mCallbacksMask & E_LOC_CB_BATCHING_BIT) {
                            const LocAPIBatchingIndMsg* pBatchingIndMsg =
                                    (LocAPIBatchingIndMsg*)(pMsg);
                            std::vector<Location> locationVector;
                            BatchingStatus status = BATCHING_STATUS_INACTIVE;
                            if (BATCHING_STATUS_POSITION_AVAILABE ==
                                    pBatchingIndMsg->batchNotification.status) {
                                for (int i=0; i<pBatchingIndMsg->batchNotification.count; ++i) {
                                    locationVector.push_back(parseLocation(
                                                *(pBatchingIndMsg->batchNotification.location +
                                                i)));
                                }
                                status = BATCHING_STATUS_ACTIVE;
                            } else if (BATCHING_STATUS_TRIP_COMPLETED ==
                                    pBatchingIndMsg->batchNotification.status) {
                                mApiImpl->stopBatching(0);
                                status = BATCHING_STATUS_DONE;
                            } else {
                                LOC_LOGe("invalid Batching Status!");
                                break;
                            }

                            if (mApiImpl->mBatchingCb) {
                                mApiImpl->mBatchingCb(locationVector, status);
                            }
                        }
                        break;
                    }
                case E_LOCAPI_GEOFENCE_BREACH_MSG_ID:
                    {
                        LOC_LOGd("<<< message = geofence breach");
                        if (mApiImpl->mCallbacksMask & E_LOCAPI_GEOFENCE_BREACH_MSG_ID) {
                            const LocAPIGeofenceBreachIndMsg* pGfBreachIndMsg =
                                (LocAPIGeofenceBreachIndMsg*)(pMsg);
                            std::vector<Geofence> geofences;
                            for (int i=0; i<pGfBreachIndMsg->gfBreachNotification.count;
                                    ++i) {
                                geofences.push_back(mApiImpl->mGeofenceMap.at(
                                        *(pGfBreachIndMsg->gfBreachNotification.id + i)));
                            }
                            if (mApiImpl->mGfBreachCb) {
                                mApiImpl->mGfBreachCb(geofences,
                                        parseLocation(
                                                pGfBreachIndMsg->gfBreachNotification.location),
                                        GeofenceBreachTypeMask(
                                                pGfBreachIndMsg->gfBreachNotification.type),
                                        pGfBreachIndMsg->gfBreachNotification.timestamp);
                            }
                        }
                        break;
                    }
                case E_LOCAPI_LOCATION_INFO_MSG_ID:
                    {
                        LOC_LOGd("<<< message = location info");
                        if (mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_LOCATION_INFO_BIT) {
                            const LocAPILocationInfoIndMsg* pLocationInfoIndMsg =
                                    (LocAPILocationInfoIndMsg*)(pMsg);
                            GnssLocation gnssLocation =
                                    parseLocationInfo(pLocationInfoIndMsg->gnssLocationInfoNotification);
                            if (mApiImpl->mGnssReportCbs.gnssLocationCallback) {
                                mApiImpl->mGnssReportCbs.gnssLocationCallback(gnssLocation);
                            }
                        }
                        break;
                    }

                case E_LOCAPI_SATELLITE_VEHICLE_MSG_ID:
                    {
                        LOC_LOGd("<<< message = sv");
                        if (mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_SV_BIT) {
                            const LocAPISatelliteVehicleIndMsg* pSvIndMsg =
                                    (LocAPISatelliteVehicleIndMsg*)(pMsg);
                            std::vector<GnssSv> gnssSvsVector;
                            for (int i=0; i< pSvIndMsg->gnssSvNotification.count; i++) {
                                  GnssSv gnssSv;
                                  gnssSv = parseGnssSv(pSvIndMsg->gnssSvNotification.gnssSvs[i]);
                                  gnssSvsVector.push_back(gnssSv);
                            }
                            if (mApiImpl->mGnssReportCbs.gnssSvCallback) {
                                mApiImpl->mGnssReportCbs.gnssSvCallback(gnssSvsVector);
                            }
                        }
                        break;
                    }

                case E_LOCAPI_NMEA_MSG_ID:
                    {
                        if ((mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_NMEA_BIT) &&
                                (mApiImpl->mGnssReportCbs.gnssNmeaCallback)) {
                            const LocAPINmeaIndMsg* pNmeaIndMsg = (LocAPINmeaIndMsg*)(pMsg);
                            uint64_t timestamp = pNmeaIndMsg->gnssNmeaNotification.timestamp;
                            std::string nmea(pNmeaIndMsg->gnssNmeaNotification.nmea,
                                             pNmeaIndMsg->gnssNmeaNotification.length);
                            LOC_LOGd("<<< message = nmea[%s]", nmea.c_str());
                            std::stringstream ss(nmea);
                            std::string each;
                            while(std::getline(ss, each, '\n')) {
                                each += '\n';
                                mApiImpl->mGnssReportCbs.gnssNmeaCallback(timestamp, each);
                            }
                        }
                        break;
                    }

                case E_LOCAPI_DATA_MSG_ID:
                    {
                        LOC_LOGd("<<< message = data");
                        if (mApiImpl->mCallbacksMask & E_LOC_CB_GNSS_DATA_BIT) {
                            const LocAPIDataIndMsg* pDataIndMsg = (LocAPIDataIndMsg*)(pMsg);
                            GnssData gnssData =
                                parseGnssData(pDataIndMsg->gnssDataNotification);
                            if (mApiImpl->mGnssReportCbs.gnssDataCallback) {
                                mApiImpl->mGnssReportCbs.gnssDataCallback(gnssData);
                            }
                        }
                        break;
                    }

                case E_LOCAPI_GET_GNSS_ENGERY_CONSUMED_MSG_ID:
                    {
                        LOC_LOGd("<<< message = GNSS power consumption");
                        LocAPIGnssEnergyConsumedIndMsg* pEnergyMsg =
                                (LocAPIGnssEnergyConsumedIndMsg*) pMsg;
                        uint64_t energyNumber =
                                pEnergyMsg->totalGnssEnergyConsumedSinceFirstBoot;
                        uint32_t flags = 0;
                        if (energyNumber != 0xffffffffffffffff) {
                            flags = ENERGY_CONSUMED_SINCE_FIRST_BOOT_BIT;
                        }
                        GnssEnergyConsumedInfo energyConsumedInfo = {};
                        energyConsumedInfo.flags =
                                (location_client::GnssEnergyConsumedInfoMask) flags;
                        energyConsumedInfo.totalEnergyConsumedSinceFirstBoot = energyNumber;
                        if (flags == 0 && mApiImpl->mGnssEnergyConsumedResponseCb) {
                            mApiImpl->mGnssEnergyConsumedResponseCb(
                                    LOCATION_RESPONSE_UNKOWN_FAILURE);
                        } else if (mApiImpl->mGnssEnergyConsumedInfoCb){
                            mApiImpl->mGnssEnergyConsumedInfoCb(energyConsumedInfo);
                        }
                        break;
                    }

                case E_LOCAPI_LOCATION_SYSTEM_INFO_MSG_ID:
                        LOC_LOGd("<<< message = location system info");
                        if (mApiImpl->mCallbacksMask & E_LOC_CB_SYSTEM_INFO_BIT) {
                            const LocAPILocationSystemInfoIndMsg * pDataIndMsg =
                                    (LocAPILocationSystemInfoIndMsg*)(pMsg);
                            LocationSystemInfo locationSystemInfo =
                                parseLocationSystemInfo(pDataIndMsg->locationSystemInfo);
                            if (mApiImpl->mLocationSysInfoCb) {
                                mApiImpl->mLocationSysInfoCb(locationSystemInfo);
                            }
                        }
                        break;

                case E_LOCAPI_PINGTEST_MSG_ID:
                    {
                        LOC_LOGd("<<< ping message %d", pMsg->msgId);
                        const LocAPIPingTestIndMsg* pIndMsg = (LocAPIPingTestIndMsg*)(pMsg);
                        if (mApiImpl->mPingTestCb) {
                            uint32_t response = pIndMsg->data[0];
                            mApiImpl->mPingTestCb(response);
                        }
                        break;
                    }

                default:
                    {
                        LOC_LOGe("<<< unknown message %d", pMsg->msgId);
                        break;
                    }
            }
        }
        LocationClientApiImpl *mApiImpl;
        const string mMsgData;
    };
    mMsgTask->sendMsg(new (nothrow) OnReceiveHandler(this, data));
}

/******************************************************************************
LocationClientApiImpl - Not implemented overrides
******************************************************************************/

void LocationClientApiImpl::gnssNiResponse(uint32_t id, GnssNiResponse response) {
}

} // namespace location_client

