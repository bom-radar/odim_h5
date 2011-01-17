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
  enum DataQuantity
  {
      kDQ_TH      ///< Horizontally-polarized total (uncorrected) reflectivity factor (dBZ)
    , kDQ_TV      ///< Vertically-polarized total (uncorrected) reflectivity factor (dBZ)
    , kDQ_DBZH    ///< Horizontally-polarized (corrected) reflectivity factor (dBZ)
    , kDQ_DBZV    ///< Vertically-polarized (corrected) reflectivity factor (dBZ)
    , kDQ_ZDR     ///< Differential reflectivity (dBZ)
    , kDQ_RHOHV   ///< Correlation between Zh and Zv [0-1]
    , kDQ_LDR     ///< Linear depolarization info (dB)
    , kDQ_PHIDP   ///< Differential phase (degrees)
    , kDQ_KDP     ///< Specific differential phase (degrees/km)
    , kDQ_SQI     ///< Signal quality index [0-1]
    , kDQ_SNR     ///< Normalized signal-to-noise ratio [0-1]
    , kDQ_RATE    ///< Rain rate (mm/h)
    , kDQ_ACRR    ///< Accumulated precipitation (mm)
    , kDQ_HGHT    ///< Height of echotops (km)
    , kDQ_VIL     ///< Vertical Integrated Liquid water (kg/m2)
    , kDQ_VRAD    ///< Radial velocity (m/s)
    , kDQ_WRAD    ///< Spectral width of radial velocity (m/s)
    , kDQ_UWND    ///< Component of wind in x-direction (m/s)
    , kDQ_VWND    ///< Component of wind in y-direction (m/s)
    , kDQ_BRDR    ///< 1 denotes border between radars in composite, 0 otherwise
    , kDQ_QIND    ///< Spatially analyzed quality indicator, according to OPERA II [0-1]
    , kDQ_CLASS   ///< Classified according to legend

    , kDQ_Unknown ///< Invalid or unknown qualtity
  };

  /// Optional scalar quality attributes (doubles)
  enum QualityAttribute_Double
  {
      kQA_ZR_A              ///< Z-R constant A in Z = AR^b
    , kQA_ZR_B              ///< Z-R exponent b in Z = AR^b
    , kQA_KR_A              ///< K-R constant A in R = AK^b
    , kQA_KR_B              ///< K-R exponent b in R = AK^b
    , kQA_BeamWidth         ///< Radar half power beam width (degrees)
    , kQA_Wavelength        ///< Wavelength (cm)
    , kQA_RPM               ///< Antenna revolutions per minute
    , kQA_PulseWidth        ///< Pulse width in micro-seconds (us)
    , kQA_LowPRF            ///< Low pulse repetition frequency (Hz)
    , kQA_HighPRF           ///< High pulse repitition frequency (Hz)
    , kQA_MinRange          ///< Minimum range of data used when generating a profile (km)
    , kQA_MaxRange          ///< Maximum range of data used when generating a profile (km)
    , kQA_NyquistVelocity   ///< Unambiguous velocity (Nyquist) interval (+-m/s)
    , kQA_ElevationAccuracy ///< Antenna pointing accuracy in elevation (degrees)
    , kQA_AzimuthAccuracy   ///< Antenna pointing accuracy in azimuth (degrees)
    , kQA_RadarHorizon      ///< Radar horizon - maximum range (km)
    , kQA_MDS               ///< Minimum detectable signal at 10km (dBm)
    , kQA_OUR               ///< Overall uptime reliability (%)
    , kQA_SQI               ///< Signal Quality Index threshold value
    , kQA_CSR               ///< Clutter-to-signal ratio threshold value
    , kQA_LOG               ///< Security distance above mean noise level threshold value (dB)
    , kQA_FreezeLevel       ///< Freezing level above sea level (km)
    , kQA_Min               ///< Minimum value for continuous quality data
    , kQA_Max               ///< Maximum value for continuous quality data
    , kQA_Step              ///< Step value for continuous quality data
    , kQA_PeakPower         ///< Peak power (kW)
    , kQA_AveragePower      ///< Average power (W)
    , kQA_DynamicRange      ///< Dynamic range (dB)
    , kQA_RAC               ///< Range attenuation correction (dBm)
    , kQA_PAC               ///< Precipitation attenuation correction (dBm)
    , kQA_SignalToNoise     ///< Signal-to-noise ratio threshold value (dB)
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
  void NewAtt(hid_t hID, const char *pszName, DataQuantity eVal);

  // Alter existing attributes (or create if not existing)
  void SetAtt(hid_t hID, const char *pszName, const char *pszVal);
  void SetAtt(hid_t hID, const char *pszName, const std::string &strVal);
  void SetAtt(hid_t hID, const char *pszName, long nVal);
  void SetAtt(hid_t hID, const char *pszName, double fVal);
  void SetAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t tVal);
  void SetAtt(hid_t hID, const char *pszName, ObjectType eVal);
  void SetAtt(hid_t hID, const char *pszName, ProductType eVal);
  void SetAtt(hid_t hID, const char *pszName, DataQuantity eVal);

  // Retrieve existing attributes
  void GetAtt(hid_t hID, const char *pszName, char *pszBuf, size_t nBufSize);
  void GetAtt(hid_t hID, const char *pszName, std::string &strVal);
  void GetAtt(hid_t hID, const char *pszName, long &nVal);
  void GetAtt(hid_t hID, const char *pszName, double &fVal);
  void GetAtt(hid_t hID, const char *pszNameDate, const char *pszNameTime, time_t &tVal);
  void GetAtt(hid_t hID, const char *pszName, ObjectType &eVal);
  void GetAtt(hid_t hID, const char *pszName, ProductType &eVal);
  void GetAtt(hid_t hID, const char *pszName, DataQuantity &eVal);

  // Set an optional quality attribute (and create 'how' group if needed)
  void SetHowAtt(hid_t hParent, HID_Group &hHow, QualityAttribute_Double eAttr, double fVal);

  // Get/set an optional quality attribute
  bool GetHowAtt(const HID_Group &hHow, QualityAttribute_Double eAttr, double &fVal);

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

