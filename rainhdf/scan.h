/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_SCAN_H
#define RAINHDF_SCAN_H

#include "base.h"
#include "data.h"
#include <memory>
#include <vector>

namespace RainHDF
{
  /// Unique handle to a scan in the HDF5 file
  class Scan : public Base
  {
  public:
    typedef std::auto_ptr<Scan> Ptr;
    typedef std::auto_ptr<const Scan> ConstPtr;

  public:
    virtual ~Scan();

    /// Get the elevation of this scan
    double elevation() const { return get_att<double>(hnd_where_, kAtn_Elevation); }
    /// Get the number of azimuths in scan
    size_t azimuth_count() const { return azi_count_; }
    /// Get the number of range bins per azimuth
    size_t range_bin_count() const { return bin_count_; }
    /// Get the number of the first azimuth to be radiated
    size_t first_azimuth() const { return get_att<long>(hnd_where_, kAtn_FirstAzimuth); }
    /// Get the distance from sensor at the start of the first range bin in meters
    double range_start() const { return get_att<double>(hnd_where_, kAtn_RangeStart) * 1000.0; }
    /// Get the distance between consecutive range bins in meters
    double range_scale() const { return get_att<double>(hnd_where_, kAtn_RangeScale); }
    /// Get the time this scan commenced
    time_t start_time() const { return get_att<time_t>(hnd_what_, kAtn_StartDate, kAtn_StartTime); }
    /// Get the time this scan completed
    time_t end_time() const { return get_att<time_t>(hnd_what_, kAtn_EndDate, kAtn_EndTime); }

    /// Get the number of data layers in the scan
    size_t layer_count() const { return data_info_.size(); }
    /// Get the 'nth' data layer
    Data::Ptr layer(size_t i);
    /// Get the 'nth' data layer
    Data::ConstPtr layer(size_t i) const;
    /// Get a data layer based on it's quantity (or NULL if no such data)
    Data::Ptr layer(Quantity quantity);
    Data::ConstPtr layer(Quantity quantity) const;

    /// Add a new data or quality layer to the scan
    Data::Ptr add_layer(
          Quantity quantity
        , bool is_quality
        , const float* data
        , float no_data
        , float undetect);

  private:
    struct DataInfo
    {
      bool      is_quality; ///< True if data, false if quality
      size_t    index;      ///< Index of dataX/qualityX in file
      Quantity  quantity;   ///< Quantity stored by data layer
    };
    typedef std::vector<DataInfo> DataInfoStore_t;

  private:
    /// Create new scan in file
    Scan(
          const Base& parent
        , size_t index            ///< Scan number in file (datasetX)
        , double elevation        ///< Scan elevation angle (degrees above horizon)
        , size_t azimuth_count    ///< Number of azimuths in scan
        , size_t range_bin_count  ///< Number of range bins per azimuth
        , size_t first_azimuth    ///< Index of first azimuth to be radiated
        , double range_start      ///< Range of start of first bin (m)
        , double range_scale      ///< Distance between bins (m)
        , time_t start_time       ///< Time scan started
        , time_t end_time         ///< Time scan ended
        );
    /// Create handle to a scan that is existing in the file
    Scan(const Base& parent, size_t index);

  private:
    // Size required by data layers
    // Note: Multiple variable declaration is deliberate - used as an array internally
    hsize_t azi_count_,   ///< Number of azimuths
            bin_count_;   ///< Number of range bins

    // Handles to any data layers that are present
    DataInfoStore_t   data_info_;       ///< Information about data/quality layers

    friend class Volume;
  };
}

#endif

