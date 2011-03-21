/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "base.h"

#include <cstring>

using namespace RainHDF;

Base::Base(const std::string &strFilename, CreateFlag)
  : m_pParent(NULL)
  , m_hThis(kHID_File, strFilename.c_str(), kCreate)
  , m_hWhat(kHID_Group, m_hThis, kGrp_What, kCreate)
  , m_hWhere(kHID_Group, m_hThis, kGrp_Where, kCreate)
  , m_hHow(kHID_Group, m_hThis, kGrp_How, kCreate)
{

}

Base::Base(const Base &parent, const char *pszName, CreateFlag)
  : m_pParent(&parent)
  , m_hThis(kHID_Group, m_pParent->m_hThis, pszName, kCreate)
  , m_hWhat(kHID_Group, m_hThis, kGrp_What, kCreate)
  , m_hWhere(kHID_Group, m_hThis, kGrp_Where, kCreate)
  , m_hHow(kHID_Group, m_hThis, kGrp_How, kCreate)
{

}

Base::Base(const Base &parent, const char *pszName, int nIndex, CreateFlag)
  : m_pParent(&parent)
  , m_hThis(kHID_Group, m_pParent->m_hThis, pszName, nIndex, kCreate)
  , m_hWhat(kHID_Group, m_hThis, kGrp_What, kCreate)
  , m_hWhere(kHID_Group, m_hThis, kGrp_Where, kCreate)
  , m_hHow(kHID_Group, m_hThis, kGrp_How, kCreate)
{

}

Base::Base(const std::string &strFilename, bool bReadOnly, OpenFlag)
  : m_pParent(NULL)
  , m_hThis(kHID_File, strFilename.c_str(), bReadOnly, kOpen)
  , m_hWhat(kHID_Group, m_hThis, kGrp_What, kOpen, true)
  , m_hWhere(kHID_Group, m_hThis, kGrp_Where, kOpen, true)
  , m_hHow(kHID_Group, m_hThis, kGrp_How, kOpen, true)
{
  // Note presence of any 'how' attributes
  if (m_hHow)
    DetermineAttributePresence(m_hHow, m_AttFlags);
}

Base::Base(const Base &parent, const char *pszName, OpenFlag)
  : m_pParent(&parent)
  , m_hThis(kHID_Group, m_pParent->m_hThis, pszName, kOpen)
  , m_hWhat(kHID_Group, m_hThis, kGrp_What, kOpen, true)
  , m_hWhere(kHID_Group, m_hThis, kGrp_Where, kOpen, true)
  , m_hHow(kHID_Group, m_hThis, kGrp_How, kOpen, true)
{
  // Note presence of any 'how' attributes
  if (m_hHow)
    DetermineAttributePresence(m_hHow, m_AttFlags);
}

Base::Base(const Base &parent, const char *pszName, int nIndex, OpenFlag)
  : m_pParent(&parent)
  , m_hThis(kHID_Group, m_pParent->m_hThis, pszName, nIndex, kOpen)
  , m_hWhat(kHID_Group, m_hThis, kGrp_What, kOpen, true)
  , m_hWhere(kHID_Group, m_hThis, kGrp_Where, kOpen, true)
  , m_hHow(kHID_Group, m_hThis, kGrp_How, kOpen, true)
{
  // Note presence of any 'how' attributes
  if (m_hHow)
    DetermineAttributePresence(m_hHow, m_AttFlags);
}

Base::~Base()
{

}

