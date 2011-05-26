/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "data.h"

#include <cmath>
#include <vector>

using namespace rainhdf;

namespace rainhdf
{
  /// Compression factor used to compress data (0 - 9, 9 = max compression)
  static const int default_compression = 6;
}

data::~data()
{

}

data::data(
      const base& parent
    , bool is_quality
    , size_t index
    , rainhdf::quantity quantity
    , const hsize_t* dims
    , const float* raw
    , float no_data
    , float undetect)
  : base(parent, is_quality ? grp_quality : grp_data, index, create)
  , is_quality_(is_quality)
  , quantity_(quantity)
  , gain_(1.0f)
  , offset_(0.0f)
  , size_(dims[0] * dims[1])
{
  // Fill in the 'what' parameters
  new_att(hnd_what_, atn_quantity, quantity_);
  new_att(hnd_what_, atn_gain, gain_);
  new_att(hnd_what_, atn_offset, offset_);
  new_att(hnd_what_, atn_no_data, no_data);
  new_att(hnd_what_, atn_undetect, undetect);

  // Create the HDF dataset
  hid_handle space(hid_space, 2, dims, create);
  hid_handle plist(hid_plist, H5P_DATASET_CREATE, create);
  if (H5Pset_chunk(plist, 2, dims) < 0)
    throw error(hnd_this_, "Failed to set chunk parameters for data");
  if (H5Pset_deflate(plist, default_compression) < 0)
    throw error(hnd_this_, "Failed to set compression level for data");
  hid_handle hnd_data(hid_data, hnd_this_, dat_data, H5T_NATIVE_FLOAT, space, plist, create);
  new_att(hnd_data, atn_class, val_class);
  new_att(hnd_data, atn_image_version, val_image_version);

  // Write the actual image data
  if (H5Dwrite(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, raw) < 0)
    throw error("Failed to write data");
}

data::data(
      const base& parent
    , bool is_quality
    , size_t index
    , rainhdf::quantity quantity
    , const hsize_t* dims)
  : base(parent, is_quality ? grp_quality : grp_data, index, open)
  , is_quality_(is_quality)
  , quantity_(quantity)
  , gain_(get_att<double>(hnd_what_, atn_gain))
  , offset_(get_att<double>(hnd_what_, atn_offset))
  , size_(dims[0] * dims[1])
{

}

void data::read(float* raw, float& no_data, float& undetect) const
{
  hid_handle hnd_data(hid_data, hnd_this_, dat_data, open);

  // Verify the correct dimension to prevent memory corruption
  hid_handle space(hid_space, H5Dget_space(hnd_data));
  if (H5Sget_simple_extent_npoints(space) != size_)
    throw error(hnd_data, "Dataset dimension mismatch");

  // Read the raw data
  no_data = get_att<double>(hnd_what_, atn_no_data);
  undetect = get_att<double>(hnd_what_, atn_undetect);
  if (H5Dread(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, raw) < 0)
    throw error(hnd_this_, "Failed to read data");

  // Convert using gain and offset?
  if (   std::fabs(gain_ - 1.0) > 0.000001
      || std::fabs(offset_) > 0.000001)
  {
    no_data = (no_data * gain_) + offset_;
    undetect = (undetect * gain_) + offset_;
    for (size_t i = 0; i < size_; ++i)
      raw[i] = (raw[i] * gain_) + offset_;
  }
}

void data::write(const float* raw, float no_data, float undetect)
{
  hid_handle hnd_data(hid_data, hnd_this_, dat_data, open);

  // Verify the correct dimension to prevent memory corruption
  hid_handle space(hid_space, H5Dget_space(hnd_data));
  if (H5Sget_simple_extent_npoints(space) != size_)
    throw error(hnd_data, "Dataset dimension mismatch");

  // Do we have to convert the data?
  if (   std::fabs(gain_ - 1.0f) > 0.000001
      || std::fabs(offset_) > 0.000001)
  {
    float gain_mult = 1.0f / gain_;
    no_data = (no_data - offset_) * gain_mult;
    undetect = (undetect - offset_) * gain_mult;

    std::vector<float> vec_data;
    vec_data.reserve(size_);
    vec_data.assign(raw, raw + size_);
    for (std::vector<float>::iterator i = vec_data.begin(); i != vec_data.end(); ++i)
      *i = (*i - offset_) * gain_mult;

    // Write the converted data
    set_att(hnd_what_, atn_no_data, no_data);
    set_att(hnd_what_, atn_undetect, undetect);
    if (H5Dwrite(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec_data[0]) < 0)
      throw error("Failed to write data");
  }
  else
  {
    // Write the raw data
    set_att(hnd_what_, atn_no_data, no_data);
    set_att(hnd_what_, atn_undetect, undetect);
    if (H5Dwrite(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, raw) < 0)
      throw error("Failed to write data");
  }
}

