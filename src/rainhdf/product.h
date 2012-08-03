/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_PRODUCT_H
#define RAINHDF_PRODUCT_H

#include "base.h"

namespace rainfields {
namespace hdf {
  /// Base class for a top level ODIM_H5 object file
  class product : public base
  {
  protected:
    /// Create a new product
    product(
          const std::string& file
        , object_type type
        , const std::string& source
        , time_t valid_time);
    /// Open an existing ODIM_H5 product
    product(const std::string& file, object_type type, bool read_only);

  public:
    /// Ensure all previous writes are flushed to disk
    void flush() const;

    /// Get the source string
    std::string source() const { return get_att<std::string>(hnd_what_, atn_source); }
    /// Wrtie the product source string
    void set_source(const std::string& source) { set_att(hnd_what_, atn_source, source); }
    
    /// Get the nominal (valid) time of the data/product
    time_t valid_time() const { return get_att<time_t>(hnd_what_, atn_date, atn_time); }
    /// Write the nominal (valid) time of the data/product
    void set_valid_time(time_t valid_time) { set_att(hnd_what_, atn_date, atn_time, valid_time); }
  };
}}

#endif


