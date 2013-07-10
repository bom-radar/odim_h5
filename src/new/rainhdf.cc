/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "rainhdf.h"

#include <hdf5.h>
#include <alloca.h>
#include <cstdio>
#include <cstring>

using namespace rainfields::hdf;

/* To avoid our clients from having to include the HDF5 headers indirectly, we
 * redefine hid_t.  The following check ensures that our assumption about its
 * definition is correct.  If the definition of hid_t changes, then you must
 * also change the definition of handle::id_t.  In such a case, you will need to
 * add some smarts to the build system so that backwards compatibility is 
 * maintained for older versions of HDF5. */
static_assert(
      std::is_same<decltype(handle::id), hid_t>::value
    , "type of handle::id does not match hid_t!");
static_assert(
      dataset::max_rank >= H5S_MAX_RANK
    , "dataset::max_rank is less than H5S_MAX_RANK");

static auto make_error(
      const handle& hnd
    , const char* op
    , const char* param = nullptr
    , const char* err = nullptr
    ) -> error
{
  constexpr int len = 512;
  char msg[len];

  auto at = snprintf(msg, len, "rainhdf error: %s\n  operation: %s", err ? err : "", op);
  if (at < len && param)
    at += snprintf(msg + at, len - at, "\n  parameter: %s", param);
  if (at < len && hnd)
  {
    char loc[len];
    if (H5Iget_name(hnd, loc, 512) > 0)
    {
      loc[len - 1] = '\0';
      at += snprintf(msg + at, len - at, "\n   location: %s", loc);
    }
  }
  msg[len - 1] = '\0';
  return {msg};
}

static auto make_error(
      const handle& hnd
    , const char* op
    , const char* param
    , herr_t err
    ) -> error
{
  constexpr int len = 512;
  char buf[len];
  H5E_type_t type;

  if (H5Eget_msg(err, &type, buf, len) < 0)
    buf[0] = '\0';

  return make_error(hnd, op, param, buf);
}

template <class T> static auto hdf_native_type() -> hid_t;
template <> auto hdf_native_type<char>() -> hid_t               { return H5T_NATIVE_CHAR; }
template <> auto hdf_native_type<signed char>() -> hid_t        { return H5T_NATIVE_SCHAR; }
template <> auto hdf_native_type<unsigned char>() -> hid_t      { return H5T_NATIVE_UCHAR; }
template <> auto hdf_native_type<short>() -> hid_t              { return H5T_NATIVE_SHORT; }
template <> auto hdf_native_type<unsigned short>() -> hid_t     { return H5T_NATIVE_USHORT; }
template <> auto hdf_native_type<int>() -> hid_t                { return H5T_NATIVE_INT; }
template <> auto hdf_native_type<unsigned int>() -> hid_t       { return H5T_NATIVE_UINT; }
template <> auto hdf_native_type<long>() -> hid_t               { return H5T_NATIVE_LONG; }
template <> auto hdf_native_type<unsigned long>() -> hid_t      { return H5T_NATIVE_ULONG; }
template <> auto hdf_native_type<long long>() -> hid_t          { return H5T_NATIVE_LLONG; }
template <> auto hdf_native_type<unsigned long long>() -> hid_t { return H5T_NATIVE_ULLONG; }
template <> auto hdf_native_type<float>() -> hid_t              { return H5T_NATIVE_FLOAT; }
template <> auto hdf_native_type<double>() -> hid_t             { return H5T_NATIVE_DOUBLE; }
template <> auto hdf_native_type<long double>() -> hid_t        { return H5T_NATIVE_LDOUBLE; }

static auto hdf_storage_type(dataset::data_type type) -> hid_t
{
  switch (type)
  {
  case dataset::data_type::unknown:
    return -1;
  case dataset::data_type::i8:
    return H5T_STD_I8LE;
  case dataset::data_type::u8:
    return H5T_STD_U8LE;
  case dataset::data_type::i16:
    return H5T_STD_I16LE;
  case dataset::data_type::u16:
    return H5T_STD_U16LE;
  case dataset::data_type::i32:
    return H5T_STD_I32LE;
  case dataset::data_type::u32:
    return H5T_STD_U32LE;
  case dataset::data_type::i64:
    return H5T_STD_I64LE;
  case dataset::data_type::u64:
    return H5T_STD_U64LE;
  case dataset::data_type::f32:
    return H5T_IEEE_F32LE;
  case dataset::data_type::f64:
    return H5T_IEEE_F64LE;
  }
}

//------------------------------------------------------------------------------

auto rainfields::hdf::package_name() -> const char*
{
  return "rainhdf";
}

auto rainfields::hdf::package_version() -> const char*
{
  return "2.0.0";
}

auto rainfields::hdf::package_support() -> const char*
{
  return "m.curtis@bom.gov.au";
}

auto rainfields::hdf::default_version() -> std::pair<int, int>
{
  return {2, 1};
}

handle::handle(const handle& rhs)
  : id{rhs.id}
{
  if (id > 0)
    H5Iinc_ref(id);
}

auto handle::operator=(const handle& rhs) -> handle&
{
  if (id == rhs.id)
    return *this;
  if (id > 0)
    H5Idec_ref(id);
  id = rhs.id;
  H5Iinc_ref(id);
  return *this;
}

handle::~handle()
{
  if (id > 0)
    H5Idec_ref(id);
}

// open an existing attribute
attribute::attribute(handle hnd, bool standard)
  : hnd_{std::move(hnd)}
  , standard_{standard}
{
  char buf[512];

  // get the name
  auto len = H5Aget_name(hnd_, sizeof(buf), buf);
  if (len < 0 || len >= sizeof(buf))
    throw make_error(hnd_, "get attribute name");
  name_.assign(buf, len);

  // get the size (array elements)
  handle space{H5Aget_space(hnd_)};
  if (!space)
    throw make_error(hnd_, "get attribute space");
  auto hsize = H5Sget_simple_extent_npoints(space);
  if (hsize < 0)
    throw make_error(hnd_, "get attribute size");
  size_ = hsize;

  // determine the type
  handle type{H5Aget_type(hnd_)};
  if (!type)
    throw make_error(hnd_, "get attribute type");
  switch (H5Tget_class(type))
  {
  case H5T_INTEGER:
    type_ = size_ > 1 ? attribute_type::integer_array : attribute_type::integer;
    break;
  case H5T_FLOAT:
    type_ = size_ > 1 ? attribute_type::real_array : attribute_type::real;
    break;
  case H5T_STRING:
    type_ = attribute_type::string;
    size_ = H5Tget_size(type);
    if (size_ == 4 || size_ == 5)
    {
      if (H5Aread(hnd_, type, buf) < 0)
        throw make_error(hnd_, "read attribute");
      if (strcmp(buf, "True") || strcmp(buf, "False"))
        type_ = attribute_type::boolean;
    }
    break;
  default:
    type_ = attribute_type::unknown;
  }
}

// create an uninitialized attribute
attribute::attribute(handle grp, std::string name, bool standard)
  : hnd_{std::move(grp)}
  , name_{std::move(name)}
  , type_{attribute_type::uninitialized}
  , standard_{standard}
{

}

auto attribute::get_boolean() const -> bool
{
  if (type_ != attribute_type::boolean)
    throw make_error(hnd_, "type mismatch", "boolean");
  return size_ == 4;
}

auto attribute::get_integer() const -> long
{
  if (type_ != attribute_type::integer)
    throw make_error(hnd_, "type mismatch", "integer");
  long val;
  if (H5Aread(hnd_, H5T_NATIVE_LONG, &val) < 0)
    throw make_error(hnd_, "attribute read", "integer");
  return val;
}

auto attribute::get_real() const -> double
{
  if (type_ != attribute_type::real)
    throw make_error(hnd_, "type mismatch", "real");
  double val;
  if (H5Aread(hnd_, H5T_NATIVE_DOUBLE, &val) < 0)
    throw make_error(hnd_, "attribute read", "real");
  return val;
}

auto attribute::get_string() const -> std::string
{
  if (type_ != attribute_type::string)
    throw make_error(hnd_, "type mismatch", "string");

  // unfortunately we need to reacquire the type
  handle type{H5Aget_type(hnd_)};
  if (!type)
    throw make_error(hnd_, "get attribute type");

  // use stack allocation for short strings
  if (size_ < 256)
  {
    char* buf = static_cast<char*>(alloca(size_));
    if (H5Aread(hnd_, type, buf) < 0)
      throw make_error(hnd_, "attribute read", "string");
    return {buf, size_};
  }
  else
  {
    std::unique_ptr<char[]> buf{new char[size_]};
    if (H5Aread(hnd_, type, buf.get()) < 0)
      throw make_error(hnd_, "attribute read", "string");
    return {buf.get(), size_};
  }
}

auto attribute::get_integer_array() const -> std::vector<long>
{
  if (type_ != attribute_type::integer_array)
    throw make_error(hnd_, "type mismatch", "integer_array");
  std::vector<long> val(size_);
  if (H5Aread(hnd_, H5T_NATIVE_LONG, &val[0]) < 0)
    throw make_error(hnd_, "attribute read", "integer_array");
  return val;
}

auto attribute::get_real_array() const -> std::vector<double>
{
  if (type_ != attribute_type::integer_array)
    throw make_error(hnd_, "type mismatch", "real_array");
  std::vector<double> val(size_);
  if (H5Aread(hnd_, H5T_NATIVE_DOUBLE, &val[0]) < 0)
    throw make_error(hnd_, "attribute read", "double_array");
  return val;
}

auto attribute::set(bool val) -> void
{
  // TODO
}

auto attribute::set(long val) -> void
{
  /* for string and array, we also have to check matching size and resize if needed */
  if (type_ != attribute_type::integer)
  {
    if (type_ != attribute_type::uninitialized)
    {
      // delete old attribute
      // PROBLEM -> we don't have access to the parent group!

      type_ = attribute_type::uninitialized;
    }

    // create new attribute

    type_ = attribute_type::integer;
  }

  // write data
  if (H5Awrite(hnd_, H5T_NATIVE_LONG, &val) < 0)
    throw make_error(hnd_, "attribute write", "integer");
}

error::error(const char* what)
  : std::runtime_error{what}
{

}

group::group(handle hnd)
  : hnd_{std::move(hnd)}
  , what_{H5Gopen(hnd_, "what", H5P_DEFAULT)}
  , where_{H5Gopen(hnd_, "where", H5P_DEFAULT)}
  , how_{H5Gopen(hnd_, "how", H5P_DEFAULT)}
{

}

group::~group()
{

}

#if 0
auto group::attribute_count() const -> size_t
{
  if (!how_)
    return 0;
  H5O_info_t info;
  if (H5Oget_info(how_, &info) < 0)
    throw make_error(hnd_, "attribute count");
  return info.num_attrs;
}

auto group::attribute(size_t i) -> rainfields::hdf::attribute
{
  return rainfields::hdf::attribute{
    H5Aopen_by_idx(how_, ".", H5_INDEX_NAME, H5_ITER_NATIVE, i, H5P_DEFAULT, H5P_DEFAULT)};
}
#endif

dataset::dataset(handle hnd)
  : group{std::move(hnd)}
  , data_{H5Dopen(hnd_, "data", H5P_DEFAULT)}
{
  if (!data_)
    throw make_error(hnd_, "open dataset", "data");
}

dataset::dataset(handle hnd, data_type type, size_t rank, size_t* dims, int compression)
  : group{std::move(hnd)}
{
  // convert dimension array to hdf size type
  hsize_t hdims[max_rank];
  for (size_t i = 0; i < rank; ++i)
    hdims[i] = dims[i];

  // create the dataset
  handle space{H5Screate_simple(rank, hdims, hdims)};
  if (!space)
    throw make_error(hnd_, "create dataset");
  handle plist{H5Pcreate(H5P_DATASET_CREATE)};
  if (!plist)
    throw make_error(hnd_, "create dataset");
  if (   H5Pset_chunk(plist, rank, hdims) < 0
      || (   compression
          && H5Pset_deflate(plist, compression) < 0))
    throw make_error(hnd_, "create dataset");
  data_ = H5Dcreate(hnd_, "data", hdf_storage_type(type), space, H5P_DEFAULT, plist, H5P_DEFAULT);
  if (!data_)
    throw make_error(hnd_, "create dataset");

  // if 2d, add the image attributes (for sake of hdfview)
  if (rank == 2)
  {

  }
}

auto dataset::type() const -> data_type
{
  handle id{H5Dget_type(data_)};
  if (!id)
    throw make_error(hnd_, "get dataset type");

  auto type = H5Tget_class(id);
  auto size = H5Tget_size(id);

  if (type == H5T_INTEGER)
  {
    auto sign = H5Tget_sign(id) == H5T_SGN_2;
    switch (size)
    {
    case 1:
      return sign ? data_type::i8 : data_type::u8;
    case 2:
      return sign ? data_type::i16 : data_type::u16;
    case 4:
      return sign ? data_type::i32 : data_type::u32;
    case 8:
      return sign ? data_type::i64 : data_type::u64;
    }
  }
  else if (type == H5T_FLOAT)
  {
    switch (size)
    {
    case 4:
      return data_type::f32;
    case 8:
      return data_type::f64;
    }
  }

  return data_type::unknown;
}

auto dataset::rank() const -> size_t
{
  handle space{H5Dget_space(data_)};
  if (!space)
    throw make_error(hnd_, "get dataset rank");
  auto ret = H5Sget_simple_extent_ndims(space);
  if (ret < 0)
    throw make_error(hnd_, "get dataset rank");
  return ret;
}

auto dataset::dims(size_t* val) const -> size_t
{
  handle space{H5Dget_space(data_)};
  if (!space)
    throw make_error(hnd_, "get dataset dims");
  hsize_t dims[H5S_MAX_RANK];
  auto rank = H5Sget_simple_extent_dims(space, dims, nullptr);
  if (rank < 0)
    throw make_error(hnd_, "get dataset dims");
  for (decltype(rank) i = 0; i < rank; ++i)
    val[i] = dims[i];
  return rank;
}

auto dataset::size() const -> size_t
{
  handle space{H5Dget_space(data_)};
  if (!space)
    throw make_error(hnd_, "get dataset size");
  auto ret = H5Sget_simple_extent_npoints(space);
  if (ret < 0)
    throw make_error(hnd_, "get dataset size");
  return ret;
}

auto dataset::quantity() const -> std::string
{
  return attributes()["quantity"].get_string();
}

auto dataset::set_quantity(const std::string& val) -> void
{
  attributes()["quantity"].set(val);
}

template <typename T>
auto dataset::read(T* data) const -> void
{
  auto err = H5Dread(data_, hdf_native_type<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  if (err < 0)
    throw make_error(hnd_, "read dataset", "data", err);
}

template auto dataset::read<char>(char* data) const -> void;
template auto dataset::read<signed char>(signed char* data) const -> void;
template auto dataset::read<unsigned char>(unsigned char* data) const -> void;
template auto dataset::read<short>(short* data) const -> void;
template auto dataset::read<unsigned short>(unsigned short* data) const -> void;
template auto dataset::read<int>(int* data) const -> void;
template auto dataset::read<unsigned int>(unsigned int* data) const -> void;
template auto dataset::read<long>(long* data) const -> void;
template auto dataset::read<unsigned long>(unsigned long* data) const -> void;
template auto dataset::read<long long>(long long* data) const -> void;
template auto dataset::read<unsigned long long>(unsigned long long* data) const -> void;
template auto dataset::read<float>(float* data) const -> void;
template auto dataset::read<double>(double* data) const -> void;
template auto dataset::read<long double>(long double* data) const -> void;

template <typename T>
auto dataset::write(const T* data) -> void
{
  auto err = H5Dwrite(data_, hdf_native_type<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  if (err < 0)
    throw make_error(hnd_, "write dataset", "data", err);
}

template auto dataset::write<char>(const char* data) -> void;
template auto dataset::write<signed char>(const signed char* data) -> void;
template auto dataset::write<unsigned char>(const unsigned char* data) -> void;
template auto dataset::write<short>(const short* data) -> void;
template auto dataset::write<unsigned short>(const unsigned short* data) -> void;
template auto dataset::write<int>(const int* data) -> void;
template auto dataset::write<unsigned int>(const unsigned int* data) -> void;
template auto dataset::write<long>(const long* data) -> void;
template auto dataset::write<unsigned long>(const unsigned long* data) -> void;
template auto dataset::write<long long>(const long long* data) -> void;
template auto dataset::write<unsigned long long>(const unsigned long long* data) -> void;
template auto dataset::write<float>(const float* data) -> void;
template auto dataset::write<double>(const double* data) -> void;
template auto dataset::write<long double>(const long double* data) -> void;

file::file(const std::string& path, io_mode mode)
  : group {
    mode == io_mode::create 
    ? H5Fcreate(path.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)
    : H5Fopen(path.c_str(), mode == io_mode::read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR, H5P_DEFAULT)}
  , mode_{mode}
  , type_{object_type::unknown}
{
  if (!hnd_)
    throw make_error({}, mode == io_mode::create ? "create file" : "open file", path.c_str());
}

auto file::as_polar_volume() -> polar_volume
{
  if (   type_ == object_type::polar_volume
      || (   type_ == object_type::unknown
          && mode_ == io_mode::create))
    return {hnd_};
  throw make_error(hnd_, "open object", "polar_volume", "type mismatch");
}

auto file::as_polar_volume() const -> polar_volume
{
  if (type_ == object_type::polar_volume)
    return {hnd_};
  throw make_error(hnd_, "open object", "polar_volume", "type mismatch");
}

auto file::flush() -> void
{
  if (H5Fflush(hnd_, H5F_SCOPE_LOCAL) < 0) 
    throw make_error(hnd_, "flush");
}

