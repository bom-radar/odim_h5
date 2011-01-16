#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainHDF)
#
# Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

# Package config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rainhdf.pc

# Core library
lib_LTLIBRARIES = librainhdf.la

nobase_include_HEADERS = \
  rainhdf/base.h \
  rainhdf/error.h \
  rainhdf/raii.h \
  rainhdf/rainhdf.h \
  rainhdf/util.h \
  rainhdf/volume.h

librainhdf_la_SOURCES = \
  rainhdf/base.cc \
  rainhdf/error.cc \
  rainhdf/util.cc \
  rainhdf/volume.cc
librainhdf_la_CPPFLAGS = -std=c++0x $(DEPS_CFLAGS)
librainhdf_la_LIBADD = $(DEPS_LIBS)
librainhdf_la_LDFLAGS = -version-info @SHARED_LIB_VERSION@

# Test application (TEMP)
bin_PROGRAMS = test
test_SOURCES = test.cc
test_CPPFLAGS = -std=c++0x $(DEPS_CFLAGS)
test_LDADD = librainhdf.la -lhdf5

