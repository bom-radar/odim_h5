/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_RAINHDF_H
#define RAINHDF_RAINHDF_H

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rainfields {
namespace hdf {

  /// Get the name of the rainhdf library
  auto package_name() -> const char*;

  /// Get the version of the rainhdf library
  auto package_version() -> const char*;

  /// Get the support email for rainhdf library
  auto package_support() -> const char*;

  /// Get the default ODIM_H5 conventions version used
  auto default_version() -> std::pair<int, int>;

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
      , boolean
      , integer
      , real
      , string
      , integer_array
      , real_array
    };

  public:
    /// Get attribute name
    auto name() const -> const std::string&                     { return name_; }

    /// Get data type of attribute
    auto type() const -> data_type;

    /// Get whether the attribute is also exposed as a stand-alone function
    auto standard() const -> bool                               { return standard_; }

    auto get_boolean() const -> bool;
    auto get_integer() const -> long;
    auto get_real() const -> double;
    auto get_string() const -> std::string;
    auto get_integer_array() const -> std::vector<long>;
    auto get_real_array() const -> std::vector<double>;

    auto set(bool val) -> void;
    auto set(long val) -> void;
    auto set(double val) -> void;
    auto set(const std::string& val) -> void;
    auto set(const std::vector<long>& val) -> void;
    auto set(const std::vector<double>& val) -> void;

  private:
    attribute(handle* parent, std::string name, bool existing);
    auto open(handle* type_out = nullptr) const -> handle;
    auto open_or_create(data_type type, size_t size, handle* type_out = nullptr) -> handle;

  private:
    handle*           parent_;
    std::string       name_;
    mutable data_type type_;
    mutable size_t    size_;      // number of elements in array or characters in string

    bool              standard_;  // true if attribute has dedicated functions

    friend class attribute_store;
  };

  /// Interface to metadata attributes at a particular level
  class attribute_store
  {
  public:
    typedef std::vector<attribute> store_impl;
    typedef store_impl::iterator iterator;
    typedef store_impl::const_iterator const_iterator;
    typedef store_impl::reverse_iterator reverse_iterator;
    typedef store_impl::const_reverse_iterator const_reverse_iterator;

  public:
    auto size() const noexcept -> size_t                        { return attrs_.size(); }

    auto begin() noexcept -> iterator                           { return attrs_.begin(); }
    auto begin() const noexcept -> const_iterator               { return attrs_.begin(); }
    auto cbegin() const noexcept -> const_iterator              { return attrs_.begin(); }
    auto rbegin() noexcept -> reverse_iterator                  { return attrs_.rbegin(); }
    auto rbegin() const noexcept -> const_reverse_iterator      { return attrs_.rbegin(); }
    auto crbegin() const noexcept -> const_reverse_iterator     { return attrs_.rbegin(); }

    auto end() noexcept -> iterator                             { return attrs_.end(); }
    auto end() const noexcept -> const_iterator                 { return attrs_.end(); }
    auto cend() const noexcept -> const_iterator                { return attrs_.end(); }
    auto rend() noexcept -> reverse_iterator                    { return attrs_.rend(); }
    auto rend() const noexcept -> const_reverse_iterator        { return attrs_.rend(); }
    auto crend() const noexcept -> const_reverse_iterator       { return attrs_.rend(); }

    auto find(const std::string& name) noexcept -> iterator;
    auto find(const std::string& name) const noexcept -> const_iterator;

    auto operator[](const std::string& name) -> attribute&;
    auto operator[](const std::string& name) const -> const attribute&;

    auto erase(iterator i) -> void;
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

    auto attributes() -> attribute_store&                       { return *this; }
    auto attributes() const -> const attribute_store&           { return *this; }

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
        unknown
      , i8
      , u8
      , i16
      , u16
      , i32
      , u32
      , i64
      , u64
      , f32
      , f64
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
    /// Create a new quality layer
    auto quality_create(
          size_t rank
        , const size_t* dims
        , int compression = default_compression
        ) -> data;

    auto type() const -> data_type;
    auto rank() const -> size_t;
    auto dims(size_t* val) const -> size_t;
    auto size() const -> size_t;

    auto quantity() const -> std::string;
    auto set_quantity(const std::string& val) -> void;

    auto gain() const -> double;
    auto set_gain(double val) -> void;

    auto offset() const -> double;
    auto set_offset(double val) -> void;

    auto nodata() const -> double;
    auto set_nodata(double val) -> void;

    auto undetect() const -> double;
    auto set_undetect(double val) -> void;

    // all the POD types except for bool and pointers are supported
    template <typename T>
    auto read(T* data) const -> void;

    template <typename T>
    auto read_unpack(T* data, T nodata, T undetect) const -> void;

    template <typename T>
    auto write(const T* data) -> void;

    template <typename T, class NoDataTest, class UndetectTest>
    auto write_pack(const T* data, NoDataTest is_nodata, UndetectTest is_undetect) -> void;

  protected:
    data(const handle& parent, const char* name, size_t index);
    data(
          const handle& parent
        , const char* name
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
  auto data::read_unpack(T* data, T nodata, T undetect) const -> void
  {
    read(data);

    const T nd = nodata();
    const T ud = undetect();
    const auto a = gain();
    const auto b = offset();
    const auto size = size();

    for (size_t i = 0; i < size; ++i)
    {
      if (data[i] == nd)
        data[i] = nodata;
      else if (data[i] == ud)
        data[i] = undetect;
      else
        data[i] = a * data[i] + b;
    }
  }

  template <typename T, class NoDataTest, class UndetectTest>
  auto data::write_pack(const T* data, NoDataTest is_nodata, UndetectTest is_undetect) -> void
  {
    const T nd = nodata();
    const T ud = undetect();
    const auto a = gain();
    const auto b = offset();
    const auto size = size();

    std::unique_ptr<T[]> buf{new T[size]};
    for (size_t i = 0; i < size; ++i)
    {
      if (is_nodata(data[i]))
        buf[i] = nd;
      else if (is_undetect(data[i]))
        buf[i] = ud;
      else
        buf[i] = (data[i] - b) / a;
    }

    write(buf);
  }

  /// Dataset group which contains data and optional quality layers
  class dataset : public group
  {
  public:
    /// Get the number of data layers
    auto data_count() const -> size_t                           { return size_data_; }
    /// Open a data layer
    auto data_open(size_t i) const -> data;
    /// Create a data layer
    auto data_create(
          size_t rank
        , const size_t* dims
        , int compression = data::default_compression
        ) -> data;

    /// Get the number of quality layers
    auto quality_count() const -> size_t                        { return size_quality_; }
    /// Open a quality layer
    auto quality_open(size_t i) const -> data;
    /// Create a new quality layer
    auto quality_create(
          size_t rank
        , const size_t* dims
        , int compression = data::default_compression
        ) -> data;

  protected:
    dataset(const handle& parent, const char* name, size_t index, bool existing);

  protected:
    size_t  size_data_;
    size_t  size_quality_;

    friend class file;
  };

  /// Top level ODIM_H5 file manager
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

    /// Get the ODIM_H5 conventions string
    auto conventions() const -> const std::string&;
    auto set_conventions(const std::string& val) -> void;

    auto version() const -> std::pair<int, int>;
    auto set_version(int major, int minor) -> void;

    auto time() const -> time_t;
    auto set_time(time_t val) -> void;

    auto source() const -> std::string;
    auto set_source(const std::string& val) -> void;

    auto object() const -> object_type;

    /// Get the number of datasets in the file
    auto dataset_count() const -> size_t                        { return size_; }
    /// Open a dataset
    auto dataset_open(size_t i) const -> dataset;
    /// Create a new dataset
    auto dataset_create() -> dataset;

    /// Ensure all write actions have been synced to disk
    auto flush() -> void;

  private:
    io_mode     mode_;
    object_type type_;
    size_t      size_;
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
}}

#endif
