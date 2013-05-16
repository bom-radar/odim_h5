#-------------------------------------------------------------------------------
# Rainfields ODIM HDF5 Library (rainhdf)
#
# Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
# See COPYING for licensing and warranty details
#-------------------------------------------------------------------------------

add_executable(test_rainhdf test.cc)
target_link_libraries(test_rainhdf rainhdf)

