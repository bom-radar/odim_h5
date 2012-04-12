/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "error.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include "config.h"

using namespace rainfields::hdf;

error::error(hid_t loc, failure_type ft, hdf_object_type ht, const char* name)
  : type_(ft)
  , target_(ht)
  , target_name_(name)
{
  char buf[512];
  if (   loc >= 0
      && H5Iget_name(loc, buf, 512) > 0)
  {
    buf[511] = '\0';
    location_.assign(buf);
  }
}

error::error(hid_t loc, failure_type ft, hdf_object_type ht, const std::string& name)
  : type_(ft)
  , target_(ht)
  , target_name_(name)
{
  char buf[512];
  if (   loc >= 0
      && H5Iget_name(loc, buf, 512) > 0)
  {
    buf[511] = '\0';
    location_.assign(buf);
  }
}

error::~error() RAINFIELDS_NOTHROW
{

}

const char* error::what() const RAINFIELDS_NOTHROW
{
  static const char* errs[] = 
  {
      "failed to create"
    , "failed to open"
    , "failed to read"
    , "failed to write"
    , "failed to delete"
    , "type mismatch in "
    , "size mismatch in "
    , "overflow in "
    , "bad value in "
  };
  static const char* objs[] = 
  {
      "file"
    , "group"
    , "type"
    , "dataspace"
    , "attribute"
    , "property_list"
    , "dataset"
  };

  char buf[512];
  if (location_.empty())
    if (target_name_.empty())
      snprintf(buf, 512, "rainhdf: %s %s", errs[type_], objs[target_]);
    else
      snprintf(buf, 512, "rainhdf: %s %s '%s'", errs[type_], objs[target_], target_name_.c_str());
  else
    if (target_name_.empty())
      snprintf(buf, 512, "rainhdf: %s %s at %s", errs[type_], objs[target_], location_.c_str());
    else
      snprintf(
            buf
          , 512
          , "rainhdf: %s %s '%s' at %s"
          , errs[type_]
          , objs[target_]
          , target_name_.c_str()
          , location_.c_str());
  buf[511] = '\0';

  description_.assign(buf);
  return description_.c_str();
}


