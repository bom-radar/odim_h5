#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

HDR=\
  attribute.h\
  base.h\
  data.h\
  error.h\
  product.h\
  profile.h\
  raii.h\
  rainhdf.h\
  scan.h\
  util.h\
  vertical_profile.h\
  volume.h

SRC=\
  attribute.cc\
  base.cc\
  data.cc\
  error.cc\
  product.cc\
  profile.cc\
  rainhdf.cc\
  scan.cc\
  util.cc\
  vertical_profile.cc\
  volume.cc

nobase_pkginclude_HEADERS=$(HDR)
librainhdf_la_SOURCES=../../config.h $(SRC)
librainhdf_la_CPPFLAGS=$(AM_CPPFLAGS) @API_CPPFLAGS@
librainhdf_la_LDFLAGS=$(AM_LDFLAGS) @API_LDFLAGS@
librainhdf_la_LIBADD=@API_LIBS@
lib_LTLIBRARIES=librainhdf.la

