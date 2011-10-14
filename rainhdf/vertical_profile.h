/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_VERTICAL_PROFILE_H
#define RAINHDF_VERTICAL_PROFILE_H

#include "product.h"
#include "profile.h"

namespace rainhdf
{
  /// ODIM_H5 compliant vertical profile file manipulator
  class vertical_profile : public product
  {
  public:
    /// Create a new vertical profile product
    vertical_profile(
          const std::string& file
        , const std::string& source
        , time_t valid_time
        , double latitude
        , double longitude
        , double height
        , size_t levels
        , double interval
        , double min_height
        , double max_height
        );

    /// Open an existing vertical profile product
    vertical_profile(const std::string& file, bool read_only);

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

    /// Read the number of levels in the profile
    size_t levels() const { return levels_; }
    // not allowed to change number of levels!

    /// Read the vertical distance (m) between height intervals
    double interval() const { return get_att<double>(hnd_where_, atn_interval); }
    /// Write the vertical distance (m) between height intervals
    void set_interval(double interval) { set_att(hnd_where_, atn_interval, interval); }

    /// Read the minimum height (m) above mean sea level
    double min_height() const { return get_att<double>(hnd_where_, atn_min_height); }
    /// Write the minimum height (m) above mean sea level
    void set_min_height(double min_height) { set_att(hnd_where_, atn_min_height, min_height); }

    /// Read the maximum height (m) above mean sea level
    double max_height() const { return get_att<double>(hnd_where_, atn_max_height); }
    /// Write the maximum height (m) above mean sea level
    void set_max_height(double max_height) { set_att(hnd_where_, atn_max_height, max_height); }

    /// Get the number of profiles in the file
    size_t profile_count() const { return profile_count_; }
    /// Get the 'nth' profile
    rainhdf::profile::ptr profile(size_t i) { return rainhdf::profile::ptr(new rainhdf::profile(*this, i + 1, levels_)); }
    /// Get the 'nth' profile
    rainhdf::profile::const_ptr profile(size_t i) const { return rainhdf::profile::const_ptr(new rainhdf::profile(*this, i + 1, levels_)); }
    /// Add a new profile to the file
    rainhdf::profile::ptr add_profile(time_t start_time, time_t end_time);

  private:
    size_t        profile_count_; ///< Number of profiles in file
    size_t        levels_;        ///< Number of height levels
  };

  inline rainhdf::profile::ptr vertical_profile::add_profile(time_t start_time, time_t end_time)
  {
    return rainhdf::profile::ptr(
        new rainhdf::profile(
              *this
            , ++profile_count_
            , levels_
            , start_time
            , end_time));
  }
}

#endif

