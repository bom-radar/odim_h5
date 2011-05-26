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
using namespace rainhdf;

int main(int argc, const char *argv[])
{
  try
  {
    // Open an existing volume for reading
    const volume hdfr("201001221325Kurnell.h5", false);
    printf("opened file\n");

    // Create a new volume to copy it into
    volume hdfw(
        "hello.h5", 
        hdfr.valid_time(),
        hdfr.latitude(),
        hdfr.longitude(),
        hdfr.height());

    // Read the source attribute
    string s1, s2, s3, s4, s5, s6;
    hdfr.get_source(s1, s2, s3, s4, s5, s6);
    hdfw.set_source(s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str(), s5.c_str(), s6.c_str());
    cout 
      << "met " << s1 << endl
      << "rad " << s2 << endl
      << "org " << s3 << endl
      << "plc " << s4 << endl
      << "cty " << s5 << endl
      << "cmt " << s6 << endl;

    // Set some quality attributes
    hdfw.set_attribute(att_simulated, true);
    hdfw.set_attribute(att_malfunction, false);
    hdfw.set_attribute(att_level_count, 1234L);
    hdfw.set_attribute(att_rpm, 2.5);
    hdfw.set_attribute(att_task, "RainHDF Test Application");
    hdfw.set_attribute(att_system, std::string("Test string"));
    hdfw.set_attribute(att_cartesian_method, mth_gauge_adjust);
    hdfw.set_attribute(att_nyquist_velocity, 1.0);

    // Get some quality attributes
    bool b1, b2; long n; double f; method e;
    hdfw.attribute(att_simulated, b1);
    hdfw.attribute(att_malfunction, b2);
    hdfw.attribute(att_level_count, n);
    hdfw.attribute(att_rpm, f);
    hdfw.attribute(att_task, s1);
    hdfw.attribute(att_system, s2);
    hdfw.attribute(att_cartesian_method, e);

    cout << b1 << " " << b2 << " " << n 
      << " " << f << " " << s1 << " " << s2 
      << " " << e 
      << endl;

    for (size_t i = 0; i < hdfr.scan_count(); ++i)
    {
      scan::const_ptr sr = hdfr.get_scan(i);
      scan::ptr sw = 
          hdfw.add_scan(
              sr->elevation(),
              sr->azimuth_count(),
              sr->range_bin_count(),
              sr->first_azimuth(),
              sr->range_start(),
              sr->range_scale(),
              sr->start_time(),
              sr->end_time());

      float no_data, undetect;
      vector<float> vec_data(sr->azimuth_count() * sr->range_bin_count());

      for (size_t j = 0; j < sr->layer_count(); ++j)
      {
        // Read the layer data
        data::const_ptr lr = sr->layer(j);
        lr->read(&vec_data[0], no_data, undetect);

        // Write it out as a new layer
        data::ptr lw = 
          sw->add_layer(
              lr->get_quantity(),
              lr->is_quality(),
              &vec_data[0],
              no_data,
              undetect);

        // Now go through and filter our reflectivity (only)
        if (lr->get_quantity() == qty_dbzh)
        {
          for (vector<float>::iterator i = vec_data.begin(); i != vec_data.end(); ++i)
            if (*i < 10)
              *i = 0;

          sw->add_layer(
              lr->get_quantity(),
              true,
              &vec_data[0],
              no_data,
              undetect);
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


