/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_BASE_H
#define RAINHDF_BASE_H

#include "util.h"

namespace RainHDF
{
  /// Base class for a single level in a ODIM_H5 product hierarchy
  class Base
  {
  public:
    /// Create a new level as the root of a file
    Base(const std::string &strFilename, CreateFlag);

    /// Create a new level by name
    Base(const Base &parent, const char *pszName, CreateFlag);

    /// Create a new level by appending an index to a name
    Base(const Base &parent, const char *pszName, int nIndex, CreateFlag);

    /// Open the rool level from a file
    Base(const std::string &strFilename, bool bReadOnly, OpenFlag);

    /// Open a level by name
    Base(const Base &parent, const char *pszName, OpenFlag);

    /// Open a level by appending an index to a name
    Base(const Base &parent, const char *pszName, int nIndex, OpenFlag);

    /// Destroy this object
    virtual ~Base();

    /// Get the set of attributes that are available at this level
    const AttFlags & GetAttributeFlags() const { return m_AttFlags; }

    /// Read an optional attribute
    template <class T>
    bool GetAttribute(Attribute eAtt, T &val) const;

    /// Write an optional attribute
    template <class T>
    void SetAttribute(Attribute eAtt, const T &val);

  protected:
    const Base *  m_pParent;    ///< Parent level (used to recursive search attributes)
    HID_Handle    m_hThis;      ///< 'This' group
    HID_Handle    m_hWhat;      ///< What group
    HID_Handle    m_hWhere;     ///< Where group
    HID_Handle    m_hHow;       ///< How group
    AttFlags      m_AttFlags;   ///< Flags to indicate presence of 'how' group attributes
  };

  template <class T>
  bool Base::GetAttribute(Attribute eAtt, T &val) const 
  { 
    // TODO - manually verify type of attribute? (will throw read error if reading wrong type)
    for (const Base * pLevel = this; pLevel != NULL; pLevel = pLevel->m_pParent)
    {
      if (pLevel->m_AttFlags.test(eAtt))
      {
        GetAtt(pLevel->m_hHow, kAtn_Attribute[eAtt], val);
        return true;
      }
    }
    return false;
  }

  template <class T>
  void Base::SetAttribute(Attribute eAtt, const T &val)
  {
    // TODO - manually verify type of attribute? (will allow writing of non-standard type)
    if (!m_hHow)
      m_hHow = HID_Handle(kHID_Group, m_hThis, kGrp_How, kCreate);
    SetAtt(m_hHow, kAtn_Attribute[eAtt], val);
    m_AttFlags.set(eAtt);
  }
}

#endif

