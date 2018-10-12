/* Copyright (c) 2018 The Linux Foundation. All rights reserved.
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

#define LOG_TAG "LocSvc_LocationClientApi"

#include <loc_cfg.h>

#include <LocationDataTypes.h>
#include <LocationClientApi.h>
#include <LocationClientApiImpl.h>
#include <LocationApiMsg.h>
#include <log_util.h>

namespace location_client {

static uint32_t gDebug = 0;

static const loc_param_s_type gConfigTable[] =
{
    {"DEBUG_LEVEL", &gDebug, NULL, 'n'}
};

/******************************************************************************
LocationClientApi
******************************************************************************/
LocationClientApi::LocationClientApi(CapabilitiesCb capabitiescb) :
        mApiImpl(new LocationClientApiImpl(capabitiescb)) {

    // read configuration file
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gConfigTable);
    LOC_LOGd("gDebug=%u", gDebug);
}

LocationClientApi::~LocationClientApi() {
    if (mApiImpl) {
        delete mApiImpl;
    }
}

bool LocationClientApi::startPositionSession(
        uint32_t intervalInMs,
        uint32_t distanceInMeters,
        LocationCb locationCallback,
        ResponseCb responseCallback) {

    //Input parameter check
    if (!locationCallback) {
        LOC_LOGe ("NULL locationCallback");
        return false;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }

    // callback functions
    ClientCallbacks cbs = {0};
    cbs.responsecb = responseCallback;
    cbs.locationcb = locationCallback;
    mApiImpl->updateCallbackFunctions(cbs);

    // callback masks
    LocationCallbacks callbacksOption = {0};
    callbacksOption.responseCb = [](::LocationError err, uint32_t id) {};
    callbacksOption.trackingCb = [](::Location n) {};
    mApiImpl->updateCallbacks(callbacksOption);

    // options
    LocationOptions locationOption;
    TrackingOptions trackingOption;
    locationOption.minInterval = intervalInMs;
    locationOption.minDistance = distanceInMeters;
    trackingOption.setLocationOptions(locationOption);
    mApiImpl->startTracking(trackingOption);
    return true;
}

bool LocationClientApi::startPositionSession(
        uint32_t intervalInMs,
        const GnssReportCbs& gnssReportCallbacks,
        ResponseCb responseCallback) {

    //Input parameter check
    if (!gnssReportCallbacks.gnssLocationCallback) {
        LOC_LOGe ("gnssLocation Callbacks can't be NULL");
        return false;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }

    // callback functions
    ClientCallbacks cbs = {0};
    cbs.responsecb = responseCallback;
    cbs.gnssreportcbs = gnssReportCallbacks;
    mApiImpl->updateCallbackFunctions(cbs);

    // callback masks
    LocationCallbacks callbacksOption = {0};
    callbacksOption.responseCb = [](::LocationError err, uint32_t id) {};
    if (gnssReportCallbacks.gnssLocationCallback) {
        callbacksOption.gnssLocationInfoCb = [](::GnssLocationInfoNotification n) {};
    }
    if (gnssReportCallbacks.gnssSvCallback) {
        callbacksOption.gnssSvCb = [](::GnssSvNotification n) {};
    }
    if (gnssReportCallbacks.gnssNmeaCallback) {
        callbacksOption.gnssNmeaCb = [](::GnssNmeaNotification n) {};
    }
    if (gnssReportCallbacks.gnssDataCallback) {
       callbacksOption.gnssDataCb = [] (::GnssDataNotification n) {};
    }
    mApiImpl->updateCallbacks(callbacksOption);

    // options
    LocationOptions locationOption;
    TrackingOptions trackingOption;
    locationOption.minInterval = intervalInMs;
    locationOption.minDistance = 0;
    trackingOption.setLocationOptions(locationOption);
    mApiImpl->startTracking(trackingOption);
    return true;
}

void LocationClientApi::stopPositionSession() {
    if (mApiImpl) {
        mApiImpl->stopTracking(0);
    }
}

void LocationClientApi::updateNetworkAvailability(bool available) {
    if (mApiImpl) {
        mApiImpl->updateNetworkAvailability(available);
    }
}

void LocationClientApi::getGnssEnergyConsumed(
        GnssEnergyConsumedCb gnssEnergyConsumedCallback,
        ResponseCb responseCallback) {

    if (mApiImpl) {
        mApiImpl->getGnssEnergyConsumed(gnssEnergyConsumedCallback,
                                        responseCallback);
    }
}

void LocationClientApi::updateLocationSystemInfoListener(
    LocationSystemInfoCb locSystemInfoCallback,
    ResponseCb responseCallback) {

    if (mApiImpl) {
        mApiImpl->updateLocationSystemInfoListener(
            locSystemInfoCallback, responseCallback);
    }
}

} // namespace location_client

