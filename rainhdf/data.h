/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_DATA_H
#define RAINHDF_DATA_H

#include "base.h"
#include <memory>

namespace rainfields {
namespace hdf {
  /// Single data layer (image) of data used by a scan
  class data : public base
  {
  public:
    typedef std::auto_ptr<data> ptr;
    typedef std::auto_ptr<const data> const_ptr;

  public:
    virtual ~data();

    /// Is this a floating-point based layer?
    bool is_floating_point() const { return floating_; }

    /// Is this layer quality data?
    bool is_quality() const { return is_quality_; }

    /// Get the quantity stored by this data
    const std::string& quantity() const { return quantity_; }

    /// Get the number of elements in the data image
    size_t size() const { return size_; }

    void read(int* raw, int& no_data, int& undetect) const;
    void read(float* raw, float& no_data, float& undetect) const;
    void read(double* raw, double& no_data, double& undetect) const;

    void write(const int* raw, int no_data, int undetect);
    void write(const float* raw, float no_data, float undetect);
    void write(const double* raw, double no_data, double undetect);

  private:
    data(
          const base& parent
        , bool floating_point
        , bool is_quality
        , size_t index
        , const std::string& quantity
        , int rank
        , const hsize_t* dims);
    data(
          const base& parent
        , bool is_quality
        , size_t index
        , const std::string& quantity
        , int rank
        , const hsize_t* dims);

  private:
    bool        floating_;    ///< Is this a floating-point layer?
    bool        is_quality_;  ///< Is this a quality layer?
    std::string quantity_;    ///< Quantity stored by this data layer
    double      gain_;        ///< Gain (a), in ax+b data unpacking function
    double      offset_;      ///< Offset (b), in ax+b data unpacking function
    hid_handle  hnd_data_;    ///< Handle to data object

    // Cached values
    size_t      size_;        ///< Number of elements in dataset

    friend class scan;
    friend class profile;
  };
}}

#endif

