AM_CFLAGS = \
    -I. \
    $(LOCPLA_CFLAGS) \
    $(GPSUTILS_CFLAGS) \
    $(QMI_CFLAGS) \
    $(QMIFW_CFLAGS) \
    $(DATA_CFLAGS) \
    -fno-short-enums \
    -D__func__=__PRETTY_FUNCTION__ \
    -DTARGET_USES_QCOM_BSP

libloc_ds_api_la_SOURCES = \
    ds_client.c

if USE_GLIB
libloc_ds_api_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_ds_api_la_LDFLAGS = -lstdc++ -Wl,-z,defs @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_ds_api_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_ds_api_la_CFLAGS = $(AM_CFLAGS)
libloc_ds_api_la_LDFLAGS = -lstdc++ -Wl,-z,defs -shared -version-info 1:0:0
libloc_ds_api_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_ds_api_la_LIBADD = $(QM_LIBS) $(QMFW_LIBS) -lqmiservices -ldsi_netctrl $(GPSUTILS_LIBS)

library_include_HEADERS = \
    ds_client.h

library_includedir = $(pkgincludedir)/loc_ds

#Create and Install libraries
lib_LTLIBRARIES = libloc_ds_api.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-ds.pc
EXTRA_DIST = $(pkgconfig_DATA)
