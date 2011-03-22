/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_ENUM_H
#define RAINHDF_ENUM_H

#include "error.h"

#include <string>
#include <cstring>
#include <bitset>

namespace RainHDF
{
  /// Macro to declare a basic enumerate with serialization features
  #define BASIC_ENUM(x, c) \
    template <> \
    struct enum_traits<x> \
    { \
      static const char * s_pszName; \
      static const int s_nCount = (int) c; \
      static const char ** s_pszStrings; \
    };

  /// Macro to define the traits of a basic enumerate
  /**
   * \param x The enumerate type
   * \param s Static constant array holding enumerate strings
   */
  #define BASIC_ENUM_DEFN(x, s) \
    const char * enum_traits<x>::s_pszName = #x; \
    const char ** enum_traits<x>::s_pszStrings = s

  /// Empty enumerate traits template
  template <typename T>
  struct enum_traits 
  { 
    // static const char *  s_pszName;      // Name of enumerate
    // static const int     s_nCount;       // Number of entries
    // static const char ** s_pszStrings;   // String representation
    // static const char ** s_pszLabels;    // User friendly labels (optional)
  };

  /// Convert an enumerate into it's string name
  template <typename T>
  const char * to_string(T eVal)
  {
    return enum_traits<T>::s_pszStrings[(int) eVal];
  }

  /// Retrieve an enumerate from it's string
  template <typename T>
  T from_string(const char *pszVal)
  {
    for (int i = 0; i < enum_traits<T>::s_nCount; ++i)
      if (std::strcmp(pszVal, enum_traits<T>::s_pszStrings[i]) == 0)
        return (T) i;
    throw Error("Invalid %s enumerate '%s'", enum_traits<T>::s_pszName, pszVal);
  }

  /// Extended bitset class that allows using the enumerate as flags
  template <typename T>
  class bitset : public std::bitset<enum_traits<T>::s_nCount>
  {
  private:
    typedef std::bitset<enum_traits<T>::s_nCount> base;

  public:
    // Allow access via integer types
    using base::operator=;
    using base::operator==;
    using base::operator[];
    using base::set;
    using base::reset;
    using base::flip;
    using base::test;

    // Extend to allow access via enumerate type (without laborious casting)
    inline bool operator[] (T pos) const { return base::operator[]((size_t) pos); }
    inline typename base::reference operator[] ( T pos) { return base::operator[]((size_t) pos); }
    inline bitset & set() { base::set(); return *this; }
    inline bitset & set(T pos) { base::set((size_t) pos); return *this; }
    inline bitset & set(T pos, bool val) { base::set((size_t) pos, val); return *this; }
    inline bitset & reset() { base::reset(); return *this; }
    inline bitset & reset(T pos) { base::reset((size_t) pos); return *this; }
    inline bitset & flip() { base::flip(); return *this; }
    inline bitset & flip(T pos) { base::flip((size_t) pos); return *this; }
    inline bool test(T pos) const { return base::test((size_t) pos); }

    // Extended test - check all bits in val are set
    inline bool test(const bitset &val) const { return val == (*this & val); }

    // Extend constructors
    inline bitset() { }
    inline bitset(const bitset &bset) : base(bset) { }
    inline bitset(T pos) { base::set((size_t) pos); }
  };

  /// Construct a bitset from two enumerates
  template <typename T>
  inline bitset<T> operator|(T l, T r)
  {
    return bitset<T>(l).set(r);
  }

  /// Construct a bitset from a bitset and an enumerate
  template <typename T>
  inline bitset<T> operator|(bitset<T> l, T r)
  {
    return l.set(r);
  }
}

#endif

