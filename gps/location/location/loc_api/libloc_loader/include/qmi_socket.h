#pragma once

#ifdef USE_QSOCKET

// kernel/sony/msm-4.9/kernel/include/uapi/linux/msm_ipc.h
#include <linux/msm_ipc.h>

#define AF_IPC_ROUTER AF_MSM_IPC
#define IPCR_ADDR_NAME MSM_IPC_ADDR_NAME
#define qsockaddr_ipcr sockaddr_msm_ipc

typedef struct msm_ipc_port_name ipcr_name_t;

typedef int (*ipcr_find_name_t)(int sid, ipcr_name_t *ipcr_name,
    struct qsockaddr_ipcr *ipcr_addr, unsigned int *unused,
    unsigned int *num_entries, unsigned int conf);

extern ipcr_find_name_t ipcr_find_name;

#endif
