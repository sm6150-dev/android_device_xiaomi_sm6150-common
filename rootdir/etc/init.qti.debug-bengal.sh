#!/vendor/bin/sh
# Copyright (c) 2019, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

enable_bengal_tracing_events()
{
    # timer
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_expire_entry/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_expire_exit/enable
    #echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_cancel/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_expire_entry/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_expire_exit/enable
    #echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_init/enable
    #echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_start/enable
    #enble FTRACE for softirq events
    echo 1 > /sys/kernel/debug/tracing/events/irq/enable
    #enble FTRACE for Workqueue events
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/enable
    # schedular
    #echo 1 > /sys/kernel/debug/tracing/events/sched/sched_cpu_hotplug/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_migrate_task/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_pi_setprio/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_switch/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wakeup/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wakeup_new/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_isolate/enable
    # video
    echo 1 > /sys/kernel/debug/tracing/events/msm_vidc_events/enable
    # clock
    echo 1 > /sys/kernel/debug/tracing/events/power/clock_set_rate/enable
    echo 1 > /sys/kernel/debug/tracing/events/power/clock_enable/enable
    echo 1 > /sys/kernel/debug/tracing/events/power/clock_disable/enable
    echo 1 > /sys/kernel/debug/tracing/events/power/cpu_frequency/enable
    # regulator
    echo 1 > /sys/kernel/debug/tracing/events/regulator/enable

    # power
    #echo 1 > /sys/kernel/debug/tracing/events/msm_low_power/enable
    echo 1 > /sys/kernel/debug/tracing/events/msm_low_power/cpu_idle_enter/enable
    echo 1 > /sys/kernel/debug/tracing/events/msm_low_power/cpu_idle_exit/enable
    #thermal
    echo 1 > /sys/kernel/debug/tracing/events/thermal/enable
    #scm
    echo 1 > /sys/kernel/debug/tracing/events/scm/enable

    #enable aop with timestamps
    # echo 33 0x680000 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_msr
    # echo 48 0xC0 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_msr
    # echo 0x4 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/mcmb_lanes_select
    # echo 1 0 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_mode
    # echo 1 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_trig_ts
    # echo 1 >  /sys/bus/coresight/devices/coresight-tpdm-swao-0/enable_source
    # echo 4 2 > /sys/bus/coresight/devices/coresight-cti-swao_cti0/map_trigin
    # echo 4 2 > /sys/bus/coresight/devices/coresight-cti-swao_cti0/map_trigout

    #memory pressure events/oom
    echo 1 > /sys/kernel/debug/tracing/events/psi/psi_event/enable
    echo 1 > /sys/kernel/debug/tracing/events/psi/psi_window_vmstat/enable

    #iommu events
    echo 1 > /sys/kernel/debug/tracing/events/iommu/map/enable
    echo 1 > /sys/kernel/debug/tracing/events/iommu/map_sg/enable
    echo 1 > /sys/kernel/debug/tracing/events/iommu/unmap/enable

    echo 1 > /sys/kernel/debug/tracing/tracing_on
}

# function to enable ftrace events
enable_bengal_ftrace_event_tracing()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi

    # bail out if ftrace events aren't present
    if [ ! -d /sys/kernel/debug/tracing/events ]
    then
        return
    fi
    echo 0x4096 > /sys/kernel/debug/tracing/buffer_size_kb
    enable_bengal_tracing_events
}

# function to enable ftrace event transfer to CoreSight STM
enable_bengal_stm_events()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi
    # bail out if coresight isn't present
    if [ ! -d /sys/bus/coresight ]
    then
        return
    fi
    # bail out if ftrace events aren't present
    if [ ! -d /sys/kernel/debug/tracing/events ]
    then
        return
    fi

    echo $etr_size > /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size
    if uname -m | grep "armv7l"; then
        echo 0x1000000 > /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size
    fi
    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/$sinkenable
    echo 1 > /sys/bus/coresight/devices/coresight-stm/$srcenable
    echo 1 > /sys/kernel/debug/tracing/tracing_on
    echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
    enable_bengal_tracing_events
}

#BIMC Register
config_bengal_dcc_bimc()
{
    #BIMC_M_APP_MPORT
    echo 0x4488100 1 > $DCC_PATH/config
    echo 0x4488400 2 > $DCC_PATH/config
    echo 0x4488410 1 > $DCC_PATH/config
    echo 0x4488420 2 > $DCC_PATH/config
    echo 0x4488430 2 > $DCC_PATH/config

    #BIMC_M_GPU_MPORT
    echo 0x448c100 1 > $DCC_PATH/config
    echo 0x448c400 2 > $DCC_PATH/config
    echo 0x448c410 1 > $DCC_PATH/config
    echo 0x448c420 2 > $DCC_PATH/config
    echo 0x448c430 2 > $DCC_PATH/config

    #BIMC_M_MMSS_RT_MPORT
    echo 0x4490100 1 > $DCC_PATH/config
    echo 0x4490400 2 > $DCC_PATH/config
    echo 0x4490410 1 > $DCC_PATH/config
    echo 0x4490420 2 > $DCC_PATH/config
    echo 0x4490430 2 > $DCC_PATH/config

    #BIMC_M_MMSS_NRT_MPORT
    echo 0x4494100 1 > $DCC_PATH/config
    echo 0x4494400 2 > $DCC_PATH/config
    echo 0x4494410 1 > $DCC_PATH/config
    echo 0x4494420 2 > $DCC_PATH/config
    echo 0x4494430 2 > $DCC_PATH/config

    #BIMC_M_TCU_MPORT
    echo 0x449810c 1 > $DCC_PATH/config
    echo 0x4498400 2 > $DCC_PATH/config
    echo 0x4498410 1 > $DCC_PATH/config
    echo 0x4498420 2 > $DCC_PATH/config
    echo 0x4498430 2 > $DCC_PATH/config

    #BIMC_M_MDSP_MPORT
    #echo 0x449c100 1 > $DCC_PATH/config
    #echo 0x449c400 2 > $DCC_PATH/config
    #echo 0x449c410 1 > $DCC_PATH/config
    #echo 0x449c420 2 > $DCC_PATH/config
    #echo 0x449c430 2 > $DCC_PATH/config

    #BIMC_M_SYS_MPORT
    echo 0x44a0100 1 > $DCC_PATH/config
    echo 0x44a0400 2 > $DCC_PATH/config
    echo 0x44a0410 1 > $DCC_PATH/config
    echo 0x44a0420 2 > $DCC_PATH/config
    echo 0x44a0430 2 > $DCC_PATH/config

    #BIMC_S_DDR0
    echo 0x44b0560 1 > $DCC_PATH/config
    echo 0x44b05a0 1 > $DCC_PATH/config
    echo 0x44b1800 1 > $DCC_PATH/config
    echo 0x44b408c 1 > $DCC_PATH/config
    echo 0x44b409c 1 > $DCC_PATH/config
    echo 0x44b0520 1 > $DCC_PATH/config
    echo 0x44b5070 2 > $DCC_PATH/config

    # BIMC_S_SYS_SWAY
    echo 0x44bc220 1 > $DCC_PATH/config
    echo 0x44bc400 7 > $DCC_PATH/config
    echo 0x44bc420 9 > $DCC_PATH/config

    echo 0x44bd800 1 > $DCC_PATH/config
    echo 0x44c5800 1 > $DCC_PATH/config
    echo 0x4480040 2 > $DCC_PATH/config
    echo 0x4480810 2 > $DCC_PATH/config
    echo 0x44b0a40 1 > $DCC_PATH/config

    #CH0_DDRCC
    echo 0x4506044 1 > $DCC_PATH/config
    echo 0x45061dc 1 > $DCC_PATH/config
    echo 0x45061ec 1 > $DCC_PATH/config
    echo 0x4506028 2 > $DCC_PATH/config
    echo 0x4506094 1 > $DCC_PATH/config
    echo 0x4506608 1 > $DCC_PATH/config

    #CCC_MCCC_CH0
    echo 0x447d02c 4 > $DCC_PATH/config
    echo 0x447d040 1 > $DCC_PATH/config

    #CH0_CA0_DDRPHY
    echo 0x450002c 2 > $DCC_PATH/config
    echo 0x4500094 1 > $DCC_PATH/config
    echo 0x450009c 1 > $DCC_PATH/config
    echo 0x45000c4 2 > $DCC_PATH/config
    echo 0x45003dc 1 > $DCC_PATH/config
    echo 0x45005d8 1 > $DCC_PATH/config

    #CH0_CA1_DDRPHY
    echo 0x450102c 2 > $DCC_PATH/config
    echo 0x4501094 1 > $DCC_PATH/config
    echo 0x450109c 1 > $DCC_PATH/config
    echo 0x45010c4 2 > $DCC_PATH/config
    echo 0x45013dc 1 > $DCC_PATH/config
    echo 0x45015d8 1 > $DCC_PATH/config

    #CH0_DQ0_DDRPHY
    echo 0x450202c 2 > $DCC_PATH/config
    echo 0x4502094 1 > $DCC_PATH/config
    echo 0x450209c 1 > $DCC_PATH/config
    echo 0x45020c4 2 > $DCC_PATH/config
    echo 0x45023dc 1 > $DCC_PATH/config
    echo 0x45025d8 1 > $DCC_PATH/config

    #CH0_DQ1_DDRPHY
    echo 0x450302c 2 > $DCC_PATH/config
    echo 0x4503094 1 > $DCC_PATH/config
    echo 0x450309c 1 > $DCC_PATH/config
    echo 0x45030c4 2 > $DCC_PATH/config
    echo 0x45033dc 1 > $DCC_PATH/config
    echo 0x45035d8 1 > $DCC_PATH/config

    #CH0_DQ2_DDRPHY
    echo 0x450402c 2 > $DCC_PATH/config
    echo 0x4504094 1 > $DCC_PATH/config
    echo 0x450409c 1 > $DCC_PATH/config
    echo 0x45040c8 2 > $DCC_PATH/config
    echo 0x45043dc 1 > $DCC_PATH/config
    echo 0x45045d8 1 > $DCC_PATH/config

    #CH0_DQ3_DDRPHY
    echo 0x450502c 2 > $DCC_PATH/config
    echo 0x4505094 1 > $DCC_PATH/config
    echo 0x450509c 1 > $DCC_PATH/config
    echo 0x45050c4 2 > $DCC_PATH/config
    echo 0x45053dc 1 > $DCC_PATH/config
    echo 0x45055d8 1 > $DCC_PATH/config

}

config_bengal_dcc_osm()
{
    echo 0x0F522C14 > $DCC_PATH/config
    echo 0x0F522C1C > $DCC_PATH/config
    echo 0x0F522C10 > $DCC_PATH/config
    echo 0x0F524C10 > $DCC_PATH/config
    echo 0x0F524C14 > $DCC_PATH/config
    echo 0x0F524C18 > $DCC_PATH/config
    echo 0x0F524C1C > $DCC_PATH/config
    echo 0x0F521920 > $DCC_PATH/config
    echo 0x0F52102C > $DCC_PATH/config
    echo 0x0F521044 > $DCC_PATH/config
    echo 0x0F521710 > $DCC_PATH/config
    echo 0x0F52176C > $DCC_PATH/config
    echo 0x0F523920 > $DCC_PATH/config
    echo 0x0F52302C > $DCC_PATH/config
    echo 0x0F523044 > $DCC_PATH/config
    echo 0x0F523710 > $DCC_PATH/config
    echo 0x0F52376C > $DCC_PATH/config
    echo 0x0F116000 > $DCC_PATH/config
    echo 0x0F116004 > $DCC_PATH/config
    echo 0x0F11602C > $DCC_PATH/config
    echo 0x0F016000 > $DCC_PATH/config
    echo 0x0F016004 > $DCC_PATH/config
    echo 0x0F01602C > $DCC_PATH/config
    echo 0x0F111250 > $DCC_PATH/config
    echo 0x0F111254 > $DCC_PATH/config
    echo 0x0F111258 > $DCC_PATH/config
    echo 0x0F11125C > $DCC_PATH/config
    echo 0x0F111260 > $DCC_PATH/config
    echo 0x0F188078 > $DCC_PATH/config
    echo 0x0F188084 > $DCC_PATH/config
    echo 0x0F198078 > $DCC_PATH/config
    echo 0x0F198084 > $DCC_PATH/config
    echo 0x0F1A8078 > $DCC_PATH/config
    echo 0x0F1A8084 > $DCC_PATH/config
    echo 0x0F1B8078 > $DCC_PATH/config
    echo 0x0F1B8084 > $DCC_PATH/config
    echo 0x0F521818 > $DCC_PATH/config
    echo 0x0F52181C > $DCC_PATH/config
    echo 0x0F521828 > $DCC_PATH/config
    echo 0x0F523818 > $DCC_PATH/config
    echo 0x0F52381C > $DCC_PATH/config
    echo 0x0F523828 > $DCC_PATH/config
    echo 0x0F522C18 > $DCC_PATH/config
    echo 0x0F111310 > $DCC_PATH/config
    echo 0x0F111314 > $DCC_PATH/config
    echo 0x0F111318 > $DCC_PATH/config
    echo 0x0F01125C > $DCC_PATH/config
    echo 0x0F011258 > $DCC_PATH/config
    echo 0x0F011310 > $DCC_PATH/config
    echo 0x0F011314 > $DCC_PATH/config
    echo 0x0F011318 > $DCC_PATH/config
}

config_bengal_dcc_gpu()
{
    #GCC
    echo 0x141102C > $DCC_PATH/config
    echo 0x1436004 > $DCC_PATH/config
    echo 0x1471154 > $DCC_PATH/config
    echo 0x141050C > $DCC_PATH/config
    echo 0x143600C > $DCC_PATH/config
    echo 0x1436018 > $DCC_PATH/config
    echo 0x1480220 > $DCC_PATH/config
    echo 0x147C000 > $DCC_PATH/config
    echo 0x147D000 > $DCC_PATH/config
    echo 0x14800A0 > $DCC_PATH/config
    echo 0x1480164 > $DCC_PATH/config
    echo 0x14801E4 > $DCC_PATH/config
    echo 0x1436048 > $DCC_PATH/config
    echo 0x1436040 > $DCC_PATH/config

    #GPUCC
    echo 0x5991004 > $DCC_PATH/config
    echo 0x599100c > $DCC_PATH/config
    echo 0x5991010 > $DCC_PATH/config
    echo 0x5991014 > $DCC_PATH/config
    echo 0x5991054 > $DCC_PATH/config
    echo 0x5991060 > $DCC_PATH/config
    echo 0x599106c > $DCC_PATH/config
    echo 0x5991070 > $DCC_PATH/config
    echo 0x5991074 > $DCC_PATH/config
    echo 0x5991078 > $DCC_PATH/config
    echo 0x599107c > $DCC_PATH/config
    echo 0x599108c > $DCC_PATH/config
    echo 0x5991098 > $DCC_PATH/config
    echo 0x599109c > $DCC_PATH/config
    echo 0x5991540 > $DCC_PATH/config
    echo 0x5995000 > $DCC_PATH/config
    echo 0x5995004 > $DCC_PATH/config

    echo 0x599101C > $DCC_PATH/config
    echo 0x5991020 > $DCC_PATH/config
    echo 0x5990000 > $DCC_PATH/config
    echo 0x5990100 > $DCC_PATH/config
    echo 0x5991508 > $DCC_PATH/config
    echo 0x59910A4 > $DCC_PATH/config
    echo 0x5991578 > $DCC_PATH/config
    echo 0x5990010 > $DCC_PATH/config
    echo 0x5990110 > $DCC_PATH/config
}

config_bengal_dcc_gcc_mm()
{
    echo 0x01480140 > $DCC_PATH/config
    echo 0x01481140 > $DCC_PATH/config
    echo 0x0148014C > $DCC_PATH/config
    echo 0x0148114C > $DCC_PATH/config

    echo 0x01477008 > $DCC_PATH/config
    echo 0x01439014 > $DCC_PATH/config
    echo 0x0143900C > $DCC_PATH/config
    echo 0x01439004 > $DCC_PATH/config
    echo 0x01439000 > $DCC_PATH/config
    echo 0x01439008 > $DCC_PATH/config
    echo 0x01415010 > $DCC_PATH/config
    echo 0x01416010 > $DCC_PATH/config
    echo 0x0142A00C > $DCC_PATH/config
}

config_bengal_dcc_gcc()
{
    echo 0x1400000  > $DCC_PATH/config
    echo 0x1400004  > $DCC_PATH/config
    echo 0x1400008  > $DCC_PATH/config
    echo 0x140000C  > $DCC_PATH/config
    echo 0x1400010  > $DCC_PATH/config
    echo 0x1400014  > $DCC_PATH/config
    echo 0x1400018  > $DCC_PATH/config
    echo 0x140001C  > $DCC_PATH/config
    echo 0x1400020  > $DCC_PATH/config
    echo 0x1400024  > $DCC_PATH/config
    echo 0x1401000  > $DCC_PATH/config
    echo 0x1401004  > $DCC_PATH/config
    echo 0x1401008  > $DCC_PATH/config
    echo 0x140100C  > $DCC_PATH/config
    echo 0x1401010  > $DCC_PATH/config
    echo 0x1401014  > $DCC_PATH/config
    echo 0x1401018  > $DCC_PATH/config
    echo 0x140101C  > $DCC_PATH/config
    echo 0x1401020  > $DCC_PATH/config
    echo 0x1401024  > $DCC_PATH/config
    echo 0x1402000  > $DCC_PATH/config
    echo 0x1402004  > $DCC_PATH/config
    echo 0x1402008  > $DCC_PATH/config
    echo 0x140200C  > $DCC_PATH/config
    echo 0x1402010  > $DCC_PATH/config
    echo 0x1402014  > $DCC_PATH/config
    echo 0x1402018  > $DCC_PATH/config
    echo 0x140201C  > $DCC_PATH/config
    echo 0x1402020  > $DCC_PATH/config
    echo 0x1402024  > $DCC_PATH/config
    echo 0x1403000  > $DCC_PATH/config
    echo 0x1403004  > $DCC_PATH/config
    echo 0x1403008  > $DCC_PATH/config
    echo 0x140300C  > $DCC_PATH/config
    echo 0x1403010  > $DCC_PATH/config
    echo 0x1403014  > $DCC_PATH/config
    echo 0x1403018  > $DCC_PATH/config
    echo 0x140301C  > $DCC_PATH/config
    echo 0x1403020  > $DCC_PATH/config
    echo 0x1403024  > $DCC_PATH/config
    echo 0x1404000  > $DCC_PATH/config
    echo 0x1404004  > $DCC_PATH/config
    echo 0x1404008  > $DCC_PATH/config
    echo 0x140400C  > $DCC_PATH/config
    echo 0x1404010  > $DCC_PATH/config
    echo 0x1404014  > $DCC_PATH/config
    echo 0x1404018  > $DCC_PATH/config
    echo 0x140401C  > $DCC_PATH/config
    echo 0x1404020  > $DCC_PATH/config
    echo 0x1404024  > $DCC_PATH/config
    echo 0x1405000  > $DCC_PATH/config
    echo 0x1405004  > $DCC_PATH/config
    echo 0x1405008  > $DCC_PATH/config
    echo 0x140500C  > $DCC_PATH/config
    echo 0x1405010  > $DCC_PATH/config
    echo 0x1405014  > $DCC_PATH/config
    echo 0x1405018  > $DCC_PATH/config
    echo 0x140501C  > $DCC_PATH/config
    echo 0x1405020  > $DCC_PATH/config
    echo 0x1405024  > $DCC_PATH/config
    echo 0x1406000  > $DCC_PATH/config
    echo 0x1406004  > $DCC_PATH/config
    echo 0x1406008  > $DCC_PATH/config
    echo 0x140600C  > $DCC_PATH/config
    echo 0x1406010  > $DCC_PATH/config
    echo 0x1406014  > $DCC_PATH/config
    echo 0x1406018  > $DCC_PATH/config
    echo 0x140601C  > $DCC_PATH/config
    echo 0x1406020  > $DCC_PATH/config
    echo 0x1406024  > $DCC_PATH/config
    echo 0x1407000  > $DCC_PATH/config
    echo 0x1407004  > $DCC_PATH/config
    echo 0x1407008  > $DCC_PATH/config
    echo 0x140700C  > $DCC_PATH/config
    echo 0x1407010  > $DCC_PATH/config
    echo 0x1407014  > $DCC_PATH/config
    echo 0x1407018  > $DCC_PATH/config
    echo 0x140701C  > $DCC_PATH/config
    echo 0x1407020  > $DCC_PATH/config
    echo 0x1407024  > $DCC_PATH/config
    echo 0x1407028  > $DCC_PATH/config
    echo 0x140702C  > $DCC_PATH/config
    echo 0x1407030  > $DCC_PATH/config
    echo 0x1407034  > $DCC_PATH/config
    echo 0x1408000  > $DCC_PATH/config
    echo 0x1408004  > $DCC_PATH/config
    echo 0x1408008  > $DCC_PATH/config
    echo 0x140800C  > $DCC_PATH/config
    echo 0x1408010  > $DCC_PATH/config
    echo 0x1408014  > $DCC_PATH/config
    echo 0x1408018  > $DCC_PATH/config
    echo 0x140801C  > $DCC_PATH/config
    echo 0x1408020  > $DCC_PATH/config
    echo 0x1408024  > $DCC_PATH/config
    echo 0x1409000  > $DCC_PATH/config
    echo 0x1409004  > $DCC_PATH/config
    echo 0x1409008  > $DCC_PATH/config
    echo 0x140900C  > $DCC_PATH/config
    echo 0x1409010  > $DCC_PATH/config
    echo 0x1409014  > $DCC_PATH/config
    echo 0x1409018  > $DCC_PATH/config
    echo 0x140901C  > $DCC_PATH/config
    echo 0x1409020  > $DCC_PATH/config
    echo 0x1409024  > $DCC_PATH/config
    echo 0x141001C  > $DCC_PATH/config
    echo 0x14103EC  > $DCC_PATH/config
    echo 0x1414024  > $DCC_PATH/config
    echo 0x1415004  > $DCC_PATH/config
    echo 0x1415008  > $DCC_PATH/config
    echo 0x141500C  > $DCC_PATH/config
    echo 0x1415034  > $DCC_PATH/config
    echo 0x1416004  > $DCC_PATH/config
    echo 0x1416008  > $DCC_PATH/config
    echo 0x141600C  > $DCC_PATH/config
    echo 0x1416038  > $DCC_PATH/config
    echo 0x141703C  > $DCC_PATH/config
    echo 0x1417040  > $DCC_PATH/config
    echo 0x141A004  > $DCC_PATH/config
    echo 0x141A008  > $DCC_PATH/config
    echo 0x141A00C  > $DCC_PATH/config
    echo 0x141A01C  > $DCC_PATH/config
    echo 0x141A020  > $DCC_PATH/config
    echo 0x141A034  > $DCC_PATH/config
    echo 0x141A038  > $DCC_PATH/config
    echo 0x141A060  > $DCC_PATH/config
    echo 0x141A064  > $DCC_PATH/config
    echo 0x141E00C  > $DCC_PATH/config
    echo 0x141E010  > $DCC_PATH/config
    echo 0x141F02C  > $DCC_PATH/config
    echo 0x141F148  > $DCC_PATH/config
    echo 0x141F14C  > $DCC_PATH/config
    echo 0x141F15C  > $DCC_PATH/config
    echo 0x141F278  > $DCC_PATH/config
    echo 0x141F27C  > $DCC_PATH/config
    echo 0x141F28C  > $DCC_PATH/config
    echo 0x141F3A8  > $DCC_PATH/config
    echo 0x141F3AC  > $DCC_PATH/config
    echo 0x141F3BC  > $DCC_PATH/config
    echo 0x141F4D8  > $DCC_PATH/config
    echo 0x141F4DC  > $DCC_PATH/config
    echo 0x141F4EC  > $DCC_PATH/config
    echo 0x141F608  > $DCC_PATH/config
    echo 0x141F60C  > $DCC_PATH/config
    echo 0x141F61C  > $DCC_PATH/config
    echo 0x141F738  > $DCC_PATH/config
    echo 0x141F73C  > $DCC_PATH/config
    echo 0x141F74C  > $DCC_PATH/config
    echo 0x1420010  > $DCC_PATH/config
    echo 0x1420014  > $DCC_PATH/config
    echo 0x1426018  > $DCC_PATH/config
    echo 0x142601C  > $DCC_PATH/config
    echo 0x1426030  > $DCC_PATH/config
    echo 0x1426034  > $DCC_PATH/config
    echo 0x1427024  > $DCC_PATH/config
    echo 0x1428014  > $DCC_PATH/config
    echo 0x1428018  > $DCC_PATH/config
    echo 0x142802C  > $DCC_PATH/config
    echo 0x1428030  > $DCC_PATH/config
    echo 0x1429004  > $DCC_PATH/config
    echo 0x1429008  > $DCC_PATH/config
    echo 0x142900C  > $DCC_PATH/config
    echo 0x1429040  > $DCC_PATH/config
    echo 0x1429044  > $DCC_PATH/config
    echo 0x142A000  > $DCC_PATH/config
    echo 0x142A004  > $DCC_PATH/config
    echo 0x142A008  > $DCC_PATH/config
    echo 0x142A154  > $DCC_PATH/config
    echo 0x142A158  > $DCC_PATH/config
    echo 0x142B13C  > $DCC_PATH/config
    echo 0x142B140  > $DCC_PATH/config
    echo 0x142B158  > $DCC_PATH/config
    echo 0x142B15C  > $DCC_PATH/config
    echo 0x142E00C  > $DCC_PATH/config
    echo 0x142E010  > $DCC_PATH/config
    echo 0x142F00C  > $DCC_PATH/config
    echo 0x142F010  > $DCC_PATH/config
    echo 0x1432004  > $DCC_PATH/config
    echo 0x1432008  > $DCC_PATH/config
    echo 0x143200C  > $DCC_PATH/config
    echo 0x1432034  > $DCC_PATH/config
    echo 0x1432080  > $DCC_PATH/config
    echo 0x1436024  > $DCC_PATH/config
    echo 0x1436028  > $DCC_PATH/config
    echo 0x1438010  > $DCC_PATH/config
    echo 0x1438014  > $DCC_PATH/config
    echo 0x1438028  > $DCC_PATH/config
    echo 0x143802C  > $DCC_PATH/config
    echo 0x1439148  > $DCC_PATH/config
    echo 0x143914C  > $DCC_PATH/config
    echo 0x143915C  > $DCC_PATH/config
    echo 0x1439278  > $DCC_PATH/config
    echo 0x143927C  > $DCC_PATH/config
    echo 0x143928C  > $DCC_PATH/config
    echo 0x14393A8  > $DCC_PATH/config
    echo 0x14393AC  > $DCC_PATH/config
    echo 0x14393BC  > $DCC_PATH/config
    echo 0x14394D8  > $DCC_PATH/config
    echo 0x14394DC  > $DCC_PATH/config
    echo 0x14394EC  > $DCC_PATH/config
    echo 0x1439608  > $DCC_PATH/config
    echo 0x143960C  > $DCC_PATH/config
    echo 0x143961C  > $DCC_PATH/config
    echo 0x1439738  > $DCC_PATH/config
    echo 0x143973C  > $DCC_PATH/config
    echo 0x143974C  > $DCC_PATH/config
    echo 0x143B000  > $DCC_PATH/config
    echo 0x143B004  > $DCC_PATH/config
    echo 0x143B008  > $DCC_PATH/config
    echo 0x143B00C  > $DCC_PATH/config
    echo 0x143B010  > $DCC_PATH/config
    echo 0x143B014  > $DCC_PATH/config
    echo 0x143B018  > $DCC_PATH/config
    echo 0x143B01C  > $DCC_PATH/config
    echo 0x143B020  > $DCC_PATH/config
    echo 0x143B024  > $DCC_PATH/config
    echo 0x143B028  > $DCC_PATH/config
    echo 0x143D01C  > $DCC_PATH/config
    echo 0x143D020  > $DCC_PATH/config
    echo 0x143E000  > $DCC_PATH/config
    echo 0x143E004  > $DCC_PATH/config
    echo 0x143E008  > $DCC_PATH/config
    echo 0x143E00C  > $DCC_PATH/config
    echo 0x143E010  > $DCC_PATH/config
    echo 0x143E014  > $DCC_PATH/config
    echo 0x143E018  > $DCC_PATH/config
    echo 0x143E01C  > $DCC_PATH/config
    echo 0x143E020  > $DCC_PATH/config
    echo 0x143E024  > $DCC_PATH/config
    echo 0x143E028  > $DCC_PATH/config
    echo 0x143E060  > $DCC_PATH/config
    echo 0x143F000  > $DCC_PATH/config
    echo 0x143F004  > $DCC_PATH/config
    echo 0x143F008  > $DCC_PATH/config
    echo 0x143F00C  > $DCC_PATH/config
    echo 0x143F010  > $DCC_PATH/config
    echo 0x143F014  > $DCC_PATH/config
    echo 0x143F018  > $DCC_PATH/config
    echo 0x143F01C  > $DCC_PATH/config
    echo 0x143F020  > $DCC_PATH/config
    echo 0x143F024  > $DCC_PATH/config
    echo 0x143F028  > $DCC_PATH/config
    echo 0x1442018  > $DCC_PATH/config
    echo 0x144201C  > $DCC_PATH/config
    echo 0x1442030  > $DCC_PATH/config
    echo 0x1442034  > $DCC_PATH/config
    echo 0x1445004  > $DCC_PATH/config
    echo 0x1445008  > $DCC_PATH/config
    echo 0x144500C  > $DCC_PATH/config
    echo 0x1445020  > $DCC_PATH/config
    echo 0x1445024  > $DCC_PATH/config
    echo 0x1445048  > $DCC_PATH/config
    echo 0x144504C  > $DCC_PATH/config
    echo 0x1445060  > $DCC_PATH/config
    echo 0x1445064  > $DCC_PATH/config
    echo 0x144507C  > $DCC_PATH/config
    echo 0x1445080  > $DCC_PATH/config
    echo 0x1446004  > $DCC_PATH/config
    echo 0x1446008  > $DCC_PATH/config
    echo 0x1446024  > $DCC_PATH/config
    echo 0x1446150  > $DCC_PATH/config
    echo 0x1448024  > $DCC_PATH/config
    echo 0x144D004  > $DCC_PATH/config
    echo 0x144D008  > $DCC_PATH/config
    echo 0x144E004  > $DCC_PATH/config
    echo 0x144E008  > $DCC_PATH/config
    echo 0x144F004  > $DCC_PATH/config
    echo 0x144F008  > $DCC_PATH/config
    echo 0x1450000  > $DCC_PATH/config
    echo 0x1450004  > $DCC_PATH/config
    echo 0x145001C  > $DCC_PATH/config
    echo 0x1450020  > $DCC_PATH/config
    echo 0x1451000  > $DCC_PATH/config
    echo 0x1451004  > $DCC_PATH/config
    echo 0x145101C  > $DCC_PATH/config
    echo 0x1451020  > $DCC_PATH/config
    echo 0x1451038  > $DCC_PATH/config
    echo 0x145103C  > $DCC_PATH/config
    echo 0x1451054  > $DCC_PATH/config
    echo 0x1451058  > $DCC_PATH/config
    echo 0x1452004  > $DCC_PATH/config
    echo 0x1452008  > $DCC_PATH/config
    echo 0x1452028  > $DCC_PATH/config
    echo 0x145202C  > $DCC_PATH/config
    echo 0x1453004  > $DCC_PATH/config
    echo 0x1453008  > $DCC_PATH/config
    echo 0x1453024  > $DCC_PATH/config
    echo 0x1453028  > $DCC_PATH/config
    echo 0x1453044  > $DCC_PATH/config
    echo 0x1453048  > $DCC_PATH/config
    echo 0x1454004  > $DCC_PATH/config
    echo 0x1454008  > $DCC_PATH/config
    echo 0x145400C  > $DCC_PATH/config
    echo 0x1454010  > $DCC_PATH/config
    echo 0x1454014  > $DCC_PATH/config
    echo 0x145403C  > $DCC_PATH/config
    echo 0x1454040  > $DCC_PATH/config
    echo 0x1454044  > $DCC_PATH/config
    echo 0x1454048  > $DCC_PATH/config
    echo 0x145404C  > $DCC_PATH/config
    echo 0x1455000  > $DCC_PATH/config
    echo 0x1455004  > $DCC_PATH/config
    echo 0x1455030  > $DCC_PATH/config
    echo 0x1455034  > $DCC_PATH/config
    echo 0x145506C  > $DCC_PATH/config
    echo 0x1455070  > $DCC_PATH/config
    echo 0x14550A4  > $DCC_PATH/config
    echo 0x14550A8  > $DCC_PATH/config
    echo 0x14550E0  > $DCC_PATH/config
    echo 0x14550E4  > $DCC_PATH/config
    echo 0x145607C  > $DCC_PATH/config
    echo 0x1456080  > $DCC_PATH/config
    echo 0x1456084  > $DCC_PATH/config
    echo 0x1456088  > $DCC_PATH/config
    echo 0x145608C  > $DCC_PATH/config
    echo 0x14560BC  > $DCC_PATH/config
    echo 0x14560C0  > $DCC_PATH/config
    echo 0x14560C4  > $DCC_PATH/config
    echo 0x14560C8  > $DCC_PATH/config
    echo 0x14560CC  > $DCC_PATH/config
    echo 0x1457000  > $DCC_PATH/config
    echo 0x1457004  > $DCC_PATH/config
    echo 0x1457008  > $DCC_PATH/config
    echo 0x145700C  > $DCC_PATH/config
    echo 0x1457010  > $DCC_PATH/config
    echo 0x145A000  > $DCC_PATH/config
    echo 0x145A004  > $DCC_PATH/config
    echo 0x145A008  > $DCC_PATH/config
    echo 0x145A00C  > $DCC_PATH/config
    echo 0x145A010  > $DCC_PATH/config
    echo 0x145B000  > $DCC_PATH/config
    echo 0x145B004  > $DCC_PATH/config
    echo 0x145B00C  > $DCC_PATH/config
    echo 0x1463020  > $DCC_PATH/config
    echo 0x1469000  > $DCC_PATH/config
    echo 0x1469004  > $DCC_PATH/config
    echo 0x1469008  > $DCC_PATH/config
    echo 0x146900C  > $DCC_PATH/config
    echo 0x1469010  > $DCC_PATH/config
    echo 0x146B000  > $DCC_PATH/config
    echo 0x146B004  > $DCC_PATH/config
    echo 0x146B008  > $DCC_PATH/config
    echo 0x146B00C  > $DCC_PATH/config
    echo 0x146B010  > $DCC_PATH/config
    echo 0x146B014  > $DCC_PATH/config
    echo 0x146B018  > $DCC_PATH/config
    echo 0x146B01C  > $DCC_PATH/config
    echo 0x146B020  > $DCC_PATH/config
    echo 0x146C000  > $DCC_PATH/config
    echo 0x146C004  > $DCC_PATH/config
    echo 0x146C00C  > $DCC_PATH/config
    echo 0x1475000  > $DCC_PATH/config
    echo 0x1475004  > $DCC_PATH/config
    echo 0x147500C  > $DCC_PATH/config
    echo 0x1477000  > $DCC_PATH/config
    echo 0x1477004  > $DCC_PATH/config
    echo 0x147700C  > $DCC_PATH/config
    echo 0x1478030  > $DCC_PATH/config
    echo 0x1479000  > $DCC_PATH/config
    echo 0x1479004  > $DCC_PATH/config
    echo 0x147900C  > $DCC_PATH/config
    echo 0x147A000  > $DCC_PATH/config
    echo 0x147A004  > $DCC_PATH/config
    echo 0x147A00C  > $DCC_PATH/config
    echo 0x147B000  > $DCC_PATH/config
    echo 0x147B004  > $DCC_PATH/config
    echo 0x147B00C  > $DCC_PATH/config
    echo 0x1480018  > $DCC_PATH/config
    echo 0x1480028  > $DCC_PATH/config
    echo 0x148005C  > $DCC_PATH/config
    echo 0x1480144  > $DCC_PATH/config
    echo 0x1480154  > $DCC_PATH/config
    echo 0x14801B8  > $DCC_PATH/config
    echo 0x14801C0  > $DCC_PATH/config
    echo 0x14801C8  > $DCC_PATH/config
    echo 0x14801D0  > $DCC_PATH/config
    echo 0x1480250  > $DCC_PATH/config
    echo 0x1480254  > $DCC_PATH/config
    echo 0x1480258  > $DCC_PATH/config
    echo 0x1480270  > $DCC_PATH/config
    echo 0x1481144  > $DCC_PATH/config
    echo 0x148B004  > $DCC_PATH/config
    echo 0x1490004  > $DCC_PATH/config
    echo 0x1490008  > $DCC_PATH/config
    echo 0x1490024  > $DCC_PATH/config
    echo 0x1490028  > $DCC_PATH/config
    echo 0x149002C  > $DCC_PATH/config
    echo 0x1490030  > $DCC_PATH/config
    echo 0x1490034  > $DCC_PATH/config
    echo 0x1495000  > $DCC_PATH/config
    echo 0x1495004  > $DCC_PATH/config
    echo 0x149500C  > $DCC_PATH/config
    echo 0x14B5000  > $DCC_PATH/config
    echo 0x14C4000  > $DCC_PATH/config
    echo 0x14C5000  > $DCC_PATH/config
    echo 0x14C6000  > $DCC_PATH/config
    echo 0x14C7000  > $DCC_PATH/config
    echo 0x440C000  > $DCC_PATH/config
    echo 0x440C004  > $DCC_PATH/config
    echo 0x440C008  > $DCC_PATH/config
    echo 0x440C040  > $DCC_PATH/config
    echo 0x440C044  > $DCC_PATH/config
    echo 0x440C048  > $DCC_PATH/config
    echo 0x440C04C  > $DCC_PATH/config
    echo 0x440C050  > $DCC_PATH/config
    echo 0x440C054  > $DCC_PATH/config
    echo 0x440C058  > $DCC_PATH/config
    echo 0x440C05C  > $DCC_PATH/config
    echo 0x440C060  > $DCC_PATH/config
    echo 0x440C068  > $DCC_PATH/config
    echo 0x440C06C  > $DCC_PATH/config
    echo 0x440E050  > $DCC_PATH/config
    echo 0x440E054  > $DCC_PATH/config
    echo 0x440E0A0  > $DCC_PATH/config
    echo 0x440E0A4  > $DCC_PATH/config
    echo 0x440F010  > $DCC_PATH/config
}

config_bengal_dcc_lpm()
{
    #APCS_ALIAS0_SAW4
    echo 0xf189000 > $DCC_PATH/config
    echo 0xf18900c > $DCC_PATH/config
    echo 0xf189c0c > $DCC_PATH/config
    echo 0xf189c10 > $DCC_PATH/config
    echo 0xf189c20 > $DCC_PATH/config
    #APCS_ALIAS1_SAW4
    echo 0xf199000 > $DCC_PATH/config
    echo 0xf19900c > $DCC_PATH/config
    echo 0xf199c0c > $DCC_PATH/config
    echo 0xf199c10 > $DCC_PATH/config
    echo 0xf199c20 > $DCC_PATH/config
    #APCS_ALIAS2_SAW4
    echo 0xf1a9000 > $DCC_PATH/config
    echo 0xf1a900c > $DCC_PATH/config
    echo 0xf1a9c0c > $DCC_PATH/config
    echo 0xf1a9c10 > $DCC_PATH/config
    echo 0xf1a9c20 > $DCC_PATH/config
    #APCS_ALIAS3_SAW4
    echo 0xf1b9000 > $DCC_PATH/config
    echo 0xf1b900c > $DCC_PATH/config
    echo 0xf1b9c0c > $DCC_PATH/config
    echo 0xf1b9c10 > $DCC_PATH/config
    echo 0xf1b9c18 > $DCC_PATH/config
    #APCS_ALIAS4_SAW4
    echo 0xf089000 > $DCC_PATH/config
    echo 0xf08900c > $DCC_PATH/config
    echo 0xf089c0c > $DCC_PATH/config
    echo 0xf089c10 > $DCC_PATH/config
    echo 0xf089c20 > $DCC_PATH/config
    #APCS_ALIAS5_SAW4
    echo 0xf099000 > $DCC_PATH/config
    echo 0xf09900c > $DCC_PATH/config
    echo 0xf099c0c > $DCC_PATH/config
    echo 0xf099c10 > $DCC_PATH/config
    echo 0xf099c20 > $DCC_PATH/config
    #APCS_ALIAS6_SAW4
    echo 0xf0a9000 > $DCC_PATH/config
    echo 0xf0a900c > $DCC_PATH/config
    echo 0xf0a9c0c > $DCC_PATH/config
    echo 0xf0a9c10 > $DCC_PATH/config
    echo 0xf0a9c20 > $DCC_PATH/config
    #APCS_ALIAS7_SAW4
    echo 0xf0b9000 > $DCC_PATH/config
    echo 0xf0b900c > $DCC_PATH/config
    echo 0xf0b9c0c > $DCC_PATH/config
    echo 0xf0b9c10 > $DCC_PATH/config
    echo 0xf0b9c20 > $DCC_PATH/config

    #APCLUS0_L2_SAW4
    echo 0xf112000 > $DCC_PATH/config
    echo 0xf11200c > $DCC_PATH/config
    echo 0xf112c0c > $DCC_PATH/config
    echo 0xf112c10 > $DCC_PATH/config
    echo 0xf112c20 > $DCC_PATH/config

    #APCLUS1_L2_SAW4
    echo 0xf012000 > $DCC_PATH/config
    echo 0xf01200c > $DCC_PATH/config
    echo 0xf012c0c > $DCC_PATH/config
    echo 0xf012c10 > $DCC_PATH/config
    echo 0xf012c20 > $DCC_PATH/config

    #CCI_SAW4
    echo 0xf1d2000 > $DCC_PATH/config
    echo 0xf1d200c > $DCC_PATH/config
    echo 0xf1d2c0c > $DCC_PATH/config
    echo 0xf1d2c10 > $DCC_PATH/config
    echo 0xf1d2c20 > $DCC_PATH/config

    #APCS_ALIAS1_L2
    echo 0xf011014 > $DCC_PATH/config
    echo 0xf011018 > $DCC_PATH/config
    echo 0xf011218 > $DCC_PATH/config
    echo 0xf011234 > $DCC_PATH/config
    echo 0xf011220 > $DCC_PATH/config
    echo 0xf011264 > $DCC_PATH/config
    echo 0xf011290 > $DCC_PATH/config

    #APCS_ALIAS0_L2
    echo 0xf111014 > $DCC_PATH/config
    echo 0xf111018 > $DCC_PATH/config
    echo 0xf111218 > $DCC_PATH/config
    echo 0xf111234 > $DCC_PATH/config
    echo 0xf111264 > $DCC_PATH/config
    echo 0xf111290 > $DCC_PATH/config

    #Curr Frequency
    echo 0x0F521700 > $DCC_PATH/config
    echo 0x0F523700 > $DCC_PATH/config

    #Cluster voltage
    echo 0x0F112C18 > $DCC_PATH/config
    echo 0x0F012C18 > $DCC_PATH/config

    #CPRh info
    echo 0x0F513A84 > $DCC_PATH/config
    echo 0x0F513A88 > $DCC_PATH/config

    #PIMEM
    echo 0x01B60110 > $DCC_PATH/config
}

config_bengal_dcc_noc()
{
    echo 0x1900010 > $DCC_PATH/config
    echo 0x1900020 > $DCC_PATH/config
    echo 0x1900024 > $DCC_PATH/config
    echo 0x1900028 > $DCC_PATH/config
    echo 0x190002C > $DCC_PATH/config
    echo 0x1900030 > $DCC_PATH/config
    echo 0x1900034 > $DCC_PATH/config
    echo 0x1900038 > $DCC_PATH/config
    echo 0x190003C > $DCC_PATH/config
    echo 0x1900300 > $DCC_PATH/config
    echo 0x1900304 > $DCC_PATH/config
    echo 0x1900308 > $DCC_PATH/config
    echo 0x190030C > $DCC_PATH/config
    echo 0x1900310 > $DCC_PATH/config
    echo 0x1900314 > $DCC_PATH/config
    echo 0x1900900 > $DCC_PATH/config
    echo 0x1900904 > $DCC_PATH/config
    echo 0x1900B00 > $DCC_PATH/config
    echo 0x1900D00 > $DCC_PATH/config
    echo 0x1909100 > $DCC_PATH/config
    echo 0x1909104 > $DCC_PATH/config
    echo 0x1480140 > $DCC_PATH/config
    echo 0x1481140 > $DCC_PATH/config
    echo 0x1415008 > $DCC_PATH/config
    echo 0x1416008 > $DCC_PATH/config
    echo 0x44B0120 > $DCC_PATH/config
    echo 0x44B0124 > $DCC_PATH/config
    echo 0x44B0128 > $DCC_PATH/config
    echo 0x44B012C > $DCC_PATH/config
    echo 0x44B0130 > $DCC_PATH/config
    echo 0x44B0100 > $DCC_PATH/config
    echo 0x44B0020 > $DCC_PATH/config
    echo 0x44C4000 > $DCC_PATH/config
    echo 0x44C4020 > $DCC_PATH/config
    echo 0x44C4030 > $DCC_PATH/config
    echo 0x44C4100 > $DCC_PATH/config
    echo 0x44C410C > $DCC_PATH/config
    echo 0x44C4400 > $DCC_PATH/config
    echo 0x44C4410 > $DCC_PATH/config
    echo 0x44C4420 > $DCC_PATH/config

    echo 0x1900240 > $DCC_PATH/config
    echo 0x1900244 > $DCC_PATH/config
    echo 0x1900248 > $DCC_PATH/config
    echo 0x190024C > $DCC_PATH/config
    echo 0x1900250 > $DCC_PATH/config
    echo 0x1900258 > $DCC_PATH/config

    echo 0x1411004 > $DCC_PATH/config
    echo 0x1411028 > $DCC_PATH/config
    #echo 0x141102C > $DCC_PATH/config

    echo 0x1458004 > $DCC_PATH/config

    echo 0x1880108 > $DCC_PATH/config
    echo 0x1880110 > $DCC_PATH/config
    echo 0x1880120 > $DCC_PATH/config
    echo 0x1880124 > $DCC_PATH/config
    echo 0x1880128 > $DCC_PATH/config
    echo 0x188012C > $DCC_PATH/config
    echo 0x1880130 > $DCC_PATH/config
    echo 0x1880134 > $DCC_PATH/config
    echo 0x1880138 > $DCC_PATH/config
    echo 0x188013C > $DCC_PATH/config
    echo 0x1880300 > $DCC_PATH/config
    echo 0x1880304 > $DCC_PATH/config
    echo 0x1880308 > $DCC_PATH/config
    echo 0x188030C > $DCC_PATH/config
    echo 0x1880310 > $DCC_PATH/config
    echo 0x1880314 > $DCC_PATH/config
    echo 0x1880318 > $DCC_PATH/config
    echo 0x188031C > $DCC_PATH/config
    #echo 0x1880500 > $DCC_PATH/config
    echo 0x1880700 > $DCC_PATH/config
    echo 0x1880704 > $DCC_PATH/config
    echo 0x1880708 > $DCC_PATH/config
    echo 0x188070C > $DCC_PATH/config
    echo 0x1880710 > $DCC_PATH/config
    echo 0x1880714 > $DCC_PATH/config
    echo 0x1880718 > $DCC_PATH/config
    echo 0x188071C > $DCC_PATH/config
    #echo 0x1880B00 > $DCC_PATH/config
    #echo 0x1880B04 > $DCC_PATH/config
    #echo 0x1880D00 > $DCC_PATH/config
    echo 0x1881100 > $DCC_PATH/config

    echo 0x1880240 > $DCC_PATH/config
    echo 0x1880248 > $DCC_PATH/config

}

config_bengal_dcc_qdsp()
{
    echo 0xA754520  > $DCC_PATH/config
    echo 0xA751020  > $DCC_PATH/config
    echo 0xA751024  > $DCC_PATH/config
    echo 0xA751030  > $DCC_PATH/config
    echo 0xA751200  > $DCC_PATH/config
    echo 0xA751214  > $DCC_PATH/config
    echo 0xA751228  > $DCC_PATH/config
    echo 0xA75123C  > $DCC_PATH/config
    echo 0xA751250  > $DCC_PATH/config
    echo 0xA751204  > $DCC_PATH/config
    echo 0xA751218  > $DCC_PATH/config
    echo 0xA75122C  > $DCC_PATH/config
    echo 0xA751240  > $DCC_PATH/config
    echo 0xA751254  > $DCC_PATH/config
    echo 0xA751208  > $DCC_PATH/config
    echo 0xA75121C  > $DCC_PATH/config
    echo 0xA751230  > $DCC_PATH/config
    echo 0xA751244  > $DCC_PATH/config
    echo 0xA751258  > $DCC_PATH/config
    echo 0xA754510  > $DCC_PATH/config
    echo 0xA754514  > $DCC_PATH/config
    echo 0xA750010  > $DCC_PATH/config
    echo 0xA750014  > $DCC_PATH/config
    echo 0xA750900  > $DCC_PATH/config
    echo 0xA750904  > $DCC_PATH/config

    echo 0x0A402028 > $DCC_PATH/config

    echo 0x0A900010 > $DCC_PATH/config
    echo 0x0A900014 > $DCC_PATH/config
    echo 0x0A900018 > $DCC_PATH/config
    echo 0x0A900030 > $DCC_PATH/config
    echo 0x0A900038 > $DCC_PATH/config
    echo 0x0A900040 > $DCC_PATH/config
    echo 0x0A900048 > $DCC_PATH/config
    echo 0x0A9000D0 > $DCC_PATH/config
    echo 0x0A900210 > $DCC_PATH/config
    echo 0x0A900230 > $DCC_PATH/config
    echo 0x0A900250 > $DCC_PATH/config
    echo 0x0A900270 > $DCC_PATH/config
    echo 0x0A900290 > $DCC_PATH/config
    echo 0x0A9002B0 > $DCC_PATH/config
    echo 0x0A900208 > $DCC_PATH/config
    echo 0x0A900228 > $DCC_PATH/config
    echo 0x0A900248 > $DCC_PATH/config
    echo 0x0A900268 > $DCC_PATH/config
    echo 0x0A900288 > $DCC_PATH/config
    echo 0x0A9002A8 > $DCC_PATH/config
    echo 0x0A90020C > $DCC_PATH/config
    echo 0x0A90022C > $DCC_PATH/config
    echo 0x0A90024C > $DCC_PATH/config
    echo 0x0A90026C > $DCC_PATH/config
    echo 0x0A90028C > $DCC_PATH/config
    echo 0x0A9002AC > $DCC_PATH/config
    echo 0x0A900404 > $DCC_PATH/config
    echo 0x0A900408 > $DCC_PATH/config
    echo 0x0A900400 > $DCC_PATH/config
    echo 0x0A900D04 > $DCC_PATH/config

    echo 0x0A4B0010 > $DCC_PATH/config
    echo 0x0A4B0014 > $DCC_PATH/config
    echo 0x0A4B0018 > $DCC_PATH/config
    echo 0x0A4B0210 > $DCC_PATH/config
    echo 0x0A4B0230 > $DCC_PATH/config
    echo 0x0A4B0250 > $DCC_PATH/config
    echo 0x0A4B0270 > $DCC_PATH/config
    echo 0x0A4B0290 > $DCC_PATH/config
    echo 0x0A4B02B0 > $DCC_PATH/config
    echo 0x0A4B0208 > $DCC_PATH/config
    echo 0x0A4B0228 > $DCC_PATH/config
    echo 0x0A4B0248 > $DCC_PATH/config
    echo 0x0A4B0268 > $DCC_PATH/config
    echo 0x0A4B0288 > $DCC_PATH/config
    echo 0x0A4B02A8 > $DCC_PATH/config
    echo 0x0A4B020C > $DCC_PATH/config
    echo 0x0A4B022C > $DCC_PATH/config
    echo 0x0A4B024C > $DCC_PATH/config
    echo 0x0A4B026C > $DCC_PATH/config
    echo 0x0A4B028C > $DCC_PATH/config
    echo 0x0A4B02AC > $DCC_PATH/config
    echo 0x0A4B0400 > $DCC_PATH/config
    echo 0x0A4B0404 > $DCC_PATH/config
    echo 0x0A4B0408 > $DCC_PATH/config
}

config_bengal_dcc_misc()
{
    echo 0xF017000 > $DCC_PATH/config
    echo 0xF01700C > $DCC_PATH/config
    echo 0xF017010 > $DCC_PATH/config
    echo 0xF017014 > $DCC_PATH/config
    echo 0xF017018 > $DCC_PATH/config
    echo 0xF017020 > $DCC_PATH/config
    echo 0x1414008 > $DCC_PATH/config
    echo 0x1414004 > $DCC_PATH/config

    echo 0x5991554 > $DCC_PATH/config
    echo 0x5991544 > $DCC_PATH/config
    echo 0x599155C > $DCC_PATH/config

    #MPM_SSCAON_STATUS
    echo 0x440B00C > $DCC_PATH/config
    echo 0x440B014 > $DCC_PATH/config
}

config_modem_rscc()
{
    echo 0x06130010 > $DCC_PATH/config
    echo 0x06130014 > $DCC_PATH/config
    echo 0x06130018 > $DCC_PATH/config
    echo 0x06130210 > $DCC_PATH/config
    echo 0x06130230 > $DCC_PATH/config
    echo 0x06130250 > $DCC_PATH/config
    echo 0x06130270 > $DCC_PATH/config
    echo 0x06130290 > $DCC_PATH/config
    echo 0x061302B0 > $DCC_PATH/config
    echo 0x06130208 > $DCC_PATH/config
    echo 0x06130228 > $DCC_PATH/config
    echo 0x06130248 > $DCC_PATH/config
    echo 0x06130268 > $DCC_PATH/config
    echo 0x06130288 > $DCC_PATH/config
    echo 0x061302A8 > $DCC_PATH/config
    echo 0x0613020C > $DCC_PATH/config
    echo 0x0613022C > $DCC_PATH/config
    echo 0x0613024C > $DCC_PATH/config
    echo 0x0613026C > $DCC_PATH/config
    echo 0x0613028C > $DCC_PATH/config
    echo 0x061302AC > $DCC_PATH/config
    echo 0x06130400 > $DCC_PATH/config
    echo 0x06130404 > $DCC_PATH/config
    echo 0x06130408 > $DCC_PATH/config

    echo 0x6082028  > $DCC_PATH/config
    echo 0x0143300C > $DCC_PATH/config
}

config_cdsp_rscc()
{
    echo 0x0B3B0010  > $DCC_PATH/config
    echo 0x0B3B0014  > $DCC_PATH/config
    echo 0x0B3B0018  > $DCC_PATH/config
    echo 0x0B3B0210  > $DCC_PATH/config
    echo 0x0B3B0230  > $DCC_PATH/config
    echo 0x0B3B0250  > $DCC_PATH/config
    echo 0x0B3B0270  > $DCC_PATH/config
    echo 0x0B3B0290  > $DCC_PATH/config
    echo 0x0B3B02B0  > $DCC_PATH/config
    echo 0x0B3B0208  > $DCC_PATH/config
    echo 0x0B3B0228  > $DCC_PATH/config
    echo 0x0B3B0248  > $DCC_PATH/config
    echo 0x0B3B0268  > $DCC_PATH/config
    echo 0x0B3B0288  > $DCC_PATH/config
    echo 0x0B3B02A8  > $DCC_PATH/config
    echo 0x0B3B020C  > $DCC_PATH/config
    echo 0x0B3B022C  > $DCC_PATH/config
    echo 0x0B3B024C  > $DCC_PATH/config
    echo 0x0B3B026C  > $DCC_PATH/config
    echo 0x0B3B028C  > $DCC_PATH/config
    echo 0x0B3B02AC  > $DCC_PATH/config
    echo 0x0B3B0400  > $DCC_PATH/config
    echo 0x0B3B0404  > $DCC_PATH/config
    echo 0x0B3B0408  > $DCC_PATH/config

    echo 0x0B302028  > $DCC_PATH/config
    echo 0x0B300044 > $DCC_PATH/config
    echo 0x0B300304 > $DCC_PATH/config

}

config_acp_status()
{
    echo  0x9870010 0x14000 > $DCC_PATH/config_write

    echo 0xF011600 > $DCC_PATH/config
    echo 0xF011604 > $DCC_PATH/config
    echo 0xF011608 > $DCC_PATH/config
    echo 0xF01160C > $DCC_PATH/config
    echo 0xF011610 > $DCC_PATH/config
    echo 0xF011614 > $DCC_PATH/config
    echo 0xF011618 > $DCC_PATH/config
    echo 0xF01161C > $DCC_PATH/config
    echo 0xF011620 > $DCC_PATH/config
    echo 0xF011624 > $DCC_PATH/config
    echo 0xF011628 > $DCC_PATH/config
    echo 0xF01162C > $DCC_PATH/config
    echo 0xF011630 > $DCC_PATH/config
    echo 0xF011634 > $DCC_PATH/config
    echo 0xF011638 > $DCC_PATH/config
    echo 0xF01163C > $DCC_PATH/config
    echo 0xF011640 > $DCC_PATH/config
    echo 0xF011644 > $DCC_PATH/config
    echo 0xF011648 > $DCC_PATH/config
    echo 0xF01164C > $DCC_PATH/config
    echo 0xF011650 > $DCC_PATH/config
    echo 0xF011654 > $DCC_PATH/config
    echo 0xF011658 > $DCC_PATH/config
    echo 0xF01165C > $DCC_PATH/config
    echo 0xF011660 > $DCC_PATH/config
    echo 0xF011664 > $DCC_PATH/config

    echo 0xF111600 > $DCC_PATH/config
    echo 0xF111604 > $DCC_PATH/config
    echo 0xF111608 > $DCC_PATH/config
    echo 0xF11160C > $DCC_PATH/config
    echo 0xF111610 > $DCC_PATH/config
    echo 0xF111614 > $DCC_PATH/config
    echo 0xF111618 > $DCC_PATH/config
    echo 0xF11161C > $DCC_PATH/config
    echo 0xF111620 > $DCC_PATH/config
    echo 0xF111624 > $DCC_PATH/config
    echo 0xF111628 > $DCC_PATH/config
    echo 0xF11162C > $DCC_PATH/config
    echo 0xF111630 > $DCC_PATH/config
    echo 0xF111634 > $DCC_PATH/config
    echo 0xF111638 > $DCC_PATH/config
    echo 0xF11163C > $DCC_PATH/config
    echo 0xF111640 > $DCC_PATH/config
    echo 0xF111644 > $DCC_PATH/config
    echo 0xF111648 > $DCC_PATH/config
    echo 0xF11164C > $DCC_PATH/config
    echo 0xF111650 > $DCC_PATH/config
    echo 0xF111654 > $DCC_PATH/config
    echo 0xF111658 > $DCC_PATH/config
    echo 0xF11165C > $DCC_PATH/config
    echo 0xF111660 > $DCC_PATH/config
    echo 0xF111664 > $DCC_PATH/config

    echo 0xF011400 > $DCC_PATH/config
    echo 0xF111400 > $DCC_PATH/config

    echo  0x9870010 0x0 > $DCC_PATH/config_write
}

config_bengal_dcc_core()
{
    # core hang
    echo 0x0F1880B4 1 > $DCC_PATH/config
    echo 0x0F1980B4 1 > $DCC_PATH/config
    echo 0x0F1A80B4 1 > $DCC_PATH/config
    echo 0x0F1B80B4 1 > $DCC_PATH/config
    echo 0x0F0880B4 1 > $DCC_PATH/config
    echo 0x0F0980B4 1 > $DCC_PATH/config
    echo 0x0F0A80B4 1 > $DCC_PATH/config
    echo 0x0F0B80B4 1 > $DCC_PATH/config
    #first core hung
    echo 0x0F1D1228 1 > $DCC_PATH/config
}

config_bengal_dcc_cam()
{
    echo 0x5C6F000 > $DCC_PATH/config
    echo 0x5C42000 > $DCC_PATH/config
    echo 0x5C42400 > $DCC_PATH/config
    echo 0x5C23000 > $DCC_PATH/config
}

# Function to send ASYNC package in TPDA
dcc_async_package()
{
    echo 0x08004FB0 0xc5acce55 > $DCC_PATH/config_write
    echo 0x0800408c 0xff > $DCC_PATH/config_write
    echo 0x08004FB0 0x0 > $DCC_PATH/config_write
}

# Function bengal DCC configuration
enable_bengal_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/1be2000.dcc_v2"
    soc_version=`cat /sys/devices/soc0/revision`
    soc_version=${soc_version/./}

    if [ ! -d $DCC_PATH ]; then
        echo "DCC does not exist on this build."
        return
    fi

    echo 0 > $DCC_PATH/enable
    echo 3 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo 1 > $DCC_PATH/config_reset
    config_bengal_dcc_core
    config_bengal_dcc_qdsp
    config_bengal_dcc_bimc
    config_bengal_dcc_gpu
    config_bengal_dcc_lpm
    config_bengal_dcc_noc
    config_bengal_dcc_misc
    config_bengal_dcc_osm
    config_acp_status
    config_modem_rscc
    config_cdsp_rscc
    config_bengal_dcc_cam

    #configure sink for LL3 as atb
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-dcc/enable_source
    echo 2 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo atb > $DCC_PATH/data_sink
    dcc_async_package
    config_bengal_dcc_gcc_mm
    config_bengal_dcc_gcc
    echo  1 > $DCC_PATH/enable
}

enable_bengal_smmu_hw_events()
{
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-center/reset
    #Set HW Event Register "center" startIndex 0x20 endIndex 0x20
    echo 0x20 0x20 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x20 endIndex 0x20 edgeDetect both
    echo 0x20 0x20 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set HW Event Register "center" startIndex 0x21 endIndex 0x21
    echo 0x21 0x21 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x21 endIndex 0x21 edgeDetect both
    echo 0x21 0x21 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set HW Event Register "center" startIndex 0x24 endIndex 0x24
    echo 0x24 0x24 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x24 endIndex 0x24 edgeDetect both
    echo 0x24 0x24 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set HW Event Register "center" startIndex 0x7c endIndex 0x7c
    echo 0x7c 0x7c 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x7c endIndex 0x7c edgeDetect both
    echo 0x7c 0x7c 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set HW Event Register "center" startIndex 0x7d endIndex 0x7d
    echo 0x7d 0x7d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x7d endIndex 0x7d edgeDetect both
    echo 0x7d 0x7d 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set HW Event Register "center" startIndex 0x7e endIndex 0x7e
    echo 0x7e 0x7e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x7e endIndex 0x7e edgeDetect both
    echo 0x7e 0x7e 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set HW Event Register "center" startIndex 0x7f endIndex 0x7f
    echo 0x7f 0x7f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl_mask
    #Set HW Event Edge Detection type "center" startIndex 0x7f endIndex 0x7f edgeDetect both
    echo 0x7f 0x7f 2 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_edge_ctrl
    #Set Mux Select Register "center" muxIndex 4 muxInput 0x00010011
    echo 4 0x00010011  > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_msr
    #Set Mux Select Register "center" muxIndex 15 muxInput 0x99990000
    echo 15 0x99990000  > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_msr
    #Set TPDM Type "center"
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_ts
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_type
    echo 0 > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_trig_ts
    echo 0 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 1 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 2 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 3 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 4 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 5 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 6 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask
    echo 7 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-center/dsb_patt_mask

    echo 2 > /sys/bus/coresight/devices/coresight-tpdm-center/enable_datasets
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-center/enable_source
}

enable_bengal_stm_hw_events()
{
    chmod 777 /vendor/bin/testapp_diag_senddata
    echo 33 0x0 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_msr
    echo 48 0x0 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_msr
    echo 0x0 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/mcmb_lanes_select
    echo 0 > /sys/bus/coresight/devices/coresight-tpdm-swao-0/cmb_trig_ts
    echo 0 >  /sys/bus/coresight/devices/coresight-tpdm-swao-0/enable_source
    echo 1 > /sys/bus/coresight/devices/coresight-cti-swao_cti0/reset
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss/reset
    echo 0x0 0x0 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x0 0x0 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2 0x2 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2 0x2 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x3 0x3 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x3 0x3 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x4 0x4 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x4 0x4 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x5 0x5 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x5 0x5 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x6 0x6 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x6 0x6 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x7 0x7 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x7 0x7 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x8 0x8 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x8 0x8 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x9 0x9 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x9 0x9 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0xa 0xa 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0xa 0xa 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0xb 0xb 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0xb 0xb 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0xc 0xc 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0xc 0xc 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0xd 0xd 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0xd 0xd 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0xe 0xe 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0xe 0xe 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0xf 0xf 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0xf 0xf 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x10 0x10 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x10 0x10 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x12 0x12 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x12 0x12 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x14 0x14 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x14 0x14 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x15 0x15 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x15 0x15 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x17 0x17 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x17 0x17 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x19 0x19 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x19 0x19 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x1a 0x1a 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x1a 0x1a 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x1b 0x1b 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x1b 0x1b 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x1d 0x1d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x1d 0x1d 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x1e 0x1e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x1e 0x1e 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x1f 0x1f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x1f 0x1f 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x20 0x20 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x20 0x20 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x21 0x21 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x21 0x21 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x22 0x22 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x22 0x22 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x23 0x23 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x23 0x23 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x24 0x24 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x24 0x24 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x25 0x25 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x25 0x25 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x26 0x26 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x26 0x26 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x27 0x27 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x27 0x27 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x28 0x28 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x28 0x28 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x29 0x29 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x29 0x29 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2a 0x2a 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2a 0x2a 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2b 0x2b 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2b 0x2b 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2c 0x2c 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2c 0x2c 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2d 0x2d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2d 0x2d 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2e 0x2e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2e 0x2e 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x2f 0x2f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x2f 0x2f 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x30 0x30 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x30 0x30 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x31 0x31 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x31 0x31 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x32 0x32 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x32 0x32 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x33 0x33 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x33 0x33 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x35 0x35 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x35 0x35 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x36 0x36 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x36 0x36 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x38 0x38 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x38 0x38 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x39 0x39 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x39 0x39 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x3a 0x3a 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x3a 0x3a 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x3b 0x3b 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x3b 0x3b 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0x3c 0x3c 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl_mask
    echo 0x3c 0x3c 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_edge_ctrl
    echo 0 0x10010000  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 1 0x01100100  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 2 0x70700000  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 3 0x10000070  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 4 0x01100100  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 5 0x11101010  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 6 0x00000040  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 7 0x00040000  > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_msr
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_ts
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_type
    echo 0 > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_trig_ts
    echo 0 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 1 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 2 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 3 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 4 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 5 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 6 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 7 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss/dsb_patt_mask
    echo 2 > /sys/bus/coresight/devices/coresight-tpdm-apss/enable_datasets
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss/enable_source

    enable_bengal_smmu_hw_events
}


enable_bengal_core_hang_config()
{
    CORE_PATH_SILVER="/sys/devices/system/cpu/hang_detect_silver"
    CORE_PATH_GOLD="/sys/devices/system/cpu/hang_detect_gold"
    if [ ! -d $CORE_PATH ]; then
        echo "CORE hang does not exist on this build."
        return
    fi

    #set the threshold to max
    echo 0xffffffff > $CORE_PATH_SILVER/threshold
    echo 0xffffffff > $CORE_PATH_GOLD/threshold

    #To enable core hang detection
    #It's a boolean variable. Do not use Hex value to enable/disable
    echo 1 > $CORE_PATH_SILVER/enable
    echo 1 > $CORE_PATH_GOLD/enable
}

enable_bengal_debug()
{
    echo "bengal debug"
    srcenable="enable_source"
    sinkenable="enable_sink"
    echo "Enabling STM events on bengal."
    enable_bengal_stm_events
    echo "Enabling HW  events on bengal."
    enable_bengal_stm_hw_events
    if [ "$ftrace_disable" != "Yes" ]; then
        enable_bengal_ftrace_event_tracing
    fi
    enable_bengal_dcc_config
    enable_bengal_core_hang_config
}
