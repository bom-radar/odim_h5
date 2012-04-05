#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

# initialization and sanity checks
AC_INIT(rainhdf, 0.0.5, [m.curtis@bom.gov.au])
AC_CONFIG_HEADERS([rainhdf/config.h])
SHARED_LIB_VERSION="0:5:0" # current:revision:age
AM_INIT_AUTOMAKE([foreign -Wall])
AC_PREREQ(2.53)
AC_CONFIG_SRCDIR(rainhdf/rainhdf.h)

# language checks
AC_LANG(C++)
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL

# libraries and headers
AC_HEADER_STDC
AC_HEADER_TIME

# verify presence of HDF5 library
AC_CHECK_HEADER([hdf5.h],,AC_MSG_ERROR(Cannot locate HDF5 headers))

# external library versions
pkg_rainutil="rainutil >= 0.0.1"
pkg_rainutil_cpp03="rainutil_cpp03 >= 0.0.1"

AC_SUBST(SHARED_LIB_VERSION)

# do we want to enable the RAINHDF_DEBUG code?
debug_cppflags="-Wall"
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [enable building of debug build code]))
AS_IF([test "x$enable_debug" = "xyes"], [
  debug_cppflags="-Wall -DRAINHDF_DEBUG"
])

# do we want to enable C++03 support? (yes by default)
AC_ARG_ENABLE([cpp03], AS_HELP_STRING([--disable-cpp03], [disable building C++03 version of library]), [], [enable_cpp03=yes])
AM_CONDITIONAL([BUILD_CPP03], [test "x$enable_cpp03" = "xyes"])
AS_IF([test "x$enable_cpp03" = "xyes"], [
  API03_DEPS="$pkg_rainutil_cpp03"
  AC_SUBST(API03_DEPS)
  PKG_CHECK_MODULES([API03], ["$API03_DEPS"])
  AC_SUBST(API03_CPPFLAGS, ["$API03_CPPFLAGS $debug_cppflags"])
  AC_SUBST(API03_CXXFLAGS, ["$API03_CPPFLAGS"])
])

# do we want to enable C++11 support? (yes by default)
AC_ARG_ENABLE([cpp11], AS_HELP_STRING([--disable-cpp11], [disable building C++11 version of library]), [], [enable_cpp11=yes])
AM_CONDITIONAL([BUILD_CPP11], [test "x$enable_cpp11" = "xyes"])
AS_IF([test "x$enable_cpp11" = "xyes"], [
  API11_DEPS="$pkg_rainutil"
  AC_SUBST(API11_DEPS)
  PKG_CHECK_MODULES([API11], ["$API11_DEPS"])
  AC_SUBST(API11_CPPFLAGS, ["$API11_CPPFLAGS $debug_cppflags"])
  AC_SUBST(API11_CXXFLAGS, ["$API11_CXXFLAGS"])
])

# okay, generate it all
AC_CONFIG_FILES([Makefile rainhdf.pc rainhdf_cpp03.pc])
AC_OUTPUT

