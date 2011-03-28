/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_DATA_H
#define RAINHDF_DATA_H

#include "base.h"
#include <memory>

namespace RainHDF
{
  /// Single data layer (image) of data used by a scan
  class Data : public Base
  {
  public:
    typedef std::auto_ptr<Data> Ptr;
    typedef std::auto_ptr<const Data> ConstPtr;

  public:
    virtual ~Data();

    /// Is this layer quality data?
    bool IsQualityData() const { return m_bIsQuality; }

    /// Get the quantity stored by this data
    Quantity GetQuantity() const { return m_eQuantity; }

    /// Get the number of elements in the data image
    size_t GetSize() const { return m_nSize; }

    /// Read the data
    void Read(float *pData, float &fNoData, float &fUndetect) const;
    /// Write the data
    void Write(const float *pData, float fNoData, float fUndetect);

  private:
    Data(
          const Base &parent
        , bool bIsQuality
        , size_t nIndex
        , Quantity eQuantity
        , const hsize_t *pDims
        , const float *pData
        , float fNoData
        , float fUndetect);
    Data(
          const Base &parent
        , bool bIsQuality
        , size_t nIndex
        , Quantity eQuantity
        , const hsize_t *pDims);

  private:
    bool              m_bIsQuality; ///< Is this a quality layer?
    Quantity          m_eQuantity;  ///< Quantity stored by this data layer
    float             m_fGain;      ///< Gain (a), in ax+b data unpacking function
    float             m_fOffset;    ///< Offset (b), in ax+b data unpacking function

    // Cached values
    size_t            m_nSize;      ///< Number of elements in dataset

    friend class Scan;
  };
};

#endif

