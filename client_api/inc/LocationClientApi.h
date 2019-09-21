/* Copyright (c) 2018-2019 The Linux Foundation. All rights reserved.
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

#ifndef LOCATIONCLIENTAPI_H
#define LOCATIONCLIENTAPI_H

#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace location_client
{

enum LocationCapabilitiesMask {
    /** supports startPositionSession() with intervalInMs */
    LOCATION_CAPS_TIME_BASED_TRACKING_BIT           = (1<<0),
    /** supports startBatching() with intervalInMs */
    LOCATION_CAPS_TIME_BASED_BATCHING_BIT           = (1<<1),
    /** supports startPositionSession() with distanceInMeters */
    LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT       = (1<<2),
    /** supports startBatching() with distanceInMeters */
    LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT       = (1<<3),
    /** supports addGeofences API */
    LOCATION_CAPS_GEOFENCE_BIT                      = (1<<4),
    /** support outdoor trip batching */
    LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT         = (1<<5),
};

enum GnssSvOptionsMask {
    GNSS_SV_OPTIONS_HAS_EPHEMER_BIT = (1<<0),
    GNSS_SV_OPTIONS_HAS_ALMANAC_BIT = (1<<1),
    GNSS_SV_OPTIONS_USED_IN_FIX_BIT             = (1<<2),
    GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT   = (1<<3)
};

enum LocationFlagsMask {
    /** location has valid latitude and longitude */
    LOCATION_HAS_LAT_LONG_BIT          = (1<<0),
    /** location has valid altitude */
    LOCATION_HAS_ALTITUDE_BIT          = (1<<1),
    /** location has valid speed */
    LOCATION_HAS_SPEED_BIT             = (1<<2),
    /** location has valid bearing */
    LOCATION_HAS_BEARING_BIT           = (1<<3),
    /** location has valid accuracy */
    LOCATION_HAS_ACCURACY_BIT          = (1<<4),
    /** location has valid vertical accuracy */
    LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5),
    /** location has valid speed accuracy */
    LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6),
    /** location has valid bearing accuracy */
    LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7),
    /** location has valid timestamp */
    LOCATION_HAS_TIMESTAMP_BIT         = (1<<8)
};

enum LocationTechnologyMask {
    /** location was calculated using GNSS */
    LOCATION_TECHNOLOGY_GNSS_BIT     = (1<<0),
    /** location was calculated using Cell */
    LOCATION_TECHNOLOGY_CELL_BIT     = (1<<1),
    /** location was calculated using WiFi */
    LOCATION_TECHNOLOGY_WIFI_BIT     = (1<<2),
    /** location was calculated using Sensors */
    LOCATION_TECHNOLOGY_SENSORS_BIT  = (1<<3)
};

enum GnssLocationNavSolutionMask {
    /** SBAS ionospheric correction is used  */
    LOCATION_SBAS_CORRECTION_IONO_BIT  = (1<<0),
    /** SBAS fast correction is used  */
    LOCATION_SBAS_CORRECTION_FAST_BIT  = (1<<1),
    /** SBAS long-tem correction is used  */
    LOCATION_SBAS_CORRECTION_LONG_BIT  = (1<<2),
    /** SBAS integrity information is used  */
    LOCATION_SBAS_INTEGRITY_BIT        = (1<<3),
    /** Position Report is DGNSS corrected  */
    LOCATION_NAV_CORRECTION_DGNSS_BIT  = (1<<4),
    /** Position Report is RTK corrected  */
    LOCATION_NAV_CORRECTION_RTK_BIT    = (1<<5),
    /** Position Report is PPP corrected  */
    LOCATION_NAV_CORRECTION_PPP_BIT    = (1<<6)
};

enum GnssLocationPosTechMask {
    LOCATION_POS_TECH_DEFAULT_BIT                  = 0,
    LOCATION_POS_TECH_SATELLITE_BIT                = (1<<0),
    LOCATION_POS_TECH_CELLID_BIT                   = (1<<1),
    LOCATION_POS_TECH_WIFI_BIT                     = (1<<2),
    LOCATION_POS_TECH_SENSORS_BIT                  = (1<<3),
    LOCATION_POS_TECH_REFERENCE_LOCATION_BIT       = (1<<4),
    LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT = (1<<5),
    LOCATION_POS_TECH_AFLT_BIT                     = (1<<6),
    LOCATION_POS_TECH_HYBRID_BIT                   = (1<<7),
    LOCATION_POS_TECH_PPE_BIT                      = (1<<8)
};

enum GnssLocationPosDataMask {
    /** Navigation data has Forward Acceleration  */
    LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT  = (1<<0),
    /** Navigation data has Sideward Acceleration */
    LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT   = (1<<1),
    /** Navigation data has Vertical Acceleration */
    LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT  = (1<<2),
    /** Navigation data has Heading Rate */
    LOCATION_NAV_DATA_HAS_YAW_RATE_BIT    = (1<<3),
    /** Navigation data has Body pitch */
    LOCATION_NAV_DATA_HAS_PITCH_BIT       = (1<<4),
    /** Navigation data has Forward Acceleration  */
    LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT  = (1<<5),
    /** Navigation data has Sideward Acceleration */
    LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT   = (1<<6),
    /** Navigation data has Vertical Acceleration */
    LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT  = (1<<7),
    /** Navigation data has Heading Rate */
    LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT    = (1<<8),
    /** Navigation data has Body pitch */
    LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT       = (1<<9)
};

/** GNSS Signal Type and RF Band */
enum GnssSignalTypeMask {
    /** GPS L1CA Signal */
    GNSS_SIGNAL_GPS_L1CA_BIT            = (1<<0),
    /** GPS L1C Signal */
    GNSS_SIGNAL_GPS_L1C_BIT             = (1<<1),
    /** GPS L2 RF Band */
    GNSS_SIGNAL_GPS_L2_BIT              = (1<<2),
    /** GPS L5 RF Band */
    GNSS_SIGNAL_GPS_L5_BIT              = (1<<3),
    /** GLONASS G1 (L1OF) RF Band */
    GNSS_SIGNAL_GLONASS_G1_BIT          = (1<<4),
    /** GLONASS G2 (L2OF) RF Band */
    GNSS_SIGNAL_GLONASS_G2_BIT          = (1<<5),
    /** GALILEO E1 RF Band */
    GNSS_SIGNAL_GALILEO_E1_BIT          = (1<<6),
    /** GALILEO E5A RF Band */
    GNSS_SIGNAL_GALILEO_E5A_BIT         = (1<<7),
    /** GALILEO E5B RF Band */
    GNSS_SIGNAL_GALILEO_E5B_BIT         = (1<<8),
    /** BEIDOU B1 RF Band */
    GNSS_SIGNAL_BEIDOU_B1_BIT           = (1<<9),
    /** BEIDOU B2 RF Band */
    GNSS_SIGNAL_BEIDOU_B2_BIT           = (1<<10),
    /** QZSS L1CA RF Band */
    GNSS_SIGNAL_QZSS_L1CA_BIT           = (1<<11),
    /** QZSS L1S RF Band */
    GNSS_SIGNAL_QZSS_L1S_BIT            = (1<<12),
    /** QZSS L2 RF Band */
    GNSS_SIGNAL_QZSS_L2_BIT             = (1<<13),
    /** QZSS L5 RF Band */
    GNSS_SIGNAL_QZSS_L5_BIT             = (1<<14),
    /** SBAS L1 RF Band */
    GNSS_SIGNAL_SBAS_L1_BIT             = (1<<15),
    /** BEIDOU B1I RF Band */
    GNSS_SIGNAL_BEIDOU_B1I_BIT          = (1<<16),
    /** BEIDOU B1C RF Band */
    GNSS_SIGNAL_BEIDOU_B1C_BIT          = (1<<17),
    /** BEIDOU B2I RF Band */
    GNSS_SIGNAL_BEIDOU_B2I_BIT          = (1<<18),
    /** BEIDOU B2AI RF Band */
    GNSS_SIGNAL_BEIDOU_B2AI_BIT         = (1<<19),
    /** NAVIC L5 RF Band */
    GNSS_SIGNAL_NAVIC_L5_BIT            = (1<<20),
    /** BEIDOU B2A_Q RF Band */
    GNSS_SIGNAL_BEIDOU_B2AQ_BIT         = (1<<21),
};


enum LocationResponse {
    LOCATION_RESPONSE_SUCCESS = 0,
    LOCATION_RESPONSE_UNKOWN_FAILURE,
    LOCATION_RESPONSE_NOT_SUPPORTED
};

enum GnssSvType {
    GNSS_SV_TYPE_UNKNOWN = 0,
    GNSS_SV_TYPE_GPS,
    GNSS_SV_TYPE_SBAS,
    GNSS_SV_TYPE_GLONASS,
    GNSS_SV_TYPE_QZSS,
    GNSS_SV_TYPE_BEIDOU,
    GNSS_SV_TYPE_GALILEO,
    GNSS_SV_TYPE_NAVIC
};

enum GnssLocationInfoFlagMask {
    /** valid altitude mean sea level */
    GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT      = (1<<0),
    /** valid pdop, hdop, and vdop */
    GNSS_LOCATION_INFO_DOP_BIT                          = (1<<1),
    /** valid magnetic deviation */
    GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT           = (1<<2),
    /** valid horizontal reliability */
    GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT              = (1<<3),
    /** valid vertical reliability */
    GNSS_LOCATION_INFO_VER_RELIABILITY_BIT              = (1<<4),
    /** valid elipsode semi major */
    GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT = (1<<5),
    /** valid elipsode semi minor */
    GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT = (1<<6),
    /** valid accuracy elipsode azimuth */
    GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT    = (1<<7),
    /** valid gnss sv used in pos data, including: svUsedInPosition
     *  and measUsageInfo */
    GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT            = (1<<8),
    /** valid navSolutionMask */
    GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT            = (1<<9),
    /** valid LocPosTechMask */
    GNSS_LOCATION_INFO_POS_TECH_MASK_BIT                = (1<<10),
    /** valid LocSvInfoSource */
    GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT               = (1<<11),
    /** valid position dynamics data */
    GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT            = (1<<12),
    /** valid gdop, tdop */
    GNSS_LOCATION_INFO_EXT_DOP_BIT                      = (1<<13),
    /**valid North standard deviation */
    GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT                = (1<<14),
    /** valid East standard deviation*/
    GNSS_LOCATION_INFO_EAST_STD_DEV_BIT                 = (1<<15),
    /** valid North Velocity */
    GNSS_LOCATION_INFO_NORTH_VEL_BIT                    = (1<<16),
    /** valid East Velocity */
    GNSS_LOCATION_INFO_EAST_VEL_BIT                     = (1<<17),
    /** valid Up Velocity */
    GNSS_LOCATION_INFO_UP_VEL_BIT                       = (1<<18),
    /** valid North Velocity Uncertainty */
    GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT                = (1<<19),
    /** valid East Velocity Uncertainty */
    GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT                 = (1<<20),
    /** valid Up Velocity Uncertainty */
    GNSS_LOCATION_INFO_UP_VEL_UNC_BIT                   = (1<<21),
    /** valid leap_seconds */
    GNSS_LOCATION_INFO_LEAP_SECONDS_BIT                 = (1<<22),
    /** valid timeUncMs */
    GNSS_LOCATION_INFO_TIME_UNC_BIT                     = (1<<23),
    /** valid numSvUsedInPosition */
    GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT      = (1<<24),
    /** valid sensor calibrationConfidencePercent */
    GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_PERCENT_BIT = (1<<25),
    /** valid sensor calibrationConfidence */
    GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT           = (1<<26),
    /** valid output engine type */
    GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT              = (1<<27),
    /** valid output engine mask */
    GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT              = (1<<28),
};

enum LocationReliability {
    LOCATION_RELIABILITY_NOT_SET = 0,
    LOCATION_RELIABILITY_VERY_LOW,
    LOCATION_RELIABILITY_LOW,
    LOCATION_RELIABILITY_MEDIUM,
    LOCATION_RELIABILITY_HIGH
};

enum Gnss_LocSvSystemEnumType {
    /** GPS satellite. */
    GNSS_LOC_SV_SYSTEM_GPS                    = 1,
    /** GALILEO satellite. */
    GNSS_LOC_SV_SYSTEM_GALILEO                = 2,
    /** SBAS satellite. */
    GNSS_LOC_SV_SYSTEM_SBAS                   = 3,
    /** GLONASS satellite. */
    GNSS_LOC_SV_SYSTEM_GLONASS                = 4,
    /** BDS satellite. */
    GNSS_LOC_SV_SYSTEM_BDS                    = 5,
    /** QZSS satellite. */
    GNSS_LOC_SV_SYSTEM_QZSS                   = 6,
    /** NAVIC satellite. */
    GNSS_LOC_SV_SYSTEM_NAVIC                  = 7
};

enum GnssSystemTimeStructTypeFlags {
    GNSS_SYSTEM_TIME_WEEK_VALID             = (1 << 0),
    GNSS_SYSTEM_TIME_WEEK_MS_VALID          = (1 << 1),
    GNSS_SYSTEM_CLK_TIME_BIAS_VALID         = (1 << 2),
    GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID     = (1 << 3),
    GNSS_SYSTEM_REF_FCOUNT_VALID            = (1 << 4),
    GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID      = (1 << 5)
};

enum GnssGloTimeStructTypeFlags {
    GNSS_CLO_DAYS_VALID                     = (1 << 0),
    GNSS_GLO_MSEC_VALID                     = (1 << 1),
    GNSS_GLO_CLK_TIME_BIAS_VALID            = (1 << 2),
    GNSS_GLO_CLK_TIME_BIAS_UNC_VALID        = (1 << 3),
    GNSS_GLO_REF_FCOUNT_VALID               = (1 << 4),
    GNSS_GLO_NUM_CLOCK_RESETS_VALID         = (1 << 5),
    GNSS_GLO_FOUR_YEAR_VALID                = (1 << 6)
};

enum BatchingStatus {
    BATCHING_STATUS_INACTIVE    = 0,
    BATCHING_STATUS_ACTIVE      = 1,
    BATCHING_STATUS_DONE        = 2
};

enum GeofenceBreachTypeMask {
    GEOFENCE_BREACH_ENTER_BIT     = (1<<0),
    GEOFENCE_BREACH_EXIT_BIT      = (1<<1),
    GEOFENCE_BREACH_DWELL_IN_BIT  = (1<<2),
    GEOFENCE_BREACH_DWELL_OUT_BIT = (1<<3),
};

enum DrCalibrationStatusMask {
    /** Indicate that roll calibration is needed. Need to take more
     *  turns on level ground */
    DR_ROLL_CALIBRATION_NEEDED  = (1<<0),
    /** Indicate that pitch calibration is needed. Need to take more
     *  turns on level ground */
    DR_PITCH_CALIBRATION_NEEDED = (1<<1),
    /** Indicate that yaw calibration is needed. Need to accelerate
     *  in a straight line  */
    DR_YAW_CALIBRATION_NEEDED   = (1<<2),
    /** Indicate that odo calibration is needed. Need to accelerate
     *  in a straight line  */
    DR_ODO_CALIBRATION_NEEDED   = (1<<3),
    /** Indicate that gyro calibration is needed. Need to take more
     *  turns on level ground */
    DR_GYRO_CALIBRATION_NEEDED  = (1<<4)
};

struct GnssLocationSvUsedInPosition {
    uint64_t gpsSvUsedIdsMask;
    uint64_t gloSvUsedIdsMask;
    uint64_t galSvUsedIdsMask;
    uint64_t bdsSvUsedIdsMask;
    uint64_t qzssSvUsedIdsMask;
    uint64_t navicSvUsedIdsMask;
};

struct GnssMeasUsageInfo {
    /** GnssSignalType mask */
    GnssSignalTypeMask gnssSignalType;
   /** Specifies GNSS Constellation Type */
    Gnss_LocSvSystemEnumType gnssConstellation;
    /**  GNSS SV ID.
     For GPS:      1 to 32
     For GLONASS:  65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
     For SBAS:     120 to 151
     For QZSS-L1CA:193 to 197
     For BDS:      201 to 237
     For GAL:      301 to 336 */
    uint16_t gnssSvId;
};

struct GnssLocationPositionDynamics {
    /** Contains Body frame LocPosDataMask bits. */
    GnssLocationPosDataMask  bodyFrameDataMask;
    /** Forward Acceleration in body frame (m/s2)*/
    float           longAccel;
    /** Sideward Acceleration in body frame (m/s2)*/
    float           latAccel;
    /** Vertical Acceleration in body frame (m/s2)*/
    float           vertAccel;
    /** Heading Rate (Radians/second) */
    float           yawRate;
    /** Body pitch (Radians) */
    float           pitch;
    /** Uncertainty of Forward Acceleration in body frame */
    float           longAccelUnc;
    /** Uncertainty of Side-ward Acceleration in body frame */
    float           latAccelUnc;
    /** Uncertainty of Vertical Acceleration in body frame */
    float           vertAccelUnc;
    /** Uncertainty of Heading Rate */
    float           yawRateUnc;
    /** Uncertainty of Body pitch */
    float           pitchUnc;
};

struct GnssSystemTimeStructType {
    /** Validity mask for below fields */
    GnssSystemTimeStructTypeFlags validityMask;
    /** Extended week number at reference tick.
    Unit: Week.
    Set to 65535 if week number is unknown.
    For GPS:
      Calculated from midnight, Jan. 6, 1980.
      OTA decoded 10 bit GPS week is extended to map between:
      [NV6264 to (NV6264 + 1023)].
      NV6264: Minimum GPS week number configuration.
      Default value of NV6264: 1738
    For BDS:
      Calculated from 00:00:00 on January 1, 2006 of Coordinated Universal Time (UTC).
    For GAL:
      Calculated from 00:00 UT on Sunday August 22, 1999
      (midnight between August 21 and August 22).
   */
    uint16_t systemWeek;
    /** Time in to the current week at reference tick.
       Unit: Millisecond. Range: 0 to 604799999.
       Check for systemClkTimeUncMs before use */
    uint32_t systemMsec;
    /** System clock time bias (sub-millisecond)
        Units: Millisecond
        Note: System time (TOW Millisecond) = systemMsec - systemClkTimeBias.
        Check for systemClkTimeUncMs before use. */
    float systemClkTimeBias;
    /** Single sided maximum time bias uncertainty
        Units: Millisecond */
    float systemClkTimeUncMs;
    /** FCount (free running HW timer) value. Don't use for relative time purpose
         due to possible discontinuities.
         Unit: Millisecond */
    uint32_t refFCount;
    /** Number of clock resets/discontinuities detected,
        affecting the local hardware counter value. */
    uint32_t numClockResets;
};

struct GnssGloTimeStructType {
    /** GLONASS day number in four years. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 65535 */
    uint16_t gloDays;
    /** Validity mask for below fields */
    GnssGloTimeStructTypeFlags validityMask;
    /** GLONASS time of day in Millisecond. Refer to GLONASS ICD.
        Units: Millisecond
        Check for gloClkTimeUncMs before use */
    uint32_t gloMsec;
    /** GLONASS clock time bias (sub-millisecond)
        Units: Millisecond
        Note: GLO time (TOD Millisecond) = gloMsec - gloClkTimeBias.
        Check for gloClkTimeUncMs before use. */
    float gloClkTimeBias;
    /** Single sided maximum time bias uncertainty
        Units: Millisecond */
    float gloClkTimeUncMs;
    /** FCount (free running HW timer) value. Don't use for relative time purpose
        due to possible discontinuities.
        Unit: Millisecond */
    uint32_t  refFCount;
    /** Number of clock resets/discontinuities detected,
        affecting the local hardware counter value. */
    uint32_t numClockResets;
    /** GLONASS four year number from 1996. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 255 */
    uint8_t gloFourYear;
};

union SystemTimeStructUnion {
    GnssSystemTimeStructType gpsSystemTime;
    GnssSystemTimeStructType galSystemTime;
    GnssSystemTimeStructType bdsSystemTime;
    GnssSystemTimeStructType qzssSystemTime;
    GnssGloTimeStructType gloSystemTime;
    GnssSystemTimeStructType navicSystemTime;
};

/** @struct
    Time applicability of PVT report
*/
struct GnssSystemTime {
    /** Specifies GNSS system time reported. Mandatory field */
    Gnss_LocSvSystemEnumType gnssSystemTimeSrc;
    /** Reporting of GPS system time is recommended.
      If GPS time is unknown & other satellite system time is known,
      it should be reported.
      Mandatory field
     */
    SystemTimeStructUnion u;
};

typedef enum {
    /** Mask to indicate that the fused/default position is needed
      to be reported back via EngineLocationsCb for the tracking sessions.
      The default position is the propagated/aggregated reports from
      all engines running on the system (e.g.: DR/SPE/PPE).
    */
    LOC_REQ_ENGINE_FUSED_BIT = (1<<0),
    /** Mask to indicate that the unmodified SPE position is needed
      to be reported back via EngineLocationsCb for the
      tracking sessions.
    */
    LOC_REQ_ENGINE_SPE_BIT   = (1<<1),
    /** Mask to indicate that the unmodified PPE position is needed
      to be reported back via EngineLocationsCb for the
      tracking sessions.
    */
    LOC_REQ_ENGINE_PPE_BIT   = (1<<2),
} LocReqEngineTypeMask;

typedef enum {
    /** This is the propagated/aggregated reports from all engines
        running on the system (e.g.: DR/SPE/PPE). */
    LOC_OUTPUT_ENGINE_FUSED = 0,
    /** This fix is the unmodified fix from modem GNSS engine */
    LOC_OUTPUT_ENGINE_SPE   = 1,
    /** This is the unmodified fix from PPP/RTK correction engine */
    LOC_OUTPUT_ENGINE_PPE   = 2,
    LOC_OUTPUT_ENGINE_COUNT,
} LocOutputEngineType;

typedef uint32_t PositioningEngineMask;
typedef enum {
    STANDARD_POSITIONING_ENGINE = (1 << 0),
    DEAD_RECKONING_ENGINE       = (1 << 1),
    PRECISE_POSITIONING_ENGINE  = (1 << 2)
} PositioningEngineBits;

struct Location {
    /** bitwise OR of LocationFlagsBits to mark which params are valid */
    LocationFlagsMask flags;
    /** UTC timestamp for location fix, milliseconds since January 1, 1970 */
    uint64_t timestamp;
    /** in degrees; range [-90.0, 90.0] */
    double latitude;
    /** in degrees; range [-180.0, 180.0] */
    double longitude;
    /** in meters above the WGS 84 reference ellipsoid */
    double altitude;
    /** in meters per second */
    float speed;
    /** in degrees; range [0, 360) */
    float bearing;
    /** in meters */
    float horizontalAccuracy;
    /** in meters */
    float verticalAccuracy;
    /** in meters/second */
    float speedAccuracy;
    /** in degrees (0 to 359.999) */
    float bearingAccuracy;
    LocationTechnologyMask techMask;
};

struct GnssLocation : public Location {
    /** bitwise OR of GnssLocationInfoBits for param validity */
    GnssLocationInfoFlagMask gnssInfoFlags;
    /** altitude wrt mean sea level */
    float altitudeMeanSeaLevel;
    /** position dilusion of precision */
    float pdop;
    /** horizontal dilusion of precision */
    float hdop;
    /** vertical dilusion of precision */
    float vdop;
    /** geometric  dilution of precision */
    float gdop;
    /** time dilution of precision */
    float tdop;
    /** magnetic deviation */
    float magneticDeviation;
    /** horizontal reliability */
    LocationReliability horReliability;
    /** vertical reliability */
    LocationReliability verReliability;
    /** horizontal elliptical accuracy semi-major axis */
    float horUncEllipseSemiMajor;
    /** horizontal elliptical accuracy semi-minor axis */
    float horUncEllipseSemiMinor;
    /** horizontal elliptical accuracy azimuth */
    float horUncEllipseOrientAzimuth;
    /** North standard deviation Unit: Meters */
    float northStdDeviation;
    /** East standard deviation. Unit: Meters */
    float eastStdDeviation;
    /** North Velocity.Unit: Meters/sec */
    float northVelocity;
    /** East Velocity  Unit: Meters/sec */
    float eastVelocity;
    /** Up Velocity  Unit: Meters/sec */
    float upVelocity;
    float northVelocityStdDeviation;
    float eastVelocityStdDeviation;
    float upVelocityStdDeviation;
    /** number of SV used in position report */
    uint16_t numSvUsedInPosition;
    /** Gnss sv used in position data */
    GnssLocationSvUsedInPosition svUsedInPosition;
    /** Nav solution mask to indicate sbas corrections */
    GnssLocationNavSolutionMask  navSolutionMask;
    /** Position technology used in computing this fix */
    GnssLocationPosTechMask      posTechMask;
    /** Body Frame Dynamics: 4wayAcceleration and pitch set with validity */
    GnssLocationPositionDynamics bodyFrameData;
    /**  GNSS System Time */
    GnssSystemTime               gnssSystemTime;
    /** Gnss measurement usage info  */
    std::vector<GnssMeasUsageInfo> measUsageInfo;
    /** Leap Seconds */
    uint8_t leapSeconds;
    /** Time uncertainty in milliseconds   */
    float timeUncMs;
    /** Sensor calibration confidence percent, range [0, 100] */
    uint8_t calibrationConfidencePercent;
    /** sensor calibration status  */
    DrCalibrationStatusMask calibrationStatus;
    /** location engine type. When the fix. when the type is set to
        LOC_ENGINE_SRC_FUSED, the fix is the propagated/aggregated
        reports from all engines running on the system (e.g.:
        DR/SPE/PPE). To check which location engine contributes to
        the fused output, check for locOutputEngMask. */
    LocOutputEngineType locOutputEngType;
    /** when loc output eng type is set to fused, this field
        indicates the set of engines contribute to the fix. */
    PositioningEngineMask locOutputEngMask;

    /* Default constructor to initalize GnssLocation structure*/
    inline GnssLocation() :
            Location({}), gnssInfoFlags((GnssLocationInfoFlagMask)0),
            altitudeMeanSeaLevel(0.0f), pdop(0.0f), hdop(0.0f),
            vdop(0.0f), gdop(0.0f), tdop(0.0f), magneticDeviation(0.0f),
            horReliability(LOCATION_RELIABILITY_NOT_SET),
            verReliability(LOCATION_RELIABILITY_NOT_SET),
            horUncEllipseSemiMajor(0.0f), horUncEllipseSemiMinor(0.0f),
            horUncEllipseOrientAzimuth(0.0f), northStdDeviation(0.0f),
            eastStdDeviation(0.0f), northVelocity(0.0f), eastVelocity(0.0f),
            upVelocity(0.0f), northVelocityStdDeviation(0.0f),
            eastVelocityStdDeviation(0.0f), upVelocityStdDeviation(0.0f),
            numSvUsedInPosition(0), svUsedInPosition({}),
            navSolutionMask((GnssLocationNavSolutionMask)0),
            posTechMask((GnssLocationPosTechMask)0), bodyFrameData({}),
            gnssSystemTime({}), measUsageInfo(), leapSeconds(0),
            timeUncMs(0.0f), calibrationConfidencePercent(0),
            calibrationStatus((DrCalibrationStatusMask)0),
            locOutputEngType ((LocOutputEngineType)0),
            locOutputEngMask((PositioningEngineMask)0){
    }
};

struct GnssSv {
    /** Unique Identifier */
    uint16_t svId;
    /** type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO) */
    GnssSvType type;
    /** signal strength */
    float cN0Dbhz;
    /** elevation of SV (in degrees) */
    float elevation;
    /** azimuth of SV (in degrees) */
    float azimuth;
    /** Bitwise OR of GnssSvOptionsBits */
    GnssSvOptionsMask gnssSvOptionsMask;
    /** carrier frequency of the signal tracked */
    float carrierFrequencyHz;
    /** Specifies GNSS signal type */
    GnssSignalTypeMask gnssSignalTypeMask;
};

enum GnssSignalTypes {
    GNSS_SIGNAL_TYPE_GPS_L1CA = 0,          /**<  GPS L1CA Signal  */
    GNSS_SIGNAL_TYPE_GPS_L1C = 1,           /**<  GPS L1C Signal  */
    GNSS_SIGNAL_TYPE_GPS_L2C_L = 2,         /**<  GPS L2C_L RF Band  */
    GNSS_SIGNAL_TYPE_GPS_L5_Q = 3,          /**<  GPS L5_Q RF Band  */
    GNSS_SIGNAL_TYPE_GLONASS_G1 = 4,        /**<  GLONASS G1 (L1OF) RF Band  */
    GNSS_SIGNAL_TYPE_GLONASS_G2 = 5,        /**<  GLONASS G2 (L2OF) RF Band  */
    GNSS_SIGNAL_TYPE_GALILEO_E1_C = 6,      /**<  GALILEO E1_C RF Band  */
    GNSS_SIGNAL_TYPE_GALILEO_E5A_Q = 7,     /**<  GALILEO E5A_Q RF Band  */
    GNSS_SIGNAL_TYPE_GALILEO_E5B_Q = 8,     /**<  GALILEO E5B_Q RF Band  */
    GNSS_SIGNAL_TYPE_BEIDOU_B1_I = 9,       /**<  BEIDOU B1_I RF Band  */
    GNSS_SIGNAL_TYPE_BEIDOU_B1C = 10,       /**<  BEIDOU B1C RF Band  */
    GNSS_SIGNAL_TYPE_BEIDOU_B2_I = 11,      /**<  BEIDOU B2_I RF Band  */
    GNSS_SIGNAL_TYPE_BEIDOU_B2A_I = 12,     /**<  BEIDOU B2A_I RF Band  */
    GNSS_SIGNAL_TYPE_QZSS_L1CA = 13,        /**<  QZSS L1CA RF Band  */
    GNSS_SIGNAL_TYPE_QZSS_L1S = 14,         /**<  QZSS L1S RF Band  */
    GNSS_SIGNAL_TYPE_QZSS_L2C_L = 15,       /**<  QZSS L2C_L RF Band  */
    GNSS_SIGNAL_TYPE_QZSS_L5_Q = 16,        /**<  QZSS L5_Q RF Band  */
    GNSS_SIGNAL_TYPE_SBAS_L1_CA = 17,       /**<  SBAS L1_CA RF Band  */
    GNSS_SIGNAL_TYPE_NAVIC_L5 = 18,         /**<  NAVIC L5 RF Band */
    GNSS_SIGNAL_TYPE_BEIDOU_B2A_Q = 19,     /**<  BEIDOU B2A_Q RF Band  */
    GNSS_MAX_NUMBER_OF_SIGNAL_TYPES = 20    /**< Maximum number of signal types */
};

typedef uint64_t GnssDataMask;

enum GnssDataBits {
    /** Jammer Indicator is available */
    GNSS_DATA_JAMMER_IND_BIT = (1ULL << 0),
    /** AGC is available */
    GNSS_DATA_AGC_BIT = (1ULL << 1)
};

struct GnssData {
    /** bitwise OR of GnssDataBits */
    GnssDataMask  gnssDataMask[GNSS_MAX_NUMBER_OF_SIGNAL_TYPES];
    /** Jammer Indication */
    double        jammerInd[GNSS_MAX_NUMBER_OF_SIGNAL_TYPES];
    /** Automatic gain control */
    double        agc[GNSS_MAX_NUMBER_OF_SIGNAL_TYPES];
};

typedef uint32_t LeapSecondSysInfoMask;
enum LeapSecondSysInfoDataBits{
    /** Current leap second info is available. This info will only
      be available if the leap second change info is not available
      If leap second change info is avaiable, to figure out the
      current leap second info, compare current gps time with the
      gps timestamp of leap second change to know whether to choose
      leapSecondBefore or leapSecondAfter as current leap second. */
    LEAP_SECOND_SYS_INFO_CURRENT_LEAP_SECONDS_BIT = (1ULL << 0),
    /** the last known leap change event is available.
        The info can be available on two scenario:
        1: this leap second change event has been scheduled and yet
           to happen
        2: this leap second change event has already happened and
           next leap second change event has not yet been scheduled.
    */
    LEAP_SECOND_SYS_INFO_LEAP_SECOND_CHANGE_BIT = (1ULL << 1)
};

struct LeapSecondChangeInfo {
    /** GPS timestamp that corrresponds to the last known leap
        second change event.
        The info can be available on two scenario:
        1: this leap second change event has been scheduled and yet
           to happen
        2: this leap second change event has already happened and
           next leap second change event has not yet been
           scheduled. */
    GnssSystemTimeStructType gpsTimestampLsChange;
    /** Number of leap seconds prior to the leap second change event
      that corresponds to the timestamp at gpsTimestampLsChange. */
    uint8_t leapSecondsBeforeChange;
    /** Number of leap seconds after the leap second change event
      that corresponds to the timestamp at gpsTimestampLsChange. */
    uint8_t leapSecondsAfterChange;
};

struct LeapSecondSystemInfo {
    LeapSecondSysInfoMask leapSecondInfoMask;
    uint8_t               leapSecondCurrent;
    LeapSecondChangeInfo  leapSecondChangeInfo;
};

typedef uint32_t LocationSystemInfoMask;
enum LocationSystemInfoDataBits{
    /** contains current leap second or leap second change info */
    LOCATION_SYS_INFO_LEAP_SECOND = (1ULL << 0),
};

struct LocationSystemInfo {
    LocationSystemInfoMask systemInfoMask;
    LeapSecondSystemInfo   leapSecondSysInfo;
};

class GeofenceImpl;
class LocationClientApi;
class Geofence {
    friend class GeofenceImpl;
    friend class LocationClientApi;
    std::shared_ptr<GeofenceImpl> mGeofenceImpl;
    double mLatitude;
    double mLongitude;
    double mRadius;
    GeofenceBreachTypeMask mBreachType;
    uint32_t mResponsiveness;
    uint32_t mDwellTime;
public:
    virtual ~Geofence() {}
    inline Geofence(double lat, double lon, double r, GeofenceBreachTypeMask type,
            uint32_t responsiveness, uint32_t time) :
            mGeofenceImpl(nullptr), mLatitude(lat), mLongitude(lon), mRadius(r),
            mBreachType(type), mResponsiveness(responsiveness), mDwellTime(time) {}
    inline Geofence(const Geofence& geofence): mGeofenceImpl(geofence.mGeofenceImpl),
            mLatitude(geofence.mLatitude), mLongitude(geofence.mLongitude),
            mRadius(geofence.mRadius), mBreachType(geofence.mBreachType),
            mResponsiveness(geofence.mResponsiveness), mDwellTime(geofence.mDwellTime) {}
    // clients who extends Geofence class should implement this clone method, so that its own
    // data stashed in the object gets copied to the new object, if desired so.
    virtual Geofence* clone() { return new Geofence(*this); }
    bool operator==(Geofence& other);
    inline double getLatitude() const { return mLatitude; }
    inline double getLongitude() const { return mLongitude; }
    inline double getRadius() const { return mRadius; }
    inline GeofenceBreachTypeMask getBreachType() const { return mBreachType; }
    inline uint32_t getResponsiveness() const { return mResponsiveness; }
    inline uint32_t getDwellTime() const { return mDwellTime; }
    inline void setBreachType(GeofenceBreachTypeMask type) { mBreachType = type; }
    inline void setResponsiveness(uint32_t responsiveness) { mResponsiveness = responsiveness; }
    inline void setDwellTime(uint32_t time) { mDwellTime = time; }
};

/** @fn
    @brief Provides the capabilities of the system,

    @param capsMask: bitwise OR of LocationCapabilitiesBits
*/
typedef std::function<void(
    LocationCapabilitiesMask capsMask
)> CapabilitiesCb;

/** @fn
    @brief Used by positioning, batching, and miscellanous APIs

    @param response: if not LOCATION_RESPONSE_SUCCESS, then the last API called failed
*/
typedef std::function<void(
    LocationResponse response
)> ResponseCb;

/** @fn
    @brief Used by geofence APIs

    @param responses: include Geofence objects and correponding responses.
*/
typedef std::function<void(
    std::vector<std::pair<Geofence, LocationResponse>>& responses
)> CollectiveResponseCb;

/** @fn
    @brief
    LocationCb is for receiving a location in a positioning session
    @param location: received location
*/
typedef std::function<void(
    const Location& location
)> LocationCb;

/** @fn
    @brief
    GnssLocationCb is for receiving a GNSS location in a positioning session
    @param gnssLocation: received GNSS Location when positioning
*/
typedef std::function<void(
    const GnssLocation& gnssLocation
)> GnssLocationCb;

/** @fn
    @brief
    EngineLocationsCb is for receiving registered locations from
    multiple engines in a positioning session
*/
typedef std::function<void(
    const std::vector<GnssLocation> & engLocations
)> EngineLocationsCb;


/** @fn
    @brief
    GnssNmeaCb is for receiving GNSS SV information
    @param gnssSvs: received GNSS SV info
*/
typedef std::function<void(
    const std::vector<GnssSv>& gnssSvs
)> GnssSvCb;

/** @fn
    @brief
    GnssNmeaCb is for receiving NMEA sentences
    @param locations: the locations batched in a session
*/
typedef std::function<void(
    uint64_t timestamp, const std::string& nmea
)> GnssNmeaCb;

/** @fn
    @brief
    GnssDataCb is for receiving GnssData information
    @param gnssData: the GnssData info in a session
*/
typedef std::function<void(
    const GnssData& gnssData
)> GnssDataCb;

/** @fn
    @brief
    LocationSystemInfoCb is for receiving rare occuring location
    system information update
    @param locationSystemInfo: rare location system event, e.g.:
           leap second change
*/
typedef std::function<void(
    const LocationSystemInfo & locationSystemInfo
)> LocationSystemInfoCb;

/** @fn
    @brief
    BatchingCb is for delivering locations in a batching session
    @param locations: the locations batched in a session
    @param batchStatus: BatchingStatus of the batching session, which is
        BATCHING_STATUS_INACTIVE when unable to compute positions for batching;
        BATCHING_STATUS_DONE when trip distance has been traversed for tripBatching;
        BATCHING_STATUS_ACTIVE when able to compute positions for batching.
*/
typedef std::function<void(
    const std::vector<Location>& locations,
    BatchingStatus batchStatus
)> BatchingCb;

/** @fn
    @brief
    GeofenceBreachCb is for receiving geofences that have a state change
    @param geofences: array of geofence objects that have breached
    @param location: location associated with breach
    @param type: type of breach
    @param timestamp: timestamp of breach
*/
typedef std::function<void(
    const std::vector<Geofence>& geofences,
    Location location,
    GeofenceBreachTypeMask type,
    uint64_t timestamp
)> GeofenceBreachCb;

struct GnssReportCbs {
    GnssLocationCb gnssLocationCallback;
    GnssSvCb gnssSvCallback;
    GnssNmeaCb gnssNmeaCallback;
    GnssDataCb gnssDataCallback;
};

struct EngineReportCbs {
    EngineLocationsCb engLocationsCallback;
    GnssSvCb gnssSvCallback;
    GnssNmeaCb gnssNmeaCallback;
    GnssDataCb gnssDataCallback;
};

enum GnssEnergyConsumedInfoMask {
    /** total energy consumed since device first ever boot */
    ENERGY_CONSUMED_SINCE_FIRST_BOOT_BIT = (1<<0),
};

struct GnssEnergyConsumedInfo {
    GnssEnergyConsumedInfoMask flags;

    // Energy consumed by the GNSS engine since bootup
    // in units of 0.1 milli watt seconds
    uint64_t totalEnergyConsumedSinceFirstBoot;
};

/** @fn
    @brief Used by query API that retrieves energy consumed by
           modem GNSS engine.

    @param gnssEneryConsumed: check for the flags for valid info returned.
*/
typedef std::function<void(
    const GnssEnergyConsumedInfo& gnssEneryConsumed
)> GnssEnergyConsumedCb;

struct ClientCallbacks {
    CapabilitiesCb capabilitycb;
    ResponseCb responsecb;
    CollectiveResponseCb collectivecb;
    LocationCb locationcb;
    BatchingCb batchingcb;
    GeofenceBreachCb gfbreachcb;
    GnssReportCbs gnssreportcbs;
    EngineReportCbs engreportcbs;
};

class LocationClientApiImpl;

class LocationClientApi
{
public:
    /** @brief Creates an instance of LocationClientApi object. If capsCallback is not
        nullptr, a callback would be made via it to report the capabilities of the
        underlying system.

        @param
        capsCallback, can be nullptr.  Otherwise LocationCapabilitiesMask will be reported
                      shortly after the construction of the object.
    */
    LocationClientApi(CapabilitiesCb capsCallback);

    /** @brief Default destructor */
    virtual ~LocationClientApi();

    /* ================================== POSITIONING ================================== */

    /** @brief Starts a session with specified parameters. The behavior of the call is
        non contextual. The current state or the history of actions does not influence
        the end result of this call. For example, calling this function when idle, or
        calling this function after another startPositionSession(), either one of the
        two, or calling this function after stopPositionSession() achieve the same
        result, which is one of the below:

        If locationCallback is nullptr, this call is no op. Otherwise...
        If both intervalInMs and distanceInMeters are don't care, this call is no op.
           Otherwise...
        If called during a session (no matter from which startPositionSession() API),
           parameters / callback will be updated, and the session continues but with
           the new set of parameters / callback.

        @param intervalInMs
        time between fixes, or TBF, in milliseconds. The actual interval
        of reports recieved will be no larger than milliseconds being
        rounded up the next interval granularity supported by the underlying
        system.
        0 to indicate don't care.

        When distanceInMeters is set to none zero, intervalInMs indicates
        the max latency that position report should be reported after the
        min distance criteria has been met. For example device has been
        static, at UTC time of x millisecond, the device starts to
        move, at UTC time of y milliseconds, the device has moved by
        distanceInMeters. Then the location API client shall
        expect to get a fix no later at UTC time of y+intervalInMs
        milli-seconds.

        1)  The underlying system may have a minimum interval threshold
        (e.g. 100 ms or 1000 ms). Effective intervals will not be smaller
        than this lower bound.
        2) The effective intervals may have a granularity level higher
        than 1 ms, e.g. 100 ms or 1000 ms. So milliseconds being 1559
        may be honored at 1600 or 2000 ms, depending on the system.
        3) Where there is anotehr application in they system having a
        session with shorter interval, this client may benefit and
        receive reports at that interval.
        @param distanceInMeters
        distance between fixes, in meters. 0 to indicate don't care.
        1)  The underlying system may have a minimum distance threshold
        (e.g. 1 meter). Effective distance will not be smaller
        than this lower bound.
        2) The effective distance may have a granularity level higher
        than 1 m, e.g. 5 m. So distanceInMeters being 59 may be honored
        at 60 m, depending on the system.
        3) Where there is anotehr application in they system having a
        session with shorter distance, this client may benefit and
        receive reports at that distance.
        @param locationCallback
        callback to receive positions
        @param responseCallback
        callback to receive system responses; optional.

        @return True, if a session is successfully started.
                False, if no session is started, i.e. when locationCallback is nullptr.
    */
    bool startPositionSession(uint32_t intervalInMs, uint32_t distanceInMeters,
                              LocationCb locationCallback, ResponseCb responseCallback);

    /** @brief Starts a session which may provide richer GNSS reports. The behavior of
        the call is non contextual. The current state or the history of actions does
        not influence the end result of this call. For example, calling this function
        when idle, or calling this function after another startPositionSession(),
        either one of the two, or calling this function after stopPositionSession()
        achieve the same result, which is one of the below:

        If gnssReportCallbacks is populated with nullptr's only, this call is no op.
           Otherwise...
        If intervalInMs is don't care, this call is no op. Otherwise...
        If called during a session (no matter from which startPositionSession() API),
           parameters / callback will be updated, and the session continues but with
           the new set of parameters / callback.

        @param intervalInMs
        time between fixes, or TBF, in milliseconds. The actual interval
        of reports recieved will be no larger than milliseconds being
        rounded up the next interval granularity supported by the underlying
        system.
        0 to indicate don't care.
        1)  The underlying system may have a minimum interval threshold
        (e.g. 100 ms or 1000 ms). Effective intervals will not be smaller
        than this lower bound.
        2) The effective intervals may have a granularity level higher
        than 1 ms, e.g. 100 ms or 1000 ms. So milliseconds being 1559
        may be honored at 1600 or 2000 ms, depending on the system.
        3) Where there is anotehr application in they system having a
        session with shorter interval, this client may benefit and
        receive reports at that interval.
        @param gnssReportCallbacks
        table of callbacks to receive GNSS locations / SV / NMEA
        @param responseCallback
        callback to receive system responses; optional.

        @return True, if a session is successfully started.
                False, if no session is started, i.e. when gnssReportCallbacks is
                       populated with nullptr's only.
    */
    bool startPositionSession(uint32_t intervalInMs, const GnssReportCbs& gnssReportCallbacks,
                              ResponseCb responseCallback);

    /** @brief Starts a session which may provide richer default combined
        poisiton reports and position reports from other engines. The fused
        position report type will always be supported if at least
        one engine in the system is producing valid report. For
        example, if SPE is the only engine running in the system,
        both the fused and SPE reports will be available. If the
        requested engine is not supported or the position report
        from the requested engine is old (e.g.: default is 2
        seconds), then the requested engine position will not be
        reported.

        The behavior of the call is non contextual.
        The current state or the history of actions does not influence the
        end result of this call. For example, calling this function when idle,
        or calling this function after another startPositionSession(),
        either one of the two, or calling this function after stopPositionSession()
        achieve the same result, which is one of the below:

        If EngineReportCbs is populated with nullptr's only, this call is no op.
        If called during a session (no matter from which startPositionSession() API),
           parameters / callback will be updated, and the session continues but with
           the new set of parameters / callback.

        @param intervalInMs, time between fixes, or TBF, in milliseconds. The actual interval
                      of reports recieved will be no larger than milliseconds being
                      rounded up the next interval granularity supported by the underlying
                      system.
                      1)  The underlying system may have a minimum interval threshold
                      (e.g. 100 ms or 1000 ms). Effective intervals will not be smaller
                      than this lower bound.
                      2) The effective intervals may have a granularity level higher
                      than 1 ms, e.g. 100 ms or 1000 ms. So milliseconds being 1559
                      may be honored at 1600 or 2000 ms, depending on the system.
                      3) Where there is anotehr application in they system having a
                      session with shorter interval, this client may benefit and
                      receive reports at that interval.
        @param locReqEngineSrcMask, the bit maks of the location engines that client is interested
                       to receive  position report.
                       If LOC_REQ_ENGINE_FUSED_BIT is set, but SPE is the only positioning
                       engine running on the system, then the fused position will be the
                       unpropagated SPE report.
                       If LOC_REQ_ENGINE_FUSED_BIT is set, and there is at least one additional
                       engine running on the system other than SPE engine, then the fused position
                       will be the propagated report to current time.
                       The SPE and PPE report will be the original report from the position
                       engine without any modification.
                       If LOC_REQ_ENGINE_PPE_BIT is set, but PPE is not enabled on the system,
                       then PPE position will not be returned.

        @param engReportCallbacks, table of callbacks to receive engine locations / SV / NMEA
        @param responseCallback, callback to receive system responses; optional.

        @return True, if a session is successfully started.
                False, if no session is started, i.e. when gnssReportCallbacks is
                       populated with nullptr's only.
    */
    bool startPositionSession(uint32_t intervalInMs,
                              LocReqEngineTypeMask locReqEngMask,
                              const EngineReportCbs& engReportCallbacks,
                              ResponseCb responseCallback);

    /** @brief Stops the ongoing positioning session.
    */
    void stopPositionSession();

    /** @example example1:testTrackingApi
    * <pre>
    * <code>
    *    // Sample Code
    * static void onCapabilitiesCb(location_client::LocationCapabilitiesMask mask) {
    *     //...
    * }
    * static void onResponseCb(location_client::LocationResponse response) {
    *     //...
    * }
    * static void onGnssLocationCb(const location_client::GnssLocation& location) {
    *     //...
    * }
    *
    * static void onGnssSvCb(const std::vector<location_client::GnssSv>& gnssSvs) {
    *     //...
    * }
    *
    * static void onGnssNmeaCb(uint64_t timestamp, const std::string& nmea) {
    *     //...
    * }
    * void testTrackingApi() {
    *     LocationClientApi *pClient = new LocationClientApi(onCapabilitiesCb);
    *     if (nullptr == pClient) {
    *         LOC_LOGe("failed to create LocationClientApi instance");
    *         return;
    *     }
    *
    *     uint32_t option = 0x111;
    *     uint32_t interval = 1000;
    *     // set callbacks
    *     GnssReportCbs reportcbs;
    *     if (option & 1<<0) {
    *         reportcbs.gnssLocationCallback = GnssLocationCb(onGnssLocationCb);
    *     }
    *     if (option & 1<<1) {
    *         reportcbs.gnssSvCallback = GnssSvCb(onGnssSvCb);
    *     }
    *     if (option & 1<<2) {
    *         reportcbs.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb);
    *     }
    *
    *     // start tracking session
    *     pClient->startPositionSession(interval, reportcbs, onResponseCb);
    *     //...
    *     // stop session
    *     pClient->stopPositionSession();
    * </code>
    * </pre>
    */

    /* ================================== BATCHING ================================== */

    /** @brief starts an outdoor trip mode batching session with specified parameters.
        Trip mode batching completes on its own when trip distance is covered.
        The behavior of the call is non contextual. The current state or the history of
        actions does not influence the end result of this call. For example, calling
        this function when idle, or calling this function after another startTripBatchingSession()
        or startRoutineBatchingSession(), or calling this function after stopBatchingSession()
        achieve the same result, which is one of the below:
        If batchingCallback is nullptr, this call is no op. Otherwise...
        If both minInterval and tripDistance are don't care, this call is no op.
           Otherwise...
        If called during a session (no matter from which startTripBatchingSession()/
        startRoutineBatchingSession() API), parameters / callback will be updated,
        and the session continues but with the new set of parameters / callback.
        locations are reported on the batchingCallback in batches when batch is full.
        @param minInterval
        time between fixes, or TBF, in milliseconds. The actual interval
        of reports recieved will be no larger than milliseconds being
        rounded up the next interval granularity supported by the underlying
        system.
        0 to indicate don't care.
        1)  The underlying system may have a minimum interval threshold
        (e.g. 100 ms or 1000 ms). Effective intervals will not be smaller
        than this lower bound.
        2) The effective intervals may have a granularity level higher
        than 1 ms, e.g. 100 ms or 1000 ms. So milliseconds being 1559
        may be honored at 1600 or 2000 ms, depending on the system.
        3) Where there is anotehr application in they system having a
        session with shorter interval, this client may benefit and
        receive reports at that interval.
        @param tripDistance
        the trip distance from the start of outdoor trip batching; 0 means don't care
        @param batchingCallback
        callback to receive batching positions and status
        @param responseCallback
        callback to receive system responses; optional.
        @return True, if a batching session is successfully started.
                False, if no session is started, i.e. when batchingCallback is nullptr.
    */
    bool startTripBatchingSession(uint32_t minInterval, uint32_t tripDistance,
                                  BatchingCb batchingCallback, ResponseCb responseCallback);

    /** @brief starts a routine mode batching session with specified parameters.
        The behavior of the call is non contextual. The current state or the history of
        actions does not influence the end result of this call. For example, calling
        this function when idle, or calling this function after another startTripBatchingSession()
        or startRoutineBatchingSession(), or calling this function after stopBatchingSession()
        achieve the same result, which is one of the below:
        If batchingCallback is nullptr, this call is no op. Otherwise...
        If both minInterval and minDistance are don't care, this call is no op.
           Otherwise...
        If called during a session (no matter from which startTripBatchingSession()/
        startRoutineBatchingSession() API), parameters / callback will be updated,
        and the session continues but with the new set of parameters / callback.
        locations are reported on the batchingCallback in batches when batch is full.
        @param minInterval
        time between fixes, or TBF, in milliseconds. The actual interval
        of reports recieved will be no larger than milliseconds being
        rounded up the next interval granularity supported by the underlying
        system.
        0 to indicate don't care.
        1)  The underlying system may have a minimum interval threshold
        (e.g. 100 ms or 1000 ms). Effective intervals will not be smaller
        than this lower bound.
        2) The effective intervals may have a granularity level higher
        than 1 ms, e.g. 100 ms or 1000 ms. So milliseconds being 1559
        may be honored at 1600 or 2000 ms, depending on the system.
        3) Where there is anotehr application in they system having a
        session with shorter interval, this client may benefit and
        receive reports at that interval.
        @param minDistance
        specifies the minimum distance that should be traversed before a
        position should be batched.
        If 0, the positions are batched after the minInterval period expires.
        If both minInterval and minDistance are specified, the position are batched only after
        minInterval has expired AND minDistance has been traversed.
        - Units: Meters
        @param batchingCallback
        callback to receive batching positions and status
        @param responseCallback
        callback to receive system responses; optional.
        @return True, if a batching session is successfully started.
                False, if no session is started, i.e. when batchingCallback is nullptr.
    */
    bool startRoutineBatchingSession(uint32_t minInterval, uint32_t minDistance,
                                     BatchingCb batchingCallback, ResponseCb responseCallback);
    /** @brief Stops the batching session.
    */
    void stopBatchingSession();

    /** @example example2:testBatchingApi
    * <pre>
    * <code>
    *    // Sample Code
    * static void onBatchingCb(const std::vector<location_client::Location>& locations,
    *         location_client::BatchingStatus status) {
    *     //...
    * }
    * void testBatchingApi() {
    *     // batching session
    *     uint32_t intervalInMs = 0;
    *     uint32_t distanceInMeters = 0;
    *     pClient->startRoutineBatchingSession(
    *             intervalInMs, distanceInMeters, onBatchingCb, onResponseCb);
    *     // ...
    *     pClient->stopBatchingSession();
    * }
    * </code>
    * </pre>
    */

    /* ================================== Geofence ================================== */
    /** @brief Adds any number of geofences. The geofenceBreachCallback will
        deliver the status of each geofence according to the Geofence parameter for each.
        @param geofences
        Geofence objects, Once addGeofences returns, each Geofence object in the vector would
        be the identifier throughout the remaining communication of that geofence.
        Such a Geofence object can be copied or cloned, but they would all reference
        the same geofence.
        @param gfBreachCb
        callback to receive geofences state change. addGeofences is no op if gfBreachCb is null
        @param responseCallback
        callback to receive geofence ids and system responses; optional.
    */
    void addGeofences(std::vector<Geofence>& geofences, GeofenceBreachCb gfBreachCb,
                      CollectiveResponseCb responseCallback);

    /** @brief Removes any number of geofences.
        @param geofences
        Geofence objects, must be originally added to the system. Otherwise it would be no op.
    */
    void removeGeofences(std::vector<Geofence>& geofences);

    /** @brief Modifies any number of geofences.
        @param geofences
        Geofence objects, must be originally added to the system. Otherwise it would be no op.
        Modifiable fields include breachTypeMask, responsiveness and dwelltime.
        A geofence that has been added to the system may have these fields modified.
        But it is not going to take any effect until modifyGeofences is called with
        the changed geofence passed in.
    */
    void modifyGeofences(std::vector<Geofence>& geofences);

    /** @brief Pauses any number of geofences, which is similar to removeGeofences,
        only that they can be resumed at any time.
        @param geofences
        Geofence objects, must be originally added to the system. Otherwise it would be no op.
    */
    void pauseGeofences(std::vector<Geofence>& geofences);

    /** @brief Resumes any number of geofences that are currently paused.
        @param geofences
        Geofence objects, must be originally added to the system. Otherwise it would be no op.
    */
    void resumeGeofences(std::vector<Geofence>& geofences);

    /** @example example3:testGeofenceApi
    * <pre>
    * <code>
    *    // Sample Code
    *
    * vector<Geofence> sGeofences;
    * static void onGeofenceBreachCb( const std::vector<Geofence>& geofences,
    *         location_client::Location location, location_client::GeofenceBreachTypeMask type,
    *         uint64_t timestamp) {
    *     //...
    * }
    * static void onCollectiveResponseCb(std::vector<pair<Geofence, LocationResponse>>& responses) {
    *     //...
    * }
    *
    * void testGeofenceApi() {
    *     double latitude = 32.896535;
    *     double longitude = -117.201025;
    *     double radius = 50;
    *     GeofenceBreachTypeMask type = (GeofenceBreachTypeMask)3;
    *     uint32_t responsiveness = 4000;
    *     uint32_t time = 0;
    *     Geofence gf(latitude, longitude, radius, type, responsiveness, time);
    *     sGeofences.push_back(gf);
    *
    *     pClient->addGeofences(sGeofences, onGeofenceBreachCb, onCollectiveResponseCb);
    *     vector<Geofence> pauseGeofences;
    *     pauseGeofences.push_back(sGeofences[0]);
    *     pClient->pauseGeofences(pauseGeofences);
    *     vector<Geofence> resumeGeofences;
    *     resumeGeofences.push_back(sGeofences[0]);
    *     pClient->resumeGeofences(resumeGeofences);
    *     vector<Geofence> removeGeofences;
    *     for (int i=0; i<sGeofences.size(); ++i) {
    *         removeGeofences.push_back(sGeofences[i]);
    *     }
    *     pClient->removeGeofences(removeGeofences);
    * }
    * </code>
    * </pre>
    */

    /* ================================== Other APIs ================================== */
    /** @brief Updatee network availability.
        @param
        available, true if available, false otherwise
    */
    void updateNetworkAvailability(bool available);

    /** @brief get info related to energy consumed by modem GNSS
               engine. If called while the previous call is still
               being processed, then the callback will be updated,
               and the processing continues and info will be
               delivered via the new callbacks.

        @param
        gnssEnergyConsumedCallback, callback to receive energy consumed
            info.
        responseCallback, callback to receive system responses;
            optional.
    */
    void getGnssEnergyConsumed(GnssEnergyConsumedCb gnssEnergyConsumedCallback,
                               ResponseCb responseCallback);

    /** @brief Update the listener callback to receive info related
               to location system info that are not tied with
               location fix session, e.g.: next leap second event.
               One set of callbacks can be registered per location
               api client. The callback may be invoked multiple
               times to update same or different piece of system
               info.

        @param
        locSystemInfoCallback, callback to receive system info update.
            Pass a null callback to stop receiving the update.
        responseCallback, callback to receive system responses; optional.
    */
    void updateLocationSystemInfoListener(LocationSystemInfoCb locSystemInfoCallback,
                                          ResponseCb responseCallback);

private:
    LocationClientApiImpl* mApiImpl;
};

} // namespace location_client

#endif /* LOCATIONCLIENTAPI_H */
