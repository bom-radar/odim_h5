/*------------------------------------------------------------------------------
 * ODIM (HDF5 format) Support Library
 *
 * Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef ODIM_H5_H
#define ODIM_H5_H

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace odim_h5
{
  /// Get the SCM release tag that the library was built from
  auto release_tag() -> char const*;

  /// Get the default ODIM_H5 conventions version used
  auto default_odim_version() -> std::pair<int, int>;

  // Internal - RAII object for managing an HDF5 API hid_t
  struct handle
  {
    typedef int id_t;

    id_t id;

    handle() noexcept : id{-1} { }
    handle(id_t id) noexcept : id{id} { }
    handle(const handle& rhs);
    handle(handle&& rhs) noexcept : id(rhs.id) { rhs.id = -1; }
    auto operator=(const handle& rhs) -> handle&;
    auto operator=(handle&& rhs) noexcept -> handle& { std::swap(id, rhs.id); return *this; }
    ~handle();

    operator id_t() const noexcept { return id; }
    operator bool() const noexcept { return id > 0; }

    auto close() -> void;
  };

  /// Exception thrown to indicate I/O errors
  class error : public std::runtime_error
  {
  public:
    error(const char* what);
  };

  /// Attribute handle
  class attribute
  {
  public:
    /// Attribute data types
    enum class data_type
    {
        uninitialized     ///< New attribute which has not been set yet
      , unknown           ///< Unknown data type
      , boolean           ///< Indicates bool
      , integer           ///< Indicates long
      , real              ///< Indicates double
      , string            ///< Indicates std::string
      , integer_array     ///< Indicates std::vector<long>
      , real_array        ///< Indicates std::vector<double>
    };

  public:
    /// Get attribute name
    auto name() const -> const std::string&                     { return name_; }

    /// Get data type of attribute
    auto type() const -> data_type;

    /// Get the attribute as a bool
    auto get_boolean() const -> bool;
    /// Get the attribute as a long
    auto get_integer() const -> long;
    /// Get the attribute as a double
    auto get_real() const -> double;
    /// Get the attribute as a string
    auto get_string() const -> std::string;
    /// Get the attribute as a vector of longs
    auto get_integer_array() const -> std::vector<long>;
    /// Get the attribute as a vector of doubles
    auto get_real_array() const -> std::vector<double>;

    /// Set the attribute
    auto set(bool val) -> void;
    /// Set the attribute
    auto set(long val) -> void;
    /// Set the attribute
    auto set(double val) -> void;
    /// Set the attribute
    auto set(const char* val) -> void;
    /// Set the attribute
    auto set(const std::string& val) -> void;
    /// Set the attribute
    auto set(const std::vector<long>& val) -> void;
    /// Set the attribute
    auto set(const std::vector<double>& val) -> void;

  private:
    attribute(const handle* parent, std::string name, bool existing);
    auto open(handle* type_out = nullptr) const -> handle;
    auto open_or_create(data_type type, size_t size, handle* type_out = nullptr) -> handle;

  private:
    const handle*     parent_;
    std::string       name_;
    mutable data_type type_;
    mutable size_t    size_;      // number of elements in array or characters in string

    friend class attribute_store;
    friend class data;
    friend class file;
  };

  /// Interface to metadata attributes at a particular level
  class attribute_store
  {
  private:
    typedef std::vector<attribute> store_impl;

  public:
    /// Iterator used for traversing the store
    typedef store_impl::iterator iterator;
    /// Constant iterator used for traversing the store
    typedef store_impl::const_iterator const_iterator;
    /// Iterator used for traversing the store in reverse
    typedef store_impl::reverse_iterator reverse_iterator;
    /// Constant iterator used for traversing the store in reverse
    typedef store_impl::const_reverse_iterator const_reverse_iterator;

  public:
    /// Get the number of attributes in the store
    auto size() const noexcept -> size_t                        { return attrs_.size(); }

    /// Get an iterator to the first attribute in the store
    auto begin() noexcept -> iterator                           { return attrs_.begin(); }
    /// Get an iterator to the first attribute in the store
    auto begin() const noexcept -> const_iterator               { return attrs_.begin(); }
    /// Get an iterator to the first attribute in the store
    auto cbegin() const noexcept -> const_iterator              { return attrs_.begin(); }
    /// Get an iterator to the first attribute in the store (reversed)
    auto rbegin() noexcept -> reverse_iterator                  { return attrs_.rbegin(); }
    /// Get an iterator to the first attribute in the store (reversed)
    auto rbegin() const noexcept -> const_reverse_iterator      { return attrs_.rbegin(); }
    /// Get an iterator to the first attribute in the store (reversed)
    auto crbegin() const noexcept -> const_reverse_iterator     { return attrs_.rbegin(); }

    /// Get an iterator referring to the past-the-end attribute in the store
    auto end() noexcept -> iterator                             { return attrs_.end(); }
    /// Get an iterator referring to the past-the-end attribute in the store
    auto end() const noexcept -> const_iterator                 { return attrs_.end(); }
    /// Get an iterator referring to the past-the-end attribute in the store
    auto cend() const noexcept -> const_iterator                { return attrs_.end(); }
    /// Get an iterator referring to the past-the-end attribute in the store (reversed)
    auto rend() noexcept -> reverse_iterator                    { return attrs_.rend(); }
    /// Get an iterator referring to the past-the-end attribute in the store (reversed)
    auto rend() const noexcept -> const_reverse_iterator        { return attrs_.rend(); }
    /// Get an iterator referring to the past-the-end attribute in the store (reversed)
    auto crend() const noexcept -> const_reverse_iterator       { return attrs_.rend(); }

    /// Find an attribute by name
    auto find(const char* name) noexcept -> iterator;
    /// Find an attribute by name
    auto find(const char* name) const noexcept -> const_iterator;
    /// Find an attribute by name
    auto find(const std::string& name) noexcept -> iterator;
    /// Find an attribute by name
    auto find(const std::string& name) const noexcept -> const_iterator;

    /// Get an attribute by name and create if not found
    auto operator[](const char* name) -> attribute&;
    /// Get an attribute by name and throw if not found
    auto operator[](const char* name) const -> const attribute&;
    /// Get an attribute by name and create if not found
    auto operator[](const std::string& name) -> attribute&      { return operator[](name.c_str()); }
    /// Get an attribute by name and throw if not found
    auto operator[](const std::string& name) const -> const attribute& { return operator[](name.c_str()); }

    /// Erase an attribute from the store
    auto erase(iterator i) -> void;
    /// Erase an attribute from the store
    auto erase(const std::string& name) -> void;

  protected:
    attribute_store(handle::id_t hnd, bool existing);
    attribute_store(const handle& parent, const char* name, size_t index, bool existing);

    attribute_store(const attribute_store& rhs);
    attribute_store(attribute_store&& rhs) noexcept;

    auto operator=(const attribute_store& rhs) -> attribute_store&;
    auto operator=(attribute_store&& rhs) noexcept -> attribute_store&;

    auto fix_attribute_parents(const attribute_store& old) -> void;

  protected:
    handle      hnd_;
    handle      what_;
    handle      where_;
    handle      how_;
    store_impl  attrs_;
  };

  /// Base class for ODIM_H5 objects with 'what', 'where' and 'how' attributes
  class group : protected attribute_store
  {
  public:
    virtual ~group();

    /// Get the attributes stored at this level
    auto attributes() -> attribute_store&                       { return *this; }
    /// Get the attributes stored at this level
    auto attributes() const -> const attribute_store&           { return *this; }

    /// Determine whether the named attribute is directly accessible through the object API
    virtual auto is_api_attribute(const std::string& name) const -> bool;

  protected:
    group(handle::id_t hnd, bool existing);
    group(const handle& parent, const char* name, size_t index, bool existing);
  };

  /// Dataset object
  class data : public group
  {
  public:
    /// Storage type for data
    enum class data_type
    {
        unknown   ///< Unsupported storage type
      , i8        ///< 8 bit signed integer
      , u8        ///< 8 bit unsigned integer
      , i16       ///< 16 bit signed integer
      , u16       ///< 16 bit unsigned integer
      , i32       ///< 32 bit signed integer
      , u32       ///< 32 bit unsigned integer
      , i64       ///< 64 bit signed integer
      , u64       ///< 64 bit unsigned integer
      , f32       ///< 32 bit float
      , f64       ///< 64 bit float
    };

    /// Maximum supported dataset rank
    constexpr static size_t max_rank = 32;

    /// Default compression level
    constexpr static int default_compression = 6;

  public:
    /// Get the number of quality layers
    auto quality_count() const -> size_t                        { return size_quality_; }
    /// Open a quality layer
    auto quality_open(size_t i) const -> data;
    /// Append a new quality layer
    auto quality_append(
          data_type type
        , size_t rank
        , const size_t* dims
        , int compression = default_compression
        ) -> data;

    /// Get the type used to store dataset in file
    auto type() const -> data_type;
    /// Get the number of dimensions used by the dataset
    auto rank() const -> size_t;
    /// Get the size of each dataset dimension
    auto dims(size_t* val) const -> size_t;
    /// Get the total number of points in the dataset
    auto size() const -> size_t;

    /// Get the quantity identifier
    auto quantity() const -> std::string;
    /// Set the quantity identifier
    auto set_quantity(const std::string& val) -> void;

    /// Get the gain used to unpack values
    auto gain() const -> double;
    /// Set the gain used to unpack values
    auto set_gain(double val) -> void;

    /// Get the offset used to unpack values
    auto offset() const -> double;
    /// Set the offset used to unpack values
    auto set_offset(double val) -> void;

    /// Get the packed value of the no data indicator
    auto nodata() const -> double;
    /// Set the packed value of the no data indicator
    auto set_nodata(double val) -> void;

    /// Get the packed value of the no detection indicator
    auto undetect() const -> double;
    /// Set the packed value of the no detection indicator
    auto set_undetect(double val) -> void;

    auto is_api_attribute(const std::string& name) const -> bool;

    // all the POD types except for bool and pointers are supported by read() and write()

    /// Read the dataset without unpacking
    template <typename T>
    auto read(T* data) const -> void;

    /// Unpack and read the dataset, replace nodata and undetect with user values
    template <typename T>
    auto read_unpack(T* data, T undetect, T nodata) const -> void;

    /// Write the dataset without packing
    template <typename T>
    auto write(const T* data) -> void;

    /// Pack and write the dataset, use passed functors to test for undetect and nodata
    template <typename T, class UndetectTest, class NoDataTest>
    auto write_pack(const T* data, UndetectTest is_undetect, NoDataTest is_nodata) -> void;

  protected:
    data(const handle& parent, bool quality, size_t index);
    data(
          const handle& parent
        , bool quality
        , size_t index
        , data_type type
        , size_t rank
        , const size_t* dims
        , int compression);

  protected:
    size_t  size_quality_;
    handle  data_;

    friend class dataset;
  };

  template <typename T>
  auto data::read_unpack(T* data, T undetect, T nodata) const -> void
  {
    read(data);

    const T nd = this->nodata();
    const T ud = this->undetect();
    const auto a = gain();
    const auto b = offset();
    const auto size = this->size();

    for (size_t i = 0; i < size; ++i)
    {
      // TODO - if T is a floating point, then the == operations below are unsafe
      //      - should introduce a tolerance, or if store type is int, read as int and convert to T
      if (data[i] == ud)
        data[i] = undetect;
      else if (data[i] == nd)
        data[i] = nodata;
      else
        data[i] = a * data[i] + b;
    }
  }

  template <typename T, class UndetectTest, class NoDataTest>
  auto data::write_pack(const T* data, UndetectTest is_undetect, NoDataTest is_nodata) -> void
  {
    const T nd = nodata();
    const T ud = undetect();
    const auto a = gain();
    const auto b = offset();
    const auto size = this->size();

    std::unique_ptr<T[]> buf{new T[size]};
    for (size_t i = 0; i < size; ++i)
    {
      if (is_undetect(data[i]))
        buf[i] = ud;
      else if (is_nodata(data[i]))
        buf[i] = nd;
      else
        buf[i] = (data[i] - b) / a;
    }

    write(buf.get());
  }

  /// Dataset group which contains data and optional quality layers
  class dataset : public group
  {
  public:
    /// Get the number of data layers
    auto data_count() const -> size_t                           { return size_data_; }
    /// Open a data layer
    auto data_open(size_t i) const -> data;
    /// Append a data layer
    auto data_append(
          data::data_type type
        , size_t rank
        , const size_t* dims
        , int compression = data::default_compression
        ) -> data;

    /// Get the number of quality layers
    auto quality_count() const -> size_t                        { return size_quality_; }
    /// Open a quality layer
    auto quality_open(size_t i) const -> data;
    /// Append a new quality layer
    auto quality_append(
          data::data_type type
        , size_t rank
        , const size_t* dims
        , int compression = data::default_compression
        ) -> data;

  protected:
    dataset(const handle& parent, size_t index, bool existing);

  protected:
    size_t  size_data_;
    size_t  size_quality_;

    friend class file;
  };

  /// Generic ODIM_H5 file
  class file : public group
  {
  public:
    /// I/O mode for opening an HDF5 file
    enum class io_mode
    {
        create
      , read_only
      , read_write
    };

    /// ODIM_H5 file scope object types
    enum class object_type
    {
        unknown
      , polar_volume
      , cartesian_volume
      , polar_scan
      , polar_ray
      , azimuthal_object
      , cartesian_image
      , composite_image
      , vertical_cross_section
      , vertical_profile
      , graphical_image
    };

  public:
    /// Open or create an ODIM_H5 file
    /**
     * \param path  Path of file to open
     * \param mode  Mode of open
     */
    file(const std::string& path, io_mode mode);

    /// Get the io_mode used to open the file
    auto mode() const noexcept -> io_mode                       { return mode_; }

    /// Ensure all write actions have been synced to disk
    auto flush() -> void;

    /// Get the number of datasets in the file
    auto dataset_count() const -> size_t                        { return size_; }
    /// Open a dataset
    auto dataset_open(size_t i) const -> dataset                { return dset_open_as<dataset>(i); }
    /// Append a new dataset
    auto dataset_append() -> dataset                            { return dset_make_as<dataset>(); }

    /// Get the ODIM_H5 conventions string
    auto conventions() const -> std::string;
    /// Set the ODIM_H5 conventions string
    auto set_conventions(const std::string& val) -> void;

    /// Get the file object type
    auto object() const -> object_type                          { return type_; }
    /// Set the file object type
    auto set_object(object_type type) -> void;

    /// Get the ODIM_H5 version number
    auto version() const -> std::pair<int, int>;
    /// Set the ODIM_H5 version number
    auto set_version(int major, int minor) -> void;

    /// Get the product date string
    auto date() const -> std::string;
    /// Set the product date string
    auto set_date(const std::string& val) -> void;

    /// Get the product time string
    auto time() const -> std::string;
    /// Set the product time string
    auto set_time(const std::string& val) -> void;

    /// Get the product date and time as a time_t
    auto date_time() const -> time_t;
    /// Set the product date and time using a time_t
    auto set_date_time(time_t val) -> void;

    /// Get the product source identifier string
    auto source() const -> std::string;
    /// Set the proudct source identifier string
    auto set_source(const std::string& val) -> void;

    auto is_api_attribute(const std::string& name) const -> bool;

  protected:
    template <class T> auto dset_open_as(size_t i) const -> T;
    template <class T> auto dset_make_as() -> T;

  protected:
    io_mode     mode_;
    object_type type_;
    size_t      size_;
  };

  //----------------------------------------------------------------------------
  // product specific APIs (wrappers around the above classes):

  /// Polar scan object (datasetX level)
  class scan : public dataset
  {
  public:
    /// Get the antenna elevation angle
    auto elevation_angle() const -> double;
    /// Set the antenna elevation angle
    auto set_elevation_angle(double val) -> void;

    /// Get the number of range bins in each ray
    auto bin_count() const -> long;
    /// Set the number of range bins in each ray
    auto set_bin_count(long val) -> void;

    /// Get the range of the start of the first range bin (km)
    auto range_start() const -> double;
    /// Set the range of the start of the first range bin (km)
    auto set_range_start(double val) -> void;

    /// Get the distance between sucessive range bins (m)
    auto range_scale() const -> double;
    /// Set the distance between sucessive range bins (m)
    auto set_range_scale(double val) -> void;

    /// Get the number of azimuth gates in the scan
    auto ray_count() const -> long;
    /// Set the number of azimuth gates in the scan
    auto set_ray_count(long val) -> void;

    /// Get the azimuthal offset of the CCW edge of the first ray from north (degrees)
    auto ray_start() const -> double;
    /// Set the azimuthal offset of the CCW edge of the first ray from north (degrees)
    auto set_ray_start(double val) -> void;

    /// Get the index of the first azimuth gate radiated
    auto first_ray_radiated() const -> long;
    /// Set the index of the first azimuth gate radiated
    auto set_first_ray_radiated(long val) -> void;

    /// Get the scan start date string
    auto start_date() const -> std::string;
    /// Set the scan start date string
    auto set_start_date(const std::string& val) -> void;

    /// Get the scan start time string
    auto start_time() const -> std::string;
    /// Set the scan start time string
    auto set_start_time(const std::string& val) -> void;

    /// Get the scan start date and time as a time_t
    auto start_date_time() const -> time_t;
    /// Set the scan start date and time using a time_t
    auto set_start_date_time(time_t val) -> void;

    /// Get the scan end date string
    auto end_date() const -> std::string;
    /// Set the scan end date string
    auto set_end_date(const std::string& val) -> void;

    /// Get the scan end time string
    auto end_time() const -> std::string;
    /// Set the scan end time string
    auto set_end_time(const std::string& val) -> void;

    /// Get the scan end date and time as a time_t
    auto end_date_time() const -> time_t;
    /// Set the scan end date and time using a time_t
    auto set_end_date_time(time_t val) -> void;

    auto is_api_attribute(const std::string& name) const -> bool;

  protected:
    scan(const handle& parent, size_t index, bool existing) : dataset(parent, index, existing) { }
    friend class file;
  };

  /// Polar volume ODIM_H5 file
  class polar_volume : public file
  {
  public:
    /// Open or create a polar volume ODIM_H5 file
    polar_volume(const std::string& path, io_mode mode);
    /// Cast an open ODIM_H5 file to a polar volume handle
    polar_volume(file f);

    /// Get the number of scans in the volume
    auto scan_count() const -> size_t                           { return dataset_count(); }
    /// Open a scan
    auto scan_open(size_t i) const -> scan                      { return dset_open_as<scan>(i); }
    /// Append a new scan
    auto scan_append() -> scan                                  { return dset_make_as<scan>(); }

    /// Get the longitude of the antenna
    auto longitude() const -> double;
    /// Set the longitude of the antenna
    auto set_longitude(double val) -> void;

    /// Get the latitude of the antenna
    auto latitude() const -> double;
    /// Set the latitude of the antenna
    auto set_latitude(double val) -> void;

    /// Get the height above sea level of the antenna center
    auto height() const -> double;
    /// Set the height above sea level of the antenna center
    auto set_height(double val) -> void;

    auto is_api_attribute(const std::string& name) const -> bool;
  };

  /// Vertical profile object (datasetX level)
  class profile : public dataset
  {
  public:
    /// Get the profile start date string
    auto start_date() const -> std::string;
    /// Set the profile start date string
    auto set_start_date(const std::string& val) -> void;

    /// Get the profile start time string
    auto start_time() const -> std::string;
    /// Set the profile start time string
    auto set_start_time(const std::string& val) -> void;

    /// Get the profile start date and time as a time_t
    auto start_date_time() const -> time_t;
    /// Set the profile start date and time using a time_t
    auto set_start_date_time(time_t val) -> void;

    /// Get the profile end date string
    auto end_date() const -> std::string;
    /// Set the profile end date string
    auto set_end_date(const std::string& val) -> void;

    /// Get the profile end time string
    auto end_time() const -> std::string;
    /// Set the profile end time string
    auto set_end_time(const std::string& val) -> void;

    /// Get the profile end date and time as a time_t
    auto end_date_time() const -> time_t;
    /// Set the profile end date and time using a time_t
    auto set_end_date_time(time_t val) -> void;

    auto is_api_attribute(const std::string& name) const -> bool;

  protected:
    profile(const handle& parent, size_t index, bool existing) : dataset(parent, index, existing) { }
    friend class file;
  };

  /// Vertical profile ODIM_H5 file
  class vertical_profile : public file
  {
  public:
    /// Open or create a polar volume ODIM_H5 file
    vertical_profile(const std::string& path, io_mode mode);
    /// Cast an open ODIM_H5 file to a polar volume handle
    vertical_profile(file f);

    /// Get the number of profiles in the volume
    auto profile_count() const -> size_t                        { return dataset_count(); }
    /// Open a profile
    auto profile_open(size_t i) const -> profile                { return dset_open_as<profile>(i); }
    /// Append a new profile
    auto profile_append() -> profile                            { return dset_make_as<profile>(); }

    /// Get the longitude of the antenna
    auto longitude() const -> double;
    /// Set the longitude of the antenna
    auto set_longitude(double val) -> void;

    /// Get the latitude of the antenna
    auto latitude() const -> double;
    /// Set the latitude of the antenna
    auto set_latitude(double val) -> void;

    /// Get the height above sea level of the antenna center
    auto height() const -> double;
    /// Set the height above sea level of the antenna center
    auto set_height(double val) -> void;

    /// Get the number of levels in the profile
    auto level_count() const -> long;
    /// Set the number of levels in the profile
    auto set_level_count(long val) -> void;

    /// Get the vertical distance between levels
    auto interval() const -> double;
    /// Set the vertical distance between levels
    auto set_interval(double val) -> void;

    /// Get the minimum height above sea level
    auto min_height() const -> double;
    /// Set the minimum height above sea level
    auto set_min_height(double val) -> void;

    /// Get the maximum height above sea level
    auto max_height() const -> double;
    /// Set the maximum height above sea level
    auto set_max_height(double val) -> void;

    auto is_api_attribute(const std::string& name) const -> bool;
  };

  /* efficient use of library:
   *
   * // best...
   * polar_volume vol("path.vol.h5", io_mode::read_only);
   *
   * // next best...
   * file f("path.vol.h5", io_mode::read_only);
   * if (f.object() == object_type::polar_volume)
   * {
   *   polar_volume vol{std::move(f)};
   *   ...
   * }
   *
   * // still okay (extra H5Iinc/decs)
   * file f("path.vol.h5", io_mode::read_only);
   * if (f.object() == object_type::polar_volume)
   * {
   *   polar_volume vol{f};
   *   do_something(static_cast<polar_volume>(f));
   * }
   */
}

#endif
