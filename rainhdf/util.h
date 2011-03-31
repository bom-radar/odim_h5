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

namespace RainHDF
{
  /// List of supported ODIM_H5 object types
  enum ObjectType
  {
      kObj_VolumePolar        ///< Polar volume
    , kObj_VolumeCartesian    ///< Cartesian volume
    , kObj_Scan               ///< Polar scan
    , kObj_Ray                ///< Single polar ray
    , kObj_Azimuth            ///< Azimuthal object
    , kObj_Image              ///< 2-D cartesian image
    , kObj_CompositeImage     ///< Cartesian composite images(s)
    , kObj_CrossSection       ///< 2-D vertical cross sections(s)
    , kObj_VerticalProfile    ///< 1-D vertical profile
    , kObj_Picture            ///< Embedded graphical image

    , kObjectTypeCount
  };
  /// Traits for the object type enumerate
  template <>
  struct enum_traits<ObjectType>
  {
    enum { kCount = (int) kObjectTypeCount };
    static const char * kName;
    static const char * kStrings[];
  };

  /// Product types supported by ODIM_H5
  enum ProductType
  {
      kProd_Scan                  ///< Scan of polar data
    , kProd_PPI                   ///< Plan position indicator
    , kProd_CAPPI                 ///< Constant altitude PPI
    , kProd_PsuedoCAPPI           ///< Psuedo-CAPPI
    , kProd_EchoTop               ///< Echo top
    , kProd_Maximum               ///< Maximum
    , kProd_Accumulation          ///< Accumulation
    , kProd_VIL                   ///< Vertically integrated liquid water
    , kProd_Composite             ///< Composite
    , kProd_VerticalProfile       ///< Vertical profile
    , kProd_RangeHeight           ///< Range height indicator
    , kProd_VerticalSlice         ///< Arbitrary vertical slice
    , kProd_VerticalSidePanel     ///< Vertical side panel
    , kProd_HorizontalSidePanel   ///< Horizontal side panel
    , kProd_Ray                   ///< Ray
    , kProd_Azimuth               ///< Azimuthal type product
    , kProd_Quality               ///< Quality metric
    
    , kProductTypeCount
  };
  /// Traits for the product type enumerate
  template <>
  struct enum_traits<ProductType>
  {
    enum { kCount = (int) kProductTypeCount };
    static const char * kName;
    static const char * kStrings[];
  };

  /// Variable quantities supported by ODIM_H5
  enum Quantity
  {
    // ODIM specified quantities
      kQty_TH             ///< Horizontally-polarized total (uncorrected) reflectivity factor (dBZ)
    , kQty_TV             ///< Vertically-polarized total (uncorrected) reflectivity factor (dBZ)
    , kQty_DBZH           ///< Horizontally-polarized (corrected) reflectivity factor (dBZ)
    , kQty_DBZV           ///< Vertically-polarized (corrected) reflectivity factor (dBZ)
    , kQty_ZDR            ///< Differential reflectivity (dBZ)
    , kQty_RHOHV          ///< Correlation between Zh and Zv [0-1]
    , kQty_LDR            ///< Linear depolarization info (dB)
    , kQty_PHIDP          ///< Differential phase (degrees)
    , kQty_KDP            ///< Specific differential phase (degrees/km)
    , kQty_SQI            ///< Signal quality index [0-1]
    , kQty_SNR            ///< Normalized signal-to-noise ratio [0-1]
    , kQty_RATE           ///< Rain rate (mm/h)
    , kQty_ACRR           ///< Accumulated precipitation (mm)
    , kQty_HGHT           ///< Height of echotops (km)
    , kQty_VIL            ///< Vertical Integrated Liquid water (kg/m2)
    , kQty_VRAD           ///< Radial velocity (m/s)
    , kQty_WRAD           ///< Spectral width of radial velocity (m/s)
    , kQty_UWND           ///< Component of wind in x-direction (m/s)
    , kQty_VWND           ///< Component of wind in y-direction (m/s)
    , kQty_BRDR           ///< 1 denotes border between radars in composite, 0 otherwise
    , kQty_QIND           ///< Spatially analyzed quality indicator, according to OPERA II [0-1]
    , kQty_CLASS          ///< Classified according to legend

    // Extended quantities unique to BOM
    , kQty_Occultation    ///< Proportion of bin that is occulded [0-1]
    , kQty_Attenuation    ///< Fraction of bin power that is attenuated (PIA) [0-1]
    , kQty_ClutterProb    ///< Probability that bin is clutter [0-1]
    , kQty_HailProb       ///< Probability that bin is hail [0-1]

    , kQty_CleanDBZH      ///< Copy of DBZH that has been cleaned
    , kQty_CleanVRAD      ///< Copy of VRAD that has been cleaned

    , kQty_Generic        ///< Generic quantity for general purpose use

    , kQuantityCount
  };
  /// Traits for the quantity enumerate
  template <>
  struct enum_traits<Quantity>
  {
    enum { kCount = (int) kQuantityCount };
    static const char * kName;
    static const char * kStrings[];
  };

  /// Methods supported by ODIM_H5
  enum Method
  {
      kMth_Nearest        ///< Nearest neighbour or closest radar
    , kMth_Interpolate    ///< Interpolation
    , kMth_Average        ///< Average of all values
    , kMth_Random         ///< Random
    , kMth_MinDistEarth   ///< Minimum distance to earth
    , kMth_Latest         ///< Most recent radar
    , kMth_Maximum        ///< Maximum value
    , kMth_Domain         ///< User-defined compositing
    , kMth_VAD            ///< Velocity azimuth display
    , kMth_VVP            ///< Volume velocity procesing
    , kMth_GaugeAdjust    ///< Gauge-adjustment

    , kMethodCount
  };
  /// Traits for the method enumerate
  template <>
  struct enum_traits<Method>
  {
    enum { kCount = (int) kMethodCount };
    static const char * kName;
    static const char * kStrings[];
  };

  /// Optional scalar quality attributes (longs)
  enum Attribute
  {
      kAtt_Task               ///< Name of the acquisition task or product generator
    , kAtt_StartEpoch         ///< Product start time (UNIX epoch)
    , kAtt_EndEpoch           ///< Product end time (UNIX epoch)
    , kAtt_System             ///< Radar system
    , kAtt_Software           ///< Processing software
    , kAtt_SoftwareVersion    ///< Software version
    , kAtt_ZR_A               ///< Z-R constant A in Z = AR^b
    , kAtt_ZR_B               ///< Z-R exponent b in Z = AR^b
    , kAtt_KR_A               ///< K-R constant A in R = AK^b
    , kAtt_KR_B               ///< K-R exponent b in R = AK^b
    , kAtt_Simulated          ///< True if data is simulated
    , kAtt_BeamWidth          ///< Radar half power beam width (degrees)
    , kAtt_Wavelength         ///< Wavelength (cm)
    , kAtt_RPM                ///< Antenna revolutions per minute
    , kAtt_PulseWidth         ///< Pulse width in micro-seconds (us)
    , kAtt_LowPRF             ///< Low pulse repetition frequency (Hz)
    , kAtt_HighPRF            ///< High pulse repitition frequency (Hz)
    , kAtt_AzimuthMethod      ///< How raw data in azimuth are processed to arrive at given value
    , kAtt_RangeMethod        ///< How raw data in range are processed to arrive at given value
    , kAtt_AzimuthAngles      ///< Azimuthal start and stop angles for each gate (degrees) (SEQUENCE)
    , kAtt_ElevationAngles    ///< Elevation angles for each azimuth (degrees) (SEQUENCE)
    , kAtt_AzimuthTimes       ///< Start/stop times for each azimuth gate in scan (SEQUENCE)
    , kAtt_Angles             ///< Elevation angles used to generate the product (degrees) (SEQUENCE)
    , kAtt_RotationSpeed      ///< Antenna rotation speed (SEQUENCE)
    , kAtt_CartesianMethod    ///< How cartesian data are processed
    , kAtt_Nodes              ///< Radar nodes that contributed to the composite (SEQUENCE)
    , kAtt_AccumImgCount      ///< Number of images used in precipitation accumulation
    , kAtt_MinRange           ///< Minimum range of data used when generating a profile (km)
    , kAtt_MaxRange           ///< Maximum range of data used when generating a profile (km)
    , kAtt_NyquistVelocity    ///< Unambiguous velocity (Nyquist) interval (+-m/s)
    , kAtt_Dealiased          ///< True if data has been dealiased
    , kAtt_ElevationAccuracy  ///< Antenna pointing accuracy in elevation (degrees)
    , kAtt_AzimuthAccuracy    ///< Antenna pointing accuracy in azimuth (degrees)
    , kAtt_Malfunction        ///< Radar malfunction indicator (true indicates malfunction)
    , kAtt_MalfunctionMsg     ///< Radar malfunction message
    , kAtt_RadarHorizon       ///< Radar horizon - maximum range (km)
    , kAtt_MDS                ///< Minimum detectable signal at 10km (dBm)
    , kAtt_OUR                ///< Overall uptime reliability (%)
    , kAtt_DopplerFilters     ///< Doppler clutter filters used when collecting data (SEQUENCE)
    , kAtt_Comment            ///< Free text description
    , kAtt_SQI                ///< Signal Quality Index threshold value
    , kAtt_CSR                ///< Clutter-to-signal ratio threshold value
    , kAtt_LOG                ///< Security distance above mean noise level threshold value (dB)
    , kAtt_VPRCorrection      ///< True if VPR correction has been applied
    , kAtt_FreezeLevel        ///< Freezing level above sea level (km)
    , kAtt_Min                ///< Minimum value for continuous quality data
    , kAtt_Max                ///< Maximum value for continuous quality data
    , kAtt_Step               ///< Step value for continuous quality data
    , kAtt_LevelCount         ///< Number of levels in discrete data legend
    , kAtt_PeakPower          ///< Peak power (kW)
    , kAtt_AveragePower       ///< Average power (W)
    , kAtt_DynamicRange       ///< Dynamic range (dB)
    , kAtt_RAC                ///< Range attenuation correction (dBm)
    , kAtt_BBCorrection       ///< True if bright-band correction has been applied
    , kAtt_PAC                ///< Precipitation attenuation correction (dBm)
    , kAtt_SignalToNoise      ///< Signal-to-noise ratio threshold value (dB)
    , kAtt_Polarization       ///< Type of polarization transmitted by the radar (H,V)

    // Extended scalars unique to BOM
    , kAtt_ConfigurationQC    ///< Configuration parameters for system used to perform QC

    , kAttributeCount
  };
  /// Traits for the attribute enumerate
  template <>
  struct enum_traits<Attribute>
  {
    enum { kCount = (int) kAttributeCount };
    static const char * kName;
    static const char * kStrings[];
  };

  /// Type used to store presence flags for optional attributes
  typedef bitset<Attribute> AttFlags;

  // Retrieve existing attributes
  void get_att(const HID_Handle &hid, const char *name, bool &val);
  void get_att(const HID_Handle &hid, const char *name, long &val);
  void get_att(const HID_Handle &hid, const char *name, double &val);
  void get_att(const HID_Handle &hid, const char *name, char *buf, size_t buf_size);
  void get_att(const HID_Handle &hid, const char *name, std::string &val);
  void get_att(const HID_Handle &hid, const char *name_date, const char *name_time, time_t &val);

  // Create new attributes
  void new_att(const HID_Handle &hid, const char *name, bool val);
  void new_att(const HID_Handle &hid, const char *name, long val);
  void new_att(const HID_Handle &hid, const char *name, double val);
  void new_att(const HID_Handle &hid, const char *name, const char *val);
  void new_att(const HID_Handle &hid, const char *name, const std::string &val);
  void new_att(const HID_Handle &hid, const char *name_date, const char *name_time, time_t val);
  inline void new_att(const HID_Handle &hid, const char *name, float val)
  {
    new_att(hid, name, (double) val);
  }
  inline void new_att(const HID_Handle &hid, const char *name, char *val)
  {
    new_att(hid, name, (const char *) val);
  }

  // Alter existing attributes (or create if not existing)
  void set_att(const HID_Handle &hid, const char *name, bool val);
  void set_att(const HID_Handle &hid, const char *name, long val);
  void set_att(const HID_Handle &hid, const char *name, double val);
  void set_att(const HID_Handle &hid, const char *name, const char *val);
  void set_att(const HID_Handle &hid, const char *name, const std::string &val);
  void set_att(const HID_Handle &hid, const char *name_date, const char *name_time, time_t val);
  inline void set_att(const HID_Handle &hid, const char *name, float val)
  {
    set_att(hid, name, (double) val);
  }
  inline void set_att(const HID_Handle &hid, const char *name, char *val) 
  { 
    set_att(hid, name, (const char *) val); 
  }

  template <typename T>
  void get_att(const HID_Handle &hid, const char *name, T &val)
  {
    char buf[128];
    get_att(hid, name, buf, sizeof(buf));
    val = from_string<T>(buf);
  }

  template <typename T>
  void new_att(const HID_Handle &hid, const char *name, T val)
  {
    new_att(hid, name, to_string(val));
  }

  template <typename T>
  void set_att(const HID_Handle &hid, const char *name, T val)
  {
    set_att(hid, name, to_string(val));
  }

  // Determine which 'how' attributes are present for an object
  void check_attribs_presence(const HID_Handle &hid, AttFlags &flags);

  // Convenient value returning versions of above functions (for use in initializer lists)
  template <class T>
  inline T get_att(const HID_Handle &hid, const char *name)
  {
    T t;
    get_att(hid, name, t);
    return t;
  }
  template <class T>
  inline T get_att(const HID_Handle &hid, const char *name1, const char *name2)
  {
    T t;
    get_att(hid, name1, name2, t);
    return t;
  }

  // Group name constants
  extern const char * kGrp_What;
  extern const char * kGrp_Where;
  extern const char * kGrp_How;
  extern const char * kGrp_Dataset;
  extern const char * kGrp_Data;
  extern const char * kGrp_Quality;

  // Attribute name constants
  extern const char * kAtn_Conventions;
  extern const char * kAtn_Object;
  extern const char * kAtn_Version;
  extern const char * kAtn_Date;
  extern const char * kAtn_Time;
  extern const char * kAtn_Source;
  extern const char * kAtn_Latitude;
  extern const char * kAtn_Longitude;
  extern const char * kAtn_Height;
  extern const char * kAtn_Product;
  extern const char * kAtn_StartDate;
  extern const char * kAtn_StartTime;
  extern const char * kAtn_EndDate;
  extern const char * kAtn_EndTime;
  extern const char * kAtn_Elevation;
  extern const char * kAtn_FirstAzimuth;
  extern const char * kAtn_RangeCount;
  extern const char * kAtn_RangeStart;
  extern const char * kAtn_RangeScale;
  extern const char * kAtn_AzimuthCount;
  extern const char * kAtn_Quantity;
  extern const char * kAtn_Gain;
  extern const char * kAtn_Offset;
  extern const char * kAtn_NoData;
  extern const char * kAtn_Undetect;
  extern const char * kAtn_Class;
  extern const char * kAtn_ImageVersion;

  // Dataset name constants
  extern const char * kDat_Data;

  // Attribute value constants
  extern const char * kVal_True;
  extern const char * kVal_False;
  extern const char * kVal_Conventions;
  extern const char * kVal_Version;
  extern const char * kVal_Class;
  extern const char * kVal_ImageVersion;
}

#endif

