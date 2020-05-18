/*
 * Copyright (C) 2017 The Android Open Source Project
 * Copyright (C) 2020 The LineageOS Project
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

#define LOG_TAG "android.hardware.biometrics.fingerprint@2.1-service.xiaomi_sm6150"

#include <android/hardware/biometrics/fingerprint/2.1/IBiometricsFingerprint.h>
#include <android/hardware/biometrics/fingerprint/2.1/types.h>
#include <android/log.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>

#include "BiometricsFingerprint.h"

using android::sp;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::biometrics::fingerprint::V2_1::implementation::BiometricsFingerprint;

using android::status_t;

status_t BiometricsFingerprint::registerAsSystemService() {
    status_t ret = 0;

    ret = IBiometricsFingerprint::registerAsService();
    if (ret != 0) {
        ALOGE("Failed to register IBiometricsFingerprint (%d)", ret);
        goto fail;
    } else {
        ALOGI("Successfully registered IBiometricsFingerprint");
    }

    ret = IXiaomiFingerprint::registerAsService();
    if (ret != 0) {
        ALOGE("Failed to register IXiaomiFingerprint (%d)", ret);
        goto fail;
    } else {
        ALOGI("Successfully registered IXiaomiFingerprint");
    }

fail:
    return ret;
}

int main() {
    sp<BiometricsFingerprint> bio = nullptr;
    bio = new BiometricsFingerprint();

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    if (bio != nullptr) {
        if (::android::OK != bio->registerAsSystemService()) {
            return 1;
        }
    } else {
        ALOGE("Can't create instance of BiometricsFingerprint, nullptr");
    }

    joinRpcThreadpool();

    return 0;  // should never get here
}
