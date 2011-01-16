#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainHDF)
#
# Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

# Initialization and sanity checks
AC_INIT(rainhdf, 1.0.0, [m.curtis@bom.gov.au])
SHARED_LIB_VERSION="1:0:0" # current:revision:age
AM_INIT_AUTOMAKE([foreign -Wall])
AC_PREREQ(2.53)
AC_CONFIG_SRCDIR(rainhdf/rainhdf.h)

# Language checks
AC_LANG(C++)
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL

# Libraries and headers
AC_HEADER_STDC
AC_HEADER_TIME

# Verify presence of HDF5 library
AC_CHECK_HEADER([hdf5.h],,AC_MSG_ERROR(Cannot locate HDF5 headers))

AC_SUBST(SHARED_LIB_VERSION)

# Okay, generate it all
AC_CONFIG_FILES([Makefile rainhdf.pc])
AC_OUTPUT

