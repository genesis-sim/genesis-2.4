/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: netcdf_func_ext.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: netcdf_func_ext.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 05:07:02  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.2  1999/10/16 22:47:26  mhucka
 * Merged changes from Upi for supporting netCDF 3.4.
 *
 * Revision 1.1  1999/10/16 21:17:08  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  This version of diskio by
 * Venkat implemented an abstract file interface and placed the netcdf_*
 * files into the subdirectory interface/netcdf/, instead of in
 * interface/ as in the version that I started with.  So I've moved the
 * files into interface/netcdf/ now.
 *
 * Revision 1.3  1998/01/14 18:34:01  venkat
 * Declaration of the isA() class method.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#ifndef NETCDF_FUNC_EXT_H
#define NETCDF_FUNC_EXT_H

/* static isA() method */
extern int Netcdf_IsA();

/*  Concrete Factory*/
extern Abs_Diskio*	Netcdf_ConstructFile();

#endif
