LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libloc_loader
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := \
    liblog
LOCAL_SRC_FILES += \
    libloc_loader.c
LOCAL_HEADER_LIBRARIES := \
    libcutils_headers \
    libutils_headers

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include
# Make headers under include/ available to all modules
# that link to libloc_loader:
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

# 4.9 targets require libqsocket.so and ipc headers
ifeq ($(TARGET_KERNEL_VERSION),$(filter $(TARGET_KERNEL_VERSION),3.18 4.4 4.9))
LOCAL_CFLAGS += -DUSE_QSOCKET
# For UAPI <linux/msm_ipc.h>
LOCAL_HEADER_LIBRARIES += \
    device_kernel_headers
endif # TARGET_KERNEL_VERSION <= 4.9

include $(BUILD_SHARED_LIBRARY)
