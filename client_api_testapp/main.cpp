/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#include <LocationClientApi.h>

using namespace location_client;

/******************************************************************************
Callback functions
******************************************************************************/
static void onCapabilitiesCb(location_client::LocationCapabilitiesMask mask) {
    printf("<<< onCapabilitiesCb mask=0x%x \n", mask);
}

static void onResponseCb(location_client::LocationResponse response) {
    printf("<<< onResponseCb err=%u\n", response);
}

static void onLocationCb(const location_client::Location& location) {
    static bool printMessage = true;
    if (printMessage) {
    printf("<<< onLocationCb time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f\n",
        location.timestamp,
        location.flags,
        location.latitude,
        location.longitude,
        location.altitude);
        printMessage = false;
    }
}

static void onGnssLocationCb(const location_client::GnssLocation& location) {
    static bool printMessage = true;
    if (printMessage) {
        printf("<<< onGnssLocationCb time=%" PRIu64" mask=0x%x lat=%f lon=%f alt=%f\n",
            location.timestamp,
            location.flags,
            location.latitude,
            location.longitude,
            location.altitude);
        printMessage = false;
    }
}

static void onGnssSvCb(const std::vector<location_client::GnssSv>& gnssSvs) {
    static bool printMessage = true;
    if (printMessage) {
        printf("<<< onGnssSvCb count=%u\n", gnssSvs.size());
        for (int sv=0; sv < gnssSvs.size(); sv++) {
            printf("type=%u svid=%u\n",
                gnssSvs[sv].type,
                gnssSvs[sv].svId);
        }
        printMessage = false;
    }
}

static void onGnssNmeaCb(uint64_t timestamp, const std::string& nmea) {
    static bool printMessage = true;
    if (printMessage) {
        printf("<<< onGnssNmeaCb time=%" PRIu64" nmea=%s\n",
            timestamp, nmea.c_str());
        printMessage = false;
    }
}

static void help() {
    printf("location_client_api_testapp - 0424:1\n");

    printf("Test command format: client# actionType(g/l/s/u) "
           "interval(optional) distance(optional)\n");
    printf("Start client1 with Gnss Session type, interval 1000 ms, distance 0 meters: "
           "\"1 g 1000\" \n");
    printf("g: Gnss report session with all 3 types of callbacks\n");
    printf("g1: Gnss report session with LocationInfo callback\n");
    printf("g3: Gnss report session with LocationInfo and SV callbacks\n");
    printf("g5: Gnss report session with LocationInfo and NMEA callbacks\n");
    printf("l: Location session \n");
    printf("s: Stop a session \n");
    printf("u: Set network available \n");
    printf("d: Set network not available \n");
    printf("q: Quit\n");
    printf("\nEnter Command:");
    fflush (stdout);
}

#define BUFFERSIZE 50

enum ActionType{
    TEST_NO_ACTION = 0,
    TEST_GNSS_REPORT,
    TEST_LOCATION_REPORT,
    TEST_NETWORK_UP,
    TEST_NETWORK_DOWN,
    TEST_STOP
};

enum GnssCallbackOptionsMask{
    GNSS_LOCATION_BIT = (1<<0),
    GNSS_SV_BIT       = (1<<1),
    GNSS_NMEA_BIT     = (1<<2)
};

/******************************************************************************
Main function
******************************************************************************/
int main(int argc, char *argv[]) {

    help();

    // create Location client API
    LocationClientApi* pClient_1 = new LocationClientApi(onCapabilitiesCb);
    LocationClientApi* pClient_2 = nullptr;

    // wait for socket comes ready
    sleep(1);

    // main loop
    while (1) {
        char str[BUFFERSIZE];
        int len;

        printf("#command: \n");

        fgets(str, BUFFERSIZE, stdin);
        /* remove newline, if present */
        len = strlen(str)-1;
        if( str[len] == '\n') {
            str[len] = '\0';
        }

        if (str[0] != '1' && str[0] != '2') {
            if (str[0] == 'Q' || str[0] == 'q') {
                break;
            } else {
                help();
            }
        } else {
            char * pch;
            pch = strtok (str," ");
            uint32_t clientId = 0;
            uint32_t intervalInMs = 0;
            uint32_t distanceInMeters = 0;
            ActionType type = TEST_NO_ACTION;
            int gnssReportOptions = GNSS_LOCATION_BIT | GNSS_SV_BIT | GNSS_NMEA_BIT;
            GnssReportCbs reportcbs;
            int i=0;
            bool parseError = false;

            while (nullptr != pch) {
                switch (i) {
                    case 0:
                        clientId = std::stoul(pch,nullptr,10);
                        if ((2==clientId) && (nullptr == pClient_2) ) {
                            pClient_2 = new LocationClientApi(onCapabilitiesCb);
                        }
                        break;

                    case 1:
                        if (*pch == 'G' || *pch == 'g') {
                            type = TEST_GNSS_REPORT;
                            if (strlen(pch) > 1) {
                                pch++;
                                gnssReportOptions = std::stoul(pch,nullptr,10);
                            }

                            if (gnssReportOptions < GNSS_LOCATION_BIT) {
                                printf ("Gnss report option must great than 1, set to default\n" );
                                gnssReportOptions =
                                        GNSS_LOCATION_BIT | GNSS_SV_BIT | GNSS_NMEA_BIT;
                            }

                            printf ("gnssReportOptions is %d\n",gnssReportOptions);
                        } else if (*pch == 'L' || *pch == 'l') {
                            type = TEST_LOCATION_REPORT;
                        } else if (*pch == 'U' || *pch == 'u') {
                            type = TEST_NETWORK_UP;
                        } else if (*pch == 'D' || *pch == 'd') {
                            type = TEST_NETWORK_DOWN;
                        } else if (*pch == 'S' || *pch == 's') {
                            type = TEST_STOP;
                        } else{
                            printf ("Action type is not recognized %c \n",*pch);
                            parseError = true;
                            pch = nullptr;
                        }
                        break;

                    case 2:
                        intervalInMs = std::stoul(pch,nullptr,10);
                        break;

                    case 3:
                        distanceInMeters = std::stoul(pch,nullptr,10);
                        break;

                    default:
                        pch = nullptr;
                        break;
                }
                pch = strtok(nullptr, " ");
                i++;
            }

            if (parseError) {
                help();
            } else {
                switch (type) {
                    case TEST_GNSS_REPORT:
                        if (gnssReportOptions & GNSS_LOCATION_BIT) {
                            reportcbs.gnssLocationCallback = GnssLocationCb(onGnssLocationCb);
                        }
                        if (gnssReportOptions & GNSS_SV_BIT) {
                            reportcbs.gnssSvCallback = GnssSvCb(onGnssSvCb);
                        }
                        if (gnssReportOptions & GNSS_NMEA_BIT) {
                            reportcbs.gnssNmeaCallback = GnssNmeaCb(onGnssNmeaCb);
                        }

                        if (1 == clientId) {
                            pClient_1->startPositionSession(intervalInMs, reportcbs, onResponseCb);
                        } else {
                            pClient_2->startPositionSession(intervalInMs, reportcbs, onResponseCb);
                        }
                        break;

                    case TEST_LOCATION_REPORT:
                        if (1 == clientId) {
                            pClient_1->startPositionSession(
                                    intervalInMs, distanceInMeters, onLocationCb, onResponseCb);
                        } else {
                            pClient_2->startPositionSession(
                                    intervalInMs, distanceInMeters, onLocationCb, onResponseCb);
                        }
                        break;

                    case TEST_NETWORK_UP:
                        if (1 == clientId) {
                            pClient_1->updateNetworkAvailability(true);
                        } else {
                            pClient_2->updateNetworkAvailability(true);
                        }
                        break;

                    case TEST_NETWORK_DOWN:
                        if (1 == clientId) {
                            pClient_1->updateNetworkAvailability(false);
                        } else {
                            pClient_2->updateNetworkAvailability(false);
                        }
                        break;

                    case TEST_STOP:
                        if (1 == clientId) {
                            pClient_1->stopPositionSession();
                        } else {
                            pClient_2->stopPositionSession();
                        }
                        break;
                }
            }
        }
    }//while(1)

    if (nullptr != pClient_1) {
        delete pClient_1;
    }
    if (nullptr != pClient_2) {
        delete pClient_2;
    }
    printf("LocationClientApi test application exit\n");
    exit(0);
}
