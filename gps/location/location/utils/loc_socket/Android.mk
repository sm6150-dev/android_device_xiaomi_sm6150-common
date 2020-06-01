LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libloc_socket
LOCAL_SANITIZE += $(GNSS_SANITIZE)
# activate the following line for debug purposes only, comment out for production
#LOCAL_SANITIZE_DIAG += $(GNSS_SANITIZE_DIAG)
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    liblog \
    libgps.utils

LOCAL_SRC_FILES := \
    LocSocket.cpp

LOCAL_CFLAGS := \
    -fno-short-enums \
    -D_ANDROID_

## Includes
LOCAL_HEADER_LIBRARIES := \
    libloc_core_headers \
    libgps.utils_headers \
    libloc_pla_headers \
    liblocation_api_headers
LOCAL_CFLAGS += $(GNSS_CFLAGS)

ifeq ($(TARGET_KERNEL_VERSION),$(filter $(TARGET_KERNEL_VERSION),3.18 4.4 4.9))
LOCAL_CFLAGS += -DUSE_QSOCKET
# Need libqsocket.so proprietary library, dlopened by libloc_loader
LOCAL_SHARED_LIBRARIES += \
    libloc_loader
endif

include $(BUILD_SHARED_LIBRARY)
