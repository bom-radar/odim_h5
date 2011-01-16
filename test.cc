/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "rainhdf/rainhdf.h"

#include <iostream>
#include <cstring>
#include <exception>

using namespace std;
using namespace RainHDF;

int main(int argc, const char *argv[])
{
  try
  {
    // Open an existing volume for reading
    const Volume hdfr("../data/201001221325Kurnell.h5", false);

    // Create a new volume to copy it into
    Volume hdfw(
        "hello.h5", 
        hdfr.GetTime(),
        hdfr.GetLatitude(),
        hdfr.GetLongitude(),
        hdfr.GetHeight());

    for (size_t i = 0; i < hdfr.GetScanCount(); ++i)
    {
      const Volume::Scan &sr(hdfr.GetScan(i));
      Volume::Scan &sw(
          hdfw.AddScan(
              sr.GetElevation(),
              sr.GetAzimuthCount(),
              sr.GetRangeBinCount(),
              sr.GetFirstAzimuth(),
              sr.GetRangeStart(),
              sr.GetRangeScale(),
              sr.GetStartTime(),
              sr.GetEndTime()));

      float fNoData, fUndetect;
      vector<float> vecData(sr.GetAzimuthCount() * sr.GetRangeBinCount());

      for (size_t j = 0; j < sr.GetLayerCount(); ++j)
      {
        const Volume::Scan::Layer &lr(sr.GetLayer(j));
        lr.Read(&vecData[0], fNoData, fUndetect);
        Volume::Scan::Layer &lw(
            sw.AddLayer(
                kDQ_DBZH,
                &vecData[0],
                fNoData,
                fUndetect));

        lw.SetQuality(kQA_NyquistVelocity, 2.5);
      }
    }
  }
  catch (std::exception &err)
  {
    cout << "error: " << err.what() << endl;
  }

  cout << "done" << endl;
}


