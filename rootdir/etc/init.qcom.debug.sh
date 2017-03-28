#!/system/bin/sh
# Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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

HERE=/system/etc
source $HERE/init.qcom.debug-sdm660.sh
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
    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/$sinkenable
    echo 1 > /sys/bus/coresight/devices/coresight-stm/$srcenable
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

# Function SDM845 DCC configuration
enable_sdm845_dcc_config()
{
    DCC_PATH="/sys/bus/platform/devices/10a2000.dcc_v2"

    if [ ! -d $DCC_PATH ]; then
        echo "DCC does not exist on this build."
        return
    fi

    echo 0 > $DCC_PATH/enable
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo 1 > $DCC_PATH/config_reset
    echo 2 > $DCC_PATH/curr_list

    #configuration start

    #Apply configuration and enable DCC
    echo  1 > $DCC_PATH/enable
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

    # CCI ACE / Stalled Transaction
    echo 0x179082B0 > $DCC_PATH/config

    # 8 times, same register
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config

    echo  1 > $DCC_PATH/enable
}

# Function MSMFALCON DCC configuration
enable_sdm660_dcc_config()
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
    echo 0x179C4000 1 > $DCC_PATH/config
    echo 0x179C8000 1 > $DCC_PATH/config
    echo 0x179CBAA4 1 > $DCC_PATH/config
    echo 0x179C7AA4 1 > $DCC_PATH/config

    #APCS_ALIAS0_APSS_ACS
    echo 0x17988004 2 > $DCC_PATH/config    #CPU_PWR_CTL and APC_PWR_STATUS
    echo 0x17988064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS0_SAW4_1_1_SPM
    echo 0x17989000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS0_SAW4_1_1_STS
    echo 0x17989C0C 2 > $DCC_PATH/config    #SPM_STS
    echo 0x17989C10 > $DCC_PATH/config    #SPM_STS2
    echo 0x17989C20 > $DCC_PATH/config    #SPM_STS3
    echo 0x17989C18 > $DCC_PATH/config    #PMIC_STS

    #APCS_ALIAS1_APSS_ACS
    echo 0x17998004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x17998064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS1_SAW4_1_1_SPM
    echo 0x17999000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS1_SAW4_1_1_STS
    echo 0x17999C0C > $DCC_PATH/config    #SPM_STS
    echo 0x17999C10 > $DCC_PATH/config    #SPM_STS2
    echo 0x17999C20 > $DCC_PATH/config    #SPM_STS3

    #APCS_ALIAS2_APSS_ACS
    echo 0x179A8004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x179A8064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS2_SAW4_1_1_SPM
    echo 0x179A9000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS2_SAW4_1_1_STS
    echo 0x179A9C0C > $DCC_PATH/config    #SPM_STS
    echo 0x179A9C10 > $DCC_PATH/config    #SPM_STS2
    echo 0x179A9C20 > $DCC_PATH/config    #SPM_STS3

    #APCS_ALIAS3_APSS_ACS
    echo 0x179B8004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x179B8064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS3_SAW4_1_1_SPM
    echo 0x179B9000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS3_SAW4_1_1_STS
    echo 0x179B9C0C > $DCC_PATH/config    #SPM_STS
    echo 0x179B9C10 > $DCC_PATH/config    #SPM_STS2
    echo 0x179B9C20 > $DCC_PATH/config    #SPM_STS3

    #APCS_ALIAS0_APSS_GLB
    echo 0x17911014 2 > $DCC_PATH/config    #L2_PWR_CTL and #L2_PWR_STATUS
    echo 0x17911210 > $DCC_PATH/config    #L2_SPM_QCHANNEL_CFG
    echo 0x17911218 > $DCC_PATH/config    #L2_FLUSH_CTL
    echo 0x17911234 > $DCC_PATH/config    #L2_FLUSH_STS
    echo 0x17911290 > $DCC_PATH/config    #DX_FSM_STATUS
    #APCLUS0_L2_SAW4_1_1_SPM
    echo 0x17912000 2 > $DCC_PATH/config    #SPM_CTL and #SPM_DLY
    echo 0x1791200C > $DCC_PATH/config    #SPM_CFG
    #APCLUS0_L2_SAW4_1_1_STS
    echo 0x17912C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x17912C20 > $DCC_PATH/config    #SPM_STS3
    echo 0x17912C18 > $DCC_PATH/config    #PMIC_STS


    #APCS_ALIAS4_APSS_ACS
    echo 0x17888004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x17888064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS4_SAW4_1_1_SPM
    echo 0x17889000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS4_SAW4_1_1_STS
    echo 0x17889C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x17889C20 > $DCC_PATH/config    #SPM_STS3
    echo 0x17889C18 > $DCC_PATH/config    #PMIC_STS

    #APCS_ALIAS5_APSS_ACS
    echo 0x17898004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x17898064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS5_SAW4_1_1_SPM
    echo 0x17899000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS5_SAW4_1_1_STS
    echo 0x17899C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x17899C20 > $DCC_PATH/config    #SPM_STS3

    #APCS_ALIAS6_APSS_ACS
    echo 0x178A8004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x178A8064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS6_SAW4_1_1_SPM
    echo 0x178A9000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS6_SAW4_1_1_STS
    echo 0x178A9C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x178A9C20 > $DCC_PATH/config    #SPM_STS3

    #APCS_ALIAS7_APSS_ACS
    echo 0x178B8004 2 > $DCC_PATH/config    #CPU_PWR_CTL and #APC_PWR_STATUS
    echo 0x178B8064 > $DCC_PATH/config    #SPM_QCHANNEL_CFG
    #APCS_ALIAS7_SAW4_1_1_SPM
    echo 0x178B9000 > $DCC_PATH/config    #SPM_CTL
    #APCS_ALIAS7_SAW4_1_1_STS
    echo 0x178B9C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x178B9C20 > $DCC_PATH/config    #SPM_STS3

    #APCS_ALIAS1_APSS_GLB
    echo 0x17811014 > $DCC_PATH/config    #L2_PWR_CTL
    echo 0x17811018 > $DCC_PATH/config    #L2_PWR_STATUS
    echo 0x17811210 > $DCC_PATH/config    #L2_SPM_QCHANNEL_CFG
    echo 0x17811218 > $DCC_PATH/config    #L2_FLUSH_CTL
    echo 0x17811234 > $DCC_PATH/config    #L2_FLUSH_STS
    echo 0x17811290 > $DCC_PATH/config    #DX_FSM_STATUS
    #APCLUS1_L2_SAW4_1_1_SPM
    echo 0x17812000 2 > $DCC_PATH/config    #SPM_CTL and #SPM_DLY
    echo 0x1781200C > $DCC_PATH/config    #SPM_CFG
    #APCLUS1_L2_SAW4_1_1_STS
    echo 0x17812C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x17812C20 > $DCC_PATH/config    #SPM_STS3
    echo 0x17812C18 > $DCC_PATH/config    #PMIC_STS

    #APCS_CCI_GLADIATOR
    echo 0x07BA0008 > $DCC_PATH/config    #APCS_CCI_GLADIATOR_MAIN_CRIXUS_CFGIO
    echo 0x07BA0078 > $DCC_PATH/config    #APCS_CCI_GLADIATOR_MAIN_CRIXUS_CFGDUT
    echo 0x07BA0080 3 > $DCC_PATH/config  #GNOC ACE0 Status
    echo 0x07BA00A0 3 > $DCC_PATH/config  #GNOC ACE1 Status
    echo 0x07BA1014 11 > $DCC_PATH/config    #APCS_CCI_GLADIATOR_MAIN_CRIXUS_MAIN ERRORS
    echo 0x07BA4008 4 > $DCC_PATH/config    #APCS_CCI_PD_GNOC_MAIN_PROGPOWERCONTROLLER_TARGET0
    echo 0x07BA800C 12 > $DCC_PATH/config #APCS_CCI_TRACE_MAIN ERRORS
    echo 0x07BA82B0 > $DCC_PATH/config    #APCS_CCI_PORT_STATUS
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1000 4 > $DCC_PATH/config    #GLADIATOR STALLED TRANSACTION READ 8 TIMES
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config    #Read Same addresses 100 times
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config
    echo 0x07BA1008 2 > $DCC_PATH/config

    #SNOC Fault/Error registers
    echo 0x0162000C > $DCC_PATH/config   #SNOC_OBS_ERRVLD
    echo 0x01620014 6 > $DCC_PATH/config
    echo 0x01620108 4 > $DCC_PATH/config  # SNOC_SBM_FAULTSTATUS
    echo 0x016201B0 >  $DCC_PATH/config  # SNOC_SBM_SENSEIN0

    #CCI_SAW4_1_1_SPM
    echo 0x179D2000 > $DCC_PATH/config    #SPM_CTL
    #CCI_SAW4_1_1_STS
    echo 0x179D2C0C 2 > $DCC_PATH/config    #SPM_STS and #SPM_STS2
    echo 0x179D2C20 > $DCC_PATH/config    #SPM_STS3
    echo 0x179D2C18 > $DCC_PATH/config    #PMIC_STS

    #LMH_PERF
    echo 0x179CD0C0 > $DCC_PATH/config  #TSense0_Reg
    echo 0x179CD0C4 > $DCC_PATH/config  #Tsense1_Reg
    echo 0x179CD0CC > $DCC_PATH/config  #Tsense2_Reg
    echo 0x179CD0CD > $DCC_PATH/config  #Tsense3_Reg
    echo 0x179CD03C > $DCC_PATH/config  #TTL_WORD_REG
    echo 0x179CD300 > $DCC_PATH/config  #FIFO0
    echo 0x179CD310 > $DCC_PATH/config  #FIFO1
    echo 0x179CD320 > $DCC_PATH/config  #FIFO2
    echo 0x179CD330 > $DCC_PATH/config  #FIFO2
    echo 0x179CD810 > $DCC_PATH/config  #LMH_DCVS_CS_STATUS
    echo 0x179CD814 > $DCC_PATH/config  #LMH_DCVS_THERMAL_STATUS
    echo 0x179CD81C > $DCC_PATH/config  #LMH_DCVS_BCL_STATUS

    #LMH_PWR
    echo 0x179CF0C0 > $DCC_PATH/config  #TSense0_Reg
    echo 0x179CF0C4 > $DCC_PATH/config  #Tsense1_Reg
    echo 0x179CF03C > $DCC_PATH/config  #TTL_WORD_REG
    echo 0x179CF300 > $DCC_PATH/config  #FIFO0
    echo 0x179CF310 > $DCC_PATH/config  #FIFO1
    echo 0x179CF814 > $DCC_PATH/config  #LMH_DCVS_THERMAL_STATUS

    #ISENSE
    echo 0x179DC10C > $DCC_PATH/config  #Isense core01
    echo 0x179DC110 > $DCC_PATH/config  #Isense core23

    #BIMC_GLOBAL2
    echo 0x010021F0 4 > $DCC_PATH/config    #BIMC_BRIC_DEFAULT_SEGMENT

    #GCC_GPLL0
    echo 0x100000 10 > $DCC_PATH/config

    #GCC_GPLL1
    echo 0x101000 10 > $DCC_PATH/config

    #GCC_GPLL2
    echo 0x102000 10 > $DCC_PATH/config

    #GCC_GPLL3
    echo 0x103000 10  > $DCC_PATH/config

    #GCC_GPLL4
    echo 0x177000 10 > $DCC_PATH/config

    #GCC_GPLL5
    echo 0x174000 10 > $DCC_PATH/config

    echo 0x151000 > $DCC_PATH/config    #GCC_RPM_GPLL_ENA_VOTE

    echo 0x144004 > $DCC_PATH/config    #GCC_BIMC_GDSCR
    echo 0x146048 > $DCC_PATH/config    #GCC_DDR_DIM_WRAPPER_GDSCR
    echo 0x183004 > $DCC_PATH/config    #GCC_AGGRE2_NOC_GDSCR

    echo 0x104020 2 > $DCC_PATH/config
    echo 0x104040 2 > $DCC_PATH/config
    echo 0x105038 2 > $DCC_PATH/config
    echo 0x10A008 2 > $DCC_PATH/config

    #GCC_CNOC
    echo 0x105050 2 > $DCC_PATH/config

    #GCC_QDSS
    echo 0x10D000 2 > $DCC_PATH/config
    echo 0x10D018 2 > $DCC_PATH/config
    echo 0x10D030 2 > $DCC_PATH/config

    echo 0x10E000 2 > $DCC_PATH/config
    echo 0x10E018 2 > $DCC_PATH/config
    echo 0x10E030 2 > $DCC_PATH/config
    echo 0x10E048 2 > $DCC_PATH/config

    #GCC_RPM
    echo 0x13C018 2 > $DCC_PATH/config

    #GCC_CE1
    echo 0x141010 2 > $DCC_PATH/config

    #GCC_BIMC
    echo 0x145018 2 > $DCC_PATH/config

    #GCC_BIMC_DDR_CPLL0_ROOT
    echo 0x146010 2 > $DCC_PATH/config

    #GCC_BIMC_DDR_CPLL1_ROOT
    echo 0x146028 2 > $DCC_PATH/config

    #GCC_CDSP_BIMC
    echo 0x147030 2 > $DCC_PATH/config

    echo 0x148014 2 > $DCC_PATH/config

    echo 0x14802C 2 > $DCC_PATH/config

    echo 0x18A024 2 > $DCC_PATH/config

    echo 0x17101C 2 > $DCC_PATH/config

    echo 0x189018 2 > $DCC_PATH/config

    echo 0x17A014 2 > $DCC_PATH/config

    echo 0x17A02C 2 > $DCC_PATH/config
    echo 0x17A044 2 > $DCC_PATH/config
    echo 0x17A05C 2 > $DCC_PATH/config
    echo 0x17A074 2 > $DCC_PATH/config

    echo 0x146004 2 > $DCC_PATH/config

    echo 0xC8CC000 10 > $DCC_PATH/config

    echo 0xC8CC050 10 > $DCC_PATH/config

    echo 0xC8C00F0 10 > $DCC_PATH/config

    echo 0xC8C4000 > $DCC_PATH/config	 #MMSS_PLL_VOTE_RPM

    echo 0xC8CD000 2 > $DCC_PATH/config

    echo  0x1030560 1 > $DCC_PATH/config 	##BIMC_S_DDR0_SCMO_RCH_STATUS
    echo  0x10305a0 1 > $DCC_PATH/config 	##BIMC_S_DDR0_SCMO_WCH_STATUS
    echo  0x103c560 1 > $DCC_PATH/config 	##BIMC_S_DDR1_SCMO_RCH_STATUS
    echo  0x103c5a0 1 > $DCC_PATH/config 	##BIMC_S_DDR1_SCMO_WCH_STATUS
    echo  0x1030520 1 > $DCC_PATH/config 	##BIMC_S_DDR0_SCMO_CMD_BUF_STATUS
    echo  0x103c520 1 > $DCC_PATH/config 	##BIMC_S_DDR1_SCMO_CMD_BUF_STATUS
    echo  0x103408c 1 > $DCC_PATH/config 	##BIMC_S_DDR0_DPE_DRAM_STATUS_0
    echo  0x104008c 1 > $DCC_PATH/config 	##BIMC_S_DDR1_DPE_DRAM_STATUS_0
    echo  0x103409c 1 > $DCC_PATH/config 	##BIMC_S_DDR0_DPE_MEMC_STATUS_0
    echo  0x104009c 1 > $DCC_PATH/config 	##BIMC_S_DDR1_DPE_MEMC_STATUS_0
    echo  0xffd220  1 > $DCC_PATH/config 	##MCCC_MCCC_CLK_PERIOD
    echo  0x108f094 1 > $DCC_PATH/config 	##DDR_CC_MCCC_DDRCC_MCCC_TOP_STATUS
    echo  0xffd200  1 > $DCC_PATH/config 	##MCCC_MCCC_FREQ_SWITCH_UPDATE
    echo  0xffd02c  1 > $DCC_PATH/config 	##MCCC_MCCC_CH0_FSC_STATUS0
    echo  0xffd038  1 > $DCC_PATH/config 	##MCCC_MCCC_CH0_FSC_STATUS3
    echo  0xffd12c  1 > $DCC_PATH/config 	##MCCC_MCCC_CH1_FSC_STATUS0
    echo  0xffd138  1 > $DCC_PATH/config 	##MCCC_MCCC_CH1_FSC_STATUS3
    echo  0x1086020 1 > $DCC_PATH/config 	##CH0_DDRCC_PLLCTRL_GCC_CTRL
    echo  0x1086028 1 > $DCC_PATH/config 	##CH0_DDRCC_PLLCTRL_CLK_SWITCH_STATUS
    echo  0x1086030 1 > $DCC_PATH/config 	##CH0_DDRCC_PLLCTRL_STATUS
    echo  0x1086124 1 > $DCC_PATH/config 	##CH0_DDRCC_PLL0_STATUS
    echo  0x1086164 1 > $DCC_PATH/config 	##CH0_DDRCC_PLL1_STATUS
    echo  0x10861dc 1 > $DCC_PATH/config 	##CH0_DDRCC_DLL0_STATUS
    echo  0x10861ec 1 > $DCC_PATH/config 	##CH0_DDRCC_DLL1_STATUS
    echo  0x1035070 1 > $DCC_PATH/config 	##BIMC_S_DDR0_SHKE_MREG_RDATA_STATUS
    echo  0x1041070 1 > $DCC_PATH/config 	##BIMC_S_DDR1_SHKE_MREG_RDATA_STATUS
    echo  0x10340b8 1 > $DCC_PATH/config 	##BIMC_S_DDR0_DPE_INTERRUPT_STATUS
    echo  0x103408c 1 > $DCC_PATH/config 	##BIMC_S_DDR0_DPE_DRAM_STATUS_0
    echo  0x1035074 1 > $DCC_PATH/config 	##BIMC_S_DDR0_SHKE_DRAM_STATUS

    ##BIMC_S_DDR0_SHKE_PERIODIC_MRR_RDATA
    echo 0x10350b0 4 > $DCC_PATH/config

    echo  0x10400b8 1 > $DCC_PATH/config 	##BIMC_S_DDR1_DPE_INTERRUPT_STATUS
    echo  0x1041074 1 > $DCC_PATH/config 	##BIMC_S_DDR1_SHKE_DRAM_STATUS

    ##BIMC_S_DDR1_SHKE_PERIODIC_MRR_RDATA
    echo  0x10410b0 4 > $DCC_PATH/config

    echo  0x1008100 1 > $DCC_PATH/config 	##BIMC_M_APP_MPORT_INTERRUPT_STATUS
    echo  0x1020100 1 > $DCC_PATH/config 	##BIMC_M_CDSP_MPORT_INTERRUPT_STATUS
    echo  0x100c100 1 > $DCC_PATH/config 	##BIMC_M_GPU_MPORT_INTERRUPT_STATUS
    echo  0x101c100 1 > $DCC_PATH/config 	##BIMC_M_MDSP_MPORT_INTERRUPT_STATUS
    echo  0x1010100 1 > $DCC_PATH/config 	##BIMC_M_MMSS_MPORT_INTERRUPT_STATUS
    echo  0x1018100 1 > $DCC_PATH/config 	##BIMC_M_PIMEM_MPORT_INTERRUPT_STATUS
    echo  0x1014100 1 > $DCC_PATH/config 	##BIMC_M_SYS_MPORT_INTERRUPT_STATUS



    echo  0x1030100 1 > $DCC_PATH/config     ##BIMC_S_DDR0_SCMO_INTERRUPT_STATUS
    echo  0x1030124 1 > $DCC_PATH/config     ##BIMC_S_DDR0_SCMO_ESYN_ADDR1
    echo  0x103012c 1 > $DCC_PATH/config     ##BIMC_S_DDR0_SCMO_ESYN_APACKET_1
    echo  0x1030130 1 > $DCC_PATH/config     ##BIMC_S_DDR0_SCMO_ESYN_APACKET_2
    echo  0x103c100 1 > $DCC_PATH/config     ##BIMC_S_DDR1_SCMO_INTERRUPT_STATUS
    echo  0x103c124 1 > $DCC_PATH/config     ##BIMC_S_DDR1_SCMO_ESYN_ADDR1
    echo  0x103c12c 1 > $DCC_PATH/config     ##BIMC_S_DDR1_SCMO_ESYN_APACKET_1
    echo  0x103c130 1 > $DCC_PATH/config     ##BIMC_S_DDR1_SCMO_ESYN_APACKET_2

    echo 0x01FC0804 1 > $DCC_PATH/config
    echo 0x00149024 1 > $DCC_PATH/config

    echo 0x7ba0078  4 > $DCC_PATH/config
    echo 0x7ba00A0  3 > $DCC_PATH/config
    echo 0x7ba4150  3 > $DCC_PATH/config
    echo 0x7ba4250  3 > $DCC_PATH/config

    echo 0x1008260  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_PIPE0_GATHERING
    echo 0x1008264  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_PIPE1_GATHERING
    echo 0x1008268  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_PIPE2_GATHERING
    echo 0x1008400  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_0A
    echo 0x1008404  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_0B
    echo 0x1008410  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_1A
    echo 0x1008420  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_2A
    echo 0x1008424  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_2B
    echo 0x1008430  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_3A
    echo 0x1008434  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_3B
    echo 0x1008440  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_4A
    echo 0x1008444  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_4B
    echo 0x1008450  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_5A
    echo 0x1008454  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_5B
    echo 0x1008460  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_6A
    echo 0x1008464  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_6B
    echo 0x1008470  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_7A
    echo 0x1008474  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_7B
    echo 0x1008480  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_8A
    echo 0x1008484  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_8B
    echo 0x1008490  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_9A
    echo 0x1008494  1 > $DCC_PATH/config     ##BIMC_M_APP_MPORT_STATUS_9B

    echo 0x100c260  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_PIPE0_GATHERING
    echo 0x100c264  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_PIPE1_GATHERING
    echo 0x100c268  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_PIPE2_GATHERING
    echo 0x100c400  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_0A
    echo 0x100c404  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_0B
    echo 0x100c410  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_1A
    echo 0x100c420  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_2A
    echo 0x100c424  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_2B
    echo 0x100c430  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_3A
    echo 0x100c434  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_3B
    echo 0x100c440  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_4A
    echo 0x100c444  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_4B
    echo 0x100c450  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_5A
    echo 0x100c454  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_5B
    echo 0x100c460  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_6A
    echo 0x100c464  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_6B
    echo 0x100c470  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_7A
    echo 0x100c474  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_7B
    echo 0x100c480  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_8A
    echo 0x100c484  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_8B
    echo 0x100c490  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_9A
    echo 0x100c494  1 > $DCC_PATH/config     ##BIMC_M_GPU_MPORT_STATUS_9B

    echo 0x1010260  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_PIPE0_GATHERING
    echo 0x1010264  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_PIPE1_GATHERING
    echo 0x1010268  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_PIPE2_GATHERING
    echo 0x1010400  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_0A
    echo 0x1010404  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_0B
    echo 0x1010410  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_1A
    echo 0x1010420  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_2A
    echo 0x1010424  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_2B
    echo 0x1010430  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_3A
    echo 0x1010434  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_3B
    echo 0x1010440  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_4A
    echo 0x1010444  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_4B
    echo 0x1010450  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_5A
    echo 0x1010454  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_5B
    echo 0x1010460  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_6A
    echo 0x1010464  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_6B
    echo 0x1010470  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_7A
    echo 0x1010474  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_7B
    echo 0x1010480  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_8A
    echo 0x1010484  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_8B
    echo 0x1010490  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_9A
    echo 0x1010494  1 > $DCC_PATH/config     ##BIMC_M_MMSS_MPORT_STATUS_9B

    echo 0x1014260  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_PIPE0_GATHERING
    echo 0x1014264  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_PIPE1_GATHERING
    echo 0x1014268  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_PIPE2_GATHERING
    echo 0x1014400  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_0A
    echo 0x1014404  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_0B
    echo 0x1014410  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_1A
    echo 0x1014420  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_2A
    echo 0x1014424  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_2B
    echo 0x1014430  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_3A
    echo 0x1014434  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_3B
    echo 0x1014440  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_4A
    echo 0x1014444  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_4B
    echo 0x1014450  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_5A
    echo 0x1014454  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_5B
    echo 0x1014460  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_6A
    echo 0x1014464  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_6B
    echo 0x1014470  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_7A
    echo 0x1014474  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_7B
    echo 0x1014480  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_8A
    echo 0x1014484  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_8B
    echo 0x1014490  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_9A
    echo 0x1014494  1 > $DCC_PATH/config     ##BIMC_M_SYS_MPORT_STATUS_9B

    echo 0x1018260  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_PIPE0_GATHERING
    echo 0x1018264  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_PIPE1_GATHERING
    echo 0x1018268  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_PIPE2_GATHERING
    echo 0x1018400  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_0A
    echo 0x1018404  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_0B
    echo 0x1018410  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_1A
    echo 0x1018420  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_2A
    echo 0x1018424  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_2B
    echo 0x1018430  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_3A
    echo 0x1018434  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_3B
    echo 0x1018440  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_4A
    echo 0x1018444  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_4B
    echo 0x1018450  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_5A
    echo 0x1018454  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_5B
    echo 0x1018460  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_6A
    echo 0x1018464  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_6B
    echo 0x1018470  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_7A
    echo 0x1018474  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_7B
    echo 0x1018480  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_8A
    echo 0x1018484  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_8B
    echo 0x1018490  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_9A
    echo 0x1018494  1 > $DCC_PATH/config     ##BIMC_M_PIMEM_MPORT_STATUS_9B

    echo 0x101c260  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_PIPE0_GATHERING
    echo 0x101c264  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_PIPE1_GATHERING
    echo 0x101c268  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_PIPE2_GATHERING
    echo 0x101c400  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_0A
    echo 0x101c404  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_0B
    echo 0x101c410  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_1A
    echo 0x101c420  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_2A
    echo 0x101c424  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_2B
    echo 0x101c430  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_3A
    echo 0x101c434  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_3B
    echo 0x101c440  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_4A
    echo 0x101c444  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_4B
    echo 0x101c450  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_5A
    echo 0x101c454  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_5B
    echo 0x101c460  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_6A
    echo 0x101c464  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_6B
    echo 0x101c470  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_7A
    echo 0x101c474  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_7B
    echo 0x101c480  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_8A
    echo 0x101c484  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_8B
    echo 0x101c490  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_9A
    echo 0x101c494  1 > $DCC_PATH/config     ##BIMC_M_MDSP_MPORT_STATUS_9B

    echo 0x1020260  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_PIPE0_GATHERING
    echo 0x1020264  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_PIPE1_GATHERING
    echo 0x1020268  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_PIPE2_GATHERING
    echo 0x1020400  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_0A
    echo 0x1020404  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_0B
    echo 0x1020410  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_1A
    echo 0x1020420  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_2A
    echo 0x1020424  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_2B
    echo 0x1020430  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_3A
    echo 0x1020434  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_3B
    echo 0x1020440  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_4A
    echo 0x1020444  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_4B
    echo 0x1020450  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_5A
    echo 0x1020454  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_5B
    echo 0x1020460  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_6A
    echo 0x1020464  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_6B
    echo 0x1020470  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_7A
    echo 0x1020474  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_7B
    echo 0x1020480  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_8A
    echo 0x1020484  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_8B
    echo 0x1020490  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_9A
    echo 0x1020494  1 > $DCC_PATH/config     ##BIMC_M_CDSP_MPORT_STATUS_9B

    echo 0x1049800  1 > $DCC_PATH/config     ##BIMC_S_APP_ARB_STATUS_0A
    echo 0x1051800  1 > $DCC_PATH/config     ##BIMC_S_SYS0_ARB_STATUS_0A
    echo 0x1031800  1 > $DCC_PATH/config     ##BIMC_S_DDR0_ARB_STATUS_0A
    echo 0x103d800  1 > $DCC_PATH/config     ##BIMC_S_DDR1_ARB_STATUS_0A
    echo 0x1048400  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_0A
    echo 0x1050400  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_0A
    echo 0x1048404  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_0B
    echo 0x1048408  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_0C
    echo 0x104840c  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_0D
    echo 0x1048410  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_1A
    echo 0x1048414  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_1B
    echo 0x1048418  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_1C
    echo 0x1048420  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_2A
    echo 0x1048424  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_2B
    echo 0x1048428  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_2C
    echo 0x104842c  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_2D
    echo 0x1048430  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_3A
    echo 0x1048434  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_3B
    echo 0x1048438  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_3C
    echo 0x104843c  1 > $DCC_PATH/config     ##BIMC_S_APP_SWAY_STATUS_3D
    echo 0x1050404  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_0B
    echo 0x1050408  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_0C
    echo 0x105040c  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_0D
    echo 0x1050410  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_1A
    echo 0x1050414  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_1B
    echo 0x1050418  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_1C
    echo 0x1050420  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_2A
    echo 0x1050424  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_2B
    echo 0x1050428  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_2C
    echo 0x105042c  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_2D
    echo 0x1050430  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_3A
    echo 0x1050434  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_3B
    echo 0x1050438  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_3C
    echo 0x105043c  1 > $DCC_PATH/config     ##BIMC_S_SYS0_SWAY_STATUS_3D

    ##PIMEM registers
    echo 0x00610070 9 > $DCC_PATH/config
    echo 0x006100D0 2 > $DCC_PATH/config

    ##GIC registers
    echo 0x17A00100 20 > $DCC_PATH/config
    echo 0x17A00200 20 > $DCC_PATH/config
    echo 0x17B10100 > $DCC_PATH/config
    echo 0x17B10200 > $DCC_PATH/config
    echo 0x17B30100 > $DCC_PATH/config
    echo 0x17B30200 > $DCC_PATH/config
    echo 0x17B50100 > $DCC_PATH/config
    echo 0x17B50200 > $DCC_PATH/config
    echo 0x17B70100 > $DCC_PATH/config
    echo 0x17B70200 > $DCC_PATH/config
    echo 0x17B90100 > $DCC_PATH/config
    echo 0x17B90200 > $DCC_PATH/config
    echo 0x17B90100 > $DCC_PATH/config
    echo 0x17BB0200 > $DCC_PATH/config
    echo 0x17BB0100 > $DCC_PATH/config
    echo 0x17BD0200 > $DCC_PATH/config
    echo 0x17BD0100 > $DCC_PATH/config
    echo 0x17BF0200 > $DCC_PATH/config
    echo 0x17BF0100 > $DCC_PATH/config

    # CCI ACE / Stalled Transaction
    echo 0x179082B0 > $DCC_PATH/config

    # 8 times, same register
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config
    echo 0x17901000 > $DCC_PATH/config

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

        "sdm660")
            echo "Enabling DCC config for sdm660."
            enable_sdm660_dcc_config
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

        "sdm845")
            echo "Enabling DCC config for sdm845."
            enable_sdm845_dcc_config
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
srcenable="enable"
sinkenable="curr_sink"

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
        if [ $target == "sdm660" ];
        then
            echo "Enabling STM/Debug events for SDM660"
            enable_sdm660_debug
        else
            if [ $target == "sdm845" ];
            then
                srcenable="enable_source"
                sinkenable="enable_sink"
            fi
            echo "Enabling STM events."
            enable_stm_events
            setprop ro.dbg.coresight.stm_cfg_done 1
        fi
        ;;
    *)
        echo "Skipping coresight configuration."
        exit
        ;;
esac
