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
    bool is_quality() const { return is_quality_; }

    /// Get the quantity stored by this data
    Quantity quantity() const { return quantity_; }

    /// Get the number of elements in the data image
    size_t size() const { return size_; }

    /// Read the data
    void read(float* data, float& no_data, float& undetect) const;
    /// Write the data
    void write(const float* data, float no_data, float undetect);

  private:
    Data(
          const Base &parent
        , bool is_quality
        , size_t index
        , Quantity quantity
        , const hsize_t* dims
        , const float* data
        , float no_data
        , float undetect);
    Data(
          const Base &parent
        , bool is_quality
        , size_t index
        , Quantity quantity
        , const hsize_t* dims);

  private:
    bool      is_quality_;  ///< Is this a quality layer?
    Quantity  quantity_;    ///< Quantity stored by this data layer
    float     gain_;        ///< Gain (a), in ax+b data unpacking function
    float     offset_;      ///< Offset (b), in ax+b data unpacking function

    // Cached values
    size_t    size_;        ///< Number of elements in dataset

    friend class Scan;
  };
};

#endif

