/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "volume.h"

#include <cstring>
#include <cmath>

using namespace RainHDF;

namespace RainHDF
{
  static const char * kAtt_Product = "product";
  static const char * kAtt_AzimuthCount = "nrays";
  static const char * kAtt_RangeCount = "nbins";

  static const char * kAtt_Quantity = "quantity";
  static const char * kAtt_Gain = "gain";
  static const char * kAtt_Offset = "offset";
  static const char * kAtt_NoData = "nodata";
  static const char * kAtt_Undetect = "undetect";

  static const char * kAtt_Class = "CLASS";
  static const char * kAtt_ImageVersion = "IMAGE_VERSION";

  static const char * kDat_Data = "data";

  static const char * kVal_Class = "IMAGE";
  static const char * kVal_ImageVersion = "1.2";

  static const int kDefScanCount = 16;
  static const int kDefCompression = 6;
}

const char * Volume::Scan::kAtt_RangeStart = "rstart";
const char * Volume::Scan::kAtt_RangeScale = "rscale";
const char * Volume::Scan::kAtt_StartDate = "startdate";
const char * Volume::Scan::kAtt_StartTime = "starttime";
const char * Volume::Scan::kAtt_EndDate = "enddate";
const char * Volume::Scan::kAtt_EndTime = "endtime";
const char * Volume::Scan::kAtt_Elevation = "elangle";
const char * Volume::Scan::kAtt_FirstAzimuth = "a1gate";

Volume::Scan::Layer::Layer(Layer &&layer)
  : m_hLayer(std::move(layer.m_hLayer))
  , m_hWhat(std::move(layer.m_hWhat))
  , m_hHow(std::move(layer.m_hHow))
  , m_nSize(layer.m_nSize)
  , m_eQuantity(layer.m_eQuantity)
{

}

Volume::Scan::Layer & Volume::Scan::Layer::operator=(Layer &&layer)
{
  m_hLayer = std::move(layer.m_hLayer);
  m_hWhat = std::move(layer.m_hWhat);
  m_hHow = std::move(layer.m_hHow);
  m_nSize = layer.m_nSize;
  m_eQuantity = layer.m_eQuantity;
  return *this;
}

Volume::Scan::Layer::Layer(
      hid_t hParent
    , const char *pszName
    , const hsize_t *pDims)
  : m_hLayer(hParent, pszName, kOpen)
  , m_hWhat(m_hLayer, kGrp_What, kOpen)
  , m_hHow(m_hLayer, kGrp_How, kOpen, true)
  , m_nSize(pDims[0] * pDims[1])
  , m_eQuantity(GetAtt<Quantity>(m_hWhat, kAtt_Quantity))
{

}

Volume::Scan::Layer::Layer(
      hid_t hParent
    , const char *pszName
    , const hsize_t *pDims
    , Quantity eQuantity
    , const float *pData
    , float fNoData
    , float fUndetect)
  : m_hLayer(hParent, pszName, kCreate)
  , m_hWhat(m_hLayer, kGrp_What, kCreate)
  , m_nSize(pDims[0] * pDims[1])
  , m_eQuantity(eQuantity)
{
  // Fill in the 'what' parameters
  NewAtt(m_hWhat, kAtt_Quantity, m_eQuantity);
  NewAtt(m_hWhat, kAtt_Gain, 1.0);
  NewAtt(m_hWhat, kAtt_Offset, 0.0);
  NewAtt(m_hWhat, kAtt_NoData, fNoData);
  NewAtt(m_hWhat, kAtt_Undetect, fUndetect);

  // Create the HDF dataset
  HID_Space hSpace(2, pDims, kCreate);
  HID_PList hPList(H5P_DATASET_CREATE, kCreate);
  if (H5Pset_chunk(hPList, 2, pDims) < 0)
    throw Error(m_hLayer, "Failed to set chunk parameters for layer");
  if (H5Pset_deflate(hPList, kDefCompression) < 0)
    throw Error(m_hLayer, "Failed to set compression level for layer");
  HID_Data hData(m_hLayer, kDat_Data, H5T_NATIVE_FLOAT, hSpace, hPList, kCreate);
  NewAtt(hData, kAtt_Class, kVal_Class);
  NewAtt(hData, kAtt_ImageVersion, kVal_ImageVersion);

  // Write the actual image data
  if (H5Dwrite(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pData) < 0)
    throw Error("Failed to write layer data");
}

void Volume::Scan::Layer::Read(float *pData, float &fNoData, float &fUndetect) const
{
  HID_Data hData(m_hLayer, kDat_Data, kOpen);

  // Verify the correct dimension to prevent memory corruption
  HID_Space hSpace(H5Dget_space(hData));
  if (H5Sget_simple_extent_npoints(hSpace) != m_nSize)
    throw Error(hData, "Dataset dimension mismatch");

  // Read the raw data
  HID_Group hWhat(m_hLayer, kGrp_What, kOpen);
  fNoData = GetAtt<double>(hWhat, kAtt_NoData);
  fUndetect = GetAtt<double>(hWhat, kAtt_Undetect);
  if (H5Dread(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pData) < 0)
    throw Error(m_hLayer, "Failed to read layer data");

  // Convert using gain and offset?
  double fGain(GetAtt<double>(hWhat, kAtt_Gain));
  double fOffset(GetAtt<double>(hWhat, kAtt_Offset));
  if (   std::fabs(fGain - 1.0) > 0.000001
      || std::fabs(fOffset) > 0.000001)
  {
    fNoData = (fNoData * fGain) + fOffset;
    fUndetect = (fUndetect * fGain) + fOffset;
    for (size_t i = 0; i < m_nSize; ++i)
      pData[i] = (pData[i] * fGain) + fOffset;
  }
}

void Volume::Scan::Layer::Write(const float *pData, float fNoData, float fUndetect)
{
  HID_Data hData(m_hLayer, kDat_Data, kOpen);

  // Verify the correct dimension to prevent memory corruption
  HID_Space hSpace(H5Dget_space(hData));
  if (H5Sget_simple_extent_npoints(hSpace) != m_nSize)
    throw Error(hData, "Dataset dimension mismatch");

  // Write the raw data
  HID_Group hWhat(m_hLayer, kGrp_What, kOpen);
  SetAtt(hWhat, kAtt_Gain, 1.0);
  SetAtt(hWhat, kAtt_Offset, 0.0);
  SetAtt(hWhat, kAtt_NoData, fNoData);
  SetAtt(hWhat, kAtt_Undetect, fUndetect);
  if (H5Dwrite(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pData) < 0)
    throw Error("Failed to write layer data");
}

Volume::Scan::Scan(Scan &&scan)
  : m_hScan(std::move(scan.m_hScan))
  , m_hWhat(std::move(scan.m_hWhat))
  , m_hWhere(std::move(scan.m_hWhere))
  , m_hHow(std::move(scan.m_hHow))
  , m_nAzimuthCount(scan.m_nAzimuthCount)
  , m_nRangeCount(scan.m_nRangeCount)
  , m_Layers(std::move(scan.m_Layers))
{

}

Volume::Scan::Scan(
      hid_t hParent
    , const char *pszName
    , double fElevation
    , size_t nAzimuths
    , size_t nRangeBins
    , size_t nFirstAzimuth
    , double fRangeStart
    , double fRangeScale
    , time_t tStart
    , time_t tEnd)
  : m_hScan(hParent, pszName, kCreate)
  , m_hWhat(m_hScan, kGrp_What, kCreate)
  , m_hWhere(m_hScan, kGrp_Where, kCreate)
  , m_nAzimuthCount(nAzimuths)
  , m_nRangeCount(nRangeBins)
{
  NewAtt(m_hWhat, kAtt_Product, kPT_Scan);
  NewAtt(m_hWhat, kAtt_StartDate, kAtt_StartTime, tStart);
  NewAtt(m_hWhat, kAtt_EndDate, kAtt_EndTime, tEnd);

  NewAtt(m_hWhere, kAtt_Elevation, fElevation);
  NewAtt(m_hWhere, kAtt_RangeCount, (long) m_nRangeCount);
  NewAtt(m_hWhere, kAtt_RangeStart, fRangeStart);
  NewAtt(m_hWhere, kAtt_RangeScale, fRangeScale);
  NewAtt(m_hWhere, kAtt_AzimuthCount, (long) m_nAzimuthCount);
  NewAtt(m_hWhere, kAtt_FirstAzimuth, (long) nFirstAzimuth);
}

Volume::Scan::Scan(hid_t hParent, const char *pszName) 
  : m_hScan(hParent, pszName, kOpen)
  , m_hWhat(m_hScan, kGrp_What, kOpen)
  , m_hWhere(m_hScan, kGrp_Where, kOpen)
  , m_hHow(m_hScan, kGrp_How, kOpen, true)
  , m_nAzimuthCount(GetAtt<long>(m_hWhere, kAtt_AzimuthCount))
  , m_nRangeCount(GetAtt<long>(m_hWhere, kAtt_RangeCount))
{
  char pszLayerName[32];

  // Verify that this dataset is indeed a scan
  if (GetAtt<ProductType>(m_hWhat, kAtt_Product) != kPT_Scan)
    throw Error(m_hScan, "Scan product code mismatch");

  // Check for any data layers
  for (int i = 1; IndexedGroupExists(m_hScan, kGrp_Data, i, pszLayerName); ++i)
    m_Layers.push_back(Layer(m_hScan, pszLayerName, &m_nAzimuthCount));

  // Check for quality layers
  for (int i = 1; IndexedGroupExists(m_hScan, kGrp_Quality, i, pszLayerName); ++i)
    m_Layers.push_back(Layer(m_hScan, pszLayerName, &m_nAzimuthCount));
}

Volume::Scan & Volume::Scan::operator=(Scan &&scan)
{
  m_hScan = std::move(scan.m_hScan);
  m_hWhat = std::move(scan.m_hWhat);
  m_hWhere = std::move(scan.m_hWhere);
  m_hHow = std::move(scan.m_hHow);
  m_nAzimuthCount = scan.m_nAzimuthCount;
  m_nRangeCount = scan.m_nRangeCount;
  m_Layers = std::move(scan.m_Layers);
  return *this;
}

Volume::Scan::Layer & Volume::Scan::AddLayer(
      Quantity eQuantity
    , const float *pData
    , float fNoData
    , float fUndetect)
{
  // TODO - should be either dataX or qualityX depending on nature of layer
  char pszName[16];
  sprintf(pszName, "%s%d", kGrp_Data, m_Layers.size() + 1);
  m_Layers.push_back(
      Layer(
          m_hScan,
          pszName,
          &m_nAzimuthCount,
          eQuantity,
          pData,
          fNoData,
          fUndetect));
  return m_Layers.back();
}

Volume::Volume(
      const std::string &strFilename
    , time_t tValid
    , double fLatitude
    , double fLongitude
    , double fHeight)
  : Base(strFilename, kOT_VolumePolar, tValid)
{
  NewAtt(m_hWhere, "lat", fLatitude);
  NewAtt(m_hWhere, "lon", fLongitude);
  NewAtt(m_hWhere, "height", fHeight);

  // Reserve some memory for the scan information
  m_Scans.reserve(kDefScanCount);
}

Volume::Volume(const std::string &strFilename, bool bReadOnly)
  : Base(strFilename, bReadOnly, kOT_VolumePolar)
{
  // Reserve some memory for the scan information
  m_Scans.reserve(kDefScanCount);

  // Load our scans
  char pszName[32];
  for (int i = 1; IndexedGroupExists(m_hFile, kGrp_Dataset, i, pszName); ++i)
    m_Scans.push_back(Scan(m_hFile, pszName));
}

Volume::Scan & Volume::AddScan(
      double fElevation
    , size_t nAzimuths
    , size_t nRangeBins
    , size_t nFirstAzimuth
    , double fRangeStart
    , double fRangeScale
    , time_t tStart
    , time_t tEnd)
{
  // Create the new dataset group
  char pszName[32];
  sprintf(pszName, "%s%d", kGrp_Dataset, m_Scans.size() + 1);
  m_Scans.push_back(
      Scan(
          m_hFile,
          pszName,
          fElevation,
          nAzimuths,
          nRangeBins,
          nFirstAzimuth,
          fRangeStart,
          fRangeScale,
          tStart,
          tEnd));
  return m_Scans.back();
}


