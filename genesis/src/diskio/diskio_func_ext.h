/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See interface/netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: diskio_func_ext.h,v 1.2 2005/06/17 17:37:06 svitak Exp $ */
/*
 * $Log: diskio_func_ext.h,v $
 * Revision 1.2  2005/06/17 17:37:06  svitak
 * Deprecated varargs.h replaced by stdarg.h. Old-style argument lists updated.
 * These changes were part of varargs-genesis.patch. Systems without stdarg.h
 * will no longer compile.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/06/12 05:07:01  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.6  1999/10/16 21:52:45  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.5  1999/10/16 21:23:47  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
 * changes into the distribution, I'm reverting to his version, then will
 * update his version to incorporate recent changes by Upi for netCDF 3.4.
 *
 * Revision 1.3  1998/01/13 20:48:09  venkat
 * Definitions of the field protection utility functions
 *
 * Revision 1.2  1997/07/26 00:12:00  venkat
 * Added
 * */

#ifndef DISKIO_FUNC_EXT_H
#define DISKIO_FUNC_EXT_H

 extern char * ParentDiskio_Interface(/* Element* */);
 extern char * ParentVariable_Interface(/* Element * */); 
 extern char * SelfVariable_Interface(/* Element * */);
 extern char * SelfMetadat_Interface(/* Element * */);

 extern int ffProtectMetadataFields(/* Element * */);
 extern int ffProtectVariableFields(/* Element * */);
 extern char * ffCreateName(/* char *genesis_name, int genesis_index */);
 extern char * ffRecreateName(/* char *nameinfile , int *index_val*/);

 extern long ffParseIndexedField(/*char *field */);
 extern long ffParseWhiteSpacedString(/*char *string, char **eachstring*/);

 extern void ffError(char *format, ...);
 extern void Expand_And_Copy_String(/* char**, char* */);

#endif
