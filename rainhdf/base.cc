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
  : parent_(NULL)
  , hnd_this_(hid_file, file.c_str(), create)
  , hnd_what_(hid_group, hnd_this_, grp_what, create)
  , hnd_where_(hid_group, hnd_this_, grp_where, create)
  , hnd_how_(hid_group, hnd_this_, grp_how, create)
{

}

base::base(const base& parent, const char *name, create_flag)
  : parent_(&parent)
  , hnd_this_(hid_group, parent_->hnd_this_, name, create)
  , hnd_what_(hid_group, hnd_this_, grp_what, create)
  , hnd_where_(hid_group, hnd_this_, grp_where, create)
  , hnd_how_(hid_group, hnd_this_, grp_how, create)
{

}

base::base(const base& parent, const char *name, int index, create_flag)
  : parent_(&parent)
  , hnd_this_(hid_group, parent_->hnd_this_, name, index, create)
  , hnd_what_(hid_group, hnd_this_, grp_what, create)
  , hnd_where_(hid_group, hnd_this_, grp_where, create)
  , hnd_how_(hid_group, hnd_this_, grp_how, create)
{

}

base::base(const std::string& file, bool read_only, open_flag)
  : parent_(NULL)
  , hnd_this_(hid_file, file.c_str(), read_only, open)
  , hnd_what_(hid_group, hnd_this_, grp_what, open, true)
  , hnd_where_(hid_group, hnd_this_, grp_where, open, true)
  , hnd_how_(hid_group, hnd_this_, grp_how, open, true)
{
  // Note presence of any 'how' attributes
  if (hnd_how_)
    check_attribs_presence(hnd_how_, att_flags_);
}

base::base(const base& parent, const char *name, open_flag)
  : parent_(&parent)
  , hnd_this_(hid_group, parent_->hnd_this_, name, open)
  , hnd_what_(hid_group, hnd_this_, grp_what, open, true)
  , hnd_where_(hid_group, hnd_this_, grp_where, open, true)
  , hnd_how_(hid_group, hnd_this_, grp_how, open, true)
{
  // Note presence of any 'how' attributes
  if (hnd_how_)
    check_attribs_presence(hnd_how_, att_flags_);
}

base::base(const base& parent, const char *name, int index, open_flag)
  : parent_(&parent)
  , hnd_this_(hid_group, parent_->hnd_this_, name, index, open)
  , hnd_what_(hid_group, hnd_this_, grp_what, open, true)
  , hnd_where_(hid_group, hnd_this_, grp_where, open, true)
  , hnd_how_(hid_group, hnd_this_, grp_how, open, true)
{
  // Note presence of any 'how' attributes
  if (hnd_how_)
    check_attribs_presence(hnd_how_, att_flags_);
}

base::~base()
{

}

