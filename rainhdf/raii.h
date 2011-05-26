/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_RAII_H
#define RAINHDF_RAII_H

#include "error.h"

#include <hdf5.h>

namespace rainhdf
{
  // Flags used to distinguish between open and create constructors
  enum create_flag { create };
  enum open_flag { open };

  enum hid_file_flag  { hid_file };
  enum hid_group_flag { hid_group };
  enum hid_type_flag  { hid_type };
  enum hid_space_flag { hid_space };
  enum hid_attr_flag  { hid_attr };
  enum hid_plist_flag { hid_plist };
  enum hid_data_flag  { hid_data };

  /// Invalid value indicator for hid_t type
  const int invalid_hid = -1;

  /// RAII wrapper for HDF hid_t handles
  class hid_handle
  {
  public:
    /// Construct a null handle
    hid_handle() 
      : hid_(invalid_hid) 
    { 

    }
    /// Create a HDF5 file
    hid_handle(hid_file_flag, const char* file, create_flag)
      : hid_(H5Fcreate(file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw error("Failed to create HDF5 file '%s'", file);
    }
    /// Open an existing HDF5 file
    hid_handle(hid_file_flag, const char* file, bool read_only, open_flag) 
      : hid_(H5Fopen(file, read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw error("Failed to open HDF5 file '%s'", file);
    }
    /// Create a HDF5 group
    hid_handle(hid_group_flag, hid_t parent, const char* name, create_flag)
      : hid_(H5Gcreate(parent, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw error(parent, "Failed to create group '%s'", name);
    }
    /// Create a HDF5 group (append index to name)
    hid_handle(hid_group_flag, hid_t parent, const char* name, size_t index, create_flag)
    {
      char name2[32];
      sprintf(name2, "%s%d", name, (int) index);
      hid_ = H5Gcreate(parent, name2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      if (hid_ < 0) 
        throw error(parent, "Failed to create group '%s'", name2);
    }
    /// Open an existing HDF5 group
    hid_handle(hid_group_flag, hid_t parent, const char* name, open_flag)
      : hid_(H5Gopen(parent, name, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw error(parent, "Failed to open group '%s'", name);
    }
    /// Open an existing HDF5 group (append index to name)
    hid_handle(hid_group_flag, hid_t parent, const char* name, size_t index, open_flag)
    {
      char name2[32];
      sprintf(name2, "%s%d", name, (int) index);
      hid_ = H5Gopen(parent, name2, H5P_DEFAULT);
      if (hid_ < 0) 
        throw error(parent, "Failed to open group '%s'", name2);
    }
    /// Open an existing HDF5 group if it exists
    hid_handle(hid_group_flag, hid_t parent, const char* name, open_flag, bool bOptional)
    {
      htri_t ret = H5Lexists(parent, name, H5P_DEFAULT);
      if (ret < 0)
        throw error(parent, "Failed to verify existance of group '%s'", name);
      else if (ret == 0 && bOptional)
        hid_ = invalid_hid;
      else
      {
        hid_ = H5Gopen(parent, name, H5P_DEFAULT);
        if (hid_ < 0)
          throw error(parent, "Failed to open group '%s'", name);
      }
    }
    /// Create a HDF5 type
    hid_handle(hid_type_flag, hid_t hid) : hid_(hid)
    {
      if (hid_ < 0) 
        throw error("Failed to acquire type");
    }
    /// Create a scalar HDF5 dataspace
    hid_handle(hid_space_flag, create_flag) 
      : hid_(H5Screate(H5S_SCALAR))
    {
      if (hid_ < 0) 
        throw error("Failed to create scalar dataspace");
    }
    /// Create a 2D HDF5 dataspace
    hid_handle(hid_space_flag, int rank, const hsize_t* dims, create_flag)
      : hid_(H5Screate_simple(rank, dims, NULL))
    {
      if (hid_ < 0)
        throw error("Failed to create rank %d dataspace", rank);
    }
    /// Construct from existing hid_t
    hid_handle(hid_space_flag, hid_t hid)
      : hid_(hid)
    {
      if (hid_ < 0)
        throw error("Failed to acquire dataspace");
    }
    /// Create a HDF5 attribute
    hid_handle(
          hid_attr_flag
        , hid_t parent
        , const char* name
        , hid_t type
        , hid_t space
        , create_flag)
      : hid_(H5Acreate(parent, name, type, space, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw error(parent, "Failed to create attribute '%s'", name);
    }
    /// Open an existing HDF5 attribute
    hid_handle(hid_attr_flag, hid_t parent, const char* name, open_flag)
      : hid_(H5Aopen(parent, name, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw error(parent, "Failed to open HDF5 attribute '%s'", name);
    }
    /// Create a HDF5 property list
    hid_handle(hid_plist_flag, hid_t cls_id, create_flag) 
      : hid_(H5Pcreate(cls_id))
    {
      if (hid_ < 0) 
        throw error("Failed to create property list for class %d", cls_id);
    }
    /// Create a HDF5 dataset
    hid_handle(
          hid_data_flag
        , hid_t parent
        , const char* name
        , hid_t type
        , hid_t space
        , hid_t plist
        , create_flag)
      : hid_(H5Dcreate(parent, name, type, space, H5P_DEFAULT, plist, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw error(parent, "Failed to create dataset '%s'", name);
    }
    /// Open an existing HDF5 dataset
    hid_handle(hid_data_flag, hid_t parent, const char* name, open_flag)
      : hid_(H5Dopen(parent, name, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw error(parent, "Failed to open HDF5 dataset '%s'", name);
    }

    /// Copy constructor
    hid_handle(const hid_handle &hid)
      : hid_(hid.hid_)
    {
      H5Iinc_ref(hid_);
    }

    /// Assignment operator
    hid_handle& operator=(const hid_handle &hid)
    {
      if (hid_ >= 0)
        H5Idec_ref(hid_);
      hid_ = hid.hid_;
      H5Iinc_ref(hid_);
    }

    /// Release the handle
    ~hid_handle()
    {
      if (hid_ >= 0)
        H5Idec_ref(hid_);
    }

    /// Check if a handle is valid
    operator bool () const 
    { 
      return hid_ >= 0; 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const
    {
      return hid_;
    }

  private:
    hid_t hid_;  ///< HDF5 library object handle
  };
}

#endif

