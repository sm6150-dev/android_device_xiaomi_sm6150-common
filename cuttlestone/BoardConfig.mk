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

# Inherit from cuttlefish
include device/google/cuttlefish/vsoc_x86/BoardConfig.mk
# TODO(b/147756744) Re-evaluate sdclang environment variable usage
BUILD_BROKEN_NINJA_USES_ENV_VARS := SDCLANG_AE_CONFIG SDCLANG_CONFIG SDCLANG_SA_ENABLED
# TODO(b/150232543) Work with QTI to use header libraries.
BUILD_BROKEN_USES_BUILD_COPY_HEADERS := true

BUILD_BROKEN_USES_BUILD_HOST_SHARED_LIBRARY := true
BUILD_BROKEN_USES_BUILD_HOST_STATIC_LIBRARY := true
BUILD_BROKEN_USES_BUILD_HOST_EXECUTABLE := true
BUILD_BROKEN_USES_BUILD_COPY_HEADERS := true

BUILD_BROKEN_DUP_RULES := true
BUILD_BROKEN_NINJA_USES_ENV_VARS += TEMPORARY_DISABLE_PATH_RESTRICTIONS
BUILD_BROKEN_NINJA_USES_ENV_VARS += RTIC_MPGEN
BUILD_BROKEN_PREBUILT_ELF_FILES := true
BUILD_BROKEN_OUTSIDE_INCLUDE_DIRS := true
PRODUCT_FULL_TREBLE_OVERRIDE := true
PRODUCT_VENDOR_MOVE_ENABLED := true
BOARD_VNDK_VERSION := current
TARGET_MOUNT_POINTS_SYMLINKS := fals
#Set WITH_DEXPREOPT_DEBUG_INFO := false to resolve below error
#Cannot override readonly variable PRODUCT_DEX_PREOPT_BOOT_FLAGS
WITH_DEXPREOPT_DEBUG_INFO := false
ALLOW_MISSING_DEPENDENCIES := true

# Add cuttlestone-specific framework compatibility matrix
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE := device/qcom/common/cuttlestone/device_framework_matrix.xml
