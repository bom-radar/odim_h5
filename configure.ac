#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

# initialization and sanity checks
AC_PREREQ(2.59)
AC_INIT(rainhdf, 1.0.0, [m.curtis@bom.gov.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/rainhdf/rainhdf.h])
AC_CONFIG_HEADERS([config.h])
AC_REVISION([Revision: 1.0.0])
AM_INIT_AUTOMAKE([foreign -Wall])

# setup the shared library version
AC_SUBST([SHARED_LIB_VERSION], ["1:0:0"]) # current:revision:age

# external library versions
pkg_rainutil="rainutil >= 1.0.0"
# HDF5 has no pkg-config support :(

# check user options
AC_ARG_ENABLE([debug],  AS_HELP_STRING([--enable-debug],  [enable building of debug build code [default=no]]))

# set our language as c++
AC_LANG(C++)

# program checks
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# checks for libraries
PKG_CHECK_MODULES([rainutil], ["$pkg_rainutil"])

# checks for headers
AC_CHECK_HEADER([hdf5.h],,AC_MSG_ERROR(Cannot locate HDF5 headers))

# checks for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AX_CXX_COMPILE_STDCXX_11([], [pass])

# setup debug flags
debug_cppflags="-Wall"
AS_IF([test "x$enable_debug" = "xyes"], [
  debug_cppflags="-Wall -DRAINHDF_DEBUG"
])

# setup flags for core library
AC_SUBST(API_CPPFLAGS, ["$debug_cppflags $rainutil_CFLAGS"])
AC_SUBST(API_LDFLAGS,  ["-version-info $SHARED_LIB_VERSION"])
AC_SUBST(API_LIBS,     ["$rainutil_LIBS -lhdf5"])
AC_SUBST(API_DEPS,     ["$pkg_rainutil"])

# setup flags for the unit test exectuable
AC_SUBST(TEST_CPPFLAGS,   ["$debug_cppflags"])

# okay, generate it all
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/rainhdf/Makefile
  src/test/Makefile
  rainhdf.pc
])
AC_OUTPUT

