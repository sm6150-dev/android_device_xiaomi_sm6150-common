
#pragma once

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <vendor/lineage/touch/1.0/ITouchFeature.h>

namespace vendor::lineage::touch::implementation {

using ::android::sp;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct TouchFeature : public V1_0::ITouchFeature {
    // Methods from ::vendor::lineage::touch::V1_0::ITouchFeature follow.
    Return<void> setFeature(::vendor::lineage::touch::V1_0::TouchFeature feature,
                            bool activate) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
};
}  // namespace vendor::lineage::touch::implementation
