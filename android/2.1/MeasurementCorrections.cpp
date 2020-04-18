/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

#define LOG_TAG "LocSvc_MeasurementCorrectionsInterface"

#include <log_util.h>
#include "MeasurementCorrections.h"

namespace android {
namespace hardware {
namespace gnss {
namespace measurement_corrections {
namespace V1_1 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
using ::android::hardware::gnss::measurement_corrections::V1_0::IMeasurementCorrectionsCallback;
using ::android::hardware::gnss::measurement_corrections::V1_0::IMeasurementCorrections;

static MeasurementCorrections* spMeasurementCorrections = nullptr;

MeasurementCorrections::MeasurementCorrections(Gnss* gnss) : mGnss(gnss) {
    spMeasurementCorrections = this;
}

MeasurementCorrections::~MeasurementCorrections() {
    spMeasurementCorrections = nullptr;
}

Return<bool> MeasurementCorrections::setCorrections(
        const MeasurementCorrectionsV1_0& corrections) {
    return true;
}

Return<bool> MeasurementCorrections::setCorrections_1_1(
        const MeasurementCorrectionsV1_1& corrections) {
    return true;
}

Return<bool> MeasurementCorrections::setCallback(
        const sp<V1_0::IMeasurementCorrectionsCallback>& callback) {

    if (nullptr == mGnss || nullptr == mGnss->getGnssInterface()) {
        LOC_LOGe("Null GNSS interface");
        return false;
    }
    mMeasurementCorrectionsCbIface = callback;

    return true;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace measurement_corrections
}  // namespace gnss
}  // namespace hardware
}  // namespace android
