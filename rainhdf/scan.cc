/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "scan.h"

using namespace RainHDF;

Scan::~Scan()
{

}

Scan::Scan(
      const Base &parent
    , size_t index
    , double elevation
    , size_t azimuth_count
    , size_t range_bin_count
    , size_t first_azimuth
    , double range_start
    , double range_scale
    , time_t start_time
    , time_t end_time)
  : Base(parent, kGrp_Dataset, index, kCreate)
  , azi_count_(azimuth_count)
  , bin_count_(range_bin_count)
{
  new_att(hnd_what_, kAtn_Product, kProd_Scan);
  new_att(hnd_what_, kAtn_StartDate, kAtn_StartTime, start_time);
  new_att(hnd_what_, kAtn_EndDate, kAtn_EndTime, end_time);

  new_att(hnd_where_, kAtn_Elevation, elevation);
  new_att(hnd_where_, kAtn_RangeCount, (long) bin_count_);
  new_att(hnd_where_, kAtn_RangeStart, range_start / 1000.0);
  new_att(hnd_where_, kAtn_RangeScale, range_scale);
  new_att(hnd_where_, kAtn_AzimuthCount, (long) azi_count_);
  new_att(hnd_where_, kAtn_FirstAzimuth, (long) first_azimuth);
}

Scan::Scan(const Base &parent, size_t index)
  : Base(parent, kGrp_Dataset, index, kOpen)
  , azi_count_(get_att<long>(hnd_where_, kAtn_AzimuthCount))
  , bin_count_(get_att<long>(hnd_where_, kAtn_RangeCount))
{
  hsize_t obj_count;
  char name[32];

  // Verify that this dataset is indeed a scan
  if (get_att<ProductType>(hnd_what_, kAtn_Product) != kProd_Scan)
    throw Error(hnd_this_, "Scan product code mismatch");

  // Reserve some space in our data info vector for efficency sake
  if (H5Gget_num_objs(hnd_this_, &obj_count) < 0)
    throw Error(hnd_this_, "Failed to determine number of objects in group");
  data_info_.reserve(obj_count);

  // Check for any data layers
  for (size_t i = 1; true; ++i)
  {
    // Do we have this 'dataX'?
    sprintf(name, "%s%d", kGrp_Data, i);
    htri_t ret = H5Lexists(hnd_this_, name, H5P_DEFAULT);
    if (ret < 0)
      throw Error(hnd_this_, "Failed to verify existence of group '%s'", name);
    if (!ret)
      break;

    // Yes - open it up for inspection
    HID_Handle hnd_data(kHID_Group, hnd_this_, name, kOpen);
    HID_Handle hnd_data_what(kHID_Group, hnd_data, kGrp_What, kOpen);

    // Store some vitals
    DataInfo li;
    li.is_quality = false;
    li.index = i;
    li.quantity = get_att<Quantity>(hnd_data_what, kAtn_Quantity);
    data_info_.push_back(li);
  }

  // Check for any quality layers
  for (size_t i = 1; true; ++i)
  {
    // Do we have this 'dataX'?
    sprintf(name, "%s%d", kGrp_Quality, i);
    htri_t ret = H5Lexists(hnd_this_, name, H5P_DEFAULT);
    if (ret < 0)
      throw Error(hnd_this_, "Failed to verify existence of group '%s'", name);
    if (!ret)
      break;

    // Yes - open it up for inspection
    HID_Handle hnd_data(kHID_Group, hnd_this_, name, kOpen);
    HID_Handle hnd_data_what(kHID_Group, hnd_data, kGrp_What, kOpen);

    // Store some vitals
    DataInfo li;
    li.is_quality = true;
    li.index = i;
    li.quantity = get_att<Quantity>(hnd_data_what, kAtn_Quantity);
    data_info_.push_back(li);
  }
}

Data::Ptr Scan::layer(size_t i)
{
  return Data::Ptr(
      new Data(
          *this, 
          data_info_[i].is_quality,
          data_info_[i].index,
          data_info_[i].quantity,
          &azi_count_));
}

Data::ConstPtr Scan::layer(size_t i) const
{
  return Data::ConstPtr(
      new Data(
          *this, 
          data_info_[i].is_quality,
          data_info_[i].index,
          data_info_[i].quantity,
          &azi_count_));
}

Data::Ptr Scan::layer(Quantity quantity)
{
  for (DataInfoStore_t::iterator i = data_info_.begin(); i != data_info_.end(); ++i)
    if (i->quantity == quantity)
      return Data::Ptr(
          new Data(
              *this, 
              i->is_quality, 
              i->index,
              i->quantity,
              &azi_count_));
  return Data::Ptr(NULL);
}

Data::ConstPtr Scan::layer(Quantity quantity) const
{
  for (DataInfoStore_t::const_iterator i = data_info_.begin(); i != data_info_.end(); ++i)
    if (i->quantity == quantity)
      return Data::ConstPtr(
          new Data(
              *this, 
              i->is_quality, 
              i->index,
              i->quantity,
              &azi_count_));
  return Data::ConstPtr(NULL);
}

Data::Ptr Scan::add_layer(
      Quantity quantity
    , bool is_quality
    , const float *data
    , float no_data
    , float undetect)
{
  DataInfo li;
  li.is_quality = is_quality;
  li.index = 1;
  for (DataInfoStore_t::reverse_iterator i = data_info_.rbegin(); 
       i != data_info_.rend(); 
       ++i)
  {
    if (i->is_quality == li.is_quality)
    {
      li.index = i->index + 1;
      break;
    }
  }
  li.quantity = quantity;

  Data::Ptr ret(
      new Data(
          *this,
          li.is_quality,
          li.index,
          li.quantity,
          &azi_count_,
          data,
          no_data,
          undetect));

  // Must do the push_back last so that exceptions don't screw with our 
  // layer count
  data_info_.push_back(li);
  return ret;
}

