/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_MACROS_H
#define RAINHDF_MACROS_H

// detect c++11 support
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#define RAINHDF_CXX11 1
#endif

// build the gcc version
#define RAINHDF_GCC_VERSION (  __GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

// gcc extension used to validate arguments to printf style vararg functions
#ifdef __GNUC__
  #define RAINHDF_CHECK_PRINTF_ARGS(m, n) __attribute__((__format__(__printf__, m, n)))
#else
  #define RAINHDF_CHECK_PRINTF_ARGS(m, n)
#endif

// throw() replacement for c++11
#if RAINHDF_CXX11
  #if defined(__GNUC__) && RAINHDF_GCC_VERSION < 40600
    #define RAINHDF_NOTHROW throw()
  #else
    #define RAINHDF_NOTHROW noexcept
  #endif
#else
  #define RAINHDF_NOTHROW throw()
#endif

#endif
