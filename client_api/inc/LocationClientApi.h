/* Copyright (c) 2017-2018 The Linux Foundation. All rights reserved.
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

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>

namespace location_client
{

enum LocationCapabilitiesMask{
    /**< supports startPositionSession() with intervalInMs */
    LOCATION_CAPS_TIME_BASED_TRACKING_BIT           = (1<<0),
    /**< supports startBatching() with intervalInMs */
    LOCATION_CAPS_TIME_BASED_BATCHING_BIT           = (1<<1),
    /**< supports startPositionSession() with distanceInMeters */
    LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT       = (1<<2),
    /**< supports startBatching() with distanceInMeters */
    LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT       = (1<<3),
    /**< supports addGeofences API */
    LOCATION_CAPS_GEOFENCE_BIT                      = (1<<4),
    /**< support outdoor trip batching */
    LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT         = (1<<5)
};

enum GnssSvOptionsMask{
    GNSS_SV_OPTIONS_HAS_EPHEMER_BIT = (1<<0),
    GNSS_SV_OPTIONS_HAS_ALMANAC_BIT = (1<<1),
    GNSS_SV_OPTIONS_USED_IN_FIX_BIT = (1<<2)
};

enum LocationFlagsMask {
    LOCATION_HAS_LAT_LONG_BIT          = (1<<0), /**< location has valid latitude and longitude */
    LOCATION_HAS_ALTITUDE_BIT          = (1<<1), /**< location has valid altitude */
    LOCATION_HAS_SPEED_BIT             = (1<<2), /**< location has valid speed */
    LOCATION_HAS_BEARING_BIT           = (1<<3), /**< location has valid bearing */
    LOCATION_HAS_ACCURACY_BIT          = (1<<4), /**< location has valid accuracy */
    LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5), /**< location has valid vertical accuracy */
    LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6), /**< location has valid speed accuracy */
    LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7)  /**< location has valid bearing accuracy */
};

enum LocationTechnologyMask{
    LOCATION_TECHNOLOGY_GNSS_BIT     = (1<<0), /**< location was calculated using GNSS */
    LOCATION_TECHNOLOGY_CELL_BIT     = (1<<1), /**< location was calculated using Cell */
    LOCATION_TECHNOLOGY_WIFI_BIT     = (1<<2), /**< location was calculated using WiFi */
    LOCATION_TECHNOLOGY_SENSORS_BIT  = (1<<3)  /**< location was calculated using Sensors */
};

enum GnssLocationNavSolutionMask {
    LOCATION_SBAS_CORRECTION_IONO_BIT  = (1<<0),
    /**< SBAS ionospheric correction is used  */
    LOCATION_SBAS_CORRECTION_FAST_BIT  = (1<<1),
    /**< SBAS fast correction is used  */
    LOCATION_SBAS_CORRECTION_LONG_BIT  = (1<<2),
    /**< SBAS long-tem correction is used  */
    LOCATION_SBAS_INTEGRITY_BIT        = (1<<3),
    /**< SBAS integrity information is used  */
    LOCATION_NAV_CORRECTION_DGNSS_BIT  = (1<<4),
    /**< Position Report is DGNSS corrected  */
    LOCATION_NAV_CORRECTION_RTK_BIT    = (1<<5),
    /**< Position Report is RTK corrected  */
    LOCATION_NAV_CORRECTION_PPP_BIT    = (1<<6)
    /**< Position Report is PPP corrected  */
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
    /**< Navigation data has Forward Acceleration  */
    LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT  = (1<<0),
    /**< Navigation data has Sideward Acceleration */
    LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT   = (1<<1),
    /**< Navigation data has Vertical Acceleration */
    LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT  = (1<<2),
    /**< Navigation data has Heading Rate */
    LOCATION_NAV_DATA_HAS_YAW_RATE_BIT    = (1<<3),
    /**< Navigation data has Body pitch */
    LOCATION_NAV_DATA_HAS_PITCH_BIT       = (1<<4)
};

enum LocationResponse{
    LOCATION_RESPONSE_SUCCESS = 0,
    LOCATION_RESPONSE_UNKOWN_FAILURE,
    LOCATION_RESPONSE_GEOFENCES_AT_MAX,
    LOCATION_RESPONSE_NOT_SUPPORTED
};

enum GnssSvType{
    GNSS_SV_TYPE_UNKNOWN = 0,
    GNSS_SV_TYPE_GPS,
    GNSS_SV_TYPE_SBAS,
    GNSS_SV_TYPE_GLONASS,
    GNSS_SV_TYPE_QZSS,
    GNSS_SV_TYPE_BEIDOU,
    GNSS_SV_TYPE_GALILEO
};

enum GnssLocationInfoFlagMask{
    GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT      = (1<<0), /**< valid altitude mean sea level */
    GNSS_LOCATION_INFO_DOP_BIT                          = (1<<1), /**< valid pdop, hdop, and vdop */
    GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT           = (1<<2), /**< valid magnetic deviation */
    GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT              = (1<<3), /**< valid horizontal reliability */
    GNSS_LOCATION_INFO_VER_RELIABILITY_BIT              = (1<<4), /**< valid vertical reliability */
    GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT = (1<<5), /**< valid elipsode semi major */
    GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT = (1<<6), /**< valid elipsode semi minor */
    GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT    = (1<<7), /**< valid accuracy elipsode azimuth */
    GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT            = (1<<8), /**< valid gnss sv used in pos data */
    GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT            = (1<<9), /**< valid navSolutionMask */
    GNSS_LOCATION_INFO_POS_TECH_MASK_BIT                = (1<<10),/**< valid LocPosTechMask */
    GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT               = (1<<11),/**< valid LocSvInfoSource */
    GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT            = (1<<12),/**< valid position dynamics data */
    GNSS_LOCATION_INFO_GPS_TIME_BIT                     = (1<<13),/**< valid GPS Time */
    GNSS_LOCATION_INFO_EXT_DOP_BIT                      = (1<<14) /**< valid gdop, tdop */
};

enum LocationReliability {
    LOCATION_RELIABILITY_NOT_SET = 0,
    LOCATION_RELIABILITY_VERY_LOW,
    LOCATION_RELIABILITY_LOW,
    LOCATION_RELIABILITY_MEDIUM,
    LOCATION_RELIABILITY_HIGH,
};

enum GnssSystemTimeFlagsMask {
    GNSS_SYSTEM_TIME_WEEK_VALID         = (1 << 0),
    GNSS_SYSTEM_TIME_WEEK_MS_VALID      = (1 << 1),
    GNSS_SYSTEM_CLK_TIME_BIAS_VALID     = (1 << 2),
    GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID = (1 << 3),
    GNSS_SYSTEM_REF_FCOUNT_VALID        = (1 << 4),
    GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID  = (1 << 5)
};

enum GnssGloTimeFlagsMask {
    GNSS_GLOS_MSEC_VALID             = (1 << 0),
    GNSS_GLO_CLK_TIME_BIAS_VALID     = (1 << 1),
    GNSS_GLO_CLK_TIME_BIAS_UNC_VALID = (1 << 2),
    GNSS_GLO_REF_FCOUNT_VALID        = (1 << 3),
    GNSS_GLO_NUM_CLOCK_RESETS_VALID  = (1 << 4),
    GNSS_GLO_FOUR_YEAR_VALID         = (1 << 5)
};

struct GnssLocationSvUsedInPosition {
    uint64_t gpsSvUsedIdsMask;
    uint64_t gloSvUsedIdsMask;
    uint64_t galSvUsedIdsMask;
    uint64_t bdsSvUsedIdsMask;
    uint64_t qzssSvUsedIdsMask;
};

struct GnssLocationPositionDynamics {
    /**< Contains Body frame LocPosDataMask bits. */
    GnssLocationPosDataMask  bodyFrameDatamask;
    /**< Forward Acceleration in body frame (m/s2)*/
    float           longAccel;
    /**< Sideward Acceleration in body frame (m/s2)*/
    float           latAccel;
    /**< Vertical Acceleration in body frame (m/s2)*/
    float           vertAccel;
    /**< Heading Rate (Radians/second) */
    float           yawRate;
    /**< Body pitch (Radians) */
    float           pitch;
};

struct GnssLocationGPSTimeStruct {
    uint16_t gpsWeek;
    /**< Current GPS week as calculated from midnight, Jan. 6, 1980.
        - Units: Weeks */
    uint32_t gpsTimeOfWeekMs;
    /**< Amount of time into the current GPS week.
        -Units: Milliseconds */
};

struct GnssLocationGPSTime {
    /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
        - Units: Weeks */
    uint16_t gpsWeek;
    /**<   Amount of time into the current GPS week. \n
        - Units: Milliseconds */
    uint32_t gpsTimeOfWeekMs;
};

struct GnssSystemTimeStructType {
    /**< Validity mask for below fields */
    GnssSystemTimeFlagsMask      validityMask;
    /**< Extended week number at reference tick.
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
      Calculated from 00:00 UT on Sunday August 22, 1999 (midnight between August 21 and August 22).
   */
    uint16_t systemWeek;
    /**< Time in to the current week at reference tick.
       Unit: Millisecond. Range: 0 to 604799999.
       Check for systemClkTimeUncMs before use */
    uint32_t systemMsec;
    /**< System clock time bias (sub-millisecond)
        Units: Millisecond
        Note: System time (TOW Millisecond) = systemMsec - systemClkTimeBias.
        Check for systemClkTimeUncMs before use. */
    float systemClkTimeBias;
    /**< Single sided maximum time bias uncertainty
        Units: Millisecond */
    float systemClkTimeUncMs;
    /**< FCount (free running HW timer) value. Don't use for relative time purpose
         due to possible discontinuities.
         Unit: Millisecond */
    uint32_t refFCount;
    /**< Number of clock resets/discontinuities detected, affecting the local hardware counter value. */
    uint32_t numClockResets;
};

struct GnssGloTimeStructType {
    /**< GLONASS day number in four years. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 65535 */
    uint16_t gloDays;
    /**< GLONASS time of day in Millisecond. Refer to GLONASS ICD.
        Units: Millisecond
        Check for gloClkTimeUncMs before use */
    /**< Validity mask for below fields */
    GnssGloTimeFlagsMask validityMask;
    uint32_t gloMsec;
    /**< GLONASS clock time bias (sub-millisecond)
        Units: Millisecond
        Note: GLO time (TOD Millisecond) = gloMsec - gloClkTimeBias.
        Check for gloClkTimeUncMs before use. */
    float gloClkTimeBias;
    /**< Single sided maximum time bias uncertainty
        Units: Millisecond */
    float gloClkTimeUncMs;
    /**< FCount (free running HW timer) value. Don't use for relative time purpose
        due to possible discontinuities.
        Unit: Millisecond */
    uint32_t  refFCount;
    /**< Number of clock resets/discontinuities detected, affecting the local hardware counter value. */
    uint32_t numClockResets;
    /**< GLONASS four year number from 1996. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 255 */
    uint8_t gloFourYear;
};

struct Location {
    LocationFlagsMask flags; /**< bitwise OR of LocationFlagsBits to mark which params are valid */
    uint64_t timestamp;      /**< UTC timestamp for location fix, milliseconds since January 1, 1970 */
    double latitude;         /**< in degrees */
    double longitude;        /**< in degrees */
    double altitude;         /**< in meters above the WGS 84 reference ellipsoid */
    float speed;             /**< in meters per second */
    float bearing;           /**< in degrees; range [0, 360) */
    float horizontalAccuracy;/**< in meters */
    float verticalAccuracy;  /**< in meters */
    float speedAccuracy;     /**< in meters/second */
    float bearingAccuracy;   /**< in degrees (0 to 359.999) */
    LocationTechnologyMask techMask;
};

struct GnssLocation : public Location {
    GnssLocationInfoFlagMask flags;     /**< bitwise OR of GnssLocationInfoBits for param validity */
    float altitudeMeanSeaLevel;         /**< altitude wrt mean sea level */
    float pdop;                         /**< position dilusion of precision */
    float hdop;                         /**< horizontal dilusion of precision */
    float vdop;                         /**< vertical dilusion of precision */
    float gdop;                         /**< geometric  dilution of precision */
    float tdop;                         /**< time dilution of precision */
    float magneticDeviation;            /**< magnetic deviation */
    LocationReliability horReliability; /**< horizontal reliability */
    LocationReliability verReliability; /**< vertical reliability */
    float horUncEllipseSemiMajor;       /**< horizontal elliptical accuracy semi-major axis */
    float horUncEllipseSemiMinor;       /**< horizontal elliptical accuracy semi-minor axis */
    float horUncEllipseOrientAzimuth;   /**< horizontal elliptical accuracy azimuth */
    float northVelocity;                /**< North Velocity.Unit: Meters/sec */
    float eastVelocity;                 /**< East Velocity */
    float upVelocity;
    float northVelocityStdDeviation;
    float eastVelocityStdDeviation;
    float upVelocityStdDeviation;
    GnssLocationSvUsedInPosition svUsedInPosition;   /**< Gnss sv used in position data */
    GnssLocationNavSolutionMask  navSolutionMask;    /**< Nav solution mask to indicate sbas corrections */
    GnssLocationPosTechMask      posTechMask;        /**< Position technology used in computing this fix */
    GnssLocationPositionDynamics bodyFrameData;      /**< Body Frame Dynamics: 4wayAcceleration and pitch set with validity */
    GnssLocationGPSTimeStruct    gnssTime;           /**< GNSS Time */
    /**< Time applicability of PVT report */
    struct {
        /**< Specifies GNSS system time reported. Mandatory field */
        GnssSvType gnssSystemTimeSrc;
        /**< Reporting of GPS system time is recommended.
         If GPS time is unknown & other satellite system time is known,
         it should be reported.
         Mandatory field
        */
        union {
            GnssSystemTimeStructType gpsSystemTime;
            GnssSystemTimeStructType galSystemTime;
            GnssSystemTimeStructType bdsSystemTime;
            GnssSystemTimeStructType qzssSystemTime;
            GnssGloTimeStructType    gloSytemTime;
        }u;
    } gnssSystemTime;
};

struct GnssSv {
    uint16_t svId;     /**< Unique Identifier */
    GnssSvType type;   /**< type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO) */
    float cN0Dbhz;     /**< signal strength */
    float elevation;   /**< elevation of SV (in degrees) */
    float azimuth;     /**< azimuth of SV (in degrees) */
    GnssSvOptionsMask gnssSvOptionsMask; /**< Bitwise OR of GnssSvOptionsBits */
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

    @param err: if not SUCCESS, then id is not valid
*/
typedef std::function<void(
    LocationResponse response
)> ResponseCb;

/** @fn
    @brief
    LocationCb is for receiving a location in a positioning session
*/
typedef std::function<void(
    const Location& location
)> LocationCb;

/** @fn
    @brief
    GnssLocationCb is for receiving a GNSS location in a positioning session
*/
typedef std::function<void(
    const GnssLocation& gnssLocation
)> GnssLocationCb;

/** @fn
    @brief
    GnssNmeaCb is for receiving GNSS SV information
*/
typedef std::function<void(
    const std::vector<GnssSv>& gnssSvs
)> GnssSvCb;

/** @fn
    @brief
    GnssNmeaCb is for receiving NMEA sentences
*/
typedef std::function<void(
    uint64_t timestamp, const std::string& nmea
)> GnssNmeaCb;

struct GnssReportCbs {
    GnssLocationCb gnssLocationCallback;
    GnssSvCb gnssSvCallback;
    GnssNmeaCb gnssNmeaCallback;
    inline GnssReportCbs(GnssLocationCb locationCb,
                         GnssSvCb svCb, GnssNmeaCb nmeaCb) :
        gnssLocationCallback(locationCb),
        gnssSvCallback(svCb), gnssNmeaCallback(nmeaCb) {
    }
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

        @param
        intervalInMs, time between fixes, or TBF, in milliseconds. The actual interval
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
        distanceInMeters, distance between fixes, in meters. 0 to indicate don't care.
                      1)  The underlying system may have a minimum distance threshold
                      (e.g. 1 meter). Effective distance will not be smaller
                      than this lower bound.
                      2) The effective distance may have a granularity level higher
                      than 1 m, e.g. 5 m. So distanceInMeters being 59 may be honored
                      at 60 m, depending on the system.
                      3) Where there is anotehr application in they system having a
                      session with shorter distance, this client may benefit and
                      receive reports at that distance.
        locationCallback, callback to receive positions
        responseCallback, callback to receive system responses; optional.

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

        @param
        intervalInMs, time between fixes, or TBF, in milliseconds. The actual interval
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
        gnssReportCallbacks, table of callbacks to receive GNSS locations / SV / NMEA
        responseCallback, callback to receive system responses; optional.

        @return True, if a session is successfully started.
                False, if no session is started, i.e. when gnssReportCallbacks is
                       populated with nullptr's only.
    */
    bool startPositionSession(uint32_t intervalInMs, const GnssReportCbs& gnssReportCallbacks,
                              ResponseCb responseCallback);

    /** @brief Stops the ongoing positioning session.
    */
    void stopPositionSession();


private:
    LocationClientApiImpl* mApiImpl;
};

} // namespace location_client

#endif /* LOCATIONCLIENTAPI_H */
