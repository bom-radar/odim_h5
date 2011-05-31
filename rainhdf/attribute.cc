/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "attribute.h"

#include "util.h"

using namespace rainhdf;

attribute::attribute(const hid_handle& parent, int index)
  : hnd_this_(hid_attr, parent, index, open)
{
  char buf[64];
  H5Aget_name(hnd_this_, 64, buf);
  name_.assign(buf);
}

attribute::attribute(const hid_handle& parent, const char* name)
  : hnd_this_(hid_attr, parent, name, open)
  , name_(name)
{

}

#if 0
attribute::attribute(const hid_handle& hid, const char* name, data_type type)
  : hid_(hid)
  , name_(name)
  , type_(type)
{
  switch (type_)
  {
  case at_unknown:
    break;
  case at_bool:
    data_.as_bool = false;
    break;
  case at_long:
    data_.as_long = 0L;
    break;
  case at_double:
    data_.as_double = 0.0;
    break;
  case at_string:
    data_.as_string = new std::string();
    break;
  case at_long_array:
    data_.as_long_array = new std::vector<long>();
    break;
  case at_double_array:
    data_.as_double_array = new std::vector<double>();
    break;
  }
}

attribute::~attribute()
{
  switch (type_)
  {
  case at_unknown:
  case at_bool:
  case at_long:
  case at_double:
    break;
  case at_string:
    delete data_.as_string;
    break;
  case at_long_array:
    delete data_.as_long_array;
    break;
  case at_double_array:
    delete data_.as_double_array;
    break;
  }
}

void attribute::load() const
{
  hid_handle attr(hid_attr, hid_, name_.c_str(), open);

  hid_handle space(hid_space, H5Aget_space(attr));
  int size = H5Sget_simple_extent_npoints(space);

  hid_handle type(hid_type, H5Aget_type(attr));
  printf("TYPE of %s is %d\n", name_.c_str(), H5Tget_class(type));
  switch (H5Tget_class(type))
  {
  case H5T_INTEGER:
    if (size > 1)
    {
      type_ = at_long_array;
      data_.as_long_array = new std::vector<long>(size);
      if (H5Aread(attr, H5T_NATIVE_LONG, &(*data_.as_long_array)[0]) < 0)
        throw error(hid_, err_fail_att_read, name_.c_str());
    }
    else
    {
      type_ = at_long;
      if (H5Aread(attr, H5T_NATIVE_LONG, &data_.as_long) < 0)
        throw error(hid_, err_fail_att_read, name_.c_str());
    }
    break;

  case H5T_FLOAT:
    if (size > 1)
    {
      type_ = at_double_array;
      data_.as_double_array = new std::vector<double>(size);
      if (H5Aread(attr, H5T_NATIVE_DOUBLE, &(*data_.as_double_array)[0]) < 0)
        throw error(hid_, err_fail_att_read, name_.c_str());
    }
    else
    {
      type_ = at_double;
      if (H5Aread(attr, H5T_NATIVE_DOUBLE, &data_.as_double) < 0)
        throw error(hid_, err_fail_att_read, name_.c_str());
    }
    break;
     
  case H5T_STRING:
    {
      char buf[BUFSIZ];
      size = H5Tget_size(type);
      size = std::min(BUFSIZ, size);

      if (H5Aread(attr, type, buf) < 0)
        throw error(hid_, err_fail_att_read, name_.c_str());

      if (strcmp(buf, val_true) == 0)
      {
        type_ = at_bool;
        data_.as_bool = true;
      }
      else if (strcmp(buf, val_false) == 0)
      {
        type_ = at_bool;
        data_.as_bool = false;
      }
      else
      {
        type_ = at_string;
        data_.as_string = new std::string(buf, size);
      }
    }
    break;

  default:
    throw error(hid_, err_fail_att_type, name_.c_str());
  }
}

void attribute::set(bool val)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_bool)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val);
  data_.as_bool = val;
}

void attribute::set(long val)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_long)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val);
  data_.as_long = val;
}

void attribute::set(double val)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_double)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val);
  data_.as_double = val;
}

void attribute::set(const char* val)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_string)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val);
  *data_.as_string = val;
}

void attribute::set(const std::string& val)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_string)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val);
  *data_.as_string = val;
}

void attribute::set(const long* val, int size)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_long_array)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val, size);
  data_.as_long_array->assign(val, val + size);
}

void attribute::set(const double* val, int size)
{
  if (type_ == at_unknown)
    load();
  if (type_ != at_double_array)
    throw error(hid_, err_fail_att_type, name_.c_str());
  set_att(hid_, name_.c_str(), val, size);
  data_.as_double_array->assign(val, val + size);
}
#endif

