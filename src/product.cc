/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainhdf)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "product.h"
#include <cstring>

using namespace rainfields::hdf;

product::product(
      const std::string& file
    , const char* type
    , const std::string& source
    , time_t valid_time)
  : base(file, create)
{
  // Set the conventions value
  new_att(hnd_this_, atn_conventions, val_conventions);

  // Fill in the global 'what' group constants
  check_create_what();
  new_att(hnd_what_, atn_object, type);
  new_att(hnd_what_, atn_version, val_version);
  new_att(hnd_what_, atn_source, source);
  new_att(hnd_what_, atn_date, atn_time, valid_time);
}

product::product(const std::string& file, const char* type, bool read_only)
  : base(file, read_only, open)
{
  // Check the object type
  char buf[32];
  get_att(hnd_what_, atn_object, buf, sizeof(buf));
  if (strncmp(buf, type, sizeof(buf)) != 0)
    throw error(hnd_what_, ft_bad_value, ht_attribute, atn_object);
}

void product::flush() const
{
  if (H5Fflush(hnd_this_, H5F_SCOPE_LOCAL) < 0)
    throw error(hnd_this_, ft_write, ht_file, "H5Fflush failed");
}

