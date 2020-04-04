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


#include <LocationDataTypes.h>
#include <LocationIntegrationApi.h>
#include <LocationIntegrationApiImpl.h>
#include <log_util.h>

namespace location_integration {


/******************************************************************************
LocationIntegrationApi
******************************************************************************/
LocationIntegrationApi::LocationIntegrationApi(
        const LocConfigPriorityMap& priorityMap,
        LocIntegrationCbs& integrationCbs) {

    mApiImpl = new LocationIntegrationApiImpl(integrationCbs);
}

LocationIntegrationApi::~LocationIntegrationApi() {

    LOC_LOGd("calling destructor of LocationIntegrationApi");
    if (mApiImpl) {
        mApiImpl->destroy();
    }
}

bool LocationIntegrationApi::configConstellations(
        const LocConfigBlacklistedSvIdList* blacklistedSvIds){

    bool retVal = false;

    if (mApiImpl) {
        if (nullptr == blacklistedSvIds) {
            LOC_LOGd("reset to default");
            mApiImpl->resetConstellationConfig();
            retVal = true;
        } else {
            GnssSvTypeConfig svTypeConfig = {};
            svTypeConfig.size = sizeof(svTypeConfig);
            svTypeConfig.enabledSvTypesMask =
                    GNSS_SV_TYPES_MASK_GLO_BIT|GNSS_SV_TYPES_MASK_BDS_BIT|
                    GNSS_SV_TYPES_MASK_QZSS_BIT|GNSS_SV_TYPES_MASK_GAL_BIT;
            GnssSvIdConfig svIdConfig = {};
            svIdConfig.size = sizeof(GnssSvIdConfig);

            for (GnssSvIdInfo it : *blacklistedSvIds) {
                GnssSvTypesMask svTypeMask = (GnssSvTypesMask) 0;
                uint64_t* svMaskPtr = NULL;
                GnssSvId initialSvId = 0;
                uint16_t svIndexOffset = 0;
                switch (it.constellation) {
                case GNSS_CONSTELLATION_TYPE_GLONASS:
                    svTypeMask = (GnssSvTypesMask) GNSS_SV_TYPES_MASK_GLO_BIT;
                    svMaskPtr = &svIdConfig.gloBlacklistSvMask;
                    initialSvId = GNSS_SV_CONFIG_GLO_INITIAL_SV_ID;
                    break;
                case GNSS_CONSTELLATION_TYPE_QZSS:
                    svTypeMask = (GnssSvTypesMask) GNSS_SV_TYPES_MASK_QZSS_BIT;
                    svMaskPtr = &svIdConfig.qzssBlacklistSvMask;
                    initialSvId = GNSS_SV_CONFIG_QZSS_INITIAL_SV_ID;
                    break;
                case GNSS_CONSTELLATION_TYPE_BEIDOU:
                    svTypeMask = (GnssSvTypesMask) GNSS_SV_TYPES_MASK_BDS_BIT;
                    svMaskPtr = &svIdConfig.bdsBlacklistSvMask;
                    initialSvId = GNSS_SV_CONFIG_BDS_INITIAL_SV_ID;
                    break;
                case GNSS_CONSTELLATION_TYPE_GALILEO:
                    svTypeMask = (GnssSvTypesMask) GNSS_SV_TYPES_MASK_GAL_BIT;
                    svMaskPtr = &svIdConfig.galBlacklistSvMask;
                    initialSvId = GNSS_SV_CONFIG_GAL_INITIAL_SV_ID;
                    break;
                case GNSS_CONSTELLATION_TYPE_SBAS:
                    // SBAS does not support enable/disable whole constellation
                    // so do not set up svTypeMask for SBAS
                    svMaskPtr = &svIdConfig.sbasBlacklistSvMask;
                    // SBAS currently has two ranges
                    // range of SV id: 183 to 191
                    if (GNSS_SV_ID_BLACKLIST_ALL == it.svId) {
                        LOC_LOGd("blacklist all SBAS SV");
                    } else if (it.svId >= GNSS_SV_CONFIG_SBAS_INITIAL2_SV_ID) {
                        initialSvId = GNSS_SV_CONFIG_SBAS_INITIAL2_SV_ID;
                        svIndexOffset = GNSS_SV_CONFIG_SBAS_INITIAL_SV_LENGTH;
                    } else if ((it.svId >= GNSS_SV_CONFIG_SBAS_INITIAL_SV_ID) &&
                               (it.svId < (GNSS_SV_CONFIG_SBAS_INITIAL_SV_ID +
                                           GNSS_SV_CONFIG_SBAS_INITIAL_SV_LENGTH))){
                        // range of SV id: 120 to 158
                        initialSvId = GNSS_SV_CONFIG_SBAS_INITIAL_SV_ID;
                    } else {
                        LOC_LOGe("invalid SBAS sv id %d", it.svId);
                        svMaskPtr = nullptr;
                    }
                    break;
                default:
                    break;
                }

                if (nullptr == svMaskPtr) {
                    LOC_LOGe("Invalid constellation %d", it.constellation);
                } else {
                    // SV ID 0 = Blacklist All SVs
                    if (GNSS_SV_ID_BLACKLIST_ALL == it.svId) {
                        // blacklist all SVs in this constellation
                        *svMaskPtr = GNSS_SV_CONFIG_ALL_BITS_ENABLED_MASK;
                        svTypeConfig.enabledSvTypesMask &= ~svTypeMask;
                        svTypeConfig.blacklistedSvTypesMask |= svTypeMask;
                    } else if (it.svId < initialSvId || it.svId >= initialSvId + 64) {
                        LOC_LOGe ("SV TYPE %d, Invalid sv id %d ", it.constellation, it.svId);
                    } else {
                        uint32_t shiftCnt = it.svId + svIndexOffset - initialSvId;
                        *svMaskPtr |= (1ULL << shiftCnt);
                    }
                }
            }

            LOC_LOGd(">>> configConstellations, "
                     "enabled mask =0x%" PRIx64 ", "
                     "blacklisted mask =0x%" PRIx64 ",\n"
                     "glo blacklist mask =0x%" PRIx64 ", "
                     "qzss blacklist mask =0x%" PRIx64 ",\n"
                     "bds blacklist mask =0x%" PRIx64 ", "
                     "gal blacklist mask =0x%" PRIx64 ",\n"
                     "sbas blacklist mask =0x%" PRIx64 ", ",
                     svTypeConfig.enabledSvTypesMask,
                     svTypeConfig.blacklistedSvTypesMask,
                     svIdConfig.gloBlacklistSvMask,
                     svIdConfig.qzssBlacklistSvMask,
                     svIdConfig.bdsBlacklistSvMask,
                     svIdConfig.galBlacklistSvMask,
                     svIdConfig.sbasBlacklistSvMask);
            mApiImpl->configConstellations(svTypeConfig, svIdConfig);
            retVal = true;
        }
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }

    return retVal;
}

bool LocationIntegrationApi::configConstrainedTimeUncertainty(
        bool enable, float tuncThreshold, uint32_t energyBudget) {
    if (mApiImpl) {
        mApiImpl->configConstrainedTimeUncertainty(
                enable, tuncThreshold, energyBudget);
        return true;
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
}

bool LocationIntegrationApi::configPositionAssistedClockEstimator(bool enable) {

    if (mApiImpl) {
        mApiImpl->configPositionAssistedClockEstimator(enable);
        return true;
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
}

bool LocationIntegrationApi::deleteAllAidingData() {

    if (mApiImpl) {
        GnssAidingData aidingData = {};
        aidingData.deleteAll = true;
        aidingData.posEngineMask = POSITION_ENGINE_MASK_ALL;
        mApiImpl->gnssDeleteAidingData(aidingData);
        return true;
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
}

bool LocationIntegrationApi::configLeverArm(const LeverArmParamsMap& configInfo) {

    if (mApiImpl) {
        LeverArmConfigInfo halLeverArmConfigInfo = {};
        for (auto it = configInfo.begin(); it != configInfo.end(); ++it) {
            ::LeverArmParams* params = nullptr;
            LeverArmTypeMask mask = (LeverArmTypeMask) 0;
            switch (it->first){
            case LEVER_ARM_TYPE_GNSS_TO_VRP:
                mask = LEVER_ARM_TYPE_GNSS_TO_VRP_BIT;
                params = &halLeverArmConfigInfo.gnssToVRP;
                break;
            case LEVER_ARM_TYPE_DR_IMU_TO_GNSS:
                mask = LEVER_ARM_TYPE_DR_IMU_TO_GNSS_BIT;
                params = &halLeverArmConfigInfo.drImuToGnss;
                break;
            case LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS:
                mask = LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS_BIT;
                params = &halLeverArmConfigInfo.veppImuToGnss;
                break;
            default:
                break;
            }
            if (nullptr != params) {
                halLeverArmConfigInfo.leverArmValidMask |= mask;
                params->forwardOffsetMeters = it->second.forwardOffsetMeters;
                params->sidewaysOffsetMeters = it->second.sidewaysOffsetMeters;
                params->upOffsetMeters = it->second.upOffsetMeters;
                LOC_LOGd("mask 0x%x, %f %f %f", mask,  params->forwardOffsetMeters,
                       params->sidewaysOffsetMeters, params->upOffsetMeters);
            }
        }
        mApiImpl->configLeverArm(halLeverArmConfigInfo);
        return true;
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
}

bool LocationIntegrationApi::configRobustLocation(bool enable, bool enableForE911) {

    if (mApiImpl) {
        mApiImpl->configRobustLocation(enable, enableForE911);
        return true;
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
}

bool LocationIntegrationApi::getRobustLocationConfig() {
    if (mApiImpl) {
        // mApiImpl->getRobustLocationConfig returns none-zero when
        // there is no callback
        return (mApiImpl->getRobustLocationConfig() == 0);
    } else {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
}

} // namespace location_integration
