/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: netcdf_utilfunc.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: netcdf_utilfunc.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:41:55  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 * 3) Some calls to nc_type_to_Abs_Type() should really have been calls to
 *    Abs_Type_To_nc_type().
 *
 * Revision 1.3  2000/05/29 02:12:56  mhucka
 * Moved the definition of nc_type_To_Abs_Type() up to before it was used,
 * to provide a definition and quiet the compiler.
 *
 * Revision 1.2  1999/10/16 22:47:26  mhucka
 * Merged changes from Upi for supporting netCDF 3.4.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include "netcdf_ext.h"

void *Netcdf_Convert_To_nc_type(vals, type, size)
 void *vals;
 Abs_Type type;
 int size;
{
 nc_type ntype;
 void *retvals;

 nclong *nclong_retvals;
 int *int_vals;
 int i, sz;

 if(vals == NULL) return NULL;

 ntype = Abs_Type_To_nc_type(type);
 sz = (size == 0)? 1: size; 

 switch (ntype){
	case NC_LONG:
		int_vals = (int *)vals;
		retvals = (nclong *) malloc(sz * sizeof(nclong));
		nclong_retvals = (nclong *)retvals;
		for(i=0; i<sz; i++)
			nclong_retvals[i] = int_vals[i];
		break;

	case NC_INVALID:
		retvals = NULL;
		break;
	
	default:
		retvals = vals;
		break;
 }
		
 return retvals;	
}

nc_type Abs_Type_To_nc_type(type)
 Abs_Type type;
{
 return netcdf_type[(int)type];
}  

Abs_Type nc_type_To_Abs_Type(ntype)
 nc_type ntype;
{
 switch (ntype){
	
	case NC_BYTE:
			return abs_int8;
			/* NOTREACHED */
			break;
	case NC_SHORT:
			return abs_int16;
			/* NOTREACHED */
			break;

	case NC_LONG:
			return abs_int32;
			/* NOTREACHED */
			break;
	
	case NC_FLOAT:
			return abs_float32;
			/* NOTREACHED */
			break;

	case NC_DOUBLE:
			return abs_float64;
			/* NOTREACHED */
			break;
	case NC_CHAR:
			return abs_char8;	
			/* NOTREACHED */
			break;
	default:

			return (Abs_Type)-1;
			/* NOTREACHED */
			break;
 }
}

void * Netcdf_AllocValues(type, size)
 Abs_Type type;
 int size;
{
 nc_type ntype;
 void *retval;

 ntype = Abs_Type_To_nc_type(type);
 retval = NULL;

 switch (ntype){

        case NC_BYTE:
                        retval = (char*) malloc(size * sizeof(char));
                        break;
        case NC_SHORT:
                        retval = (short*) malloc(size * sizeof(short));
                        break;

        case NC_LONG:
                        retval = (nclong*) malloc(size * sizeof(nclong));
 			break;

        case NC_FLOAT:
                        retval = (float*) malloc(size * sizeof(float));
                        break;

        case NC_DOUBLE:

                        retval = (double*) malloc(size * sizeof(double));
                        break;
        case NC_CHAR:

                        retval = (char*) malloc(size * sizeof(char));
                        break;
        default:

                        break;
 }

 return retval;

}

void * Netcdf_ReallocValues(val, type, size)
 void *val;
 Abs_Type type;
 int size;
{
 nc_type ntype;

 if(val == NULL) return val;
 ntype = Abs_Type_To_nc_type(type);

 switch (ntype){

        case NC_BYTE:
                        val = (char*) realloc(val, size * sizeof(char));
                        break;
        case NC_SHORT:
                        val = (short*) realloc(val, size * sizeof(short));
                        break;


	case NC_LONG:
                        val = (nclong*) realloc(val, size * sizeof(nclong));
                        break;

        case NC_FLOAT:
                        val = (float*) realloc(val, size * sizeof(float));
                        break;

        case NC_DOUBLE:

                        val = (double*) realloc(val, size * sizeof(double));
                        break;
        case NC_CHAR:

                        val = (char*) realloc(val, size * sizeof(char));
                        break;
        default:

                        break;
 }

 return val;

}
