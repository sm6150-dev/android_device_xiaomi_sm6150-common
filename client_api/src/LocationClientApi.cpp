/* Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
#include <cmath>
#include <LocationDataTypes.h>
#include <LocationClientApi.h>
#include <LocationClientApiImpl.h>
#include <LocationApiMsg.h>
#include <log_util.h>

namespace location_client {



bool Geofence::operator==(Geofence& other) {
    return mGeofenceImpl != nullptr && other.mGeofenceImpl != nullptr &&
            mGeofenceImpl == other.mGeofenceImpl;
}

/******************************************************************************
LocationClientApi
******************************************************************************/
LocationClientApi::LocationClientApi(CapabilitiesCb capabitiescb) :
        mApiImpl(new LocationClientApiImpl(capabitiescb)) {
}

LocationClientApi::~LocationClientApi() {
    if (mApiImpl) {
        // two steps processes due to asynchronous message processing
        mApiImpl->destroy();
        // deletion of mApiImpl will be done after messages in the queue are processed
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
    // only register for trackingCb if distance is not 0
    if (distanceInMeters != 0) {
        callbacksOption.trackingCb = [](::Location n) {};
    } else {
        // for time based, register gnss location cb
        callbacksOption.gnssLocationInfoCb = [](::GnssLocationInfoNotification n) {};
    }
    mApiImpl->updateCallbacks(callbacksOption);

    // options
    LocationOptions locationOption;
    TrackingOptions trackingOption;
    locationOption.size = sizeof(locationOption);
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
    mApiImpl->updateCallbackFunctions(cbs, REPORT_CB_GNSS_INFO);

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
    if (gnssReportCallbacks.gnssMeasurementsCallback) {
        callbacksOption.gnssMeasurementsCb = [](::GnssMeasurementsNotification n) {};
    }
    mApiImpl->updateCallbacks(callbacksOption);

    // options
    LocationOptions locationOption;
    TrackingOptions trackingOption;
    locationOption.size = sizeof(locationOption);
    locationOption.minInterval = intervalInMs;
    locationOption.minDistance = 0;
    trackingOption.setLocationOptions(locationOption);
    mApiImpl->startTracking(trackingOption);
    return true;
}

bool LocationClientApi::startPositionSession(
        uint32_t intervalInMs,
        LocReqEngineTypeMask locEngReqMask,
        const EngineReportCbs& engReportCallbacks,
        ResponseCb responseCallback) {

    //Input parameter check
    if (!engReportCallbacks.engLocationsCallback) {
        LOC_LOGe ("engLocations Callbacks can't be NULL");
        return false;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }

    // callback functions
    ClientCallbacks cbs = {0};
    cbs.responsecb = responseCallback;
    cbs.engreportcbs = engReportCallbacks;
    mApiImpl->updateCallbackFunctions(cbs, REPORT_CB_ENGINE_INFO);

    // callback masks
    LocationCallbacks callbacksOption = {0};
    callbacksOption.responseCb = [](::LocationError err, uint32_t id) {};

    if (engReportCallbacks.engLocationsCallback) {
        callbacksOption.engineLocationsInfoCb =
                [](uint32_t count, ::GnssLocationInfoNotification* locArr) {};
    }
    if (engReportCallbacks.gnssSvCallback) {
        callbacksOption.gnssSvCb = [](::GnssSvNotification n) {};
    }
    if (engReportCallbacks.gnssNmeaCallback) {
        callbacksOption.gnssNmeaCb = [](::GnssNmeaNotification n) {};
    }
    if (engReportCallbacks.gnssDataCallback) {
       callbacksOption.gnssDataCb = [] (::GnssDataNotification n) {};
    }
    if (engReportCallbacks.gnssMeasurementsCallback) {
        callbacksOption.gnssMeasurementsCb = [](::GnssMeasurementsNotification n) {};
    }
    mApiImpl->updateCallbacks(callbacksOption);

    // options
    LocationOptions locationOption;
    TrackingOptions trackingOption;
    locationOption.size = sizeof(locationOption);
    locationOption.minInterval = intervalInMs;
    locationOption.minDistance = 0;
    locationOption.locReqEngTypeMask =(::LocReqEngineTypeMask)locEngReqMask;
    trackingOption.setLocationOptions(locationOption);

    mApiImpl->startTracking(trackingOption);
    return true;
}

void LocationClientApi::stopPositionSession() {
    if (mApiImpl) {
        mApiImpl->stopTracking(0);
    }
}

bool LocationClientApi::startTripBatchingSession(uint32_t minInterval, uint32_t tripDistance,
        BatchingCb batchingCallback, ResponseCb responseCallback) {
    //Input parameter check
    if (!batchingCallback) {
        LOC_LOGe ("NULL batchingCallback");
        return false;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
    // callback functions
    ClientCallbacks cbs = {0};
    cbs.responsecb = responseCallback;
    cbs.batchingcb = batchingCallback;
    mApiImpl->updateCallbackFunctions(cbs);

    // callback masks
    LocationCallbacks callbacksOption = {0};
    callbacksOption.responseCb = [](::LocationError err, uint32_t id) {};
    callbacksOption.batchingCb = [](size_t count, ::Location* location,
            BatchingOptions batchingOptions) {};
    callbacksOption.batchingStatusCb = [](BatchingStatusInfo batchingStatus,
            std::list<uint32_t>& listOfcompletedTrips) {};
    mApiImpl->updateCallbacks(callbacksOption);

    LocationOptions locOption = {};
    locOption.size = sizeof(locOption);
    locOption.minInterval = minInterval;
    locOption.minDistance = tripDistance;
    locOption.mode = GNSS_SUPL_MODE_STANDALONE;

    BatchingOptions     batchOption = {};
    batchOption.size = sizeof(batchOption);
    batchOption.batchingMode = BATCHING_MODE_TRIP;
    batchOption.setLocationOptions(locOption);
    mApiImpl->startBatching(batchOption);
    return true;
}

bool LocationClientApi::startRoutineBatchingSession(uint32_t minInterval, uint32_t minDistance,
        BatchingCb batchingCallback, ResponseCb responseCallback) {
    //Input parameter check
    if (!batchingCallback) {
        LOC_LOGe ("NULL batchingCallback");
        return false;
    }

    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return false;
    }
    // callback functions
    ClientCallbacks cbs = {0};
    cbs.responsecb = responseCallback;
    cbs.batchingcb = batchingCallback;
    mApiImpl->updateCallbackFunctions(cbs);

    // callback masks
    LocationCallbacks callbacksOption = {0};
    callbacksOption.responseCb = [](::LocationError err, uint32_t id) {};
    callbacksOption.batchingCb = [](size_t count, ::Location* location,
            BatchingOptions batchingOptions) {};
    mApiImpl->updateCallbacks(callbacksOption);

    LocationOptions locOption = {};
    locOption.size = sizeof(locOption);
    locOption.minInterval = minInterval;
    locOption.minDistance = minDistance;
    locOption.mode = GNSS_SUPL_MODE_STANDALONE;

    BatchingOptions     batchOption = {};
    batchOption.size = sizeof(batchOption);
    batchOption.batchingMode = BATCHING_MODE_ROUTINE;
    batchOption.setLocationOptions(locOption);
    mApiImpl->startBatching(batchOption);
    return true;
}

void LocationClientApi::stopBatchingSession() {
    if (mApiImpl) {
        mApiImpl->stopBatching(0);
    }
}

void LocationClientApi::addGeofences(std::vector<Geofence>& geofences,
        GeofenceBreachCb gfBreachCb,
        CollectiveResponseCb responseCallback) {
    //Input parameter check
    if (!gfBreachCb) {
        LOC_LOGe ("NULL GeofenceBreachCb");
        return;
    }
    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return;
    }
    // callback functions
    ClientCallbacks cbs = {0};
    cbs.collectivecb = responseCallback;
    cbs.gfbreachcb = gfBreachCb;
    mApiImpl->updateCallbackFunctions(cbs);

    // callback masks
    LocationCallbacks callbacksOption = {0};
    callbacksOption.responseCb = [](LocationError err, uint32_t id) {};
    callbacksOption.collectiveResponseCb = [](size_t, LocationError*, uint32_t*) {};
    callbacksOption.geofenceBreachCb = [](GeofenceBreachNotification geofenceBreachNotification)
            {};
    mApiImpl->updateCallbacks(callbacksOption);
    size_t count = geofences.size();
    mApiImpl->mLastAddedClientIds.clear();
    if (count > 0) {
        GeofenceOption* gfOptions = (GeofenceOption*)malloc(sizeof(GeofenceOption) * count);
        GeofenceInfo* gfInfos = (GeofenceInfo*)malloc(sizeof(GeofenceInfo) * count);

        for (int i=0; i<count; ++i) {
            if (geofences[i].mGeofenceImpl) {
                continue;
            }
            gfOptions[i].breachTypeMask = geofences[i].getBreachType();
            gfOptions[i].responsiveness = geofences[i].getResponsiveness();
            gfOptions[i].dwellTime = geofences[i].getDwellTime();
            gfOptions[i].size = sizeof(gfOptions[i]);
            gfInfos[i].latitude = geofences[i].getLatitude();
            gfInfos[i].longitude = geofences[i].getLongitude();
            gfInfos[i].radius = geofences[i].getRadius();
            gfInfos[i].size = sizeof(gfInfos[i]);
            std::shared_ptr<GeofenceImpl> gfImpl(new GeofenceImpl(&geofences[i]));
            gfImpl->bindGeofence(&geofences[i]);
            mApiImpl->mLastAddedClientIds.push_back(gfImpl->getClientId());
            mApiImpl->addGeofenceMap(mApiImpl->mLastAddedClientIds[i], geofences[i]);
        }

        mApiImpl->addGeofences(geofences.size(), reinterpret_cast<GeofenceOption*>(gfOptions),
                reinterpret_cast<GeofenceInfo*>(gfInfos));
    }
}
void LocationClientApi::removeGeofences(std::vector<Geofence>& geofences) {
    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return;
    }
    size_t count = geofences.size();
    if (count > 0) {
        uint32_t* gfIds = (uint32_t*)malloc(sizeof(uint32_t) * count);
        for (int i=0; i<count; ++i) {
            if (!geofences[i].mGeofenceImpl) {
                LOC_LOGe ("Geofence not added yet");
                free(gfIds);
                return;
            }
            gfIds[i] = geofences[i].mGeofenceImpl->getClientId();
        }
        if (!mApiImpl->checkGeofenceMap(geofences.size(), gfIds)) {
            LOC_LOGe ("Wrong geofence IDs");
            free(gfIds);
            return;
        }
        mApiImpl->removeGeofences(count, gfIds);
    }
}
void LocationClientApi::modifyGeofences(std::vector<Geofence>& geofences) {
    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return;
    }
    size_t count = geofences.size();
    if (count > 0) {
        GeofenceOption* gfOptions = (GeofenceOption*)malloc(sizeof(GeofenceOption) * count);
        uint32_t* gfIds = (uint32_t*)malloc(sizeof(uint32_t) * count);
        for (int i=0; i<count; ++i) {
            gfOptions[i].breachTypeMask = geofences[i].getBreachType();
            gfOptions[i].responsiveness = geofences[i].getResponsiveness();
            gfOptions[i].dwellTime = geofences[i].getDwellTime();
            gfOptions[i].size = sizeof(gfOptions[i]);
            if (!geofences[i].mGeofenceImpl) {
                LOC_LOGe ("Geofence not added yet");
                free(gfIds);
                free(gfOptions);
                return;
            }
            gfIds[i] = geofences[i].mGeofenceImpl->getClientId();
        }
        if (!mApiImpl->checkGeofenceMap(geofences.size(), gfIds)) {
            LOC_LOGe ("Wrong geofence IDs");
            free(gfIds);
            free(gfOptions);
            return;
        }
        mApiImpl->modifyGeofences(geofences.size(), const_cast<uint32_t*>(gfIds),
                reinterpret_cast<GeofenceOption*>(gfOptions));
    }
}

void LocationClientApi::pauseGeofences(std::vector<Geofence>& geofences) {
    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return;
    }
    size_t count = geofences.size();
    if (count > 0) {
        uint32_t* gfIds = (uint32_t*)malloc(sizeof(uint32_t) * count);
        for (int i=0; i<count; ++i) {
            if (!geofences[i].mGeofenceImpl) {
                LOC_LOGe ("Geofence not added yet");
                free(gfIds);
                return;
            }
            gfIds[i] = geofences[i].mGeofenceImpl->getClientId();
        }
        if (!mApiImpl->checkGeofenceMap(geofences.size(), gfIds)) {
            LOC_LOGe ("Wrong geofence IDs");
            free(gfIds);
            return;
        }
        mApiImpl->pauseGeofences(count, gfIds);
    }
}

void LocationClientApi::resumeGeofences(std::vector<Geofence>& geofences) {
    if (!mApiImpl) {
        LOC_LOGe ("NULL mApiImpl");
        return;
    }
    size_t count = geofences.size();
    if (count > 0) {
        uint32_t* gfIds = (uint32_t*)malloc(sizeof(uint32_t) * count);
        for (int i=0; i<count; ++i) {
            if (!geofences[i].mGeofenceImpl) {
                LOC_LOGe ("Geofence not added yet");
                free(gfIds);
                return;
            }
            gfIds[i] = geofences[i].mGeofenceImpl->getClientId();
        }
        if (!mApiImpl->checkGeofenceMap(geofences.size(), gfIds)) {
            LOC_LOGe ("Wrong geofence IDs");
            free(gfIds);
            return;
        }
        mApiImpl->resumeGeofences(count, gfIds);
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

    if (mApiImpl && gnssEnergyConsumedCallback) {
        mApiImpl->getGnssEnergyConsumed(gnssEnergyConsumedCallback,
                                        responseCallback);
    } else {
        if (responseCallback) {
            responseCallback(LOCATION_RESPONSE_PARAM_INVALID);
        }
    }
}

void LocationClientApi::updateLocationSystemInfoListener(
    LocationSystemInfoCb locSystemInfoCallback,
    ResponseCb responseCallback) {

    if (mApiImpl && locSystemInfoCallback) {
        mApiImpl->updateLocationSystemInfoListener(
            locSystemInfoCallback, responseCallback);
    } else {
        if (responseCallback) {
            responseCallback(LOCATION_RESPONSE_PARAM_INVALID);
        }
    }
}

} // namespace location_client
