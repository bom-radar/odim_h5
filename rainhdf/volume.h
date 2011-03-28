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

namespace RainHDF
{
  /// ODIM_H5 compliant polar volume file manipulator
  class Volume : public Product
  {
  public:
    /// Create a new volume product
    Volume(
          const std::string &strFilename
        , time_t tValid
        , double fLatitude
        , double fLongitude
        , double fHeight
        );

    /// Open an existing volume product
    Volume(const std::string &strFilename, bool bReadOnly);

    /// Read the station latitude
    double GetLatitude() const { return GetAtt<double>(m_hWhere, kAtn_Latitude); }
    /// Write the station latitude
    void SetLatitude(double fLat) { SetAtt(m_hWhere, kAtn_Latitude, fLat); }

    /// Read the station longitude
    double GetLongitude() const { return GetAtt<double>(m_hWhere, kAtn_Longitude); }
    /// Write the station longitude
    void SetLongitude(double fLon) { SetAtt(m_hWhere, kAtn_Longitude, fLon); }

    /// Read the station elevation
    double GetHeight() const { return GetAtt<double>(m_hWhere, kAtn_Height); }
    /// Write the station elevation
    void SetHeight(double fHeight) { SetAtt(m_hWhere, kAtn_Height, fHeight); }

    /// Get the number of scans in the volume
    size_t GetScanCount() const { return m_nScanCount; }
    /// Get the 'nth' scan
    Scan::Ptr GetScan(size_t nScan) { return Scan::Ptr(new Scan(*this, nScan + 1)); }
    /// Get the 'nth' scan
    Scan::ConstPtr GetScan(size_t nScan) const { return Scan::ConstPtr(new Scan(*this, nScan + 1)); }
    /// Add a new scan to the file
    Scan::Ptr AddScan(
          double fElevation     ///< Scan elevation angle (degrees above horizon)
        , size_t nAzimuths      ///< Number of azimuths scanned
        , size_t nRangeBins     ///< Number of range bins
        , size_t nFirstAzimuth  ///< Index of first azimuth to be radiated
        , double fRangeStart    ///< Range of start of first bin (m)
        , double fRangeScale    ///< Distance between bins (m)
        , time_t tStart         ///< Time scan started
        , time_t tEnd           ///< Time scan ended
        );

  private:
    size_t    m_nScanCount;   ///< Number of scans in file
  };

  inline Scan::Ptr Volume::AddScan(
        double fElevation
      , size_t nAzimuths
      , size_t nRangeBins
      , size_t nFirstAzimuth
      , double fRangeStart
      , double fRangeScale
      , time_t tStart
      , time_t tEnd)
  {
    return Scan::Ptr(
        new Scan(
            *this,
            ++m_nScanCount,
            fElevation,
            nAzimuths,
            nRangeBins,
            nFirstAzimuth,
            fRangeStart,
            fRangeScale,
            tStart,
            tEnd));
  }
};

#endif


