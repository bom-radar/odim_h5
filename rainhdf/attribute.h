/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_ATTRIBUTE_H
#define RAINHDF_ATTRIBUTE_H

#include "raii.h"

#include <memory>
#include <string>
#include <vector>

namespace rainhdf
{
  /// An optional (ie: 'how' group) attribute
  class attribute
  {
  public:
    typedef std::auto_ptr<attribute> ptr;
    typedef std::auto_ptr<const attribute> const_ptr;

    enum data_type
    {
        at_unknown
      , at_bool
      , at_long
      , at_double
      , at_string
      , at_long_array
      , at_double_array
    };

  public:
    std::string name() const  { return name_; }

    data_type type() const;

    bool as_bool() const            { if (type_ == at_unknown) load(); return data_.as_bool; }
    long as_long() const            { if (type_ == at_unknown) load(); return data_.as_long; }
    double as_double() const        { if (type_ == at_unknown) load(); return data_.as_double; }
    const std::string& as_string() const 
    { 
      if (type_ == at_unknown) 
        load(); 
      return *data_.as_string; 
    }
    const std::vector<long>& as_long_array() const 
    { 
      if (type_ == at_unknown) 
        load(); 
      return *data_.as_long_array; 
    }
    const std::vector<double>& as_double_array() const 
    { 
      if (type_ == at_unknown) 
        load(); 
      return *data_.as_double_array; 
    }

  private:
    attribute(const hid_handle& parent, int index);
    attribute(const hid_handle& parent, const char* name);

  private:
    hid_handle  hnd_this_;    ///< Handle to our attribute!
    std::string name_;        ///< Name of the attribute

    friend class base;
  };

#if 0
  public:

    attribute(const hid_handle& hid, const char* name, data_type type = at_unknown);
    ~attribute();

    const std::string& name() const { return name_; }
    data_type type() const          { if (type_ == at_unknown) load(); return type_; }

    bool as_bool() const            { if (type_ == at_unknown) load(); return data_.as_bool; }
    long as_long() const            { if (type_ == at_unknown) load(); return data_.as_long; }
    double as_double() const        { if (type_ == at_unknown) load(); return data_.as_double; }
    const std::string& as_string() const 
    { 
      if (type_ == at_unknown) 
        load(); 
      return *data_.as_string; 
    }
    const std::vector<long>& as_long_array() const 
    { 
      if (type_ == at_unknown) 
        load(); 
      return *data_.as_long_array; 
    }
    const std::vector<double>& as_double_array() const 
    { 
      if (type_ == at_unknown) 
        load(); 
      return *data_.as_double_array; 
    }

    void set(bool val);
    void set(long val);
    void set(double val);
    void set(const char* val);
    void set(const std::string& val);
    void set(const long* val, int size);
    void set(const double* val, int size);

  private:
    void load() const;

  private:
    const hid_handle& hid_;
    std::string name_;
    mutable data_type   type_;
    mutable union
    {
      bool                  as_bool;
      long                  as_long;
      double                as_double;
      std::string*          as_string;
      std::vector<long>*    as_long_array;
      std::vector<double>*  as_double_array;
    } data_;
  };
#endif
}

#endif


