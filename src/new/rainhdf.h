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
    int id;

    handle() : id{-1} { }
    handle(int id) : id{id} { }
    handle(const handle& rhs);
    handle(handle&& rhs) : id(rhs.id) { rhs.id = -1; }
    auto operator=(const handle& rhs) -> handle&;
    auto operator=(handle&& rhs) -> handle& { std::swap(id, rhs.id); return *this; }
    ~handle();

    operator int() const { return id; }
    operator bool() const { return id > 0; }
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
    enum class attribute_type
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
    auto type() const -> attribute_type                         { return type_; }

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
    attribute(handle hnd, bool standard);
    attribute(handle grp, std::string name, bool standard);

  private:
    handle          hnd_;       // parent if uninitialized, else attribute itself
    std::string     name_;
    attribute_type  type_;
    bool            standard_;  // true if attribute has dedicated functions
    size_t          size_;      // number of elements in array or characters in string

    friend class group;
  };

  /// Interface to metadata attributes at a particular level
  class attribute_store
  {
  public:
    auto size() const -> size_t;

#if 0
    auto begin() const -> iterator; // etc
#endif

    auto operator[](const std::string& val) -> attribute&;
    auto operator[](const std::string& val) const -> const attribute&;

#if 0
    auto erase(iterator i) -> void;
#endif

  private:
  };

  /// Base class for ODIM_H5 objects with 'what', 'where' and 'how' attributes
  class group : private attribute_store
  {
  public:
    virtual ~group();

    auto attributes() -> attribute_store&                 { return *this; }
    auto attributes() const -> const attribute_store&     { return *this; }

  protected:
    group(handle hnd);

  protected:
    handle      hnd_;
    handle      what_;
    handle      where_;
    handle      how_;
  };

  /// Dataset object
  class dataset : public group
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

  public:
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
    dataset(handle hnd);
    dataset(handle hnd, data_type type, size_t rank, size_t* dims, int compression);

  protected:
    handle  data_;
  };

  template <typename T>
  auto dataset::read_unpack(T* data, T nodata, T undetect) const -> void
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
  auto dataset::write_pack(const T* data, NoDataTest is_nodata, UndetectTest is_undetect) -> void
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

  /// Polar volume file object
  class polar_volume : public group
  {
  public:
    
  protected:
    polar_volume(handle hnd) : group{std::move(hnd)} { }
    friend class file;
  };

  // TODO - all the other ODIM object types
  class cartesian_volume { };

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

    auto as_polar_volume() -> polar_volume;
    auto as_polar_volume() const -> polar_volume;

    auto as_cartesian_volume() -> cartesian_volume;
    auto as_cartesian_volume() const -> cartesian_volume;

    // get the objects of the various types

    auto flush() -> void;

  private:
    io_mode     mode_;
    object_type type_;
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
