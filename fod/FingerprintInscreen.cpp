/*
 * Copyright (C) 2019-2020 The LineageOS Project
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

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <cmath>

#define FINGERPRINT_ERROR_VENDOR 8

#define ON 1
#define OFF 0

#define COMMAND_NIT 10

#define Touch_Fod_Enable 10

#define FOD_SENSOR_X 445
#define FOD_SENSOR_Y 1931
#define FOD_SENSOR_SIZE 190

namespace {

#define PPCAT_NX(A, B) A/B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define STRINGIFY_INNER(x) #x
#define STRINGIFY(x) STRINGIFY_INNER(x)
#define DEV(x) PPCAT(/sys/devices/platform/soc, x)
#define DSP "soc:qcom,dsi-display";
#define DSI(x) STRINGIFY(PPCAT(DEV(DSP), x))

using ::android::base::ReadFileToString;
using ::android::base::WriteStringToFile;

// Write value to path and close file.
bool WriteToFile(const std::string& path, uint32_t content) {
    return WriteStringToFile(std::to_string(content), path);
}

void CalDimAlpha() {
    std::string buf;
    uint32_t bkl_;
    ReadFileToString(DSI(bkl), &buf);
    bkl_ = std::stoi(buf);
    int realBrightness =  bkl_ * 2047 / 255;
    double dim = (1.0 - pow(realBrightness/2047.0, 1/2.2));
    int alpha = (int)(255.0 * dim);
    LOG(INFO) << "Setting Xiaomi dim alpha to " << alpha << ", from " << bkl_;
    WriteToFile(DSI(dim_alpha), alpha);
}

}
namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {

FingerprintInscreen::FingerprintInscreen() {
    TouchFeatureService = ITouchFeature::getService();
    xiaomiFingerprintService = IXiaomiFingerprint::getService();
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_SENSOR_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_SENSOR_Y;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SENSOR_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    CalDimAlpha();
    WriteToFile(DSI(hbm), ON);
    TouchFeatureService->setTouchMode(Touch_Fod_Enable, ON);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, ON);
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    WriteToFile(DSI(hbm), OFF);
    TouchFeatureService->resetTouchMode(Touch_Fod_Enable);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, ON);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    LOG(ERROR) << "acquiredInfo: " << acquiredInfo << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode << "\n";
    return error == FINGERPRINT_ERROR_VENDOR && vendorCode == 6;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t /*brightness*/) {
    return 0;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>&) {
    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
