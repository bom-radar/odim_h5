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
    , "???"
    , NULL
  };

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
    , "???"
    , NULL
  };

  static const char * kVal_DataQuantity[] =
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
    , "???"
    , NULL
  };

  static const char * kAtt_QualityAttribute_Double[] = 
  {
      "zr_a"
    , "zr_b"
    , "kr_a"
    , "kr_b"
    , "beamwidth"
    , "wavelength"
    , "rpm"
    , "pulsewidth"
    , "lowprf"
    , "highprf"
    , "minrange"
    , "maxrange"
    , "NI"
    , "pointaccEL"
    , "pointaccAZ"
    , "radhoriz"
    , "MDS"
    , "OUR"
    , "SQI"
    , "CSR"
    , "LOG"
    , "freeze"
    , "min"
    , "max"
    , "step"
    , "peakpwr"
    , "avgpwr"
    , "dynrange"
    , "RAC"
    , "PAC"
    , "S2N"
  };
}

bool RainHDF::IndexedGroupExists(
    hid_t hParent, 
    const char *pszGroup,
    int nIndex,
    char *pszNameOut)
{
  sprintf(pszNameOut, "%s%d", pszGroup, nIndex);
  htri_t ret = H5Lexists(hParent, pszNameOut, H5P_DEFAULT);
  if (ret < 0)
    throw Error(hParent, "Failed to verify existance of group '%s'", pszNameOut);
  return ret != 0;
}

void RainHDF::NewAtt(hid_t hID, const char *pszName, const char *pszVal)
{
  // Setup a new type for the string
  HID_Type hType(H5Tcopy(H5T_C_S1));
  if (H5Tset_size(hType, strlen(pszVal) + 1) < 0)
    throw Error(hID, "Unable to set string size for atttribute '%s'", pszName);
  if (H5Tset_strpad(hType, H5T_STR_NULLTERM) < 0)
    throw Error(hID, "Unable to set nullterm property of attribute '%s'", pszName);

  // Setup the dataspace for the attribute
  HID_Space hSpace(kCreate);

  // Create and write the attribute
  HID_Attr hAttr(hID, pszName, hType, hSpace, kCreate);
  if (H5Awrite(hAttr, hType, pszVal) < 0)
    throw Error(hID, "Unable to write string attribute '%s'", pszName);
}

void RainHDF::NewAtt(hid_t hID, const char *pszName, const std::string &strVal)
{
  // Setup a new type for the string
  HID_Type hType(H5Tcopy(H5T_C_S1));
  if (H5Tset_size(hType, strVal.size() + 1) < 0)
    throw Error(hID, "Unable to set string size for atttribute '%s'", pszName);
  if (H5Tset_strpad(hType, H5T_STR_NULLTERM) < 0)
    throw Error(hID, "Unable to set nullterm property of attribute '%s'", pszName);

  // Setup the dataspace for the attribute
  HID_Space hSpace(kCreate);

  // Create and write the attribute
  HID_Attr hAttr(hID, pszName, hType, hSpace, kCreate);
  if (H5Awrite(hAttr, hType, strVal.c_str()) < 0)
    throw Error(hID, "Unable to write string attribute '%s'", pszName);
}

void RainHDF::NewAtt(hid_t hID, const char *pszName, long nVal)
{
  // Create a dataspace for the variable
  HID_Space hSpace(kCreate);

  // Create and write the attribute
  HID_Attr hAttr(hID, pszName, H5T_NATIVE_LONG, hSpace, kCreate);
  if (H5Awrite(hAttr, H5T_NATIVE_LONG, &nVal) < 0)
    throw Error(hID, "Unable to write long attribute '%s'", pszName);
}

void RainHDF::NewAtt(hid_t hID, const char *pszName, double fVal)
{
  // Create a dataspace for the variable
  HID_Space hSpace(kCreate);

  // Create and write the attribute
  HID_Attr hAttr(hID, pszName, H5T_NATIVE_DOUBLE, hSpace, kCreate);
  if (H5Awrite(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
    throw Error(hID, "Unable to write double attribute '%s'", pszName);
}

void RainHDF::NewAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t tVal)
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

void RainHDF::NewAtt(hid_t hID, const char *pszName, ObjectType eVal)
{
  NewAtt(hID, pszName, kVal_ObjectType[eVal]);
}

void RainHDF::NewAtt(hid_t hID, const char *pszName, ProductType eVal)
{
  NewAtt(hID, pszName, kVal_ProductType[eVal]);
}

void RainHDF::NewAtt(hid_t hID, const char *pszName, DataQuantity eVal)
{
  NewAtt(hID, pszName, kVal_DataQuantity[eVal]);
}

void RainHDF::SetAtt(hid_t hID, const char *pszName, const std::string &strVal)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, "Attribute existance check failed for attribute '%s'", pszName);
  else if (ret)
    if (H5Adelete(hID, pszName) < 0)
      throw Error(hID, "Failed to delete attribute '%s' before write", pszName);

  // Okay, now re-create it
  NewAtt(hID, pszName, strVal);
}

void RainHDF::SetAtt(hid_t hID, const char *pszName, const char *pszVal)
{
  // For a string attribute, we always delete and recreate (in case size changes)
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, "Attribute existance check failed for attribute '%s'", pszName);
  else if (ret)
    if (H5Adelete(hID, pszName) < 0)
      throw Error(hID, "Failed to delete attribute '%s' before write", pszName);

  // Okay, now re-create it
  NewAtt(hID, pszName, pszVal);
}

void RainHDF::SetAtt(hid_t hID, const char *pszName, long nVal)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, "Attribute existance check failed for attribute '%s'", pszName);
  else if (ret == 0)
    NewAtt(hID, pszName, nVal);
  else
  {
    // Okay, it's existing - just open and write
    HID_Attr hAttr(hID, pszName, kOpen);
    if (H5Awrite(hAttr, H5T_NATIVE_LONG, &nVal) < 0)
      throw Error(hID, "Unable to write long attribute '%s'", pszName);
  }
}

void RainHDF::SetAtt(hid_t hID, const char *pszName, double fVal)
{
  // Check if it's already been created
  htri_t ret = H5Aexists(hID, pszName);
  if (ret < 0)
    throw Error(hID, "Attribute existance check failed for attribute '%s'", pszName);
  else if (ret == 0)
    NewAtt(hID, pszName, fVal);
  else
  {
    // Okay, it's existing - just open and write
    HID_Attr hAttr(hID, pszName, kOpen);
    if (H5Awrite(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
      throw Error(hID, "Unable to write double attribute '%s'", pszName);
  }
}

void RainHDF::SetAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t tVal)
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

void RainHDF::SetAtt(hid_t hID, const char *pszName, ObjectType eVal)
{
  SetAtt(hID, pszName, kVal_ObjectType[eVal]);
}

void RainHDF::SetAtt(hid_t hID, const char *pszName, ProductType eVal)
{
  SetAtt(hID, pszName, kVal_ProductType[eVal]);
}

void RainHDF::SetAtt(hid_t hID, const char *pszName, DataQuantity eVal)
{
  SetAtt(hID, pszName, kVal_DataQuantity[eVal]);
}

void RainHDF::GetAtt(hid_t hID, const char *pszName, char *pszBuf, size_t nBufSize)
{
  HID_Attr hAttr(hID, pszName, kOpen);
  HID_Type hType(H5Aget_type(hAttr));
  if (H5Tget_class(hType) != H5T_STRING)
    throw Error(hID, "Type mismatch on attribute '%s'", pszName);
  if (H5Tget_size(hType) > nBufSize)
    throw Error(hID, "String overflow on attribute '%s'", pszName);
  if (H5Aread(hAttr, hType, pszBuf) < 0)
    throw Error(hID, "Unable to string attribute '%s'", pszName);
}

void RainHDF::GetAtt(hid_t hID, const char *pszName, std::string &strVal)
{
  // Use a fixed size buffer to read our string attributes
  static const int kAttBufSize = 2048;
  char pszBuf[kAttBufSize];

  HID_Attr hAttr(hID, pszName, kOpen);
  HID_Type hType(H5Aget_type(hAttr));
  if (H5Tget_class(hType) != H5T_STRING)
    throw Error(hID, "Type mismatch on attribute '%s'", pszName);
  if (H5Tget_size(hType) > kAttBufSize)
    throw Error(hID, "String overflow on attribute '%s'", pszName);
  if (H5Aread(hAttr, hType, pszBuf) < 0)
    throw Error(hID, "Unable to string attribute '%s'", pszName);

  strVal.assign(pszBuf);
}

void RainHDF::GetAtt(hid_t hID, const char *pszName, long &nVal)
{
  HID_Attr hAttr(hID, pszName, kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_LONG, &nVal) < 0)
    throw Error(hID, "Unable to read long attribute '%s'", pszName);
}

void RainHDF::GetAtt(hid_t hID, const char *pszName, double &fVal)
{
  HID_Attr hAttr(hID, pszName, kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
    throw Error(hID, "Unable to read double attribute '%s'", pszName);
}

void RainHDF::GetAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t &tVal)
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

void RainHDF::GetAtt(hid_t hID, const char *pszName, ObjectType &eVal)
{
  char pszBuf[16];
  GetAtt(hID, pszName, pszBuf, sizeof(pszBuf));
  for (int nVal = 0; kVal_ObjectType[nVal] != NULL; ++nVal)
  {
    if (strcmp(kVal_ObjectType[nVal], pszBuf) == 0)
    {
      eVal = static_cast<ObjectType>(nVal);
      return;
    }
  }
  eVal = kOT_Unknown;
}

void RainHDF::GetAtt(hid_t hID, const char *pszName, ProductType &eVal)
{
  char pszBuf[16];
  GetAtt(hID, pszName, pszBuf, sizeof(pszBuf));
  for (int nVal = 0; kVal_ProductType[nVal] != NULL; ++nVal)
  {
    if (strcmp(kVal_ProductType[nVal], pszBuf) == 0)
    {
      eVal = static_cast<ProductType>(nVal);
      return;
    }
  }
  eVal = kPT_Unknown;
}

void RainHDF::GetAtt(hid_t hID, const char *pszName, DataQuantity &eVal)
{
  char pszBuf[16];
  GetAtt(hID, pszName, pszBuf, sizeof(pszBuf));
  for (int nVal = 0; kVal_DataQuantity[nVal] != NULL; ++nVal)
  {
    if (strcmp(kVal_DataQuantity[nVal], pszBuf) == 0)
    {
      eVal = static_cast<DataQuantity>(nVal);
      return;
    }
  }
  eVal = kDQ_Unknown;
}

void RainHDF::SetHowAtt(hid_t hParent, HID_Group &hHow, QualityAttribute_Double eAttr, double fVal)
{
  if (!hHow)
    hHow = HID_Group(hParent, "how", kCreate);
  SetAtt(hHow, kAtt_QualityAttribute_Double[eAttr], fVal);
}

bool RainHDF::GetHowAtt(const HID_Group &hHow, QualityAttribute_Double eAttr, double &fVal)
{
  if (!hHow)
    return false;

  htri_t ret = H5Aexists(hHow, kAtt_QualityAttribute_Double[eAttr]);
  if (ret < 0)
    throw Error(
        hHow, 
        "Attribute existance check failed for attribute '%s'", 
        kAtt_QualityAttribute_Double[eAttr]);

  if (ret == 0)
    return false;

  // Okay, it's existing - attempt to read it
  HID_Attr hAttr(hHow, kAtt_QualityAttribute_Double[eAttr], kOpen);
  if (H5Aread(hAttr, H5T_NATIVE_DOUBLE, &fVal) < 0)
    throw Error(
        hHow, 
        "Unable to read double attribute '%s'", 
        kAtt_QualityAttribute_Double[eAttr]);

  return true;
}


