LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := meta_image
LOCAL_CFLAGS := -Wall
LOCAL_SRC_FILES := meta_image.c
include $(BUILD_HOST_EXECUTABLE)
