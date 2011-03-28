/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "data.h"

#include <cmath>
#include <vector>

using namespace RainHDF;

namespace RainHDF
{
  /// Compression factor used to compress data (0 - 9, 9 = max compression)
  static const int kDefCompression = 6;
}

Data::~Data()
{

}

Data::Data(
      const Base &parent
    , bool bIsQuality
    , size_t nIndex
    , Quantity eQuantity
    , const hsize_t *pDims
    , const float *pData
    , float fNoData
    , float fUndetect)
  : Base(parent, bIsQuality ? kGrp_Quality : kGrp_Data, nIndex, kCreate)
  , m_bIsQuality(bIsQuality)
  , m_eQuantity(eQuantity)
  , m_fGain(1.0f)
  , m_fOffset(0.0f)
  , m_nSize(pDims[0] * pDims[1])
{
  // Fill in the 'what' parameters
  NewAtt(m_hWhat, kAtn_Quantity, m_eQuantity);
  NewAtt(m_hWhat, kAtn_Gain, m_fGain);
  NewAtt(m_hWhat, kAtn_Offset, m_fOffset);
  NewAtt(m_hWhat, kAtn_NoData, fNoData);
  NewAtt(m_hWhat, kAtn_Undetect, fUndetect);

  // Create the HDF dataset
  HID_Handle hSpace(kHID_Space, 2, pDims, kCreate);
  HID_Handle hPList(kHID_PList, H5P_DATASET_CREATE, kCreate);
  if (H5Pset_chunk(hPList, 2, pDims) < 0)
    throw Error(m_hThis, "Failed to set chunk parameters for data");
  if (H5Pset_deflate(hPList, kDefCompression) < 0)
    throw Error(m_hThis, "Failed to set compression level for data");
  HID_Handle hData(kHID_Data, m_hThis, kDat_Data, H5T_NATIVE_FLOAT, hSpace, hPList, kCreate);
  NewAtt(hData, kAtn_Class, kVal_Class);
  NewAtt(hData, kAtn_ImageVersion, kVal_ImageVersion);

  // Write the actual image data
  if (H5Dwrite(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pData) < 0)
    throw Error("Failed to write data");
}

Data::Data(
      const Base &parent
    , bool bIsQuality
    , size_t nIndex
    , Quantity eQuantity
    , const hsize_t *pDims)
  : Base(parent, bIsQuality ? kGrp_Quality : kGrp_Data, nIndex, kOpen)
  , m_bIsQuality(bIsQuality)
  , m_eQuantity(eQuantity)
  , m_fGain(GetAtt<double>(m_hWhat, kAtn_Gain))
  , m_fOffset(GetAtt<double>(m_hWhat, kAtn_Offset))
  , m_nSize(pDims[0] * pDims[1])
{

}

void Data::Read(float *pData, float &fNoData, float &fUndetect) const
{
  HID_Handle hData(kHID_Data, m_hThis, kDat_Data, kOpen);

  // Verify the correct dimension to prevent memory corruption
  HID_Handle hSpace(kHID_Space, H5Dget_space(hData));
  if (H5Sget_simple_extent_npoints(hSpace) != m_nSize)
    throw Error(hData, "Dataset dimension mismatch");

  // Read the raw data
  fNoData = GetAtt<double>(m_hWhat, kAtn_NoData);
  fUndetect = GetAtt<double>(m_hWhat, kAtn_Undetect);
  if (H5Dread(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pData) < 0)
    throw Error(m_hThis, "Failed to read data");

  // Convert using gain and offset?
  if (   std::fabs(m_fGain - 1.0) > 0.000001
      || std::fabs(m_fOffset) > 0.000001)
  {
    fNoData = (fNoData * m_fGain) + m_fOffset;
    fUndetect = (fUndetect * m_fGain) + m_fOffset;
    for (size_t i = 0; i < m_nSize; ++i)
      pData[i] = (pData[i] * m_fGain) + m_fOffset;
  }
}

void Data::Write(const float *pData, float fNoData, float fUndetect)
{
  HID_Handle hData(kHID_Data, m_hThis, kDat_Data, kOpen);

  // Verify the correct dimension to prevent memory corruption
  HID_Handle hSpace(kHID_Space, H5Dget_space(hData));
  if (H5Sget_simple_extent_npoints(hSpace) != m_nSize)
    throw Error(hData, "Dataset dimension mismatch");

  // Do we have to convert the data?
  if (   std::fabs(m_fGain - 1.0f) > 0.000001
      || std::fabs(m_fOffset) > 0.000001)
  {
    float fGainMult = 1.0f / m_fGain;
    fNoData = (fNoData - m_fOffset) * fGainMult;
    fUndetect = (fUndetect - m_fOffset) * fGainMult;

    std::vector<float> vecData;
    vecData.reserve(m_nSize);
    vecData.assign(pData, pData + m_nSize);
    for (std::vector<float>::iterator i = vecData.begin(); i != vecData.end(); ++i)
      *i = (*i - m_fOffset) * fGainMult;

    // Write the converted data
    SetAtt(m_hWhat, kAtn_NoData, fNoData);
    SetAtt(m_hWhat, kAtn_Undetect, fUndetect);
    if (H5Dwrite(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vecData[0]) < 0)
      throw Error("Failed to write data");
  }
  else
  {
    // Write the raw data
    SetAtt(m_hWhat, kAtn_NoData, fNoData);
    SetAtt(m_hWhat, kAtn_Undetect, fUndetect);
    if (H5Dwrite(hData, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pData) < 0)
      throw Error("Failed to write data");
  }
}

