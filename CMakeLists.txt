#-------------------------------------------------------------------------------
# ODIM (HDF5 format) Support Library
#
# Copyright 2016 Commonwealth of Australia, Bureau of Meteorology
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)

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

# setup standard install locations
include(GNUInstallDirs)

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

# require C++11 or better, disable compiler extensions by default
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set a high warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wno-unused-parameter")

# build our library
add_library(odim_h5 SHARED odim_h5.h odim_h5.cc)
target_link_libraries(odim_h5 ${HDF5_LIBRARIES})
set_target_properties(odim_h5 PROPERTIES VERSION ${ODIM_H5_VERSION})
set_target_properties(odim_h5 PROPERTIES PUBLIC_HEADER odim_h5.h)
install(TARGETS odim_h5
  EXPORT odim_h5-targets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT runtime
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT devel)

# create pkg-config file
configure_file(odim_h5.pc.in "${PROJECT_BINARY_DIR}/odim_h5.pc" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/odim_h5.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig" COMPONENT devel)

# create cmake package config files
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/odim_h5" "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
configure_file(odim_h5-config.cmake.in "${PROJECT_BINARY_DIR}/odim_h5-config.cmake" @ONLY)
configure_file(odim_h5-config-version.cmake.in "${PROJECT_BINARY_DIR}/odim_h5-config-version.cmake" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}/odim_h5-config.cmake"
  "${PROJECT_BINARY_DIR}/odim_h5-config-version.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/odim_h5" COMPONENT devel)

# install the export set for use with the install-tree
install(EXPORT odim_h5-targets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/odim_h5" COMPONENT devel)

# build installation packages
set(CPACK_PACKAGE_VENDOR "Australian Government, Bureau of Meteorology")
set(CPACK_PACKAGE_VERSION_MAJOR ${ODIM_H5_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ODIM_H5_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${ODIM_H5_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ODIM (HDF5 format) Support Library")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/description")
set(CPACK_API_DEPS ${API_DEPS})
set(CPACK_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(CPACK_COMPONENT_DEVEL_DEPENDS runtime)
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_SOURCE_DIR}/CPackOptions.cmake")
include(CPack)
