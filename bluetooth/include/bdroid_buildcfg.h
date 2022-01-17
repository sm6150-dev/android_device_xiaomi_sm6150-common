/*
 *
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution, Apache license notifications and license are retained
 *  for attribution purposes only.
 *
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2018-2020 The LineageOS Project
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

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H


#include <cutils/properties.h>
#include <string.h>

#include "osi/include/osi.h"

#pragma push_macro("PROPERTY_VALUE_MAX")

typedef struct {
    const char *product_device;
    const char *product_region;
    const char *product_model;
} device_t;

static const device_t devices[] = {
    {"davinci", "CN", "Xiaomi Redmi K20"},
    {"davinci", "GLOBAL", "Xiaomi Mi 9T"},
    {"davinciin", "INDIA", "Xiaomi Redmi K20"},
    {"phoenix", "ALL", "Redmi K30"},
    {"phoenixin", "INDIA", "POCO X2"},
    {"violet", "ALL", "Xiaomi Redmi Note 7 Pro"},
};

static inline const char *BtmGetDefaultName()
{
    char product_device[PROPERTY_VALUE_MAX];
    char product_region[PROPERTY_VALUE_MAX];
    property_get("ro.product.device", product_device, "");
    property_get("ro.boot.hwc", product_region, "");

    for (unsigned int i = 0; i < ARRAY_SIZE(devices); i++) {
        device_t device = devices[i];

        if (strcmp(device.product_device, product_device) == 0 &&
               (strcmp(device.product_region, product_region) == 0 ||
               strcmp(device.product_region, "ALL") == 0)) {
           return device.product_model;
       }
    }

    // Fallback to ro.product.model
    return "";
}

#define BTM_DEF_LOCAL_NAME BtmGetDefaultName()
// Disables read remote device feature
#define MAX_ACL_CONNECTIONS   16
#define MAX_L2CAP_CHANNELS    32
#define BLE_VND_INCLUDED   TRUE
#define GATT_MAX_PHY_CHANNEL  10

#pragma pop_macro("PROPERTY_VALUE_MAX")

#endif
