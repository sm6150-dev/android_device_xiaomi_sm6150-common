LOCHIDL :=
ifneq ($(TARGET_HAS_LOW_RAM),true)
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
ifneq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)

# GPS-HIDL
LOC_HIDL_1_0_TARGET_LIST := msm8960
LOC_HIDL_1_0_TARGET_LIST += msm8974
LOC_HIDL_1_0_TARGET_LIST += msm8226
LOC_HIDL_1_0_TARGET_LIST += msm8610
LOC_HIDL_1_0_TARGET_LIST += apq8084
LOC_HIDL_1_0_TARGET_LIST += msm8916
LOC_HIDL_1_0_TARGET_LIST += msm8994
LOC_HIDL_1_0_TARGET_LIST += msm8909
LOC_HIDL_1_0_TARGET_LIST += msm8952
LOC_HIDL_1_0_TARGET_LIST += msm8992
LOC_HIDL_1_0_TARGET_LIST += msm8996
LOC_HIDL_1_0_TARGET_LIST += msm8937
LOC_HIDL_1_0_TARGET_LIST += msm8953
LOC_HIDL_1_0_TARGET_LIST += msm8998
LOC_HIDL_1_0_TARGET_LIST += apq8098_latv
LOC_HIDL_1_2_TARGET_LIST := sdm710
LOC_HIDL_1_2_TARGET_LIST += qcs605
LOC_HIDL_2_0_TARGET_LIST := sdm845
LOC_HIDL_2_0_TARGET_LIST += sdm660
LOC_HIDL_2_0_TARGET_LIST += msmnile
LOC_HIDL_2_0_TARGET_LIST += $(MSMSTEPPE)

ifneq (,$(filter $(LOC_HIDL_1_0_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
LOCHIDL += vendor.qti.gnss@1.0
LOCHIDL += vendor.qti.gnss@1.0_vendor
LOCHIDL += vendor.qti.gnss@1.0-impl
LOCHIDL += vendor.qti.gnss@1.0-service
LOC_HIDL_VERSION = 1.0
endif

ifneq (,$(filter $(LOC_HIDL_1_1_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
LOCHIDL += vendor.qti.gnss@1.0
LOCHIDL += vendor.qti.gnss@1.0_vendor
LOCHIDL += vendor.qti.gnss@1.1
LOCHIDL += vendor.qti.gnss@1.1_vendor
LOCHIDL += vendor.qti.gnss@1.1-impl
LOCHIDL += vendor.qti.gnss@1.1-service
LOC_HIDL_VERSION = 1.1
endif

ifneq (,$(filter $(LOC_HIDL_1_2_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
LOCHIDL += vendor.qti.gnss@1.0
LOCHIDL += vendor.qti.gnss@1.0_vendor
LOCHIDL += vendor.qti.gnss@1.1
LOCHIDL += vendor.qti.gnss@1.1_vendor
LOCHIDL += vendor.qti.gnss@1.2
LOCHIDL += vendor.qti.gnss@1.2_vendor
LOCHIDL += vendor.qti.gnss@1.2-impl
LOCHIDL += vendor.qti.gnss@1.2-service
LOC_HIDL_VERSION = 1.2
endif

ifneq (,$(filter $(LOC_HIDL_2_0_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
LOCHIDL += vendor.qti.gnss@1.0
LOCHIDL += vendor.qti.gnss@1.0_vendor
LOCHIDL += vendor.qti.gnss@1.1
LOCHIDL += vendor.qti.gnss@1.1_vendor
LOCHIDL += vendor.qti.gnss@1.2
LOCHIDL += vendor.qti.gnss@1.2_vendor
LOCHIDL += vendor.qti.gnss@2.0
LOCHIDL += vendor.qti.gnss@2.0_vendor
LOCHIDL += vendor.qti.gnss@2.0-impl
LOCHIDL += vendor.qti.gnss@2.0-service
LOC_HIDL_VERSION = 2.0
endif

endif # TARGET_SUPPORTS_ANDROID_WEAR
endif # BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET
endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
endif # TARGET_HAS_LOW_RAM
