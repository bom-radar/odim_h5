/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "config.h"
#include "util.h"
#include <cstring>

using namespace rainfields::hdf;

namespace rainfields
{
  const char* enum_traits<object_type>::name = "object_type";
  const char* enum_traits<object_type>::strings[] =
  {
      "PVOL"
    , "CVOL"
    , "SCAN"
    , "RAY"
    , "AZIM"
    , "IMAGE"
    , "COMP"
    , "XSEC"
    , "VP"
    , "PIC"
  };

  const char* enum_traits<product_type>::name = "product_type";
  const char* enum_traits<product_type>::strings[] =
  {
      "SCAN"
    , "PPI"
    , "CAPPI"
    , "PCAPPI"
    , "ETOP"
    , "MAX"
    , "RR"
    , "VIL"
    , "COMP"
    , "VP"
    , "RHI"
    , "XSEC"
    , "VSP"
    , "HSP"
    , "RAY"
    , "AZIM"
    , "QUAL"
  };

  const char* enum_traits<quantity>::name = "quantity";
  const char* enum_traits<quantity>::strings[] =
  {
      "TH"
    , "TV"
    , "DBZH"
    , "DBZV"
    , "ZDR"
    , "RHOHV"
    , "LDR"
    , "PHIDP"
    , "KDP"
    , "SQI"
    , "SNR"
    , "RATE"
    , "ACRR"
    , "HGHT"
    , "VIL"
    , "VRAD"
    , "WRAD"
    , "UWND"
    , "VWND"
    , "BRDR"
    , "QIND"
    , "CLASS"

    , "ff"
    , "dd"
    , "ff_dev"
    , "dd_dev"
    , "n"
    , "dbz"
    , "dbz_dev"
    , "z"
    , "z_dev"
    , "w"
    , "w_dev"
    , "div"
    , "div_dev"
    , "def"
    , "def_dev"
    , "ad"
    , "ad_dev"
    , "chi2"
    , "rhohv"
    , "rhohv_dev"

    , "OCCUL"
    , "ATTEN"
    , "CPROB"
    , "HPROB"

    , "DBZH_CLEAN"
    , "VRAD_CLEAN"

    , "GENERIC"
  };

  const char* enum_traits<method>::name = "method";
  const char* enum_traits<method>::strings[] =
  {
      "NEAREST"
    , "INTERPOL"
    , "AVERAGE"
    , "RANDOM"
    , "MDE"
    , "LATEST"
    , "MAXIMUM"
    , "DOMAIN"
    , "VAD"
    , "VVP"
    , "RGA"
  };

  const char* enum_traits<attrib>::name = "attribute";
  const char* enum_traits<attrib>::strings[] =
  {
      "task"
    , "startepochs"
    , "endepochs"
    , "system"
    , "software"
    , "sw_version"
    , "zr_a"
    , "zr_b"
    , "kr_a"
    , "kr_b"
    , "simulated"
    , "beamwidth"
    , "wavelength"
    , "rpm"
    , "pulsewidth"
    , "RXbandwidth"
    , "lowprf"
    , "highprf"
    , "TXloss"
    , "RXloss"
    , "radomeloss"
    , "antgain"
    , "beamwH"
    , "beamwV"
    , "gasattn"
    , "radconstH"
    , "radconstV"
    , "nomTXpower"
    , "TXpower"
    , "NI"
    , "Vsamples"
    , "azmethod"
    , "binmethod"
    , "elangles"
    , "startazA"
    , "stopazA"
    , "startazT"
    , "stopazT"
    , "angles"
    , "arotation"
    , "camethod"
    , "nodes"
    , "ACCnum"
    , "minrange"
    , "maxrange"
    , "dealiased"
    , "pointaccEL"
    , "pointaccAZ"
    , "malfunc"
    , "radar_msg"
    , "radhoriz"
    , "NEZ"
    , "OUR"
    , "Dclutter"
    , "comment"
    , "SQI"
    , "CSR"
    , "LOG"
    , "VPRCorr"
    , "freeze"
    , "min"
    , "max"
    , "step"
    , "levels"
    , "peakpwr"
    , "avgpwr"
    , "dynrange"
    , "RAC"
    , "BBC"
    , "PAC"
    , "S2N"
    , "polarization"
    , "config_qc"
  };

  namespace hdf
  {
    const char* grp_what = "what";
    const char* grp_where = "where";
    const char* grp_how = "how";
    const char* grp_dataset = "dataset";
    const char* grp_data = "data";
    const char* grp_quality = "quality";

    const char* atn_conventions = "Conventions";
    const char* atn_object = "object";
    const char* atn_version = "version";
    const char* atn_date = "date";
    const char* atn_time = "time";
    const char* atn_source = "source";
    const char* atn_latitude = "lat";
    const char* atn_longitude = "lon";
    const char* atn_height = "height";
    const char* atn_product = "product";
    const char* atn_start_date = "startdate";
    const char* atn_start_time = "starttime";
    const char* atn_end_date = "enddate";
    const char* atn_end_time = "endtime";
    const char* atn_elevation = "elangle";
    const char* atn_first_azimuth = "a1gate";
    const char* atn_range_count = "nbins";
    const char* atn_range_start = "rstart";
    const char* atn_range_scale = "rscale";
    const char* atn_azimuth_count = "nrays";
    const char* atn_quantity = "quantity";
    const char* atn_gain = "gain";
    const char* atn_offset = "offset";
    const char* atn_no_data = "nodata";
    const char* atn_undetect = "undetect";
    const char* atn_class = "CLASS";
    const char* atn_image_version = "IMAGE_VERSION";
    const char* atn_levels = "levels";
    const char* atn_interval = "interval";
    const char* atn_min_height = "minheight";
    const char* atn_max_height = "maxheight";

    const char* dat_data = "data";

    const char* val_true = "True";
    const char* val_false = "False";
    const char* val_conventions = "ODIM_H5/V2_0";
    const char* val_version = "H5rad 2.0";
    const char* val_class = "IMAGE";
    const char* val_image_version = "1.2";
  }
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, bool& val)
{
  char buf[6];
  get_att(hid, name, buf, 6);
  if (strcmp(buf, val_true) == 0)
    val = true;
  else if (strcmp(buf, val_false) == 0)
    val = false;
  else 
    throw error(hid, ft_bad_value, ht_attribute, name);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, long& val)
{
  hid_handle attr(hid_attr, hid, name, open);
  if (H5Aread(attr, H5T_NATIVE_LONG, &val) < 0)
    throw error(hid, ft_read, ht_attribute, name);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, double& val)
{
  hid_handle attr(hid_attr, hid, name, open);
  if (H5Aread(attr, H5T_NATIVE_DOUBLE, &val) < 0)
    throw error(hid, ft_read, ht_attribute, name);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, char* buf, size_t buf_size)
{
  hid_handle attr(hid_attr, hid, name, open);
  hid_handle type(hid_type, attr, true, open);
  if (H5Tget_class(type) != H5T_STRING)
    throw error(hid, ft_type_mismatch, ht_attribute, name);
  if (H5Tget_size(type) > buf_size)
    throw error(hid, ft_size_mismatch, ht_attribute, name);
  if (H5Aread(attr, type, buf) < 0)
    throw error(hid, ft_read, ht_attribute, name);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, std::string& val)
{
  // Use a fixed size buffer to read our string attributes
  static const size_t kAttBufSize = 2048;
  char buf[kAttBufSize];

  hid_handle attr(hid_attr, hid, name, open);
  hid_handle type(hid_type, attr, true, open);
  if (H5Tget_class(type) != H5T_STRING)
    throw error(hid, ft_type_mismatch, ht_attribute, name);
  if (H5Tget_size(type) > kAttBufSize)
    throw error(hid, ft_size_mismatch, ht_attribute, name);
  if (H5Aread(attr, type, buf) < 0)
    throw error(hid, ft_read, ht_attribute, name);

  val.assign(buf);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, long* vals, size_t& size)
{
  hid_handle attr(hid_attr, hid, name, open);

  // Check it's an integer based type
  hid_handle type(hid_type, attr, true, open);
  if (H5Tget_class(type) != H5T_INTEGER)
    throw error(hid, ft_type_mismatch, ht_attribute, name);

  // Check we've got room in our buffer
  hid_handle space(hid_space, attr, true, open);
  size_t real_size = H5Sget_simple_extent_npoints(space);
  if (real_size > size)
    throw error(hid, ft_size_mismatch, ht_attribute, name);

  // Read it in
  size = real_size;
  if (H5Aread(attr, H5T_NATIVE_LONG, vals) < 0)
    throw error(hid, ft_read, ht_attribute, name);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name, double* vals, size_t& size)
{
  hid_handle attr(hid_attr, hid, name, open);

  // Check it's a floating point based type
  hid_handle type(hid_type, attr, true, open);
  if (H5Tget_class(type) != H5T_INTEGER)
    throw error(hid, ft_type_mismatch, ht_attribute, name);

  // Check we've got room in our buffer
  hid_handle space(hid_space, attr, true, open);
  size_t real_size = H5Sget_simple_extent_npoints(space);
  if (real_size > size)
    throw error(hid, ft_size_mismatch, ht_attribute, name);

  // Read it in
  size = real_size;
  if (H5Aread(attr, H5T_NATIVE_DOUBLE, vals) < 0)
    throw error(hid, ft_read, ht_attribute, name);
}

void rainfields::hdf::get_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t& val)
{
  char buf[9+7];
  struct tm tms;

  // Read the attributes into a single string
  get_att(hid, name_date, buf, 9);
  get_att(hid, name_time, &buf[9], 7);
  buf[8] = ' ';

  // Convert back to numerical values
  if (sscanf(
          buf,
          "%04d%02d%02d %02d%02d%02d",
          &tms.tm_year,
          &tms.tm_mon,
          &tms.tm_mday,
          &tms.tm_hour,
          &tms.tm_min,
          &tms.tm_sec) != 6)
    throw error(
          hid
        , ft_bad_value
        , ht_attribute
        , std::string(name_date).append("/").append(name_time));

  tms.tm_year -= 1900;
  tms.tm_mon -= 1;
  tms.tm_wday = 0;
  tms.tm_yday = 0;
  tms.tm_isdst = 0;

  // Convert back to a time_t
  // Note: This function is NOT portable - but there is no single portable way to
  //       perform the inverse of gmtime other than by changing TZ, calling
  //       mktime and then restoring TZ.  (see man timegm)
  val = timegm(&tms);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, bool val)
{
  new_att(hid, name, val ? val_true : val_false);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, long val)
{
  // Create a dataspace for the variable
  hid_handle space(hid_space, create);

  // Create and write the attribute
  hid_handle attr(hid_attr, hid, name, H5T_STD_I64LE, space, create);
  if (H5Awrite(attr, H5T_NATIVE_LONG, &val) < 0)
    throw error(hid, ft_write, ht_attribute, name);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, double val)
{
  // Create a dataspace for the variable
  hid_handle space(hid_space, create);

  // Create and write the attribute
  hid_handle attr(hid_attr, hid, name, H5T_IEEE_F64LE, space, create);
  if (H5Awrite(attr, H5T_NATIVE_DOUBLE, &val) < 0)
    throw error(hid, ft_write, ht_attribute, name);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, const char* val)
{
  // Setup a new type for the string
  hid_handle type(hid_type, H5T_C_S1, copy);
  if (H5Tset_size(type, strlen(val) + 1) < 0)
    throw error(hid, ft_write, ht_type, name);
  if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
    throw error(hid, ft_write, ht_type, name);

  // Setup the dataspace for the attribute
  hid_handle space(hid_space, create);

  // Create and write the attribute
  hid_handle attr(hid_attr, hid, name, type, space, create);
  if (H5Awrite(attr, type, val) < 0)
    throw error(hid, ft_write, ht_attribute, name);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, const std::string& val)
{
  // Setup a new type for the string
  hid_handle type(hid_type, H5T_C_S1, copy);
  if (H5Tset_size(type, val.size() + 1) < 0)
    throw error(hid, ft_write, ht_type, name);
  if (H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
    throw error(hid, ft_write, ht_type, name);

  // Setup the dataspace for the attribute
  hid_handle space(hid_space, create);

  // Create and write the attribute
  hid_handle attr(hid_attr, hid, name, type, space, create);
  if (H5Awrite(attr, type, val.c_str()) < 0)
    throw error(hid, ft_write, ht_attribute, name);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, const long* vals, size_t size)
{
  // Create a dataspace for the variable
  hsize_t hs = size;
  hid_handle space(hid_space, 1, &hs, create);

  // Create and write the attribute
  hid_handle attr(hid_attr, hid, name, H5T_STD_I64LE, space, create);
  if (H5Awrite(attr, H5T_NATIVE_LONG, vals) < 0)
    throw error(hid, ft_write, ht_attribute, name);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name, const double* vals, size_t size)
{
  // Create a dataspace for the variable
  hsize_t hs = size;
  hid_handle space(hid_space, 1, &hs, create);

  // Create and write the attribute
  hid_handle attr(hid_attr, hid, name, H5T_IEEE_F64LE, space, create);
  if (H5Awrite(attr, H5T_NATIVE_DOUBLE, vals) < 0)
    throw error(hid, ft_write, ht_attribute, name);
}

void rainfields::hdf::new_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t val)
{
  // Print the date and time in one string
  char buf[9+7];
  struct tm* ptm = gmtime(&val);
  snprintf(
      buf, 
      9+7, 
      "%04d%02d%02d %02d%02d%02d",
      ptm->tm_year + 1900,
      ptm->tm_mon + 1,
      ptm->tm_mday,
      ptm->tm_hour,
      ptm->tm_min,
      ptm->tm_sec);

  // Split the string into two
  buf[8] = '\0';

  // Output each as an attribute
  new_att(hid, name_date, buf);
  new_att(hid, name_time, &buf[9]);
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, bool val)
{
  set_att(hid, name, val ? val_true : val_false);
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, long val)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw error(hid, ft_open, ht_attribute, name);
  else if (ret == 0)
    new_att(hid, name, val);
  else
  {
    // Okay, it's existing - just open and write
    hid_handle attr(hid_attr, hid, name, open);
    if (H5Awrite(attr, H5T_NATIVE_LONG, &val) < 0)
      throw error(hid, ft_write, ht_attribute, name);
  }
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, double val)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw error(hid, ft_open, ht_attribute, name);
  else if (ret == 0)
    new_att(hid, name, val);
  else
  {
    // Okay, it's existing - just open and write
    hid_handle attr(hid_attr, hid, name, open);
    if (H5Awrite(attr, H5T_NATIVE_DOUBLE, &val) < 0)
      throw error(hid, ft_write, ht_attribute, name);
  }
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, const std::string& val)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw error(hid, ft_open, ht_attribute, name);
  else if (ret)
    if (H5Adelete(hid, name) < 0)
      throw error(hid, ft_remove, ht_attribute, name);

  // Okay, now re-create it
  new_att(hid, name, val);
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, const char* val)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw error(hid, ft_open, ht_attribute, name);
  else if (ret)
    if (H5Adelete(hid, name) < 0)
      throw error(hid, ft_remove, ht_attribute, name);

  // Okay, now re-create it
  new_att(hid, name, val);
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, const long* vals, size_t size)
{
  // Always delete and recreate array attributes in-case the size changed
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw error(hid, ft_open, ht_attribute, name);
  else if (ret)
    if (H5Adelete(hid, name) < 0)
      throw error(hid, ft_remove, ht_attribute, name);

  // Okay, now re-create it
  new_att(hid, name, vals, size);
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name, const double* vals, size_t size)
{
  // Always delete and recreate array attributes in-case the size changed
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw error(hid, ft_open, ht_attribute, name);
  else if (ret)
    if (H5Adelete(hid, name) < 0)
      throw error(hid, ft_remove, ht_attribute, name);

  // Okay, now re-create it
  new_att(hid, name, vals, size);
}

void rainfields::hdf::set_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t val)
{
  // Print the date and time in one string
  char buf[9+7];
  struct tm tms;
  gmtime_r(&val, &tms);
  snprintf(
      buf, 
      9+7, 
      "%04d%02d%02d %02d%02d%02d",
      tms.tm_year + 1900,
      tms.tm_mon + 1,
      tms.tm_mday,
      tms.tm_hour,
      tms.tm_min,
      tms.tm_sec);

  // Split the string into two
  buf[8] = '\0';

  // Output each as an attribute
  set_att(hid, name_date, buf);
  set_att(hid, name_time, &buf[9]);
}

