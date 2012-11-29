/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2012 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_GAUGE_CALIBRATION_H
#define RAINHDF_GAUGE_CALIBRATION_H

#include "util.h"

namespace rainfields {
namespace hdf {
  /// Gauge calibration information for a single radar
  class gauge_calibration
  {
  public:
    /// Data for a single gauge
    struct gauge
    {
      // gauge id
      int     id;
      float   latitude;
      float   longitude;

      // measurement information
      float   rainfall_gauge;
      float   rainfall_radar;

      // bias information
      float   bias;
      time_t  kalman_time;
      double  kalman_beta;
      double  kalman_variance;

      // measurement error rolling stats
      // 0 = since start of this calendar month
      // 1 = since start of prev calendar month, etc
      int     err_samples[4];
      double  err_mean[4];
      double  err_m2[4];      // sum of squares of difference from current mean
      double  err_variance[4];
    };

  public:
    /// Create a new gauge calibration product
    gauge_calibration(
          const std::string& file
        , const std::string& source
        , time_t valid_time
        , double latitude
        , double longitude
        , double height
        , size_t gauge_count);

    /// Open an existing gauge calibration product
    gauge_calibration(const std::string& file, bool read_only);
    
    /// Ensure all previous writes are flushed to disk
    void flush() const;

    /// Get the source string
    std::string source() const { return get_att<std::string>(root_, atn_source); }
    /// Wrtie the product source string
    void set_source(const std::string& source) { set_att(root_, atn_source, source); }
    
    /// Get the nominal (valid) time of the data/product
    time_t valid_time() const { return get_att<time_t>(root_, atn_date, atn_time); }
    /// Write the nominal (valid) time of the data/product
    void set_valid_time(time_t valid_time) { set_att(root_, atn_date, atn_time, valid_time); }

    /// Read the station latitude
    double latitude() const { return get_att<double>(root_, atn_latitude); }
    /// Write the station latitude
    void set_latitude(double latitude) { set_att(root_, atn_latitude, latitude); }

    /// Read the station longitude
    double longitude() const { return get_att<double>(root_, atn_longitude); }
    /// Write the station longitude
    void set_longitude(double longitude) { set_att(root_, atn_longitude, longitude); }

    /// Read the station elevation
    double height() const { return get_att<double>(root_, atn_height); }
    /// Write the station elevation
    void set_height(double height) { set_att(root_, atn_height, height); }

    /// Get the number of gauges stored
    size_t gauge_count() const { return gauge_count_; }

    /// Read the gauge data
    void read_gauges(gauge* gauges) const;
    /// Write the gauge data
    void write_gauges(const gauge* gauges);

  private:
    hid_handle  root_;
    hid_handle  memory_type_;
    hid_handle  gauges_;
    size_t      gauge_count_;
  };
}}

#endif

