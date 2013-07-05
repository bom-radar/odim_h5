/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2013 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "rainhdf.h"

#include <hdf5.h>
#include <cstdio>

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

group::~group()
{

}

dataset::dataset(handle hnd)
  : group{std::move(hnd)}
{

}

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

