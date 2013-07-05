/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_RAINHDF_H
#define RAINHDF_RAINHDF_H

#include <stdexcept>
#include <string>
#include <utility>

namespace rainfields {
namespace hdf {

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

  /// Attribute data types
  enum class attribute_type
  {
      boolean
    , integer
    , real
    , string
    , integer_array
    , real_array
  };

  /// Storage type for data
  enum class data_type
  {
      i8
    , etc // TODO
  };

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

  /// Base class for ODIM_H5 objects with 'what', 'where' and 'how' attributes
  class group
  {
  public:
    virtual ~group();

    // TODO - do we put the attribute api in here, or do we make an attribute_store class?

  protected:
    group(handle hnd) : hnd_{std::move(hnd)} { }

  protected:
    handle      hnd_;
  };

  /// Dataset object
  class dataset : public group
  {
  public:
    auto data_type() const -> data_type;
    auto compression() const -> int;

    auto rank() const -> size_t;
    auto dimension(size_t dim) const -> size_t;
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

    auto read(int* data, bool unpack = false, int nodata = 0, int undetect = 0) const -> void;
    auto read(float* data, bool unpack = false, float nodata = 0.0f, float undetect = 0.0f) const -> void;
    auto read(double* data, bool unpack = false, double nodata = 0.0, double undetect = 0.0) const -> void;

    auto write(const int* data, bool pack = false, int nodata = 0, int undetect = 0) -> void;
    auto write(const float* data, bool pack = false, float nodata = 0.0f, float undetect = 0.0f) -> void;
    auto write(const double* data, bool pack = false, double nodata = 0.0, double undetect = 0.0) -> void;

  protected:
    dataset(handle hnd);

  private:
  };

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
}}

#endif
