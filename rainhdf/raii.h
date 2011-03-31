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

namespace RainHDF
{
  // Flags used to distinguish between open and create constructors
  enum CreateFlag { kCreate };
  enum OpenFlag { kOpen };

  enum HID_File { kHID_File };
  enum HID_Group { kHID_Group };
  enum HID_Type { kHID_Type };
  enum HID_Space { kHID_Space };
  enum HID_Attr { kHID_Attr };
  enum HID_PList { kHID_PList };
  enum HID_Data { kHID_Data };

  /// Invalid value indicator for hid_t type
  const int kInvalidHID = -1;

  /// RAII wrapper for HDF hid_t handles
  class HID_Handle
  {
  public:
    /// Construct a null handle
    HID_Handle() 
      : hid_(kInvalidHID) 
    { 

    }
    /// Create a HDF5 file
    HID_Handle(HID_File, const char* file, CreateFlag)
      : hid_(H5Fcreate(file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw Error("Failed to create HDF5 file '%s'", file);
    }
    /// Open an existing HDF5 file
    HID_Handle(HID_File, const char* file, bool read_only, OpenFlag) 
      : hid_(H5Fopen(file, read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw Error("Failed to open HDF5 file '%s'", file);
    }
    /// Create a HDF5 group
    HID_Handle(HID_Group, hid_t parent, const char* name, CreateFlag)
      : hid_(H5Gcreate(parent, name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw Error(parent, "Failed to create group '%s'", name);
    }
    /// Create a HDF5 group (append index to name)
    HID_Handle(HID_Group, hid_t parent, const char* name, size_t index, CreateFlag)
    {
      char name2[32];
      sprintf(name2, "%s%d", name, (int) index);
      hid_ = H5Gcreate(parent, name2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      if (hid_ < 0) 
        throw Error(parent, "Failed to create group '%s'", name2);
    }
    /// Open an existing HDF5 group
    HID_Handle(HID_Group, hid_t parent, const char* name, OpenFlag)
      : hid_(H5Gopen(parent, name, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw Error(parent, "Failed to open group '%s'", name);
    }
    /// Open an existing HDF5 group (append index to name)
    HID_Handle(HID_Group, hid_t parent, const char* name, size_t index, OpenFlag)
    {
      char name2[32];
      sprintf(name2, "%s%d", name, (int) index);
      hid_ = H5Gopen(parent, name2, H5P_DEFAULT);
      if (hid_ < 0) 
        throw Error(parent, "Failed to open group '%s'", name2);
    }
    /// Open an existing HDF5 group if it exists
    HID_Handle(HID_Group, hid_t parent, const char* name, OpenFlag, bool bOptional)
    {
      htri_t ret = H5Lexists(parent, name, H5P_DEFAULT);
      if (ret < 0)
        throw Error(parent, "Failed to verify existance of group '%s'", name);
      else if (ret == 0 && bOptional)
        hid_ = kInvalidHID;
      else
      {
        hid_ = H5Gopen(parent, name, H5P_DEFAULT);
        if (hid_ < 0)
          throw Error(parent, "Failed to open group '%s'", name);
      }
    }
    /// Create a HDF5 type
    HID_Handle(HID_Type, hid_t hid) : hid_(hid)
    {
      if (hid_ < 0) 
        throw Error("Failed to acquire type");
    }
    /// Create a scalar HDF5 dataspace
    HID_Handle(HID_Space, CreateFlag) 
      : hid_(H5Screate(H5S_SCALAR))
    {
      if (hid_ < 0) 
        throw Error("Failed to create scalar dataspace");
    }
    /// Create a 2D HDF5 dataspace
    HID_Handle(HID_Space, int rank, const hsize_t* dims, CreateFlag)
      : hid_(H5Screate_simple(rank, dims, NULL))
    {
      if (hid_ < 0)
        throw Error("Failed to create rank %d dataspace", rank);
    }
    /// Construct from existing hid_t
    HID_Handle(HID_Space, hid_t hid)
      : hid_(hid)
    {
      if (hid_ < 0)
        throw Error("Failed to acquire dataspace");
    }
    /// Create a HDF5 attribute
    HID_Handle(
          HID_Attr
        , hid_t parent
        , const char* name
        , hid_t type
        , hid_t space
        , CreateFlag)
      : hid_(H5Acreate(parent, name, type, space, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw Error(parent, "Failed to create attribute '%s'", name);
    }
    /// Open an existing HDF5 attribute
    HID_Handle(HID_Attr, hid_t parent, const char* name, OpenFlag)
      : hid_(H5Aopen(parent, name, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw Error(parent, "Failed to open HDF5 attribute '%s'", name);
    }
    /// Create a HDF5 property list
    HID_Handle(HID_PList, hid_t cls_id, CreateFlag) 
      : hid_(H5Pcreate(cls_id))
    {
      if (hid_ < 0) 
        throw Error("Failed to create property list for class %d", cls_id);
    }
    /// Create a HDF5 dataset
    HID_Handle(
          HID_Data
        , hid_t parent
        , const char* name
        , hid_t type
        , hid_t space
        , hid_t plist
        , CreateFlag)
      : hid_(H5Dcreate(parent, name, type, space, H5P_DEFAULT, plist, H5P_DEFAULT))
    {
      if (hid_ < 0) 
        throw Error(parent, "Failed to create dataset '%s'", name);
    }
    /// Open an existing HDF5 dataset
    HID_Handle(HID_Data, hid_t parent, const char* name, OpenFlag)
      : hid_(H5Dopen(parent, name, H5P_DEFAULT))
    { 
      if (hid_ < 0)
        throw Error(parent, "Failed to open HDF5 dataset '%s'", name);
    }

    /// Copy constructor
    HID_Handle(const HID_Handle &hid)
      : hid_(hid.hid_)
    {
      H5Iinc_ref(hid_);
    }

    /// Assignment operator
    HID_Handle & operator=(const HID_Handle &hid)
    {
      hid_ = hid.hid_;
      H5Iinc_ref(hid_);
    }

    /// Release the handle
    ~HID_Handle()
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

