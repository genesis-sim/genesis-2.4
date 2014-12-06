/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See interface/netcdf-3.4/copyright.html for the full notice.

********************************************************************/

/* $Id: diskio_struct.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: diskio_struct.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/06/12 05:07:01  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.6  1999/10/16 21:52:46  mhucka
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
 * Revision 1.3  1998/01/13 20:51:13  venkat
 * Addition of members to the various structures to support FMT1
 *
 * Revision 1.2  1997/07/26 00:12:00  venkat
 * Added
 * */

#include "struct_defs.h"



/** 
 	###############################
	 The object structures 
	###############################
**/

struct Gen_Diskio_Type {
 ELEMENT_TYPE
 /* 
    ###########
	Fields to be shown with a showfield 
    ###########
 */
 
 /* Name of file to which interfaced */
 char *filename;

 /* Format of the file - FMT1, Netcdf etc., */
 char *fileformat;

 /* States of the file */
 short is_open;
 short is_writable;
 char accessmode;

 /* Hidden Abstract interface members */
 char *adi; 
 char *coordi;
 /* Hidden count of time step */
 long start_time_index[1];
 /* Hidden cache of time value */
 float currenttime;

 /* Backwards compatibility (with disk_out/disk_in) fields */

 float **val; /* Frame of values to be read from file on disk every time step*/
 char **autoSAVEavi; /* The frame of variable interface handles */ 
 int nx; 	    /* The number of such frames read every time step - Is 1 for FMT1 and
		     the same holds for diskio as well at this point */
 int ny;	    /* Number of values in a single frame = number of autoSAVE variable children*/	

 short valsallocated; /* flag- Are the frames allocated ? */
 short append; /* Should data be appended after resetting */

 char **autoSAVEamdi; /* Frame of 3-D Pos metadata interface handles */

 /**** The same fields as in disk_in *******/

 /* These hold the 3-D coords of the src element*/
 struct interpol_struct *xpts;
 struct interpol_struct *ypts;
 struct interpol_struct *zpts;

 float start_time;
 float dt;
 float time_offset;
 int datatype;
 int header_size;
 short leave_open; 
 short flush;

 /* Flag that helps disallow additions/deletions of messages after a RESET */
 short already_reset;
};

struct Gen_Variable_Type {
 ELEMENT_TYPE

 /* The number of values stored */
 int size;
 /* The datatype of the values stored */
 char *datatype;
 /* A specific value as string */
 char *value;
 
 /* input and output fields for messages and basic simulation functionality */
 /* Made hidden for FMT1 since variables cannot receive and send messages
    directly
 */
 double input;
 double output;

 /* Hidden Abstract interface members */
 char *avi;
 char *coordi;
 /* Hidden count of time step */
 long start_time_index[1];
};

struct Gen_Metadata_Type {
 ELEMENT_TYPE

 /* The number of attrib vals */
 int size;
 /* The datatype of attrib vals */
 char *datatype; 
 /* A value as string */
 char *value;

 /* Hidden Abstract interface member */
 char *amdi;
};
