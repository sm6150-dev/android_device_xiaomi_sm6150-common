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

#ifndef LOCATIONCLIENTAPIIMPL_H
#define LOCATIONCLIENTAPIIMPL_H

#include <mutex>

#include <LocIpc.h>
#include <LocationDataTypes.h>
#include <ILocationAPI.h>
#include <LocationClientApi.h>
#include <MsgTask.h>
#include <LocationApiMsg.h>


using namespace std;

using loc_util::LocIpc;
using loc_util::LocIpcSender;

namespace location_client
{

class LocationClientApiImpl : public LocIpc, public ILocationAPI,
                              public ILocationControlAPI {
public:
    LocationClientApiImpl(CapabilitiesCb capabitiescb);
    ~LocationClientApiImpl();

    // Tracking
    virtual void updateCallbacks(LocationCallbacks&) override;

    virtual uint32_t startTracking(TrackingOptions&) override;

    virtual void stopTracking(uint32_t id) override;

    virtual void updateTrackingOptions(uint32_t id, TrackingOptions&) override;

    //Batching
    virtual uint32_t startBatching(BatchingOptions&) override;

    virtual void stopBatching(uint32_t id) override;

    virtual void updateBatchingOptions(uint32_t id, BatchingOptions&) override;

    virtual void getBatchedLocations(uint32_t id, size_t count) override;

    //Geofence
    virtual uint32_t* addGeofences(size_t count, GeofenceOption*,
                                   GeofenceInfo*) override;

    virtual void removeGeofences(size_t count, uint32_t* ids) override;

    virtual void modifyGeofences(size_t count, uint32_t* ids,
                                 GeofenceOption* options) override;

    virtual void pauseGeofences(size_t count, uint32_t* ids) override;

    virtual void resumeGeofences(size_t count, uint32_t* ids) override;

    //GNSS
    virtual void gnssNiResponse(uint32_t id, GnssNiResponse response) override;

    // other
    virtual uint32_t* gnssUpdateConfig(GnssConfig config) override;
    virtual uint32_t gnssDeleteAidingData(GnssAidingData& data) override;

    // override from LocIpc
    virtual void onListenerReady() override;
    virtual void onReceive(const string& data) override;

    // other interface
    void updateNetworkAvailability(bool available);
    void updateCallbackFunctions(const ClientCallbacks&);

private:
    void capabilitesCallback(ELocMsgID  msgId, const void* msgData);

    // internal session parameter
    static uint32_t         mClientIdGenerator;
    static mutex            mMutex;
    uint32_t                mClientId;
    uint32_t                mSessionId;
    bool                    mHalRegistered;
    char                    mSocketName[MAX_SOCKET_PATHNAME_LENGTH];

    // callbacks
    CapabilitiesCb          mCapabilitiesCb;
    ResponseCb              mResponseCb;
    LocationCb              mLocationCb;
    GnssReportCbs           mGnssReportCbs;

    LocationCallbacksMask   mCallbacksMask;
    LocationOptions         mLocationOptions;

    MsgTask*                mMsgTask;
    LocIpcSender*           mIpcSender;
};

} // namespace location_client

#endif /* LOCATIONCLIENTAPIIMPL_H */
