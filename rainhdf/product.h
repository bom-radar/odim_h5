/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_PRODUCT_H
#define RAINHDF_PRODUCT_H

#include "base.h"

namespace RainHDF
{
  /// Base class for a top level ODIM_H5 object file
  class Product : public Base
  {
  protected:
    // TODO - source is mandatory and should be in the constructor
    /// Create a new product
    Product(const std::string &strFilename, ObjectType eType, time_t tValid);
    /// Open an existing ODIM_H5 product
    Product(const std::string &strFilename, ObjectType eType, bool bReadOnly);

  public:
    /// Get the nominal (valid) time of the data/product
    time_t GetTime() const { return GetAtt<time_t>(m_hWhat, kAtn_Date, kAtn_Time); }
    /// Write the nominal (valid) time of the data/product
    void SetTime(time_t tValid) { SetAtt(m_hWhat, kAtn_Date, kAtn_Time, tValid); }

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
  };
}

#endif


