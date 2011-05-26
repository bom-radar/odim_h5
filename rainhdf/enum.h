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

namespace rainhdf
{
  /// Empty enumerate traits template
  template <typename T>
  struct enum_traits 
  { 
    enum { count = -1 };
    // enum { count };                  // Number of enumerates
    // static const char *  name;       // Name of enumerate
    // static const char ** strings;    // String representation
    // static const char ** labels;     // User friendly labels (optional)
  };

  /// Convert an enumerate into it's serializable string name
  template <typename T>
  const char* to_string(T val)
  {
    return enum_traits<T>::strings[(int) val];
  }

  /// Retrieve an enumerate from it's string
  template <typename T>
  T from_string(const char* val)
  {
    for (int i = 0; i < enum_traits<T>::count; ++i)
      if (std::strcmp(val, enum_traits<T>::strings[i]) == 0)
        return (T) i;
    throw error("Invalid %s enumerate '%s'", enum_traits<T>::name, val);
  }

  /// Extended bitset class that allows using the enumerate as flags
  template <typename T, int C = enum_traits<T>::count >
  class bitset : public std::bitset<C>
  {
  private:
    typedef std::bitset<C> base;

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
    inline bitset& set() { base::set(); return *this; }
    inline bitset& set(T pos) { base::set((size_t) pos); return *this; }
    inline bitset& set(T pos, bool val) { base::set((size_t) pos, val); return *this; }
    inline bitset& reset() { base::reset(); return *this; }
    inline bitset& reset(T pos) { base::reset((size_t) pos); return *this; }
    inline bitset& flip() { base::flip(); return *this; }
    inline bitset& flip(T pos) { base::flip((size_t) pos); return *this; }
    inline bool test(T pos) const { return base::test((size_t) pos); }

    // Extended test - check all bits in val are set
    inline bool test(const bitset& val) const { return val == (*this & val); }

    // Extend constructors
    inline bitset() { }
    inline bitset(const bitset& bset) : base(bset) { }
    inline bitset(T pos) { base::set((size_t) pos); }
  };
}

#endif

