# Activity Manager/Trim
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.fw.use_trim_settings=true

# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    af.fast_track_multiplier=1 \
    audio.deep_buffer.media=true \
    audio.offload.buffer.size.kb=32 \
    audio.offload.gapless.enabled=true \
    audio.offload.min.duration.secs=30 \
    audio.offload.video=true \
    av.offload.enable=true \
    vendor.audio.dolby.ds2.enabled=false \
    vendor.audio.dolby.ds2.hardbypass=false \
    vendor.audio.flac.sw.decoder.24bit=true \
    vendor.audio.noisy.broadcast.delay=600 \
    vendor.audio.offload.buffer.size.kb=32 \
    vendor.audio.offload.gapless.enabled=true \
    vendor.audio.offload.multiaac.enable=true \
    vendor.audio.offload.passthrough=false \
    vendor.audio.offload.pstimeout.secs=3 \
    vendor.audio.offload.track.enable=false \
    vendor.audio.parser.ip.buffer.size=262144 \
    vendor.audio.safx.pbe.enabled=true \
    vendor.audio.tunnel.encode=false \
    vendor.audio.use.sw.alac.decoder=true \
    vendor.audio.use.sw.ape.decoder=true \
    vendor.audio_hal.in_period_size=144 \
    vendor.audio_hal.period_multiplier=3 \
    vendor.audio_hal.period_size=192 \
    persist.vendor.audio.ras.enabled=false \
    ro.qc.sdk.audio.ssr=false \
    ro.vendor.audio.sdk.ssr=false \
    ro.vendor.audio.sos=true \
    ro.vendor.audio.voice.volume.boost=manual \
    tunnel.audio.encode = true \
    qcom.hw.aac.encoder=true

# AudioFlinger client heap size
PRODUCT_PROPERTY_OVERRIDES += \
    ro.af.client_heap_size_kbyte=7168

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.bt.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac-aptxadaptive \
    persist.vendor.btstack.a2dp_offload_cap=sbc-aptx-aptxtws-aptxhd-aac-ldac \
    persist.vendor.btstack.enable.splita2dp=true \
    vendor.bluetooth.soc=cherokee \
    vendor.qcom.bluetooth.soc=cherokee \
    bt.max.hfpclient.connections=1

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    camera.aux.packagelist="org.codeaurora.snapcam,com.android.camera,com.android.lightboxtest" \
    camera.disable_zsl_mode=1 \
    vendor.camera.aux.packagelist="org.codeaurora.snapcam,com.android.camera"

# Charger
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.chg.max_volt_mv=9000

# CnE
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.cne.feature=1

# Crypto
 PRODUCT_PROPERTY_OVERRIDES += \
    ro.crypto.allow_encrypt_override=true \
    ro.crypto.volume.filenames_mode=aes-256-cts

# Dalvik VM
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapmaxfree=8m \
    dalvik.vm.heapminfree=512k \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heaptargetutilization=0.75 \
    ro.dalvik.vm.native.bridge=0

# Display
 PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.display.sensortype=2 \
    ro.vendor.display.svi=1 \
    vendor.display.comp_mask=0 \
    vendor.display.dataspace_saturation_matrix=1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0 \
    vendor.display.disable_decimation=1 \
    vendor.display.disable_excl_rect=0 \
    vendor.display.disable_excl_rect_partial_fb=1 \
    vendor.display.disable_hw_recovery=1 \
    vendor.display.disable_inline_rotator=1 \
    vendor.display.disable_scaler=0 \
    vendor.display.enable_default_color_mode=0 \
    vendor.display.enable_null_display=0 \
    vendor.display.enable_optimize_refresh=1 \
    debug.mdpcomp.logs=0 \
    persist.sys.sf.color_saturation=1.0 \
    ro.colorpick_adjust=true

# DPM
PRODUCT_PROPERTY_OVERRIDES += \
    persist.dpm.feature=1

# DRM
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

# Enable stm-events
PRODUCT_PROPERTY_OVERRIDES += \
	persist.debug.coresight.config=stm-events

# FM
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.hw.fm.init=0

# Factory Reset Protection
PRODUCT_PROPERTY_OVERRIDES += \
    ro.frp.pst=/dev/block/bootdevice/by-name/frp

# Fluence/Audio Proccessing
PRODUCT_PROPERTY_OVERRIDES += \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicerec=false \
    persist.vendor.audio.fluence.speaker=true \
    persist.vendor.audio.fluence.voicecall=true \
    persist.vendor.audio.fluence.voicerec=false \
    ro.qc.sdk.audio.fluencetype=none \
    ro.vendor.audio.sdk.fluencetype=none

# GPS/GNSS
PRODUCT_PROPERTY_OVERRIDES += \
    sys.qca1530=detect \
    persist.backup.ntpServer=0.pool.ntp.org

# Graphics
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.gralloc.disable_ubwc=0 \
    ro.opengles.version=196610 \
    dev.pm.dyn_samplingrate=1 \
    persist.demo.hdmirotationlock=false \
    persist.sys.force_sw_gles=0

# IO CGroup
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.iocgrp.config=1

# Keystore
PRODUCT_PROPERTY_OVERRIDES += \
    ro.hardware.keystore_desede=true

# Media
PRODUCT_PROPERTY_OVERRIDES += \
    media.aac_51_output_enabled=true \
    media.stagefright.enable-aac=true \
    media.stagefright.enable-http=true \
    media.stagefright.enable-player=true \
    media.stagefright.enable-qcp=true \
    media.stagefright.enable-scan=true \
    mm.enable.smoothstreaming=true \
    mmp.enable.3g2=true \
    persist.mm.enable.prefetch=true \
    vendor.mm.enable.qcom_parser=63963135 \
    media.stagefright.enable-fma2dp=true

# Memory Optimisations
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.sys.fw.bservice_enable=true

# Misc
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.audio.game.effect=true \
    persist.sys.job_delay=true \
    persist.vendor.qg.log_level=1 \
    sys.vendor.shutdown.waittime=500 # ro.kernel.qemu.gles=0

# NetFlix
PRODUCT_PROPERTY_OVERRIDES += \
ro.netflix.bsp_rev=Q6150-17263-1

# Netmgr
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.data.mode=concurrent \
    ro.vendor.use_data_netmgrd=true

# Network Location Provider
PRODUCT_PROPERTY_OVERRIDES += \
    ro.location.osnlp.package=com.google.android.gms \
    ro.location.osnlp.region.package=""

# Perf Stack
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so \
    vendor.perf.dolphin.enable=false \
    vendor.perf.gestureflingboost.enable=true \
    vendor.perf.workloadclassifier.enable=true \
    persist.vendor.qti.games.gt.prof=1 \
    ro.vendor.qti.config.zram=true

# Qualcomm System Daemon
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.qcomsysd.enabled=1

# RMNet Data
PRODUCT_PROPERTY_OVERRIDES += \
    persist.data.df.dev_name=rmnet_usb0

# Radio/RIL
 PRODUCT_PROPERTY_OVERRIDES += \
    use.voice.path.for.pcm.voip=true \
    persist.radio.atfwd.start=false \
    persist.radio.multisim.config=dsds \
    persist.vendor.radio.apm_sim_not_pwdn=1 \
    persist.vendor.radio.custom_ecc=1 \
    persist.vendor.radio.data_con_rprt=1 \
    persist.vendor.radio.rat_on=combine \
    persist.vendor.radio.sib16_support=1 \
    ril.subscription.types=NV,RUIM \
    rild.libpath=/vendor/lib64/libril-qc-hal-qmi.so \
    ro.carrier=unknown \
    ro.com.android.dataroaming=true \
    ro.telephony.default_network=22,22 \
    telephony.lteOnCdmaDevice=1 \
    keyguard.no_require_sim=true \
    DEVICE_PROVISIONED=1

# SSR
PRODUCT_PROPERTY_OVERRIDES += \
persist.vendor.ssr.restart_level=ALL_ENABLE

# Simulate sdcard on /data/media
PRODUCT_PROPERTY_OVERRIDES += \
    persist.fuse_sdcard=true

# Time
PRODUCT_PROPERTY_OVERRIDES += \
	persist.timed.enable=true

# VoWIFI
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.data.iwlan.enable=true

# WLAN
PRODUCT_PROPERTY_OVERRIDES += \
    config.disable_rtt=true \
    ro.wlan.chip=39xx

# WiFi Display
PRODUCT_PROPERTY_OVERRIDES += \
    persist.debug.wfd.enable=1 \
    persist.hwc.enable_vds=1 \
    persist.sys.wfd.virtual=0
