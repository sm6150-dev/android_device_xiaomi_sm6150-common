ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libloc_loader

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    liblog

LOCAL_SRC_FILES += \
    libloc_loader.c

LOCAL_HEADER_LIBRARIES := libcutils_headers libutils_headers

LOCAL_COPY_HEADERS_TO:= libloc_loader/

LOCAL_COPY_HEADERS:= \
    libloc_loader.h

LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif # not BUILD_TINY_ANDROID
