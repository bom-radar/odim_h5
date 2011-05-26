/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#include "product.h"

#include <cstring>

using namespace rainhdf;

product::product(const std::string& file, object_type type, time_t valid_time)
  : base(file, create)
{
  // Set the conventions value
  new_att(hnd_this_, atn_conventions, val_conventions);

  // Fill in the global 'what' group constants
  new_att(hnd_what_, atn_object, type);
  new_att(hnd_what_, atn_version, val_version);
  new_att(hnd_what_, atn_date, atn_time, valid_time);

  // Initialize source to be empty to ensure structural conformance
  new_att(hnd_what_, atn_source, "");
}

product::product(const std::string& file, object_type type, bool read_only)
  : base(file, read_only, open)
{
  // Check the object type
  if (get_att<object_type>(hnd_what_, atn_object) != type)
    throw error("ODIM_H5 object type mismatch");
}

void product::get_source(
      std::string& wmo
    , std::string& radar
    , std::string& orig_centre
    , std::string& place
    , std::string& country
    , std::string& comment) const
{
  // Get the raw attribute
  char buf[512];
  get_att(hnd_what_, atn_source, buf, 512);

  // Clear out the passed values
  wmo.clear();
  radar.clear();
  orig_centre.clear();
  place.clear();
  country.clear();
  comment.clear();

  // Tokenize it
  char *tok = buf;
  char *val = NULL;
  bool finished = false;
  for (int i = 0; !finished; ++i)
  {
    // Check for end of identifier
    if (buf[i] == ':')
    {
      if (val != NULL)
        throw error(hnd_what_, "Badly formed source attribute (pair mismatch)");

      buf[i] = '\0';
      val = &buf[i+1];
      continue;
    }

    // Check for end of string
    if (buf[i] == '\0')
      finished = true;

    // Check for end of value and process
    if (finished || buf[i] == ',')
    {
      buf[i] = '\0';

      if (val == NULL)
        throw error(hnd_what_, "Badly formed source attribute (pair mismatch)");
      else if (strcmp(tok, "WMO") == 0)
        wmo.assign(val);
      else if (strcmp(tok, "RAD") == 0)
        radar.assign(val);
      else if (strcmp(tok, "ORG") == 0)
        orig_centre.assign(val);
      else if (strcmp(tok, "PLC") == 0)
        place.assign(val);
      else if (strcmp(tok, "CTY") == 0)
        country.assign(val);
      else if (strcmp(tok, "CMT") == 0)
        comment.assign(val);
      else
        throw error(hnd_what_, "Badly formed source attribute (unknown identifier)");

      tok = &buf[i+1];
      val = NULL;
    }
  }
}

void product::set_source(
      const std::string& wmo
    , const std::string& radar
    , const std::string& orig_centre
    , const std::string& place
    , const std::string& country
    , const std::string& comment)
{
  std::string val;
  val.reserve(256);

  if (!wmo.empty())
    val.append(val.empty() ? "WMO:" : ",WMO:").append(wmo);
  if (!radar.empty())
    val.append(val.empty() ? "RAD:" : ",RAD:").append(radar);
  if (!orig_centre.empty())
    val.append(val.empty() ? "ORG:" : ",ORG:").append(orig_centre);
  if (!place.empty())
    val.append(val.empty() ? "PLC:" : ",PLC:").append(place);
  if (!country.empty())
    val.append(val.empty() ? "CTY:" : ",CTY:").append(country);
  if (!comment.empty())
    val.append(val.empty() ? "CMT:" : ",CMT:").append(country);

  set_att(hnd_what_, atn_source, val);
}


