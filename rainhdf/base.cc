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
  static const char * kGrp_What = "what";
  static const char * kGrp_Where = "where";
  static const char * kGrp_How = "how";

  static const char * kAtt_Conventions = "Conventions";
  static const char * kAtt_Object = "object";
  static const char * kAtt_Version = "version";
  static const char * kAtt_Date = "date";
  static const char * kAtt_Time = "time";
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
  // TODO - source
  NewAtt(m_hWhat, kAtt_Source, "sourjcvkls");
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

