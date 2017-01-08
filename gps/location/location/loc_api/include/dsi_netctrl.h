#ifndef DSI_NETCTRL_H
#define DSI_NETCTRL_H

#include "qmi_idl_lib.h"

#define QMI_WDS_GET_PROFILE_LIST_REQ_V01 0x002A
#define QMI_WDS_GET_PROFILE_LIST_RESP_V01 0x002A
#define QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01 0x002B

#define WDS_PROFILE_TYPE_3GPP_V01 0

// FIXME: Find actual value?
#define DSI_EVT_WDS_CONNECTED 0

#define DSI_CALL_INFO_UMTS_PROFILE_IDX 1
#define DSI_CALL_INFO_IP_VERSION 11

#define DSI_SUCCESS 0
#define DSI_MODE_GENERAL 0

#define DSI_IP_VERSION_4 4
#define DSI_IP_VERSION_6 6
#define DSI_IP_VERSION_4_6 10

#define WDS_PDP_TYPE_PDP_IPV4_V01 0
#define WDS_PDP_TYPE_PDP_IPV6_V01 2
#define WDS_PDP_TYPE_PDP_IPV4V6_V01 3

#define LOC_V01_IDL_MAJOR_VERS 0x01
#define LOC_V01_IDL_MINOR_VERS 0x23
#define LOC_V01_IDL_TOOL_VERS  0x06

typedef enum
{
  DSI_EVT_INVALID = 0x0,              /**< Invalid event. */
  DSI_EVT_NET_IS_CONN,                /**< Call is connected. */
  DSI_EVT_NET_NO_NET,                 /**< Call is disconnected. */
  DSI_EVT_PHYSLINK_DOWN_STATE,        /**< Physlink becomes dormant. */
  DSI_EVT_PHYSLINK_UP_STATE,          /**< Physlink becomes active. */
  DSI_EVT_NET_RECONFIGURED,           /**< Interface is reconfigured. */
  DSI_EVT_QOS_STATUS_IND,             /**< A status for the associated QoS has
                                           changed. */
  DSI_EVT_NET_NEWADDR,                /**< New address is generated. */
  DSI_EVT_NET_DELADDR,                /**< An address for the interface has been deleted. */
  DSI_EVT_NET_PARTIAL_CONN,           /**< Address is available for either IPv4
                                           or IPv6 only. @newpage */
  DSI_NET_TMGI_ACTIVATED,
  DSI_NET_TMGI_DEACTIVATED,
  DSI_NET_TMGI_ACTIVATED_DEACTIVATED,
  DSI_NET_TMGI_LIST_CHANGED,
  DSI_NET_SAI_LIST_CHANGED,
  DSI_EVT_NET_HANDOFF,
  DSI_EVT_MAX
} dsi_net_evt_t;

qmi_idl_service_object_type wds_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define wds_get_service_object_v01( ) \
          wds_get_service_object_internal_v01( \
            LOC_V01_IDL_MAJOR_VERS, LOC_V01_IDL_MINOR_VERS, \
            LOC_V01_IDL_TOOL_VERS )

/* Structs */

typedef struct wds_get_profile_list_req_msg_v01
{
    uint8_t profile_type_valid;
    uint32_t profile_type;
} wds_get_profile_list_req_msg_v01;

typedef struct wds_get_profile_settings_req_msg_v01
{
    struct {
        uint32_t profile_type;
        uint8_t profile_index;
    } profile;
} wds_get_profile_settings_req_msg_v01;

typedef struct wds_get_profile_list_resp_msg_v01
{
    uint32_t profile_list_len;
    struct {
      uint32_t profile_index;
      uint32_t profile_type;
    }profile_list[255];
    qmi_response_type_v01 resp;
    uint8_t unk1;
    uint16_t unk2;
} wds_get_profile_list_resp_msg_v01;

// Only the fields filled in below are accessed by the open source code.
// The remainder is potentially filled out by library code which we can't access
// The total size found by reversing is 2016 bytes.
typedef struct wds_get_profile_settings_resp_msg_v01
{
    qmi_response_type_v01 resp;
    uint8_t profile_name_valid;
    char profile_name[51];
    uint8_t pdp_type_valid;
    uint32_t pdp_type;
    uint8_t padding1[948]; // Needed for offsets to be correct
    uint32_t support_emergency_calls_valid;
    uint32_t support_emergency_calls;
    // Needed to ensure a sufficiently large struct is allocated.
    uint8_t padding2[992];
} wds_get_profile_settings_resp_msg_v01;

typedef struct wds_profile_identifier_type_v01
{
    uint32_t profile_type;
    uint8_t profile_index;
} wds_profile_identifier_type_v01;

typedef struct dsi_call_param_value_t
{
    uint32_t *buf_val;
    uint32_t num_val;
} dsi_call_param_value_t;

typedef void* dsi_hndl_t;
typedef void dsi_evt_payload_t;

/* Callbacks */
typedef void (*net_ev_cb_type)(
    dsi_hndl_t handle, void* user_data,
    dsi_net_evt_t evt, dsi_evt_payload_t *payload_ptr
);

/* Functions */
extern int dsi_init();
extern int dsi_start_data_call(dsi_hndl_t handle);
extern int dsi_stop_data_call(dsi_hndl_t handle);
extern int dsi_set_data_call_param(dsi_hndl_t handle, uint8_t call_info, dsi_call_param_value_t *param_info);
extern int dsi_rel_data_srvc_hndl(dsi_hndl_t handle);
extern dsi_hndl_t *dsi_get_data_srvc_hndl(net_ev_cb_type callback, void *cb_data);

#endif /* DSI_NETCTRL_H */
