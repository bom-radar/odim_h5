/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "vertical_profile.h"
#include "config.h"

using namespace rainfields::hdf;

vertical_profile::vertical_profile(
      const std::string& file
    , const std::string& source
    , time_t valid_time
    , double latitude
    , double longitude
    , double height
    , size_t levels
    , double interval
    , double min_height
    , double max_height)
  : product(file, ot_vertical_profile, source, valid_time)
  , profile_count_(0)
  , levels_(levels)
{
  check_create_where();
  new_att(hnd_where_, atn_latitude, latitude);
  new_att(hnd_where_, atn_longitude, longitude);
  new_att(hnd_where_, atn_height, height);
  new_att(hnd_where_, atn_levels, (long) levels_);
  new_att(hnd_where_, atn_interval, interval);
  new_att(hnd_where_, atn_min_height, min_height);
  new_att(hnd_where_, atn_max_height, max_height);
}
  
vertical_profile::vertical_profile(const std::string& file, bool read_only)
  : product(file, ot_vertical_profile, read_only)
  , levels_((size_t) get_att<long>(hnd_where_, atn_levels))
{
  // Determine the number of profiles
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
  profile_count_ = obj_count;
}


