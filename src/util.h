/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_UTIL_H
#define RAINHDF_UTIL_H

#include "error.h"
#include "raii.h"
#include <string>

namespace rainfields {
namespace hdf {
  // Retrieve existing attributes
  void get_att(const hid_handle& hid, const char* name, bool& val);
  void get_att(const hid_handle& hid, const char* name, long& val);
  void get_att(const hid_handle& hid, const char* name, double& val);
  void get_att(const hid_handle& hid, const char* name, char* buf, size_t buf_size);
  void get_att(const hid_handle& hid, const char* name, std::string& val);
  void get_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t& val);
  void get_att(const hid_handle& hid, const char* name, long* vals, size_t& size);
  void get_att(const hid_handle& hid, const char* name, double* vals, size_t& size);

  // Create new attributes
  void new_att(const hid_handle& hid, const char* name, bool val);
  void new_att(const hid_handle& hid, const char* name, long val);
  void new_att(const hid_handle& hid, const char* name, double val);
  void new_att(const hid_handle& hid, const char* name, const char* val);
  void new_att(const hid_handle& hid, const char* name, const std::string& val);
  void new_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t val);
  void new_att(const hid_handle& hid, const char* name, const long* vals, size_t size);
  void new_att(const hid_handle& hid, const char* name, const double* vals, size_t size);
  inline void new_att(const hid_handle& hid, const char* name, float val)
  {
    new_att(hid, name, (double) val);
  }
  inline void new_att(const hid_handle& hid, const char* name, char* val)
  {
    new_att(hid, name, (const char*) val);
  }

  // Alter existing attributes (or create if not existing)
  void set_att(const hid_handle& hid, const char* name, bool val);
  void set_att(const hid_handle& hid, const char* name, long val);
  void set_att(const hid_handle& hid, const char* name, double val);
  void set_att(const hid_handle& hid, const char* name, const char* val);
  void set_att(const hid_handle& hid, const char* name, const std::string& val);
  void set_att(const hid_handle& hid, const char* name, const long* vals, size_t size);
  void set_att(const hid_handle& hid, const char* name, const double* vals, size_t size);
  void set_att(const hid_handle& hid, const char* name_date, const char* name_time, time_t val);
  inline void set_att(const hid_handle& hid, const char* name, float val)
  {
    set_att(hid, name, (double) val);
  }
  inline void set_att(const hid_handle& hid, const char* name, char* val) 
  { 
    set_att(hid, name, (const char*) val); 
  }

#if 0
  template <typename T>
  void get_att(const hid_handle& hid, const char* name, T& val)
  {
    char buf[128];
    get_att(hid, name, buf, sizeof(buf));
    val = from_string<T>(buf);
  }

  template <typename T>
  void new_att(const hid_handle& hid, const char* name, T val)
  {
    new_att(hid, name, to_string(val));
  }

  template <typename T>
  void set_att(const hid_handle& hid, const char* name, T val)
  {
    set_att(hid, name, to_string(val));
  }
#endif

  // Convenient value returning versions of above functions (for use in initializer lists)
  template <class T>
  inline T get_att(const hid_handle& hid, const char* name)
  {
    T t;
    get_att(hid, name, t);
    return t;
  }
  template <class T>
  inline T get_att(const hid_handle& hid, const char* name1, const char* name2)
  {
    T t;
    get_att(hid, name1, name2, t);
    return t;
  }

  // Top level file type constants
  extern const char* ot_volume_polar;
  extern const char* ot_volume_cartesian;
  extern const char* ot_scan;
  extern const char* ot_ray;
  extern const char* ot_azimuth;
  extern const char* ot_image;
  extern const char* ot_composite_image;
  extern const char* ot_cross_section;
  extern const char* ot_vertical_profile;
  extern const char* ot_picture;

  // Dataset level object type constants
  extern const char* pt_scan;
  extern const char* pt_ppi;
  extern const char* pt_cappi;
  extern const char* pt_psuedo_cappi;
  extern const char* pt_echo_top;
  extern const char* pt_maximum;
  extern const char* pt_accumulation;
  extern const char* pt_vil;
  extern const char* pt_composite;
  extern const char* pt_vertical_profile;
  extern const char* pt_range_height;
  extern const char* pt_vertical_slice;
  extern const char* pt_vertical_side_panel;
  extern const char* pt_horizontal_side_panel;
  extern const char* pt_ray;
  extern const char* pt_azimuth;
  extern const char* pt_quality;

  // Group name constants
  extern const char* grp_what;
  extern const char* grp_where;
  extern const char* grp_how;
  extern const char* grp_dataset;
  extern const char* grp_data;
  extern const char* grp_quality;

  // Attribute name constants
  extern const char* atn_conventions;
  extern const char* atn_object;
  extern const char* atn_version;
  extern const char* atn_date;
  extern const char* atn_time;
  extern const char* atn_source;
  extern const char* atn_latitude;
  extern const char* atn_longitude;
  extern const char* atn_height;
  extern const char* atn_product;
  extern const char* atn_start_date;
  extern const char* atn_start_time;
  extern const char* atn_end_date;
  extern const char* atn_end_time;
  extern const char* atn_elevation;
  extern const char* atn_first_azimuth;
  extern const char* atn_range_count;
  extern const char* atn_range_start;
  extern const char* atn_range_scale;
  extern const char* atn_azimuth_count;
  extern const char* atn_quantity;
  extern const char* atn_gain;
  extern const char* atn_offset;
  extern const char* atn_no_data;
  extern const char* atn_undetect;
  extern const char* atn_class;
  extern const char* atn_image_version;
  extern const char* atn_levels;
  extern const char* atn_interval;
  extern const char* atn_min_height;
  extern const char* atn_max_height;
  extern const char* atn_xsize;
  extern const char* atn_ysize;
  extern const char* atn_xscale;
  extern const char* atn_yscale;

  // Dataset name constants
  extern const char* dat_data;

  // Attribute value constants
  extern const char* val_true;
  extern const char* val_false;
  extern const char* val_conventions;
  extern const char* val_version;
  extern const char* val_class;
  extern const char* val_image_version;
}}

namespace rainfields
{
#if 0
  /// Traits for the object type enumerate
  template <>
  struct enum_traits<hdf::object_type>
  {
    enum { count = (int) hdf::count_ot };
    static const char* name;
    static const char* strings[];
  };

  /// Traits for the product type enumerate
  template <>
  struct enum_traits<hdf::product_type>
  {
    enum { count = (int) hdf::count_pt };
    static const char* name;
    static const char* strings[];
  };
#endif

#if 0
  /// Traits for the quantity enumerate
  template <>
  struct enum_traits<hdf::quantity>
  {
    enum { count = (int) hdf::count_qt };
    static const char* name;
    static const char* strings[];
  };

  /// Traits for the method enumerate
  template <>
  struct enum_traits<hdf::method>
  {
    enum { count = (int) hdf::count_mth };
    static const char* name;
    static const char* strings[];
  };

  /// Traits for the attribute enumerate
  template <>
  struct enum_traits<hdf::attrib>
  {
    enum { count = (int) hdf::count_att };
    static const char* name;
    static const char* strings[];
  };
#endif
}

#endif

