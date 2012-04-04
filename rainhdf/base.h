/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_BASE_H
#define RAINHDF_BASE_H

#include "util.h"
#include "attribute.h"

namespace rainfields {
namespace hdf {
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

    /// Get the number of 'how' attributes
    int attribute_count() const { return num_attrs_; }
    rainfields::hdf::attribute::ptr       attribute(int i);
    rainfields::hdf::attribute::const_ptr attribute(int i) const;
    rainfields::hdf::attribute::ptr       attribute(const char* name, bool create_if_missing = false);
    rainfields::hdf::attribute::const_ptr attribute(const char* name) const;

  protected:
    void check_create_what()
    {
      if (!hnd_what_)
        hnd_what_ = hid_handle(hid_group, hnd_this_, grp_what, create);
    }
    void check_create_where()
    {
      if (!hnd_where_)
        hnd_where_ = hid_handle(hid_group, hnd_this_, grp_where, create);
    }
    void check_create_how()
    {
      if (!hnd_how_)
        hnd_how_ = hid_handle(hid_group, hnd_this_, grp_how, create);
    }

  protected:
    hid_handle    hnd_this_;    ///< 'This' group
    hid_handle    hnd_what_;    ///< What group
    hid_handle    hnd_where_;   ///< Where group
    hid_handle    hnd_how_;     ///< How group
    int           num_attrs_;   ///< Number of attributes available
  };
}}

#endif

