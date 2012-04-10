/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "volume.h"
#include "config.h"

using namespace rainfields::hdf;

volume::volume(
      const std::string& file
    , const std::string& source
    , time_t valid_time
    , double latitude
    , double longitude
    , double height)
  : product(file, ot_volume_polar, source, valid_time)
  , scan_count_(0)
{
  check_create_where();
  new_att(hnd_where_, atn_latitude, latitude);
  new_att(hnd_where_, atn_longitude, longitude);
  new_att(hnd_where_, atn_height, height);
}

volume::volume(const std::string& file, bool read_only)
  : product(file, ot_volume_polar, read_only)
{
  // Determine the number of scans
  hsize_t obj_count;
  if (H5Gget_num_objs(hnd_this_, &obj_count) < 0)
    throw error(hnd_this_, ft_read, ht_group);
  for (/*empty*/; obj_count > 0; --obj_count)
  {
    char name[32];
    sprintf(name, "%s%d", grp_dataset, (int) obj_count);
    htri_t ret = H5Lexists(hnd_this_, name, H5P_DEFAULT);
    if (ret < 0)
      throw error(hnd_this_, ft_open, ht_group, name);
    else if (ret)
      break;
  }
  scan_count_ = obj_count;
}


