LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := healthd_board_msm.cpp
LOCAL_MODULE := libhealthd.msm
LOCAL_CFLAGS := -Werror
LOCAL_C_INCLUDES := \
    system/core/healthd/include/healthd/ \
    system/core/base/include \
    bootable/recovery
ifneq ($(TARGET_USES_AOSP),true)
LOCAL_SRC_FILES += healthd_msm_alarm.cpp
LOCAL_CFLAGS    += -DQTI_BSP=1
endif
include $(BUILD_STATIC_LIBRARY)
