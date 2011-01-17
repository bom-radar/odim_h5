/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "base.h"

#include <cstring>

using namespace RainHDF;

namespace RainHDF
{
  static const char * kAtt_Conventions = "Conventions";
  static const char * kAtt_Object = "object";
  static const char * kAtt_Version = "version";
  static const char * kAtt_Source = "source";

  static const char * kVal_Conventions = "ODIM_H5/V2_0";
  static const char * kVal_Version = "H5rad 2.0";
}

Base::Base(
    const std::string &strFilename, 
    ObjectType eType,
    time_t tValid)
  : m_bReadOnly(false)
  , m_hFile(strFilename.c_str(), kCreate)
  , m_hWhat(m_hFile, kGrp_What, kCreate)
  , m_hWhere(m_hFile, kGrp_Where, kCreate)
{
  // Set the conventions value
  NewAtt(m_hFile, kAtt_Conventions, kVal_Conventions);

  // Fill in the global 'what' group constants
  NewAtt(m_hWhat, kAtt_Object, eType);
  NewAtt(m_hWhat, kAtt_Version, kVal_Version);
  NewAtt(m_hWhat, kAtt_Date, kAtt_Time, tValid);
}

Base::Base(
    const std::string &strFilename, 
    bool bReadOnly, 
    ObjectType eType)
  : m_bReadOnly(bReadOnly)
  , m_hFile(strFilename.c_str(), bReadOnly, kOpen)
  , m_hWhat(m_hFile, kGrp_What, kOpen)
  , m_hWhere(m_hFile, kGrp_Where, kOpen)
  , m_hHow(m_hFile, kGrp_How, kOpen, true)
{
  // Check the object type
  if (GetAtt<ObjectType>(m_hWhat, kAtt_Object) != eType)
    throw Error("ODIM_H5 object type mismatch");
}

Base::~Base()
{

}

void Base::GetSource(
      std::string &strWMO
    , std::string &strRadar
    , std::string &strOrigCentre
    , std::string &strPlace
    , std::string &strCountry
    , std::string &strComment) const
{
  // Get the raw attribute
  char pszBuf[512];
  GetAtt(m_hWhat, kAtt_Source, pszBuf, 512);

  // Clear out the passed values
  strWMO.clear();
  strRadar.clear();
  strOrigCentre.clear();
  strPlace.clear();
  strCountry.clear();
  strComment.clear();

  // Tokenize it
  char *pszTok = pszBuf;
  char *pszVal = NULL;
  bool bContinue = true;
  for (int i = 0; bContinue; ++i)
  {
    // Check for end of identifier
    if (pszBuf[i] == ':')
    {
      if (pszVal != NULL)
        throw Error(m_hWhat, "Badly formed source attribute (pair mismatch)");

      pszBuf[i] = '\0';
      pszVal = &pszBuf[i+1];
      continue;
    }

    // Check for end of string
    if (pszBuf[i] == '\0')
      bContinue = false;

    // Check for end of value and process
    if (!bContinue || pszBuf[i] == ',')
    {
      pszBuf[i] = '\0';

      if (pszVal == NULL)
        throw Error(m_hWhat, "Badly formed source attribute (pair mismatch)");
      else if (strcmp(pszTok, "WMO") == 0)
        strWMO.assign(pszVal);
      else if (strcmp(pszTok, "RAD") == 0)
        strRadar.assign(pszVal);
      else if (strcmp(pszTok, "ORG") == 0)
        strOrigCentre.assign(pszVal);
      else if (strcmp(pszTok, "PLC") == 0)
        strPlace.assign(pszVal);
      else if (strcmp(pszTok, "CTY") == 0)
        strCountry.assign(pszVal);
      else if (strcmp(pszTok, "CMT") == 0)
        strComment.assign(pszVal);
      else
        throw Error(m_hWhat, "Badly formed source attribute (unknown identifier)");

      pszTok = &pszBuf[i+1];
      pszVal = NULL;
    }
  }
}

void Base::SetSource(
      const std::string &strWMO
    , const std::string &strRadar
    , const std::string &strOrigCentre
    , const std::string &strPlace
    , const std::string &strCountry
    , const std::string &strComment)
{
  std::string strVal;
  strVal.reserve(256);

  if (!strWMO.empty())
    strVal.append(strVal.empty() ? "WMO:" : ",WMO:").append(strWMO);
  if (!strRadar.empty())
    strVal.append(strVal.empty() ? "RAD:" : ",RAD:").append(strRadar);
  if (!strOrigCentre.empty())
    strVal.append(strVal.empty() ? "ORG:" : ",ORG:").append(strOrigCentre);
  if (!strPlace.empty())
    strVal.append(strVal.empty() ? "PLC:" : ",PLC:").append(strPlace);
  if (!strCountry.empty())
    strVal.append(strVal.empty() ? "CTY:" : ",CTY:").append(strCountry);
  if (!strComment.empty())
    strVal.append(strVal.empty() ? "CMT:" : ",CMT:").append(strCountry);

  SetAtt(m_hWhat, kAtt_Source, strVal);
}


