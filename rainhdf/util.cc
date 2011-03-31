/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "util.h"

#include <cstring>

using namespace RainHDF;

namespace RainHDF
{
  const char * enum_traits<ObjectType>::kName = "ObjectType";
  const char * enum_traits<ObjectType>::kStrings[] =
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

  const char * enum_traits<ProductType>::kName = "ProductType";
  const char * enum_traits<ProductType>::kStrings[] =
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

  const char * enum_traits<Quantity>::kName = "Quantity";
  const char * enum_traits<Quantity>::kStrings[] =
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

    , "OCCUL"
    , "ATTEN"
    , "CPROB"
    , "HPROB"

    , "DBZH_CLEAN"
    , "VRAD_CLEAN"

    , "GENERIC"
  };

  const char * enum_traits<Method>::kName = "Method";
  const char * enum_traits<Method>::kStrings[] =
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

  const char * enum_traits<Attribute>::kName = "Attribute";
  const char * enum_traits<Attribute>::kStrings[] =
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
    , "lowprf"
    , "highprf"
    , "azmethod"
    , "binmethod"
    , "azangles"
    , "elangles"
    , "aztimes"
    , "angles"
    , "arotation"
    , "camethod"
    , "nodes"
    , "ACCnum"
    , "minrange"
    , "maxrange"
    , "NI"
    , "dealiased"
    , "pointaccEL"
    , "pointaccAZ"
    , "malfunc"
    , "radar_msg"
    , "radhoriz"
    , "MDS"
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

  const char * kGrp_What = "what";
  const char * kGrp_Where = "where";
  const char * kGrp_How = "how";
  const char * kGrp_Dataset = "dataset";
  const char * kGrp_Data = "data";
  const char * kGrp_Quality = "quality";

  const char * kAtn_Conventions = "Conventions";
  const char * kAtn_Object = "object";
  const char * kAtn_Version = "version";
  const char * kAtn_Date = "date";
  const char * kAtn_Time = "time";
  const char * kAtn_Source = "source";
  const char * kAtn_Latitude = "lat";
  const char * kAtn_Longitude = "lon";
  const char * kAtn_Height = "height";
  const char * kAtn_Product = "product";
  const char * kAtn_StartDate = "startdate";
  const char * kAtn_StartTime = "starttime";
  const char * kAtn_EndDate = "enddate";
  const char * kAtn_EndTime = "endtime";
  const char * kAtn_Elevation = "elangle";
  const char * kAtn_FirstAzimuth = "a1gate";
  const char * kAtn_RangeCount = "nbins";
  const char * kAtn_RangeStart = "rstart";
  const char * kAtn_RangeScale = "rscale";
  const char * kAtn_AzimuthCount = "nrays";
  const char * kAtn_Quantity = "quantity";
  const char * kAtn_Gain = "gain";
  const char * kAtn_Offset = "offset";
  const char * kAtn_NoData = "nodata";
  const char * kAtn_Undetect = "undetect";
  const char * kAtn_Class = "CLASS";
  const char * kAtn_ImageVersion = "IMAGE_VERSION";

  const char * kDat_Data = "data";

  const char * kVal_True = "True";
  const char * kVal_False = "False";
  const char * kVal_Conventions = "ODIM_H5/V2_0";
  const char * kVal_Version = "H5rad 2.0";
  const char * kVal_Class = "IMAGE";
  const char * kVal_ImageVersion = "1.2";

  // Error strings
  static const char * kErr_FailAttExists = "Attribute existance check failed for attribute '%s'";
  static const char * kErr_FailAttRead = "Failed to read attribute '%s'";
  static const char * kErr_FailAttType = "Type mismatch on attribute '%s'";
  static const char * kErr_FailAttSize = "String overflow on attribute '%s'";
  static const char * kErr_FailAttWrite = "Failed to write attribute '%s'";
  static const char * kErr_FailAttDelete = "failed to delete attribute '%s' before write";
}

void RainHDF::get_att(const HID_Handle &hid, const char *name, bool &val)
{
  char buf[6];
  get_att(hid, name, buf, 6);
  if (strcmp(buf, kVal_True) == 0)
    val = true;
  else if (strcmp(buf, kVal_False) == 0)
    val = false;
  else 
    throw Error(hid, "Parse error reading attribute '%s'", name);
}

void RainHDF::get_att(const HID_Handle &hid, const char *name, long &val)
{
  HID_Handle hAttr(kHID_Attr, hid, name, kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_LONG, &val) < 0)
    throw Error(hid, kErr_FailAttRead, name);
}

void RainHDF::get_att(const HID_Handle &hid, const char *name, double &val)
{
  HID_Handle hAttr(kHID_Attr, hid, name, kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_DOUBLE, &val) < 0)
    throw Error(hid, kErr_FailAttRead, name);
}

void RainHDF::get_att(const HID_Handle &hid, const char *name, char *buf, size_t buf_size)
{
  HID_Handle hAttr(kHID_Attr, hid, name, kOpen);
  HID_Handle hType(kHID_Type, H5Aget_type(hAttr));
  if (H5Tget_class(hType) != H5T_STRING)
    throw Error(hid, kErr_FailAttType, name);
  if (H5Tget_size(hType) > buf_size)
    throw Error(hid, kErr_FailAttSize, name);
  if (H5Aread(hAttr, hType, buf) < 0)
    throw Error(hid, kErr_FailAttRead, name);
}

void RainHDF::get_att(const HID_Handle &hid, const char *name, std::string &val)
{
  // Use a fixed size buffer to read our string attributes
  static const int kAttBufSize = 2048;
  char buf[kAttBufSize];

  HID_Handle hAttr(kHID_Attr, hid, name, kOpen);
  HID_Handle hType(kHID_Type, H5Aget_type(hAttr));
  if (H5Tget_class(hType) != H5T_STRING)
    throw Error(hid, kErr_FailAttType, name);
  if (H5Tget_size(hType) > kAttBufSize)
    throw Error(hid, kErr_FailAttSize, name);
  if (H5Aread(hAttr, hType, buf) < 0)
    throw Error(hid, kErr_FailAttRead, name);

  val.assign(buf);
}

void RainHDF::get_att(const HID_Handle &hid, const char *name_date, const char *name_time, time_t &val)
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
    throw Error(
        hid,
        "Invalid date/time format for attributes '%s' & '%s'",
        name_date,
        name_time);

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

void RainHDF::new_att(const HID_Handle &hid, const char *name, bool val)
{
  new_att(hid, name, val ? kVal_True : kVal_False);
}

void RainHDF::new_att(const HID_Handle &hid, const char *name, long val)
{
  // Create a dataspace for the variable
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hid, name, H5T_STD_I64LE, hSpace, kCreate);
  if (H5Awrite(hAttr, H5T_NATIVE_LONG, &val) < 0)
    throw Error(hid, kErr_FailAttWrite, name);
}

void RainHDF::new_att(const HID_Handle &hid, const char *name, double val)
{
  // Create a dataspace for the variable
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hid, name, H5T_IEEE_F64LE, hSpace, kCreate);
  if (H5Awrite(hAttr, H5T_NATIVE_DOUBLE, &val) < 0)
    throw Error(hid, kErr_FailAttWrite, name);
}

void RainHDF::new_att(const HID_Handle &hid, const char *name, const char *val)
{
  // Setup a new type for the string
  HID_Handle hType(kHID_Type, H5Tcopy(H5T_C_S1));
  if (H5Tset_size(hType, strlen(val) + 1) < 0)
    throw Error(hid, "Unable to set string size for atttribute '%s'", name);
  if (H5Tset_strpad(hType, H5T_STR_NULLTERM) < 0)
    throw Error(hid, "Unable to set nullterm property of attribute '%s'", name);

  // Setup the dataspace for the attribute
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hid, name, hType, hSpace, kCreate);
  if (H5Awrite(hAttr, hType, val) < 0)
    throw Error(hid, kErr_FailAttWrite, name);
}

void RainHDF::new_att(const HID_Handle &hid, const char *name, const std::string &val)
{
  // Setup a new type for the string
  HID_Handle hType(kHID_Type, H5Tcopy(H5T_C_S1));
  if (H5Tset_size(hType, val.size() + 1) < 0)
    throw Error(hid, "Unable to set string size for atttribute '%s'", name);
  if (H5Tset_strpad(hType, H5T_STR_NULLTERM) < 0)
    throw Error(hid, "Unable to set nullterm property of attribute '%s'", name);

  // Setup the dataspace for the attribute
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hid, name, hType, hSpace, kCreate);
  if (H5Awrite(hAttr, hType, val.c_str()) < 0)
    throw Error(hid, kErr_FailAttWrite, name);
}

void RainHDF::new_att(const HID_Handle &hid, const char *name_date, const char *name_time, time_t val)
{
  // Print the date and time in one string
  char buf[9+7];
  struct tm *pTm = gmtime(&val);
  snprintf(
      buf, 
      9+7, 
      "%04d%02d%02d %02d%02d%02d",
      pTm->tm_year + 1900,
      pTm->tm_mon + 1,
      pTm->tm_mday,
      pTm->tm_hour,
      pTm->tm_min,
      pTm->tm_sec);

  // Split the string into two
  buf[8] = '\0';

  // Output each as an attribute
  new_att(hid, name_date, buf);
  new_att(hid, name_time, &buf[9]);
}

void RainHDF::set_att(const HID_Handle &hid, const char *name, bool val)
{
  set_att(hid, name, val ? kVal_True : kVal_False);
}

void RainHDF::set_att(const HID_Handle &hid, const char *name, long val)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw Error(hid, kErr_FailAttExists, name);
  else if (ret == 0)
    new_att(hid, name, val);
  else
  {
    // Okay, it's existing - just open and write
    HID_Handle hAttr(kHID_Attr, hid, name, kOpen);
    if (H5Awrite(hAttr, H5T_NATIVE_LONG, &val) < 0)
      throw Error(hid, kErr_FailAttWrite, name);
  }
}

void RainHDF::set_att(const HID_Handle &hid, const char *name, double val)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw Error(hid, kErr_FailAttExists, name);
  else if (ret == 0)
    new_att(hid, name, val);
  else
  {
    // Okay, it's existing - just open and write
    HID_Handle hAttr(kHID_Attr, hid, name, kOpen);
    if (H5Awrite(hAttr, H5T_NATIVE_DOUBLE, &val) < 0)
      throw Error(hid, kErr_FailAttWrite, name);
  }
}

void RainHDF::set_att(const HID_Handle &hid, const char *name, const std::string &val)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw Error(hid, kErr_FailAttExists, name);
  else if (ret)
    if (H5Adelete(hid, name) < 0)
      throw Error(hid, kErr_FailAttDelete, name);

  // Okay, now re-create it
  new_att(hid, name, val);
}

void RainHDF::set_att(const HID_Handle &hid, const char *name, const char *val)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hid, name);
  if (ret < 0)
    throw Error(hid, kErr_FailAttExists, name);
  else if (ret)
    if (H5Adelete(hid, name) < 0)
      throw Error(hid, kErr_FailAttDelete, name);

  // Okay, now re-create it
  new_att(hid, name, val);
}

void RainHDF::set_att(const HID_Handle &hid, const char *name_date, const char *name_time, time_t val)
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

static herr_t check_att_callback(hid_t hid, const char *name, const H5A_info_t *pInfo, void *pData)
{
  for (int i = 0; i < enum_traits<Attribute>::kCount; ++i)
  {
    if (strcmp(name, enum_traits<Attribute>::kStrings[i]) == 0)
    {
      static_cast<AttFlags*>(pData)->set(i);
      break;
    }
  }
  return 0;
}

void RainHDF::check_attribs_presence(const HID_Handle &hid, AttFlags &flags)
{
  hsize_t n = 0;
  H5Aiterate(hid, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, &n, check_att_callback, &flags);
}

