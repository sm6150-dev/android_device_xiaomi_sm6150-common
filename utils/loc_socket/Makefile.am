AM_CFLAGS = \
    -DDEBUG \
    -I src/ \
    -I inc/ \
    $(QMIFW_CFLAGS) \
    $(GPSUTILS_CFLAGS) \
    -fpermissive \
    -std=c++11

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -std=c++11

requiredlibs = \
    $(QMIFW_LIBS) \
    $(GPSUTILS_LIBS)

h_sources = \
    LocSocket.h

c_sources = \
    LocSocket.cpp

libloc_socket_la_SOURCES = \
    $(c_sources) $(h_sources)

######################
# Build loc_socket
######################

if USE_GLIB
libloc_socket_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_socket_la_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_socket_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_socket_la_CFLAGS = $(AM_CFLAGS)
libloc_socket_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
libloc_socket_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_socket_la_LIBADD = $(requiredlibs) -lstdc++

#Create and Install libraries
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libloc_socket.la

library_includedir = $(pkgincludedir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = loc-socket.pc
EXTRA_DIST = $(pkgconfig_DATA)

