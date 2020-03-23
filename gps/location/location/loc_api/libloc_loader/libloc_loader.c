#include <dlfcn.h>

#define LOG_TAG "LIBLOC_LOADER"

#include <log/log.h>

#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "libloc_loader.h"

void *lib_handle = NULL;

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

void load_proprietary_symbols() {
   load_from_libqmi_cci();
   load_from_libqmi_common_so();
}
