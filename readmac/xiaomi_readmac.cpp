/*
 * Copyright (C) 2016 The CyanogenMod Project
 *               2017-2019 The LineageOS Project
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

// #define LOG_NDEBUG 0
#define LOG_TAG "xiaomi_readmac"

#include <dlfcn.h>

#include <array>
#include <cstring>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>
#include <cutils/android_filesystem_config.h> /* for AID_SYSTEM */

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

#define LOAD_NV_FUNCTION(name) fn_##name = LoadFunction(handle.get(), "qmi_nv_" #name);
#define CLOSE_NV_FUNCTION(name) fn_##name = nullptr;
#define FOR_EACH_FUNCTION(MACRO) \
    MACRO(read_bd_addr)          \
    MACRO(read_wlan_mac)

using android::base::ReadFileToString;
using android::base::StringPrintf;
using android::base::WriteStringToFile;

namespace {

constexpr size_t kMacSize = 6;
typedef std::array<uint8_t, kMacSize> mac_t;

constexpr char kFilename[] = "libqminvapi.so";
constexpr char kBtMacFile[] = "/data/vendor/mac_addr/bt.mac";
constexpr char kWlanMacFile[] = "/mnt/vendor/persist/wlan_mac.bin";
constexpr uint8_t kXiaomiMacPrefix[] = {0xf4, 0xf5, 0xdb};

// NV apis from libqminvapi.so
typedef int (*qmi_nv_read_mac_t)(uint8_t** mac);
qmi_nv_read_mac_t fn_read_bd_addr = nullptr;
qmi_nv_read_mac_t fn_read_wlan_mac = nullptr;

qmi_nv_read_mac_t LoadFunction(void* handle, const char* name) {
    void* fn = dlsym(handle, name);
    if (fn == nullptr) {
        LOG(ERROR) << "LoadFunction -- failed to load function " << name;
    }
    return reinterpret_cast<qmi_nv_read_mac_t>(fn);
}

mac_t GetRandomMac() {
    mac_t mac;
    static bool seeded = false;

    std::copy_n(kXiaomiMacPrefix, ARRAY_SIZE(kXiaomiMacPrefix), mac.begin());

    /*
     * We don't need strong randomness, and if the NV is corrupted
     * any hardware values are suspect, so just seed it with the
     * current time.
     */
    if (!seeded) {
        seeded = true;
        srand(time(0));
    }

    for (size_t i = ARRAY_SIZE(kXiaomiMacPrefix); i < kMacSize; i++) {
        mac[i] = rand() % 256;
    }

    return mac;
}

mac_t ReadMac(qmi_nv_read_mac_t fn_read_mac) {
    uint8_t* nv_mac = nullptr;

    if (fn_read_mac) {
        fn_read_mac(&nv_mac);
        if (nv_mac == nullptr) {
            LOG(ERROR) << "Failed to read mac from nv";
        } else {
            mac_t mac;
            static_assert(
                std::is_same<std::remove_reference_t<decltype(*nv_mac)>, mac_t::value_type>::value,
                "Type mismatch!");
            std::copy_n(nv_mac, kMacSize, mac.begin());
            return mac;
        }
    }

    return GetRandomMac();
}

inline bool WriteMacToFile(const std::string& content, const std::string& path) {
    return WriteStringToFile(content, path, 0644, AID_SYSTEM, AID_SYSTEM);
}

bool ValidateBtMacFile() {
    struct stat st;
    stat(kBtMacFile, &st);
    return st.st_size == kMacSize;
}

void SetBtMac() {
    if (ValidateBtMacFile()) {
        LOG(VERBOSE) << kBtMacFile << " is valid";
        return;
    }

    auto bt_mac = ReadMac(fn_read_bd_addr);

    /*
     * Convert to std::string to make use of the helper functions provided by libbase.
     * The retrieved BT MAC is in reversed order, so reverse copy it.
     */
    std::string content(bt_mac.rbegin(), bt_mac.rend());

    if (!WriteMacToFile(content, kBtMacFile)) {
        LOG(ERROR) << "Failed to write " << kWlanMacFile;
    }
}

bool ValidateWlanMacFile() {
    std::string content;

    if (!ReadFileToString(kWlanMacFile, &content)) {
        return false;
    }

    if (content.find("Intf0MacAddress") == std::string::npos) {
        LOG(VERBOSE) << kWlanMacFile << " is missing Intf0MacAddress entry value";
        return false;
    }

    if (content.find("Intf1MacAddress") == std::string::npos) {
        LOG(VERBOSE) << kWlanMacFile << " is missing Intf1MacAddress entry value";
        return false;
    }

    return true;
}

void SetWlanMac() {
    if (ValidateWlanMacFile()) {
        LOG(VERBOSE) << kWlanMacFile << " is valid";
        return;
    }

    auto wlan_mac = ReadMac(fn_read_wlan_mac);
    std::string content = StringPrintf(
        "Intf0MacAddress=%02X%02X%02X%02X%02X%02X\n"
        "Intf1MacAddress=%02X%02X%02X%02X%02X%02X\n"
        "END\n",
        wlan_mac[0], wlan_mac[1], wlan_mac[2], wlan_mac[3], wlan_mac[4], wlan_mac[5], wlan_mac[0],
        wlan_mac[1], wlan_mac[2], wlan_mac[3], wlan_mac[4], wlan_mac[5] + 1);

    if (!WriteMacToFile(content, kWlanMacFile)) {
        LOG(ERROR) << "Failed to write " << kWlanMacFile;
    }
}

}  // anonymous namespace

int main() {
    std::unique_ptr<void, std::function<void(void*)>> handle(
        dlopen(kFilename, RTLD_NOW), [&](void* p) {
            FOR_EACH_FUNCTION(CLOSE_NV_FUNCTION)
            if (p != nullptr) {
                int err = dlclose(p);
                p = nullptr;
                if (err != 0) {
                    LOG(ERROR) << "DLCLOSE failed for " << kFilename;
                }
            }
        });

    if (handle == nullptr) {
        LOG(ERROR) << "DLOPEN failed for " << kFilename << " (" << dlerror() << ")";
    } else {
        FOR_EACH_FUNCTION(LOAD_NV_FUNCTION)
    }

    SetBtMac();
    SetWlanMac();

    return 0;
}
