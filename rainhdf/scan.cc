/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "scan.h"

using namespace RainHDF;

Scan::~Scan()
{

}

Scan::Scan(
      const Base &parent
    , size_t nIndex
    , double fElevation
    , size_t nAzimuths
    , size_t nRangeBins
    , size_t nFirstAzimuth
    , double fRangeStart
    , double fRangeScale
    , time_t tStart
    , time_t tEnd)
  : Base(parent, kGrp_Dataset, nIndex, kCreate)
  , m_nAzimuthCount(nAzimuths)
  , m_nRangeCount(nRangeBins)
{
  NewAtt(m_hWhat, kAtn_Product, kProd_Scan);
  NewAtt(m_hWhat, kAtn_StartDate, kAtn_StartTime, tStart);
  NewAtt(m_hWhat, kAtn_EndDate, kAtn_EndTime, tEnd);

  NewAtt(m_hWhere, kAtn_Elevation, fElevation);
  NewAtt(m_hWhere, kAtn_RangeCount, (long) m_nRangeCount);
  NewAtt(m_hWhere, kAtn_RangeStart, fRangeStart / 1000.0);
  NewAtt(m_hWhere, kAtn_RangeScale, fRangeScale);
  NewAtt(m_hWhere, kAtn_AzimuthCount, (long) m_nAzimuthCount);
  NewAtt(m_hWhere, kAtn_FirstAzimuth, (long) nFirstAzimuth);
}

Scan::Scan(const Base &parent, size_t nIndex)
  : Base(parent, kGrp_Dataset, nIndex, kOpen)
  , m_nAzimuthCount(GetAtt<long>(m_hWhere, kAtn_AzimuthCount))
  , m_nRangeCount(GetAtt<long>(m_hWhere, kAtn_RangeCount))
{
  hsize_t nObjs;
  char pszName[32];

  // Verify that this dataset is indeed a scan
  if (GetAtt<ProductType>(m_hWhat, kAtn_Product) != kProd_Scan)
    throw Error(m_hThis, "Scan product code mismatch");

  // Reserve some space in our data info vector for efficency sake
  if (H5Gget_num_objs(m_hThis, &nObjs) < 0)
    throw Error(m_hThis, "Failed to determine number of objects in group");
  m_DataInfos.reserve(nObjs);

  // Check for any data layers
  for (size_t i = 1; true; ++i)
  {
    // Do we have this 'dataX'?
    sprintf(pszName, "%s%d", kGrp_Data, i);
    htri_t ret = H5Lexists(m_hThis, pszName, H5P_DEFAULT);
    if (ret < 0)
      throw Error(m_hThis, "Failed to verify existence of group '%s'", pszName);
    if (!ret)
      break;

    // Yes - open it up for inspection
    HID_Handle hData(kHID_Group, m_hThis, pszName, kOpen);
    HID_Handle hDataWhat(kHID_Group, hData, kGrp_What, kOpen);

    // Store some vitals
    DataInfo li;
    li.m_bIsQuality = false;
    li.m_nIndex = i;
    li.m_eQuantity = GetAtt<Quantity>(hDataWhat, kAtn_Quantity);
    m_DataInfos.push_back(li);
  }

  // Check for any quality layers
  for (size_t i = 1; true; ++i)
  {
    // Do we have this 'dataX'?
    sprintf(pszName, "%s%d", kGrp_Quality, i);
    htri_t ret = H5Lexists(m_hThis, pszName, H5P_DEFAULT);
    if (ret < 0)
      throw Error(m_hThis, "Failed to verify existence of group '%s'", pszName);
    if (!ret)
      break;

    // Yes - open it up for inspection
    HID_Handle hData(kHID_Group, m_hThis, pszName, kOpen);
    HID_Handle hDataWhat(kHID_Group, hData, kGrp_What, kOpen);

    // Store some vitals
    DataInfo li;
    li.m_bIsQuality = true;
    li.m_nIndex = i;
    li.m_eQuantity = GetAtt<Quantity>(hDataWhat, kAtn_Quantity);
    m_DataInfos.push_back(li);
  }
}

Data::Ptr Scan::GetData(size_t nLayer)
{
  return Data::Ptr(
      new Data(
          *this, 
          m_DataInfos[nLayer].m_bIsQuality,
          m_DataInfos[nLayer].m_nIndex,
          m_DataInfos[nLayer].m_eQuantity,
          &m_nAzimuthCount));
}

Data::ConstPtr Scan::GetData(size_t nLayer) const
{
  return Data::ConstPtr(
      new Data(
          *this, 
          m_DataInfos[nLayer].m_bIsQuality,
          m_DataInfos[nLayer].m_nIndex,
          m_DataInfos[nLayer].m_eQuantity,
          &m_nAzimuthCount));
}

Data::Ptr Scan::GetData(Quantity eQuantity)
{
  for (DataInfoStore_t::iterator i = m_DataInfos.begin(); i != m_DataInfos.end(); ++i)
    if (i->m_eQuantity == eQuantity)
      return Data::Ptr(
          new Data(
              *this, 
              i->m_bIsQuality, 
              i->m_nIndex,
              i->m_eQuantity,
              &m_nAzimuthCount));
  return Data::Ptr(NULL);
}

Data::ConstPtr Scan::GetData(Quantity eQuantity) const
{
  for (DataInfoStore_t::const_iterator i = m_DataInfos.begin(); i != m_DataInfos.end(); ++i)
    if (i->m_eQuantity == eQuantity)
      return Data::ConstPtr(
          new Data(
              *this, 
              i->m_bIsQuality, 
              i->m_nIndex,
              i->m_eQuantity,
              &m_nAzimuthCount));
  return Data::ConstPtr(NULL);
}

Data::Ptr Scan::AddData(
      Quantity eQuantity
    , bool bIsQuality
    , const float *pData
    , float fNoData
    , float fUndetect)
{
  DataInfo li;
  li.m_bIsQuality = bIsQuality;
  li.m_nIndex = 1;
  for (DataInfoStore_t::reverse_iterator i = m_DataInfos.rbegin(); 
       i != m_DataInfos.rend(); 
       ++i)
  {
    if (i->m_bIsQuality == li.m_bIsQuality)
    {
      li.m_nIndex = i->m_nIndex + 1;
      break;
    }
  }
  li.m_eQuantity = eQuantity;

  Data::Ptr pLayer(
      new Data(
          *this,
          li.m_bIsQuality,
          li.m_nIndex,
          li.m_eQuantity,
          &m_nAzimuthCount,
          pData,
          fNoData,
          fUndetect));

  // Must do the push_back last so that exceptions don't screw with our 
  // layer count
  m_DataInfos.push_back(li);
  return pLayer;
}

