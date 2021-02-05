/*
 * Copyright (C) 2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TouchFeature.h"
#include <linux/input.h>
#include <fcntl.h>

constexpr int kWakeupModeOff = 4;
constexpr int kWakeupModeOn = 5;

namespace vendor::lineage::touch::implementation {

int open_ts_input() {
    int fd = -1;
    DIR* dir = opendir("/dev/input");

    if (dir != NULL) {
        struct dirent* ent;

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_CHR) {
                char absolute_path[PATH_MAX] = {0};
                char name[80] = {0};

                strcpy(absolute_path, "/dev/input/");
                strcat(absolute_path, ent->d_name);

                fd = open(absolute_path, O_RDWR);
                if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) > 0) {
                    if (strcmp(name, "atmel_mxt_ts") == 0 || strcmp(name, "ft5435_ts") == 0 ||
                        strcmp(name, "ft5x46") == 0 || strcmp(name, "ft8006m_ts") == 0 ||
                        strcmp(name, "fts") == 0 || strcmp(name, "fts_ts") == 0 ||
                        strcmp(name, "goodix_ts") == 0 || strcmp(name, "synaptics_dsx") == 0 ||
                        strcmp(name, "synaptics_dsx_v21") == 0 ||
                        strcmp(name, "synaptics_force") == 0 ||
                        strcmp(name, "synaptics_tcm") == 0 ||
                        strcmp(name, "NVTCapacitiveTouchScreen") == 0)
                        break;
                }

                close(fd);
                fd = -1;
            }
        }

        closedir(dir);
    }

    return fd;
}
// Methods from ::vendor::lineage::touch::V1_0::ITouchFeature follow.
Return<void> TouchFeature::setFeature(::vendor::lineage::touch::V1_0::TouchFeature feature,
                                      bool activate) {
    switch (feature) {
        case ::vendor::lineage::touch::V1_0::TouchFeature::POWER_FEATURE_DOUBLE_TAP_TO_WAKE: {
            int fd = open_ts_input();
            if (fd == -1) {
                return Void();
            }
            struct input_event ev;
            ev.type = EV_SYN;
            ev.code = SYN_CONFIG;
            ev.value = activate ? kWakeupModeOn : kWakeupModeOff;
            write(fd, &ev, sizeof(ev));
            close(fd);
            break;
        }
        default:
            break;
    }
    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.
}  // namespace vendor::lineage::touch::implementation

