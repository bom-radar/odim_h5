/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_BASE_H
#define RAINHDF_BASE_H

#include "util.h"

namespace rainhdf
{
  /// Base class for a single level in a ODIM_H5 product hierarchy
  class base
  {
  public:
    /// Create a new level as the root of a file
    base(const std::string& file, create_flag);

    /// Create a new level by name
    base(const base& parent, const char* name, create_flag);

    /// Create a new level by appending an index to a name
    base(const base& parent, const char* name, int index, create_flag);

    /// Open the rool level from a file
    base(const std::string& file, bool read_only, open_flag);

    /// Open a level by name
    base(const base& parent, const char* name, open_flag);

    /// Open a level by appending an index to a name
    base(const base& parent, const char* name, int index, open_flag);

    /// Destroy this object
    virtual ~base();

    /// Get the set of attributes that are local to this level
    const att_flags& attribute_flags() const { return att_flags_; }

    /// Read an optional attribute
    template <class T>
    bool attribute(rainhdf::attribute attrib, T& val) const;

    /// Write an optional attribute
    template <class T>
    void set_attribute(rainhdf::attribute attrib, const T& val);

  protected:
    const base*   parent_;      ///< Parent level (used to recursive search attributes)
    hid_handle    hnd_this_;    ///< 'This' group
    hid_handle    hnd_what_;    ///< What group
    hid_handle    hnd_where_;   ///< Where group
    hid_handle    hnd_how_;     ///< How group
    att_flags     att_flags_;   ///< Flags to indicate presence of 'how' group attributes
  };

  template <class T>
  bool base::attribute(rainhdf::attribute attrib, T& val) const 
  { 
    // TODO - manually verify type of attribute? (will throw read error if reading wrong type)
    for (const base* pLevel = this; pLevel != NULL; pLevel = pLevel->parent_)
    {
      if (pLevel->att_flags_.test(attrib))
      {
        get_att(pLevel->hnd_how_, to_string(attrib), val);
        return true;
      }
    }
    return false;
  }

  template <class T>
  void base::set_attribute(rainhdf::attribute attrib, const T& val)
  {
    // TODO - manually verify type of attribute? (will allow writing of non-standard type)
    if (!hnd_how_)
      hnd_how_ = hid_handle(hid_group, hnd_this_, grp_how, create);
    set_att(hnd_how_, to_string(attrib), val);
    att_flags_.set(attrib);
  }
}

#endif

