/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "../src/rainhdf.h"

#include <iostream>
#include <cstring>
#include <exception>

using namespace std;
using namespace rainfields;
using namespace rainfields::hdf;

int main(int argc, const char *argv[])
{
  try
  {
    // Create a vertical profile
    vertical_profile vprof("myprof.h5", "SOMEWHERE", time(NULL), 
                           123.4, 567.8, 90.0, 
                           10, 100.0, 0.0, 1000.0);
    profile::ptr prof(vprof.add_profile(time(NULL) - 10, time(NULL) - 1));
    int blah[10];
    float blah2[10];
    float blah3[10];
    for (int i = 0; i < 10; ++i)
    {
      blah[i] = i * 2;
      blah2[i] = i * 10.0 / 3;
      blah3[i] = i * 11.5f;
    }

    data::ptr ff(prof->add_layer("ff", false, true));
    ff->write(blah2, -1.0, -1.0);
    data::ptr n(prof->add_layer("n", false, false));
    n->write(blah, 0, 0);
    data::ptr dd(prof->add_layer("dd", false, true));
    dd->write(blah3, -1.0, -1.0);

#if 0
    // Open an existing volume for reading
    const volume hdfr("201001221325Kurnell.h5", false);
    printf("opened file\n");

    // Create a new volume to copy it into
    volume hdfw(
        "hello.h5", 
        hdfr.source(),
        hdfr.valid_time(),
        hdfr.latitude(),
        hdfr.longitude(),
        hdfr.height());

    bool b; long l; double d; std::string s; std::vector<long> vl; std::vector<double> vd;

    for (int i = 0; i < hdfr.attribute_count(); ++i)
    {
      attribute::const_ptr attr(hdfr.attribute(i));
      cout << "attribute '" << attr->name() << "' ";
      switch (attr->type())
      {
      case attribute::at_unknown:
        cout << "unknown" << endl;
        break;
      case attribute::at_bool:
        attr->get(b);
        hdfw.attribute(attr->name().c_str(), true)->set(b);
        cout << "bool " << b << endl;
        break;
      case attribute::at_long:
        attr->get(l);
        hdfw.attribute(attr->name().c_str(), true)->set(l);
        cout << "long " << l << endl;
        break;
      case attribute::at_double:
        attr->get(d);
        hdfw.attribute(attr->name().c_str(), true)->set(d);
        cout << "double " << d << endl;
        break;
      case attribute::at_string:
        attr->get(s);
        hdfw.attribute(attr->name().c_str(), true)->set(s);
        cout << "string " << s << endl;
        break;
      case attribute::at_long_array:
        attr->get(vl);
        hdfw.attribute(attr->name().c_str(), true)->set(vl);
        cout << "long_array ";
        for (int i = 0; i < vl.size(); ++i)
          cout << vl[i] << " ";
        cout << endl;
        break;
      case attribute::at_double_array:
        attr->get(vd);
        hdfw.attribute(attr->name().c_str(), true)->set(vd);
        cout << "double_array ";
        for (int i = 0; i < vd.size(); ++i)
          cout << vd[i] << " ";
        cout << endl;
        break;
      }
    }

    hdfw.attribute("hello", true)->set(std::string("blah blah"));

#if 0
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
#endif

    for (size_t i = 0; i < hdfr.scan_count(); ++i)
    {
      scan::const_ptr sr = hdfr.scan(i);
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
        data::ptr lw = sw->add_layer(lr->quantity().c_str(), lr->is_quality(), true);
        lw->write(&vec_data[0], no_data, undetect);
      }

      printf("open objs: %d\n", H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_ALL));
    }
#endif

    printf("open objs: %d (should be 8?)\n", H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_ALL));
  }
  catch (std::exception &err)
  {
    cout << "error: " << err.what() << endl;
  }

  printf("open objs: %d (should be 0)\n", H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_ALL));

  cout << "done" << endl;
}


