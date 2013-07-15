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

// these MUST remain in ASCII sorted order
static const char* what_names[] = 
{
    "date"
  , "enddate"
  , "endtime"
  , "gain"
  , "nodata"
  , "object"
  , "offset"
  , "prodpar"
  , "product"
  , "quantity"
  , "source"
  , "startdate"
  , "starttime"
  , "time"
  , "undetect"
  , "version"
};

static const char* where_names[] =
{
    "LL_lat"
  , "LL_lon"
  , "LR_lat"
  , "LR_lon"
  , "UL_lat"
  , "UL_lon"
  , "UR_lat"
  , "UR_lon"
  , "a1gate"
  , "angles"
  , "az_angle"
  , "elangle"
  , "height"
  , "interval"
  , "lat"
  , "levels"
  , "lon"
  , "maxheight"
  , "minheight"
  , "nbins"
  , "nrays"
  , "projdef"
  , "range"
  , "rscale"
  , "rstart"
  , "start_lat"
  , "start_lon"
  , "startaz"
  , "stop_lon"
  , "stop_lat"
  , "stopaz"
  , "xscale"
  , "xsize"
  , "yscale"
  , "ysize"
};

static auto is_what_attribute(const std::string& name) -> bool
{
  auto beg = &what_names[0];
  auto end = &what_names[std::extent<decltype(what_names)>::value];
  auto mid = &what_names[std::extent<decltype(what_names)>::value / 2];
  while (beg <= end)
  {
    auto cmp = name.compare(*mid);
    if (cmp == 0)
      return true;
    else if (cmp < 0)
      end = mid - 1;
    else
      beg = mid + 1;
    mid = beg + (end - beg) / 2;
  }
  return false;
}

static auto is_where_attribute(const std::string& name) -> bool
{
  auto beg = &where_names[0];
  auto end = &where_names[std::extent<decltype(where_names)>::value];
  auto mid = &where_names[std::extent<decltype(where_names)>::value / 2];
  while (beg <= end)
  {
    auto cmp = name.compare(*mid);
    if (cmp == 0)
      return true;
    else if (cmp < 0)
      end = mid - 1;
    else
      beg = mid + 1;
    mid = beg + (end - beg) / 2;
  }
  return false;
}

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

error::error(const char* what)
  : std::runtime_error{what}
{

}

attribute::attribute(handle* parent, std::string name, bool existing)
  : parent_(parent)
  , name_(std::move(name))
  , type_(existing ? data_type::unknown : data_type::uninitialized)
{

}

auto attribute::type() const -> data_type
{
  if (type_ == data_type::unknown)
    open();
  return type_;
}

auto attribute::get_boolean() const -> bool
{
  if (type_ == data_type::unknown)
    open();
  if (type_ != data_type::boolean)
    throw make_error(open(), "type mismatch", "boolean");
  return size_ == 5;
}

auto attribute::get_integer() const -> long
{
  auto hnd = open();
  if (type_ != data_type::integer)
    throw make_error(hnd, "type mismatch", "integer");
  long val;
  if (H5Aread(hnd, H5T_NATIVE_LONG, &val) < 0)
    throw make_error(hnd, "attribute read", "integer");
  return val;
}

auto attribute::get_real() const -> double
{
  auto hnd = open();
  if (type_ != data_type::real)
    throw make_error(hnd, "type mismatch", "real");
  double val;
  if (H5Aread(hnd, H5T_NATIVE_DOUBLE, &val) < 0)
    throw make_error(hnd, "attribute read", "real");
  return val;
}

auto attribute::get_string() const -> std::string
{
  handle type;

  auto hnd = open(&type);
  if (type_ != data_type::string)
    throw make_error(hnd, "type mismatch", "string");

  // use stack allocation for short strings
  if (size_ < 256)
  {
    char* buf = static_cast<char*>(alloca(size_));
    if (H5Aread(hnd, type, buf) < 0)
      throw make_error(hnd, "attribute read", "string");
    return {buf, size_};
  }
  else
  {
    std::unique_ptr<char[]> buf{new char[size_]};
    if (H5Aread(hnd, type, buf.get()) < 0)
      throw make_error(hnd, "attribute read", "string");
    return {buf.get(), size_};
  }
}

auto attribute::get_integer_array() const -> std::vector<long>
{
  auto hnd = open();
  if (type_ != data_type::integer_array)
    throw make_error(hnd, "type mismatch", "integer_array");
  std::vector<long> val(size_);
  if (H5Aread(hnd, H5T_NATIVE_LONG, &val[0]) < 0)
    throw make_error(hnd, "attribute read", "integer_array");
  return val;
}

auto attribute::get_real_array() const -> std::vector<double>
{
  auto hnd = open();
  if (type_ != data_type::integer_array)
    throw make_error(hnd, "type mismatch", "real_array");
  std::vector<double> val(size_);
  if (H5Aread(hnd, H5T_NATIVE_DOUBLE, &val[0]) < 0)
    throw make_error(hnd, "attribute read", "double_array");
  return val;
}

auto attribute::set(bool val) -> void
{
  handle type;
  auto hnd = open_or_create(data_type::boolean, val ? 5 : 6, &type);
  if (H5Awrite(hnd, type, val ? "True" : "False") < 0)
    throw make_error(hnd, "attribute write", "integer");
}

auto attribute::set(long val) -> void
{
  auto hnd = open_or_create(data_type::integer, 1);
  if (H5Awrite(hnd, H5T_NATIVE_LONG, &val) < 0)
    throw make_error(hnd, "attribute write", "integer");
}

auto attribute::set(double val) -> void
{
  auto hnd = open_or_create(data_type::real, 1);
  if (H5Awrite(hnd, H5T_NATIVE_DOUBLE, &val) < 0)
    throw make_error(hnd, "attribute write", "real");
}

auto attribute::set(const std::string& val) -> void
{
  handle type;
  auto hnd = open_or_create(data_type::string, val.size() + 1, &type);
  if (H5Awrite(hnd, type, &val) < 0)
    throw make_error(hnd, "attribute write", "string");
}

auto attribute::set(const std::vector<long>& val) -> void
{
  auto hnd = open_or_create(data_type::integer_array, val.size());
  if (H5Awrite(hnd, H5T_NATIVE_LONG, val.data()) < 0)
    throw make_error(hnd, "attribute write", "integer_array");
}

auto attribute::set(const std::vector<double>& val) -> void
{
  auto hnd = open_or_create(data_type::real_array, val.size());
  if (H5Awrite(hnd, H5T_NATIVE_DOUBLE, val.data()) < 0)
    throw make_error(hnd, "attribute write", "real_array");
}

// open an existing attribute
auto attribute::open(handle* type_out) const -> handle
{
  // attempt to open the attribute
  handle hnd{H5Aopen(*parent_, name_.c_str(), H5P_DEFAULT)};
  if (!hnd)
    throw make_error(*parent_, "attribute open", name_.c_str());

  // get the size (array elements)
  handle space{H5Aget_space(hnd)};
  if (!space)
    throw make_error(hnd, "get attribute space");
  auto hsize = H5Sget_simple_extent_npoints(space);
  if (hsize < 0)
    throw make_error(hnd, "get attribute size");
  size_ = hsize;

  // determine the type
  handle type{H5Aget_type(hnd)};
  if (!type)
    throw make_error(hnd, "get attribute type");
  switch (H5Tget_class(type))
  {
  case H5T_INTEGER:
    type_ = size_ > 1 ? data_type::integer_array : data_type::integer;
    break;
  case H5T_FLOAT:
    type_ = size_ > 1 ? data_type::real_array : data_type::real;
    break;
  case H5T_STRING:
    type_ = data_type::string;
    size_ = H5Tget_size(type);
    if (size_ == 5 || size_ == 6)
    {
      char buf[6];
      if (H5Aread(hnd, type, buf) < 0)
        throw make_error(hnd, "read attribute");
      if (strcmp(buf, "True") || strcmp(buf, "False"))
        type_ = data_type::boolean;
    }
    break;
  default:
    type_ = data_type::unknown;
  }

  // return the type to the caller if needed
  if (type_out)
    *type_out = std::move(type);

  return hnd;
}

auto attribute::open_or_create(data_type type, size_t size, handle* type_out) -> handle
{
  if (type_ != data_type::uninitialized)
  {
    // if type is a match, just open as normal
    if (type_ == type && size_ == size)
      return open();

    // typemismatch - delete existing attribute
    if (type_ != data_type::uninitialized)
    {
      if (H5Adelete(*parent_, name_.c_str()) < 0)
        throw make_error(*parent_, "delete attribute");
    }
  }

  type_ = type;
  size_ = size;

  // create the new attribute
  switch (type)
  {
  case data_type::boolean:
    {
      handle type{H5Tcopy(H5T_C_S1)};
      if (   !type
          || H5Tset_size(type, size) < 0
          || H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
        throw make_error(*parent_, "create attribute", name_.c_str());
      handle space{H5Screate(H5S_SCALAR)};
      handle hnd{H5Acreate(*parent_, name_.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT)};
      if (!hnd)
        throw make_error(*parent_, "create attribute", name_.c_str());
      if (type_out)
        *type_out = std::move(type);
      return hnd;
    }
  case data_type::integer:
    {
      handle space{H5Screate(H5S_SCALAR)};
      if (!space)
        throw make_error(*parent_, "create attribute", name_.c_str());
      handle hnd{H5Acreate(*parent_, name_.c_str(), H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT)};
      if (!hnd)
        throw make_error(*parent_, "create attribute", name_.c_str());
      return hnd;
    }
  case data_type::real:
    {
      handle space{H5Screate(H5S_SCALAR)};
      if (!space)
        throw make_error(*parent_, "create attribute", name_.c_str());
      handle hnd{H5Acreate(*parent_, name_.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT)};
      if (!hnd)
        throw make_error(*parent_, "create attribute", name_.c_str());
      return hnd;
    }
  case data_type::string:
    {
      handle type{H5Tcopy(H5T_C_S1)};
      if (   !type
          || H5Tset_size(type, size) < 0
          || H5Tset_strpad(type, H5T_STR_NULLTERM) < 0)
        throw make_error(*parent_, "create attribute", name_.c_str());
      handle space{H5Screate(H5S_SCALAR)};
      handle hnd{H5Acreate(*parent_, name_.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT)};
      if (!hnd)
        throw make_error(*parent_, "create attribute", name_.c_str());
      if (type_out)
        *type_out = std::move(type);
      return hnd;
    }
  case data_type::integer_array:
    {
      hsize_t dim = size;
      handle space{H5Screate_simple(1, &dim, nullptr)};
      if (!space)
        throw make_error(*parent_, "create attribute", name_.c_str());
      handle hnd{H5Acreate(*parent_, name_.c_str(), H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT)};
      if (!hnd)
        throw make_error(*parent_, "create attribute", name_.c_str());
      return hnd;
    }
  case data_type::real_array:
    {
      hsize_t dim = size;
      handle space{H5Screate_simple(1, &dim, nullptr)};
      if (!space)
        throw make_error(*parent_, "create attribute", name_.c_str());
      handle hnd{H5Acreate(*parent_, name_.c_str(), H5T_IEEE_F64LE, space, H5P_DEFAULT, H5P_DEFAULT)};
      if (!hnd)
        throw make_error(*parent_, "create attribute", name_.c_str());
      return hnd;
    }
  default:
    /* unreacable */
    throw make_error(*parent_, "create attribute", name_.c_str());
  }
}

attribute_store::attribute_store(handle hnd)
  : hnd_{std::move(hnd)}
  , what_{H5Gopen(hnd_, "what", H5P_DEFAULT)}
  , where_{H5Gopen(hnd_, "where", H5P_DEFAULT)}
  , how_{H5Gopen(hnd_, "how", H5P_DEFAULT)}
{
  hsize_t n = 0;
  H5O_info_t info;

  // determine the number of attributes available and reserve space in the vector
  if (what_ && H5Oget_info(what_, &info) >= 0)
    n += info.num_attrs;
  if (where_ && H5Oget_info(where_, &info) >= 0)
    n += info.num_attrs;
  if (how_ && H5Oget_info(how_, &info) >= 0)
    n += info.num_attrs;
  attrs_.reserve(n);

  // define operation needed to iterate through attribute
  struct op_data
  {
    attribute_store& store;
    handle* hnd;
  };
  op_data od{*this};
  auto op = [](hid_t loc, const char* name, const H5A_info_t* info, void* odata) -> herr_t
  {
    auto p = reinterpret_cast<op_data*>(odata);
    p->store.attrs_.push_back({p->hnd, name, true});
    return 0;
  };

  // iterate through each group to fetch the attribute names
  n = 0; od.hnd = &what_;
  if (what_ && H5Aiterate(what_, H5_INDEX_NAME, H5_ITER_NATIVE, &n, op, &od) < 0)
    throw make_error(hnd_, "iterate attributes", "what");
  n = 0; od.hnd = &where_;
  if (where_ && H5Aiterate(where_, H5_INDEX_NAME, H5_ITER_NATIVE, &n, op, &od) < 0)
    throw make_error(hnd_, "iterate attributes", "where");
  n = 0; od.hnd = &how_;
  if (how_ && H5Aiterate(how_, H5_INDEX_NAME, H5_ITER_NATIVE, &n, op, &od) < 0)
    throw make_error(hnd_, "iterate attributes", "how");
}

auto attribute_store::find(const std::string& name) noexcept -> iterator
{
  for (auto i = attrs_.begin(); i != attrs_.end(); ++i)
    if (i->name() == name)
      return i;
  return attrs_.end();
}

auto attribute_store::find(const std::string& name) const noexcept -> const_iterator
{
  for (auto i = attrs_.begin(); i != attrs_.end(); ++i)
    if (i->name() == name)
      return i;
  return attrs_.end();
}

auto attribute_store::operator[](const std::string& name) -> attribute&
{
  for (auto& a : attrs_)
    if (a.name() == name)
      return a;

  // okay, need to insert it
  if (is_what_attribute(name))
  {
    if (!what_)
    {
      what_ = handle{H5Gcreate(hnd_, "what", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)};
      if (!what_)
        throw make_error(hnd_, "create group", "what");
    }
    attrs_.push_back({&what_, name, false});
  }
  else if (is_where_attribute(name))
  {
    if (!where_)
    {
      where_ = handle{H5Gcreate(hnd_, "where", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)};
      if (!where_)
        throw make_error(hnd_, "create group", "where");
    }
    attrs_.push_back({&where_, name, false});
  }
  else
  {
    if (!how_)
    {
      how_ = handle{H5Gcreate(hnd_, "how", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)};
      if (!how_)
        throw make_error(hnd_, "create group", "how");
    }
    attrs_.push_back({&how_, name, false});
  }
}

auto attribute_store::operator[](const std::string& name) const -> const attribute&
{
  for (auto& a : attrs_)
    if (a.name() == name)
      return a;
  throw make_error(hnd_, "no such attribute", name.c_str());
}

auto attribute_store::erase(iterator i) -> void
{
  // remove the attribute from the file
  if (i->type_ != attribute::data_type::uninitialized)
  {
    if (H5Adelete(*i->parent_, i->name().c_str()) < 0)
      throw make_error(hnd_, "attribute delete", i->name().c_str());
  }
  
  // now remove it from the store
  attrs_.erase(i);
}

auto attribute_store::erase(const std::string& name) -> void
{
  for (auto i = attrs_.begin(); i != attrs_.end(); ++i)
  {
    if (i->name() == name)
    {
      erase(i);
      break;
    }
  }
}

group::group(handle hnd)
  : attribute_store{std::move(hnd)}
{

}

group::~group()
{

}

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

