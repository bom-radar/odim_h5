/*------------------------------------------------------------------------------
 * Rainfields ODIM HDF5 Library (rainHDF)
 *
 * Copyright (C) 2011 Commonwealth of Australia, Bureau of Meteorology
 * See COPYING for licensing and warranty details
 *----------------------------------------------------------------------------*/
#ifndef RAINHDF_RAINHDF_H
#define RAINHDF_RAINHDF_H

#include "volume.h"

/// Classes for reading and writing Rainfields products in ODIM_H5 compliant HDF5 format
/**
 *
 */
namespace RainHDF
{
  
}

/**
 * \mainpage RainHDF
 *
 * \section intro Introduction
 * Welcome to the Rainfields HDF (RainHDF) library documentation.
 *
 * The RainHDF library provides several C++ classes that can be used to read
 * and write files that conform to the OPERA Data Information Model for HDF5
 * (ODIM or ODIM_H5 for short).  As the standard is quite generic, allowing
 * a large range of products to be stored, only a subset has been implemented
 * within the library.
 *
 * The currently supported ODIM objects (file types) are:
 * - Polar Volume
 *
 * \section usage Usage
 * \subsection read Reading products
 * To open a product file, simply instanciate the class corresponding to the
 * type of product that you wish to read passing in both the path to the file
 * and a boolean indicating whether to open the file as read-only.
 * Once the file is opened you may use the methods of the product class to obtain
 * information about the product.
 *
 * \code
 * RainHDF::Volume vol("my_volume_file.h5", true);
 *
 * float lat = vol.latitude();
 * float lon = vol.longitude();
 * int nscan = vol.scan_count();
 *
 * cout << "volume at (" << lat << ", " << lon << ") contains " << nscan << " scans\n";
 * \endcode
 *
 * Most products contain collections of nested data.  For example a polar volume
 * is a collection of scans, and each scan contains any number of data layers
 * (DBZH, VRAD, WRAD etc).  Items within a collection are accessed via smart 
 * pointers that are returned by the containing object.  The following example
 * shows how to retrieve the reflectivity data of the 3rd elevation scan in
 * a polar volume file.
 *
 * \code
 * RainHDF::Volume vol("my_volume_file.h5", false);
 * if (vol.scan_count() < 3)
 * {
 *   cerr << "Insufficient scans" << endl;
 *   return;
 * }
 *
 * RainHDF::Scan::Ptr scan = vol.scan(2);
 * RainHDF::Data::Ptr dbz = scan.layer(kQty_DBZH);
 * if (dbz.get() == NULL)
 * {
 *   cerr << "No reflectivity data in file" << endl;
 *   return;
 * }
 *
 * float no_data, undetect;
 * std::vector<float> data(scan.azimuth_count() * scan.range_bin_count());
 *
 * dbz.read(&data[0], no_data, undetect);
 * // data, no_data and undetect now contain valid values
 * \endcode
 *
 * When a product is openened RainHDF does not read the entire product into
 * memory, but rather maintains handles to parts of the file via the
 * underlying HDF5 API.  To minimize the resources that are consumed at any 
 * one time, smart pointers are used to automatically close datasets that are
 * no longer being inspected.
 *
 * Upon calling Volume::scan, the HDF5 resources associated with
 * the scan are opened.  These resources remain open until the smart pointer is
 * destroyed.
 *
 * For this reason, it is important to:
 * - Open a resource once and reuse the returned smart pointer instead of 
 *   calling the 'Get' function multiple times.  Each call that returns a
 *   resource handle (smart pointer) involves querying the file via the
 *   underlying HDF5 API.
 * - Ensure that any returned smart pointers are destroyed as soon as
 *   possible.  Resources used in reading/writing the dataset remain
 *   open as long as the smart pointer handle exists.
 *
 * \subsection writing Writing Products
 * Writing products is much the same as reading them.  The appropriate product
 * class must be instianciated, however a number of product specific parameters
 * will be required in addition to the filename.
 *
 * An example of writing a single scan of reflectivity data to a polar volume
 * file is shown below:
 *
 * \code
 * const int bins = 512;
 * const int azimuths = 360;
 * float data[bins * azimuths];
 *
 * // ... data filled with radar image ...
 *
 * RainHDF::Volume vol(
 *     "my_volume_file.h5",
 *     time(NULL),          // Time data is valid
 *     123.4,               // Station latitude
 *     36.8,                // Station longitude
 *     10.0);               // Station height
 *      
 * RainHDF::Scan::Ptr scan = vol.add_scan(
 *     0.2,                 // elevation angle
 *     azimuths,            // no of azimuths
 *     bins,                // no of range bins
 *     0.1,                 // range of first bin
 *     1.0,                 // distance between bins
 *     time(NULL) - 30,     // scan start time
 *     time(NULL));         // scan end time
 *               
 * RainHDF::Data::Ptr layer = scan.add_layer(
 *     kQty_DBZH,           // quantity stored by this layer
 *     false,               // data or quality information (false == data)
 *     data,                // layer data to write
 *     0.0,                 // value that indicates no data
 *     0.0);                // value that indicates no detection
 * \endcode
 *
 * \subsection errors Error Handling
 * Whenever an error occurs while reading or writing a RainHDF product, an
 * exception is thrown of the class RainHDF::Error.  This error class extends
 * the standard std::exception error and can be caught as such.
 *
 * The library is written to be fully exception safe, and will not leak any 
 * resources as a result of an exception.  That is, it is safe for the application
 * using the library to recover from a RainHDF::Error exception and continue
 * executing.  In the case that an exception is thrown during a call which writes
 * to a HDF5 file, the state of the file is undefined.  The file will still
 * constitude valid HDF5, but may contain a partly written dataset or attribute.
 *
 * Example error handling code is shown below:
 *
 * \code
 * try
 * {
 *   // Attempt to open a non-existant scan
 *   RainHDF::Volume vol("my_volume_file.h5", true);
 *   RainHDF::Scan::ConstPtr scan = vol.scan(vol.scan_count());
 * }
 * catch (std::exception &err)
 * {
 *   cerr << "Read of volume failed with exception:\n"
 *        << err.what() << endl;
 * }
 * \endcode
 *
 */

#endif

