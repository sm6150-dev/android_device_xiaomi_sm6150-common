/*
 * Copyright (C) 2019 The Android Open Source Project
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

#define LOG_TAG "android.hardware.lights-service_xiaomi.sm6150"

#include "Lights.h"
#include <android-base/file.h>
#include <android-base/logging.h>

namespace {

/* clang-format off */
#define PPCAT_NX(A, B) A/B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define STRINGIFY_INNER(x) #x
#define STRINGIFY(x) STRINGIFY_INNER(x)

#define LEDS(x) PPCAT(/sys/class/leds, x)
#define BLUE_ATTR(x) STRINGIFY(PPCAT(LEDS(blue), x))
#define GREEN_ATTR(x) STRINGIFY(PPCAT(LEDS(green), x))
#define RED_ATTR(x) STRINGIFY(PPCAT(LEDS(red), x))
#define WHITE_ATTR(x) STRINGIFY(PPCAT(LEDS(white), x))
/* clang-format on */

using ::android::base::ReadFileToString;
using ::android::base::WriteStringToFile;

// Default max brightness
constexpr auto kDefaultMaxLedBrightness = 255;

// Each step will stay on for 70ms by default.
constexpr auto kRampStepDurationDefault = 70;

// Each step will stay on for 35ms for popup.
constexpr auto kRampStepDurationPopUp = kRampStepDurationDefault / 2;

// Each value represents a duty percent (0 - 100) for the led pwm.
constexpr std::array kBrightnessRamp = {0, 12, 25, 37, 50, 72, 85, 100};

// Write value to path and close file.
bool WriteToFile(const std::string& path, uint32_t content) {
    return WriteStringToFile(std::to_string(content), path);
}

bool WriteToFile(const std::string& path, const std::string& content) {
    return WriteStringToFile(content, path);
}

uint32_t RgbaToBrightness(uint32_t color) {
    // Extract brightness from AARRGGBB.
    uint32_t alpha = (color >> 24) & 0xFF;

    // Retrieve each of the RGB colors
    uint32_t red = (color >> 16) & 0xFF;
    uint32_t green = (color >> 8) & 0xFF;
    uint32_t blue = color & 0xFF;

    // Scale RGB colors if a brightness has been applied by the user
    if (alpha != 0xFF) {
        red = red * alpha / 0xFF;
        green = green * alpha / 0xFF;
        blue = blue * alpha / 0xFF;
    }

    return (77 * red + 150 * green + 29 * blue) >> 8;
}

inline uint32_t RgbaToBrightness(uint32_t color, uint32_t max_brightness) {
    return RgbaToBrightness(color) * max_brightness / 0xFF;
}

/*
 * Scale each value of the brightness ramp according to the
 * brightness of the color.
 */
std::string GetScaledDutyPcts(uint32_t brightness) {
    std::stringstream ramp;

    for (size_t i = 0; i < kBrightnessRamp.size(); i++) {
        if (i > 0) ramp << ",";
        ramp << kBrightnessRamp[i] * brightness / 0xFF;
    }

    return ramp.str();
}

inline bool IsLit(uint32_t color) {
    return color & 0x00ffffff;
}

}  // anonymous namespace

namespace aidl {
namespace android {
namespace hardware {
namespace light {

Lights::Lights() {
    std::string buf;

    std::lock_guard<std::mutex> lock(globalLock);

    for (auto&& lights : lights_) {
        LightType type = lights.first;
        HwLight light{};
        light.id = availableLights.size();
        light.type = type;
        availableLights.emplace_back(light);
    }

    if (ReadFileToString(BLUE_ATTR(max_brightness), &buf) ||
        ReadFileToString(RED_ATTR(max_brightness), &buf) ||
        ReadFileToString(WHITE_ATTR(max_brightness), &buf)) {
        max_led_brightness_ = std::stoi(buf);
    } else {
        max_led_brightness_ = kDefaultMaxLedBrightness;
        LOG(ERROR) << "Failed to read max LED brightness, fallback to " << kDefaultMaxLedBrightness;
    }
}

ndk::ScopedAStatus Lights::setLightState(int id, const HwLightState& state) {
    if (!(0 <= id && id < availableLights.size())) {
        LOG(ERROR) << "Light id " << id << " does not exist.";
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    HwLight const& light = availableLights[id];
    auto it = lights_.find(light.type);

    if (it == lights_.end()) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    it->second(light.type, state);

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* lights) {
    for (auto i = availableLights.begin(); i != availableLights.end(); i++) {
        lights->push_back(*i);
    }
    return ndk::ScopedAStatus::ok();
}

void Lights::setLightNotification(LightType type, const HwLightState& state) {
    bool found = false;
    for (auto&& [cur_type, cur_state] : notif_states_) {
        if (cur_type == type) {
            cur_state = state;
        }

        // Fallback to battery light
        if (!found && (cur_type == LightType::BATTERY || IsLit(cur_state.color))) {
            found = true;
            LOG(DEBUG) << __func__ << ": type=" << toString(cur_type);
            applyNotificationState(cur_state);
        }
    }
}

void Lights::applyNotificationState(const HwLightState& state) {
    std::map<std::string, int> colorValues;
    colorValues["red"] = colorValues["green"] = colorValues["blue"] = colorValues["white"] =
            RgbaToBrightness(state.color, max_led_brightness_);

    auto makeLedPath = [](const std::string& led, const std::string& op) -> std::string {
        return "/sys/class/leds/" + led + "/" + op;
    };
    int blink = state.flashOnMs != 0 && state.flashOffMs != 0;
    switch (state.flashMode) {
        case FlashMode::HARDWARE:
            for (const auto& entry : colorValues) {
                WriteToFile(makeLedPath(entry.first, "breath"), blink);
            }
            break;
        case FlashMode::TIMED:
            if (state.flashOnMs != 801 && state.flashOffMs != 801) {
                for (const auto& entry : colorValues) {
                    WriteToFile(makeLedPath(entry.first, "step_ms"), 35),
                            WriteToFile(makeLedPath(entry.first, "pause_lo_count"), 0),
                            WriteToFile(makeLedPath(entry.first, "io_idx"), 1);
                    WriteToFile(makeLedPath(entry.first, "lux_pattern"), 22);
                }
            } else if (state.flashOnMs != 802 && state.flashOffMs != 802) {
                for (const auto& entry : colorValues) {
                    WriteToFile(makeLedPath(entry.first, "step_ms"), 70),
                            WriteToFile(makeLedPath(entry.first, "pause_lo_count"), 30),
                            WriteToFile(makeLedPath(entry.first, "io_idx"), 0);
                    WriteToFile(makeLedPath(entry.first, "lux_pattern"), 0);
                }
            } else {
                for (const auto& entry : colorValues) {
                    WriteToFile(makeLedPath(entry.first, "step_ms"), 35),
                            WriteToFile(makeLedPath(entry.first, "pause_lo_count"), 5),
                            WriteToFile(makeLedPath(entry.first, "io_idx"), 0);
                    WriteToFile(makeLedPath(entry.first, "lux_pattern"), 1);
                }
            }
            break;
        case FlashMode::NONE:
        default:
            for (const auto& entry : colorValues) {
                WriteToFile(makeLedPath(entry.first, "brightness"), entry.second);
            }
    }
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
