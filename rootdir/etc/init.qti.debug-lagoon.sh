#!/vendor/bin/sh

#Copyright (c) 2020, The Linux Foundation. All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#    * Neither the name of The Linux Foundation nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
#WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
#ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
#BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
#BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
#OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
#IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

enable_tracing_events_lagoon()
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
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_cpu_hotplug/enable
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
    #rmph_send_msg
    echo 1 > /sys/kernel/debug/tracing/events/rpmh/rpmh_send_msg/enable

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

    #enable preemption and irq off traces 500 ms for preemption and 100 ms for irq off
    echo 500000000 > /proc/sys/kernel/preemptoff_tracing_threshold_ns
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_preempt_disable/enable
    echo 100000000 > /proc/sys/kernel/irqsoff_tracing_threshold_ns
    echo 1 > /sys/kernel/debug/tracing/events/preemptirq/irqs_disable/enable

    echo 1 > /sys/kernel/debug/tracing/tracing_on
}

# function to enable ftrace events
enable_ftrace_event_tracing_lagoon()
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

    enable_tracing_events_lagoon
}

# function to enable ftrace event transfer to CoreSight STM
enable_stm_events_lagoon()
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
    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/$sinkenable
    echo 1 > /sys/bus/coresight/devices/coresight-stm/$srcenable
    echo 1 > /sys/kernel/debug/tracing/tracing_on
    echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
    enable_tracing_events_lagoon
}

config_lagoon_dcc_ddr()
{
    #DDR - DCC starts here.
    #Start Link list #4
    #LLCC
    echo 0x9220344 2 > $DCC_PATH/config
    echo 0x9220480 > $DCC_PATH/config
    echo 0x9222398 > $DCC_PATH/config
    echo 0x92223A4 > $DCC_PATH/config
    echo 0x9222408 > $DCC_PATH/config
    echo 0x922358C > $DCC_PATH/config
    echo 0x923201C 5 > $DCC_PATH/config
    echo 0x9232050 > $DCC_PATH/config
    echo 0x9232100 > $DCC_PATH/config
    echo 0x9236028 5 > $DCC_PATH/config
    echo 0x9236040 6 > $DCC_PATH/config
    echo 0x9236060 > $DCC_PATH/config
    echo 0x92360b0 > $DCC_PATH/config
    echo 0x923e030 > $DCC_PATH/config
    echo 0x9241000 > $DCC_PATH/config
    echo 0x9248048 > $DCC_PATH/config
    echo 0x9248058 4 > $DCC_PATH/config


    #CABO
    echo 0x9260400 2 > $DCC_PATH/config
    echo 0x9260410 3 > $DCC_PATH/config
    echo 0x9260420 2 > $DCC_PATH/config
    echo 0x9260430 > $DCC_PATH/config
    echo 0x9260440 > $DCC_PATH/config
    echo 0x9260448 > $DCC_PATH/config
    echo 0x92604B0 > $DCC_PATH/config
    echo 0x92604B8 > $DCC_PATH/config
    echo 0x92604a0 > $DCC_PATH/config
    echo 0x9265804 > $DCC_PATH/config
    echo 0x9265840 > $DCC_PATH/config
    echo 0x9265B18 > $DCC_PATH/config
    echo 0x9266418 > $DCC_PATH/config
    echo 0x92E0400 2 > $DCC_PATH/config
    echo 0x92E04B0 > $DCC_PATH/config
    echo 0x92E04B8 > $DCC_PATH/config
    echo 0x92E04D0 2 > $DCC_PATH/config
    echo 0x92E5804 > $DCC_PATH/config
    echo 0x92E5B1C > $DCC_PATH/config
    echo 0x92E6418 > $DCC_PATH/config
    echo 0x92E6420 > $DCC_PATH/config
    echo 0x92e0410 3 > $DCC_PATH/config
    echo 0x92e0420 2 > $DCC_PATH/config
    echo 0x92e0430 > $DCC_PATH/config
    echo 0x92e0440 > $DCC_PATH/config
    echo 0x92e0448 > $DCC_PATH/config
    echo 0x92e04a0 > $DCC_PATH/config

    #LLCC Broadcast
    echo 0x9600000 2 > $DCC_PATH/config
    echo 0x9601000 2 > $DCC_PATH/config
    echo 0x9602000 2 > $DCC_PATH/config
    echo 0x9603000 2 > $DCC_PATH/config
    echo 0x9604000 2 > $DCC_PATH/config
    echo 0x9605000 2 > $DCC_PATH/config
    echo 0x9606000 2 > $DCC_PATH/config
    echo 0x9607000 2 > $DCC_PATH/config
    echo 0x9608000 2 > $DCC_PATH/config
    echo 0x9609000 2 > $DCC_PATH/config
    echo 0x960a000 2 > $DCC_PATH/config
    echo 0x960b000 2 > $DCC_PATH/config
    echo 0x960c000 2 > $DCC_PATH/config
    echo 0x960d000 2 > $DCC_PATH/config
    echo 0x960e000 2 > $DCC_PATH/config
    echo 0x960f000 2 > $DCC_PATH/config
    echo 0x9610000 2 > $DCC_PATH/config
    echo 0x9611000 2 > $DCC_PATH/config
    echo 0x9612000 2 > $DCC_PATH/config
    echo 0x9613000 2 > $DCC_PATH/config
    echo 0x9614000 2 > $DCC_PATH/config
    echo 0x9615000 2 > $DCC_PATH/config
    echo 0x9616000 2 > $DCC_PATH/config
    echo 0x9617000 2 > $DCC_PATH/config
    echo 0x9618000 2 > $DCC_PATH/config
    echo 0x9619000 2 > $DCC_PATH/config
    echo 0x961a000 2 > $DCC_PATH/config
    echo 0x961b000 2 > $DCC_PATH/config
    echo 0x961c000 2 > $DCC_PATH/config
    echo 0x961d000 2 > $DCC_PATH/config
    echo 0x961e000 2 > $DCC_PATH/config
    echo 0x961f000 2 > $DCC_PATH/config

    #SHRM
    echo 0x9050008 > $DCC_PATH/config
    echo 0x9050068 > $DCC_PATH/config
    echo 0x9050078 > $DCC_PATH/config

    #MCCC
    echo 0x90B0004 > $DCC_PATH/config
    echo 0x90C012C > $DCC_PATH/config
    echo 0x90C8040 > $DCC_PATH/config
    echo 0x90b0280 > $DCC_PATH/config
    echo 0x90b0288 7 > $DCC_PATH/config
    echo 0x9186048 > $DCC_PATH/config
    echo 0x9186054 > $DCC_PATH/config
    echo 0x9186078 > $DCC_PATH/config
    echo 0x9186164 > $DCC_PATH/config
    echo 0x9186170 > $DCC_PATH/config
    echo 0x9186264 > $DCC_PATH/config
    echo 0x9223318 > $DCC_PATH/config
    echo 0x9250110 > $DCC_PATH/config

   #End Link list #4
}

config_lagoon_dcc_cam()
{
    #Cam CC
    echo 0x10B008 > $DCC_PATH/config
    echo 0xAD0C1C4 > $DCC_PATH/config
    echo 0xAD0C12C > $DCC_PATH/config
    echo 0xAD0C130 > $DCC_PATH/config
    echo 0xAD0C144 > $DCC_PATH/config
    echo 0xAD0C148 > $DCC_PATH/config
}

config_lagoon_dcc_gemnoc()
{
    #GemNOC for lagoon start
    echo 0x9680000 3 > $DCC_PATH/config
    echo 8 > $DCC_PATH/loop
    echo 0x9681000 3 > $DCC_PATH/config
    echo 1 > $DCC_PATH/loop
    echo 0x09680078 > $DCC_PATH/config
    echo 0x9681008 12> $DCC_PATH/config
    echo 0xA6 > $DCC_PATH/loop
    echo 0x9681008 > $DCC_PATH/config
    echo 0x968100C > $DCC_PATH/config
    echo 1 > $DCC_PATH/loop
    echo 0x968103C > $DCC_PATH/config
    echo 0x9698100 3 > $DCC_PATH/config
    echo 0x9698110 > $DCC_PATH/config
    echo 0x9698120 8 > $DCC_PATH/config
    #GemNOC for lagoon end
}

config_lagoon_dcc_gpu()
{
    #GCC
    echo 0x12D038 > $DCC_PATH/config
    echo 0x145004 > $DCC_PATH/config
    echo 0x14500C > $DCC_PATH/config
    echo 0x1B502C > $DCC_PATH/config
    echo 0x1B602C > $DCC_PATH/config
    echo 0x1B702C > $DCC_PATH/config
    echo 0x1B802C > $DCC_PATH/config
    echo 0x1BD02C > $DCC_PATH/config

    #GPUCC
    echo 0x3D91004 > $DCC_PATH/config
    echo 0x3D9100C 2 > $DCC_PATH/config
    echo 0x3D91054 > $DCC_PATH/config
    echo 0x3D9106C 2 > $DCC_PATH/config
    echo 0x3D91078 > $DCC_PATH/config
    echo 0x3D91098 2 > $DCC_PATH/config
    echo 0x3D91540 > $DCC_PATH/config
    echo 0x3D92004 > $DCC_PATH/config
    echo 0x3D93004 > $DCC_PATH/config
    echo 0x3D95004 > $DCC_PATH/config
    echo 0x3D96004 > $DCC_PATH/config
    echo 0x3D97004 > $DCC_PATH/config
}

config_lagoon_dcc_lpm()
{
    #PCU Register
    echo 0x18000024 > $DCC_PATH/config
    echo 0x18000040 > $DCC_PATH/config
    echo 0x18010024 > $DCC_PATH/config
    echo 0x18010040 > $DCC_PATH/config
    echo 0x18020024 > $DCC_PATH/config
    echo 0x18020040 > $DCC_PATH/config
    echo 0x18030024 > $DCC_PATH/config
    echo 0x18030040 > $DCC_PATH/config
    echo 0x18040024 > $DCC_PATH/config
    echo 0x18040040 > $DCC_PATH/config
    echo 0x18050024 > $DCC_PATH/config
    echo 0x18050040 > $DCC_PATH/config
    echo 0x18060024 > $DCC_PATH/config
    echo 0x18060040 > $DCC_PATH/config
    echo 0x18070024 > $DCC_PATH/config
    echo 0x18070040 > $DCC_PATH/config
    echo 0x18080024 > $DCC_PATH/config
    echo 0x18080040 4 > $DCC_PATH/config
    echo 0x18080054 > $DCC_PATH/config
    echo 0x1808006c 5 > $DCC_PATH/config
    echo 0x180800f4 2 > $DCC_PATH/config
    echo 0x18080104 > $DCC_PATH/config
    echo 0x18080118 2 > $DCC_PATH/config
    echo 0x18080128 5 > $DCC_PATH/config
    echo 0x180801b4 3 > $DCC_PATH/config
    echo 0x180801f0 > $DCC_PATH/config
    echo 0x18280000 > $DCC_PATH/config
    echo 0x18282000 > $DCC_PATH/config
    echo 0x18284000 > $DCC_PATH/config

}

config_lagoon_dcc_osm()
{
    #APSS_OSM_RAIN0/RAIL1
    echo 0x18200040 > $DCC_PATH/config
    echo 0x1832102c > $DCC_PATH/config
    echo 0x18321044 > $DCC_PATH/config
    echo 0x18321700 > $DCC_PATH/config
    echo 0x18321710 > $DCC_PATH/config
    echo 0x1832176c > $DCC_PATH/config
    echo 0x18321818 2 > $DCC_PATH/config
    echo 0x18321824 > $DCC_PATH/config
    echo 0x18321920 > $DCC_PATH/config
    echo 0x18322C14 2 > $DCC_PATH/config
    echo 0x1832302c > $DCC_PATH/config
    echo 0x18323044 > $DCC_PATH/config
    echo 0x18323700 2 > $DCC_PATH/config
    echo 0x18323710 > $DCC_PATH/config
    echo 0x1832372C > $DCC_PATH/config
    echo 0x1832376c > $DCC_PATH/config
    echo 0x18323818 2 > $DCC_PATH/config
    echo 0x18323824 > $DCC_PATH/config
    echo 0x18323920 > $DCC_PATH/config
    echo 0x18324c18 > $DCC_PATH/config
    echo 0x1832582c > $DCC_PATH/config
    echo 0x18325844 > $DCC_PATH/config
    echo 0x18325F00 2 > $DCC_PATH/config
    echo 0x18325F2C > $DCC_PATH/config
    echo 0x18325f10 > $DCC_PATH/config
    echo 0x18325f6c > $DCC_PATH/config
    echo 0x18326018 2 > $DCC_PATH/config
    echo 0x18326024 > $DCC_PATH/config
    echo 0x18326120 > $DCC_PATH/config
    echo 0x18327414 2 > $DCC_PATH/config
    echo 0x18371034 > $DCC_PATH/config
    echo 0x1837103C > $DCC_PATH/config
    echo 0x18371810 2 > $DCC_PATH/config
    echo 0x18371820 > $DCC_PATH/config
    echo 0x18379034 > $DCC_PATH/config
    echo 0x1837903C > $DCC_PATH/config
    echo 0x18379810 2 > $DCC_PATH/config
    echo 0x1837981C 2 > $DCC_PATH/config

}

config_lagoon_dcc_core()
{
    # core hang
    echo 0x1800005C 1 > $DCC_PATH/config
    echo 0x1801005C 1 > $DCC_PATH/config
    echo 0x1802005C 1 > $DCC_PATH/config
    echo 0x1803005C 1 > $DCC_PATH/config
    echo 0x1804005C 1 > $DCC_PATH/config
    echo 0x1805005C 1 > $DCC_PATH/config
    echo 0x1806005C 1 > $DCC_PATH/config
    echo 0x1807005C 1 > $DCC_PATH/config
    echo 0x17C0003C 1 > $DCC_PATH/config

    #CPRh
    echo 0x18100908 > $DCC_PATH/config
    echo 0x18100C18 > $DCC_PATH/config
    echo 0x18101908 > $DCC_PATH/config
    echo 0x18101C18 > $DCC_PATH/config
    echo 0x18390810 3 > $DCC_PATH/config
    echo 0x18393500 > $DCC_PATH/config
    echo 0x18393A84 > $DCC_PATH/config
    echo 0x183A0810 3 > $DCC_PATH/config
    echo 0x183A3500 > $DCC_PATH/config
    echo 0x183A3A84 2 > $DCC_PATH/config

    #Silver / L3 / Gold PLL
    echo 0x18280000 4 > $DCC_PATH/config
    echo 0x18284000 4 > $DCC_PATH/config

    #Gold PLL
    echo 0x18280084 > $DCC_PATH/config
    echo 0x18282000 4 > $DCC_PATH/config
    echo 0x18282028 1 > $DCC_PATH/config
    echo 0x18282038 1 > $DCC_PATH/config
    echo 0x18282080 5 > $DCC_PATH/config

    #rpmh
    echo 0x0C201244 1 > $DCC_PATH/config
    echo 0x0C202244 1 > $DCC_PATH/config
    echo 0x18300000 1 > $DCC_PATH/config

    #GOLD
    echo 0x1829208C 1 > $DCC_PATH/config

}
config_lagoon_dcc_rsc_tcs()
{
    #APPS RSC TCS
    echo 0x18200d1c > $DCC_PATH/config
    echo 0x18200d34 3 > $DCC_PATH/config
    echo 0x18200d48 3 > $DCC_PATH/config
    echo 0x18220d00 2 > $DCC_PATH/config
    echo 0x18220d10 4 > $DCC_PATH/config
    echo 0x18220fb4 3 > $DCC_PATH/config
    echo 0x18221254 3 > $DCC_PATH/config
    echo 0x18221274 2 > $DCC_PATH/config
    echo 0x18221288 2 > $DCC_PATH/config
    echo 0x1822129c 2 > $DCC_PATH/config
    echo 0x182212b0 2 > $DCC_PATH/config
    echo 0x182212c4 2 > $DCC_PATH/config
    echo 0x182212d8 2 > $DCC_PATH/config
    echo 0x182212ec 2 > $DCC_PATH/config
    echo 0x18221300 2 > $DCC_PATH/config
    echo 0x18221314 2 > $DCC_PATH/config
    echo 0x18221328 2 > $DCC_PATH/config
    echo 0x1822133c 2 > $DCC_PATH/config
    echo 0x18221350 2 > $DCC_PATH/config
    echo 0x18221364 2 > $DCC_PATH/config
    echo 0x18221378 2 > $DCC_PATH/config
    echo 0x1822138c 2 > $DCC_PATH/config
    echo 0x182213a0 2 > $DCC_PATH/config
    echo 0x182214f4 3 > $DCC_PATH/config
    echo 0x18221514 2 > $DCC_PATH/config
    echo 0x18221528 2 > $DCC_PATH/config
    echo 0x1822153c 2 > $DCC_PATH/config
    echo 0x18221550 2 > $DCC_PATH/config
    echo 0x18221564 2 > $DCC_PATH/config
    echo 0x18221578 2 > $DCC_PATH/config
    echo 0x1822158c 2 > $DCC_PATH/config
    echo 0x182215a0 2 > $DCC_PATH/config
    echo 0x182215b4 2 > $DCC_PATH/config
    echo 0x182215c8 2 > $DCC_PATH/config
    echo 0x182215dc 2 > $DCC_PATH/config
    echo 0x182215f0 2 > $DCC_PATH/config
    echo 0x18221604 2 > $DCC_PATH/config
    echo 0x18221618 2 > $DCC_PATH/config
    echo 0x1822162c 2 > $DCC_PATH/config
    echo 0x18221640 2 > $DCC_PATH/config
    echo 0x18221794 3 > $DCC_PATH/config
    echo 0x182217b4 2 > $DCC_PATH/config
    echo 0x182217c8 2 > $DCC_PATH/config
    echo 0x182217dc 2 > $DCC_PATH/config
    echo 0x182217f0 2 > $DCC_PATH/config
    echo 0x18221804 2 > $DCC_PATH/config
    echo 0x18221818 2 > $DCC_PATH/config
    echo 0x1822182c 2 > $DCC_PATH/config
    echo 0x18221840 2 > $DCC_PATH/config
    echo 0x18221854 2 > $DCC_PATH/config
    echo 0x18221868 2 > $DCC_PATH/config
    echo 0x1822187c 2 > $DCC_PATH/config
    echo 0x18221890 2 > $DCC_PATH/config
    echo 0x182218a4 2 > $DCC_PATH/config
    echo 0x182218b8 2 > $DCC_PATH/config
    echo 0x182218cc 2 > $DCC_PATH/config
    echo 0x182218e0 2 > $DCC_PATH/config
    echo 0x18221a34 3 > $DCC_PATH/config
    echo 0x18221a54 2 > $DCC_PATH/config
    echo 0x18221a68 2 > $DCC_PATH/config
    echo 0x18221a7c 2 > $DCC_PATH/config
    echo 0x18221a90 2 > $DCC_PATH/config
    echo 0x18221aa4 2 > $DCC_PATH/config
    echo 0x18221ab8 2 > $DCC_PATH/config
    echo 0x18221acc 2 > $DCC_PATH/config
    echo 0x18221ae0 2 > $DCC_PATH/config
    echo 0x18221af4 2 > $DCC_PATH/config
    echo 0x18221b08 2 > $DCC_PATH/config
    echo 0x18221b1c 2 > $DCC_PATH/config
    echo 0x18221b30 2 > $DCC_PATH/config
    echo 0x18221b44 2 > $DCC_PATH/config
    echo 0x18221b58 2 > $DCC_PATH/config
    echo 0x18221b6c 2 > $DCC_PATH/config
    echo 0x18221b80 2 > $DCC_PATH/config
    echo 0x18221cd4 3 > $DCC_PATH/config
    echo 0x18221cf4 2 > $DCC_PATH/config
    echo 0x18221d08 2 > $DCC_PATH/config
    echo 0x18221d1c 2 > $DCC_PATH/config
    echo 0x18221d30 2 > $DCC_PATH/config
    echo 0x18221d44 2 > $DCC_PATH/config
    echo 0x18221d58 2 > $DCC_PATH/config
    echo 0x18221d6c 2 > $DCC_PATH/config
    echo 0x18221d80 2 > $DCC_PATH/config
    echo 0x18221d94 2 > $DCC_PATH/config
    echo 0x18221da8 2 > $DCC_PATH/config
    echo 0x18221dbc 2 > $DCC_PATH/config
    echo 0x18221dd0 2 > $DCC_PATH/config
    echo 0x18221de4 2 > $DCC_PATH/config
    echo 0x18221df8 2 > $DCC_PATH/config
    echo 0x18221e0c 2 > $DCC_PATH/config
    echo 0x18221e20 2 > $DCC_PATH/config
    echo 0x18221f74 3 > $DCC_PATH/config
    echo 0x18221f94 2 > $DCC_PATH/config
    echo 0x18221fa8 2 > $DCC_PATH/config
    echo 0x18221fbc 2 > $DCC_PATH/config
    echo 0x18221fd0 2 > $DCC_PATH/config
    echo 0x18221fe4 2 > $DCC_PATH/config
    echo 0x18221ff8 2 > $DCC_PATH/config
    echo 0x1822200c 2 > $DCC_PATH/config
    echo 0x18222020 2 > $DCC_PATH/config
    echo 0x18222034 2 > $DCC_PATH/config
    echo 0x18222048 2 > $DCC_PATH/config
    echo 0x1822205c 2 > $DCC_PATH/config
    echo 0x18222070 2 > $DCC_PATH/config
    echo 0x18222084 2 > $DCC_PATH/config
    echo 0x18222098 2 > $DCC_PATH/config
    echo 0x182220ac 2 > $DCC_PATH/config
    echo 0x182220c0 2 > $DCC_PATH/config

    #NPU RSC
    echo 0x9802028 > $DCC_PATH/config
    echo 0x98B0010 3 > $DCC_PATH/config
    echo 0x98B0208 3 > $DCC_PATH/config
    echo 0x98B0228 3 > $DCC_PATH/config
    echo 0x98B0248 3 > $DCC_PATH/config
    echo 0x98B0268 3 > $DCC_PATH/config
    echo 0x98B0288 3 > $DCC_PATH/config
    echo 0x98B02A8 3 > $DCC_PATH/config
    echo 0x98B0400 3 > $DCC_PATH/config

    #CDSP RSCp
    echo 0x80A4010 3 > $DCC_PATH/config
    echo 0x80A4030 > $DCC_PATH/config
    echo 0x80A4038 > $DCC_PATH/config
    echo 0x80A4040 > $DCC_PATH/config
    echo 0x80A4048 > $DCC_PATH/config
    echo 0x80A40D0 > $DCC_PATH/config
    echo 0x80A4208 3 > $DCC_PATH/config
    echo 0x80A4228 3 > $DCC_PATH/config
    echo 0x80A4248 3 > $DCC_PATH/config
    echo 0x80A4268 3 > $DCC_PATH/config
    echo 0x80A4288 3 > $DCC_PATH/config
    echo 0x80A42A8 3 > $DCC_PATH/config
    echo 0x80A4400 3 > $DCC_PATH/config
    echo 0x80A4D04 > $DCC_PATH/config

    #QDSP6 RSC

    echo 0x83B0010 3 > $DCC_PATH/config
    echo 0x83B0208 3 > $DCC_PATH/config
    echo 0x83B0228 3 > $DCC_PATH/config
    echo 0x83B0248 3 > $DCC_PATH/config
    echo 0x83B0268 3 > $DCC_PATH/config
    echo 0x83B0288 3 > $DCC_PATH/config
    echo 0x83B02A8 3 > $DCC_PATH/config
    echo 0x83B0400 3 > $DCC_PATH/config

    #CDSP PDC
    echo 0xb2b0010 > $DCC_PATH/config
    echo 0xb2b0900 > $DCC_PATH/config
    echo 0xb2b1020 2 > $DCC_PATH/config
    echo 0xb2b1030 > $DCC_PATH/config
    echo 0xb2b103c > $DCC_PATH/config
    echo 0xb2b1200 3 > $DCC_PATH/config
    echo 0xb2b4510 2 > $DCC_PATH/config
    echo 0xb2b4520 > $DCC_PATH/config

    #QDSP6 General
    echo 0x8300468 > $DCC_PATH/config
    echo 0x8302000 > $DCC_PATH/config
    echo 0x8390380 32 > $DCC_PATH/config
}

config_lagoon_dcc_lpass_rsc(){
    #Audio PDC
    echo 0xb250010 > $DCC_PATH/config
    echo 0xb250900 > $DCC_PATH/config
    echo 0xb251020 > $DCC_PATH/config
    echo 0xb251024 > $DCC_PATH/config
    echo 0xb251030 > $DCC_PATH/config
    echo 0xb25103c > $DCC_PATH/config
    echo 0xb251200 > $DCC_PATH/config
    echo 0xb251204 > $DCC_PATH/config
    echo 0xb251208 > $DCC_PATH/config
    echo 0xb254510 > $DCC_PATH/config
    echo 0xb254514 > $DCC_PATH/config
    echo 0xb254520 > $DCC_PATH/config

    #LPASS General
    echo 0x3000468 > $DCC_PATH/config
    echo 0x3002000 > $DCC_PATH/config
    echo 0x3002004 > $DCC_PATH/config
    echo 0x3090380 32 > $DCC_PATH/config
    echo 0x62402028  > $DCC_PATH/config

    #WDOG
    echo 0x8384004 5 > $DCC_PATH/config
    echo 0x08300304 > $DCC_PATH/config

    echo 0xC2A2040 > $DCC_PATH/config
    #LPASS RSC
    echo 0x3500010 3 > $DCC_PATH/config
    echo 0x3500030 > $DCC_PATH/config
    echo 0x3500038 > $DCC_PATH/config
    echo 0x3500040 > $DCC_PATH/config
    echo 0x3500048 > $DCC_PATH/config
    echo 0x35000d0 > $DCC_PATH/config
    echo 0x3500208 3 > $DCC_PATH/config
    echo 0x3500228 3 > $DCC_PATH/config
    echo 0x3500248 3 > $DCC_PATH/config
    echo 0x3500268 3 > $DCC_PATH/config
    echo 0x3500288 3 > $DCC_PATH/config
    echo 0x35002a8 3 > $DCC_PATH/config
    echo 0x3500400 3 > $DCC_PATH/config
    echo 0x3500d04 > $DCC_PATH/config

    #LPASS RSCc
    echo 0x30b0010 3 > $DCC_PATH/config
    echo 0x30b0208 3 > $DCC_PATH/config
    echo 0x30b0228 3 > $DCC_PATH/config
    echo 0x30b0248 3 > $DCC_PATH/config
    echo 0x30b0268 3 > $DCC_PATH/config
    echo 0x30b0288 3 > $DCC_PATH/config
    echo 0x30b02a8 3 > $DCC_PATH/config
    echo 0x30b0400 3 > $DCC_PATH/config

    #Core status and NMI for modem / Lpass / Turing
    echo 0x8300044 > $DCC_PATH/config
    echo 0x8302028 > $DCC_PATH/config
    echo 0x3002028 > $DCC_PATH/config
    echo 0x3000044 > $DCC_PATH/config
    echo 0x4082028 > $DCC_PATH/config
    echo 0x4080044 > $DCC_PATH/config
}

config_lagoon_dcc_mss_rsc(){
    #MSS RSCp
    echo 0x4200010 3 > $DCC_PATH/config
    echo 0x4200030 > $DCC_PATH/config
    echo 0x4200038 > $DCC_PATH/config
    echo 0x4200040 > $DCC_PATH/config
    echo 0x4200048 > $DCC_PATH/config
    echo 0x42000D0 > $DCC_PATH/config
    echo 0x4200208 3 > $DCC_PATH/config
    echo 0x4200228 3 > $DCC_PATH/config
    echo 0x4200248 3 > $DCC_PATH/config
    echo 0x4200268 3 > $DCC_PATH/config
    echo 0x4200288 3 > $DCC_PATH/config
    echo 0x42002A8 3 > $DCC_PATH/config
    echo 0x4200400 3 > $DCC_PATH/config
    echo 0x4200D04 > $DCC_PATH/config

    #MSS RSCc
    echo 0x4130010 3 > $DCC_PATH/config
    echo 0x4130208 3 > $DCC_PATH/config
    echo 0x4130228 3 > $DCC_PATH/config
    echo 0x4130248 3 > $DCC_PATH/config
    echo 0x4130268 3 > $DCC_PATH/config
    echo 0x4130288 3 > $DCC_PATH/config
    echo 0x41302A8 3 > $DCC_PATH/config
    echo 0x4130400 3 > $DCC_PATH/config

    #MSS PDC
    echo 0x18A008 > $DCC_PATH/config
    echo 0xb2c0010 2 > $DCC_PATH/config
    echo 0xb2c0900 2 > $DCC_PATH/config
    echo 0xb2c1020 2 > $DCC_PATH/config
    echo 0xb2c1030 > $DCC_PATH/config
    echo 0xb2c1200 3 > $DCC_PATH/config
    echo 0xb2c4510 2 > $DCC_PATH/config
    echo 0xb2c4520 > $DCC_PATH/config
    # MSS PDC new additions
    echo 0xb2c1214 3 > $DCC_PATH/config
    echo 0xb2c1228 3 > $DCC_PATH/config
    echo 0xb2c123C 3 > $DCC_PATH/config
    echo 0xb2c1250 3 > $DCC_PATH/config

    # GCC_MSS_AXIS2_CBCR
    echo 0x18A004 > $DCC_PATH/config

    #MSS_QDSP6SS_NMI_STATUS
    echo 0x04080044 > $DCC_PATH/config

    #MSS_QDSP6SS_DBG_NMI_PWR_STATUS
    echo  0x04080304 > $DCC_PATH/config

    #MSS_QDSP6SS_CORE_STATUS
    echo  0x04082028 > $DCC_PATH/config
}

config_lagoon_dcc_noc(){

    # Enable clock for SNOC Sensein register
    echo 0x11100C 0x1 > DCC_PATH/config_write
    echo 0x110144 0x1 > DCC_PATH/config_write
    echo 0x11102C 0x1 > DCC_PATH/config_write

    #A1NOC
    echo 0x16E0400 > $DCC_PATH/config
    echo 0x16e0300 > $DCC_PATH/config
    echo 0x16e0408 > $DCC_PATH/config
    echo 0x16e0410 > $DCC_PATH/config
    echo 0x16e0420 8 > $DCC_PATH/config
    echo 0x16e0688 > $DCC_PATH/config
    echo 0x16e0690 > $DCC_PATH/config
    echo 0x16e0700 > $DCC_PATH/config

    #A2NOC
    echo 0x1700204 > $DCC_PATH/config
    echo 0x1700240 > $DCC_PATH/config
    echo 0x1700248 > $DCC_PATH/config
    echo 0x1700288 > $DCC_PATH/config
    echo 0x1700290 > $DCC_PATH/config
    echo 0x1700300 5 > $DCC_PATH/config
    echo 0x1700400 2 > $DCC_PATH/config
    echo 0x1700488 > $DCC_PATH/config
    echo 0x1700490 > $DCC_PATH/config
    echo 0x1700500 4 > $DCC_PATH/config
    echo 0x1700c00 3 > $DCC_PATH/config
    echo 0x1700c10 > $DCC_PATH/config
    echo 0x1700c20 8 > $DCC_PATH/config

    #SNOC
    echo 0x1620000 3 > $DCC_PATH/config
    echo 0x1620010 > $DCC_PATH/config
    echo 0x1620020 8 > $DCC_PATH/config
    echo 0x1620200 2 > $DCC_PATH/config
    echo 0x1620240 > $DCC_PATH/config
    echo 0x1620248 > $DCC_PATH/config
    echo 0x1620288 4 > $DCC_PATH/config
    echo 0x16202a8 4 > $DCC_PATH/config
    echo 0x1620300 > $DCC_PATH/config
    echo 0x1620400 2 > $DCC_PATH/config
    echo 0x1620488 > $DCC_PATH/config
    echo 0x1620490 > $DCC_PATH/config
    echo 0x1620500 5 > $DCC_PATH/config
    echo 0x1620600 2 > $DCC_PATH/config
    echo 0x1620688 > $DCC_PATH/config
    echo 0x1620690 > $DCC_PATH/config
    echo 0x1620700 5 > $DCC_PATH/config
    echo 0x1620800 2 > $DCC_PATH/config
    echo 0x1620900 > $DCC_PATH/config
    echo 0x1620a00 2 > $DCC_PATH/config
    echo 0x1620b00 2 > $DCC_PATH/config
    echo 0x1639000 2 > $DCC_PATH/config

    #GEMNOC
    echo 0x1B9064 > $DCC_PATH/config
    echo 0x1B906C > $DCC_PATH/config
    echo 0x9681010 10 > $DCC_PATH/config
    echo 0x968103c > $DCC_PATH/config
    echo 0x9692000 3 > $DCC_PATH/config
    echo 0x9692040 > $DCC_PATH/config
    echo 0x9692048 > $DCC_PATH/config
    echo 0x9695000 2 > $DCC_PATH/config
    echo 0x9695080 6 > $DCC_PATH/config
    echo 0x96950a0 > $DCC_PATH/config
    echo 0x96950a8 > $DCC_PATH/config
    echo 0x96950b0 > $DCC_PATH/config
    echo 0x9695100 8 > $DCC_PATH/config
    echo 0x9696000 2 > $DCC_PATH/config
    echo 0x9696080 > $DCC_PATH/config
    echo 0x9696088 > $DCC_PATH/config
    echo 0x9696090 > $DCC_PATH/config
    echo 0x9696100 4 > $DCC_PATH/config
    echo 0x9696114 3 > $DCC_PATH/config
    echo 0x9698000 3 > $DCC_PATH/config
    echo 0x9698010 > $DCC_PATH/config
    echo 0x9698100 3 > $DCC_PATH/config
    echo 0x9698110 > $DCC_PATH/config
    echo 0x9698118 > $DCC_PATH/config
    echo 0x9698120 8 > $DCC_PATH/config
    echo 0x9698200 2 > $DCC_PATH/config
    echo 0x9698240 4 > $DCC_PATH/config

}

config_lagoon_dcc_gcc(){
    #GCC
    echo 0x100000 13 > $DCC_PATH/config
    echo 0x100040 > $DCC_PATH/config

    echo 0x101000 13 > $DCC_PATH/config
    echo 0x10103c > $DCC_PATH/config
    echo 0x101040 > $DCC_PATH/config
    echo 0x102000 13 > $DCC_PATH/config
    echo 0x10203c > $DCC_PATH/config
    echo 0x102040 > $DCC_PATH/config
    echo 0x103000 13 > $DCC_PATH/config
    echo 0x10303c > $DCC_PATH/config
    echo 0x103040 > $DCC_PATH/config
    echo 0x10401c > $DCC_PATH/config
    echo 0x105008 > $DCC_PATH/config
    echo 0x10504c > $DCC_PATH/config
    echo 0x113000 > $DCC_PATH/config
    echo 0x113004 > $DCC_PATH/config
    echo 0x113008 > $DCC_PATH/config

    # CPR global Rev register
    echo 0x7802E0 > $DCC_PATH/config


}

config_lagoon_dcc_l3_rsc(){
}

config_lagoon_dcc_pimem()
{
    echo 0x610100 11 > $DCC_PATH/config
}

config_lagoon_dcc_misc()
{
}

config_lagoon_dcc_gic()
{
    echo 0x17A00104 29 > $DCC_PATH/config
    echo 0x17A00204 29 > $DCC_PATH/config
}

config_lagoon_dcc_apps_rsc_pdc()
{
    #APPS RSC
    echo 0x18200010 > $DCC_PATH/config
    echo 0x18200030 > $DCC_PATH/config
    echo 0x18200038 > $DCC_PATH/config
    echo 0x18200048 > $DCC_PATH/config
    echo 0x18200400 3 > $DCC_PATH/config
    echo 0x18220010 > $DCC_PATH/config
    echo 0x18220030 > $DCC_PATH/config
    echo 0x18220038 > $DCC_PATH/config
    echo 0x18220040 > $DCC_PATH/config
    echo 0x182200D0 > $DCC_PATH/config

    #RPMH PDC
    echo 0xb201020 2 > $DCC_PATH/config
    echo 0xb20103c > $DCC_PATH/config
    echo 0xb204510 2 > $DCC_PATH/config
    echo 0xb204520 > $DCC_PATH/config

}

# Function to send ASYNC package in TPDA
lagoon_dcc_async_package()
{
    echo 0x06004FB0 0xc5acce55 > $DCC_PATH/config_write
    echo 0x0600408c 0xff > $DCC_PATH/config_write
    echo 0x06004FB0 0x0 > $DCC_PATH/config_write
}

# Function lagoon DCC configuration
enable_lagoon_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/109f000.dcc_v2"
    soc_version=`cat /sys/devices/soc0/revision`
    soc_version=${soc_version/./}

    if [ ! -d $DCC_PATH ]; then
        echo "DCC does not exist on this build."
        return
    fi

    #DCC will trigger in following order LL4 -> LL3
    echo 0 > $DCC_PATH/enable
    echo 1 > $DCC_PATH/config_reset
    echo 4 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    config_lagoon_dcc_lpm
    config_lagoon_dcc_apps_rsc_pdc
    config_lagoon_dcc_core
    config_lagoon_dcc_gpu
    config_lagoon_dcc_osm
    config_lagoon_dcc_gemnoc
    config_lagoon_dcc_noc

    config_lagoon_dcc_pimem

    config_lagoon_dcc_ddr
    config_lagoon_dcc_ddr

    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-dcc/enable_source
    echo 3 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo atb > $DCC_PATH/data_sink
    lagoon_dcc_async_package
    config_lagoon_dcc_rsc_tcs
    config_lagoon_dcc_lpass_rsc
    config_lagoon_dcc_mss_rsc
    #config_lagoon_dcc_gcc
    #config_lagoon_dcc_l3_rsc
    #config_lagoon_dcc_gcc_other
    #config_lagoon_dcc_misc
    config_lagoon_dcc_gic

    echo  1 > $DCC_PATH/enable
}

enable_lagoon_pcu_pll_hw_events()
{
}

enable_lagoon_stm_hw_events()
{
   QMI_HELPER=/system/vendor/bin/qdss_qmi_helper
   #enable_lagoon_pcu_pll_hw_events
}

enable_lagoon_core_hang_config()
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


ftrace_disable=`getprop persist.debug.ftrace_events_disable`
srcenable="enable"
sinkenable="curr_sink"
enable_lagoon_debug()
{
    echo "lagoon debug"
    srcenable="enable_source"
    sinkenable="enable_sink"
    echo "Enabling STM events on lagoon."
    enable_stm_events_lagoon
    if [ "$ftrace_disable" != "Yes" ]; then
        enable_ftrace_event_tracing_lagoon
    fi
    enable_lagoon_dcc_config
    enable_lagoon_core_hang_config
    enable_lagoon_stm_hw_events
}
