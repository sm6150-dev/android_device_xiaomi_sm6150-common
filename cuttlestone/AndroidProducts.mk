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

PRODUCT_MAKEFILES := \
    cuttlestone_x86_phone:$(LOCAL_DIR)/device_cuttlestone_x86_phone.mk \
    cuttlestone_x86_64_phone:$(LOCAL_DIR)/device_cuttlestone_x86_64_phone.mk \

COMMON_LUNCH_CHOICES := \
    cuttlestone_x86_phone-userdebug \
    cuttlestone_x86_64_phone-userdebug \
