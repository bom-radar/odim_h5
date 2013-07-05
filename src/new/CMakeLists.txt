#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

set(lib_HDR
  rainhdf.h
  )

set(lib_SRC
  rainhdf.cc
  )

# build the rainhdf library
add_library(new SHARED ${lib_HDR} ${lib_SRC})
target_link_libraries(new ${HDF5_LIBRARIES})
set_target_properties(new PROPERTIES VERSION ${RAINHDF_VERSION})
set_target_properties(new PROPERTIES PUBLIC_HEADER "${lib_HDR}")

# install the library
install(TARGETS new
  EXPORT new-targets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/rainhdf"
    COMPONENT dev)

