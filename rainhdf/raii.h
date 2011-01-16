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

  /// Invalid value indicator for hid_t type
  const int kInvalidHID = -1;

  /// RAII HDF file ID handle
  class HID_File
  {
  public:
    /// Create a HDF5 file
    HID_File(const char *pszFile, CreateFlag eCreate)
      : m_hID(H5Fcreate(pszFile, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error("Failed to create HDF5 file '%s'", pszFile);
    }

    /// Open an existing HDF5 file
    HID_File(const char *pszFile, bool bReadOnly, OpenFlag eOpen) 
      : m_hID(H5Fopen(pszFile, bReadOnly ? H5F_ACC_RDONLY : H5F_ACC_RDWR, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error("Failed to open HDF5 file '%s'", pszFile);
    }

    /// Disable copy construction
    HID_File(const HID_File &hid) = delete;

    /// Move construction
    HID_File(HID_File &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_File & operator=(const HID_File &hid) = delete;

    /// Move assignment
    HID_File & operator=(HID_File &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 file
    ~HID_File() 
    { 
      if (m_hID >= 0)
        H5Fclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const 
    { 
      return m_hID; 
    }

  private:
    hid_t m_hID;
  };

  /// RAII HDF group ID handle
  class HID_Group
  {
  public:
    /// Construct a null handle
    HID_Group() : m_hID(kInvalidHID) { }

    /// Create a HDF5 group
    HID_Group(hid_t hParent, const char *pszName, CreateFlag eCreate)
      : m_hID(H5Gcreate(hParent, pszName, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create group '%s'", pszName);
    }

    /// Open an existing HDF5 group
    HID_Group(hid_t hParent, const char *pszName, OpenFlag eOpen)
      : m_hID(H5Gopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open HDF5 group '%s'", pszName);
    }

    /// Open an existing HDF5 group if it exists
    HID_Group(hid_t hParent, const char *pszName, OpenFlag eOpen, bool bOptional)
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
          throw Error(hParent, "Failed to open HDF5 group '%s'", pszName);
      }
    }

    /// Disable copy construction
    HID_Group(const HID_Group &hid) = delete;

    /// Move construction
    HID_Group(HID_Group &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_Group & operator=(const HID_Group &hid) = delete;

    /// Move assignment
    HID_Group & operator=(HID_Group &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 group
    ~HID_Group() 
    { 
      if (m_hID >= 0) 
        H5Gclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const { return m_hID; }

    /// Check if a handle is valid
    operator bool () const { printf("checking it yall\n");return m_hID >= 0; }

  private:
    hid_t m_hID;
  };

  /// RAII HDF type ID handle
  class HID_Type
  {
  public:
    /// Create a HDF5 type
    HID_Type(hid_t hID) : m_hID(hID)
    {
      if (m_hID < 0) 
        throw Error("Failed to acquire type");
    }

    /// Disable copy construction
    HID_Type(const HID_Type &hid) = delete;

    /// Move construction
    HID_Type(HID_Type &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_Type & operator=(const HID_Type &hid) = delete;

    /// Move assignment
    HID_Type & operator=(HID_Type &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 type
    ~HID_Type() 
    { 
      if (m_hID >= 0)
        H5Tclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const 
    { 
      return m_hID; 
    }

  private:
    hid_t m_hID;
  };

  /// RAII HDF dataspace ID handle
  class HID_Space
  {
  public:
    /// Create a scalar HDF5 dataspace
    HID_Space(CreateFlag eCreate) 
      : m_hID(H5Screate(H5S_SCALAR))
    {
      if (m_hID < 0) 
        throw Error("Failed to create scalar dataspace");
    }

    /// Create a 2D HDF5 dataspace
    HID_Space(int nRank, const hsize_t *pDims, CreateFlag eCreate)
      : m_hID(H5Screate_simple(nRank, pDims, NULL))
    {
      if (m_hID < 0)
        throw Error("Failed to create rank %d dataspace", nRank);
    }

    /// Construct from existing hid_t
    HID_Space(hid_t hID)
      : m_hID(hID)
    {
      if (m_hID < 0)
        throw Error("Failed to acquire dataspace");
    }

    /// Disable copy construction
    HID_Space(const HID_Space &hid) = delete;

    /// Move construction
    HID_Space(HID_Space &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_Space & operator=(const HID_Space &hid) = delete;

    /// Move assignment
    HID_Space & operator=(HID_Space &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 dataspace
    ~HID_Space() 
    { 
      if (m_hID >= 0)
        H5Sclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const 
    { 
      return m_hID; 
    }

  private:
    hid_t m_hID;
  };

  /// RAII HDF attribute ID handle
  class HID_Attr
  {
  public:
    /// Create a HDF5 attribute
    HID_Attr(
          hid_t hParent
        , const char *pszName
        , hid_t hType
        , hid_t hSpace
        , CreateFlag eCreate)
      : m_hID(H5Acreate(hParent, pszName, hType, hSpace, H5P_DEFAULT, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create attribute '%s'", pszName);
    }

    /// Open an existing HDF5 attribute
    HID_Attr(hid_t hParent, const char *pszName, OpenFlag eOpen)
      : m_hID(H5Aopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open HDF5 attribute '%s'", pszName);
    }

    /// Disable copy construction
    HID_Attr(const HID_Attr &hid) = delete;

    /// Move construction
    HID_Attr(HID_Attr &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_Attr & operator=(const HID_Attr &hid) = delete;

    /// Move assignment
    HID_Attr & operator=(HID_Attr &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 attribute
    ~HID_Attr() 
    { 
      if (m_hID >= 0)
        H5Aclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const 
    { 
      return m_hID; 
    }

  private:
    hid_t m_hID;
  };

  /// RAII HDF property list ID handle
  class HID_PList
  {
  public:
    /// Create a HDF5 property list
    HID_PList(hid_t cls_id, CreateFlag eCreate) 
      : m_hID(H5Pcreate(cls_id))
    {
      if (m_hID < 0) 
        throw Error("Failed to create property list for class %d", cls_id);
    }

    /// Disable copy construction
    HID_PList(const HID_PList &hid) = delete;

    /// Move construction
    HID_PList(HID_PList &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_PList & operator=(const HID_PList &hid) = delete;

    /// Move assignment
    HID_PList & operator=(HID_PList &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 property list
    ~HID_PList() 
    { 
      if (m_hID >= 0)
        H5Pclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const 
    { 
      return m_hID; 
    }

  private:
    hid_t m_hID;
  };

  /// RAII HDF dataset ID handle
  class HID_Data
  {
  public:
    /// Create a HDF5 dataset
    HID_Data(
          hid_t hParent
        , const char *pszName
        , hid_t hType
        , hid_t hSpace
        , hid_t hPList
        , CreateFlag eCreate)
      : m_hID(H5Dcreate(hParent, pszName, hType, hSpace, H5P_DEFAULT, hPList, H5P_DEFAULT))
    {
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create dataset '%s'", pszName);
    }

    /// Open an existing HDF5 dataset
    HID_Data(hid_t hParent, const char *pszName, OpenFlag eOpen)
      : m_hID(H5Dopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open HDF5 dataset '%s'", pszName);
    }

    /// Disable copy construction
    HID_Data(const HID_Data &hid) = delete;

    /// Move construction
    HID_Data(HID_Data &&hid) 
      : m_hID(hid.m_hID) 
    { 
      hid.m_hID = kInvalidHID;
    }

    /// Disable copy assignment
    HID_Data & operator=(const HID_Data &hid) = delete;

    /// Move assignment
    HID_Data & operator=(HID_Data &&hid) 
    { 
      hid_t hTemp = m_hID;
      m_hID = hid.m_hID; 
      hid.m_hID = hTemp;
      return *this; 
    }

    /// Close the HDF5 dataset
    ~HID_Data() 
    { 
      if (m_hID >= 0)
        H5Dclose(m_hID); 
    }

    /// Convert to underlying hid_t handle
    operator const hid_t &() const 
    { 
      return m_hID; 
    }

  private:
    hid_t m_hID;
  };
}

#endif

