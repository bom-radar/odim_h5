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
      std::is_same<handle::id_t, hid_t>::value
    , "handle::id_t does not match hid_t!");
static_assert(
      data::max_rank >= H5S_MAX_RANK
    , "data::max_rank is less than H5S_MAX_RANK");

static constexpr int default_version_major = 2;
static constexpr int default_version_minor = 1;
static constexpr const char* default_conventions = "ODIM_H5/V2_1";

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

static auto is_what_attribute(const char* name) -> bool
{
  auto beg = &what_names[0];
  auto end = &what_names[std::extent<decltype(what_names)>::value];
  auto mid = &what_names[std::extent<decltype(what_names)>::value / 2];
  while (beg <= end)
  {
    // TODO - may be opposite arg order required
    auto cmp = strcmp(name, *mid);
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

static auto is_where_attribute(const char* name) -> bool
{
  auto beg = &where_names[0];
  auto end = &where_names[std::extent<decltype(where_names)>::value];
  auto mid = &where_names[std::extent<decltype(where_names)>::value / 2];
  while (beg <= end)
  {
    // TODO - may be opposite arg order required
    auto cmp = strcmp(name, *mid);
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

static auto hdf_storage_type(data::data_type type) -> hid_t
{
  switch (type)
  {
  case data::data_type::i8:
    return H5T_STD_I8LE;
  case data::data_type::u8:
    return H5T_STD_U8LE;
  case data::data_type::i16:
    return H5T_STD_I16LE;
  case data::data_type::u16:
    return H5T_STD_U16LE;
  case data::data_type::i32:
    return H5T_STD_I32LE;
  case data::data_type::u32:
    return H5T_STD_U32LE;
  case data::data_type::i64:
    return H5T_STD_I64LE;
  case data::data_type::u64:
    return H5T_STD_U64LE;
  case data::data_type::f32:
    return H5T_IEEE_F32LE;
  case data::data_type::f64:
    return H5T_IEEE_F64LE;
  default:
    return -1;
  }
}

static auto strings_to_time(const std::string& date, const std::string& time) -> time_t
{
  struct tm tms;
  if (   sscanf(date.c_str(), "%04d%02d%02d", &tms.tm_year, &tms.tm_mon, &tms.tm_mday) != 3
      || sscanf(time.c_str(), "%02d%02d%02d", &tms.tm_hour, &tms.tm_min, &tms.tm_sec) != 3)
    throw make_error({}, "date/time syntax error");
  tms.tm_year -= 1900;
  tms.tm_mon -= 1;
  tms.tm_wday = 0;
  tms.tm_yday = 0;
  tms.tm_isdst = 0;
  return timegm(&tms);
}

static auto time_to_strings(time_t val, char date[9], char time[7]) -> void
{
  struct tm tms;
  gmtime_r(&val, &tms);
  snprintf(date, 9, "%04d%02d%02d", tms.tm_year + 1900, tms.tm_mon + 1, tms.tm_mday);
  snprintf(time, 7, "%02d%02d%02d", tms.tm_hour, tms.tm_min, tms.tm_sec);
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
  return {default_version_major, default_version_minor};
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

attribute::attribute(const handle* parent, std::string name, bool existing)
  : parent_{parent}
  , name_{std::move(name)}
  , type_{existing ? data_type::unknown : data_type::uninitialized}
  , size_{0}
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
    throw make_error(open(), "type mismatch", name_.c_str(), "boolean");
  return size_ == 5;
}

auto attribute::get_integer() const -> long
{
  auto hnd = open();
  if (type_ != data_type::integer)
    throw make_error(hnd, "type mismatch", name_.c_str(), "integer");
  long val;
  if (H5Aread(hnd, H5T_NATIVE_LONG, &val) < 0)
    throw make_error(hnd, "attribute read", name_.c_str(), "integer");
  return val;
}

auto attribute::get_real() const -> double
{
  auto hnd = open();
  if (type_ != data_type::real)
    throw make_error(hnd, "type mismatch", name_.c_str(), "real");
  double val;
  if (H5Aread(hnd, H5T_NATIVE_DOUBLE, &val) < 0)
    throw make_error(hnd, "attribute read", name_.c_str(), "real");
  return val;
}

auto attribute::get_string() const -> std::string
{
  handle type;

  auto hnd = open(&type);
  if (type_ != data_type::string)
    throw make_error(hnd, "type mismatch", name_.c_str(), "string");

  // use stack allocation for short strings
  if (size_ < 256)
  {
    char* buf = static_cast<char*>(alloca(size_));
    if (H5Aread(hnd, type, buf) < 0)
      throw make_error(hnd, "attribute read", name_.c_str(), "string");
    return {buf, size_ - 1};
  }
  else
  {
    std::unique_ptr<char[]> buf{new char[size_]};
    if (H5Aread(hnd, type, buf.get()) < 0)
      throw make_error(hnd, "attribute read", name_.c_str(), "string");
    return {buf.get(), size_ - 1};
  }
}

auto attribute::get_integer_array() const -> std::vector<long>
{
  auto hnd = open();
  if (type_ != data_type::integer_array)
    throw make_error(hnd, "type mismatch", name_.c_str(), "integer_array");
  std::vector<long> val(size_);
  if (H5Aread(hnd, H5T_NATIVE_LONG, &val[0]) < 0)
    throw make_error(hnd, "attribute read", name_.c_str(), "integer_array");
  return val;
}

auto attribute::get_real_array() const -> std::vector<double>
{
  auto hnd = open();
  if (type_ != data_type::integer_array)
    throw make_error(hnd, "type mismatch", name_.c_str(), "real_array");
  std::vector<double> val(size_);
  if (H5Aread(hnd, H5T_NATIVE_DOUBLE, &val[0]) < 0)
    throw make_error(hnd, "attribute read", name_.c_str(), "double_array");
  return val;
}

auto attribute::set(bool val) -> void
{
  handle type;
  auto hnd = open_or_create(data_type::boolean, val ? 5 : 6, &type);
  if (H5Awrite(hnd, type, val ? "True" : "False") < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "integer");
}

auto attribute::set(long val) -> void
{
  auto hnd = open_or_create(data_type::integer, 1);
  if (H5Awrite(hnd, H5T_NATIVE_LONG, &val) < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "integer");
}

auto attribute::set(double val) -> void
{
  auto hnd = open_or_create(data_type::real, 1);
  if (H5Awrite(hnd, H5T_NATIVE_DOUBLE, &val) < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "real");
}

auto attribute::set(const char* val) -> void
{
  handle type;
  auto hnd = open_or_create(data_type::string, strlen(val) + 1, &type);
  if (H5Awrite(hnd, type, val) < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "string");
}

auto attribute::set(const std::string& val) -> void
{
  handle type;
  auto hnd = open_or_create(data_type::string, val.size() + 1, &type);
  if (H5Awrite(hnd, type, val.c_str()) < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "string");
}

auto attribute::set(const std::vector<long>& val) -> void
{
  auto hnd = open_or_create(data_type::integer_array, val.size());
  if (H5Awrite(hnd, H5T_NATIVE_LONG, val.data()) < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "integer_array");
}

auto attribute::set(const std::vector<double>& val) -> void
{
  auto hnd = open_or_create(data_type::real_array, val.size());
  if (H5Awrite(hnd, H5T_NATIVE_DOUBLE, val.data()) < 0)
    throw make_error(hnd, "attribute write", name_.c_str(), "real_array");
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
    throw make_error(hnd, "get attribute space", name_.c_str());
  auto hsize = H5Sget_simple_extent_npoints(space);
  if (hsize < 0)
    throw make_error(hnd, "get attribute size", name_.c_str());
  size_ = hsize;

  // determine the type
  handle type{H5Aget_type(hnd)};
  if (!type)
    throw make_error(hnd, "get attribute type", name_.c_str());
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
        throw make_error(hnd, "read attribute", name_.c_str());
      if (strcmp(buf, "True") == 0 || strcmp(buf, "False") == 0)
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
      return open(type_out);

    // typemismatch - delete existing attribute
    if (type_ != data_type::uninitialized)
    {
      if (H5Adelete(*parent_, name_.c_str()) < 0)
        throw make_error(*parent_, "delete attribute", name_.c_str());
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

static inline auto group_checked_open_or_create(
      const handle& parent
    , const char* name
    , size_t index
    , bool open
    ) -> handle::id_t
{
  char buf[32];
  sprintf(buf, name, index + 1);
  auto ret = open 
    ? H5Gopen(parent, buf, H5P_DEFAULT) 
    : H5Gcreate(parent, buf, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (ret < 0)
    throw make_error(parent, "group open", buf);
  return ret;
}

attribute_store::attribute_store(handle::id_t hnd, bool existing)
  : hnd_{hnd}
{
  if (existing)
  {
    if (H5Lexists(hnd_, "what", H5P_DEFAULT) > 0)
      what_ = H5Gopen(hnd_, "what", H5P_DEFAULT);
    if (H5Lexists(hnd_, "where", H5P_DEFAULT) > 0)
      where_ = H5Gopen(hnd_, "where", H5P_DEFAULT);
    if (H5Lexists(hnd_, "how", H5P_DEFAULT) > 0)
      how_ = H5Gopen(hnd_, "how", H5P_DEFAULT);

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
}

attribute_store::attribute_store(
      const handle& parent
    , const char* name
    , size_t index
    , bool existing)
  : attribute_store{group_checked_open_or_create(parent, name, index, existing), existing}
{

}

attribute_store::attribute_store(const attribute_store& rhs)
  : hnd_{rhs.hnd_}
  , what_{rhs.what_}
  , where_{rhs.where_}
  , how_{rhs.how_}
  , attrs_(rhs.attrs_)
{
  fix_attribute_parents(rhs);
}

attribute_store::attribute_store(attribute_store&& rhs) noexcept
  : hnd_{std::move(rhs.hnd_)}
  , what_{std::move(rhs.what_)}
  , where_{std::move(rhs.where_)}
  , how_{std::move(rhs.how_)}
  , attrs_(std::move(rhs.attrs_))
{
  fix_attribute_parents(rhs);
}

auto attribute_store::operator=(const attribute_store& rhs) -> attribute_store&
{
  hnd_ = rhs.hnd_;
  what_ = rhs.what_;
  where_ = rhs.where_;
  how_ = rhs.how_;
  attrs_ = rhs.attrs_;
  fix_attribute_parents(rhs);
  return *this;
}

auto attribute_store::operator=(attribute_store&& rhs) noexcept -> attribute_store&
{
  hnd_ = std::move(rhs.hnd_);
  what_ = std::move(rhs.what_);
  where_ = std::move(rhs.where_);
  how_ = std::move(rhs.how_);
  attrs_ = std::move(rhs.attrs_);
  fix_attribute_parents(rhs);
  return *this;
}

auto attribute_store::fix_attribute_parents(const attribute_store& old) -> void
{
  // update the 'parent group' pointers for each attribute
  for (auto& a : attrs_)
  {
    if (a.parent_ == &old.what_)
      a.parent_ = &what_;
    else if (a.parent_ == &old.where_)
      a.parent_ = &where_;
    else
      a.parent_ = &how_;
  }
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

auto attribute_store::operator[](const char* name) -> attribute&
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
  return attrs_.back();
}

auto attribute_store::operator[](const char* name) const -> const attribute&
{
  for (auto& a : attrs_)
    if (a.name() == name)
      return a;
  throw make_error(hnd_, "no such attribute", name);
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

group::group(handle::id_t hnd, bool existing)
  : attribute_store{hnd, existing}
{

}

group::group(const handle& parent, const char* name, size_t index, bool existing)
  : attribute_store{parent, name, index, existing}
{

}

group::~group()
{

}

auto group::is_api_attribute(const std::string& name) const -> bool
{
  return false;
}

data::data(const handle& parent, bool quality, size_t index)
  : group{parent, quality ? "quality%zu" : "data%zu", index, true}
  , size_quality_{0}
  , data_{H5Dopen(hnd_, "data", H5P_DEFAULT)}
{
  if (!data_)
    throw make_error(hnd_, "open dataset", "data");

  // determine the number of dataX and qualityX layers
  H5G_info_t info;
  if (H5Gget_info(hnd_, &info) < 0)
    throw make_error(hnd_, "get group info");
  if (what_) --info.nlinks;
  if (where_) --info.nlinks;
  if (how_) --info.nlinks;
  for (size_t i = info.nlinks; i > 0; --i)
  {
    char name[32];
    sprintf(name, "quality%zu", i);
    htri_t ret = H5Lexists(hnd_, name, H5P_DEFAULT);
    if (ret < 0)
      throw make_error(hnd_, "check group exists", name);
    if (ret)
    {
      size_quality_ = i;
      break;
    }
  }
}

data::data(
      const handle& parent
    , bool quality
    , size_t index
    , data_type type
    , size_t rank
    , const size_t* dims
    , int compression)
  : group{parent, quality ? "quality%zu" : "data%zu", index, false}
  , size_quality_{0}
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
      || (   compression > 0
          && H5Pset_deflate(plist, compression) < 0))
    throw make_error(hnd_, "create dataset");
  data_ = H5Dcreate(hnd_, "data", hdf_storage_type(type), space, H5P_DEFAULT, plist, H5P_DEFAULT);
  if (!data_)
    throw make_error(hnd_, "create dataset");

  // if 2d, add the image attributes (for sake of hdfview)
  if (rank == 2)
  {
    attribute{&data_, "CLASS", false}.set("IMAGE");
    attribute{&data_, "IMAGE_VERSION", false}.set("1.2");
  }
}

auto data::quality_open(size_t i) const -> data
{
  return {hnd_, true, i};
}

auto data::quality_append(data_type type, size_t rank, const size_t* dims, int compression) -> data
{
  return {hnd_, true, size_quality_++, type, rank, dims, compression};
}

auto data::type() const -> data_type
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

auto data::rank() const -> size_t
{
  handle space{H5Dget_space(data_)};
  if (!space)
    throw make_error(hnd_, "get dataset rank");
  auto ret = H5Sget_simple_extent_ndims(space);
  if (ret < 0)
    throw make_error(hnd_, "get dataset rank");
  return ret;
}

auto data::dims(size_t* val) const -> size_t
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

auto data::size() const -> size_t
{
  handle space{H5Dget_space(data_)};
  if (!space)
    throw make_error(hnd_, "get dataset size");
  auto ret = H5Sget_simple_extent_npoints(space);
  if (ret < 0)
    throw make_error(hnd_, "get dataset size");
  return ret;
}

auto data::quantity() const -> std::string
{
  return attributes()["quantity"].get_string();
}

auto data::set_quantity(const std::string& val) -> void
{
  attributes()["quantity"].set(val);
}

auto data::gain() const -> double
{
  return attributes()["gain"].get_real();
}

auto data::set_gain(double val) -> void
{
  attributes()["gain"].set(val);
}

auto data::offset() const -> double
{
  return attributes()["offset"].get_real();
}

auto data::set_offset(double val) -> void
{
  attributes()["offset"].set(val);
}

auto data::nodata() const -> double
{
  return attributes()["nodata"].get_real();
}

auto data::set_nodata(double val) -> void
{
  attributes()["nodata"].set(val);
}

auto data::undetect() const -> double
{
  return attributes()["undetect"].get_real();
}

auto data::set_undetect(double val) -> void
{
  attributes()["undetect"].set(val);
}

auto data::is_api_attribute(const std::string& name) const -> bool
{
  return 
       name == "quantity"
    || name == "gain"
    || name == "offset"
    || name == "nodata"
    || name == "undetect"
    || group::is_api_attribute(name);
}

template <typename T>
auto data::read(T* data) const -> void
{
  auto err = H5Dread(data_, hdf_native_type<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  if (err < 0)
    throw make_error(hnd_, "read dataset", "data", err);
}

template auto data::read<char>(char* data) const -> void;
template auto data::read<signed char>(signed char* data) const -> void;
template auto data::read<unsigned char>(unsigned char* data) const -> void;
template auto data::read<short>(short* data) const -> void;
template auto data::read<unsigned short>(unsigned short* data) const -> void;
template auto data::read<int>(int* data) const -> void;
template auto data::read<unsigned int>(unsigned int* data) const -> void;
template auto data::read<long>(long* data) const -> void;
template auto data::read<unsigned long>(unsigned long* data) const -> void;
template auto data::read<long long>(long long* data) const -> void;
template auto data::read<unsigned long long>(unsigned long long* data) const -> void;
template auto data::read<float>(float* data) const -> void;
template auto data::read<double>(double* data) const -> void;
template auto data::read<long double>(long double* data) const -> void;

template <typename T>
auto data::write(const T* data) -> void
{
  auto err = H5Dwrite(data_, hdf_native_type<T>(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  if (err < 0)
    throw make_error(hnd_, "write dataset", "data", err);
}

template auto data::write<char>(const char* data) -> void;
template auto data::write<signed char>(const signed char* data) -> void;
template auto data::write<unsigned char>(const unsigned char* data) -> void;
template auto data::write<short>(const short* data) -> void;
template auto data::write<unsigned short>(const unsigned short* data) -> void;
template auto data::write<int>(const int* data) -> void;
template auto data::write<unsigned int>(const unsigned int* data) -> void;
template auto data::write<long>(const long* data) -> void;
template auto data::write<unsigned long>(const unsigned long* data) -> void;
template auto data::write<long long>(const long long* data) -> void;
template auto data::write<unsigned long long>(const unsigned long long* data) -> void;
template auto data::write<float>(const float* data) -> void;
template auto data::write<double>(const double* data) -> void;
template auto data::write<long double>(const long double* data) -> void;

dataset::dataset(const handle& parent, size_t index, bool existing)
  : group{parent, "dataset%zu", index, existing}
  , size_data_{0}
  , size_quality_{0}
{
  if (existing)
  {
    // determine the number of dataX and qualityX layers
    H5G_info_t info;
    if (H5Gget_info(hnd_, &info) < 0)
      throw make_error(hnd_, "get group info");
    if (what_) --info.nlinks;
    if (where_) --info.nlinks;
    if (how_) --info.nlinks;
    for (size_t i = info.nlinks; i > 0; --i)
    {
      char name[32];
      sprintf(name, "data%zu", i);
      htri_t ret = H5Lexists(hnd_, name, H5P_DEFAULT);
      if (ret < 0)
        throw make_error(hnd_, "check group exists", name);
      if (ret)
      {
        size_data_ = i;
        break;
      }
    }
    info.nlinks -= size_data_;
    for (size_t i = info.nlinks; i > 0; --i)
    {
      char name[32];
      sprintf(name, "quality%zu", i);
      htri_t ret = H5Lexists(hnd_, name, H5P_DEFAULT);
      if (ret < 0)
        throw make_error(hnd_, "check group exists", name);
      if (ret)
      {
        size_quality_ = i;
        break;
      }
    }
  }
}

auto dataset::data_open(size_t i) const -> data
{
  return {hnd_, false, i};
}

auto dataset::data_append(data::data_type type, size_t rank, const size_t* dims, int compression) -> data
{
  return {hnd_, false, size_data_++, type, rank, dims, compression};
}

auto dataset::quality_open(size_t i) const -> data
{
  return {hnd_, true, i};
}

auto dataset::quality_append(data::data_type type, size_t rank, const size_t* dims, int compression) -> data
{
  return {hnd_, true, size_quality_++, type, rank, dims, compression};
}

static inline auto file_checked_open_or_create(
      const char* path
    , file::io_mode mode
    ) -> handle::id_t
{
  auto ret = mode == file::io_mode::create
   ? H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)
   : H5Fopen(path, mode == file::io_mode::read_only ? H5F_ACC_RDONLY : H5F_ACC_RDWR, H5P_DEFAULT);
  if (ret < 0)
    throw make_error({}, "file open", path);
  return ret;
}

file::file(const std::string& path, io_mode mode)
  : group{file_checked_open_or_create(path.c_str(), mode), mode != io_mode::create}
  , mode_{mode}
  , type_{object_type::unknown}
  , size_{0}
{
  if (mode != io_mode::create)
  {
    // determine the number of datasetX groups
    H5G_info_t info;
    if (H5Gget_info(hnd_, &info) < 0)
      throw make_error(hnd_, "get group info");
    if (what_) --info.nlinks;
    if (where_) --info.nlinks;
    if (how_) --info.nlinks;
    for (size_t i = info.nlinks; i > 0; --i)
    {
      char name[32];
      sprintf(name, "dataset%zu", i);
      htri_t ret = H5Lexists(hnd_, name, H5P_DEFAULT);
      if (ret < 0)
        throw make_error(hnd_, "check group exists", name);
      if (ret)
      {
        size_ = i;
        break;
      }
    }

    // determine the object type
    auto str = attributes()["object"].get_string();
    if (str == "PVOL")
      type_ = object_type::polar_volume;
    else if (str == "CVOL")
      type_ = object_type::cartesian_volume;
    else if (str == "SCAN")
      type_ = object_type::polar_scan;
    else if (str == "RAY")
      type_ = object_type::polar_ray;
    else if (str == "AZIM")
      type_ = object_type::azimuthal_object;
    else if (str == "IMAGE")
      type_ = object_type::cartesian_image;
    else if (str == "COMP")
      type_ = object_type::composite_image;
    else if (str == "XSEC")
      type_ = object_type::vertical_cross_section;
    else if (str == "VP")
      type_ = object_type::vertical_profile;
    else if (str == "PIC")
      type_ = object_type::graphical_image;
  }
  else
  {
    // set the default conventions and version attributes
    set_conventions(default_conventions);
    set_version(default_version_major, default_version_minor);
  }
}

auto file::flush() -> void
{
  if (H5Fflush(hnd_, H5F_SCOPE_LOCAL) < 0) 
    throw make_error(hnd_, "flush");
}

template <class T>
auto file::dset_open_as(size_t i) const -> T
{
  return {hnd_, i, true};
}

template auto file::dset_open_as<scan>(size_t i) const -> scan;
template auto file::dset_open_as<profile>(size_t i) const -> profile;

template <class T>
auto file::dset_make_as() -> T
{
  return {hnd_, size_++, false};
}

template auto file::dset_make_as<scan>() -> scan;
template auto file::dset_make_as<profile>() -> profile;

auto file::conventions() const -> std::string
{
  return attribute{&hnd_, "Conventions", true}.get_string();
}

auto file::set_conventions(const std::string& val) -> void
{
  attribute{&hnd_, "Conventions", false}.set(val);
}

auto file::set_object(object_type type) -> void
{
  const char* val;
  switch (type)
  {
  case object_type::polar_volume:
    val = "PVOL";
    break;
  case object_type::cartesian_volume:
    val = "CVOL";
    break;
  case object_type::polar_scan:
    val = "SCAN";
    break;
  case object_type::polar_ray:
    val = "RAY";
    break;
  case object_type::azimuthal_object:
    val = "AZIM";
    break;
  case object_type::cartesian_image:
    val = "IMAGE";
    break;
  case object_type::composite_image:
    val = "COMP";
    break;
  case object_type::vertical_cross_section:
    val = "XSEC";
    break;
  case object_type::vertical_profile:
    val = "VP";
    break;
  case object_type::graphical_image:
    val = "PIC";
    break;
  default:
    val = "UNKNOWN";
    break;
  }
  attributes()["object"].set(val);
}

auto file::version() const -> std::pair<int, int>
{
  std::pair<int, int> ret;
  auto str = attributes()["version"].get_string();
  if (sscanf(str.c_str(), "H5rad %d.%d", &ret.first, &ret.second) != 2)
    throw make_error(hnd_, "read attribute", "version", "syntax error");
  return ret;
}

auto file::set_version(int major, int minor) -> void
{
  char buf[32];
  sprintf(buf, "H5rad %d.%d", major, minor);
  attributes()["version"].set(buf);
}

auto file::date() const -> std::string
{
  return attributes()["date"].get_string();
}

auto file::set_date(const std::string& val) -> void
{
  attributes()["date"].set(val);
}

auto file::time() const -> std::string
{
  return attributes()["time"].get_string();
}

auto file::set_time(const std::string& val) -> void
{
  attributes()["time"].set(val);
}

auto file::date_time() const -> time_t
{
  return strings_to_time(attributes()["date"].get_string(), attributes()["time"].get_string());
}

auto file::set_date_time(time_t val) -> void
{
  char date[9], time[7];
  time_to_strings(val, date, time);
  attributes()["date"].set(date);
  attributes()["time"].set(time);
}

auto file::source() const -> std::string
{
  return attributes()["source"].get_string();
}

auto file::set_source(const std::string& val) -> void
{
  attributes()["source"].set(val);
}

auto file::is_api_attribute(const std::string& name) const -> bool
{
  return 
       name == "object"
    || name == "version"
    || name == "date"
    || name == "time"
    || name == "source"
    || group::is_api_attribute(name);
}

//------------------------------------------------------------------------------

auto scan::elevation_angle() const -> double
{
  return attributes()["elangle"].get_real();
}

auto scan::set_elevation_angle(double val) -> void
{
  attributes()["elangle"].set(val);
}

auto scan::bin_count() const -> long
{
  return attributes()["nbins"].get_integer();
}

auto scan::set_bin_count(long val) -> void
{
  attributes()["nbins"].set(val);
}

auto scan::range_start() const -> double
{
  return attributes()["rstart"].get_real();
}

auto scan::set_range_start(double val) -> void
{
  attributes()["rstart"].set(val);
}

auto scan::range_scale() const -> double
{
  return attributes()["rscale"].get_real();
}

auto scan::set_range_scale(double val) -> void
{
  attributes()["rscale"].set(val);
}

auto scan::ray_count() const -> long
{
  return attributes()["nrays"].get_integer();
}

auto scan::set_ray_count(long val) -> void
{
  attributes()["nrays"].set(val);
}

auto scan::first_ray_radiated() const -> long
{
  return attributes()["a1gate"].get_integer();
}

auto scan::set_first_ray_radiated(long val) -> void
{
  attributes()["a1gate"].set(val);
}

auto scan::start_date() const -> std::string
{
  return attributes()["startdate"].get_string();
}

auto scan::set_start_date(const std::string& val) -> void
{
  attributes()["startdate"].set(val);
}

auto scan::start_time() const -> std::string
{
  return attributes()["starttime"].get_string();
}

auto scan::set_start_time(const std::string& val) -> void
{
  attributes()["starttime"].set(val);
}

auto scan::start_date_time() const -> time_t
{
  return strings_to_time(attributes()["startdate"].get_string(), attributes()["starttime"].get_string());
}

auto scan::set_start_date_time(time_t val) -> void
{
  char date[9], time[7];
  time_to_strings(val, date, time);
  attributes()["startdate"].set(date);
  attributes()["starttime"].set(time);
}

auto scan::end_date() const -> std::string
{
  return attributes()["enddate"].get_string();
}

auto scan::set_end_date(const std::string& val) -> void
{
  attributes()["enddate"].set(val);
}

auto scan::end_time() const -> std::string
{
  return attributes()["endtime"].get_string();
}

auto scan::set_end_time(const std::string& val) -> void
{
  attributes()["endtime"].set(val);
}

auto scan::end_date_time() const -> time_t
{
  return strings_to_time(attributes()["enddate"].get_string(), attributes()["endtime"].get_string());
}

auto scan::set_end_date_time(time_t val) -> void
{
  char date[9], time[7];
  time_to_strings(val, date, time);
  attributes()["enddate"].set(date);
  attributes()["endtime"].set(time);
}

auto scan::is_api_attribute(const std::string& name) const -> bool
{
  return 
       name == "elangle"
    || name == "nbins"
    || name == "rstart"
    || name == "rscale"
    || name == "nrays"
    || name == "a1gate"
    || name == "startdate"
    || name == "starttime"
    || name == "enddate"
    || name == "endtime"
    || dataset::is_api_attribute(name);
}

polar_volume::polar_volume(const std::string& path, io_mode mode)
  : file{path, mode}
{
  if (mode_ == io_mode::create)
    set_object(object_type::polar_volume);
  else if (type_ != object_type::polar_volume)
    throw make_error(hnd_, "unexpected object type", "polar_volume");
}

polar_volume::polar_volume(file f)
  : file{std::move(f)}
{
  if (mode_ == io_mode::create)
    set_object(object_type::polar_volume);
  else if (type_ != object_type::polar_volume)
    throw make_error(hnd_, "unexpected object type", "polar_volume");
}

auto polar_volume::longitude() const -> double
{
  return attributes()["lon"].get_real();
}

auto polar_volume::set_longitude(double val) -> void
{
  attributes()["lon"].set(val);
}

auto polar_volume::latitude() const -> double
{
  return attributes()["lat"].get_real();
}

auto polar_volume::set_latitude(double val) -> void
{
  attributes()["lat"].set(val);
}

auto polar_volume::height() const -> double
{
  return attributes()["height"].get_real();
}

auto polar_volume::set_height(double val) -> void
{
  attributes()["height"].set(val);
}

auto polar_volume::is_api_attribute(const std::string& name) const -> bool
{
  return 
       name == "lon"
    || name == "lat"
    || name == "height"
    || file::is_api_attribute(name);
}

vertical_profile::vertical_profile(const std::string& path, io_mode mode)
  : file{path, mode}
{
  if (mode_ == io_mode::create)
    set_object(object_type::vertical_profile);
  else if (type_ != object_type::vertical_profile)
    throw make_error(hnd_, "unexpected object type", "vertical_profile");
}

vertical_profile::vertical_profile(file f)
  : file{std::move(f)}
{
  if (mode_ == io_mode::create)
    set_object(object_type::vertical_profile);
  else if (type_ != object_type::vertical_profile)
    throw make_error(hnd_, "unexpected object type", "vertical_profile");
}

auto vertical_profile::longitude() const -> double
{
  return attributes()["lon"].get_real();
}

auto vertical_profile::set_longitude(double val) -> void
{
  attributes()["lon"].set(val);
}

auto vertical_profile::latitude() const -> double
{
  return attributes()["lat"].get_real();
}

auto vertical_profile::set_latitude(double val) -> void
{
  attributes()["lat"].set(val);
}

auto vertical_profile::height() const -> double
{
  return attributes()["height"].get_real();
}

auto vertical_profile::set_height(double val) -> void
{
  attributes()["height"].set(val);
}

auto vertical_profile::level_count() const -> long
{
  return attributes()["levels"].get_integer();
}

auto vertical_profile::set_level_count(long val) -> void
{
  attributes()["levels"].set(val);
}

auto vertical_profile::interval() const -> double
{
  return attributes()["interval"].get_real();
}

auto vertical_profile::set_interval(double val) -> void
{
  attributes()["interval"].set(val);
}

auto vertical_profile::min_height() const -> double
{
  return attributes()["minheight"].get_real();
}

auto vertical_profile::set_min_height(double val) -> void
{
  attributes()["minheight"].set(val);
}

auto vertical_profile::max_height() const -> double
{
  return attributes()["maxheight"].get_real();
}

auto vertical_profile::set_max_height(double val) -> void
{
  attributes()["maxheight"].set(val);
}

auto vertical_profile::is_api_attribute(const std::string& name) const -> bool
{
  return 
       name == "lon"
    || name == "lat"
    || name == "height"
    || name == "levels"
    || name == "interval"
    || name == "minheight"
    || name == "maxheight"
    || file::is_api_attribute(name);
}

auto profile::start_date() const -> std::string
{
  return attributes()["startdate"].get_string();
}

auto profile::set_start_date(const std::string& val) -> void
{
  attributes()["startdate"].set(val);
}

auto profile::start_time() const -> std::string
{
  return attributes()["starttime"].get_string();
}

auto profile::set_start_time(const std::string& val) -> void
{
  attributes()["starttime"].set(val);
}

auto profile::start_date_time() const -> time_t
{
  return strings_to_time(attributes()["startdate"].get_string(), attributes()["starttime"].get_string());
}

auto profile::set_start_date_time(time_t val) -> void
{
  char date[9], time[7];
  time_to_strings(val, date, time);
  attributes()["startdate"].set(date);
  attributes()["starttime"].set(time);
}

auto profile::end_date() const -> std::string
{
  return attributes()["enddate"].get_string();
}

auto profile::set_end_date(const std::string& val) -> void
{
  attributes()["enddate"].set(val);
}

auto profile::end_time() const -> std::string
{
  return attributes()["endtime"].get_string();
}

auto profile::set_end_time(const std::string& val) -> void
{
  attributes()["endtime"].set(val);
}

auto profile::end_date_time() const -> time_t
{
  return strings_to_time(attributes()["enddate"].get_string(), attributes()["endtime"].get_string());
}

auto profile::set_end_date_time(time_t val) -> void
{
  char date[9], time[7];
  time_to_strings(val, date, time);
  attributes()["enddate"].set(date);
  attributes()["endtime"].set(time);
}

auto profile::is_api_attribute(const std::string& name) const -> bool
{
  return 
       name == "startdate"
    || name == "starttime"
    || name == "enddate"
    || name == "endtime"
    || dataset::is_api_attribute(name);
}

