/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "data.h"

#include <cmath>
#include <vector>

using namespace RainHDF;

namespace RainHDF
{
  /// Compression factor used to compress data (0 - 9, 9 = max compression)
  static const int kDefCompression = 6;
}

Data::~Data()
{

}

Data::Data(
      const Base& parent
    , bool is_quality
    , size_t index
    , Quantity quantity
    , const hsize_t* dims
    , const float* data
    , float no_data
    , float undetect)
  : Base(parent, is_quality ? kGrp_Quality : kGrp_Data, index, kCreate)
  , is_quality_(is_quality)
  , quantity_(quantity)
  , gain_(1.0f)
  , offset_(0.0f)
  , size_(dims[0] * dims[1])
{
  // Fill in the 'what' parameters
  new_att(hnd_what_, kAtn_Quantity, quantity_);
  new_att(hnd_what_, kAtn_Gain, gain_);
  new_att(hnd_what_, kAtn_Offset, offset_);
  new_att(hnd_what_, kAtn_NoData, no_data);
  new_att(hnd_what_, kAtn_Undetect, undetect);

  // Create the HDF dataset
  HID_Handle space(kHID_Space, 2, dims, kCreate);
  HID_Handle plist(kHID_PList, H5P_DATASET_CREATE, kCreate);
  if (H5Pset_chunk(plist, 2, dims) < 0)
    throw Error(hnd_this_, "Failed to set chunk parameters for data");
  if (H5Pset_deflate(plist, kDefCompression) < 0)
    throw Error(hnd_this_, "Failed to set compression level for data");
  HID_Handle hnd_data(kHID_Data, hnd_this_, kDat_Data, H5T_NATIVE_FLOAT, space, plist, kCreate);
  new_att(hnd_data, kAtn_Class, kVal_Class);
  new_att(hnd_data, kAtn_ImageVersion, kVal_ImageVersion);

  // Write the actual image data
  if (H5Dwrite(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
    throw Error("Failed to write data");
}

Data::Data(
      const Base& parent
    , bool is_quality
    , size_t index
    , Quantity quantity
    , const hsize_t* dims)
  : Base(parent, is_quality ? kGrp_Quality : kGrp_Data, index, kOpen)
  , is_quality_(is_quality)
  , quantity_(quantity)
  , gain_(get_att<double>(hnd_what_, kAtn_Gain))
  , offset_(get_att<double>(hnd_what_, kAtn_Offset))
  , size_(dims[0] * dims[1])
{

}

void Data::read(float* data, float& no_data, float& undetect) const
{
  HID_Handle hnd_data(kHID_Data, hnd_this_, kDat_Data, kOpen);

  // Verify the correct dimension to prevent memory corruption
  HID_Handle space(kHID_Space, H5Dget_space(hnd_data));
  if (H5Sget_simple_extent_npoints(space) != size_)
    throw Error(hnd_data, "Dataset dimension mismatch");

  // Read the raw data
  no_data = get_att<double>(hnd_what_, kAtn_NoData);
  undetect = get_att<double>(hnd_what_, kAtn_Undetect);
  if (H5Dread(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
    throw Error(hnd_this_, "Failed to read data");

  // Convert using gain and offset?
  if (   std::fabs(gain_ - 1.0) > 0.000001
      || std::fabs(offset_) > 0.000001)
  {
    no_data = (no_data * gain_) + offset_;
    undetect = (undetect * gain_) + offset_;
    for (size_t i = 0; i < size_; ++i)
      data[i] = (data[i] * gain_) + offset_;
  }
}

void Data::write(const float* data, float no_data, float undetect)
{
  HID_Handle hnd_data(kHID_Data, hnd_this_, kDat_Data, kOpen);

  // Verify the correct dimension to prevent memory corruption
  HID_Handle space(kHID_Space, H5Dget_space(hnd_data));
  if (H5Sget_simple_extent_npoints(space) != size_)
    throw Error(hnd_data, "Dataset dimension mismatch");

  // Do we have to convert the data?
  if (   std::fabs(gain_ - 1.0f) > 0.000001
      || std::fabs(offset_) > 0.000001)
  {
    float gain_mult = 1.0f / gain_;
    no_data = (no_data - offset_) * gain_mult;
    undetect = (undetect - offset_) * gain_mult;

    std::vector<float> vec_data;
    vec_data.reserve(size_);
    vec_data.assign(data, data + size_);
    for (std::vector<float>::iterator i = vec_data.begin(); i != vec_data.end(); ++i)
      *i = (*i - offset_) * gain_mult;

    // Write the converted data
    set_att(hnd_what_, kAtn_NoData, no_data);
    set_att(hnd_what_, kAtn_Undetect, undetect);
    if (H5Dwrite(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec_data[0]) < 0)
      throw Error("Failed to write data");
  }
  else
  {
    // Write the raw data
    set_att(hnd_what_, kAtn_NoData, no_data);
    set_att(hnd_what_, kAtn_Undetect, undetect);
    if (H5Dwrite(hnd_data, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
      throw Error("Failed to write data");
  }
}

