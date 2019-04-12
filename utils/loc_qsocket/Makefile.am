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
    LocQsocket.h

c_sources = \
    LocQsocket.cpp

libloc_qsocket_la_SOURCES = \
    $(c_sources) $(h_sources)

######################
# Build loc_qsocket
######################

if USE_GLIB
libloc_qsocket_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_qsocket_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_qsocket_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_qsocket_la_CFLAGS = $(AM_CFLAGS)
libloc_qsocket_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_qsocket_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_qsocket_la_LIBADD = $(requiredlibs) -lstdc++

#Create and Install libraries
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libloc_qsocket.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-qsocket.pc
EXTRA_DIST = $(pkgconfig_DATA)

