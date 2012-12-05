/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "config.h"
#include "gauge_calibration.h"
#include <cstring>

using namespace rainfields::hdf;

static compound_spec mspec[] =
{
    { "id", HOFFSET(gauge_calibration::gauge, id), H5T_NATIVE_INT }
  , { "latitude", HOFFSET(gauge_calibration::gauge, latitude), H5T_NATIVE_FLOAT }
  , { "longitude", HOFFSET(gauge_calibration::gauge, longitude), H5T_NATIVE_FLOAT }
  , { "precip_gauge", HOFFSET(gauge_calibration::gauge, precip_gauge), H5T_NATIVE_FLOAT }
  , { "precip_radar", HOFFSET(gauge_calibration::gauge, precip_radar), H5T_NATIVE_FLOAT }
  , { "bias", HOFFSET(gauge_calibration::gauge, bias), H5T_NATIVE_FLOAT }
  , { "kalman_beta", HOFFSET(gauge_calibration::gauge, kalman_beta), H5T_NATIVE_DOUBLE }
  , { "kalman_variance", HOFFSET(gauge_calibration::gauge, kalman_variance), H5T_NATIVE_DOUBLE }
  , { "kalman_betap", HOFFSET(gauge_calibration::gauge, kalman_betap), H5T_NATIVE_DOUBLE }
  , { "obs0_samples", HOFFSET(gauge_calibration::gauge, obs_stats[0].samples), H5T_NATIVE_INT }
  , { "obs0_mean", HOFFSET(gauge_calibration::gauge, obs_stats[0].mean), H5T_NATIVE_DOUBLE }
  , { "obs0_m2", HOFFSET(gauge_calibration::gauge, obs_stats[0].m2), H5T_NATIVE_DOUBLE }
  , { "obs0_var", HOFFSET(gauge_calibration::gauge, obs_stats[0].variance), H5T_NATIVE_DOUBLE }
  , { "obs1_samples", HOFFSET(gauge_calibration::gauge, obs_stats[1].samples), H5T_NATIVE_INT }
  , { "obs1_mean", HOFFSET(gauge_calibration::gauge, obs_stats[1].mean), H5T_NATIVE_DOUBLE }
  , { "obs1_m2", HOFFSET(gauge_calibration::gauge, obs_stats[1].m2), H5T_NATIVE_DOUBLE }
  , { "obs1_var", HOFFSET(gauge_calibration::gauge, obs_stats[1].variance), H5T_NATIVE_DOUBLE }
  , { "obs2_samples", HOFFSET(gauge_calibration::gauge, obs_stats[2].samples), H5T_NATIVE_INT }
  , { "obs2_mean", HOFFSET(gauge_calibration::gauge, obs_stats[2].mean), H5T_NATIVE_DOUBLE }
  , { "obs2_m2", HOFFSET(gauge_calibration::gauge, obs_stats[2].m2), H5T_NATIVE_DOUBLE }
  , { "obs2_var", HOFFSET(gauge_calibration::gauge, obs_stats[2].variance), H5T_NATIVE_DOUBLE }
  , { "obs3_samples", HOFFSET(gauge_calibration::gauge, obs_stats[3].samples), H5T_NATIVE_INT }
  , { "obs3_mean", HOFFSET(gauge_calibration::gauge, obs_stats[3].mean), H5T_NATIVE_DOUBLE }
  , { "obs3_m2", HOFFSET(gauge_calibration::gauge, obs_stats[3].m2), H5T_NATIVE_DOUBLE }
  , { "obs3_var", HOFFSET(gauge_calibration::gauge, obs_stats[3].variance), H5T_NATIVE_DOUBLE }
  , { NULL, 0, invalid_hid }
};

static compound_spec fspec[] =
{
    { "id", 0, H5T_STD_I32LE }
  , { "latitude", 4, H5T_IEEE_F32LE }
  , { "longitude", 8, H5T_IEEE_F32LE }
  , { "precip_gauge", 12, H5T_IEEE_F32LE }
  , { "precip_radar", 16, H5T_IEEE_F32LE }
  , { "bias", 20, H5T_IEEE_F32LE }
  , { "kalman_beta", 24, H5T_IEEE_F64LE }
  , { "kalman_variance", 32, H5T_IEEE_F64LE }
  , { "kalman_betap", 40, H5T_IEEE_F64LE }
  , { "obs0_samples", 48, H5T_STD_I32LE }
  , { "obs0_mean", 52, H5T_IEEE_F64LE }
  , { "obs0_m2", 60, H5T_IEEE_F64LE }
  , { "obs0_var", 68, H5T_IEEE_F64LE }
  , { "obs1_samples", 76, H5T_STD_I32LE }
  , { "obs1_mean", 80, H5T_IEEE_F64LE }
  , { "obs1_m2", 88, H5T_IEEE_F64LE }
  , { "obs1_var", 96, H5T_IEEE_F64LE }
  , { "obs2_samples", 104, H5T_STD_I32LE }
  , { "obs2_mean", 108, H5T_IEEE_F64LE }
  , { "obs2_m2", 116, H5T_IEEE_F64LE }
  , { "obs2_var", 124, H5T_IEEE_F64LE }
  , { "obs3_samples", 132, H5T_STD_I32LE }
  , { "obs3_mean", 136, H5T_IEEE_F64LE }
  , { "obs3_m2", 144, H5T_IEEE_F64LE }
  , { "obs3_var", 152, H5T_IEEE_F64LE }
  , { NULL, 0, invalid_hid }
};
const size_t file_type_size = 160;

gauge_calibration::gauge_calibration(
      const std::string& file
    , const std::string& source
    , time_t valid_time
    , double latitude
    , double longitude
    , double height
    , size_t gauge_count)
  : root_(hid_file, file.c_str(), create)
  , memory_type_(hid_type, sizeof(gauge), mspec)
  , gauge_count_(gauge_count)
{
  hsize_t count = gauge_count;
  hid_handle file_type(hid_type, file_type_size, fspec);
  hid_handle space(hid_space, 1, &count, create);
  hid_handle plist(hid_plist, H5P_DATASET_CREATE, create);
  gauges_ = hid_handle(hid_data, root_, "gauges", file_type, space, plist, create);

  new_att(root_, atn_source, source);
  new_att(root_, atn_date, atn_time, valid_time);
  new_att(root_, atn_latitude, latitude);
  new_att(root_, atn_longitude, longitude);
  new_att(root_, atn_height, height);
}

gauge_calibration::gauge_calibration(const std::string& file, bool read_only)
  : root_(hid_file, file.c_str(), read_only, open)
  , memory_type_(hid_type, sizeof(gauge), mspec)
  , gauges_(hid_data, root_, "gauges", open)
  , gauge_count_(H5Sget_simple_extent_npoints(hid_handle(hid_space, gauges_, false, open)))
{

}

void gauge_calibration::flush() const
{
  if (H5Fflush(root_, H5F_SCOPE_LOCAL) < 0)
    throw error(root_, ft_write, ht_file, "H5Fflush failed");
}

void gauge_calibration::read_gauges(gauge* gauges) const
{
  if (H5Dread(gauges_, memory_type_, H5S_ALL, H5S_ALL, H5P_DEFAULT, gauges) < 0)
    throw error(gauges_, ft_read, ht_dataset);
}

void gauge_calibration::write_gauges(const gauge* gauges)
{
  if (H5Dwrite(gauges_, memory_type_, H5S_ALL, H5S_ALL, H5P_DEFAULT, gauges) < 0)
    throw error(gauges_, ft_write, ht_dataset);
}

