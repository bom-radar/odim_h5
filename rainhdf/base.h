/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_BASE_H
#define RAINHDF_BASE_H

#include "util.h"

namespace RainHDF
{
  /// Base class for a single level in a ODIM_H5 product hierarchy
  class Base
  {
  public:
    /// Create a new level as the root of a file
    Base(const std::string& file, CreateFlag);

    /// Create a new level by name
    Base(const Base& parent, const char* name, CreateFlag);

    /// Create a new level by appending an index to a name
    Base(const Base& parent, const char* name, int index, CreateFlag);

    /// Open the rool level from a file
    Base(const std::string& file, bool read_only, OpenFlag);

    /// Open a level by name
    Base(const Base& parent, const char* name, OpenFlag);

    /// Open a level by appending an index to a name
    Base(const Base& parent, const char* name, int index, OpenFlag);

    /// Destroy this object
    virtual ~Base();

    /// Get the set of attributes that are local to this level
    const AttFlags& attribute_flags() const { return att_flags_; }

    /// Read an optional attribute
    template <class T>
    bool attribute(Attribute attrib, T& val) const;

    /// Write an optional attribute
    template <class T>
    void set_attribute(Attribute attrib, const T& val);

  protected:
    const Base*   parent_;      ///< Parent level (used to recursive search attributes)
    HID_Handle    hnd_this_;    ///< 'This' group
    HID_Handle    hnd_what_;    ///< What group
    HID_Handle    hnd_where_;   ///< Where group
    HID_Handle    hnd_how_;     ///< How group
    AttFlags      att_flags_;   ///< Flags to indicate presence of 'how' group attributes
  };

  template <class T>
  bool Base::attribute(Attribute attrib, T& val) const 
  { 
    // TODO - manually verify type of attribute? (will throw read error if reading wrong type)
    for (const Base* pLevel = this; pLevel != NULL; pLevel = pLevel->parent_)
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
  void Base::set_attribute(Attribute attrib, const T& val)
  {
    // TODO - manually verify type of attribute? (will allow writing of non-standard type)
    if (!hnd_how_)
      hnd_how_ = HID_Handle(kHID_Group, hnd_this_, kGrp_How, kCreate);
    set_att(hnd_how_, to_string(attrib), val);
    att_flags_.set(attrib);
  }
}

#endif

