#include <dlfcn.h>
#include <log/log.h>

#include "../include/qmi_client.h"
#include "../include/qmi_idl_lib.h"
#include "../include/dsi_netctrl.h"
#include "libloc_loader.h"

#define LOG_TAG "LIBLOC_LOADER"

void *lib_handle = NULL;

void load_from_libdsi_netctrl() {
    lib_handle = dlopen(LIBDSI_NETCTRL, RTLD_NOW);
    if (!lib_handle) {
        ALOGE("%s: DLOPEN failed for %s", __func__, LIBDSI_NETCTRL);
    } else {
        dsi_init = dlsym(lib_handle, "dsi_init");
        dsi_release = dlsym(lib_handle, "dsi_release");
        dsi_start_data_call = dlsym(lib_handle, "dsi_start_data_call");
        dsi_stop_data_call = dlsym(lib_handle, "dsi_stop_data_call");
        dsi_set_data_call_param = dlsym(lib_handle, "dsi_set_data_call_param");
        dsi_rel_data_srvc_hndl = dlsym(lib_handle, "dsi_rel_data_srvc_hndl");
        dsi_get_data_srvc_hndl = dlsym(lib_handle, "dsi_get_data_srvc_hndl");
    }
}

void load_from_libqmi_cci() {
    lib_handle = dlopen(LIBQMI_CCI, RTLD_NOW);
    if (!lib_handle) {
        ALOGE("%s: DLOPEN failed for %s", __func__, LIBQMI_CCI);
    } else {
        qmi_client_message_decode = dlsym(lib_handle, "qmi_client_message_decode");
        qmi_client_get_service_instance = dlsym(lib_handle, "qmi_client_get_service_instance");
        qmi_client_get_any_service = dlsym(lib_handle, "qmi_client_get_any_service");
        qmi_client_init = dlsym(lib_handle, "qmi_client_init");
        qmi_client_register_error_cb = dlsym(lib_handle, "qmi_client_register_error_cb");
        qmi_client_get_service_list = dlsym(lib_handle, "qmi_client_get_service_list");
        qmi_client_send_msg_sync = dlsym(lib_handle, "qmi_client_send_msg_sync");
        qmi_client_release = dlsym(lib_handle, "qmi_client_release");
    }
}

void load_from_libqmi_common_so() {
    lib_handle = dlopen(LIBQMI_COMMON_SO, RTLD_NOW);
    if (!lib_handle) {
        ALOGE("%s: DLOPEN failed for %s", __func__, LIBQMI_COMMON_SO);
    } else {
        common_qmi_idl_type_table_object_v01 = *(qmi_idl_type_table_object*) dlsym(lib_handle, "common_qmi_idl_type_table_object_v01");
    }
}

void load_from_libqmiservices() {
    lib_handle = dlopen(LIBQMISERVICES, RTLD_NOW);
    if (!lib_handle) {
        ALOGE("%s: DLOPEN failed for %s", __func__, LIBQMISERVICES);
    } else {
        wds_get_service_object_internal_v01 = dlsym(lib_handle, "wds_get_service_object_internal_v01");
    }
}

void load_proprietary_symbols() {
   load_from_libdsi_netctrl();
   load_from_libqmi_cci();
   load_from_libqmi_common_so();
   load_from_libqmiservices();
}
