/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "error.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

using namespace rainhdf;

error::error(const char* format, ...)
{
  static const int err_buf_size = 512;
  char buf[err_buf_size];
  va_list ap;

  va_start(ap, format);
  vsnprintf(buf, err_buf_size, format, ap);
  va_end(ap);

  buf[err_buf_size-1] = '\0';
  description_.assign(buf);
}

error::error(hid_t loc, const char* format, ...)
{
  static const int err_buf_size = 512;
  char buf[err_buf_size];
  va_list ap;

  va_start(ap, format);
  vsnprintf(buf, err_buf_size, format, ap);
  va_end(ap);

  buf[err_buf_size-1] = '\0';
  description_.assign(buf);

  // Get the location path from the file
  if (H5Iget_name(loc, buf, err_buf_size) > 0)
  {
    buf[err_buf_size-1] = '\0';
    description_.append(" at ");
    description_.append(buf);
  }
}

error::~error() throw()
{

}

const char* error::what() const throw()
{
  return description_.c_str();
}

