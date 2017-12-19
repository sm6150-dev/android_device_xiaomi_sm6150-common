AM_CFLAGS = \
    -DDEBUG \
    -I./ \
    $(LOCCORE_CFLAGS) \
    $(GPSUTILS_CFLAGS) \
    $(LOCCLIENTAPI_CFLAGS) \
    -std=c++11

requiredlibs = \
    $(LOCATIONAPI_LIBS) \
    $(GPSUTILS_LIBS)

h_sources = \
    LocHalDaemonIPCReceiver.h \
    LocHalDaemonIPCSender.h \
    LocHalDaemonClientHandler.h \
    LocationApiService.h \
    main.h

c_sources = \
    LocHalDaemonIPCReceiver.cpp \
    LocHalDaemonIPCSender.cpp \
    LocationApiService.cpp \
    main.cpp

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

