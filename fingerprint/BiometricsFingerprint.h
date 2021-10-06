/*
 * Copyright (C) 2017 The Android Open Source Project
 * Copyright (C) 2020-2021 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_BIOMETRICS_FINGERPRINT_V2_3_BIOMETRICSFINGERPRINT_H
#define ANDROID_HARDWARE_BIOMETRICS_FINGERPRINT_V2_3_BIOMETRICSFINGERPRINT_H

#define LOG_TAG "android.hardware.biometrics.fingerprint@2.3-service.xiaomi_sm6150"

#include "fingerprint.h"

#include <android/log.h>
#include <cutils/properties.h>
#include <hardware/hardware.h>
#include <hardware/hw_auth_token.h>
#include <hidl/HidlTransportSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <inttypes.h>
#include <log/log.h>
#include <unistd.h>

#include <android/hardware/biometrics/fingerprint/2.3/IBiometricsFingerprint.h>

#ifdef USES_FOD_EXTENSION
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <vendor/xiaomi/hardware/fingerprintextension/1.0/IXiaomiFingerprint.h>
#include <vendor/xiaomi/hardware/touchfeature/1.0/ITouchFeature.h>
#include <thread>
#endif

namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {
namespace V2_3 {
namespace implementation {

using ::android::sp;
using ::android::status_t;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallback;
using ::android::hardware::biometrics::fingerprint::V2_3::IBiometricsFingerprint;

#ifdef USES_FOD_EXTENSION
using ::vendor::xiaomi::hardware::fingerprintextension::V1_0::IXiaomiFingerprint;
using ::vendor::xiaomi::hardware::touchfeature::V1_0::ITouchFeature;
#endif

using RequestStatus = android::hardware::biometrics::fingerprint::V2_1::RequestStatus;
using FingerprintError = android::hardware::biometrics::fingerprint::V2_1::FingerprintError;
using FingerprintAcquiredInfo =
        android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo;

#ifdef USES_FOD_EXTENSION
struct BiometricsFingerprint : public IBiometricsFingerprint, public IXiaomiFingerprint {
#else
struct BiometricsFingerprint : public IBiometricsFingerprint {
  public:
#endif
    BiometricsFingerprint();
    ~BiometricsFingerprint();

    status_t registerAsSystemService();

    // Method to wrap legacy HAL with BiometricsFingerprint class
    static IBiometricsFingerprint* getInstance();

    // Methods from ::android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint
    // follow.
    Return<uint64_t> setNotify(
            const sp<IBiometricsFingerprintClientCallback>& clientCallback) override;
    Return<uint64_t> preEnroll() override;
    Return<RequestStatus> enroll(const hidl_array<uint8_t, 69>& hat, uint32_t gid,
                                 uint32_t timeoutSec) override;
    Return<RequestStatus> postEnroll() override;
    Return<uint64_t> getAuthenticatorId() override;
    Return<RequestStatus> cancel() override;
    Return<RequestStatus> enumerate() override;
    Return<RequestStatus> remove(uint32_t gid, uint32_t fid) override;
    Return<RequestStatus> setActiveGroup(uint32_t gid, const hidl_string& storePath) override;
    Return<RequestStatus> authenticate(uint64_t operationId, uint32_t gid) override;

    // Methods from ::android::hardware::biometrics::fingerprint::V2_3::IBiometricsFingerprint
    // follow.
    Return<bool> isUdfps(uint32_t sensorId) override;
    Return<void> onFingerDown(uint32_t x, uint32_t y, float minor, float major) override;
    Return<void> onFingerUp() override;

#ifdef USES_FOD_EXTENSION
    Return<int32_t> extCmd(int32_t cmd, int32_t param) override;
#endif

    static fingerprint_device_t* openHal();
    static void notify(
            const fingerprint_msg_t* msg); /* Static callback for legacy HAL implementation */
    static Return<RequestStatus> ErrorFilter(int32_t error);
    static FingerprintError VendorErrorFilter(int32_t error, int32_t* vendorCode);
    static FingerprintAcquiredInfo VendorAcquiredFilter(int32_t error, int32_t* vendorCode);
    static BiometricsFingerprint* sInstance;

    std::mutex mClientCallbackMutex;
    sp<IBiometricsFingerprintClientCallback> mClientCallback;
    fingerprint_device_t* mDevice;
#ifdef USES_FOD_EXTENSION
    int mFod = 1;
    sp<ITouchFeature> TouchFeatureService;
#else
    int mFod = 0;
#endif
};

}  // namespace implementation
}  // namespace V2_3
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_BIOMETRICS_FINGERPRINT_V2_3_BIOMETRICSFINGERPRINT_H
