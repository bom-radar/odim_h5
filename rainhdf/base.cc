/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "base.h"

#include <cstring>
#include "config.h"

using namespace rainfields::hdf;

base::base(const std::string& file, create_flag)
  : hnd_this_(hid_file, file.c_str(), create)
  , num_attrs_(0)
{

}

base::base(const base& parent, const char *name, create_flag)
  : hnd_this_(hid_group, parent.hnd_this_, name, create)
  , num_attrs_(0)
{

}

base::base(const base& parent, const char *name, int index, create_flag)
  : hnd_this_(hid_group, parent.hnd_this_, name, index, create)
  , num_attrs_(0)
{

}

base::base(const std::string& file, bool read_only, open_flag)
  : hnd_this_(hid_file, file.c_str(), read_only, open)
  , hnd_what_(hid_group, hnd_this_, grp_what, open, true)
  , hnd_where_(hid_group, hnd_this_, grp_where, open, true)
  , hnd_how_(hid_group, hnd_this_, grp_how, open, true)
  , num_attrs_(hnd_how_ ? H5Aget_num_attrs(hnd_how_) : 0)
{

}

base::base(const base& parent, const char *name, open_flag)
  : hnd_this_(hid_group, parent.hnd_this_, name, open)
  , hnd_what_(hid_group, hnd_this_, grp_what, open, true)
  , hnd_where_(hid_group, hnd_this_, grp_where, open, true)
  , hnd_how_(hid_group, hnd_this_, grp_how, open, true)
  , num_attrs_(hnd_how_ ? H5Aget_num_attrs(hnd_how_) : 0)
{

}

base::base(const base& parent, const char *name, int index, open_flag)
  : hnd_this_(hid_group, parent.hnd_this_, name, index, open)
  , hnd_what_(hid_group, hnd_this_, grp_what, open, true)
  , hnd_where_(hid_group, hnd_this_, grp_where, open, true)
  , hnd_how_(hid_group, hnd_this_, grp_how, open, true)
  , num_attrs_(hnd_how_ ? H5Aget_num_attrs(hnd_how_) : 0)
{

}

base::~base()
{

}

attribute::ptr base::attribute(int i)
{
  return rainfields::hdf::attribute::ptr(new rainfields::hdf::attribute(hnd_how_, i));
}

attribute::const_ptr base::attribute(int i) const
{
  return rainfields::hdf::attribute::const_ptr(new rainfields::hdf::attribute(hnd_how_, i));
}

attribute::ptr base::attribute(const char* name, bool create_if_missing)
{
  if (!hnd_how_)
  {
    if (create_if_missing)
    {
      hnd_how_ = hid_handle(hid_group, hnd_this_, grp_how, create);
      return rainfields::hdf::attribute::ptr(new rainfields::hdf::attribute(hnd_how_, name, true));
    }
    else
    {
      return rainfields::hdf::attribute::ptr();
    }
  }
  else
  {
    htri_t ret = H5Aexists(hnd_how_, name);
    if (ret < 0)
      throw error(hnd_how_, ft_open, ht_attribute, name);
    else if (ret == 0)
    {
      if (create_if_missing)
        return rainfields::hdf::attribute::ptr(new rainfields::hdf::attribute(hnd_how_, name, true));
      else
        return rainfields::hdf::attribute::ptr();
    }
    else
      return rainfields::hdf::attribute::ptr(new rainfields::hdf::attribute(hnd_how_, name, false));
  }
}

attribute::const_ptr base::attribute(const char* name) const
{
  if (!hnd_how_)
    return rainfields::hdf::attribute::const_ptr();
  htri_t ret = H5Aexists(hnd_how_, name);
  if (ret < 0)
    throw error(hnd_how_, ft_open, ht_attribute, name);
  else if (ret == 0)
    return rainfields::hdf::attribute::const_ptr();
  return rainfields::hdf::attribute::const_ptr(new rainfields::hdf::attribute(hnd_how_, name, false));
}

