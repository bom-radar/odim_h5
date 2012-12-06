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
    static const size_t stats_slots = 4;

    struct stats
    {
      int     samples;    // number of observations contributing to statistics
      double  mean;       // mean of G/R bias
      double  m2;         // sum of squares of difference from current mean
      double  variance;   // variance of G/R bias
    };

    /// Data for a single gauge
    struct gauge
    {
      // gauge id
      int     id;
      float   latitude;
      float   longitude;

      // measurement information
      float   precip_gauge;
      float   precip_radar;

      // bias information
      float   bias;
      double  kalman_beta;      // estimated beta (state)
      double  kalman_variance;
      double  kalman_betap;     // predicted beta (used for evalutaion of filter optimality)

      // rolling stats of observed G/R bias
      // 3 = since start of this calendar month
      // 2 = since start of prev calendar month, etc
      // ...
      // 0 = oldest stats (for current use)
      stats   obs_stats[stats_slots];
    };

  public:
    /// Create a new gauge calibration product
    gauge_calibration(
          const std::string& file
        , int station_id, const std::string& station_name
        , time_t start_time, time_t valid_time
        , double latitude, double longitude, double height
        , long proj_size_x, long proj_size_y
        , double proj_scale_x, double proj_scale_y
        , size_t gauge_count);

    /// Open an existing gauge calibration product
    gauge_calibration(const std::string& file, bool read_only);
    
    /// Ensure all previous writes are flushed to disk
    void flush() const;

    /// Get the radar station name
    int station_id() const { return get_att<int>(root_, "station_id"); }
    /// Wrtie the radar station name
    void set_station_id(int id) { set_att(root_, "station_id", id); }
    
    /// Get the radar station name
    std::string station_name() const { return get_att<std::string>(root_, "station_name"); }
    /// Wrtie the radar station name
    void set_station_name(const std::string& name) { set_att(root_, "station_name", name); }
    
    /// Get the start time for the product
    time_t start_time() const { return get_att<time_t>(root_, atn_start_date, atn_start_time); }
    /// Write the start time for the product
    void set_start_time(time_t start_time) { set_att(root_, atn_start_date, atn_start_time, start_time); }

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

    /// Read the X dimension of the radar accumulation projection
    long proj_size_x() const { return get_att<long>(root_, atn_xsize); }
    /// Write the X dimension of the radar accumultion projection
    void set_proj_size_x(long size) { set_att(root_, atn_xsize, size); }

    /// Read the Y dimension of the radar accumulation projection
    long proj_size_y() const { return get_att<long>(root_, atn_ysize); }
    /// Write the Y dimension of the radar accumultion projection
    void set_proj_size_y(long size) { set_att(root_, atn_ysize, size); }

    /// Read the X dimension of the radar accumulation projection
    double proj_scale_x() const { return get_att<double>(root_, atn_xscale); }
    /// Write the X dimension of the radar accumultion projection
    void set_proj_scale_x(double scale) { set_att(root_, atn_xscale, scale); }

    /// Read the Y dimension of the radar accumulation projection
    double proj_scale_y() const { return get_att<double>(root_, atn_yscale); }
    /// Write the Y dimension of the radar accumultion projection
    void set_proj_scale_y(double scale) { set_att(root_, atn_yscale, scale); }

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

