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
    const std::string& name() const  { return name_; }

    data_type type() const;

    void get(bool& val) const;
    void get(long& val) const;
    void get(double& val) const;
    void get(std::string& val) const;
    void get(std::vector<long>& val) const;
    void get(std::vector<double>& val) const;

    void set(bool val);
    void set(long val);
    void set(double val);
    void set(const char* val);
    void set(const std::string& val);
    void set(const std::vector<long>& val);
    void set(const std::vector<double>& val);

  private:
    attribute(const hid_handle& parent, int index);
    attribute(const hid_handle& parent, const char* name, bool creating);

    void determine_type(const hid_handle& attr) const;

  private:
    hid_handle          parent_;      ///< Handle to our attribute
    std::string         name_;        ///< Name of the attribute
    bool                creating_;    ///< Is attribute being created?

    mutable data_type   type_;        ///< Type of attribute data (cached)
    mutable int         size_;        ///< Size of array types (cached)
    mutable std::string value_;       ///< Cached attribute value for string/bool types only

    friend class base;
  };
}

#endif


