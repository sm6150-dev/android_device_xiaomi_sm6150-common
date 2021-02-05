/*
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

#define LOG_TAG "vendor.lineage.touch1.0-service.xiaomi_sm6150"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "TouchFeature.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using vendor::lineage::touch::V1_0::ITouchFeature;
using vendor::lineage::touch::implementation::TouchFeature;

using android::OK;
using android::status_t;

int main() {
    android::sp<ITouchFeature> service = new TouchFeature();

    configureRpcThreadpool(1, true);

    status_t status = service->registerAsService();
    if (status != OK) {
        return 1;
    }

    joinRpcThreadpool();

    return 1;
}
