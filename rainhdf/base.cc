/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "base.h"

#include <cstring>

using namespace RainHDF;

Base::Base(const std::string& file, CreateFlag)
  : parent_(NULL)
  , hnd_this_(kHID_File, file.c_str(), kCreate)
  , hnd_what_(kHID_Group, hnd_this_, kGrp_What, kCreate)
  , hnd_where_(kHID_Group, hnd_this_, kGrp_Where, kCreate)
  , hnd_how_(kHID_Group, hnd_this_, kGrp_How, kCreate)
{

}

Base::Base(const Base& parent, const char *name, CreateFlag)
  : parent_(&parent)
  , hnd_this_(kHID_Group, parent_->hnd_this_, name, kCreate)
  , hnd_what_(kHID_Group, hnd_this_, kGrp_What, kCreate)
  , hnd_where_(kHID_Group, hnd_this_, kGrp_Where, kCreate)
  , hnd_how_(kHID_Group, hnd_this_, kGrp_How, kCreate)
{

}

Base::Base(const Base& parent, const char *name, int index, CreateFlag)
  : parent_(&parent)
  , hnd_this_(kHID_Group, parent_->hnd_this_, name, index, kCreate)
  , hnd_what_(kHID_Group, hnd_this_, kGrp_What, kCreate)
  , hnd_where_(kHID_Group, hnd_this_, kGrp_Where, kCreate)
  , hnd_how_(kHID_Group, hnd_this_, kGrp_How, kCreate)
{

}

Base::Base(const std::string& file, bool read_only, OpenFlag)
  : parent_(NULL)
  , hnd_this_(kHID_File, file.c_str(), read_only, kOpen)
  , hnd_what_(kHID_Group, hnd_this_, kGrp_What, kOpen, true)
  , hnd_where_(kHID_Group, hnd_this_, kGrp_Where, kOpen, true)
  , hnd_how_(kHID_Group, hnd_this_, kGrp_How, kOpen, true)
{
  // Note presence of any 'how' attributes
  if (hnd_how_)
    check_attribs_presence(hnd_how_, att_flags_);
}

Base::Base(const Base& parent, const char *name, OpenFlag)
  : parent_(&parent)
  , hnd_this_(kHID_Group, parent_->hnd_this_, name, kOpen)
  , hnd_what_(kHID_Group, hnd_this_, kGrp_What, kOpen, true)
  , hnd_where_(kHID_Group, hnd_this_, kGrp_Where, kOpen, true)
  , hnd_how_(kHID_Group, hnd_this_, kGrp_How, kOpen, true)
{
  // Note presence of any 'how' attributes
  if (hnd_how_)
    check_attribs_presence(hnd_how_, att_flags_);
}

Base::Base(const Base& parent, const char *name, int index, OpenFlag)
  : parent_(&parent)
  , hnd_this_(kHID_Group, parent_->hnd_this_, name, index, kOpen)
  , hnd_what_(kHID_Group, hnd_this_, kGrp_What, kOpen, true)
  , hnd_where_(kHID_Group, hnd_this_, kGrp_Where, kOpen, true)
  , hnd_how_(kHID_Group, hnd_this_, kGrp_How, kOpen, true)
{
  // Note presence of any 'how' attributes
  if (hnd_how_)
    check_attribs_presence(hnd_how_, att_flags_);
}

Base::~Base()
{

}

