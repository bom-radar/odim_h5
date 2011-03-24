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
  static const char * kVal_ObjectType[] = 
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
  RAINHDF_ENUM_TRAITS_IMPL(ObjectType, kVal_ObjectType);

  static const char * kVal_ProductType[] = 
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
  RAINHDF_ENUM_TRAITS_IMPL(ProductType, kVal_ProductType);

  static const char * kVal_Quantity[] =
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

    , "GENERIC"
  };
  RAINHDF_ENUM_TRAITS_IMPL(Quantity, kVal_Quantity);

  static const char * kVal_Method[] =
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
  RAINHDF_ENUM_TRAITS_IMPL(Method, kVal_Method);

  static const char * kAtn_Attribute[] =
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
  };
  RAINHDF_ENUM_TRAITS_IMPL(Attribute, kAtn_Attribute);

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

void RainHDF::GetAtt(const HID_Handle &hID, const char *pszName, bool &bVal)
{
  char pszBuf[6];
  GetAtt(hID, pszName, pszBuf, 6);
  if (strcmp(pszBuf, kVal_True) == 0)
    bVal = true;
  else if (strcmp(pszBuf, kVal_False) == 0)
    bVal = false;
  else 
    throw Error(hID, "Parse error reading attribute '%s'", pszName);
}

void RainHDF::GetAtt(const HID_Handle &hID, const char *pszName, long &nVal)
{
  HID_Handle hAttr(kHID_Attr, hID, pszName, kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_LONG, &nVal) < 0)
    throw Error(hID, kErr_FailAttRead, pszName);
}

void RainHDF::GetAtt(const HID_Handle &hID, const char *pszName, double &fVal)
{
  HID_Handle hAttr(kHID_Attr, hID, pszName, kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
    throw Error(hID, kErr_FailAttRead, pszName);
}

void RainHDF::GetAtt(const HID_Handle &hID, const char *pszName, char *pszBuf, size_t nBufSize)
{
  HID_Handle hAttr(kHID_Attr, hID, pszName, kOpen);
  HID_Handle hType(kHID_Type, H5Aget_type(hAttr));
  if (H5Tget_class(hType) != H5T_STRING)
    throw Error(hID, kErr_FailAttType, pszName);
  if (H5Tget_size(hType) > nBufSize)
    throw Error(hID, kErr_FailAttSize, pszName);
  if (H5Aread(hAttr, hType, pszBuf) < 0)
    throw Error(hID, kErr_FailAttRead, pszName);
}

void RainHDF::GetAtt(const HID_Handle &hID, const char *pszName, std::string &strVal)
{
  // Use a fixed size buffer to read our string attributes
  static const int kAttBufSize = 2048;
  char pszBuf[kAttBufSize];

  HID_Handle hAttr(kHID_Attr, hID, pszName, kOpen);
  HID_Handle hType(kHID_Type, H5Aget_type(hAttr));
  if (H5Tget_class(hType) != H5T_STRING)
    throw Error(hID, kErr_FailAttType, pszName);
  if (H5Tget_size(hType) > kAttBufSize)
    throw Error(hID, kErr_FailAttSize, pszName);
  if (H5Aread(hAttr, hType, pszBuf) < 0)
    throw Error(hID, kErr_FailAttRead, pszName);

  strVal.assign(pszBuf);
}

void RainHDF::GetAtt(const HID_Handle &hID, const char *pszNameDate, const char *pszNameTime, time_t &tVal)
{
  char pszBuf[9+7];
  struct tm tms;

  // Read the attributes into a single string
  GetAtt(hID, pszNameDate, pszBuf, 9);
  GetAtt(hID, pszNameTime, &pszBuf[9], 7);
  pszBuf[8] = ' ';

  // Convert back to numerical values
  if (sscanf(
          pszBuf,
          "%04d%02d%02d %02d%02d%02d",
          &tms.tm_year,
          &tms.tm_mon,
          &tms.tm_mday,
          &tms.tm_hour,
          &tms.tm_min,
          &tms.tm_sec) != 6)
    throw Error(
        hID,
        "Invalid date/time format for attributes '%s' & '%s'",
        pszNameDate,
        pszNameTime);

  tms.tm_year -= 1900;
  tms.tm_mon -= 1;
  tms.tm_wday = 0;
  tms.tm_yday = 0;
  tms.tm_isdst = 0;

  // Convert back to a time_t
  // Note: This function is NOT portable - but there is no single portable way to
  //       perform the inverse of gmtime other than by changing TZ, calling
  //       mktime and then restoring TZ.  (see man timegm)
  tVal = timegm(&tms);
}

void RainHDF::NewAtt(const HID_Handle &hID, const char *pszName, bool bVal)
{
  NewAtt(hID, pszName, bVal ? kVal_True : kVal_False);
}

void RainHDF::NewAtt(const HID_Handle &hID, const char *pszName, long nVal)
{
  // Create a dataspace for the variable
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hID, pszName, H5T_STD_I64LE, hSpace, kCreate);
  if (H5Awrite(hAttr, H5T_NATIVE_LONG, &nVal) < 0)
    throw Error(hID, kErr_FailAttWrite, pszName);
}

void RainHDF::NewAtt(const HID_Handle &hID, const char *pszName, double fVal)
{
  // Create a dataspace for the variable
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hID, pszName, H5T_IEEE_F64LE, hSpace, kCreate);
  if (H5Awrite(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
    throw Error(hID, kErr_FailAttWrite, pszName);
}

void RainHDF::NewAtt(const HID_Handle &hID, const char *pszName, const char *pszVal)
{
  // Setup a new type for the string
  HID_Handle hType(kHID_Type, H5Tcopy(H5T_C_S1));
  if (H5Tset_size(hType, strlen(pszVal) + 1) < 0)
    throw Error(hID, "Unable to set string size for atttribute '%s'", pszName);
  if (H5Tset_strpad(hType, H5T_STR_NULLTERM) < 0)
    throw Error(hID, "Unable to set nullterm property of attribute '%s'", pszName);

  // Setup the dataspace for the attribute
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hID, pszName, hType, hSpace, kCreate);
  if (H5Awrite(hAttr, hType, pszVal) < 0)
    throw Error(hID, kErr_FailAttWrite, pszName);
}

void RainHDF::NewAtt(const HID_Handle &hID, const char *pszName, const std::string &strVal)
{
  // Setup a new type for the string
  HID_Handle hType(kHID_Type, H5Tcopy(H5T_C_S1));
  if (H5Tset_size(hType, strVal.size() + 1) < 0)
    throw Error(hID, "Unable to set string size for atttribute '%s'", pszName);
  if (H5Tset_strpad(hType, H5T_STR_NULLTERM) < 0)
    throw Error(hID, "Unable to set nullterm property of attribute '%s'", pszName);

  // Setup the dataspace for the attribute
  HID_Handle hSpace(kHID_Space, kCreate);

  // Create and write the attribute
  HID_Handle hAttr(kHID_Attr, hID, pszName, hType, hSpace, kCreate);
  if (H5Awrite(hAttr, hType, strVal.c_str()) < 0)
    throw Error(hID, kErr_FailAttWrite, pszName);
}

void RainHDF::NewAtt(const HID_Handle &hID, const char *pszNameDate, const char *pszNameTime, time_t tVal)
{
  // Print the date and time in one string
  char pszBuf[9+7];
  struct tm *pTm = gmtime(&tVal);
  snprintf(
      pszBuf, 
      9+7, 
      "%04d%02d%02d %02d%02d%02d",
      pTm->tm_year + 1900,
      pTm->tm_mon + 1,
      pTm->tm_mday,
      pTm->tm_hour,
      pTm->tm_min,
      pTm->tm_sec);

  // Split the string into two
  pszBuf[8] = '\0';

  // Output each as an attribute
  NewAtt(hID, pszNameDate, pszBuf);
  NewAtt(hID, pszNameTime, &pszBuf[9]);
}

void RainHDF::SetAtt(const HID_Handle &hID, const char *pszName, bool bVal)
{
  SetAtt(hID, pszName, bVal ? kVal_True : kVal_False);
}

void RainHDF::SetAtt(const HID_Handle &hID, const char *pszName, long nVal)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, kErr_FailAttExists, pszName);
  else if (ret == 0)
    NewAtt(hID, pszName, nVal);
  else
  {
    // Okay, it's existing - just open and write
    HID_Handle hAttr(kHID_Attr, hID, pszName, kOpen);
    if (H5Awrite(hAttr, H5T_NATIVE_LONG, &nVal) < 0)
      throw Error(hID, kErr_FailAttWrite, pszName);
  }
}

void RainHDF::SetAtt(const HID_Handle &hID, const char *pszName, double fVal)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, kErr_FailAttExists, pszName);
  else if (ret == 0)
    NewAtt(hID, pszName, fVal);
  else
  {
    // Okay, it's existing - just open and write
    HID_Handle hAttr(kHID_Attr, hID, pszName, kOpen);
    if (H5Awrite(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
      throw Error(hID, kErr_FailAttWrite, pszName);
  }
}

void RainHDF::SetAtt(const HID_Handle &hID, const char *pszName, const std::string &strVal)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, kErr_FailAttExists, pszName);
  else if (ret)
    if (H5Adelete(hID, pszName) < 0)
      throw Error(hID, kErr_FailAttDelete, pszName);

  // Okay, now re-create it
  NewAtt(hID, pszName, strVal);
}

void RainHDF::SetAtt(const HID_Handle &hID, const char *pszName, const char *pszVal)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, kErr_FailAttExists, pszName);
  else if (ret)
    if (H5Adelete(hID, pszName) < 0)
      throw Error(hID, kErr_FailAttDelete, pszName);

  // Okay, now re-create it
  NewAtt(hID, pszName, pszVal);
}

void RainHDF::SetAtt(const HID_Handle &hID, const char *pszNameDate, const char *pszNameTime, time_t tVal)
{
  // Print the date and time in one string
  char pszBuf[9+7];
  struct tm tms;
  gmtime_r(&tVal, &tms);
  snprintf(
      pszBuf, 
      9+7, 
      "%04d%02d%02d %02d%02d%02d",
      tms.tm_year + 1900,
      tms.tm_mon + 1,
      tms.tm_mday,
      tms.tm_hour,
      tms.tm_min,
      tms.tm_sec);

  // Split the string into two
  pszBuf[8] = '\0';

  // Output each as an attribute
  SetAtt(hID, pszNameDate, pszBuf);
  SetAtt(hID, pszNameTime, &pszBuf[9]);
}

static herr_t DetAttCallback(hid_t hID, const char *pszName, const H5A_info_t *pInfo, void *pData)
{
  for (int i = 0; i < enum_traits<Attribute>::kCount; ++i)
  {
    if (strcmp(pszName, kAtn_Attribute[i]) == 0)
    {
      static_cast<AttFlags*>(pData)->set(i);
      break;
    }
  }
  return 0;
}

void RainHDF::DetermineAttributePresence(const HID_Handle &hID, AttFlags &flags)
{
  hsize_t n = 0;
  H5Aiterate(hID, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, &n, DetAttCallback, &flags);
}

