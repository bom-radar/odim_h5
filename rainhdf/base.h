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
  protected:
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

  public:
    /// Destroy this object
    virtual ~Base();

#if 0
    /// Read an optional attribute
    template <class E, class T>
    bool GetAttribute(E eAtt, T &val) const { return GetHowAtt(m_hHow, eAtt, val); }

    /// Write an optional attribute
    template <class E, class T>
    void SetAttribute(E eAtt, const T &val) { SetHowAtt(m_hFile, m_hHow, eAtt, val); }
#endif

  protected:
    const Base *  m_pParent;    ///< Parent level (used to recursive search attributes)
    HID_Handle    m_hThis;      ///< 'This' group
    HID_Handle    m_hWhat;      ///< What group
    HID_Handle    m_hWhere;     ///< Where group
    HID_Handle    m_hHow;       ///< How group
    AttFlags      m_AttFlags;   ///< Flags to indicate presence of 'how' group attributes
  };
}

#endif

