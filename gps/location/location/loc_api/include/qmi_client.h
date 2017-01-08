#ifndef QMI_CLIENT_H
#define QMI_CLIENT_H

#include "qmi_idl_lib.h"

extern qmi_client_error_type qmi_client_message_decode(
    qmi_client_type user_handle,
    qmi_idl_message_type message_type,
    unsigned int msg_id,
    void *ind_buf,
    unsigned int ind_buf_len,
    void *indBuffer,
    size_t indSize
);

extern qmi_client_error_type qmi_client_get_service_instance(
    qmi_idl_service_object_type service_object,
    int instanceId,
    qmi_service_info *serviceInfo
);

extern qmi_client_error_type qmi_client_get_any_service(
    qmi_idl_service_object_type service_object,
    qmi_service_info *serviceInfo
);

typedef void (*locClientIndCbType)(
    qmi_client_type user_handle,
    unsigned int msg_id,
    void *ind_buf,
    unsigned int ind_buf_len,
    void *ind_cb_data
);

extern qmi_client_error_type qmi_client_init(
    qmi_service_info *serviceInfo,
    qmi_idl_service_object_type service_object,
    locClientIndCbType init_callback,
    void *cb_data,
    void * unknown,
    qmi_client_type *client
);

typedef void (*qmi_client_error_cb_type)(
    qmi_client_type user_handle,
    qmi_client_error_type error,
    void *err_cb_data
);

extern qmi_client_error_type qmi_client_register_error_cb(
    qmi_client_type user_handle,
    qmi_client_error_cb_type error_cb,
    void *cb_data
);

extern qmi_client_error_type qmi_client_get_service_list(
    qmi_idl_service_object_type ds_client,
    qmi_service_info *service_info,
    uint32_t *num_entries,
    uint32_t *num_services
);

extern qmi_client_error_type qmi_client_send_msg_sync(
    qmi_client_type client_handle,
    uint32_t req_id,
    void *list_req,
    uint32_t req_len,
    void* list_resp,
    uint32_t resp_len,
    uint32_t timeout
);

extern int qmi_client_release ();

#endif /* QMI_CLIENT_H */
