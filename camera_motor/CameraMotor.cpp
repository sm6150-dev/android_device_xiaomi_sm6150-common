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

#define LOG_TAG "CameraMotorService"

#include "CameraMotor.h"
#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include <misc/drv8846.h>
#include <thread>
#include <string>

#define CAMERA_ID_FRONT "1"
#define MOTOR_DEV_PATH "/dev/drv8846_dev"

#include <fstream>

#define BLUE_LED        "/sys/class/leds/blue/"
#define GREEN_LED       "/sys/class/leds/green/"
#define BRIGHTNESS      "brightness"

#define MAX_LED_BRIGHTNESS    255
#define MIN_LED_BRIGHTNESS     0

namespace vendor {
namespace lineage {
namespace camera {
namespace motor {
namespace V1_0 {
namespace implementation {

/*
 * Write value to path and close file.
 */
static void set(std::string path, std::string value) {
    std::ofstream file(path);
        
    if (!file.is_open()) {
        ALOGW("failed to write %s to %s", value.c_str(), path.c_str());
        return;
    }

    file << value;
}

static void set(std::string path, int value) {
    set(path, std::to_string(value));
}

CameraMotor::CameraMotor() {
    motor_fd_ = android::base::unique_fd(open(MOTOR_DEV_PATH, O_RDWR));
}

Return<void> CameraMotor::onConnect(const hidl_string& cameraId) {
    uint8_t state = STILL;

    ioctl(motor_fd_.get(), MOTOR_IOC_GET_STATE, &state);

    LOG(INFO) << "Camera state: " + std::to_string(state);

    if (state != STILL) {
        uint8_t time_ms = 0;
        ioctl(motor_fd_.get(), MOTOR_IOC_GET_REMAIN_TIME, &time_ms);
        
        LOG(INFO) << "Remaining time: " + std::to_string(time_ms);

        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }

    if (cameraId == CAMERA_ID_FRONT && !(is_up)) {
        LOG(INFO) << "Camera is uprising.";
        uint8_t arg = UP;
        ioctl(motor_fd_.get(), MOTOR_IOC_SET_AUTORUN, &arg);
        set(BLUE_LED BRIGHTNESS, MAX_LED_BRIGHTNESS);
        set(GREEN_LED BRIGHTNESS, MAX_LED_BRIGHTNESS);
        is_up = 1;
    }

    return Void();
}

Return<void> CameraMotor::onDisconnect(const hidl_string& cameraId) {
    uint8_t state = STILL;

    ioctl(motor_fd_.get(), MOTOR_IOC_GET_STATE, &state);

    LOG(INFO) << "Camera state: " + std::to_string(state);

    if (state != STILL) {
        uint8_t time_ms = 0;
        ioctl(motor_fd_.get(), MOTOR_IOC_GET_REMAIN_TIME, &time_ms);

        LOG(INFO) << "Remaining time: " + std::to_string(time_ms);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
    }

    if (cameraId == CAMERA_ID_FRONT && is_up) {
        LOG(INFO) << "Camera is descending";
        uint8_t arg = DOWN;
        ioctl(motor_fd_.get(), MOTOR_IOC_SET_AUTORUN, &arg);
        set(BLUE_LED BRIGHTNESS, MIN_LED_BRIGHTNESS);
        set(GREEN_LED BRIGHTNESS, MIN_LED_BRIGHTNESS);
        is_up = 0;
    }

    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace motor
}  // namespace camera
}  // namespace lineage
}  // namespace vendor
