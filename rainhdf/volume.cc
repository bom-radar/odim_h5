/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "volume.h"

using namespace RainHDF;

Volume::Volume(
      const std::string &strFilename
    , time_t tValid
    , double fLatitude
    , double fLongitude
    , double fHeight)
  : Product(strFilename, kObj_VolumePolar, tValid)
  , m_nScanCount(0)
{
  NewAtt(m_hWhere, kAtn_Latitude, fLatitude);
  NewAtt(m_hWhere, kAtn_Longitude, fLongitude);
  NewAtt(m_hWhere, kAtn_Height, fHeight);
}

Volume::Volume(const std::string &strFilename, bool bReadOnly)
  : Product(strFilename, kObj_VolumePolar, bReadOnly)
{
  // Determine the number of scans
  hsize_t nObjs;
  if (H5Gget_num_objs(m_hThis, &nObjs) < 0)
    throw Error(m_hThis, "Failed to determine number of objects in group");
  for (nObjs; nObjs > 0; --nObjs)
  {
    char pszName[32];
    sprintf(pszName, "%s%d", kGrp_Dataset, (int) nObjs);
    htri_t ret = H5Lexists(m_hThis, pszName, H5P_DEFAULT);
    if (ret < 0)
      throw Error(m_hThis, "Failed to verify existance of group '%s'", pszName);
    else if (ret)
      break;
  }
  m_nScanCount = nObjs;
}


