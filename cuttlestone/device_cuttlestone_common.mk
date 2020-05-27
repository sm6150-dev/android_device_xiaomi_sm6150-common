#
# Copyright 2020 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Keystone branch hasn't updated package_whitelist.txt
SKIP_BOOT_JARS_CHECK := true

# tcmiface for tcm support
PRODUCT_PACKAGES += tcmiface
PRODUCT_BOOT_JARS += tcmiface

# IMS Extension module for Android Telephony
PRODUCT_PACKAGES += ims-ext-common
PRODUCT_PACKAGES += ims_ext_common.xml

# Android telephony extension
PRODUCT_PACKAGES += telephony-ext
PRODUCT_BOOT_JARS += telephony-ext

# Add Cuttlestone-specific resource overlays
DEVICE_PACKAGE_OVERLAYS += device/google/cuttlestone/overlay

# Add Cuttlefish platform tests
platform_tests += CuttlefishRilTests
platform_tests += CuttlefishWifiTests

# Add Widevine DRM
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true
PRODUCT_PACKAGES += \
    drmserver \
    libdrmdecrypt \
    libdrmframework \
    libdrmframework_jni \
    libfwdlockengine \
    liboemcrypto \
    libwvdrmengine

# Force virtual displays to use HWC (this is automatic in upstream cuttlefish)
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.display.vds_allow_hwc=1

# Dummy hostapd and hostapd_fst services
PRODUCT_PACKAGES += \
    cuttlestone_hostapd.rc

# The Bluetooth namespace needs to be selected in Keystone
PRODUCT_SOONG_NAMESPACES += packages/apps/Bluetooth

