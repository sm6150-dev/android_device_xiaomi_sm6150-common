# configure.ac -- Autoconf script for gps location-utils
#
# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the loc_socket package version 1.0.0
AC_INIT([loc-socket],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign subdir-objects])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([Makefile.am])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([GPSUTILS], [gps-utils])
AC_SUBST([GPSUTILS_CFLAGS])
AC_SUBST([GPSUTILS_LIBS])

AC_ARG_ENABLE([coretech20],
         [  --enable-coretech20 Enable core tech 2.0],
         [case "${enableval}" in
             yes) coretech20=true ;;
             no)  coretech20=false ;;
             *) AC_MSG_ERROR([bad value ${enableval} for --enable-coretech20]) ;;
         esac],[coretech20=false])

if (test "x${coretech20}" = "xtrue"); then
    AC_MSG_NOTICE([Core tech is 2.0])
    PKG_CHECK_MODULES([QMIFW], [qmi-framework-vendor])
    AC_SUBST([QMIFW_CFLAGS])
    AC_SUBST([QMIFW_LIBS])
else
    AC_MSG_NOTICE([Core tech is 1.0])
    PKG_CHECK_MODULES([QMIFW], [qmi-framework])
    AC_SUBST([QMIFW_CFLAGS])
    AC_SUBST([QMIFW_LIBS])
fi

AC_ARG_WITH([kernel-qrtr],
       AC_HELP_STRING([--with-kernel-qrtr],
          [enable kernel qrtr, kernel supports qrtr]))
AC_ARG_ENABLE([kernel-qrtr],
         [  --enable-kernel-qrtr kernel qrtr is enabled],
         [case "${enableval}" in
             yes) kernelqrtr=true ;;
             no)  kernelqrtr=false ;;
             *) AC_MSG_ERROR([bad value ${enableval} for --enable-kernel-qrtr]) ;;
         esac],[kernelqrtr=false])

AM_CONDITIONAL(USE_QSOCKET, test "x${kernelqrtr}" = "xfalse")

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, building HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GLib >= 2.16 is required))
        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_CONFIG_FILES([ \
        Makefile \
        loc-socket.pc
        ])

AC_OUTPUT
