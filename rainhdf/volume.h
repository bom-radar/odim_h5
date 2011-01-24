/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_VOLUME_H
#define RAINHDF_VOLUME_H

#include "base.h"

#include <memory>
#include <vector>

namespace RainHDF
{
  /// ODIM_H5 compliant polar volume file manipulator
  class Volume : public Base
  {
  public:
    /// Unique handle to a scan in the HDF5 file
    class Scan
    {
    public:
      /// Single layer (image) of data used by a scan
      class Layer
      {
      public:
        /// Is this layer a quality layer?
        bool IsQualityLayer() const { return m_bIsQuality; }

        /// Get the quantity stored by this layer
        Quantity GetQuantity() const { return m_eQuantity; }

        /// Get the number of elements in the layer data image
        size_t GetSize() const { return m_nSize; }

        /// Read the layer data
        void Read(float *pData, float &fNoData, float &fUndetect) const;
        /// Write the layer data
        void Write(const float *pData, float fNoData, float fUndetect);

        /// Read an optional attribute
        template <class E, class T>
        bool GetAttribute(E eAtt, T &val) const { return GetHowAtt(m_hHow, eAtt, val); }

        /// Write an optional attribute
        template <class E, class T>
        void SetAttribute(E eAtt, const T &val) { SetHowAtt(m_hLayer, m_hHow, eAtt, val); }

      private:
        Layer(
              hid_t hParent
            , bool bIsQuality
            , size_t nIndex
            , Quantity eQuantity
            , const hsize_t *pDims);
        Layer(
              hid_t hParent
            , bool bIsQuality
            , size_t nIndex
            , Quantity eQuantity
            , const hsize_t *pDims
            , const float *pData
            , float fNoData
            , float fUndetect);
        Layer(const Layer &layer);
        Layer & operator=(const Layer &layer);

      private:
        bool              m_bIsQuality; ///< Is this a quality layer?
        Quantity          m_eQuantity;  ///< Quantity stored by this layer
        HID_Group         m_hLayer;     ///< Handle to the 'dataX' group
        HID_Group         m_hWhat;      ///< Compulsory 'what' group
        HID_Group         m_hHow;       ///< The optional 'how' group

        float             m_fGain;      ///< Gain (a), in ax+b layer unpacking function
        float             m_fOffset;    ///< Offset (b), in ax+b layer unpacking function

        // Cached values
        size_t            m_nSize;      ///< Number of elements in dataset

        friend class Scan;
      };

      typedef std::auto_ptr<Layer> LayerPtr;
      typedef std::auto_ptr<const Layer> LayerConstPtr;

    public:
      /// Get the elevation of this scan
      double GetElevation() const { return GetAtt<double>(m_hWhere, kAtn_Elevation); }
      /// Get the number of azimuths in scan
      size_t GetAzimuthCount() const { return m_nAzimuthCount; }
      /// Get the number of range bins per azimuth
      size_t GetRangeBinCount() const { return m_nRangeCount; }
      /// Get the number of the first azimuth to be radiated
      size_t GetFirstAzimuth() const { return GetAtt<long>(m_hWhere, kAtn_FirstAzimuth); }
      /// Get the distance from sensor at the start of the first range bin
      double GetRangeStart() const { return GetAtt<double>(m_hWhere, kAtn_RangeStart); }
      /// Get the distance between consecutive range bins
      double GetRangeScale() const { return GetAtt<double>(m_hWhere, kAtn_RangeScale); }
      /// Get the time this scan commenced
      time_t GetStartTime() const { return GetAtt<time_t>(m_hWhat, kAtn_StartDate, kAtn_StartTime); }
      /// Get the time this scan completed
      time_t GetEndTime() const { return GetAtt<time_t>(m_hWhat, kAtn_EndDate, kAtn_EndTime); }

      /// Get the number of layers in the file
      size_t GetLayerCount() const { return m_LayerInfos.size(); }
      /// Get the 'nth' layer
      LayerPtr GetLayer(size_t nLayer);
      /// Get the 'nth' layer
      LayerConstPtr GetLayer(size_t nLayer) const;
      /// Get a layer based on it's quantity (or NULL if no such layer)
      LayerPtr GetLayer(Quantity eQuantity);
      LayerConstPtr GetLayer(Quantity eQuantity) const;

      /// Add a new data or quality layer to the scan
      LayerPtr AddLayer(
            Quantity eQuantity
          , bool bIsQuality
          , const float *pData
          , float fNoData
          , float fUndetect);

      /// Read an optional attribute
      template <class E, class T>
      bool GetAttribute(E eAtt, T &val) const { return GetHowAtt(m_hHow, eAtt, val); }

      /// Write an optional attribute
      template <class E, class T>
      void SetAttribute(E eAtt, const T &val) { SetHowAtt(m_hScan, m_hHow, eAtt, val); }

    private:
      struct LayerInfo
      {
        bool      m_bIsQuality;   ///< True if data, false if quality
        size_t    m_nIndex;       ///< Index of dataX/qualityX in file
        Quantity  m_eQuantity;    ///< Quantity stored by layer
      };
      typedef std::vector<LayerInfo> LayerInfoStore_t;

    private:
      /// Create new scan in file
      Scan(
            hid_t hParent
          , size_t nIndex         ///< Scan number in file (datasetX)
          , double fElevation     ///< Scan elevation angle (degrees above horizon)
          , size_t nAzimuths      ///< Number of azimuths in scan
          , size_t nRangeBins     ///< Number of range bins per azimuth
          , size_t nFirstAzimuth  ///< Index of first azimuth to be radiated
          , double fRangeStart    ///< Range of start of first bin (km)
          , double fRangeScale    ///< Distance between bins (m)
          , time_t tStart         ///< Time scan started
          , time_t tEnd           ///< Time scan ended
          );
      /// Create handle to a scan that is existing in the file
      Scan(hid_t hParent, size_t nIndex);

      Scan(const Scan &scan);
      Scan & operator=(const Scan &scan);

    private:
      // Per scan groups
      HID_Group         m_hScan;            ///< Handle to the 'datasetX' group
      HID_Group         m_hWhat;            ///< The compulsory 'what' group
      HID_Group         m_hWhere;           ///< The compulsory 'where' group
      HID_Group         m_hHow;             ///< The optional 'how' group

      // Size required by data layers
      hsize_t           m_nAzimuthCount,    ///< Number of azimuths
                        m_nRangeCount;      ///< Number of range bins

      // Handles to any data layers that are present
      LayerInfoStore_t  m_LayerInfos;       ///< Information about data/quality layers

      friend class Volume;
    };

    typedef std::auto_ptr<Scan> ScanPtr;
    typedef std::auto_ptr<const Scan> ScanConstPtr;

  public:
    /// Create a new HDF5 volume file
    Volume(
          const std::string &strFilename
        , time_t tValid
        , double fLatitude
        , double fLongitude
        , double fHeight
        );

    /// Open an existing HDF5 volume file
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
    ScanPtr GetScan(size_t nScan) { return ScanPtr(new Scan(m_hFile, nScan + 1)); }
    /// Get the 'nth' scan
    ScanConstPtr GetScan(size_t nScan) const { return ScanConstPtr(new Scan(m_hFile, nScan + 1)); }
    /// Add a new scan to the file
    ScanPtr AddScan(
          double fElevation     ///< Scan elevation angle (degrees above horizon)
        , size_t nAzimuths      ///< Number of azimuths scanned
        , size_t nRangeBins     ///< Number of range bins
        , size_t nFirstAzimuth  ///< Index of first azimuth to be radiated
        , double fRangeStart    ///< Range of start of first bin (km)
        , double fRangeScale    ///< Distance between bins (m)
        , time_t tStart         ///< Time scan started
        , time_t tEnd           ///< Time scan ended
        );

  private:
    // No need to make private copy contructor - base is already private
    Volume & operator=(const Volume &vol);

  private:
    size_t    m_nScanCount;   ///< Number of scans in file
  };

  inline Volume::ScanPtr Volume::AddScan(
        double fElevation
      , size_t nAzimuths
      , size_t nRangeBins
      , size_t nFirstAzimuth
      , double fRangeStart
      , double fRangeScale
      , time_t tStart
      , time_t tEnd)
  {
    return ScanPtr(
        new Scan(
            m_hFile,
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


