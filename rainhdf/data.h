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

namespace rainhdf
{
  /// Single data layer (image) of data used by a scan
  class data : public base
  {
  public:
    typedef std::auto_ptr<data> ptr;
    typedef std::auto_ptr<const data> const_ptr;

  public:
    virtual ~data();

    /// Is this layer quality data?
    bool is_quality() const { return is_quality_; }

    /// Get the quantity stored by this data
    quantity get_quantity() const { return quantity_; }

    /// Get the number of elements in the data image
    size_t size() const { return size_; }

    void read(int* raw, int& no_data, int& undetect) const;
    void read(float* raw, float& no_data, float& undetect) const;

    void write(const int* raw, int no_data, int undetect);
    void write(const float* raw, float no_data, float undetect);

  private:
    data(
          const base& parent
        , bool is_quality
        , size_t index
        , quantity quantity
        , const hsize_t* dims
        , const int* raw
        , int no_data
        , int undetect);
    data(
          const base& parent
        , bool is_quality
        , size_t index
        , quantity quantity
        , const hsize_t* dims
        , const float* raw
        , float no_data
        , float undetect);
    data(
          const base& parent
        , bool is_quality
        , size_t index
        , quantity quantity
        , const hsize_t* dims);

  private:
    bool      is_quality_;  ///< Is this a quality layer?
    quantity  quantity_;    ///< Quantity stored by this data layer
    float     gain_;        ///< Gain (a), in ax+b data unpacking function
    float     offset_;      ///< Offset (b), in ax+b data unpacking function

    // Cached values
    size_t            size_;        ///< Number of elements in dataset

    friend class scan;
  };
};

#endif

