/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_VOLUME_H
#define RAINHDF_VOLUME_H

#include "base.h"

#include <vector>

#if 0
      // Optional data layers (dataX)
      HID_Group     m_hReflectivity;    ///< Raw reflectivity
      HID_Group     m_hRadialVelocity;  ///< Doppler radial velocity
      // spectral width of velocity

      // Optional quality layers (qualityX)
      HID_Group     m_hClutterProb;     ///< Probability of clutter
      HID_Group     m_hShadowing;       ///< Beam blocking amount
      HID_Group     m_hAttenuation;     ///< Path integrated attenuation
#endif
    /* TODO Scalar quality indicators to store:
     * Global:
     *  - radar frequency
     *  - beam width
     *  - pointing accuracy (elevation)
     *  - pointing accuracy (azimuth)
     *  - minimum detectable signal at 1km?
     *  - wet radome attenuation
     * Per scan:
     *  - radar horizon?
     * With reflectivity:
     * With velocity:
     *  - Maximum unambiguous velocity
     *  - Maximum unambiguous range
     */


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
        Layer(const Layer &layer) = delete;
        Layer(Layer &&layer);
        Layer & operator=(const Layer &layer) = delete;
        Layer & operator=(Layer &&layer);

        /// Get the quantity stored by this layer
        Quantity GetQuantity() const { return m_eQuantity; }

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
            , const char *pszName
            , const hsize_t *pDims);
        Layer(
              hid_t hParent
            , const char *pszName
            , const hsize_t *pDims
            , Quantity eQuantity
            , const float *pData
            , float fNoData
            , float fUndetect);

      private:
        HID_Group   m_hLayer;           ///< Handle to the 'dataX' group
        HID_Group   m_hWhat;            ///< Compulsory 'what' group
        HID_Group   m_hHow;             ///< The optional 'how' group

        // Cached values
        size_t      m_nSize;            ///< Number of elements in dataset
        Quantity    m_eQuantity;        ///< Quantity represented by this layer

        friend class Scan;
      };

    public:
      /// Disable copy construction
      Scan(const Scan &scan) = delete;
      /// Move construction
      Scan(Scan &&scan);
      /// Disable copy assignment
      Scan & operator=(const Scan &scan) = delete;
      /// Move assignment
      Scan & operator=(Scan &&scan);

      /// Get the elevation of this scan
      double GetElevation() const { return GetAtt<double>(m_hWhere, kAtt_Elevation); }
      /// Get the number of azimuths in scan
      size_t GetAzimuthCount() const { return m_nAzimuthCount; }
      /// Get the number of range bins per azimuth
      size_t GetRangeBinCount() const { return m_nRangeCount; }
      /// Get the number of the first azimuth to be radiated
      size_t GetFirstAzimuth() const { return GetAtt<long>(m_hWhere, kAtt_FirstAzimuth); }
      /// Get the distance from sensor at the start of the first range bin
      double GetRangeStart() const { return GetAtt<double>(m_hWhere, kAtt_RangeStart); }
      /// Get the distance between consecutive range bins
      double GetRangeScale() const { return GetAtt<double>(m_hWhere, kAtt_RangeScale); }
      /// Get the time this scan commenced
      time_t GetStartTime() const { return GetAtt<time_t>(m_hWhat, kAtt_StartDate, kAtt_StartTime); }
      /// Get the time this scan completed
      time_t GetEndTime() const { return GetAtt<time_t>(m_hWhat, kAtt_EndDate, kAtt_EndTime); }

      /// Get the number of layers in the file
      size_t GetLayerCount() const { return m_Layers.size(); }
      /// Get the 'nth' layer
      Layer & GetLayer(size_t nLayer) { return m_Layers[nLayer]; }
      /// Get the 'nth' layer
      const Layer & GetLayer(size_t nLayer) const { return m_Layers[nLayer]; }

      /// Add a new data or quality layer to the scan
      Layer & AddLayer(
            Quantity eQuantity
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
      /// Create new scan in file
      Scan(
            hid_t hParent
          , const char *pszName
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
      Scan(hid_t hParent, const char *pszName);

    private:
      typedef std::vector<Layer> LayerStore_t;

    private:
      // Compulsory per scan groups
      HID_Group     m_hScan;            ///< Handle to the 'datasetX' group
      HID_Group     m_hWhat;            ///< The compulsory 'what' group
      HID_Group     m_hWhere;           ///< The compulsory 'where' group
      HID_Group     m_hHow;             ///< The optional 'how' group

      // Size required by data layers
      hsize_t       m_nAzimuthCount,    ///< Number of azimuths
                    m_nRangeCount;      ///< Number of range bins

      // Handles to any data layers that are present
      LayerStore_t  m_Layers;           ///< Data and quality layers of this scan

    private:
      static const char * kAtt_RangeStart;
      static const char * kAtt_RangeScale;
      static const char * kAtt_StartDate;
      static const char * kAtt_StartTime;
      static const char * kAtt_EndDate;
      static const char * kAtt_EndTime;
      static const char * kAtt_Elevation;
      static const char * kAtt_FirstAzimuth;

      friend class Volume;
    };

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

    /// Close the HDF5 volume file and destroy the handle
    virtual ~Volume() = default;

    /// Read the station latitude
    double GetLatitude() const { return GetAtt<double>(m_hWhere, "lat"); }
    /// Write the station latitude
    void SetLatitude(double fLat) { SetAtt(m_hWhere, "lat", fLat); }

    /// Read the station longitude
    double GetLongitude() const { return GetAtt<double>(m_hWhere, "lon"); }
    /// Write the station longitude
    void SetLongitude(double fLon) { SetAtt(m_hWhere, "lon", fLon); }

    /// Read the station elevation
    double GetHeight() const { return GetAtt<double>(m_hWhere, "height"); }
    /// Write the station elevation
    void SetHeight(double fHeight) { SetAtt(m_hWhere, "height", fHeight); }

    /// Get the number of scans in the file
    size_t GetScanCount() const { return m_Scans.size(); }
    /// Get the 'nth' scan
    Scan & GetScan(size_t nScan) { return m_Scans[nScan]; }
    /// Get the 'nth' scan
    const Scan & GetScan(size_t nScan) const { return m_Scans[nScan]; }

    /// Add a new scan to the file
    /** \return The index of the new scan */
    Scan & AddScan(
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
    typedef std::vector<Scan> ScanStore_t;

  private:
    ScanStore_t   m_Scans;    ///< Handles to the scan groups
  };
};

#endif


