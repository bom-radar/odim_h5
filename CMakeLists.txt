#-------------------------------------------------------------------------------
# ODIM (HDF5 format) Support Library
#
# Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

# version number used for shared library
#  - major -> update when breaking API
#  - minor -> update when breaking ABI - users only need to re-link
#  - patch -> update when no-relink is required (ie: self-contained inside .so)
set(ODIM_H5_VERSION_MAJOR 1)
set(ODIM_H5_VERSION_MINOR 0)
set(ODIM_H5_VERSION_PATCH 0)
set(ODIM_H5_VERSION "${ODIM_H5_VERSION_MAJOR}.${ODIM_H5_VERSION_MINOR}.${ODIM_H5_VERSION_PATCH}")

# default to a release build (must be before project())
if (DEFINED CMAKE_BUILD_TYPE)
  set(DEF_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
  set(DEF_BUILD_TYPE Release)
endif()
set(CMAKE_BUILD_TYPE ${DEF_BUILD_TYPE} CACHE STRING "Build type: <none>|Debug|Release|RelWithDebInfo|MinSizeRel")

# initialize the project
project(odim_h5)

# default locations based on system configuration
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if (LIB64)
  set(DEF_INSTALL_LIB_DIR lib64)
else()
  set(DEF_INSTALL_LIB_DIR lib)
endif()
if (WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR ${DEF_INSTALL_LIB_DIR}/cmake/odim_h5)
endif()

# customizable installation locations
set(INSTALL_LIB_DIR ${DEF_INSTALL_LIB_DIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# convert relative paths to absolute
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if (NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# external dependencies
find_package(HDF5 1.8.14 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
add_definitions(${HDF5_DEFINITIONS})
set(API_DEPS "${API_DEPS} hdf5 >= 1.8.14")

# extract sourcee tree version information from git
find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --dirty
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE ODIM_H5_RELEASE_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
endif()
if(NOT DEFINED ODIM_H5_RELEASE_TAG OR "${ODIM_H5_RELEASE_TAG}" STREQUAL "")
  set(ODIM_H5_RELEASE_TAG "unknown-version")
endif()
add_definitions("-DODIM_H5_RELEASE_TAG=\"${ODIM_H5_RELEASE_TAG}\"")

# ensure C++11 support is turned on
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# set a high warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wno-unused-parameter")

# build our library
add_library(odim_h5 SHARED odim_h5.h odim_h5.cc)
target_link_libraries(odim_h5 ${HDF5_LIBRARIES})
set_target_properties(odim_h5 PROPERTIES VERSION ${ODIM_H5_VERSION})
set_target_properties(odim_h5 PROPERTIES PUBLIC_HEADER odim_h5.h)
install(TARGETS odim_h5
  EXPORT odim_h5-targets
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT dev)

# create pkg-config file
configure_file(odim_h5.pc.in "${PROJECT_BINARY_DIR}/odim_h5.pc" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/odim_h5.pc" DESTINATION "${INSTALL_LIB_DIR}/pkgconfig")

# boilerplate to generate our config and version cmake files

# create config files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "\${ODIM_H5_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(odim_h5-config.cmake.in "${PROJECT_BINARY_DIR}/odim_h5-config.cmake" @ONLY)
configure_file(odim_h5-config-version.cmake.in "${PROJECT_BINARY_DIR}/odim_h5-config-version.cmake" @ONLY)

# install the config files
install(FILES
  "${PROJECT_BINARY_DIR}/odim_h5-config.cmake"
  "${PROJECT_BINARY_DIR}/odim_h5-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# install the export set for use with the install-tree
install(EXPORT odim_h5-targets DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
