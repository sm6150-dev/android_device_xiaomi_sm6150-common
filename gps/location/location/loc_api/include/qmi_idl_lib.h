#ifndef QMI_IDL_LIB_H
#define QMI_IDL_LIB_H

#include <limits.h>

enum qmi_result_type_v01 {
        /* To force a 32 bit signed enum. Do not change or use*/
        QMI_RESULT_TYPE_MIN_ENUM_VAL_V01 = INT_MIN,
        QMI_RESULT_SUCCESS_V01 = 0,
        QMI_RESULT_FAILURE_V01 = 1,
        QMI_RESULT_TYPE_MAX_ENUM_VAL_V01 = INT_MAX,
};

enum qmi_error_type_v01 {
        /* To force a 32 bit signed enum. Do not change or use*/
        QMI_ERR_TYPE_MIN_ENUM_VAL_V01 = INT_MIN,
        QMI_ERR_NONE_V01 = 0x0000,
        QMI_ERR_MALFORMED_MSG_V01 = 0x0001,
        QMI_ERR_NO_MEMORY_V01 = 0x0002,
        QMI_ERR_INTERNAL_V01 = 0x0003,
        QMI_ERR_CLIENT_IDS_EXHAUSTED_V01 = 0x0005,
        QMI_ERR_DEVICE_IN_USE_V01 = 0x0017,
        QMI_ERR_INVALID_ID_V01 = 0x0029,
        QMI_ERR_INVALID_ARG_V01 = 0x0030,
        QMI_ERR_ENCODING_V01 = 0x003A,
        QMI_ERR_INCOMPATIBLE_STATE_V01 = 0x005A,
        QMI_ERR_NOT_SUPPORTED_V01 = 0x005E,
        QMI_ERR_TYPE_MAX_ENUM_VAL_V01 = INT_MAX,
};

typedef enum {
  QMI_IDL_REQUEST = 0,
  QMI_IDL_RESPONSE,             /**< QMI Response */
  QMI_IDL_INDICATION,           /**< QMI Indication */
  QMI_IDL_NUM_MSG_TYPES         /**< Sentinel for error checking */
} qmi_idl_message_type;

#define QMI_IDL_FLAGS_OFFSET_IS_16    0x80
#define QMI_IDL_FLAGS_IS_ARRAY        0x40
#define QMI_IDL_FLAGS_SZ_IS_16        0x20
#define QMI_IDL_FLAGS_IS_VARIABLE_LEN 0x10
#define QMI_IDL_FLAGS_FIRST_EXTENDED  0x08
#define QMI_IDL_FLAGS_TYPE            0x07

/* Build a 16-bit type field from a type index and a table index */
#define QMI_IDL_TYPE16(table, type) (((table) << 12) | (type))
#define QMI_IDL_TYPE88(table, type) (type & 0xFF), (((type & 0xFF00) >> 4) | table)


#define QMI_IDL_OFFSET8(t,f)          ((uint8_t) offsetof(t,f))
#define QMI_IDL_OFFSET16RELATIVE(t,f) offsetof(t,f)
#define QMI_IDL_OFFSET16ARRAY(t,f)    ((uint8_t) offsetof(t,f)), \
                                      ((uint8_t) (offsetof(t,f) >> 8))
#define QMI_IDL_OFFSET32ARRAY(t,f)    ((uint8_t) offsetof(t,f)), \
                                      ((uint8_t) (offsetof(t,f) >> 8)), \
                                      ((uint8_t) (offsetof(t,f) >> 16))

#define QMI_IDL_FLAG_END_VALUE 0x20

#define QMI_IDL_TLV_FLAGS_LAST_TLV              0x80
#define QMI_IDL_TLV_FLAGS_OPTIONAL              0x40

#define QMI_IDL_GENERIC_1_BYTE     0
#define QMI_IDL_GENERIC_2_BYTE     1
#define QMI_IDL_GENERIC_4_BYTE     2
#define QMI_IDL_GENERIC_8_BYTE     3
#define QMI_IDL_1_BYTE_ENUM        4
#define QMI_IDL_2_BYTE_ENUM        5
#define QMI_IDL_STRING             6
#define QMI_IDL_AGGREGATE          7

#define QMI_NO_ERR 0
#define QMI_SERVICE_ERR -2

typedef int qmi_client_error_type;

typedef struct {
    unsigned int info[5];
} qmi_service_info;

typedef void * qmi_idl_service_object_type;

typedef struct qmi_response_type_v01 {
    enum qmi_result_type_v01 result;
    enum qmi_error_type_v01 error;
} qmi_response_type_v01;

typedef struct qmi_get_supported_msgs_resp_v01 {
    uint8_t supported_msgs_valid;
    uint32_t supported_msgs_len;
    uint8_t supported_msgs[8192];
} qmi_get_supported_msgs_resp_v01;

typedef struct {
    uint32_t size;
    const uint8_t *data;
} qmi_idl_type_table_entry;

typedef struct {
    uint32_t size;
    const uint8_t *data;
} qmi_idl_message_table_entry;

typedef struct {
    uint16_t msg_id;
    uint16_t table_id;
    uint16_t msg_len;
} qmi_idl_service_message_table_entry;

// Forward declaration
struct qmi_idl_type_table_object;

typedef struct qmi_idl_service_object {
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t unk4;
    uint16_t num_messages[QMI_IDL_NUM_MSG_TYPES];
    const qmi_idl_message_table_entry *message_table[QMI_IDL_NUM_MSG_TYPES];
    const struct qmi_idl_type_table_object *type_table;
    uint32_t unk5;
    struct qmi_idl_service_object *parent;
} qmi_idl_service_object;

typedef struct qmi_idl_type_table_object {
    uint16_t num_types;
    uint16_t num_messages;
    uint8_t num_tables;
    const qmi_idl_type_table_entry *type_table;
    const qmi_idl_message_table_entry *message_table;
    const struct qmi_idl_type_table_object **type_table_object;
    const void *unk1;
} qmi_idl_type_table_object;

// Opague pointer allocated and used by external library
typedef void* qmi_client_type;

// Provided by qcom library
extern const qmi_idl_type_table_object common_qmi_idl_type_table_object_v01;

#endif /* QMI_IDL_LIB_H */
