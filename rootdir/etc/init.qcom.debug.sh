#!/system/bin/sh
# Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.
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

# function to enable ftrace events to CoreSight STM
enable_stm_events()
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

    echo 0x2000000 > /sys/bus/coresight/devices/coresight-tmc-etr/mem_size
    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/curr_sink
    echo 1 > /sys/bus/coresight/devices/coresight-stm/enable
    echo 1 > /sys/kernel/debug/tracing/tracing_on
    echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
    # timer
    echo 1 > /sys/kernel/debug/tracing/events/timer/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/filter
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_cancel/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_expire_entry/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_expire_exit/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_init/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/timer_start/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/tick_stop/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_cancel/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_expire_entry/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_expire_exit/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_init/enable
    echo 1 > /sys/kernel/debug/tracing/events/timer/hrtimer_start/enable
    #enble FTRACE for softirq events
    echo 1 > /sys/kernel/debug/tracing/events/irq/enable
    echo 1 > /sys/kernel/debug/tracing/events/irq/filter
    echo 1 > /sys/kernel/debug/tracing/events/irq/softirq_entry/enable
    echo 1 > /sys/kernel/debug/tracing/events/irq/softirq_exit/enable
    echo 1 > /sys/kernel/debug/tracing/events/irq/softirq_raise/enable
    #enble FTRACE for Workqueue events
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/enable
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/filter
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/workqueue_activate_work/enable
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/workqueue_execute_end/enable
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/workqueue_execute_start/enable
    echo 1 > /sys/kernel/debug/tracing/events/workqueue/workqueue_queue_work/enable
    # schedular
    echo 1 > /sys/kernel/debug/tracing/events/sched/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/filter
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_cpu_hotplug/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_cpu_load/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_enq_deq_task/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_kthread_stop_ret/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_kthread_stop/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_load_balance/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_migrate_task/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_pi_setprio/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_exec/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_exit/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_fork/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_free/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_wait/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_stat_blocked/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_stat_iowait/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_stat_runtime/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_stat_sleep/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_stat_wait/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_switch/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_task_load/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_update_history/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_update_task_ravg/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wait_task/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wakeup/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_wakeup_new/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_get_busy/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_get_nr_running_avg/enable
    echo 1 > /sys/kernel/debug/tracing/events/sched/sched_reset_all_window_stats/enable
    # sound
    echo 1 > /sys/kernel/debug/tracing/events/asoc/snd_soc_reg_read/enable
    echo 1 > /sys/kernel/debug/tracing/events/asoc/snd_soc_reg_write/enable
    # mdp
    echo 1 > /sys/kernel/debug/tracing/events/mdss/mdp_video_underrun_done/enable
    # video
    echo 1 > /sys/kernel/debug/tracing/events/msm_vidc/enable
    # clock
    echo 1 > /sys/kernel/debug/tracing/events/power/clock_set_rate/enable
    # regulator
    echo 1 > /sys/kernel/debug/tracing/events/regulator/enable
    # power
    echo 1 > /sys/kernel/debug/tracing/events/msm_low_power/enable
    #thermal
    echo 1 > /sys/kernel/debug/tracing/events/thermal/thermal_pre_core_offline/enable
    echo 1 > /sys/kernel/debug/tracing/events/thermal/thermal_post_core_offline/enable
    echo 1 > /sys/kernel/debug/tracing/events/thermal/thermal_pre_core_online/enable
    echo 1 > /sys/kernel/debug/tracing/events/thermal/thermal_post_core_online/enable
    echo 1 > /sys/kernel/debug/tracing/events/thermal/thermal_pre_frequency_mit/enable
    echo 1 > /sys/kernel/debug/tracing/events/thermal/thermal_post_frequency_mit/enable
}

# Function MSMCOBALT DCC configuration
enable_msm8998_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/10b3000.dcc"
    if [ ! -d $DCC_PATH ]; then
        echo "DCC don't exist on this build."
        return
    fi

    echo  0 > $DCC_PATH/enable
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo  1 > $DCC_PATH/config_reset

    #OSM WDOG
    echo 0x179C1C00 37 > $DCC_PATH/config
    echo 0x179C3C00 37 > $DCC_PATH/config
    #APM
    echo 0x179D0000 1 > $DCC_PATH/config
    echo 0x179D000C 1 > $DCC_PATH/config
    echo 0x179D0018 1 > $DCC_PATH/config
    #L2_SAW4_PMIC_STS
    echo 0x17912C18 1 > $DCC_PATH/config
    echo 0x17812C18 1 > $DCC_PATH/config
    #CPRH_STATUS
    echo 0x179CBAA4 1 > $DCC_PATH/config
    echo 0x179C7AA4 1 > $DCC_PATH/config

    # default configuration
    #SPM registers
    echo 0x17989000 > $DCC_PATH/config
    echo 0x17989C0C > $DCC_PATH/config
    echo 0x17988064 > $DCC_PATH/config
    echo 0x17999000 > $DCC_PATH/config
    echo 0x17999C0C > $DCC_PATH/config
    echo 0x17998064 > $DCC_PATH/config
    echo 0x179A9000 > $DCC_PATH/config
    echo 0x179A9C0C > $DCC_PATH/config
    echo 0x179A8064 > $DCC_PATH/config
    echo 0x179B9000 > $DCC_PATH/config
    echo 0x179B9C0C > $DCC_PATH/config
    echo 0x179B8064 > $DCC_PATH/config
    echo 0x17912000 > $DCC_PATH/config
    echo 0x17912C0C > $DCC_PATH/config
    echo 0x17911210 > $DCC_PATH/config
    echo 0x17911290 > $DCC_PATH/config
    echo 0x17911218 > $DCC_PATH/config
    echo 0x17889000 > $DCC_PATH/config
    echo 0x17889C0C > $DCC_PATH/config
    echo 0x17888064 > $DCC_PATH/config
    echo 0x17899000 > $DCC_PATH/config
    echo 0x17899C0C > $DCC_PATH/config
    echo 0x17898064 > $DCC_PATH/config
    echo 0x178A9000 > $DCC_PATH/config
    echo 0x178A9C0C > $DCC_PATH/config
    echo 0x178A8064 > $DCC_PATH/config
    echo 0x178B9000 > $DCC_PATH/config
    echo 0x178B9C0C > $DCC_PATH/config
    echo 0x178B8064 > $DCC_PATH/config
    echo 0x17812000 > $DCC_PATH/config
    echo 0x17812C0C > $DCC_PATH/config
    echo 0x17811210 > $DCC_PATH/config
    echo 0x17811290 > $DCC_PATH/config
    echo 0x17811218 > $DCC_PATH/config
    echo 0x179D2000 > $DCC_PATH/config
    echo 0x179D2C0C > $DCC_PATH/config
    echo 0x17904008 > $DCC_PATH/config
    echo 0x1790400C > $DCC_PATH/config
    echo 0x17904010 > $DCC_PATH/config
    echo 0x17904014 > $DCC_PATH/config

    echo  1 > $DCC_PATH/enable
}

# Function MSM8996 DCC configuration
enable_msm8996_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/4b3000.dcc"
    if [ ! -d $DCC_PATH ]; then
        echo "DCC don't exist on this build."
        return
    fi

    echo  0 > $DCC_PATH/enable
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo  1 > $DCC_PATH/config_reset

    #SPM Registers
    # CPU0
    echo  0x998000C > $DCC_PATH/config
    echo  0x9980030 > $DCC_PATH/config
    echo  0x998003C > $DCC_PATH/config
    # CPU1
    echo  0x999000C > $DCC_PATH/config
    echo  0x9990030 > $DCC_PATH/config
    echo  0x999003C > $DCC_PATH/config
    # CPU2
    echo  0x99B000C > $DCC_PATH/config
    echo  0x99B0030 > $DCC_PATH/config
    echo  0x99B003C > $DCC_PATH/config
    # CPU3
    echo  0x99C000C > $DCC_PATH/config
    echo  0x99C0030 > $DCC_PATH/config
    echo  0x99C003C > $DCC_PATH/config
    # PWRL2
    echo  0x99A000C > $DCC_PATH/config
    echo  0x99A0030 > $DCC_PATH/config
    echo  0x99A003C > $DCC_PATH/config
    # PERFL2
    echo  0x99D000C > $DCC_PATH/config
    echo  0x99D0030 > $DCC_PATH/config
    echo  0x99D003C > $DCC_PATH/config
    # L3
    echo  0x9A0000C > $DCC_PATH/config
    echo  0x9A00030 > $DCC_PATH/config
    echo  0x9A0003C > $DCC_PATH/config
    # CBF
    echo  0x9A1000C > $DCC_PATH/config
    echo  0x9A10030 > $DCC_PATH/config
    echo  0x9A1003C > $DCC_PATH/config
    # PWR L2 HW-FLUSH
    echo  0x99A1060 > $DCC_PATH/config
    # PERF L2 HW-FLUSH
    echo  0x99D1060 > $DCC_PATH/config
    # APCS_APC0_SLEEP_EN_VOTE
    echo  0x99A2030 > $DCC_PATH/config
    # APCS_APCC_SW_EN_VOTE
    echo  0x99E0020 > $DCC_PATH/config
    # pIMEM
    echo  0x0038070 > $DCC_PATH/config
    echo  0x0038074 > $DCC_PATH/config
    echo  0x0038078 > $DCC_PATH/config
    echo  0x003807C > $DCC_PATH/config
    echo  0x0038080 > $DCC_PATH/config
    echo  0x0038084 > $DCC_PATH/config
    echo  0x0038088 > $DCC_PATH/config
    echo  0x003808C > $DCC_PATH/config

    echo  1 > $DCC_PATH/enable
}

# Function MSM8953 DCC configuration
enable_msm8953_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/b3000.dcc"
    if [ ! -d $DCC_PATH ]; then
        echo "DCC don't exist on this build."
        return
    fi

    echo  0 > $DCC_PATH/enable
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink

    #GCC_GPLL0
    echo 0x1821000 0x9 > $DCC_PATH/config

    #GCC_GPLL2
    echo 0x184A000 0x1 > $DCC_PATH/config
    echo 0x184A010 0x5 > $DCC_PATH/config

    #GCC_GPLL4
    echo 0x1824000 0x1 > $DCC_PATH/config
    echo 0x1824010 0x5 > $DCC_PATH/config

    #GCC_GPLL5
    echo 0x1825000 0x2 > $DCC_PATH/config
    echo 0x1825010 0x1 > $DCC_PATH/config
    echo 0x1825018 0x2 > $DCC_PATH/config
    echo 0x1825024 0x1 > $DCC_PATH/config

    #GCC_BIMC
    echo 0x1823000 0x2 > $DCC_PATH/config
    echo 0x1823010 0x1 > $DCC_PATH/config
    echo 0x1823018 0x2 > $DCC_PATH/config
    echo 0x1823024 0x1 > $DCC_PATH/config

    #GCC_GPLL6
    echo 0x1837000 0x1 > $DCC_PATH/config
    echo 0x1837010 0x1 > $DCC_PATH/config
    echo 0x1837018 0x2 > $DCC_PATH/config
    echo 0x1837024 0x1 > $DCC_PATH/config

    #GCC_SYSTEM_NOC
    echo 0x1826004 0x2 > $DCC_PATH/config

    #GCC_PCNOC
    echo 0x1827000 0x2 > $DCC_PATH/config

    #GCC_SYSTEM_MMNOC
    echo 0x183D000 0x2 > $DCC_PATH/config

    #GCC_DDR
    echo 0x1832024 0x2 > $DCC_PATH/config

    #GCC_BIMC
    echo 0x1831018 0x1 > $DCC_PATH/config
    echo 0x1831004 0x1 > $DCC_PATH/config
    echo 0x1831024 0x3 > $DCC_PATH/config

    #GCC_Q6
    echo 0x1838030 0x2 > $DCC_PATH/config

    #GCC_APSS_TCU
    echo 0x1838000 0x2 > $DCC_PATH/config

    #GCC_APSS_AXI
    echo 0x1838048 0x2 > $DCC_PATH/config

    # OCIMEM START  #MARK the start
    echo 0x00448560 1 > $DCC_PATH/config
    echo 0x004485A0 1 > $DCC_PATH/config
    echo 0x00448520 1 > $DCC_PATH/config
    echo 0x00448450 1 > $DCC_PATH/config
    echo 0x0044C08C 1 > $DCC_PATH/config
    echo 0x0044C09C 1 > $DCC_PATH/config
    echo 0x00408420 1 > $DCC_PATH/config
    echo 0x00408424 1 > $DCC_PATH/config
    echo 0x00408430 1 > $DCC_PATH/config
    echo 0x00408434 1 > $DCC_PATH/config
    echo 0x0041c100 1 > $DCC_PATH/config
    echo 0x0041c420 1 > $DCC_PATH/config
    echo 0x0041c424 1 > $DCC_PATH/config
    echo 0x0041c430 1 > $DCC_PATH/config
    echo 0x0041c434 1 > $DCC_PATH/config

    echo 0x0040C420 1 > $DCC_PATH/config
    echo 0x0040C424 1 > $DCC_PATH/config
    echo 0x0040C430 1 > $DCC_PATH/config
    echo 0x0040C434 1 > $DCC_PATH/config

    echo 0x00414100 1 > $DCC_PATH/config
    echo 0x00414420 1 > $DCC_PATH/config
    echo 0x00414424 1 > $DCC_PATH/config
    echo 0x00414430 1 > $DCC_PATH/config
    echo 0x00414434 1 > $DCC_PATH/config
    echo 0x00418420 1 > $DCC_PATH/config
    echo 0x00418424 1 > $DCC_PATH/config
    echo 0x00418430 1 > $DCC_PATH/config
    echo 0x00418434 1 > $DCC_PATH/config

    echo 0x00410420 1 > $DCC_PATH/config
    echo 0x00410424 1 > $DCC_PATH/config
    echo 0x00410430 1 > $DCC_PATH/config
    echo 0x00410434 1 > $DCC_PATH/config

    echo 0x00420420 1 > $DCC_PATH/config
    echo 0x00420424 1 > $DCC_PATH/config
    echo 0x00420430 1 > $DCC_PATH/config
    echo 0x00420434 1 > $DCC_PATH/config

    echo  1 > $DCC_PATH/enable
}

# Function MSM8976 DCC configuration
enable_msm8976_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/b3000.dcc"
    if [ ! -d $DCC_PATH ]; then
        echo "DCC don't exist on this build."
        return
    fi

    echo  0 > $DCC_PATH/enable
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo  1 > $DCC_PATH/config_reset

    #BIMC_S_DDR0
    echo 0x00448500 41 > $DCC_PATH/config
    echo 0x0044D070 2 > $DCC_PATH/config
    #BIMC_S_DDR1
    echo 0x00450500 41 > $DCC_PATH/config
    echo 0x00455070 2 > $DCC_PATH/config
    #BMIC_M_XXX_MPORT
    echo 0x00408400 30 > $DCC_PATH/config
    echo 0x0040C400 30 > $DCC_PATH/config
    echo 0x00410400 30 > $DCC_PATH/config
    echo 0x00414400 30 > $DCC_PATH/config
    echo 0x00418400 30 > $DCC_PATH/config
    #APCS_ALIAS_APSS_GLB
    echo 0x0B011014 2  > $DCC_PATH/config
    echo 0x0B011210 1  > $DCC_PATH/config
    echo 0x0B011218 1  > $DCC_PATH/config
    #APCLUS1_L2_SAW2
    echo 0x0B01200C 1  > $DCC_PATH/config
    echo 0x0B012030 1  > $DCC_PATH/config
    #APCS_ALIAS4_APSS_ACS
    echo 0x0B088004 2  > $DCC_PATH/config
    echo 0x0B088064 1  > $DCC_PATH/config
    echo 0x0B088090 1  > $DCC_PATH/config
    #APCS_ALIAS4_SAW2
    echo 0x0B08900C 1  > $DCC_PATH/config
    echo 0x0B089030 1  > $DCC_PATH/config
    #APCS_ALIAS5_APSS_ACS
    echo 0x0B098004 2  > $DCC_PATH/config
    echo 0x0B098064 1  > $DCC_PATH/config
    echo 0x0B098090 1  > $DCC_PATH/config
    #APCS_ALIAS5_SAW2
    echo 0x0B09900C 1  > $DCC_PATH/config
    echo 0x0B099030 1  > $DCC_PATH/config
    #APCS_ALIAS6_APSS_ACS
    echo 0x0B0A8004 2  > $DCC_PATH/config
    echo 0x0B0A8064 1  > $DCC_PATH/config
    echo 0x0B0A8090 1  > $DCC_PATH/config
    #APCS_ALIAS6_SAW2
    echo 0x0B0A900C 1  > $DCC_PATH/config
    echo 0x0B0A9030 1  > $DCC_PATH/config
    #APCS_ALIAS7_APSS_ACS
    echo 0x0B0B8004 2  > $DCC_PATH/config
    echo 0x0B0B8064 1  > $DCC_PATH/config
    echo 0x0B0B8090 1  > $DCC_PATH/config
    #APCS_ALIAS7_SAW2
    echo 0x0B0B900C 1  > $DCC_PATH/config
    echo 0x0B0B9030 1  > $DCC_PATH/config
    #APCS_ALIAS0_APSS_GLB
    echo 0x0B111014 2  > $DCC_PATH/config
    echo 0x0B111210 1  > $DCC_PATH/config
    echo 0x0B111218 1  > $DCC_PATH/config
    #APCLUS0_L2_SAW2
    echo 0x0B11200C 1  > $DCC_PATH/config
    echo 0x0B112030 1  > $DCC_PATH/config
    #APCS_ALIAS0_APSS_ACS
    echo 0x0B188004 2  > $DCC_PATH/config
    echo 0x0B188064 1  > $DCC_PATH/config
    #APCS_ALIAS0_SAW2
    echo 0x0B18900C 1  > $DCC_PATH/config
    echo 0x0B189030 1  > $DCC_PATH/config
    #APCS_ALIAS1_APSS_ACS
    echo 0x0B198004 2  > $DCC_PATH/config
    echo 0x0B198064 1  > $DCC_PATH/config
    #APCS_ALIAS1_SAW2
    echo 0x0B19900C 1  > $DCC_PATH/config
    echo 0x0B199030 1  > $DCC_PATH/config
    #APCS_ALIAS2_APSS_ACS
    echo 0x0B1A8004 2  > $DCC_PATH/config
    echo 0x0B1A8064 1  > $DCC_PATH/config
    #APCS_ALIAS2_SAW2
    echo 0x0B1A900C 1  > $DCC_PATH/config
    echo 0x0B1A9030 1  > $DCC_PATH/config
    #APCS_ALIAS3_APSS_ACS
    echo 0x0B1B8004 2  > $DCC_PATH/config
    echo 0x0B1B8064 1  > $DCC_PATH/config
    #APCS_ALIAS3_SAW2
    echo 0x0B1B900C 1  > $DCC_PATH/config
    echo 0x0B1B9030 1  > $DCC_PATH/config
    #APCS_COMMON_APSS_SHARED
    echo 0x0B1D1058 2  > $DCC_PATH/config
    echo 0x0B1D200C 1  > $DCC_PATH/config
    echo 0x0B1D2030 1  > $DCC_PATH/config

    echo  1 > $DCC_PATH/enable
}

# Function DCC configuration
enable_dcc_config()
{
    target=`getprop ro.board.platform`

    if [ -f /sys/devices/soc0/soc_id ]; then
        soc_id=`cat /sys/devices/soc0/soc_id`
    else
        soc_id=`cat /sys/devices/system/soc/soc0/id`
    fi

    case "$target" in
        "msm8996")
            echo "Enabling DCC config for 8996."
            enable_msm8996_dcc_config
            ;;

	"msm8998")
	    echo "Enabling DCC config for msm8998."
	    enable_msm8998_dcc_config
	    ;;

        "msm8953")
            echo "Enabling DCC config for 8953."
            enable_msm8953_dcc_config
            ;;

        "msm8952")
            case "$soc_id" in
                "266" | "274" | "277" | "278")
                     echo "Enabling DCC config for 8976."
                     enable_msm8976_dcc_config
                     ;;
            esac
            ;;
    esac
}

enable_msm8998_core_hang_config()
{
    CORE_PATH_SILVER="/sys/devices/system/cpu/hang_detect_silver"
    CORE_PATH_GOLD="/sys/devices/system/cpu/hang_detect_gold"
    if [ ! -d $CORE_PATH ]; then
        echo "CORE hang does not exist on this build."
        return
    fi

    #select instruction retire as the pmu event
    echo 0x7 > $CORE_PATH_SILVER/pmu_event_sel
    echo 0xA > $CORE_PATH_GOLD/pmu_event_sel

    #set the threshold to around 9 milli-second
    echo 0x2a300 > $CORE_PATH_SILVER/threshold
    echo 0x2a300 > $CORE_PATH_GOLD/threshold

    #To the enable core hang detection
    #echo 0x1 > /sys/devices/system/cpu/hang_detect_silver/enable
    #echo 0x1 > /sys/devices/system/cpu/hang_detect_gold/enable
}

enable_msm8998_osm_wdog_status_config()
{
    echo 1 > /sys/kernel/debug/osm/pwrcl_clk/wdog_trace_enable
    echo 1 > /sys/kernel/debug/osm/perfcl_clk/wdog_trace_enable
}

enable_msm8998_gladiator_hang_config()
{
    GLADIATOR_PATH="/sys/devices/system/cpu/gladiator_hang_detect"
    if [ ! -d $GLADIATOR_PATH ]; then
        echo "Gladiator hang does not exist on this build."
        return
    fi

    #set the threshold to around 9 milli-second
    echo 0x0002a300 > $GLADIATOR_PATH/ace_threshold
    echo 0x0002a300 > $GLADIATOR_PATH/io_threshold
    echo 0x0002a300 > $GLADIATOR_PATH/m1_threshold
    echo 0x0002a300 > $GLADIATOR_PATH/m2_threshold
    echo 0x0002a300 > $GLADIATOR_PATH/pcio_threshold

    #To enable gladiator hang detection
    #echo 0x1 > /sys/devices/system/cpu/gladiator_hang_detect/enable
}

enable_osm_wdog_status_config()
{
    target=`getprop ro.board.platform`

    case "$target" in
        "msm8998")
            echo "Enabling OSM WDOG status registers for msm8998"
            enable_msm8998_osm_wdog_status_config
        ;;
    esac
}

enable_core_gladiator_hang_config()
{
    target=`getprop ro.board.platform`

    case "$target" in
        "msm8998")
            echo "Enabling core & gladiator config for msm8998"
            enable_msm8998_core_hang_config
            enable_msm8998_gladiator_hang_config
        ;;
    esac
}

coresight_config=`getprop persist.debug.coresight.config`
coresight_stm_cfg_done=`getprop ro.dbg.coresight.stm_cfg_done`

#Android turns off tracing by default. Make sure tracing is turned on after boot is done
if [ ! -z $coresight_stm_cfg_done ]
then
    echo 1 > /sys/kernel/debug/tracing/tracing_on
    exit
fi

enable_dcc_config
enable_core_gladiator_hang_config
enable_osm_wdog_status_config

case "$coresight_config" in
    "stm-events")
        echo "Enabling STM events."
        enable_stm_events
        setprop ro.dbg.coresight.stm_cfg_done 1
        ;;
    *)
        echo "Skipping coresight configuration."
        exit
        ;;
esac
