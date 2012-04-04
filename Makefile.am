#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainHDF)
#
# Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

HDR=\
  rainhdf/attribute.h \
  rainhdf/base.h \
  rainhdf/data.h \
  rainhdf/error.h \
  rainhdf/product.h \
  rainhdf/profile.h \
  rainhdf/raii.h \
  rainhdf/rainhdf.h \
  rainhdf/scan.h \
  rainhdf/util.h \
  rainhdf/vertical_profile.h \
  rainhdf/volume.h

SRC=\
  rainhdf/attribute.cc \
  rainhdf/base.cc \
  rainhdf/data.cc \
  rainhdf/error.cc \
  rainhdf/product.cc \
  rainhdf/profile.cc \
  rainhdf/rainhdf.cc \
  rainhdf/scan.cc \
  rainhdf/util.cc \
  rainhdf/vertical_profile.cc \
  rainhdf/volume.cc

nobase_include_HEADERS = $(HDR)

# C++03 compatible library
if BUILD_CPP03
librainhdf_cpp03_la_SOURCES = $(SRC)
librainhdf_cpp03_la_CPPFLAGS = @API03_CPPFLAGS@
librainhdf_cpp03_la_CXXFLAGS = -g @API03_CFLAGS@ @API03_CXXFLAGS@
librainhdf_cpp03_la_LIBADD = @API03_LIBS@
librainhdf_cpp03_la_LDFLAGS = -version-info @SHARED_LIB_VERSION@
MAYBE_CPP03=librainhdf_cpp03.la
MAYBE_PC03=rainhdf_cpp03.pc
endif

# C++11 compatible library
if BUILD_CPP11
librainhdf_la_SOURCES = $(SRC)
librainhdf_la_CPPFLAGS = @API11_CPPFLAGS@
librainhdf_la_CXXFLAGS = -g @API11_CFLAGS@ @API11_CXXFLAGS@
librainhdf_la_LIBADD = @API11_LIBS@
librainhdf_la_LDFLAGS = -version-info @SHARED_LIB_VERSION@
MAYBE_CPP11=librainhdf.la
MAYBE_PC11=rainhdf.pc
endif

# core libraries
lib_LTLIBRARIES = $(MAYBE_CPP03) $(MAYBE_CPP11)

# package config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(MAYBE_PC03) $(MAYBE_PC11)

# make sure Doxyfile gets distributed
dist_noinst_DATA = Doxyfile

# Test application (TEMP)
bin_PROGRAMS = test
test_SOURCES = test.cc
test_CPPFLAGS = -g $(DEPS_CFLAGS)
test_LDADD = librainhdf.la -lhdf5


