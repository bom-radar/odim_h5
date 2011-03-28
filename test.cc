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
    const Volume hdfr("201001221325Kurnell.h5", false);
    printf("opened file\n");

    // Create a new volume to copy it into
    Volume hdfw(
        "hello.h5", 
        hdfr.GetTime(),
        hdfr.GetLatitude(),
        hdfr.GetLongitude(),
        hdfr.GetHeight());

    // Read the source attribute
    string s1, s2, s3, s4, s5, s6;
    hdfr.GetSource(s1, s2, s3, s4, s5, s6);
    hdfw.SetSource(s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str(), s5.c_str(), s6.c_str());
    cout 
      << "met " << s1 << endl
      << "rad " << s2 << endl
      << "org " << s3 << endl
      << "plc " << s4 << endl
      << "cty " << s5 << endl
      << "cmt " << s6 << endl;

    // Set some quality attributes
    hdfw.SetAttribute(kAtt_Simulated, true);
    hdfw.SetAttribute(kAtt_Malfunction, false);
    hdfw.SetAttribute(kAtt_LevelCount, 1234L);
    hdfw.SetAttribute(kAtt_RPM, 2.5);
    hdfw.SetAttribute(kAtt_Task, "RainHDF Test Application");
    hdfw.SetAttribute(kAtt_System, std::string("Test string"));
    hdfw.SetAttribute(kAtt_CartesianMethod, kMth_GaugeAdjust);
    hdfw.SetAttribute(kAtt_NyquistVelocity, 1.0);

    // Get some quality attributes
    bool b1, b2; long n; double f; Method e;
    hdfw.GetAttribute(kAtt_Simulated, b1);
    hdfw.GetAttribute(kAtt_Malfunction, b2);
    hdfw.GetAttribute(kAtt_LevelCount, n);
    hdfw.GetAttribute(kAtt_RPM, f);
    hdfw.GetAttribute(kAtt_Task, s1);
    hdfw.GetAttribute(kAtt_System, s2);
    hdfw.GetAttribute(kAtt_CartesianMethod, e);

    cout << b1 << " " << b2 << " " << n 
      << " " << f << " " << s1 << " " << s2 
      << " " << e 
      << endl;

    for (size_t i = 0; i < hdfr.GetScanCount(); ++i)
    {
      Scan::ConstPtr sr = hdfr.GetScan(i);
      Scan::Ptr sw = 
          hdfw.AddScan(
              sr->GetElevation(),
              sr->GetAzimuthCount(),
              sr->GetRangeBinCount(),
              sr->GetFirstAzimuth(),
              sr->GetRangeStart(),
              sr->GetRangeScale(),
              sr->GetStartTime(),
              sr->GetEndTime());

      float fNoData, fUndetect;
      vector<float> vecData(sr->GetAzimuthCount() * sr->GetRangeBinCount());

      for (size_t j = 0; j < sr->GetDataCount(); ++j)
      {
        // Read the layer data
        Data::ConstPtr lr = sr->GetData(j);
        lr->Read(&vecData[0], fNoData, fUndetect);

        // Write it out as a new layer
        Data::Ptr lw = 
          sw->AddData(
              lr->GetQuantity(),
              lr->IsQualityData(),
              &vecData[0],
              fNoData,
              fUndetect);

        // Now go through and filter our reflectivity (only)
        if (lr->GetQuantity() == kQty_DBZH)
        {
          for (vector<float>::iterator i = vecData.begin(); i != vecData.end(); ++i)
            if (*i < 10)
              *i = 0;

          sw->AddData(
              lr->GetQuantity(),
              true,
              &vecData[0],
              fNoData,
              fUndetect);
        }
      }

      printf("open objs: %d\n", H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_ALL));
    }

    printf("open objs: %d (should be 8?)\n", H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_ALL));
  }
  catch (std::exception &err)
  {
    cout << "error: " << err.what() << endl;
  }

  printf("open objs: %d (should be 0)\n", H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_ALL));

  cout << "done" << endl;
}


