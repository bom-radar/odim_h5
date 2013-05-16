/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "attribute.h"
#include "util.h"
#include <cstring>

using namespace rainfields::hdf;

attribute::attribute(const hid_handle& parent, int index)
  : parent_(parent)
  , creating_(false)
  , type_(at_unknown)
{
  hid_handle attr(hid_attr, parent_, index, open);

  char buf[64];
  H5Aget_name(attr, 64, buf);
  name_.assign(buf);

  // Might as well get the type now since the attribute is already open
  determine_type(attr);
}

attribute::attribute(const hid_handle& parent, const char* name, bool creating)
  : parent_(parent)
  , name_(name)
  , creating_(creating)
  , type_(at_unknown)
{

}

void attribute::determine_type(const hid_handle& attr) const
{
  hid_handle space(hid_space, attr, true, open);
  size_ = H5Sget_simple_extent_npoints(space);

  hid_handle type(hid_type, attr, true, open);
  switch (H5Tget_class(type))
  {
  case H5T_INTEGER:
    type_ = size_ > 1 ? at_long_array : at_long;
    break;
  case H5T_FLOAT:
    type_ = size_ > 1 ? at_double_array : at_double;
    break;
  case H5T_STRING:
    {
      char buf[BUFSIZ];
      size_ = H5Tget_size(type);
      if (size_ > BUFSIZ)
        throw error(parent_, ft_size_mismatch, ht_attribute, name_);
      if (H5Aread(attr, type, buf) < 0)
        throw error(parent_, ft_read, ht_attribute, name_);

      if (   strcmp(buf, val_true) == 0
          || strcmp(buf, val_false) == 0)
        type_ = at_bool;
      else
        type_ = at_string;
      value_.assign(buf, size_ - 1);
    }
    break;
  default:
    type_ = at_unknown;
    break;
  }
}


attribute::data_type attribute::type() const
{
  // Have we already calculated this?
  if (type_ != at_unknown)
    return type_;

  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  determine_type(attr);
  return type_;
}

void attribute::get(bool& val) const
{
  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  if (type_ == at_unknown)
    determine_type(attr);
  if (type_ != at_bool)
    throw error(parent_, ft_type_mismatch, ht_attribute, name_);
  val = (value_ == val_true);
}

void attribute::get(long& val) const
{
  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  if (type_ == at_unknown)
    determine_type(attr);
  if (type_ != at_long)
    throw error(parent_, ft_type_mismatch, ht_attribute, name_);
  if (H5Aread(attr, H5T_NATIVE_LONG, &val) < 0)
    throw error(parent_, ft_read, ht_attribute, name_);
}

void attribute::get(double& val) const
{
  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  if (type_ == at_unknown)
    determine_type(attr);
  if (type_ != at_double)
    throw error(parent_, ft_type_mismatch, ht_attribute, name_);
  if (H5Aread(attr, H5T_NATIVE_DOUBLE, &val) < 0)
    throw error(parent_, ft_read, ht_attribute, name_);
}

void attribute::get(std::string& val) const
{
  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  if (type_ == at_unknown)
    determine_type(attr);
  if (type_ != at_string)
    throw error(parent_, ft_type_mismatch, ht_attribute, name_);
  val = value_;
}

void attribute::get(std::vector<long>& val) const
{
  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  if (type_ == at_unknown)
    determine_type(attr);
  if (type_ != at_long_array)
    throw error(parent_, ft_type_mismatch, ht_attribute, name_);
  val.resize(size_);
  if (H5Aread(attr, H5T_NATIVE_LONG, &val[0]) < 0)
    throw error(parent_, ft_read, ht_attribute, name_);
}

void attribute::get(std::vector<double>& val) const
{
  hid_handle attr(hid_attr, parent_, name_.c_str(), open);
  if (type_ == at_unknown)
    determine_type(attr);
  if (type_ != at_double_array)
    throw error(parent_, ft_type_mismatch, ht_attribute, name_);
  val.resize(size_);
  if (H5Aread(attr, H5T_NATIVE_DOUBLE, &val[0]) < 0)
    throw error(parent_, ft_read, ht_attribute, name_);
}

void attribute::set(bool val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    value_.assign(val ? val_true : val_false);
    hid_handle type(hid_type, H5T_C_S1, copy);
    if (H5Tset_size(type, value_.size() + 1) < 0)
      throw error(parent_, ft_write, ht_type, name_);
    if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
      throw error(parent_, ft_write, ht_type, name_);
    hid_handle space(hid_space, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), type, space, create);
    type_ = at_bool;
    size_ = value_.size() + 1;
    if (H5Awrite(attr, type, value_.c_str()) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_bool)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    value_.assign(val ? val_true : val_false);
    if (size_ != value_.size() + 1)
    {
      // Size is different, need to delete and recreate
      attr.close();
      if (H5Adelete(parent_, name_.c_str()) < 0)
        throw error(parent_, ft_remove, ht_attribute, name_);
      hid_handle type(hid_type, H5T_C_S1, copy);
      if (H5Tset_size(type, value_.size() + 1) < 0)
        throw error(parent_, ft_write, ht_type, name_);
      if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
        throw error(parent_, ft_write, ht_type, name_);
      hid_handle space(hid_space, create);
      hid_handle attr2(hid_attr, parent_, name_.c_str(), type, space, create);
      type_ = at_bool;
      size_ = value_.size() + 1;
      if (H5Awrite(attr2, type, value_.c_str()) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
    else
    {
      // Exists and is same size, just overwrite
      hid_handle type(hid_type, attr, true, open);
      if (H5Awrite(attr, type, value_.c_str()) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
  }
}

void attribute::set(long val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    hid_handle space(hid_space, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), H5T_STD_I64LE, space, create);
    type_ = at_long;
    if (H5Awrite(attr, H5T_NATIVE_LONG, &val) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_long)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    if (H5Awrite(attr, H5T_NATIVE_LONG, &val) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
}

void attribute::set(double val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    hid_handle space(hid_space, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), H5T_IEEE_F64LE, space, create);
    type_ = at_double;
    if (H5Awrite(attr, H5T_NATIVE_DOUBLE, &val) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_double)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    if (H5Awrite(attr, H5T_NATIVE_DOUBLE, &val) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
}

void attribute::set(const char* val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    value_.assign(val);
    hid_handle type(hid_type, H5T_C_S1, copy);
    if (H5Tset_size(type, value_.size() + 1) < 0)
      throw error(parent_, ft_write, ht_type, name_);
    if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
      throw error(parent_, ft_write, ht_type, name_);
    hid_handle space(hid_space, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), type, space, create);
    type_ = at_string;
    size_ = value_.size() + 1;
    if (H5Awrite(attr, type, value_.c_str()) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_string)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    value_.assign(val);
    if (size_ != value_.size() + 1)
    {
      // Size is different, need to delete and recreate
      attr.close();
      if (H5Adelete(parent_, name_.c_str()) < 0)
        throw error(parent_, ft_remove, ht_attribute, name_);
      hid_handle type(hid_type, H5T_C_S1, copy);
      if (H5Tset_size(type, value_.size() + 1) < 0)
        throw error(parent_, ft_write, ht_type, name_);
      if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
        throw error(parent_, ft_write, ht_type, name_);
      hid_handle space(hid_space, create);
      hid_handle attr2(hid_attr, parent_, name_.c_str(), type, space, create);
      type_ = at_string;
      size_ = value_.size() + 1;
      if (H5Awrite(attr2, type, value_.c_str()) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
    else
    {
      // Exists and is same size, just overwrite
      hid_handle type(hid_type, attr, true, open);
      if (H5Awrite(attr, type, value_.c_str()) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
  }
}

void attribute::set(const std::string& val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    value_.assign(val);
    hid_handle type(hid_type, H5T_C_S1, copy);
    if (H5Tset_size(type, value_.size() + 1) < 0)
      throw error(parent_, ft_write, ht_type, name_);
    if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
      throw error(parent_, ft_write, ht_type, name_);
    hid_handle space(hid_space, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), type, space, create);
    type_ = at_string;
    size_ = value_.size() + 1;
    if (H5Awrite(attr, type, value_.c_str()) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_string)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    value_.assign(val);
    if (size_ != value_.size() + 1)
    {
      // Size is different, need to delete and recreate
      attr.close();
      if (H5Adelete(parent_, name_.c_str()) < 0)
        throw error(parent_, ft_remove, ht_attribute, name_);
      hid_handle type(hid_type, H5T_C_S1, copy);
      if (H5Tset_size(type, value_.size() + 1) < 0)
        throw error(parent_, ft_write, ht_type, name_);
      if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
        throw error(parent_, ft_write, ht_type, name_);
      hid_handle space(hid_space, create);
      hid_handle attr2(hid_attr, parent_, name_.c_str(), type, space, create);
      type_ = at_string;
      size_ = value_.size() + 1;
      if (H5Awrite(attr2, type, value_.c_str()) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
    else
    {
      // Exists and is same size, just overwrite
      hid_handle type(hid_type, attr, true, open);
      if (H5Awrite(attr, type, value_.c_str()) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
  }
}

void attribute::set(const std::vector<long>& val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    hsize_t hs = val.size();
    hid_handle space(hid_space, 1, &hs, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), H5T_STD_I64LE, space, create);
    type_ = at_long_array;
    size_ = val.size();
    if (H5Awrite(attr, H5T_NATIVE_LONG, &val[0]) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_long_array)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    if (size_ != val.size())
    {
      // Size is different, need to delete and recreate
      attr.close();
      if (H5Adelete(parent_, name_.c_str()) < 0)
        throw error(parent_, ft_remove, ht_attribute, name_);
      hsize_t hs = val.size();
      hid_handle space(hid_space, 1, &hs, create);
      hid_handle attr2(hid_attr, parent_, name_.c_str(), H5T_STD_I64LE, space, create);
      type_ = at_long_array;
      size_ = val.size();
      if (H5Awrite(attr2, H5T_NATIVE_LONG, &val[0]) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
    else
    {
      // Exists and is same size, just overwrite
      if (H5Awrite(parent_, H5T_NATIVE_LONG, &val[0]) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
  }
}

void attribute::set(const std::vector<double>& val)
{
  // Do we need possibly need to create it?
  if (type_ == at_unknown && creating_)
  {
    // Need to create
    hsize_t hs = val.size();
    hid_handle space(hid_space, 1, &hs, create);
    hid_handle attr(hid_attr, parent_, name_.c_str(), H5T_IEEE_F64LE, space, create);
    type_ = at_double_array;
    size_ = val.size();
    if (H5Awrite(attr, H5T_NATIVE_DOUBLE, &val[0]) < 0)
      throw error(parent_, ft_write, ht_attribute, name_);
  }
  else
  {
    // Need to load and overwrite
    hid_handle attr(hid_attr, parent_, name_.c_str(), open);

    if (type_ == at_unknown)
      determine_type(attr);
    if (type_ != at_double_array)
      throw error(parent_, ft_type_mismatch, ht_attribute, name_);
    if (size_ != val.size())
    {
      // Size is different, need to delete and recreate
      attr.close();
      if (H5Adelete(parent_, name_.c_str()) < 0)
        throw error(parent_, ft_remove, ht_attribute, name_);
      hsize_t hs = val.size();
      hid_handle space(hid_space, 1, &hs, create);
      hid_handle attr2(hid_attr, parent_, name_.c_str(), H5T_IEEE_F64LE, space, create);
      type_ = at_double_array;
      size_ = val.size();
      if (H5Awrite(attr2, H5T_NATIVE_DOUBLE, &val[0]) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
    else
    {
      // Exists and is same size, just overwrite
      if (H5Awrite(parent_, H5T_NATIVE_DOUBLE, &val[0]) < 0)
        throw error(parent_, ft_write, ht_attribute, name_);
    }
  }
}

