/*
 * Copyright (C) 2019 The LineageOS Project
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

#define LOG_TAG "SunlightEnhancementService"

#include <fstream>

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>
#include <utils/Errors.h>

#include "SunlightEnhancement.h"

namespace {

#define PPCAT_NX(A, B) A/B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define STRINGIFY_INNER(x) #x
#define STRINGIFY(x) STRINGIFY_INNER(x)

#define DEV(x) PPCAT(/sys/devices/platform/soc, x)
#define DSI(x) STRINGIFY(PPCAT(DEV(soc:qcom,dsi-display), x))

using ::android::base::ReadFileToString;
using ::android::base::WriteStringToFile;
using ::android::base:GetProperty;

// Write value to path and close file.
bool WriteToFile(const std::string& path, uint32_t content) {
    return WriteStringToFile(std::to_string(content), path);
}

}
namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace implementation {

bool SunlightEnhancement::isSupported() {
    return true;
}

Return<bool> SunlightEnhancement::isEnabled() {
    std::string buf;
    int result;
    ReadFileToString(DSI(hbm), &buf);
    result = std::stoi(buf);
    return result > 0;
}

Return<bool> SunlightEnhancement::setEnabled(bool enabled) {
    WriteToFile(DSI(hbm), (enabled ? "1" : "0"))
    return false;
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
