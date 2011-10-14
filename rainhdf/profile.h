/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_PROFILE_H
#define RAINHDF_PROFILE_H

#include "base.h"
#include "data.h"
#include <memory>
#include <vector>

namespace rainhdf
{
  /// Unique handle to a vertical profile in the HDF5 file
  class profile : public base
  {
  public:
    typedef std::auto_ptr<profile> ptr;
    typedef std::auto_ptr<const profile> const_ptr;

  public:
    virtual ~profile();

    /// Get the time the scan(s) for this profile commenced
    time_t start_time() const { return get_att<time_t>(hnd_what_, atn_start_date, atn_start_time); }
    /// Get the time the scan(s) for this profile completed
    time_t end_time() const { return get_att<time_t>(hnd_what_, atn_end_date, atn_end_time); }

    /// Get the number of data layers in the profile
    size_t layer_count() const { return data_info_.size(); }
    /// Get the quantity stored by the 'nth' layer
    const std::string layer_quantity(size_t i) const { return data_info_[i].quantity_; }
    /// Get the 'nth' data layer
    data::ptr layer(size_t i);
    /// Get the 'nth' data layer
    data::const_ptr layer(size_t i) const;
    /// Get a data layer based on it's quantity (or NULL if no such data)
    data::ptr layer(const std::string& quantity);
    data::const_ptr layer(const std::string& quantity) const;
    /// Add a new data or quality layer to the profile
    data::ptr add_layer(const std::string& quantity, bool is_quality, bool floating_point);

  private:
    struct data_info
    {
      bool        is_quality_;  ///< True if quality, false if data
      size_t      index_;       ///< Index of dataX in file
      std::string quantity_;    ///< Quantity stored by data layer
    };
    typedef std::vector<data_info> data_info_store;

  private:
    /// Create a new profile in file
    profile(
          const base& parent
        , size_t index
        , size_t levels
        , time_t start_time
        , time_t end_time);
    /// Create handle to a scan that is existing in the file
    profile(const base& parent, size_t index, size_t levels);

  private:
    // Size required by data layers
    hsize_t           levels_;          ///< Number of height levels

    // Handles to any data layers that are present
    data_info_store   data_info_;       ///< Information about data/quality layers

    friend class vertical_profile;
  };
};

#endif

