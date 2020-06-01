LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libsynergy_loc_api
LOCAL_SANITIZE += $(GNSS_SANITIZE)
# activate the following line for debug purposes only, comment out for production
#LOCAL_SANITIZE_DIAG += $(GNSS_SANITIZE_DIAG)
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

ifeq ($(TARGET_DEVICE),apq8026_lw)
LOCAL_CFLAGS += -DPDK_FEATURE_SET
else ifeq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
LOCAL_CFLAGS += -DPDK_FEATURE_SET
endif

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libloc_core \
    libgps.utils \
    libdl \
    liblog \
    libloc_loader

LOCAL_SRC_FILES = \
    SynergyLocApi.cpp

LOCAL_CFLAGS += \
    -fno-short-enums \
    -D_ANDROID_

## Includes
LOCAL_HEADER_LIBRARIES := \
    libloc_core_headers \
    libgps.utils_headers \
    libloc_pla_headers \
    loc_sll_if_headers \
    liblocation_api_headers

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libsynergyloc_api_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_HEADER_LIBRARY)
