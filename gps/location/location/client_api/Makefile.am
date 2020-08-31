AM_CFLAGS = \
    -DDEBUG \
    -I src/ \
    -I inc/ \
    $(GPSUTILS_CFLAGS) \
    -std=c++11

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -std=c++11

requiredlibs = \
    $(GPSUTILS_LIBS)

h_sources = \
    src/LocationClientApiImpl.h \
    inc/LocationClientApi.h

c_sources = \
    src/LocationClientApiImpl.cpp \
    src/LocationClientApi.cpp

liblocation_client_api_la_SOURCES = \
    $(c_sources) $(h_sources)
liblocation_client_api_la_h_sources = $(h_sources)

######################
# Build location_client_api
######################

if USE_GLIB
liblocation_client_api_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
liblocation_client_api_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
liblocation_client_api_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
liblocation_client_api_la_CFLAGS = $(AM_CFLAGS)
liblocation_client_api_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
liblocation_client_api_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

if USE_EXTERNAL_AP
AM_CFLAGS += $(LOCSOCKET_CFLAGS)
requiredlibs += $(LOCSOCKET_LIBS)
else
AM_CFLAGS += $(DIAGIFACE_CFLAGS)
AM_CFLAGS += $(DIAG_CFLAGS)
requiredlibs += $(DIAGIFACE_LIBS)
requiredlibs += $(DIAG_LIBS)
h_sources += src/LocationClientApiLog.h
c_sources += src/LocationClientApiUtils.cpp
endif

liblocation_client_api_la_LIBADD = $(requiredlibs) -lstdc++ -ldl

#Create and Install libraries
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = liblocation_client_api.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = location-client-api.pc
EXTRA_DIST = $(pkgconfig_DATA)
