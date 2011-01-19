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
    /// Disable copy construction
    HID_File(const HID_File &hid) { throw Error("ASSERT: Copy of RAII object"); }

    /// Disable copy assignment
    HID_File & operator=(const HID_File &hid) { throw Error("ASSERT: Assign of RAII object"); }

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

    /// Create a HDF5 group (append index to name)
    HID_Group(hid_t hParent, const char *pszName, size_t nIndex, CreateFlag eCreate)
    {
      char pszName2[32];
      sprintf(pszName2, "%s%d", pszName, (int) nIndex);
      m_hID = H5Gcreate(hParent, pszName2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      if (m_hID < 0) 
        throw Error(hParent, "Failed to create group '%s'", pszName2);
    }

    /// Open an existing HDF5 group
    HID_Group(hid_t hParent, const char *pszName, OpenFlag eOpen)
      : m_hID(H5Gopen(hParent, pszName, H5P_DEFAULT))
    { 
      if (m_hID < 0)
        throw Error(hParent, "Failed to open group '%s'", pszName);
    }

    /// Open an existing HDF5 group (append index to name)
    HID_Group(hid_t hParent, const char *pszName, size_t nIndex, OpenFlag eOpen)
    {
      char pszName2[32];
      sprintf(pszName2, "%s%d", pszName, (int) nIndex);
      m_hID = H5Gopen(hParent, pszName2, H5P_DEFAULT);
      if (m_hID < 0) 
        throw Error(hParent, "Failed to open group '%s'", pszName2);
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
          throw Error(hParent, "Failed to open group '%s'", pszName);
      }
    }

    /// Copy (move) assignment
    HID_Group & operator=(HID_Group &hid)
    {
      if (m_hID >= 0)
        H5Gclose(m_hID);
      m_hID = hid.m_hID;
      hid.m_hID = kInvalidHID;
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
    /// Disable copy construction
    HID_Group(const HID_Group &hid) { throw Error("ASSERT: Copy of RAII object"); }

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
    /// Disable copy construction
    HID_Type(const HID_Type &hid) { throw Error("ASSERT: Copy of RAII object"); }

    /// Disable copy assignment
    HID_Type & operator=(const HID_Type &hid) { throw Error("ASSERT: Assign of RAII object"); }

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
    /// Disable copy construction
    HID_Space(const HID_Space &hid) { throw Error("ASSERT: Copy of RAII object"); }

    /// Disable copy assignment
    HID_Space & operator=(const HID_Space &hid) { throw Error("ASSERT: Assign of RAII object"); }

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
    /// Disable copy construction
    HID_Attr(const HID_Attr &hid) { throw Error("ASSERT: Copy of RAII object"); }

    /// Disable copy assignment
    HID_Attr & operator=(const HID_Attr &hid) { throw Error("ASSERT: Assign of RAII object"); }

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
    /// Disable copy construction
    HID_PList(const HID_PList &hid) { throw Error("ASSERT: Copy of RAII object"); }

    /// Disable copy assignment
    HID_PList & operator=(const HID_PList &hid) { throw Error("ASSERT: Assign of RAII object"); }

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
    /// Disable copy construction
    HID_Data(const HID_Data &hid) { throw Error("ASSERT: Copy of RAII object"); }

    /// Disable copy assignment
    HID_Data & operator=(const HID_Data &hid) { throw Error("ASSERT: Assign of RAII object"); }

  private:
    hid_t m_hID;
  };
}

#endif

