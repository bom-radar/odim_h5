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
  /// Base exception class
  /**
   * Exceptions derived from this class should be thrown by reference.
   * ie: throw MyException();  This ensures that the correct virtual
   * what() function will be called.
   */
  class error : public rainfields::error
  {
  public:
    /// Construct an error using printf style arguments
    error(const char* format, ...);

    /// Construct an error using printf style arguments and a file location
    error(hid_t loc, const char* format, ...);
  };
}}

#endif

