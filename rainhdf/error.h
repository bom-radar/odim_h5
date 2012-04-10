/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_ERROR_H
#define RAINHDF_ERROR_H

#include <rainutil/rainutil.h>
#include <hdf5.h>
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
  class error : public rainfields::error
  {
  public:
    error(hid_t loc, failure_type ft, hdf_object_type ht, const char* name = "");
    error(hid_t loc, failure_type ft, hdf_object_type ht, const std::string& name);
    virtual ~error() RAINFIELDS_NOTHROW;
    virtual const char* what() const RAINFIELDS_NOTHROW;

  private:
    std::string     location_;      ///< string version of location
    failure_type    type_;          ///< type of failure
    hdf_object_type target_;        ///< type of target
    std::string     target_name_;   ///< name of target
  };
}}

#endif

