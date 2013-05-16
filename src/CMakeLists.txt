#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

set(lib_HDR
  attribute.h
  base.h
  data.h
  error.h
  gauge_calibration.h
  macros.h
  product.h
  profile.h
  raii.h
  rainhdf.h
  scan.h
  util.h
  vertical_profile.h
  volume.h
  )

set(lib_SRC
  attribute.cc
  base.cc
  data.cc
  error.cc
  gauge_calibration.cc
  product.cc
  profile.cc
  rainhdf.cc
  scan.cc
  util.cc
  vertical_profile.cc
  volume.cc
  )

# build the rainhdf library
add_library(rainhdf SHARED ${lib_HDR} ${lib_SRC})
target_link_libraries(rainhdf ${HDF5_LIBRARIES})
set_target_properties(rainhdf PROPERTIES VERSION ${RAINHDF_VERSION})
set_target_properties(rainhdf PROPERTIES PUBLIC_HEADER "${lib_HDR}")

# install the library
install(TARGETS rainhdf
  EXPORT rainhdf-targets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/rainhdf"
    COMPONENT dev)

