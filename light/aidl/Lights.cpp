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
#define DEBUG

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

// Each step will stay on for 50ms by default.
constexpr auto kRampStepDuration = 50;

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

    // Turn off the leds (initially)
    for (const auto& entry : colorValues) {
        WriteToFile(makeLedPath(entry.first, "breath"), 0);
    }

    if (state.flashMode == FlashMode::TIMED && state.flashOnMs > 0 && state.flashOffMs > 0) {
        /*
         * If the flashOnMs duration is not long enough to fit ramping up
         * and down at the default step duration, step duration is modified
         * to fit.
         */
        int32_t step_duration = kRampStepDuration;
        int32_t pause_hi = state.flashOnMs - (step_duration * kBrightnessRamp.size() * 2);
        if (pause_hi < 0) {
            step_duration = state.flashOnMs / (kBrightnessRamp.size() * 2);
            pause_hi = 0;
        }

        LOG(DEBUG) << __func__ << ": color=" << std::hex << state.color << std::dec
                   << " onMs=" << state.flashOnMs << " offMs=" << state.flashOffMs;
        for (const auto& entry : colorValues) {
            WriteToFile(makeLedPath(entry.first, "lo_idx"), 0);
            WriteToFile(makeLedPath(entry.first, "delay_off"), static_cast<uint32_t>(pause_hi));
            WriteToFile(makeLedPath(entry.first, "delay_on"),
                        static_cast<uint32_t>(state.flashOffMs));
            WriteToFile(makeLedPath(entry.first, "lut_pattern"), GetScaledDutyPcts(entry.second));
            WriteToFile(makeLedPath(entry.first, "step_ms"), static_cast<uint32_t>(step_duration));
            WriteToFile(makeLedPath(entry.first, "breath"), 1);
        }
    } else {
        for (const auto& entry : colorValues) {
            WriteToFile(makeLedPath(entry.first, "brightness"), entry.second);
        }
    }
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
