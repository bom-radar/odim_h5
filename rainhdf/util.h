/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_UTIL_H
#define RAINHDF_UTIL_H

#include "enum.h"
#include "error.h"
#include "raii.h"

#include <hdf5.h>
#include <string>

namespace rainhdf
{
  /// List of supported ODIM_H5 object types
  enum object_type
  {
      ot_volume_polar       ///< Polar volume
    , ot_volume_cartesian   ///< Cartesian volume
    , ot_scan               ///< Polar scan
    , ot_ray                ///< Single polar ray
    , ot_azimuth            ///< Azimuthal object
    , ot_image              ///< 2-D cartesian image
    , ot_composite_image    ///< Cartesian composite images(s)
    , ot_cross_section      ///< 2-D vertical cross sections(s)
    , ot_vertical_profile   ///< 1-D vertical profile
    , ot_picture            ///< Embedded graphical image

    , count_ot
  };
  /// Traits for the object type enumerate
  template <>
  struct enum_traits<object_type>
  {
    enum { count = (int) count_ot };
    static const char* name;
    static const char* strings[];
  };

  /// Product types supported by ODIM_H5
  enum product_type
  {
      pt_scan                   ///< Scan of polar data
    , pt_ppi                    ///< Plan position indicator
    , pt_cappi                  ///< Constant altitude PPI
    , pt_psuedo_cappi           ///< Psuedo-CAPPI
    , pt_echo_top               ///< Echo top
    , pt_maximum                ///< Maximum
    , pt_accumulation           ///< Accumulation
    , pt_vil                    ///< Vertically integrated liquid water
    , pt_composite              ///< Composite
    , pt_vertical_profile       ///< Vertical profile
    , pt_range_height            ///< Range height indicator
    , pt_vertical_slice         ///< Arbitrary vertical slice
    , pt_vertical_side_panel    ///< Vertical side panel
    , pt_horizontal_side_panel  ///< Horizontal side panel
    , pt_ray                    ///< Ray
    , pt_azimuth                ///< Azimuthal type product
    , pt_quality                ///< Quality metric
    
    , count_pt
  };
  /// Traits for the product type enumerate
  template <>
  struct enum_traits<product_type>
  {
    enum { count = (int) count_pt };
    static const char* name;
    static const char* strings[];
  };

  /// Variable quantities supported by ODIM_H5
  enum quantity
  {
    // ODIM specified quantities
      qty_th            ///< Horizontally-polarized total (uncorrected) reflectivity factor (dBZ)
    , qty_tv            ///< Vertically-polarized total (uncorrected) reflectivity factor (dBZ)
    , qty_dbzh          ///< Horizontally-polarized (corrected) reflectivity factor (dBZ)
    , qty_dbzv          ///< Vertically-polarized (corrected) reflectivity factor (dBZ)
    , qty_zdr           ///< Differential reflectivity (dBZ)
    , qty_rhohv         ///< Correlation between Zh and Zv [0-1]
    , qty_ldr           ///< Linear depolarization info (dB)
    , qty_phidp         ///< Differential phase (degrees)
    , qty_kdp           ///< Specific differential phase (degrees/km)
    , qty_sqi           ///< Signal quality index [0-1]
    , qty_snr           ///< Normalized signal-to-noise ratio [0-1]
    , qty_rate          ///< Rain rate (mm/h)
    , qty_acrr          ///< Accumulated precipitation (mm)
    , qty_hght          ///< Height of echotops (km)
    , qty_vil           ///< Vertical Integrated Liquid water (kg/m2)
    , qty_vrad          ///< Radial velocity (m/s)
    , qty_wrad          ///< Spectral width of radial velocity (m/s)
    , qty_uwnd          ///< Component of wind in x-direction (m/s)
    , qty_vwnd          ///< Component of wind in y-direction (m/s)
    , qty_brdr          ///< 1 denotes border between radars in composite, 0 otherwise
    , qty_qind          ///< Spatially analyzed quality indicator, according to OPERA II [0-1]
    , qty_class         ///< Classified according to legend

    // Extended quantities unique to BOM
    , qty_occultation   ///< Proportion of bin that is occulded [0-1]
    , qty_attenuation   ///< Fraction of bin power that is attenuated (PIA) [0-1]
    , qty_clutter_prob  ///< Probability that bin is clutter [0-1]
    , qty_hail_prob     ///< Probability that bin is hail [0-1]

    , qty_dbzh_clean    ///< Copy of DBZH that has been cleaned
    , qty_vrad_clean    ///< Copy of VRAD that has been cleaned

    , qty_generic       ///< Generic quantity for general purpose use

    , count_qt
  };
  /// Traits for the quantity enumerate
  template <>
  struct enum_traits<quantity>
  {
    enum { count = (int) count_qt };
    static const char* name;
    static const char* strings[];
  };

  /// Methods supported by ODIM_H5
  enum method
  {
      mth_nearest         ///< Nearest neighbour or closest radar
    , mth_interpolate     ///< Interpolation
    , mth_average         ///< Average of all values
    , mth_random          ///< Random
    , mth_min_dist_earth  ///< Minimum distance to earth
    , mth_latest          ///< Most recent radar
    , mth_maximum         ///< Maximum value
    , mth_domain          ///< User-defined compositing
    , mth_vad             ///< Velocity azimuth display
    , mth_vvp             ///< Volume velocity procesing
    , mth_gauge_adjust    ///< Gauge-adjustment

    , count_mth
  };
  /// Traits for the method enumerate
  template <>
  struct enum_traits<method>
  {
    enum { count = (int) count_mth };
    static const char* name;
    static const char* strings[];
  };

  /// Optional scalar quality attributes (longs)
  enum attribute
  {
      att_task                ///< Name of the acquisition task or product generator
    , att_start_epoch         ///< Product start time (UNIX epoch)
    , att_end_epoch           ///< Product end time (UNIX epoch)
    , att_system              ///< Radar system
    , att_software            ///< Processing software
    , att_software_version    ///< Software version
    , att_zr_a                ///< Z-R constant A in Z = AR^b
    , att_zr_b                ///< Z-R exponent b in Z = AR^b
    , att_kr_a                ///< K-R constant A in R = AK^b
    , att_kr_b                ///< K-R exponent b in R = AK^b
    , att_simulated           ///< True if data is simulated
    , att_beam_width          ///< Radar half power beam width (degrees)
    , att_wave_length         ///< Wavelength (cm)
    , att_rpm                 ///< Antenna revolutions per minute
    , att_pulse_width         ///< Pulse width in micro-seconds (us)
    , att_rx_bandwidth        ///< Bandwidth (MHz) that the receiver is set to hwen operating the radar with att_pulse_width
    , att_low_prf             ///< Low pulse repetition frequency (Hz)
    , att_high_prf            ///< High pulse repitition frequency (Hz)
    , att_tx_loss             ///< Total loss in dB in the transmission chain between the signal generator and the feed horn
    , att_rx_loss             ///< Total loss in dB in the receiving chain between the feed and the receiver
    , att_radome_loss         ///< One-way dry radome loss in dB
    , att_antenna_gain        ///< Antenna gain in dB
    , att_beam_width_h        ///< Horizontal half-power (-3dB) beamwidth in degrees
    , att_beam_width_v        ///< Vertical half-power (-3dB) beamwidth in degrees
    , att_gas_attn            ///< Gaseous specific attnuation in dB/km assumed by the radar processor
    , att_rad_const_h         ///< Radar constant in dB for the horizontal channel
    , att_rad_const_v         ///< Radar constant in dB for the vertical channel
    , att_nom_tx_power        ///< Nominal transmitted peak power in kW
    , att_tx_power            ///< Transmitted peak power in kW (SIMPLE ARRAY OF DOUBLES)
    , att_nyquist_velocity    ///< Unambiguous velocity (Nyquist) interval (+-m/s)
    , att_v_samples           ///< Number of samples used for radial velocity measurements
    , att_azimuth_method      ///< How raw data in azimuth are processed to arrive at given value
    , att_range_method        ///< How raw data in range are processed to arrive at given value
    , att_elevation_angles    ///< Elevation angles for each azimuth (degrees) (DOUBLE ARRAY)
    , att_az_start_angles     ///< Start angles (degrees) for each azimuth gate (DOUBLE ARRAY)
    , att_az_stop_angles      ///< End angles (degrees) for each azimuth gate (DOUBLE ARRAY)
    , att_az_start_times      ///< Start times for each azimuth gate (DOUBLE ARRAY)
    , att_az_stop_times       ///< End times for each azimuth gate (DOUBLE ARRAY)
    , att_angles              ///< Elevation angles used to generate the product (degrees) (SEQUENCE)
    , att_rotation_speed      ///< Antenna rotation speed (SEQUENCE)
    , att_cartesian_method    ///< How cartesian data are processed
    , att_nodes               ///< Radar nodes that contributed to the composite (SEQUENCE)
    , att_accum_img_count     ///< Number of images used in precipitation accumulation
    , att_min_range           ///< Minimum range of data used when generating a profile (km)
    , att_max_range           ///< Maximum range of data used when generating a profile (km)
    , att_dealiased           ///< True if data has been dealiased
    , att_elevation_accuracy  ///< Antenna pointing accuracy in elevation (degrees)
    , att_azimuth_accuracy    ///< Antenna pointing accuracy in azimuth (degrees)
    , att_malfunction         ///< Radar malfunction indicator (true indicates malfunction)
    , att_malfunction_msg     ///< Radar malfunction message
    , att_radar_horizon       ///< Radar horizon - maximum range (km)
    , att_nez                 ///< Total system noise expressed as reflectivity (dBZ) at 1km
    , att_our                 ///< Overall uptime reliability (%)
    , att_doppler_filters     ///< Doppler clutter filters used when collecting data (SEQUENCE)
    , att_comment             ///< Free text description
    , att_sqi                 ///< Signal Quality Index threshold value
    , att_csr                 ///< Clutter-to-signal ratio threshold value
    , att_log                 ///< Security distance above mean noise level threshold value (dB)
    , att_vpr_correction      ///< True if VPR correction has been applied
    , att_freeze_level        ///< Freezing level above sea level (km)
    , att_min                 ///< Minimum value for continuous quality data
    , att_max                 ///< Maximum value for continuous quality data
    , att_step                ///< Step value for continuous quality data
    , att_level_count         ///< Number of levels in discrete data legend
    , att_peak_power          ///< Peak power (kW)
    , att_average_power       ///< Average power (W)
    , att_dynamic_range       ///< Dynamic range (dB)
    , att_rac                 ///< Range attenuation correction (dBm)
    , att_bb_correction       ///< True if bright-band correction has been applied
    , att_pac                 ///< Precipitation attenuation correction (dBm)
    , att_signal_to_noise     ///< Signal-to-noise ratio threshold value (dB)
    , att_polarization        ///< Type of polarization transmitted by the radar (H,V)

    // Extended scalars unique to BOM
    , att_configuration_qc    ///< Configuration parameters for system used to perform QC

    , count_att
  };
  /// Traits for the attribute enumerate
  template <>
  struct enum_traits<attribute>
  {
    enum { count = (int) count_att };
    static const char* name;
    static const char* strings[];
  };

  /// Type used to store presence flags for optional attributes
  typedef bitset<attribute> att_flags;

  // Retrieve existing attributes
  void get_att(const hid_handle& hid, const char* name, bool& val);
  void get_att(const hid_handle& hid, const char* name, long& val);
  void get_att(const hid_handle& hid, const char* name, double& val);
  void get_att(const hid_handle& hid, const char* name, char* buf, size_t buf_size);
  void get_att(const hid_handle& hid, const char* name, std::string& val);
  void get_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t& val);

  // Create new attributes
  void new_att(const hid_handle& hid, const char* name, bool val);
  void new_att(const hid_handle& hid, const char* name, long val);
  void new_att(const hid_handle& hid, const char* name, double val);
  void new_att(const hid_handle& hid, const char* name, const char* val);
  void new_att(const hid_handle& hid, const char* name, const std::string& val);
  void new_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t val);
  inline void new_att(const hid_handle& hid, const char* name, float val)
  {
    new_att(hid, name, (double) val);
  }
  inline void new_att(const hid_handle& hid, const char* name, char* val)
  {
    new_att(hid, name, (const char*) val);
  }

  // Alter existing attributes (or create if not existing)
  void set_att(const hid_handle& hid, const char* name, bool val);
  void set_att(const hid_handle& hid, const char* name, long val);
  void set_att(const hid_handle& hid, const char* name, double val);
  void set_att(const hid_handle& hid, const char* name, const char* val);
  void set_att(const hid_handle& hid, const char* name, const std::string& val);
  void set_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t val);
  inline void set_att(const hid_handle& hid, const char* name, float val)
  {
    set_att(hid, name, (double) val);
  }
  inline void set_att(const hid_handle& hid, const char* name, char* val) 
  { 
    set_att(hid, name, (const char*) val); 
  }

  template <typename T>
  void get_att(const hid_handle& hid, const char* name, T& val)
  {
    char buf[128];
    get_att(hid, name, buf, sizeof(buf));
    val = from_string<T>(buf);
  }

  template <typename T>
  void new_att(const hid_handle& hid, const char* name, T val)
  {
    new_att(hid, name, to_string(val));
  }

  template <typename T>
  void set_att(const hid_handle& hid, const char* name, T val)
  {
    set_att(hid, name, to_string(val));
  }

  // Determine which 'how' attributes are present for an object
  void check_attribs_presence(const hid_handle& hid, att_flags& flags);

  // Convenient value returning versions of above functions (for use in initializer lists)
  template <class T>
  inline T get_att(const hid_handle& hid, const char* name)
  {
    T t;
    get_att(hid, name, t);
    return t;
  }
  template <class T>
  inline T get_att(const hid_handle& hid, const char* name1, const char* name2)
  {
    T t;
    get_att(hid, name1, name2, t);
    return t;
  }

  // Group name constants
  extern const char* grp_what;
  extern const char* grp_where;
  extern const char* grp_how;
  extern const char* grp_dataset;
  extern const char* grp_data;
  extern const char* grp_quality;

  // Attribute name constants
  extern const char* atn_conventions;
  extern const char* atn_object;
  extern const char* atn_version;
  extern const char* atn_date;
  extern const char* atn_time;
  extern const char* atn_source;
  extern const char* atn_latitude;
  extern const char* atn_longitude;
  extern const char* atn_height;
  extern const char* atn_product;
  extern const char* atn_start_date;
  extern const char* atn_start_time;
  extern const char* atn_end_date;
  extern const char* atn_end_time;
  extern const char* atn_elevation;
  extern const char* atn_first_azimuth;
  extern const char* atn_range_count;
  extern const char* atn_range_start;
  extern const char* atn_range_scale;
  extern const char* atn_azimuth_count;
  extern const char* atn_quantity;
  extern const char* atn_gain;
  extern const char* atn_offset;
  extern const char* atn_no_data;
  extern const char* atn_undetect;
  extern const char* atn_class;
  extern const char* atn_image_version;

  // Dataset name constants
  extern const char* dat_data;

  // Attribute value constants
  extern const char* val_true;
  extern const char* val_false;
  extern const char* val_conventions;
  extern const char* val_version;
  extern const char* val_class;
  extern const char* val_image_version;
}

#endif

