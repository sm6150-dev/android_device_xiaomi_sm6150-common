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

#ifndef LOCATION_INTEGRATION_API_H
#define LOCATION_INTEGRATION_API_H

#include <loc_pla.h>

#ifdef NO_UNORDERED_SET_OR_MAP
    #include <map>
#else
    #include <unordered_map>
#endif

namespace location_integration
{
/**
 * Configuration API types that are currently supported
 */
enum LocConfigTypeEnum{
    /** Blacklist some SV constellations from being used by GNSS
     *  engine. </br> */
    CONFIG_CONSTELLATIONS = 1,
    /** Enable/disable the constrained time uncertainty feature and
     *  configure related parameters when the feature is
     *  enabled. </br> */
    CONFIG_CONSTRAINED_TIME_UNCERTAINTY = 2,
    /** Enable/disable the position assisted clock estimator
     *  feature. </br> */
    CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR = 3,
    /** Delete all aiding data. </br> */
    CONFIG_AIDING_DATA_DELETION = 4,
    /** Config lever arm parameters. </br> */
    CONFIG_LEVER_ARM = 5,
    /** Config robust location feature. </br> */
    CONFIG_ROBUST_LOCATION = 6,
    /** Get configuration regarding robust location setting used by
     *  GNSS engine.  </br> */
    GET_ROBUST_LOCATION_CONFIG = 100,
} ;

/**
 *  Specify the asynchronous response when calling location
 *  integration API. */
enum LocIntegrationResponse {
    /** Location integration API request is processed
     *  successfully */
    LOC_INT_RESPONSE_SUCCESS = 1,
    /** Location integration API request is not processed
     *  successfully */
    LOC_INT_RESPONSE_FAILURE = 2,
    /** Location integration API request is not supported */
    LOC_INT_RESPONSE_NOT_SUPPORTED = 3,
    /** Location integration API request has invalid parameter */
    LOC_INT_RESPONSE_PARAM_INVALID = 4,
} ;

/**
 * Define the priority to be used when the corresponding
 * configuration API specified by type is invoked. Priority is
 * specified via uint32_t and lower number means lower
 * priority.
 *
 * Currently, all configuration requests, regardless of
 * priority, will be honored. Priority based request
 * honoring will come in subsequent phases and more
 * detailed description on this will be available then.
 */
typedef std::unordered_map<LocConfigTypeEnum, uint32_t>
        LocConfigPriorityMap;

/** Gnss constellation type mask */
typedef uint32_t GnssConstellationMask;

/**
 *  Specify SV Constellation types that can be configured via
 *  configConstellations. </br>
 *  Please note that GPS constellation can not be disabled
 *  and thus not included in the enum list. </br> */
enum GnssConstellationType {
    /** GLONASS SV system */
    GNSS_CONSTELLATION_TYPE_GLONASS  = 1,
    /** QZSS SV system */
    GNSS_CONSTELLATION_TYPE_QZSS     = 2,
    /** BEIDOU SV system */
    GNSS_CONSTELLATION_TYPE_BEIDOU   = 3,
    /** GALILEO SV system */
    GNSS_CONSTELLATION_TYPE_GALILEO  = 4,
    /** SBAS SV system */
    GNSS_CONSTELLATION_TYPE_SBAS     = 5
};

/**
 * Specify parameters related to enable/disable SVs */
struct GnssSvIdInfo {
    /** constellation for the sv  */
    GnssConstellationType constellation;
    /** sv id for the constellation:
     * GLONASS SV id range: 65 to 96
     * QZSS SV id range: 193 to 197
     * BDS SV id range: 201 to 237
     * GAL SV id range: 301 to 336
     * SBAS SV id range: 120 to 158 and 183 to 191
     */
    uint32_t              svId;
};

/**
 *  Lever ARM type */
enum LeverArmType {
    /** Lever arm parameters regarding the VRP (Vehicle Reference
     *  Point) w.r.t the origin (at the GNSS Antenna) */
    LEVER_ARM_TYPE_GNSS_TO_VRP = 1,
    /** Lever arm regarding GNSS Antenna w.r.t the origin at the
     *  IMU (inertial measurement unit) for DR (dead reckoning
     *  engine) */
    LEVER_ARM_TYPE_DR_IMU_TO_GNSS = 2,
    /** Lever arm regarding GNSS Antenna w.r.t the origin at the
     *  IMU (inertial measurement unit) for VEPP (vision enhanced
     *  precise positioning engine) */
    LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS = 3,
};

/**
 * Specify parameters related to lever arm */
struct LeverArmParams {
    /** Offset along the vehicle forward axis, in unit of meters */
    float forwardOffsetMeters;
    /** Offset along the vehicle starboard axis, in unit of
     *  meters */
    float sidewaysOffsetMeters;
    /** Offset along the vehicle up axis, in unit of meters  */
    float upOffsetMeters;
};

/**
 * Define the lever arm parameters to be used with
 * configLeverArm function.
 *
 * Currently, there are two types of lever arm parameters that
 * can be configured:
 * 1: Lever arm regarding GNSS Antenna w.r.t the origin at the
 *    IMU (inertial measurement unit)
 * 2: Lever arm parameters regarding the VRP (Vehicle Reference Point)
 *    w.r.t the origin (at the GNSS Antenna)
 */
typedef std::unordered_map<LeverArmType, LeverArmParams> LeverArmParamsMap;

/**
 * Specify the absolute set of constellations and SVs
 * that should not be used by the GNSS engine on modem.
 *
 * To blacklist all SVs from one constellation, use
 * GNSS_SV_ID_BLACKLIST_ALL as sv id for that constellation.
 *
 * To specify only a subset of the SVs to be blacklisted, for
 * each SV, specify its constelaltion and the SV id and put in
 * the vector.
 *
 * All SVs being blacklisted should not be used in positioning.
 * For SBAS, SVs are not used by GNSS engine on modem by
 * default. Blacklisting SBAS SV only blocks SBAS data demod.
 * SBAS XCORR functionality will not be disabled for blacklisted
 * SBAS SVs.
 *
 * GLONASS SV id range: 65 to 96
 * QZSS SV id range: 193 to 197
 * BDS SV id range: 201 to 237
 * GAL SV id range: 301 to 336
 * SBAS SV id range: 120 to 158 and 183 to 191
 */
#define GNSS_SV_ID_BLACKLIST_ALL (0)
typedef std::vector<GnssSvIdInfo> LocConfigBlacklistedSvIdList;

/** @fn
    @brief
    Used to get the asynchronous notification of the processing
    status of the configuration APIs.

    In order to get the notification, an instantiation
    LocConfigCb need to be passed to the constructor of
    LocationIntegration API. Please refer to each function for
    details regarding how this callback will be invoked.

    @param
    response: if the response is not LOC_INT_API_RESPONSE_SUCCESS,
    then the integration API of requestType has failed.
*/
typedef std::function<void(
    /** location configuration request type */
    LocConfigTypeEnum      configType,
    /** processing status for the location configuration request*/
    LocIntegrationResponse response
)> LocConfigCb;

/** Specify the valid mask for robust location configuration
 *  used by GNSS engine on modem. The robust location
 *  configuraiton can be retrieved by invoking
 *  LocConfigGetRobustLocationConfigCb. <br/> */
enum RobustLocationConfigValidMask {
    /** RobustLocationConfig has valid
     *  RobustLocationConfig::enabled. <br/> */
    ROBUST_LOCATION_CONFIG_VALID_ENABLED          = (1<<0),
    /** RobustLocationConfig has valid
     *  RobustLocationConfig::enabledForE911. <br/> */
    ROBUST_LOCATION_CONFIG_VALID_ENABLED_FOR_E911 = (1<<1),
};

/** Specify the robust location configuration used by modem GNSS
 *  engine that will be returned when invoking
 *  LocConfigGetRobustLocationConfigCb. The configuration will
 *  be returned via LocConfigGetRobustLocationConfigCb. <br/> */
struct RobustLocationConfig {
    /** Bitwise OR of RobustLocationConfigValidMask to specify
     *  the valid fields. <br/> */
    RobustLocationConfigValidMask validMask;
    /** Specify whether robust location feature is enabled or
     *  not. <br/> */
    bool enabled;
    /** Specify whether robust location feature is enabled or not
     *  when device is on E911 call. <br/> */
    bool enabledForE911;
};

/**
 *  Specify the callback to retrieve the robust location setting
 *  used by modem GNSS engine. The callback will be invoked
 *  for successful processing of getRobustLocationConfig().
 *  <br/>
 *
 *  In order to receive the robust location configuration, user
 *  shall instantiate the callback and pass it to the
 *  LocationIntegrationApi constructor and then invoke
 *  getRobustLocationConfig(). <br/> */
typedef std::function<void(
    RobustLocationConfig robustLocationConfig
)> LocConfigGetRobustLocationConfigCb;

/**
 *  Specify the set of callbacks that can be passed to
 *  LocationIntegrationAPI constructor to receive configuration
 *  command processing status and the requested data.
 */
struct LocIntegrationCbs {
    /** Callback to receive the procesings status, e.g.: success
     *  or failure.  <br/> */
    LocConfigCb configCb;
    /** Callback to receive the robust location setting.  <br/> */
    LocConfigGetRobustLocationConfigCb getRobustLocationConfigCb;
};

class LocationIntegrationApiImpl;
class LocationIntegrationApi
{
public:

    /** @brief
        Creates an instance of LocationIntegrationApi object with
        the specified priority map and callback functions. For this
        phase, the priority map will be ignored.

        @param
        priorityMap: specify the priority for each of the
        configuration type that this integration API client would
        like to control.

        @param
        integrationCbs: set of callbacks to receive info from
        location integration API. For example, client can pass
        LocConfigCb to receive the asynchronous processing status of
        configuration command.
    */
    LocationIntegrationApi(const LocConfigPriorityMap& priorityMap,
                           LocIntegrationCbs& integrationCbs);

    /** @brief Default destructor */
    ~LocationIntegrationApi();

    /** @brief
        Blacklist some constellations or subset of SVs from the
        constellation from being used by the GNSS engine on modem.

        Please note this API call is not incremental and the new
        setting will completely overwrite the previous call.
        blacklistedSvList shall contain the complete list
        of blacklisted constellations and blacklisted SVs.
        Constellations and SVs not specified in the parameter will
        be considered to be allowed to get used by GNSS engine.

        Client should wait for the command to finish, e.g.: via
        configCb received before issuing a second
        configConstellations command. Behavior is not defined if
        client issues a second request of configConstellations
        without waiting for the finish of the previous
        configConstellations request.

        @param
        blacklistedSvList: specify the set of constellations and SVs
        that should not be used by the GNSS engine on modem.
        Constellations and SVs not specified in blacklistedSvList
        will be allowed to get used by the GNSS engine on modem.

        Nullptr of blacklistedSvList will be interpreted as to reset
        the constellation configuration to device default.

        @return true, if request is successfully processed as
                requested. When returning true, configCb will be
                invoked to deliver asynchronous processing status.

        @return false, if request is not successfully processed as
                requested. When returning false, configCb will
                not be invoked.
    */
    bool configConstellations(const LocConfigBlacklistedSvIdList*
                              blacklistedSvList=nullptr);

     /** @brief
         Enable or disable the constrained time uncertainty feature.

         Client should wait for the command to finish, e.g.:
         via configCb received before issuing a second
         configConstrainedTimeUncertainty command. Behavior is not
         defined if client issues a second request of
         configConstrainedTimeUncertainty without waiting for
         the finish of the previous configConstrainedTimeUncertainty
         request.

         @param
         enable: true to enable the constrained time uncertainty
         feature and false to disable the constrainted time
         uncertainty feature.

         @param
         tuncThresholdMs: this specifies the time uncertainty
         threshold that GNSS engine need to maintain, in units of
         milli-seconds. Default is 0.0 meaning that modem default
         value of time uncertainty threshold will be used. This
         parameter is ignored when request is to disable this
         feature.

         @param
         energyBudget: this specifies the power budget that GNSS
         engine is allowed to spend to maintain the time uncertainty.
         Default is 0 meaning that GPS engine is not constained by
         power budget and can spend as much power as needed. The
         parameter need to be specified in units of 0.1 milli watt
         second, e.g.: an energy budget of 2.0 milli watt will be of
         20 units. This parameter is ignored when request is to
         disable this feature.

        @return true, if the constrained time uncertainty feature is
                successfully enabled or disabled as requested.
                When returning true, configCb will be invoked to
                deliver asynchronous processing status.

        @return false, if the constrained time uncertainty feature is
                not successfully enabled or disabled as requested.
                When returning false, configCb will not be invoked.
    */
    bool configConstrainedTimeUncertainty(bool enable,
                                          float tuncThresholdMs = 0.0,
                                          uint32_t energyBudget = 0);

    /** @brief
        Enable or disable position assisted clock estimator feature.

        Client should wait for the command to finish, e.g.: via
        configCb received before issuing a second
        configPositionAssistedClockEstimator command. Behavior is
        not defined if client issues a second request of
        configPositionAssistedClockEstimator without waiting for the
        finish of the previous configPositionAssistedClockEstimator
        request.

        @param
        enable: true to enable position assisted clock estimator and
        false to disable the position assisted clock estimator
        feature.

        @return true, if position assisted clock estimator is
                successfully enabled or disabled as requested. When
                returning true, configCb will be invoked to deliver
                asynchronous processing status.

        @return false, if position assisted clock estimator is not
                successfully enabled or disabled as requested. When
                returning false, configCb will not be invoked.
    */
    bool configPositionAssistedClockEstimator(bool enable);

   /** @brief
        Request all forms of aiding data to be deleted from all
        position engines.

        @return true, if aiding data delete request is successfully
                performed. When returning true, configCb will be
                invoked to deliver asynchronous processing status.

        @return false, if aiding data delete request is not
                successfully performed. When returning false,
                configCb will not be invoked.
    */
    bool deleteAllAidingData();

    /** @brief
        Sets the lever arm parameters for the vehicle.

        LeverArm is sytem level parameters and it is not expected to
        change. So, it is needed to issue configLeverArm once for
        every application processor boot-up. For multiple
        invocations of this API, client should wait
        for the command to finish, e.g.: via configCb received
        before issuing a second configLeverArm command. Behavior is
        not defined if client issues a second request of
        cconfigLeverArm without waiting for the finish of the
        previous configLeverArm request.

        @param
        configInfo: lever arm configuration info regarding below two
        types of lever arm info:
        a: GNSS Antenna w.r.t the origin at the IMU (inertial
        measurement unit) for DR engine
        b: GNSS Antenna w.r.t the origin at the IMU (inertial
        measurement unit) for VEPP engine
        c: VRP (Vehicle Reference Point) w.r.t the origin (at the
        GNSS Antenna). Vehicle manufacturers prefer the position
        output to be tied to a specific point in the vehicle rather
        than where the antenna is placed (midpoint of the rear axle
        is typical).

        Caller can specify which types of lever arm info to
        configure via the leverMarkTypeMask.

        @return true, if lever arm parameters are successfully
                configured as requested. When returning true,
                configCb will be invoked to deliver asynchronous
                processing status.

        @return false, if lever arm parameters are not successfully
                configured as requested. When returning false,
                configCb will not be invoked.
    */
    bool configLeverArm(const LeverArmParamsMap& configInfo);


    /** @brief
        Enable/disable robust location feature and enable/disable
        robust location while device is on E911.

        @param
        enable: true to enable robust location and false to disable
        robust location.

        @param
        enableForE911: true to enable robust location when device is
        on E911 session and false to disable on E911 session. <br/>
        This parameter is only valid if robust location is enabled.
        </br>

        @return true, if robust location are successfully configured
                as requested. When returning true, configCb will be
                invoked to deliver asynchronous processing status.

        @return false, if robust location are not successfully
                configured as requested. When returning false,
                configCb will not be invoked.
    */
    bool configRobustLocation(bool enable, bool enableForE911=false);

    /** @brief
        Request robust location setting used by GNSS engine. If
        processing of the command fails, the failure status will be
        returned via configCb. If the processing of the command is
        successful, the successful status will be returned via
        configCB, and the robust location config info will be
        returned via getRobustLocationConfigCb passed via the
        constructor.

        @return true, if the API request has been accepted.

        @return false, if the API request has not been accepted for
                further processing. When returning false, configCb
                and getRobustLocationConfigCb will not be
                invoked.
    */
    bool getRobustLocationConfig();

private:
    LocationIntegrationApiImpl* mApiImpl;
};

} // namespace location_integration

#endif /* LOCATION_INTEGRATION_API_H */
