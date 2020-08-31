/* Copyright (c) 2020 The Linux Foundation. All rights reserved.
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

#ifndef WIFI_DB_COMMON_H
#define WIFI_DB_COMMON_H

#include <functional>
#include <vector>
#include <string>

using namespace std;

namespace nlp_api
{

enum ApBsListStatus {
    STD_CONT    = 0,
    STD_FINAL   = 1,
    SCAN_FINAL  = 2,
};

enum CellType {
    GSM   = 0,
    WCDMA = 1,
    CDMA  = 2,
    LTE   = 3,
};

struct CellInfo {
    CellType cellType;
    uint16_t regionId1;
    uint16_t regionId2;
    uint16_t regionId3;
    uint16_t regionId4;
    uint32_t timestamp;
};

enum LocationPositionSource : uint8_t {
    GPS    = (1<<0),
    HYBRID = (1<<1),
};

enum LocationFlagsBits : uint16_t {
    LAT_LONG_BIT          = (1<<0), // location has valid latitude and longitude
    ALTITUDE_BIT          = (1<<1), // location has valid altitude
    SPEED_BIT             = (1<<2), // location has valid speed
    BEARING_BIT           = (1<<3), // location has valid bearing
    ACCURACY_BIT          = (1<<4), // location has valid accuracy
    VERTICAL_ACCURACY_BIT = (1<<5), // location has valid vertical accuracy
    SPEED_ACCURACY_BIT    = (1<<6), // location has valid speed accuracy
    BEARING_ACCURACY_BIT  = (1<<7), // location has valid bearing accuracy
};

struct Location {
    LocationPositionSource positionSource;
    LocationFlagsBits locationFlagsMask;
    uint64_t timestamp;
    double   latitude;
    double   longitude;
    double   altitude;
    float    speed;
    float    bearing;
    float    accuracy;
    float    verticalAccuracy;
    float    speedAccuracy;
    float    bearingAccuracy;
};

typedef function<void(
)> ServiceRequest;

enum OptInStatus {
    OPT_OUT = 0,
    OPT_IN  = 1,
};

typedef function<void(
     OptInStatus optInStatus
)> LocationOptInUpdate;

struct SystemStatusListener {
    LocationOptInUpdate onLocationOptInUpdate;
};

} // namespace nlp_api

#endif /* WIFI_DB_COMMON_H */
