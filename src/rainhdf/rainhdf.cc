/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "rainhdf.h"

const char* rainfields::hdf::package_name()
{
  return "rainhdf";
}

const char* rainfields::hdf::package_version()
{
  return RAINHDF_VERSION;
}

const char* rainfields::hdf::package_support()
{
  return RAINHDF_BUGREPORT;
}

