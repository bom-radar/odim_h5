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

using namespace RainHDF;

Error::Error(const char *format, ...)
{
  static const int kErrBufSize = 512;
  char buf[kErrBufSize];
  va_list ap;

  va_start(ap, format);
  vsnprintf(buf, kErrBufSize, format, ap);
  va_end(ap);

  buf[kErrBufSize-1] = '\0';
  description_.assign(buf);
}

Error::Error(hid_t loc, const char *format, ...)
{
  static const int kErrBufSize = 512;
  char buf[kErrBufSize];
  va_list ap;

  va_start(ap, format);
  vsnprintf(buf, kErrBufSize, format, ap);
  va_end(ap);

  buf[kErrBufSize-1] = '\0';
  description_.assign(buf);

  // Get the location path from the file
  if (H5Iget_name(loc, buf, kErrBufSize) > 0)
  {
    buf[kErrBufSize-1] = '\0';
    description_.append(" at ");
    description_.append(buf);
  }
}

Error::~Error() throw()
{

}

const char * Error::what() const throw()
{
  return description_.c_str();
}

