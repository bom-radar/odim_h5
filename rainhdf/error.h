/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_ERROR_H
#define RAINHDF_ERROR_H

#include <hdf5.h>

#include <exception>
#include <string>

namespace RainHDF
{
  /// Base exception class
  /**
   * Exceptions derived from this class should be thrown by reference.
   * ie: throw MyException();  This ensures that the correct virtual
   * what() function will be called.
   */
  class Error : public std::exception
  {
  public:
    /// Construct an error using printf style arguments
    Error(const char* format, ...);

    /// Construct an error using printf style arguments and a file location
    Error(hid_t loc, const char* format, ...);

    /// Error destructor
    virtual ~Error() throw();

    /// Get the description of the error
    virtual const char* what() const throw();

  private:
    std::string description_; ///< Description of the error
  };
}

#endif

