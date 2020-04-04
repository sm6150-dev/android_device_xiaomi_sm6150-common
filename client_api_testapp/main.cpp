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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sstream>
#include <grp.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <semaphore.h>
#include <loc_pla.h>
#include <loc_cfg.h>
#ifdef NO_UNORDERED_SET_OR_MAP
    #include <map>
#else
    #include <unordered_map>
#endif

#include <LocationClientApi.h>
#include <LocationIntegrationApi.h>

using namespace location_client;
using namespace location_integration;


#define NUM_LOOP_PINGTEST (1000)

// debug events counter
static uint32_t numLocationCb = 0;
static uint32_t numGnssLocationCb = 0;
static uint32_t numGnssSvCb = 0;
static uint32_t numGnssNmeaCb = 0;
static sem_t sem_pingcbreceived;
#define DISABLE_TUNC       "disableTunc"
#define ENABLE_TUNC        "enableTunc"
#define DISABLE_PACE       "disablePACE"
#define ENABLE_PACE        "enablePACE"
#define RESET_SV_CONFIG    "resetSVConfig"
#define CONFIG_SV          "configSV"
#define MULTI_CONFIG_SV    "multiConfigSV"
#define DELETE_ALL         "deleteAll"
#define CONFIG_LEVER_ARM   "configLeverArm"
#define CONFIG_ROBUST_LOCATION  "configRobustLocation"
#define GET_ROBUST_LOCATION_CONFIG "getRobustLocationConfig"

// debug utility
static uint64_t getTimestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    uint64_t absolute_micro =
            ((uint64_t)(ts.tv_sec)) * 1000000ULL + ((uint64_t)(ts.tv_nsec)) / 1000ULL;
    return absolute_micro;
}

/******************************************************************************
Callback functions
******************************************************************************/
static void onCapabilitiesCb(location_client::LocationCapabilitiesMask mask) {
    printf("<<< onCapabilitiesCb mask=0x%x\n", mask);
}

static void onResponseCb(location_client::LocationResponse response) {
    printf("<<< onResponseCb err=%u\n", response);
}

static void onLocationCb(const location_client::Location& location) {
    numLocationCb++;
    printf("<<< onLocationCb cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f\n",
            numLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
}

static void onGnssLocationCb(const location_client::GnssLocation& location) {
    numGnssLocationCb++;
    printf("<<< onGnssLocationCb cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f\n",
            numGnssLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
}

static void onGnssSvCb(const std::vector<location_client::GnssSv>& gnssSvs) {
    numGnssSvCb++;
    std::stringstream ss;
    ss << "<<< onGnssSvCb c=" << numGnssSvCb << " s=" << gnssSvs.size();
    for (auto sv : gnssSvs) {
        ss << " " << sv.type << ":" << sv.svId << "/" << (uint32_t)sv.cN0Dbhz;
    }
    printf("%s\n", ss.str().c_str());
}

static void onGnssNmeaCb(uint64_t timestamp, const std::string& nmea) {
    numGnssNmeaCb++;
    printf("<<< onGnssNmeaCb cnt=%u time=%" PRIu64" nmea=%s",
            numGnssNmeaCb, timestamp, nmea.c_str());
}

static void onCapabilitiesCb2(location_client::LocationCapabilitiesMask mask) {
    printf("<<< onCapabilitiesCb2 mask=0x%x\n", mask);
}

static void onResponseCb2(location_client::LocationResponse response) {
    printf("<<< onResponseCb2 err=%u\n", response);
}

static void onLocationCb2(const location_client::Location& location) {
    numLocationCb++;
    printf("<<< onLocationCb2 cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f\n",
            numLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
}

static void onGnssLocationCb2(const location_client::GnssLocation& location) {
    numGnssLocationCb++;
    printf("<<< onGnssLocationCb2 cnt=%u time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f\n",
            numGnssLocationCb,
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
}

static void onGnssSvCb2(const std::vector<location_client::GnssSv>& gnssSvs) {
    numGnssSvCb++;
    std::stringstream ss;
    ss << "<<< onGnssSvCb2 c=" << numGnssSvCb << " s=" << gnssSvs.size();
    for (auto sv : gnssSvs) {
        ss << " " << sv.type << ":" << sv.svId << "/" << (uint32_t)sv.cN0Dbhz;
    }
    printf("%s\n", ss.str().c_str());
}

static void onGnssNmeaCb2(uint64_t timestamp, const std::string& nmea) {
    numGnssNmeaCb++;
    printf("<<< onGnssNmeaCb2 cnt=%u time=%" PRIu64" nmea=%s",
            numGnssNmeaCb, timestamp, nmea.c_str());
}

static void onPingTestCb(uint32_t response) {
    sem_post(&sem_pingcbreceived);
}

static void onConfigResponseCb(location_integration::LocConfigTypeEnum    requestType,
                               location_integration::LocIntegrationResponse response) {
    printf("<<< onConfigResponseCb, type %d, err %d\n", requestType, response);
}

static void onGetRobustLocationConfigCb(RobustLocationConfig robustLocationConfig) {
    printf("<<< onGetRobustLocationConfigCb, valid flags 0x%x, enabled %d, enabledForE911 %d\n",
           robustLocationConfig.validMask, robustLocationConfig.enabled,
           robustLocationConfig.enabledForE911);
}

static void printHelp() {
    printf("g: Gnss report session with 1000 ms interval\n");
    printf("u: Update a session with 2000 ms interval\n");
    printf("m: Interleaving fix session with 1000 and 2000 ms interval, change every 3 seconds\n");
    printf("s: Stop a session \n");
    printf("p: Ping test\n");
    printf("q: Quit\n");
    printf("r: delete client\n");
    printf("%s tuncThreshold energyBudget: enable TUNC\n", ENABLE_TUNC);
    printf("%s: disable TUNC\n", DISABLE_TUNC);
    printf("%s: enable PACE\n", ENABLE_PACE);
    printf("%s: disable PACE\n", DISABLE_PACE);
    printf("%s: reset sv config to default\n", RESET_SV_CONFIG);
    printf("%s: configure sv \n", CONFIG_SV);
    printf("%s: mulitple config SV \n", MULTI_CONFIG_SV);
    printf("%s: delete all aiding data\n", DELETE_ALL);
    printf("%s: config lever arm\n", CONFIG_LEVER_ARM);
    printf("%s: config robust location\n", CONFIG_ROBUST_LOCATION);
    printf("%s: get robust location config\n", GET_ROBUST_LOCATION_CONFIG);
}

void setRequiredPermToRunAsLocClient()
{
#ifdef USE_GLIB
    if (0 == getuid()) {
        printf("Test app set perm");

        // For LE, we need to also add "diag" to existing groups
        // to run in loc client mode for diag logging.
        // We need to add diag grpid at end, so MAX+1
        gid_t appGrpsIds[LOC_PROCESS_MAX_NUM_GROUPS+1] = {};
        int i = 0, numGrpIds = 0;

        // Get current set of supplementary groups.
        memset(appGrpsIds, 0, sizeof(appGrpsIds));
        numGrpIds = getgroups(LOC_PROCESS_MAX_NUM_GROUPS, appGrpsIds);
        if (numGrpIds == -1) {
            printf("Could not find groups. ngroups:%d\n", numGrpIds);
            numGrpIds = 0;
        }
        else {
            printf("Curr num_groups = %d, Current GIDs: ", numGrpIds);
            for (i=0; i<numGrpIds; i++) {
                printf("%d ", appGrpsIds[i]);
            }
        }

        // get group id for diag and update supplementary groups to add "diag" to its list.
        struct group* grp = getgrnam("diag");
        if (grp) {
            // we added diag also to grpid array and setgroups
            appGrpsIds[numGrpIds] = grp->gr_gid;
            printf("Diag Group id = %d", appGrpsIds[numGrpIds]);
            numGrpIds++;
            i = setgroups(numGrpIds, appGrpsIds);
            if (i == -1) {
                printf("Could not set groups. errno:%d, %s", errno, strerror(errno));
            } else {
                printf("Total of %d groups set for test app", numGrpIds);
            }
        }
        if (-1 == setgid(GID_LOCCLIENT)) {
            printf("%s:%d]: Error: setgid GID_LOCCLIENT failed. %s\n",
                     __func__, __LINE__, strerror(errno));
        }
        if (-1 == setuid(UID_LOCCLIENT)) {
            printf("%s:%d]: Error: setuid UID_LOCCLIENT failed. %s\n",
                     __func__, __LINE__, strerror(errno));
        }
    } else {
        printf("Test app started as user: %d", getuid());
    }
#endif// USE_GLIB
}

void parseSVConfig (char* buf, LocConfigBlacklistedSvIdList &svList) {
    static char *save = nullptr;
    char* token = strtok_r(buf, " ", &save); // skip first one of "configSV"
    token = strtok_r(NULL, " ", &save);
    if (token == nullptr) {
        return;
    }

    while (token != nullptr) {
        GnssSvIdInfo svIdInfo = {};
        svIdInfo.constellation = (GnssConstellationType) atoi(token);
        token = strtok_r(NULL, " ", &save);
        if (token == NULL) {
            break;
        }
        svIdInfo.svId = atoi(token);
        svList.push_back(svIdInfo);
        token = strtok_r(NULL, " ", &save);
    }
}

void parseLeverArm (char* buf, LeverArmParamsMap &leverArmMap) {
    static char *save = nullptr;
    char* token = strtok_r(buf, " ", &save); // skip first one of "configLeverArm"
    token = strtok_r(NULL, " ", &save);
    while (token != nullptr) {
        LeverArmParams leverArm = {};
        LeverArmType type = (LeverArmType) 0;
        if (strcmp(token, "vrp") == 0) {
            type = location_integration::LEVER_ARM_TYPE_GNSS_TO_VRP;
        } else if (strcmp(token, "drimu") == 0) {
            type = location_integration::LEVER_ARM_TYPE_DR_IMU_TO_GNSS;
        } else if (strcmp(token, "veppimu") == 0) {
            type = location_integration::LEVER_ARM_TYPE_VEPP_IMU_TO_GNSS;
        } else {
            break;
        }
        token = strtok_r(NULL, " ", &save);
        if (token == NULL) {
            break;
        }
        leverArm.forwardOffsetMeters = atof(token);

        token = strtok_r(NULL, " ", &save);
        if (token == NULL) {
            break;
        }
        leverArm.sidewaysOffsetMeters = atof(token);

        token = strtok_r(NULL, " ", &save);
        if (token == NULL) {
            break;
        }
        leverArm.upOffsetMeters = atof(token);

        printf("type %d, %f %f %f\n", type, leverArm.forwardOffsetMeters,
               leverArm.sidewaysOffsetMeters, leverArm.upOffsetMeters);

        leverArmMap.emplace(type, leverArm);
        token = strtok_r(NULL, " ", &save);
    }
}

/******************************************************************************
Main function
******************************************************************************/
int main(int argc, char *argv[]) {

    setRequiredPermToRunAsLocClient();

    // create Location client API
    LocationClientApi* pClient = new LocationClientApi(onCapabilitiesCb);

    // callbacks
    GnssReportCbs reportcbs;
    reportcbs.gnssLocationCallback = GnssLocationCb(onGnssLocationCb);
    reportcbs.gnssSvCallback = GnssSvCb(onGnssSvCb);
    reportcbs.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb);

    LocationClientApi* pClient2 = new LocationClientApi(onCapabilitiesCb2);
    // callbacks
    GnssReportCbs reportcbs2;
    reportcbs2.gnssLocationCallback = GnssLocationCb(onGnssLocationCb2);
    reportcbs2.gnssSvCallback = GnssSvCb(onGnssSvCb2);
    reportcbs2.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb2);

    // create location integratin API
    LocIntegrationCbs intCbs;

    intCbs.configCb = LocConfigCb(onConfigResponseCb);
    intCbs.getRobustLocationConfigCb =
            LocConfigGetRobustLocationConfigCb(onGetRobustLocationConfigCb);

    LocConfigPriorityMap priorityMap;
    location_integration::LocationIntegrationApi* pIntClient =
            new LocationIntegrationApi(priorityMap, intCbs);

    printHelp();
    sleep(1); // wait for capability callback if you don't like sleep

    // main loop
    while (1) {
        char buf[100];
        memset (buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);

        printf("execute command %s\n", buf);
        if (!pIntClient) {
            pIntClient =
                    new LocationIntegrationApi(priorityMap, intCbs);
            if (nullptr == pIntClient) {
                printf("failed to create integration client\n");
                break;
            }
            sleep(1); // wait for capability callback
        }

        if (strncmp(buf, DISABLE_TUNC, strlen(DISABLE_TUNC)) == 0) {
            pIntClient->configConstrainedTimeUncertainty(false);
        } else if (strncmp(buf, ENABLE_TUNC, strlen(ENABLE_TUNC)) == 0) {
            // get tuncThreshold and energyBudget from the command line
            static char *save = nullptr;
            float tuncThreshold = 0.0;
            int   energyBudget = 0;
            char* token = strtok_r(buf, " ", &save); // skip first one
            token = strtok_r(NULL, " ", &save);
            if (token != NULL) {
                tuncThreshold = atof(token);
                token = strtok_r(NULL, " ", &save);
                if (token != NULL) {
                    energyBudget = atoi(token);
                }
            }
            printf("tuncThreshold %f, energyBudget %d\n", tuncThreshold, energyBudget);
            pIntClient->configConstrainedTimeUncertainty(
                    true, tuncThreshold, energyBudget);
        } else if (strncmp(buf, DISABLE_PACE, strlen(DISABLE_TUNC)) == 0) {
            pIntClient->configPositionAssistedClockEstimator(false);
        } else if (strncmp(buf, ENABLE_PACE, strlen(ENABLE_TUNC)) == 0) {
            pIntClient->configPositionAssistedClockEstimator(true);
        } else if (strncmp(buf, DELETE_ALL, strlen(DELETE_ALL)) == 0) {
            pIntClient->deleteAllAidingData();
        } else if (strncmp(buf, RESET_SV_CONFIG, strlen(RESET_SV_CONFIG)) == 0) {
            pIntClient->configConstellations(nullptr);
        } else if (strncmp(buf, CONFIG_SV, strlen(CONFIG_SV)) == 0) {
            LocConfigBlacklistedSvIdList svList;
            parseSVConfig(buf, svList);
            pIntClient->configConstellations(&svList);
        } else if (strncmp(buf, MULTI_CONFIG_SV, strlen(MULTI_CONFIG_SV)) == 0) {
            // reset
            pIntClient->configConstellations(nullptr);
            // disable GAL
            LocConfigBlacklistedSvIdList galSvList;
            GnssSvIdInfo svIdInfo = {};
            svIdInfo.constellation = GNSS_CONSTELLATION_TYPE_GALILEO;
            svIdInfo.svId = 0;
            galSvList.push_back(svIdInfo);
            pIntClient->configConstellations(&galSvList);

            // disable SBAS
            LocConfigBlacklistedSvIdList sbasSvList;
            svIdInfo.constellation = GNSS_CONSTELLATION_TYPE_SBAS;
            svIdInfo.svId = 0;
            sbasSvList.push_back(svIdInfo);
            pIntClient->configConstellations(&sbasSvList);
        } else if (strncmp(buf, CONFIG_LEVER_ARM, strlen(CONFIG_LEVER_ARM)) == 0) {
            LeverArmParamsMap configInfo;
            parseLeverArm(buf, configInfo);
            pIntClient->configLeverArm(configInfo);
        } else if (strncmp(buf, CONFIG_ROBUST_LOCATION, strlen(CONFIG_ROBUST_LOCATION)) == 0) {
            // get enable and enableForE911
            static char *save = nullptr;
            bool enable = false;
            bool enableForE911 = false;
            // skip first one of configRobustLocation
            char* token = strtok_r(buf, " ", &save);
            token = strtok_r(NULL, " ", &save);
            if (token != NULL) {
                enable = (atoi(token) == 1);
                token = strtok_r(NULL, " ", &save);
                if (token != NULL) {
                    enableForE911 = (atoi(token) == 1);
                }
            }
            printf("enable %d, enableForE911 %d\n", enable, enableForE911);
            pIntClient->configRobustLocation(enable, enableForE911);
        } else if (strncmp(buf, GET_ROBUST_LOCATION_CONFIG,
                           strlen(GET_ROBUST_LOCATION_CONFIG)) == 0) {
            pIntClient->getRobustLocationConfig();
        } else {
            int command = buf[0];
            switch(command) {
            case 'g':
                if (!pClient) {
                    pClient = new LocationClientApi(onCapabilitiesCb);
                }
                if (pClient) {
                    pClient->startPositionSession(1000, reportcbs, onResponseCb);
                }
                break;
            case 'u':
                if (!pClient) {
                    pClient = new LocationClientApi(onCapabilitiesCb);
                }
                if (pClient) {
                    pClient->startPositionSession(2000, reportcbs, onResponseCb);
                }
                break;
            case 's':
                if (pClient) {
                    pClient->stopPositionSession();
                    delete pClient;
                    pClient = NULL;
                }
                break;
            case 'm':
                if (!pClient) {
                    pClient = new LocationClientApi(onCapabilitiesCb);
                }
                if (pClient) {
                    int i = 0;
                    do {
                        if (i%2 == 0) {
                            pClient->startPositionSession(2000, reportcbs, onResponseCb);
                        } else {
                            pClient->startPositionSession(1000, reportcbs, onResponseCb);
                        }
                        i++;
                        sleep(3);
                    } while (1);
                }
                break;
            case 'r':
                if (nullptr != pClient) {
                    delete pClient;
                    pClient = nullptr;
                }
                if (nullptr != pClient2) {
                    delete pClient2;
                    pClient2 = nullptr;
                }
                if (nullptr != pIntClient) {
                    delete pIntClient;
                    pIntClient = nullptr;
                }
                break;
            case 'q':
                goto EXIT;
                break;
            default:
                printf("unknown command %s\n", buf);
                break;
            }
        }
    }//while(1)

EXIT:
    if (nullptr != pClient) {
        delete pClient;
    }

    if (nullptr != pIntClient) {
        delete pIntClient;
    }

    printf("Done\n");
    exit(0);
}
