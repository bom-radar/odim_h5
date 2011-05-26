/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_VOLUME_H
#define RAINHDF_VOLUME_H

#include "product.h"
#include "scan.h"

namespace rainhdf
{
  /// ODIM_H5 compliant polar volume file manipulator
  class volume : public product
  {
  public:
    /// Create a new volume product
    volume(
          const std::string& file
        , time_t valid_time
        , double latitude
        , double longitude
        , double height
        );

    /// Open an existing volume product
    volume(const std::string& file, bool read_only);

    /// Read the station latitude
    double latitude() const { return get_att<double>(hnd_where_, atn_latitude); }
    /// Write the station latitude
    void set_latitude(double latitude) { set_att(hnd_where_, atn_latitude, latitude); }

    /// Read the station longitude
    double longitude() const { return get_att<double>(hnd_where_, atn_longitude); }
    /// Write the station longitude
    void set_longitude(double longitude) { set_att(hnd_where_, atn_longitude, longitude); }

    /// Read the station elevation
    double height() const { return get_att<double>(hnd_where_, atn_height); }
    /// Write the station elevation
    void set_height(double height) { set_att(hnd_where_, atn_height, height); }

    /// Get the number of scans in the volume
    size_t scan_count() const { return scan_count_; }
    /// Get the 'nth' scan
    scan::ptr get_scan(size_t i) { return scan::ptr(new scan(*this, i + 1)); }
    /// Get the 'nth' scan
    scan::const_ptr get_scan(size_t i) const { return scan::const_ptr(new scan(*this, i + 1)); }
    /// Add a new scan to the file
    scan::ptr add_scan(
          double elevation        ///< Scan elevation angle (degrees above horizon)
        , size_t azimuth_count    ///< Number of azimuths scanned
        , size_t range_bin_count  ///< Number of range bins
        , size_t first_azimuth    ///< Index of first azimuth to be radiated
        , double range_start      ///< Range of start of first bin (m)
        , double range_scale      ///< Distance between bins (m)
        , time_t start_time       ///< Time scan started
        , time_t end_time         ///< Time scan ended
        );

  private:
    size_t scan_count_; ///< Number of scans in file
  };

  inline scan::ptr volume::add_scan(
        double elevation
      , size_t azimuth_count
      , size_t range_bin_count
      , size_t first_azimuth
      , double range_start
      , double range_scale
      , time_t start_time
      , time_t end_time)
  {
    return scan::ptr(
        new scan(
            *this,
            ++scan_count_,
            elevation,
            azimuth_count,
            range_bin_count,
            first_azimuth,
            range_start,
            range_scale,
            start_time,
            end_time));
  }
};

#endif


