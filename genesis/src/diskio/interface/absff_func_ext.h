/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: absff_func_ext.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: absff_func_ext.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.8  2000/06/12 05:07:02  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.7  1999/10/16 21:53:59  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.6  1999/10/16 21:25:06  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
 * changes into the distribution, I'm reverting to his version, then will
 * update his version to incorporate recent changes by Upi for netCDF 3.4.
 *
 * Revision 1.4  1998/01/15 02:26:16  venkat
 * Inclusion of the netcdf and FMT1 external functions header is not
 * hardcoded anymore. Taken care of thro the -I option in the Makefile.
 *
 * Revision 1.3  1998/01/14 01:04:17  venkat
 * Changes/additions to include FMT1 support and declaration of abstract
 * factories and class methods.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */
#ifndef FF_FUNC_EXT_H
#define FF_FUNC_EXT_H

#include "netcdf_func_ext.h"
#include "FMT1_func_ext.h"

/*Static abstract class IsA() method */
extern int			Absff_IsA();

/* Abstract Factory */
extern Abs_Diskio*		Absff_ConstructDiskio();

/* 
  This method is a class (static) method that delegates to isA() methods on
  subclasses to define a self-identification and a default mechanism 
  (fallback if an interface is being created without specifying the format)
  for the invocation of the appropriate concrete factory method and hence 
  the creation of the fileformat interface. 
*/

extern Abs_Diskio*		Absff_CreateDiskio_Interface();

#endif
