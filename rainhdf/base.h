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
  /// Base class for ODIM_H5 compliant file manipulators
  class Base
  {
  public:
    /// Close the HDF file and destroy this object
    virtual ~Base();

    /// Return true if the file is read only
    bool IsReadOnly() const { return m_bReadOnly; }

    /// Get the nominal (valid) time of the data/product
    time_t GetTime() const { return GetAtt<time_t>(m_hWhat, kAtt_Date, kAtt_Time); }
    /// Write the nominal (valid) time of the data/product
    void SetTime(time_t tValid) { SetAtt(m_hWhat, kAtt_Date, kAtt_Time, tValid); }

    /// Read the source attribute values
    void GetSource(
          std::string &strWMO
        , std::string &strRadar
        , std::string &strOrigCentre
        , std::string &strPlace
        , std::string &strCountry
        , std::string &strComment) const;
    /// Write the data source attriubte
    void SetSource(
          const std::string &strWMO
        , const std::string &strRadar
        , const std::string &strOrigCentre
        , const std::string &strPlace
        , const std::string &strCountry
        , const std::string &strComment);

    /// Read an optional attribute
    template <class E, class T>
    bool GetAttribute(E eAtt, T &val) const { return GetHowAtt(m_hHow, eAtt, val); }

    /// Write an optional attribute
    template <class E, class T>
    void SetAttribute(E eAtt, const T &val) { SetHowAtt(m_hFile, m_hHow, eAtt, val); }

  protected:
    /// Create a new ODIM_H5 compliant HDF file
    Base(const std::string & strFilename, ObjectType eType, time_t tValid);

    /// Open an existing ODIM_H5 compliant HDF file
    Base(const std::string & strFilename, bool bReadOnly, ObjectType eType);

  protected:
    bool      m_bReadOnly;  ///< Is file read only?
    HID_File  m_hFile;      ///< HDF file identifier
    HID_Group m_hWhat;      ///< Root 'what' group
    HID_Group m_hWhere;     ///< Root 'where' group
    HID_Group m_hHow;       ///< Root 'how' (quality) group
  };
}

#endif

