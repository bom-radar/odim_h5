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
  , { "rainfall_gauge", HOFFSET(gauge_calibration::gauge, rainfall_gauge), H5T_NATIVE_FLOAT }
  , { "rainfall_radar", HOFFSET(gauge_calibration::gauge, rainfall_radar), H5T_NATIVE_FLOAT }
  , { "bias", HOFFSET(gauge_calibration::gauge, bias), H5T_NATIVE_FLOAT }
  , { "kalman_time", HOFFSET(gauge_calibration::gauge, kalman_time), H5T_NATIVE_LONG }
  , { "kalman_beta", HOFFSET(gauge_calibration::gauge, kalman_beta), H5T_NATIVE_DOUBLE }
  , { "kalman_variance", HOFFSET(gauge_calibration::gauge, kalman_variance), H5T_NATIVE_DOUBLE }
  , { "err_samples_0", HOFFSET(gauge_calibration::gauge, err_samples[0]), H5T_NATIVE_INT }
  , { "err_samples_1", HOFFSET(gauge_calibration::gauge, err_samples[1]), H5T_NATIVE_INT }
  , { "err_samples_2", HOFFSET(gauge_calibration::gauge, err_samples[2]), H5T_NATIVE_INT }
  , { "err_samples_3", HOFFSET(gauge_calibration::gauge, err_samples[3]), H5T_NATIVE_INT }
  , { "err_mean_0", HOFFSET(gauge_calibration::gauge, err_mean[0]), H5T_NATIVE_DOUBLE }
  , { "err_mean_1", HOFFSET(gauge_calibration::gauge, err_mean[1]), H5T_NATIVE_DOUBLE }
  , { "err_mean_2", HOFFSET(gauge_calibration::gauge, err_mean[2]), H5T_NATIVE_DOUBLE }
  , { "err_mean_3", HOFFSET(gauge_calibration::gauge, err_mean[3]), H5T_NATIVE_DOUBLE }
  , { "err_m2_0", HOFFSET(gauge_calibration::gauge, err_m2[0]), H5T_NATIVE_DOUBLE }
  , { "err_m2_1", HOFFSET(gauge_calibration::gauge, err_m2[1]), H5T_NATIVE_DOUBLE }
  , { "err_m2_2", HOFFSET(gauge_calibration::gauge, err_m2[2]), H5T_NATIVE_DOUBLE }
  , { "err_m2_3", HOFFSET(gauge_calibration::gauge, err_m2[3]), H5T_NATIVE_DOUBLE }
  , { "err_var_0", HOFFSET(gauge_calibration::gauge, err_variance[0]), H5T_NATIVE_DOUBLE }
  , { "err_var_1", HOFFSET(gauge_calibration::gauge, err_variance[1]), H5T_NATIVE_DOUBLE }
  , { "err_var_2", HOFFSET(gauge_calibration::gauge, err_variance[2]), H5T_NATIVE_DOUBLE }
  , { "err_var_3", HOFFSET(gauge_calibration::gauge, err_variance[3]), H5T_NATIVE_DOUBLE }
  , { NULL, 0, invalid_hid }
};

static compound_spec fspec[] =
{
    { "id", 0, H5T_STD_I32LE }
  , { "latitude", 4, H5T_IEEE_F32LE }
  , { "longitude", 8, H5T_IEEE_F32LE }
  , { "rainfall_gauge", 12, H5T_IEEE_F32LE }
  , { "rainfall_radar", 16, H5T_IEEE_F32LE }
  , { "bias", 20, H5T_IEEE_F32LE }
  , { "kalman_time", 24, H5T_STD_I64LE }
  , { "kalman_beta", 32, H5T_IEEE_F64LE }
  , { "kalman_variance", 40, H5T_IEEE_F64LE }
  , { "err_samples_0", 48, H5T_IEEE_F64LE }
  , { "err_samples_1", 56, H5T_IEEE_F64LE }
  , { "err_samples_2", 64, H5T_IEEE_F64LE }
  , { "err_samples_3", 72, H5T_IEEE_F64LE }
  , { "err_mean_0", 80, H5T_IEEE_F64LE }
  , { "err_mean_1", 88, H5T_IEEE_F64LE }
  , { "err_mean_2", 96, H5T_IEEE_F64LE }
  , { "err_mean_3", 104, H5T_IEEE_F64LE }
  , { "err_m2_0", 112, H5T_IEEE_F64LE }
  , { "err_m2_1", 120, H5T_IEEE_F64LE }
  , { "err_m2_2", 128, H5T_IEEE_F64LE }
  , { "err_m2_3", 136, H5T_IEEE_F64LE }
  , { "err_var_0", 144, H5T_IEEE_F64LE }
  , { "err_var_1", 152, H5T_IEEE_F64LE }
  , { "err_var_2", 160, H5T_IEEE_F64LE }
  , { "err_var_3", 168, H5T_IEEE_F64LE }
  , { NULL, 0, invalid_hid }
};
const size_t file_type_size = 176;

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

