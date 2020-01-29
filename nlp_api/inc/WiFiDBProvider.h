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

#ifndef WIFI_DB_PROV_H
#define WIFI_DB_PROV_H

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

class WiFiDBProvider
{
public:
    virtual void requestAPObsLocData() = 0;
    virtual ~WiFiDBProvider() { };
};

/******************************************************************************
ResponseListener
******************************************************************************/
struct ApScan {
    string macAddress;
    float rssi;
    uint64_t deltaTime;
    string ssid;
    uint16_t channelNumber;
};

typedef vector<unique_ptr<ApScan>> ApScanList;

struct APObsLocData {
    Location location;
    CellInfo cellInfo;
    uint64_t scanTimestamp;
    ApScanList ApScanList;
};

typedef vector<unique_ptr<APObsLocData>> APObsLocDataList;

typedef function<void(
    unique_ptr<APObsLocDataList> ap_obs_list,
    ApBsListStatus ap_status
)> ApObsLocDataAvailable;

struct WiFiDBProviderResponseListener {
    ApObsLocDataAvailable onApObsLocDataAvailable;
    ServiceRequest onServiceRequest;
};

} // namespace nlp_api

#endif /* WIFI_DB_PROV_H */
