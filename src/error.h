/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_ERROR_H
#define RAINHDF_ERROR_H

#include "macros.h"

#include <hdf5.h>
#include <stdexcept>
#include <string>

namespace rainfields {
namespace hdf {
  /// possible causes of failure
  enum failure_type
  {
      ft_create
    , ft_open
    , ft_read
    , ft_write
    , ft_remove
    , ft_type_mismatch
    , ft_size_mismatch
    , ft_overflow
    , ft_bad_value
  };

  /// object class on which operation failed
  enum hdf_object_type
  {
      ht_file
    , ht_group
    , ht_type
    , ht_dataspace
    , ht_attribute
    , ht_property_list
    , ht_dataset
  };

  /// rainhdf exception class
  class error : public std::runtime_error
  {
  public:
    error(hid_t loc, failure_type ft, hdf_object_type ht, const char* name = "");
    error(hid_t loc, failure_type ft, hdf_object_type ht, const std::string& name);
    virtual ~error() RAINHDF_NOTHROW;
    virtual const char* what() const RAINHDF_NOTHROW;

  private:
    void generate_description();

  private:
    std::string     location_;
    failure_type    type_;
    hdf_object_type target_;
    std::string     target_name_;
    std::string     description_;
  };
}}

#endif

