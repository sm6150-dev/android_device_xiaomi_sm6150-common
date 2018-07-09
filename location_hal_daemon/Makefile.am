AM_CFLAGS = \
    -DDEBUG \
    -I./ \
    $(LOCCORE_CFLAGS) \
    $(GPSUTILS_CFLAGS) \
    -std=c++11

requiredlibs = \
    $(LOCATIONAPI_LIBS) \
    $(GPSUTILS_LIBS)

h_sources = \
    LocationApiMsg.h \
    LocHalDaemonIPCReceiver.h \
    LocHalDaemonIPCSender.h \
    LocHalDaemonClientHandler.h \
    LocationApiService.h

c_sources = \
    LocHalDaemonIPCReceiver.cpp \
    LocHalDaemonIPCSender.cpp \
    LocHalDaemonClientHandler.cpp \
    LocationApiService.cpp \
    main.cpp

if POWERMANAGER_ENABLED
AM_CFLAGS += $(PM_CFLAGS) -DPOWERMANAGER_ENABLED
c_sources += PowerEvtHandler.cpp
requiredlibs += $(PM_LIBS)
endif

location_hal_daemon_SOURCES = \
    $(c_sources) $(h_sources)

######################
# Build location_hal_daemon
######################

if USE_GLIB
location_hal_daemon_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
location_hal_daemon_LDFLAGS = -lstdc++ -g -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared
location_hal_daemon_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
location_hal_daemon_CFLAGS = $(AM_CFLAGS)
location_hal_daemon_LDFLAGS = -Wl,-z,defs -lpthread -shared
location_hal_daemon_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

location_hal_daemon_LDADD = $(requiredlibs) -lcutils -ldl

bin_PROGRAMS = location_hal_daemon

library_include_HEADERS = $(h_sources)
library_includedir = $(pkgincludedir)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = location-hal-daemon.pc
EXTRA_DIST = $(pkgconfig_DATA)

