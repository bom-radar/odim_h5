/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "product.h"

#include <cstring>

using namespace rainhdf;

product::product(
      const std::string& file
    , object_type type
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

product::product(const std::string& file, object_type type, bool read_only)
  : base(file, read_only, open)
{
  // Check the object type
  if (get_att<object_type>(hnd_what_, atn_object) != type)
    throw error("ODIM_H5 object type mismatch");
}


