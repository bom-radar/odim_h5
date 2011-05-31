/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "base.h"

#include <cstring>

using namespace rainhdf;

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
  return rainhdf::attribute::ptr(new rainhdf::attribute(hnd_how_, i));
}

attribute::const_ptr base::attribute(int i) const
{
  return rainhdf::attribute::const_ptr(new rainhdf::attribute(hnd_how_, i));
}


