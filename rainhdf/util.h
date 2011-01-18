/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_UTIL_H
#define RAINHDF_UTIL_H

#include "error.h"
#include "raii.h"

#include <hdf5.h>
#include <string>

namespace RainHDF
{
  /// List of supported ODIM_H5 object types
  enum ObjectType
  {
      kOT_VolumePolar       ///< Polar volume
    , kOT_VolumeCartesian   ///< Cartesian volume
    , kOT_Scan              ///< Polar scan
    , kOT_Ray               ///< Single polar ray
    , kOT_Azimuth           ///< Azimuthal object
    , kOT_Image             ///< 2-D cartesian image
    , kOT_CompositeImage    ///< Cartesian composite images(s)
    , kOT_CrossSection      ///< 2-D vertical cross sections(s)
    , kOT_VerticalProfile   ///< 1-D vertical profile
    , kOT_Picture           ///< Embedded graphical image

    , kOT_Unknown           ///< Invalid or unknown object type
  };

  /// Product types supported by ODIM_H5
  enum ProductType
  {
      kPT_Scan                  ///< Scan of polar data
    , kPT_PPI                   ///< Plan position indicator
    , kPT_CAPPI                 ///< Constant altitude PPI
    , kPT_PsuedoCAPPI           ///< Psuedo-CAPPI
    , kPT_EchoTop               ///< Echo top
    , kPT_Maximum               ///< Maximum
    , kPT_Accumulation          ///< Accumulation
    , kPT_VIL                   ///< Vertically integrated liquid water
    , kPT_Composite             ///< Composite
    , kPT_VerticalProfile       ///< Vertical profile
    , kPT_RangeHeight           ///< Range height indicator
    , kPT_VerticalSlice         ///< Arbitrary vertical slice
    , kPT_VerticalSidePanel     ///< Vertical side panel
    , kPT_HorizontalSidePanel   ///< Horizontal side panel
    , kPT_Ray                   ///< Ray
    , kPT_Azimuth               ///< Azimuthal type product
    , kPT_Quality               ///< Quality metric
    
    , kPT_Unknown               ///< Invalid or unknown product type
  };

  /// Variable quantities supported by ODIM_H5
  enum Quantity
  {
      kQty_TH      ///< Horizontally-polarized total (uncorrected) reflectivity factor (dBZ)
    , kQty_TV      ///< Vertically-polarized total (uncorrected) reflectivity factor (dBZ)
    , kQty_DBZH    ///< Horizontally-polarized (corrected) reflectivity factor (dBZ)
    , kQty_DBZV    ///< Vertically-polarized (corrected) reflectivity factor (dBZ)
    , kQty_ZDR     ///< Differential reflectivity (dBZ)
    , kQty_RHOHV   ///< Correlation between Zh and Zv [0-1]
    , kQty_LDR     ///< Linear depolarization info (dB)
    , kQty_PHIDP   ///< Differential phase (degrees)
    , kQty_KDP     ///< Specific differential phase (degrees/km)
    , kQty_SQI     ///< Signal quality index [0-1]
    , kQty_SNR     ///< Normalized signal-to-noise ratio [0-1]
    , kQty_RATE    ///< Rain rate (mm/h)
    , kQty_ACRR    ///< Accumulated precipitation (mm)
    , kQty_HGHT    ///< Height of echotops (km)
    , kQty_VIL     ///< Vertical Integrated Liquid water (kg/m2)
    , kQty_VRAD    ///< Radial velocity (m/s)
    , kQty_WRAD    ///< Spectral width of radial velocity (m/s)
    , kQty_UWND    ///< Component of wind in x-direction (m/s)
    , kQty_VWND    ///< Component of wind in y-direction (m/s)
    , kQty_BRDR    ///< 1 denotes border between radars in composite, 0 otherwise
    , kQty_QIND    ///< Spatially analyzed quality indicator, according to OPERA II [0-1]
    , kQty_CLASS   ///< Classified according to legend

    , kQty_Unknown ///< Invalid or unknown qualtity
  };

  /// Optional scalar quality attributes (bool)
  enum OptAttrib_Bool
  {
      kAtt_Simulated          ///< True if data is simulated
    , kAtt_Dealiased          ///< True if data has been dealiased
    , kAtt_Malfunction        ///< Radar malfunction indicator (true indicates malfunction)
    , kAtt_VPRCorrection      ///< True if VPR correction has been applied
    , kAtt_BBCorrection       ///< True if bright-band correction has been applied
  };

  /// Optional scalar quality attributes (longs)
  enum OptAttrib_Long
  {
      kAtt_StartEpoch         ///< Product start time (UNIX epoch)
    , kAtt_EndEpoch           ///< Product end time (UNIX epoch)
    , kAtt_AccumImgCount      ///< Number of images used in precipitation accumulation
    , kAtt_LevelCount         ///< Number of levels in discrete data legend
  };

  /// Optional scalar quality attributes (doubles)
  enum OptAttrib_Double
  {
      kAtt_ZR_A               ///< Z-R constant A in Z = AR^b
    , kAtt_ZR_B               ///< Z-R exponent b in Z = AR^b
    , kAtt_KR_A               ///< K-R constant A in R = AK^b
    , kAtt_KR_B               ///< K-R exponent b in R = AK^b
    , kAtt_BeamWidth          ///< Radar half power beam width (degrees)
    , kAtt_Wavelength         ///< Wavelength (cm)
    , kAtt_RPM                ///< Antenna revolutions per minute
    , kAtt_PulseWidth         ///< Pulse width in micro-seconds (us)
    , kAtt_LowPRF             ///< Low pulse repetition frequency (Hz)
    , kAtt_HighPRF            ///< High pulse repitition frequency (Hz)
    , kAtt_MinRange           ///< Minimum range of data used when generating a profile (km)
    , kAtt_MaxRange           ///< Maximum range of data used when generating a profile (km)
    , kAtt_NyquistVelocity    ///< Unambiguous velocity (Nyquist) interval (+-m/s)
    , kAtt_ElevationAccuracy  ///< Antenna pointing accuracy in elevation (degrees)
    , kAtt_AzimuthAccuracy    ///< Antenna pointing accuracy in azimuth (degrees)
    , kAtt_RadarHorizon       ///< Radar horizon - maximum range (km)
    , kAtt_MDS                ///< Minimum detectable signal at 10km (dBm)
    , kAtt_OUR                ///< Overall uptime reliability (%)
    , kAtt_SQI                ///< Signal Quality Index threshold value
    , kAtt_CSR                ///< Clutter-to-signal ratio threshold value
    , kAtt_LOG                ///< Security distance above mean noise level threshold value (dB)
    , kAtt_FreezeLevel        ///< Freezing level above sea level (km)
    , kAtt_Min                ///< Minimum value for continuous quality data
    , kAtt_Max                ///< Maximum value for continuous quality data
    , kAtt_Step               ///< Step value for continuous quality data
    , kAtt_PeakPower          ///< Peak power (kW)
    , kAtt_AveragePower       ///< Average power (W)
    , kAtt_DynamicRange       ///< Dynamic range (dB)
    , kAtt_RAC                ///< Range attenuation correction (dBm)
    , kAtt_PAC                ///< Precipitation attenuation correction (dBm)
    , kAtt_SignalToNoise      ///< Signal-to-noise ratio threshold value (dB)
  };

  /// Optional scalar quality attributes (strings)
  enum OptAttrib_Str
  {
      kAtt_Task               ///< Name of the acquisition task or product generator
    , kAtt_System             ///< Radar system
    , kAtt_Software           ///< Processing software
    , kAtt_SoftwareVersion    ///< Software version
//enum    , kAtt_AzimuthMethod      ///< How raw data in azimuth are processed to arrive at given value
//enum    , kAtt_RangeMethod        ///< How raw data in range are processed to arrive at given value
//sequ    , kAtt_AzimuthAngles      ///< Azimuthal start and stop angles for each gate (degrees)
//sequ    , kAtt_ElevationAngles    ///< Elevation angles for each azimuth (degrees)
//sequ    , kAtt_AzimuthTimes       ///< Start/stop times for each azimuth gate in scan
//sequ    , kAtt_Angles             ///< Elevation angles used to generate the product (degrees)
//sequ    , kAtt_RotationSpeed      ///< Antenna rotation speed
//enum    , kAtt_CartesianMethod    ///< How cartesian data are processed
//sequ    , kAtt_Nodes              ///< Radar nodes that contributed to the composite
    , kAtt_MalfunctionMsg     ///< Radar malfunction message
//sequ    , kAtt_DopplerFilters     ///< Doppler clutter filters used when collecting data
    , kAtt_Comment            ///< Free text description
    , kAtt_Polarization       ///< Type of polarization transmitted by the radar (H,V)
  };

  // Common group names
  extern const char * kGrp_What;
  extern const char * kGrp_Where;
  extern const char * kGrp_How;
  extern const char * kGrp_Dataset;
  extern const char * kGrp_Data;
  extern const char * kGrp_Quality;

  // Common attribute names
  extern const char * kAtt_Date;
  extern const char * kAtt_Time;

  // Check if a numbered group exists
  bool IndexedGroupExists(hid_t hParent, const char *pszGroup, int nIndex, char *pszNameOut);

  // Create new attributes
  void NewAtt(hid_t hID, const char *pszName, const char *pszVal);
  void NewAtt(hid_t hID, const char *pszName, const std::string &strVal);
  void NewAtt(hid_t hID, const char *pszName, long nVal);
  void NewAtt(hid_t hID, const char *pszName, double fVal);
  void NewAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t tVal);
  void NewAtt(hid_t hID, const char *pszName, ObjectType eVal);
  void NewAtt(hid_t hID, const char *pszName, ProductType eVal);
  void NewAtt(hid_t hID, const char *pszName, Quantity eVal);

  // Alter existing attributes (or create if not existing)
  void SetAtt(hid_t hID, const char *pszName, const char *pszVal);
  void SetAtt(hid_t hID, const char *pszName, const std::string &strVal);
  void SetAtt(hid_t hID, const char *pszName, long nVal);
  void SetAtt(hid_t hID, const char *pszName, double fVal);
  void SetAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t tVal);
  void SetAtt(hid_t hID, const char *pszName, ObjectType eVal);
  void SetAtt(hid_t hID, const char *pszName, ProductType eVal);
  void SetAtt(hid_t hID, const char *pszName, Quantity eVal);

  // Retrieve existing attributes
  void GetAtt(hid_t hID, const char *pszName, char *pszBuf, size_t nBufSize);
  void GetAtt(hid_t hID, const char *pszName, std::string &strVal);
  void GetAtt(hid_t hID, const char *pszName, long &nVal);
  void GetAtt(hid_t hID, const char *pszName, double &fVal);
  void GetAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t &tVal);
  void GetAtt(hid_t hID, const char *pszName, ObjectType &eVal);
  void GetAtt(hid_t hID, const char *pszName, ProductType &eVal);
  void GetAtt(hid_t hID, const char *pszName, Quantity &eVal);

  // Set an optional quality attribute (and create 'how' group if needed)
  void SetHowAtt(hid_t hParent, HID_Group &hHow, OptAttrib_Long eAttr, long nVal);
  void SetHowAtt(hid_t hParent, HID_Group &hHow, OptAttrib_Double eAttr, double fVal);
  void SetHowAtt(hid_t hParent, HID_Group &hHow, OptAttrib_Str eAttr, const char *pszVal);
  void SetHowAtt(hid_t hParent, HID_Group &hHow, OptAttrib_Str eAttr, const std::string &strVal);

  // Get/set an optional quality attribute
  bool GetHowAtt(const HID_Group &hHow, OptAttrib_Double eAttr, double &fVal);

  // Convenient value returning versions of above functions
  template <class T>
  inline T GetAtt(hid_t hID, const char *pszName)
  {
    T t;
    GetAtt(hID, pszName, t);
    return t;
  }
  template <class T>
  inline T GetAtt(hid_t hID, const char *pszName1, const char *pszName2)
  {
    T t;
    GetAtt(hID, pszName1, pszName2, t);
    return t;
  }
}

#endif

