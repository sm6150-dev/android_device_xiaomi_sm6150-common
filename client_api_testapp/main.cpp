/* Copyright (c) 2019 The Linux Foundation. All rights reserved.
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

#include <LocationClientApi.h>
using namespace location_client;

#define NUM_LOOP_PINGTEST (1000)

// debug events counter
static uint32_t numLocationCb = 0;
static uint32_t numGnssLocationCb = 0;
static uint32_t numGnssSvCb = 0;
static uint32_t numGnssNmeaCb = 0;
static sem_t sem_pingcbreceived;

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

static void printHelp() {
    printf("g: Gnss report session with 1000 ms interval\n");
    printf("u: Update a session with 2000 ms interval\n");
    printf("m: Interleaving fix session with 1000 and 2000 ms interval, change every 3 seconds\n");
    printf("s: Stop a session \n");
    printf("p: Ping test\n");
    printf("q: Quit\n");
}

/******************************************************************************
Main function
******************************************************************************/
int main(int argc, char *argv[]) {

    // create Location client API
    LocationClientApi* pClient = new LocationClientApi(onCapabilitiesCb);

    // callbacks
    GnssReportCbs reportcbs;
    reportcbs.gnssLocationCallback = GnssLocationCb(onGnssLocationCb);
    reportcbs.gnssSvCallback = GnssSvCb(onGnssSvCb);
    reportcbs.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb);

    //LocationClientApi* pClient2 = new LocationClientApi(onCapabilitiesCb2);
    // callbacks
    GnssReportCbs reportcbs2;
    reportcbs2.gnssLocationCallback = GnssLocationCb(onGnssLocationCb2);
    reportcbs2.gnssSvCallback = GnssSvCb(onGnssSvCb2);
    reportcbs2.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb2);

    printHelp();
    sleep(1); // wait for capability callback if you don't like sleep

    // main loop
    while (1) {

        printf("Command:\n");
        char buf[100];
        fgets(buf, sizeof(buf), stdin);
        int command = buf[0];//getchar();
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
            case 'q':
                goto EXIT;
                break;
            default:
                printf("Unknown command...\n");
                break;
        }
    }//while(1)

EXIT:
    if (nullptr != pClient) {
        delete pClient;
    }

    printf("Done\n");
    exit(0);
}
