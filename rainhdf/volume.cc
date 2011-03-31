/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "volume.h"

using namespace RainHDF;

Volume::Volume(
      const std::string &file
    , time_t valid_time
    , double latitude
    , double longitude
    , double height)
  : Product(file, kObj_VolumePolar, valid_time)
  , scan_count_(0)
{
  new_att(hnd_where_, kAtn_Latitude, latitude);
  new_att(hnd_where_, kAtn_Longitude, longitude);
  new_att(hnd_where_, kAtn_Height, height);
}

Volume::Volume(const std::string &file, bool read_only)
  : Product(file, kObj_VolumePolar, read_only)
{
  // Determine the number of scans
  hsize_t obj_count;
  if (H5Gget_num_objs(hnd_this_, &obj_count) < 0)
    throw Error(hnd_this_, "Failed to determine number of objects in group");
  for (obj_count; obj_count > 0; --obj_count)
  {
    char name[32];
    sprintf(name, "%s%d", kGrp_Dataset, (int) obj_count);
    htri_t ret = H5Lexists(hnd_this_, name, H5P_DEFAULT);
    if (ret < 0)
      throw Error(hnd_this_, "Failed to verify existance of group '%s'", name);
    else if (ret)
      break;
  }
  scan_count_ = obj_count;
}


