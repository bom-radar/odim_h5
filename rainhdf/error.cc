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

Error::Error(const char *pszFormat, ...)
{
  static const int kErrBufSize = 512;
  char pszBuffer[kErrBufSize];
  va_list argList;

  va_start(argList, pszFormat);
  vsnprintf(pszBuffer, kErrBufSize, pszFormat, argList);
  va_end(argList);

  pszBuffer[kErrBufSize-1] = '\0';
  m_strDescription.assign(pszBuffer);
}

Error::Error(hid_t hLoc, const char *pszFormat, ...)
{
  static const int kErrBufSize = 512;
  char pszBuffer[kErrBufSize];
  va_list argList;

  va_start(argList, pszFormat);
  vsnprintf(pszBuffer, kErrBufSize, pszFormat, argList);
  va_end(argList);

  pszBuffer[kErrBufSize-1] = '\0';
  m_strDescription.assign(pszBuffer);

  // Get the location path from the file
  if (H5Iget_name(hLoc, pszBuffer, kErrBufSize) > 0)
  {
    pszBuffer[kErrBufSize-1] = '\0';
    m_strDescription.append(" at ");
    m_strDescription.append(pszBuffer);
  }
}

Error::~Error() throw()
{

}

const char * Error::what() const throw()
{
  return m_strDescription.c_str();
}

