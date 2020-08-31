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

#ifndef WIFI_DB_REC_H
#define WIFI_DB_REC_H

#include <functional>
#include <vector>
#include <string>
#include <DBCommon.h>

using namespace std;

namespace nlp_api
{

/******************************************************************************
WiFiDBReceiver
******************************************************************************/

enum ReliabilityTypes : uint8_t {
    VERY_LOW  = 0,
    LOW       = 1,
    MEDIUM    = 2,
    HIGH      = 3,
    VERY_HIGH = 4,
};

struct APLocationData {
    string macAddress;
    double latitude;
    double longitude;
    float max_antenna_range;
    float horizontal_error;
    ReliabilityTypes reliability;
};

typedef vector<const shared_ptr<APLocationData>> APLocationDataList;

enum APSpecialInfoType : uint8_t {
    NO_INFO_AVAILABLE = 0,
    MOVING_AP      = 1,
};

struct APSpecialInfo {
    string macAddress;
    APSpecialInfoType specialInfoType;
};

typedef vector<const shared_ptr<APSpecialInfo>> APSpecialInfoList;

class WiFiDBReceiver
{
public:
    virtual void requestAPList(int expire_in_days) = 0;
    virtual void requestScanList() = 0;
    virtual void pushWiFiDB(const shared_ptr<APLocationDataList> location_data,
            const shared_ptr<APSpecialInfoList> special_info, int days_valid) = 0;
    virtual void pushLookupResult(const shared_ptr<APLocationDataList> location_data,
            const shared_ptr<APSpecialInfoList> special_info) = 0;
    virtual ~WiFiDBReceiver() { };
};

/******************************************************************************
ResponseListener
******************************************************************************/

enum FdalStatus : uint8_t {
    NOT_IN_FDAL = 0,
    IN_FDAL     = 1,
    BLACKLISTED = 2,
    NA          = 3,
};

struct APInfoExtra {
    string ssid;
    CellInfo cellInfo;
};

struct APInfo {
    string macAddress;
    uint64_t timestamp;
    FdalStatus fdalStatus;
    APInfoExtra extra;
};

typedef vector<unique_ptr<APInfo>> APInfoList;

typedef function<void(
    unique_ptr<APInfoList> ap_list,
    ApBsListStatus ap_status,
    Location location
)> APListAvailable;

typedef function<void(
    unique_ptr<APInfoList> ap_list,
    Location location
)> LookupRequest;

typedef function<void(
    bool is_success,
    string error
)> StatusUpdate;

struct WiFiDBReceiverResponseListener {
    APListAvailable onAPListAvailable;
    LookupRequest onLookupRequest;
    StatusUpdate onStatusUpdate;
    ServiceRequest onServiceRequest;
};

} // namespace nlp_api

#endif /* WIFI_DB_REC_H */
