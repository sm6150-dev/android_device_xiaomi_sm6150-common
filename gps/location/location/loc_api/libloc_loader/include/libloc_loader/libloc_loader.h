#ifndef LIBLOC_LOADER_H
#define LIBLOC_LOADER_H

#define LIBDSI_NETCTRL "libdsi_netctrl.so"
#define LIBQMI_CCI "libqmi_cci.so"
#define LIBQMI_COMMON_SO "libqmi_common_so.so"
#define LIBQMISERVICES "libqmiservices.so"
#define LIBQSOCKET "libqsocket.so"

__BEGIN_DECLS

void load_proprietary_symbols();

__END_DECLS

#endif
