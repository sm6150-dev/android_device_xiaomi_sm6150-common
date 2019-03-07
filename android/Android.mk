LOCAL_PATH := $(call my-dir)
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
include $(CLEAR_VARS)
DIR_LIST := $(LOCAL_PATH)
include $(LOCAL_PATH)/utils/Android.mk
ifeq ($(GNSS_HIDL_VERSION),1.1)
include $(DIR_LIST)/1.1/Android.mk
else
include $(DIR_LIST)/1.0/Android.mk
endif

endif #BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
