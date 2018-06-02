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

#include <loc_cfg.h>

#include <LocationDataTypes.h>
#include <LocationClientApi.h>
#include <LocationClientApiImpl.h>
#include <LocationApiMsg.h>
#include <log_util.h>

using namespace std;

static void clientTrackingCb(Location location) {
    LOC_LOGd ("clientTrackingCb\n");
    return;
}

static void clientGnssLocationInfoCb(
      GnssLocationInfoNotification gnssLocationInfoNotification) {
    LOC_LOGd ("clientGnssLocationInfoCb\n");
    return;
}

static void clientGnssSvCb(GnssSvNotification gnssSvNotification) {
    LOC_LOGd ("clientGnssSvCb\n");
    return;
}

static void clientGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification) {
    LOC_LOGd ("clientGnssNmeaCb\n");
    return;
}

static location_client::LocationCapabilitiesMask parseCapabilitiesMask(
    LocationCapabilitiesMask mask) {
    uint64_t capsMask = 0;

    LOC_LOGd ("LocationCapabilitiesMask =0x%x \n", mask);

    if (LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT & mask) {
        capsMask |= location_client::LOCATION_CAPS_TIME_BASED_TRACKING_BIT;
    }
    if (LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT & mask) {
        capsMask |=  location_client::LOCATION_CAPS_TIME_BASED_BATCHING_BIT;
    }
    if (LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT & mask) {
        capsMask |=  location_client::LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT;
    }
    if (LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT & mask) {
        capsMask |=  location_client::LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT;
    }
    if (LOCATION_CAPABILITIES_GEOFENCE_BIT & mask) {
        capsMask |=  location_client::LOCATION_CAPS_GEOFENCE_BIT;
    }
    if (LOCATION_CAPABILITIES_OUTDOOR_TRIP_BATCHING_BIT & mask) {
        capsMask |=  location_client::LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT;
    }

    return static_cast<location_client::LocationCapabilitiesMask>(capsMask);
}

static location_client::Location parseLocation(const Location &halLocation) {
    location_client::Location location;
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

    if (LOCATION_HAS_LAT_LONG_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_LAT_LONG_BIT;
    }
    if (LOCATION_HAS_ALTITUDE_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_ALTITUDE_BIT;
    }
    if (LOCATION_HAS_SPEED_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_SPEED_BIT;
    }
    if (LOCATION_HAS_BEARING_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_BEARING_BIT;
    }
    if (LOCATION_HAS_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_ACCURACY_BIT;
    }
    if (LOCATION_HAS_VERTICAL_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (LOCATION_HAS_SPEED_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (LOCATION_HAS_BEARING_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
    location.flags = (location_client::LocationFlagsMask)flags;

    flags = 0;
    if (LOCATION_TECHNOLOGY_GNSS_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_GNSS_BIT;
    }
    if (LOCATION_TECHNOLOGY_CELL_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_CELL_BIT;
    }
    if (LOCATION_TECHNOLOGY_WIFI_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_WIFI_BIT;
    }
    if (LOCATION_TECHNOLOGY_SENSORS_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_SENSORS_BIT;
    }
    location.techMask = (location_client::LocationTechnologyMask)flags;

    return location;
}

static location_client::GnssLocationSvUsedInPosition parseLocationSvUsedInPosition(
    const GnssLocationSvUsedInPosition &halSv) {
    location_client::GnssLocationSvUsedInPosition clientSv;

    clientSv.gpsSvUsedIdsMask = halSv.gpsSvUsedIdsMask;
    clientSv.gloSvUsedIdsMask = halSv.gloSvUsedIdsMask;
    clientSv.galSvUsedIdsMask = halSv.galSvUsedIdsMask;
    clientSv.bdsSvUsedIdsMask = halSv.bdsSvUsedIdsMask;
    clientSv.qzssSvUsedIdsMask = halSv.qzssSvUsedIdsMask;

    return clientSv;
}

static location_client::GnssLocationPositionDynamics parseLocationPositionDynamics(
    const GnssLocationPositionDynamics &halPositionDynamics) {
    location_client::GnssLocationPositionDynamics positionDynamics;
    uint32_t bodyFrameDataMask = 0;

    if (LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= location_client::LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
    }
    if (LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= location_client::LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
    }
    if (LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= location_client::LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
    }
    if (LOCATION_NAV_DATA_HAS_YAW_RATE_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= location_client::LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
    }
    if (LOCATION_NAV_DATA_HAS_PITCH_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= location_client::LOCATION_NAV_DATA_HAS_PITCH_BIT;
    }
    positionDynamics.bodyFrameDataMask =
            (location_client::GnssLocationPosDataMask)bodyFrameDataMask;

    positionDynamics.longAccel = halPositionDynamics.longAccel;
    positionDynamics.latAccel = halPositionDynamics.latAccel;
    positionDynamics.vertAccel = halPositionDynamics.vertAccel;
    positionDynamics.yawRate = halPositionDynamics.yawRate;
    positionDynamics.pitch = halPositionDynamics.pitch;

    return positionDynamics;
}

static location_client::LocationReliability parseLocationReliability(
    const LocationReliability &halReliability) {
    location_client::LocationReliability reliability;
    switch (halReliability) {
        case LOCATION_RELIABILITY_VERY_LOW:
            reliability = location_client::LOCATION_RELIABILITY_VERY_LOW;
            break;

        case LOCATION_RELIABILITY_LOW:
            reliability = location_client::LOCATION_RELIABILITY_LOW;
            break;

        case LOCATION_RELIABILITY_MEDIUM:
            reliability = location_client::LOCATION_RELIABILITY_MEDIUM;
            break;

        case LOCATION_RELIABILITY_HIGH:
            reliability = location_client::LOCATION_RELIABILITY_HIGH;
            break;

        default:
            reliability = location_client::LOCATION_RELIABILITY_NOT_SET;
            break;
    }

    return reliability;
}

static location_client::GnssSystemTimeStructType parseGnssTime(
    const GnssSystemTimeStructType &halGnssTime) {
    location_client::GnssSystemTimeStructType   gnssTime;
    uint32_t gnssTimeFlags = 0;

    if (GNSS_SYSTEM_TIME_WEEK_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= location_client::GNSS_SYSTEM_TIME_WEEK_VALID;
    }
    if (GNSS_SYSTEM_TIME_WEEK_MS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= location_client::GNSS_SYSTEM_TIME_WEEK_MS_VALID;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= location_client::GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= location_client::GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
    }
    if (GNSS_SYSTEM_REF_FCOUNT_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= location_client::GNSS_SYSTEM_REF_FCOUNT_VALID;
    }
    if (GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= location_client::GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
    }

    gnssTime.validityMask = (location_client::GnssSystemTimeStructTypeFlags)gnssTimeFlags;
    gnssTime.systemWeek = halGnssTime.systemWeek;
    gnssTime.systemMsec = halGnssTime.systemMsec;
    gnssTime.systemClkTimeBias = halGnssTime.systemClkTimeBias;
    gnssTime.systemClkTimeUncMs = halGnssTime.systemClkTimeUncMs;
    gnssTime.refFCount = halGnssTime.refFCount;

    return gnssTime;
}

static location_client::GnssGloTimeStructType parseGloTime(
    const GnssGloTimeStructType &halGloTime) {
    location_client::GnssGloTimeStructType   gloTime;
    uint32_t gloTimeFlags = 0;

    if (GNSS_CLO_DAYS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_CLO_DAYS_VALID;
    }
    if (GNSS_GLOS_MSEC_VALID  & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_GLOS_MSEC_VALID ;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_GLO_CLK_TIME_BIAS_VALID;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_UNC_VALID & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
    }
    if (GNSS_GLO_REF_FCOUNT_VALID & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_GLO_REF_FCOUNT_VALID;
    }
    if (GNSS_GLO_NUM_CLOCK_RESETS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_GLO_NUM_CLOCK_RESETS_VALID;
    }
    if (GNSS_GLO_FOUR_YEAR_VALID & halGloTime.validityMask) {
        gloTimeFlags |= location_client::GNSS_GLO_FOUR_YEAR_VALID;
    }

    gloTime.validityMask = (location_client::GnssGloTimeStructTypeFlags)gloTimeFlags;
    gloTime.gloDays = halGloTime.gloDays;
    gloTime.gloMsec = halGloTime.gloMsec;
    gloTime.gloClkTimeBias = halGloTime.gloClkTimeBias;
    gloTime.gloClkTimeUncMs = halGloTime.gloClkTimeUncMs;
    gloTime.refFCount = halGloTime.refFCount;
    gloTime.numClockResets = halGloTime.numClockResets;
    gloTime.gloFourYear = halGloTime.gloFourYear;

    return gloTime;
}

static location_client::GnssSystemTime parseSystemTime(
    const GnssSystemTime &halSystemTime) {
    location_client::GnssSystemTime systemTime;

    switch (halSystemTime.gnssSystemTimeSrc) {
        case GNSS_LOC_SV_SYSTEM_GPS:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_GPS;
           systemTime.u.gpsSystemTime = parseGnssTime(halSystemTime.u.gpsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_GALILEO;
           systemTime.u.galSystemTime = parseGnssTime(halSystemTime.u.galSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_SBAS:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_SBAS;
           break;
        case GNSS_LOC_SV_SYSTEM_COMPASS:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_COMPASS;
           break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_GLONASS;
           systemTime.u.gloSystemTime = parseGloTime(halSystemTime.u.gloSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_BDS:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_BDS;
           systemTime.u.bdsSystemTime = parseGnssTime(halSystemTime.u.bdsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
           systemTime.gnssSystemTimeSrc = location_client::GNSS_LOC_SV_SYSTEM_QZSS;
           systemTime.u.qzssSystemTime = parseGnssTime(halSystemTime.u.qzssSystemTime);
           break;
    }

    return systemTime;
}

static location_client::GnssLocation parseLocationInfo(
    const GnssLocationInfoNotification &halLocationInfo) {
    location_client::GnssLocation locationInfo;
    Location halLocation = halLocationInfo.location;
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

    if (LOCATION_HAS_LAT_LONG_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_LAT_LONG_BIT;
    }
    if (LOCATION_HAS_ALTITUDE_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_ALTITUDE_BIT;
    }
    if (LOCATION_HAS_SPEED_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_SPEED_BIT;
    }
    if (LOCATION_HAS_BEARING_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_BEARING_BIT;
    }
    if (LOCATION_HAS_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_ACCURACY_BIT;
    }
    if (LOCATION_HAS_VERTICAL_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (LOCATION_HAS_SPEED_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (LOCATION_HAS_BEARING_ACCURACY_BIT & halLocation.flags) {
        flags |= location_client::LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
    locationInfo.flags = (location_client::LocationFlagsMask)flags;

    flags = 0;
    if (LOCATION_TECHNOLOGY_GNSS_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_GNSS_BIT;
    }
    if (LOCATION_TECHNOLOGY_CELL_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_CELL_BIT;
    }
    if (LOCATION_TECHNOLOGY_WIFI_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_WIFI_BIT;
    }
    if (LOCATION_TECHNOLOGY_SENSORS_BIT & halLocation.techMask) {
        flags |= location_client::LOCATION_TECHNOLOGY_SENSORS_BIT;
    }
    locationInfo.techMask = (location_client::LocationTechnologyMask)flags;

    flags = 0;
    if (GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
    }
    if (GNSS_LOCATION_INFO_DOP_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_DOP_BIT;
    }
    if (GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT;
    }
    if (GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT;
    }
    if (GNSS_LOCATION_INFO_VER_RELIABILITY_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_VER_RELIABILITY_BIT;
    }
    if (GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT;
    }
    if (GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT;
    }
    if (GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT;
    }
    if (GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT;
    }
    if (GNSS_LOCATION_INFO_POS_TECH_MASK_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_POS_TECH_MASK_BIT;
    }
    if (GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT;
    }
    if (GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT;
    }
    if (GNSS_LOCATION_INFO_GPS_TIME_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_GPS_TIME_BIT;
    }
    if (GNSS_LOCATION_INFO_EXT_DOP_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_EXT_DOP_BIT;
    }
    if (GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT & halLocationInfo.flags) {
        flags |= location_client::GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
    }

    locationInfo.gnssInfoFlags = (location_client::GnssLocationInfoFlagMask)flags;
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
    locationInfo.northVelocity = halLocationInfo.northVelocity;
    locationInfo.eastVelocity = halLocationInfo.eastVelocity;
    locationInfo.upVelocity = halLocationInfo.upVelocity;
    locationInfo.northVelocityStdDeviation = halLocationInfo.northVelocityStdDeviation;
    locationInfo.eastVelocityStdDeviation = halLocationInfo.eastVelocityStdDeviation;
    locationInfo.upVelocityStdDeviation = halLocationInfo.upVelocityStdDeviation;
    locationInfo.svUsedInPosition =
            parseLocationSvUsedInPosition(halLocationInfo.svUsedInPosition);

    flags = 0;
    if (LOCATION_SBAS_CORRECTION_IONO_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_SBAS_CORRECTION_IONO_BIT;
    }
    if (LOCATION_SBAS_CORRECTION_FAST_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_SBAS_CORRECTION_FAST_BIT;
    }
    if (LOCATION_SBAS_CORRECTION_LONG_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_SBAS_CORRECTION_LONG_BIT;
    }
    if (LOCATION_SBAS_INTEGRITY_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_SBAS_INTEGRITY_BIT;
    }
    if (LOCATION_NAV_CORRECTION_DGNSS_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_NAV_CORRECTION_DGNSS_BIT;
    }
    if (LOCATION_NAV_CORRECTION_RTK_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_NAV_CORRECTION_RTK_BIT;
    }
    if (LOCATION_NAV_CORRECTION_PPP_BIT & halLocationInfo.navSolutionMask) {
        flags |= location_client::LOCATION_NAV_CORRECTION_PPP_BIT;
    }
    locationInfo.navSolutionMask = (location_client::GnssLocationNavSolutionMask)flags;

    flags = 0;
    if (LOCATION_POS_TECH_DEFAULT_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_DEFAULT_BIT;
    }
    if (LOCATION_POS_TECH_SATELLITE_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_SATELLITE_BIT;
    }
    if (LOCATION_POS_TECH_CELLID_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_CELLID_BIT;
    }
    if (LOCATION_POS_TECH_WIFI_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_WIFI_BIT;
    }
    if (LOCATION_POS_TECH_SENSORS_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_SENSORS_BIT;
    }
    if (LOCATION_POS_TECH_REFERENCE_LOCATION_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_REFERENCE_LOCATION_BIT;
    }
    if (LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT;
    }
    if (LOCATION_POS_TECH_AFLT_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_AFLT_BIT;
    }
    if (LOCATION_POS_TECH_HYBRID_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_HYBRID_BIT;
    }
    if (LOCATION_POS_TECH_PPE_BIT & halLocationInfo.posTechMask) {
        flags |= location_client::LOCATION_POS_TECH_PPE_BIT;
    }
    locationInfo.posTechMask = (location_client::GnssLocationPosTechMask)flags;
    locationInfo.bodyFrameData = parseLocationPositionDynamics(halLocationInfo.bodyFrameData);
    locationInfo.gnssSystemTime = parseSystemTime(halLocationInfo.gnssSystemTime);

    return locationInfo;
}

static location_client::GnssSv parseGnssSv(const GnssSv &halGnssSv) {
    location_client::GnssSv gnssSv;

    gnssSv.svId = halGnssSv.svId;
    switch (halGnssSv.type) {
        case GNSS_SV_TYPE_GPS:
            gnssSv.type = location_client::GNSS_SV_TYPE_GPS;
            break;

        case GNSS_SV_TYPE_SBAS:
            gnssSv.type = location_client::GNSS_SV_TYPE_SBAS;
            break;

        case GNSS_SV_TYPE_GLONASS:
            gnssSv.type = location_client::GNSS_SV_TYPE_GLONASS;
            break;

        case GNSS_SV_TYPE_QZSS:
            gnssSv.type = location_client::GNSS_SV_TYPE_QZSS;
            break;

        case GNSS_SV_TYPE_BEIDOU:
            gnssSv.type = location_client::GNSS_SV_TYPE_BEIDOU;
            break;

        case GNSS_SV_TYPE_GALILEO:
            gnssSv.type = location_client::GNSS_SV_TYPE_GALILEO;
            break;

        default:
            gnssSv.type = location_client::GNSS_SV_TYPE_UNKNOWN;
            break;
    }
    gnssSv.cN0Dbhz = halGnssSv.cN0Dbhz;
    gnssSv.elevation = halGnssSv.elevation;
    gnssSv.azimuth = halGnssSv.azimuth;

    uint32_t gnssSvOptionsMask = 0;
    if (GNSS_SV_OPTIONS_HAS_EPHEMER_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask &= location_client::GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
    }
    if (GNSS_SV_OPTIONS_HAS_ALMANAC_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask &= location_client::GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
    }
    if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask &= location_client::GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }
    gnssSv.gnssSvOptionsMask = (location_client::GnssSvOptionsMask)gnssSvOptionsMask;

    return gnssSv;
}

static location_client::LocationResponse parseLocationError(LocationError error) {
    location_client::LocationResponse response;

    switch (error) {
        case LOCATION_ERROR_SUCCESS:
            response = location_client::LOCATION_RESPONSE_SUCCESS;
            break;
        case LOCATION_ERROR_NOT_SUPPORTED:
            response = location_client::LOCATION_RESPONSE_NOT_SUPPORTED;
            break;
        default:
            response = location_client::LOCATION_RESPONSE_UNKOWN_FAILURE;
            break;
    }

    return response;
}

namespace location_client {

static uint32_t gDebug = 0;

static const loc_param_s_type gConfigTable[] =
{
    {"DEBUG_LEVEL", &gDebug, NULL, 'n'}
};

/******************************************************************************
LocationClientApi
******************************************************************************/
LocationClientApi::LocationClientApi(CapabilitiesCb capabitiescb) :
    mCapabilitiesCb(capabitiescb),
    mSessionId(LOCATION_CLIENT_SESSION_ID_INVALID) {
    // read configuration file
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gConfigTable);

    LOC_LOGd("gDebug=%u\n", gDebug);
    mApiImpl = new LocationClientApiImpl(this);
}

LocationClientApi::~LocationClientApi() {
    if (mApiImpl) {
        delete mApiImpl;
    }
}

bool LocationClientApi::startPositionSession(
   uint32_t intervalInMs,
   uint32_t distanceInMeters,
   LocationCb locationCallback,
   ResponseCb responseCallback) {
    LocationCallbacks   callbacksOption = {0};
    LocationOptions     locationOption = {0};
    bool                retVal = false;

    //Input parameter check
    if (!locationCallback) {
        LOC_LOGe ("NULL locationCallback\n");
        return retVal;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl\n");
        return retVal;
    }

    callbacksOption.trackingCb = clientTrackingCb;
    locationOption.minInterval = intervalInMs;
    locationOption.minDistance = distanceInMeters;

    mApiImpl->updateCallbacks(callbacksOption);
    if (LOCATION_CLIENT_SESSION_ID_INVALID == mSessionId) {
        //start a new tracking session
        mSessionId = mApiImpl->startTracking(locationOption);
        if (LOCATION_CLIENT_SESSION_ID_INVALID != mSessionId) {
            retVal = true;
        }
    } else {
        //update a tracking session
        mApiImpl->updateTrackingOptions(mSessionId, locationOption);
        retVal = true;
    }

    if (true == retVal) {
        mLocationCb = locationCallback;
        mResponseCb = responseCallback;
        mGnssReportCbs = {{nullptr}, {nullptr}, {nullptr}};
    }

    return retVal;
}

bool LocationClientApi::startPositionSession(
    uint32_t intervalInMs,
    const GnssReportCbs& gnssReportCallbacks,
    ResponseCb responseCallback) {
    LocationCallbacks   callbacksOption = {0};
    LocationOptions     locationOption = {0};
    bool                retVal = false;

    //Input parameter check
    if (!gnssReportCallbacks.gnssLocationCallback) {
        LOC_LOGe ("gnssLocation Callbacks can't be NULL\n");
        return false;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl\n");
        return retVal;
    }

    if (gnssReportCallbacks.gnssLocationCallback) {
        callbacksOption.gnssLocationInfoCb = clientGnssLocationInfoCb;
    }

    if (gnssReportCallbacks.gnssSvCallback) {
        callbacksOption.gnssSvCb = clientGnssSvCb;
    }

    if (gnssReportCallbacks.gnssNmeaCallback) {
        callbacksOption.gnssNmeaCb = clientGnssNmeaCb;
    }
    locationOption.minInterval = intervalInMs;
    locationOption.minDistance = 0;

    mApiImpl->updateCallbacks(callbacksOption);
    if (LOCATION_CLIENT_SESSION_ID_INVALID == mSessionId) {
        //start a new gnssReport session
        mSessionId = mApiImpl->startTracking(locationOption);
        if (LOCATION_CLIENT_SESSION_ID_INVALID != mSessionId) {
            retVal = true;
        }
    } else {
        mApiImpl->updateTrackingOptions(mSessionId, locationOption);
    }

    if (true == retVal) {
        mLocationCb = nullptr;
        mResponseCb = responseCallback;
        mGnssReportCbs = gnssReportCallbacks;
    }

    return retVal;
}

void LocationClientApi::stopPositionSession() {
    if (mApiImpl) {
        mApiImpl->stopTracking(mSessionId);
    }
    mSessionId = LOCATION_CLIENT_SESSION_ID_INVALID;
}

void LocationClientApi::updateNetworkAvailability(bool available) {
    if (mApiImpl) {
        mApiImpl->updateNetworkAvailability(available);
    }
}

void LocationClientApi::locationClientApiImplCb(uint32_t  msgId, const void* msgData) {
    if (nullptr == msgData) {
        LOC_LOGe ("NULL msgData\n");
        return;
    }

    ELocMsgID  locMsgId = static_cast<ELocMsgID>(msgId);

    switch (locMsgId) {
        case E_LOCAPI_CAPABILILTIES_MSG_ID:
        {
            LocationCapabilitiesMask capsMask;
            const LocAPICapabilitiesIndMsg *pCapabilitiesIndMsg =
                    (LocAPICapabilitiesIndMsg *)(msgData);
            capsMask = parseCapabilitiesMask(pCapabilitiesIndMsg->capabilitiesMask);
            if (mCapabilitiesCb) {
                    mCapabilitiesCb(capsMask);
            } else {
                LOC_LOGe ("NULL mCapabilitiesCb\n");
            }
            break;
        }

        case E_LOCAPI_LOCATION_MSG_ID:
        {
            const LocAPILocationIndMsg *pLocationIndMsg = (LocAPILocationIndMsg *)(msgData);
            Location location = parseLocation(pLocationIndMsg->locationNotification);
            if (mLocationCb) {
                mLocationCb(location);
            } else {
                LOC_LOGe ("NULL mLocationCb\n");
            }
            break;
        }

        case E_LOCAPI_LOCATION_INFO_MSG_ID:
        {
            const LocAPILocationInfoIndMsg *pLocationInfoIndMsg =
                    (LocAPILocationInfoIndMsg *)(msgData);
            GnssLocation gnssLocation =
                    parseLocationInfo(pLocationInfoIndMsg->gnssLocationInfoNotification);
            if (mGnssReportCbs.gnssLocationCallback) {
                mGnssReportCbs.gnssLocationCallback(gnssLocation);
            } else {
                LOC_LOGe ("NULL gnssLocationCallback\n");
            }
            break;
        }

        case E_LOCAPI_SATELLITE_VEHICLE_MSG_ID:
        {
            const LocAPISatelliteVehicleIndMsg *pSvIndMsg =
                    (LocAPISatelliteVehicleIndMsg *)(msgData);
            std::vector<GnssSv> gnssSvsVector;
            for (int i=0; i< pSvIndMsg->gnssSvNotification.count; i++) {
                  GnssSv gnssSv;
                  gnssSv = parseGnssSv(pSvIndMsg->gnssSvNotification.gnssSvs[i]);
                  gnssSvsVector.push_back(gnssSv);
            }
            if (mGnssReportCbs.gnssSvCallback) {
                mGnssReportCbs.gnssSvCallback(gnssSvsVector);
            } else {
                LOC_LOGe ("NULL gnssSvCallback\n");
            }
            break;
        }

        case E_LOCAPI_NMEA_MSG_ID:
        {
            const LocAPINmeaIndMsg *pNmeaIndMsg = (LocAPINmeaIndMsg *)(msgData);
            uint64_t timestamp = pNmeaIndMsg->gnssNmeaNotification.timestamp;
            std::string  nmea(pNmeaIndMsg->gnssNmeaNotification.nmea,
                              pNmeaIndMsg->gnssNmeaNotification.length);
            if (mGnssReportCbs.gnssNmeaCallback) {
                mGnssReportCbs.gnssNmeaCallback(timestamp,nmea);
            } else {
                LOC_LOGe ("NULL gnssNmeaCallback\n");
            }
            break;
        }

        case E_LOCAPI_START_TRACKING_MSG_ID:
        case E_LOCAPI_STOP_TRACKING_MSG_ID:
        case E_LOCAPI_UPDATE_CALLBACKS_MSG_ID:
        case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID:
        {
            const LocAPIGenericRespMsg *pRespMsg = (LocAPIGenericRespMsg *)(msgData);
            LocationResponse response = parseLocationError(pRespMsg->err);
            if (mResponseCb) {
                mResponseCb(response);
            } else {
                LOC_LOGe ("NULL mResponseCb\n");
            }
            break;
        }

        default:
            LOC_LOGe ("Ignore message %d\n", msgId);
            break;
    }
}

} // namespace location_client

