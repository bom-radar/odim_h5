#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

# default to a release build (must be before project())
if (DEFINED CMAKE_BUILD_TYPE)
  set(DEF_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
  set(DEF_BUILD_TYPE Release)
endif()
set(CMAKE_BUILD_TYPE ${DEF_BUILD_TYPE} CACHE STRING "Build type: <none>|Debug|Release|RelWithDebInfo|MinSizeRel")

# initialize the project
project(rainhdf)

# package versioning
set(RAINHDF_VERSION_MAJOR 1)
set(RAINHDF_VERSION_MINOR 1)
set(RAINHDF_VERSION_PATCH 2)
set(RAINHDF_VERSION ${RAINHDF_VERSION_MAJOR}.${RAINHDF_VERSION_MINOR}.${RAINHDF_VERSION_PATCH})
set(RAINHDF_BUGREPORT "m.curtis@bom.gov.au")
add_definitions(-DRAINHDF_VERSION="${RAINHDF_VERSION}")
add_definitions(-DRAINHDF_BUGREPORT="${RAINHDF_BUGREPORT}")

# enable C++11 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# find the HDF5 library
find_package(HDF5 1.8.8 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
add_definitions(${HDF5_DEFINITIONS})
set(API_DEPS "${API_DEPS} hdf5 >= 1.8.8")

# customizable installation locations
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if (WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/rainhdf)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# convert relative paths to absolute
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if (NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# setup include directories
include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_BINARY_DIR}/src")

# add sub directories
add_subdirectory(src)
add_subdirectory(tests)

# create pkg-config file
configure_file(rainhdf.pc.in "${PROJECT_BINARY_DIR}/rainhdf.pc" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/rainhdf.pc" DESTINATION "${INSTALL_LIB_DIR}/pkgconfig")

# boilerplate to generate our config and version cmake files

# add all targets to build-tree export set
export(TARGETS rainhdf FILE "${PROJECT_BINARY_DIR}/rainhdf-targets.cmake")

# export package for use from build-tree
export(PACKAGE rainhdf)

# create config files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(rainhdf-config.cmake.in "${PROJECT_BINARY_DIR}/rainhdf-config.cmake" @ONLY)
# ... for install tree
set(CONF_INCLUDE_DIRS "\${RAINHDF_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(rainhdf-config.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/rainhdf-config.cmake" @ONLY)
# ... for both
configure_file(rainhdf-config-version.cmake.in "${PROJECT_BINARY_DIR}/rainhdf-config-version.cmake" @ONLY)

# install the config files
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/rainhdf-config.cmake"
  "${PROJECT_BINARY_DIR}/rainhdf-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# install the export set for use with the install-tree
install(EXPORT rainhdf-targets DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

