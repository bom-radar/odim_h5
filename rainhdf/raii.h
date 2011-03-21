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
      : m_hID(kInvalidHID) 
    { 

    }
    /// Create a HDF5 file
    HID_Handle(HID_File, const char *pszFile, CreateFlag)
      : m_hID(H5Fcreate(pszFile, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error("Failed to create HDF5 file '%s'", pszFile);
    }
    /// Open an existing HDF5 file
    HID_Handle(HID_File, const char *pszFile, bool bReadOnly, OpenFlag) 
      : m_hID(H5Fopen(pszFile, bReadOnly ? H5F_ACC_RDONLY : H5F_ACC_RDWR, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error("Failed to open HDF5 file '%s'", pszFile);
    }
    /// Create a HDF5 group
    HID_Handle(HID_Group, hid_t hParent, const char *pszName, CreateFlag)
      : m_hID(H5Gcreate(hParent, pszName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create group '%s'", pszName);
    }
    /// Create a HDF5 group (append index to name)
    HID_Handle(HID_Group, hid_t hParent, const char *pszName, size_t nIndex, CreateFlag)
    {
      char pszName2[32];
      sprintf(pszName2, "%s%d", pszName, (int) nIndex);
      m_hID = H5Gcreate(hParent, pszName2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create group '%s'", pszName2);
    }
    /// Open an existing HDF5 group
    HID_Handle(HID_Group, hid_t hParent, const char *pszName, OpenFlag)
      : m_hID(H5Gopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open group '%s'", pszName);
    }
    /// Open an existing HDF5 group (append index to name)
    HID_Handle(HID_Group, hid_t hParent, const char *pszName, size_t nIndex, OpenFlag)
    {
      char pszName2[32];
      sprintf(pszName2, "%s%d", pszName, (int) nIndex);
      m_hID = H5Gopen(hParent, pszName2, H5P_DEFAULT);
      if (m_hID < 0) 
        throw Error(hParent, "Failed to open group '%s'", pszName2);
    }
    /// Open an existing HDF5 group if it exists
    HID_Handle(HID_Group, hid_t hParent, const char *pszName, OpenFlag, bool bOptional)
    {
      htri_t ret = H5Lexists(hParent, pszName, H5P_DEFAULT);
      if (ret < 0)
        throw Error(hParent, "Failed to verify existance of group '%s'", pszName);
      else if (ret == 0 && bOptional)
        m_hID = kInvalidHID;
      else
      {
        m_hID = H5Gopen(hParent, pszName, H5P_DEFAULT);
        if (m_hID < 0)
          throw Error(hParent, "Failed to open group '%s'", pszName);
      }
    }
    /// Create a HDF5 type
    HID_Handle(HID_Type, hid_t hID) : m_hID(hID)
    {
      if (m_hID < 0) 
        throw Error("Failed to acquire type");
    }
    /// Create a scalar HDF5 dataspace
    HID_Handle(HID_Space, CreateFlag) 
      : m_hID(H5Screate(H5S_SCALAR))
    {
      if (m_hID < 0) 
        throw Error("Failed to create scalar dataspace");
    }
    /// Create a 2D HDF5 dataspace
    HID_Handle(HID_Space, int nRank, const hsize_t *pDims, CreateFlag)
      : m_hID(H5Screate_simple(nRank, pDims, NULL))
    {
      if (m_hID < 0)
        throw Error("Failed to create rank %d dataspace", nRank);
    }
    /// Construct from existing hid_t
    HID_Handle(HID_Space, hid_t hID)
      : m_hID(hID)
    {
      if (m_hID < 0)
        throw Error("Failed to acquire dataspace");
    }
    /// Create a HDF5 attribute
    HID_Handle(
          HID_Attr
        , hid_t hParent
        , const char *pszName
        , hid_t hType
        , hid_t hSpace
        , CreateFlag)
      : m_hID(H5Acreate(hParent, pszName, hType, hSpace, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create attribute '%s'", pszName);
    }
    /// Open an existing HDF5 attribute
    HID_Handle(HID_Attr, hid_t hParent, const char *pszName, OpenFlag)
      : m_hID(H5Aopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open HDF5 attribute '%s'", pszName);
    }
    /// Create a HDF5 property list
    HID_Handle(HID_PList, hid_t cls_id, CreateFlag) 
      : m_hID(H5Pcreate(cls_id))
    {
      if (m_hID < 0) 
        throw Error("Failed to create property list for class %d", cls_id);
    }
    /// Create a HDF5 dataset
    HID_Handle(
          HID_Data
        , hid_t hParent
        , const char *pszName
        , hid_t hType
        , hid_t hSpace
        , hid_t hPList
        , CreateFlag)
      : m_hID(H5Dcreate(hParent, pszName, hType, hSpace, H5P_DEFAULT, hPList, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create dataset '%s'", pszName);
    }
    /// Open an existing HDF5 dataset
    HID_Handle(HID_Data, hid_t hParent, const char *pszName, OpenFlag)
      : m_hID(H5Dopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open HDF5 dataset '%s'", pszName);
    }

    /// Copy constructor
    HID_Handle(const HID_Handle &hid)
      : m_hID(hid.m_hID)
    {
      H5Iinc_ref(m_hID);
    }

    /// Assignment operator
    HID_Handle & operator=(const HID_Handle &hid)
    {
      m_hID = hid.m_hID;
      H5Iinc_ref(m_hID);
    }

    /// Release the handle
    ~HID_Handle()
    {
      if (m_hID >= 0)
        H5Idec_ref(m_hID);
    }

    /// Check if a handle is valid
    operator bool () const 
    { 
      return m_hID >= 0; 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const
    {
      return m_hID;
    }

  private:
    hid_t m_hID;  ///< HDF5 library object handle
  };
}

#endif

