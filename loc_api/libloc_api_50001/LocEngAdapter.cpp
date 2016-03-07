/* Copyright (c) 2011-2014, The Linux Foundation. All rights reserved.
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
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_EngAdapter"

#include <LocEngAdapter.h>
#include "loc_eng_msg.h"
#include "loc_log.h"

using namespace loc_core;

LocInternalAdapter::LocInternalAdapter(LocEngAdapter* adapter) :
    LocAdapterBase(adapter->getMsgTask()),
    mLocEngAdapter(adapter)
{
}
void LocInternalAdapter::setPositionModeInt(LocPosMode& posMode) {
    sendMsg(new LocEngPositionMode(mLocEngAdapter, posMode));
}
void LocInternalAdapter::startFixInt() {
    sendMsg(new LocEngStartFix(mLocEngAdapter));
}
void LocInternalAdapter::stopFixInt() {
    sendMsg(new LocEngStopFix(mLocEngAdapter));
}
void LocInternalAdapter::getZppInt() {
    sendMsg(new LocEngGetZpp(mLocEngAdapter));
}

LocEngAdapter::LocEngAdapter(LOC_API_ADAPTER_EVENT_MASK_T mask,
                             void* owner, ContextBase* context,
                             MsgTask::tCreate tCreator) :
    LocAdapterBase(mask,
                   //Get the AFW context if VzW context has not already been intialized in
                   //loc_ext
                   context == NULL?
                   LocDualContext::getLocFgContext(tCreator,
                                                   LocDualContext::mLocationHalName)
                   :context),
    mOwner(owner), mInternalAdapter(new LocInternalAdapter(this)),
    mUlp(new UlpProxyBase()), mNavigating(false),
    mAgpsEnabled(false), mCPIEnabled(false)
{
    memset(&mFixCriteria, 0, sizeof(mFixCriteria));
    mFixCriteria.mode = LOC_POSITION_MODE_INVALID;
    LOC_LOGD("LocEngAdapter created");
}

inline
LocEngAdapter::~LocEngAdapter()
{
    delete mInternalAdapter;
    LOC_LOGV("LocEngAdapter deleted");
}

void LocInternalAdapter::setUlpProxy(UlpProxyBase* ulp) {
    struct LocSetUlpProxy : public LocMsg {
        LocAdapterBase* mAdapter;
        UlpProxyBase* mUlp;
        inline LocSetUlpProxy(LocAdapterBase* adapter, UlpProxyBase* ulp) :
            LocMsg(), mAdapter(adapter), mUlp(ulp) {
        }
        virtual void proc() const {
            LOC_LOGV("%s] ulp %p adapter %p", __func__,
                     mUlp, mAdapter);
            mAdapter->setUlpProxy(mUlp);
        }
    };

    sendMsg(new LocSetUlpProxy(mLocEngAdapter, ulp));
}

void LocEngAdapter::setUlpProxy(UlpProxyBase* ulp)
{
    if (ulp == mUlp) {
        //This takes care of the case when double initalization happens
        //and we get the same object back for UlpProxyBase . Do nothing
        return;
    }

    LOC_LOGV("%s] %p", __func__, ulp);
    if (NULL == ulp) {
        LOC_LOGE("%s:%d]: ulp pointer is NULL", __func__, __LINE__);
        ulp = new UlpProxyBase();
    }

    if (LOC_POSITION_MODE_INVALID != mUlp->mPosMode.mode) {
        // need to send this mode and start msg to ULP
        ulp->sendFixMode(mUlp->mPosMode);
    }

    if(mUlp->mFixSet) {
        ulp->sendStartFix();
    }

    delete mUlp;
    mUlp = ulp;
}

void LocInternalAdapter::reportPosition(UlpLocation &location,
                                        GpsLocationExtended &locationExtended,
                                        void* locationExt,
                                        enum loc_sess_status status,
                                        LocPosTechMask loc_technology_mask)
{
    sendMsg(new LocEngReportPosition(mLocEngAdapter,
                                     location,
                                     locationExtended,
                                     locationExt,
                                     status,
                                     loc_technology_mask));
}


void LocEngAdapter::reportPosition(UlpLocation &location,
                                   GpsLocationExtended &locationExtended,
                                   void* locationExt,
                                   enum loc_sess_status status,
                                   LocPosTechMask loc_technology_mask)
{
    if (! mUlp->reportPosition(location,
                               locationExtended,
                               locationExt,
                               status,
                               loc_technology_mask )) {
        mInternalAdapter->reportPosition(location,
                                         locationExtended,
                                         locationExt,
                                         status,
                                         loc_technology_mask);
    }
}

void LocInternalAdapter::reportSv(GpsSvStatus &svStatus,
                                  GpsLocationExtended &locationExtended,
                                  void* svExt){
    sendMsg(new LocEngReportSv(mLocEngAdapter, svStatus,
                               locationExtended, svExt));
}

void LocEngAdapter::reportSv(GpsSvStatus &svStatus,
                             GpsLocationExtended &locationExtended,
                             void* svExt)
{

    // We want to send SV info to ULP to help it in determining GNSS
    // signal strength ULP will forward the SV reports to HAL without
    // any modifications
    if (! mUlp->reportSv(svStatus, locationExtended, svExt)) {
        mInternalAdapter->reportSv(svStatus, locationExtended, svExt);
    }
}

void LocEngAdapter::setInSession(bool inSession)
{
    mNavigating = inSession;
    mLocApi->setInSession(inSession);
    if (!mNavigating) {
        mFixCriteria.mode = LOC_POSITION_MODE_INVALID;
    }
}

void LocInternalAdapter::reportStatus(GpsStatusValue status)
{
    sendMsg(new LocEngReportStatus(mLocEngAdapter, status));
}

void LocEngAdapter::reportStatus(GpsStatusValue status)
{
    if (!mUlp->reportStatus(status)) {
        mInternalAdapter->reportStatus(status);
    }
}

inline
void LocEngAdapter::reportNmea(const char* nmea, int length)
{
    sendMsg(new LocEngReportNmea(mOwner, nmea, length));
}

inline
bool LocEngAdapter::reportXtraServer(const char* url1,
                                        const char* url2,
                                        const char* url3,
                                        const int maxlength)
{
    if (mAgpsEnabled) {
        sendMsg(new LocEngReportXtraServer(mOwner, url1,
                                           url2, url3, maxlength));
    }
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::requestATL(int connHandle, AGpsType agps_type)
{
    if (mAgpsEnabled) {
        sendMsg(new LocEngRequestATL(mOwner,
                                     connHandle, agps_type));
    }
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::releaseATL(int connHandle)
{
    if (mAgpsEnabled) {
        sendMsg(new LocEngReleaseATL(mOwner, connHandle));
    }
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::requestXtraData()
{
    if (mAgpsEnabled) {
        sendMsg(new LocEngRequestXtra(mOwner));
    }
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::requestTime()
{
    if (mAgpsEnabled) {
        sendMsg(new LocEngRequestTime(mOwner));
    }
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::requestNiNotify(GpsNiNotification &notif, const void* data)
{
    if (mAgpsEnabled) {
        notif.size = sizeof(notif);
        notif.timeout = LOC_NI_NO_RESPONSE_TIME;

        sendMsg(new LocEngRequestNi(mOwner, notif, data));
    }
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::requestSuplES(int connHandle)
{
    if (mAgpsEnabled)
        sendMsg(new LocEngRequestSuplEs(mOwner, connHandle));
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::reportDataCallOpened()
{
    if(mAgpsEnabled)
        sendMsg(new LocEngSuplEsOpened(mOwner));
    return mAgpsEnabled;
}

inline
bool LocEngAdapter::reportDataCallClosed()
{
    if(mAgpsEnabled)
        sendMsg(new LocEngSuplEsClosed(mOwner));
    return mAgpsEnabled;
}

inline
void LocEngAdapter::handleEngineDownEvent()
{
    sendMsg(new LocEngDown(mOwner));
}

inline
void LocEngAdapter::handleEngineUpEvent()
{
    sendMsg(new LocEngUp(mOwner));
}
