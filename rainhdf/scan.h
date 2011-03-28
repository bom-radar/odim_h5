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
    double GetElevation() const { return GetAtt<double>(m_hWhere, kAtn_Elevation); }
    /// Get the number of azimuths in scan
    size_t GetAzimuthCount() const { return m_nAzimuthCount; }
    /// Get the number of range bins per azimuth
    size_t GetRangeBinCount() const { return m_nRangeCount; }
    /// Get the number of the first azimuth to be radiated
    size_t GetFirstAzimuth() const { return GetAtt<long>(m_hWhere, kAtn_FirstAzimuth); }
    /// Get the distance from sensor at the start of the first range bin in meters
    double GetRangeStart() const { return GetAtt<double>(m_hWhere, kAtn_RangeStart) * 1000.0; }
    /// Get the distance between consecutive range bins in meters
    double GetRangeScale() const { return GetAtt<double>(m_hWhere, kAtn_RangeScale); }
    /// Get the time this scan commenced
    time_t GetStartTime() const { return GetAtt<time_t>(m_hWhat, kAtn_StartDate, kAtn_StartTime); }
    /// Get the time this scan completed
    time_t GetEndTime() const { return GetAtt<time_t>(m_hWhat, kAtn_EndDate, kAtn_EndTime); }

    /// Get the number of data layers in the scan
    size_t GetDataCount() const { return m_DataInfos.size(); }
    /// Get the 'nth' data layer
    Data::Ptr GetData(size_t nLayer);
    /// Get the 'nth' data layer
    Data::ConstPtr GetData(size_t nLayer) const;
    /// Get a data layer based on it's quantity (or NULL if no such data)
    Data::Ptr GetData(Quantity eQuantity);
    Data::ConstPtr GetData(Quantity eQuantity) const;

    /// Add a new data or quality layer to the scan
    Data::Ptr AddData(
          Quantity eQuantity
        , bool bIsQuality
        , const float *pData
        , float fNoData
        , float fUndetect);

  private:
    struct DataInfo
    {
      bool      m_bIsQuality;   ///< True if data, false if quality
      size_t    m_nIndex;       ///< Index of dataX/qualityX in file
      Quantity  m_eQuantity;    ///< Quantity stored by data layer
    };
    typedef std::vector<DataInfo> DataInfoStore_t;

  private:
    /// Create new scan in file
    Scan(
          const Base &parent
        , size_t nIndex         ///< Scan number in file (datasetX)
        , double fElevation     ///< Scan elevation angle (degrees above horizon)
        , size_t nAzimuths      ///< Number of azimuths in scan
        , size_t nRangeBins     ///< Number of range bins per azimuth
        , size_t nFirstAzimuth  ///< Index of first azimuth to be radiated
        , double fRangeStart    ///< Range of start of first bin (m)
        , double fRangeScale    ///< Distance between bins (m)
        , time_t tStart         ///< Time scan started
        , time_t tEnd           ///< Time scan ended
        );
    /// Create handle to a scan that is existing in the file
    Scan(const Base &parent, size_t nIndex);

  private:
    // Size required by data layers
    hsize_t           m_nAzimuthCount,    ///< Number of azimuths
                      m_nRangeCount;      ///< Number of range bins

    // Handles to any data layers that are present
    DataInfoStore_t   m_DataInfos;       ///< Information about data/quality layers

    friend class Volume;
  };
}

#endif

