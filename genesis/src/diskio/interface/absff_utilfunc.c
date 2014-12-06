/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: absff_utilfunc.c,v 1.2 2005/07/12 07:17:26 svitak Exp $ */
/*
 * $Log: absff_utilfunc.c,v $
 * Revision 1.2  2005/07/12 07:17:26  svitak
 * Removed malloc.h in favor of stdlib.h for portability.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.9  2001/04/18 22:39:36  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.8  2000/07/12 06:12:53  mhucka
 * Added #include of string.h.
 *
 * Revision 1.7  2000/06/12 04:38:39  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.6  1999/10/16 21:53:59  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.5  1999/10/16 21:25:06  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
 * changes into the distribution, I'm reverting to his version, then will
 * update his version to incorporate recent changes by Upi for netCDF 3.4.
 *
 * Revision 1.3  1998/01/14 18:22:23  venkat
 * Definition of generalized interface utility functions to allocate
 * default values of a certain abstract type, deallocate them and
 * copy a set of values to and from another chunk of allocated memory etc.
 * Also moved the set of netcdf-specific utility functions to
 * netcdf/netcdf_utilfunc.c where they rightfully belong.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "absff_ext.h"

void * Absff_AllocDefaultValues(type, size)
 Abs_Type type;
 long size;
{
	void *def_val;

        switch(type){
            /* Determine default value based on type and size*/

		case abs_uint8:
		case abs_uchar8:
			def_val = (unsigned char *) calloc((int)size, sizeof(unsigned char));
			break;

		case abs_int8:
		case abs_char8:

			def_val = (char *) calloc((int)size, sizeof(char));
			break;

		case abs_uint16:

			def_val = (unsigned short *) calloc((int)size, sizeof(unsigned short));
			break;

		case abs_int16:

			def_val = (short *) calloc((int)size, sizeof(short));
			break;

		case abs_uint32:
			def_val = (unsigned int *) calloc((int)size, sizeof(unsigned int));
			break;

		case abs_int32:
		
			def_val = (int *) calloc((int)size, sizeof(int));
			break;

		case abs_uint64:
			
			def_val = (unsigned long *) calloc((int)size, sizeof(unsigned long));
			break;
			
		case abs_int64:

			def_val = (long *) calloc((int)size, sizeof(long));
			break;

		case abs_float32:

			def_val = (float *) calloc((int)size, sizeof(float));
			break;

		case abs_float64:

			def_val = (double *) calloc((int)size, sizeof(double));
			break;

		case abs_string:

			def_val = (char **) calloc((int)size, sizeof(char*));
			break;

		default:
			/** Error: Invalid Type **/
			def_val = NULL;
			break;

        }
	return def_val;
}

void * Absff_ReallocDefaultValues(vals, type, size)
 void *vals;
 Abs_Type type;
 long size;
{
        switch(type){
            /* Determine default value based on type and size*/

		case abs_uint8:
		case abs_uchar8:
			vals = (unsigned char *) realloc(vals, size * sizeof(unsigned char));
			break;

		case abs_int8:
		case abs_char8:

			vals = (char *) realloc(vals, size*sizeof(char));
			break;

		case abs_uint16:
			vals = (unsigned short *) realloc(vals, size*sizeof(unsigned short));
			break;

		case abs_int16:

			vals = (short *) realloc(vals, size*sizeof(short));
			break;

		case abs_uint32:
			vals = (unsigned int *) realloc(vals, size*sizeof(unsigned int));
			break;

		case abs_int32:
		
			vals = (int *) realloc(vals, size*sizeof(int));
			break;
			
		case abs_int64:
			vals = (long *) realloc(vals, size*sizeof(long));
			break;

		case abs_uint64:

			vals = (unsigned long *) realloc(vals, size*sizeof(unsigned long));
			break;

		case abs_float32:

			vals = (float *) realloc(vals, size*sizeof(float));
			break;

		case abs_float64:

			vals = (double *) realloc(vals, size*sizeof(double));
			break;

		case abs_string:

			vals = (char **) realloc(vals, size*sizeof(char*));
			break;

		default:
			/** Error: Invalid Type **/
			break;

        }
	return vals;
}

int Absff_CopyFromValues(dup_vals, orig_vals, type, size, start_index, count)
 void *dup_vals, *orig_vals;
 Abs_Type type;
 long size, start_index, count;
{
 
 register int i,j;
 
 unsigned char *dup_uchar_vals, *orig_uchar_vals;
 char *dup_char_vals, *orig_char_vals;
 unsigned short *dup_ushort_vals, *orig_ushort_vals;
 short *dup_short_vals, *orig_short_vals;
 unsigned int *dup_uint_vals, *orig_uint_vals;
 int *dup_int_vals, *orig_int_vals;
 unsigned long *dup_ulong_vals, *orig_ulong_vals;
 long *dup_long_vals, *orig_long_vals;
 float *dup_float_vals, *orig_float_vals;
 double *dup_double_vals, *orig_double_vals;
 char **dup_string_vals, **orig_string_vals;
 
	
  if(dup_vals == NULL || orig_vals==NULL)
	return -1;

  if (count < 0 || count > size || start_index < 0 || start_index > (size -1) ){
	/** Error: Overflow **/
	return -1;
  }

  switch (type) {
	case abs_uchar8:
	case abs_uint8:

		dup_uchar_vals = (unsigned char *)dup_vals;
		orig_uchar_vals = (unsigned char *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_uchar_vals[j]=orig_uchar_vals[i];
		break;


	case abs_char8:
	case abs_int8:

		dup_char_vals = (char *)dup_vals;
		orig_char_vals = (char *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_char_vals[j]=orig_char_vals[i];
		break;

	case abs_uint16:

		dup_ushort_vals = (unsigned short *)dup_vals;
		orig_ushort_vals = (unsigned short *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_ushort_vals[j]=orig_ushort_vals[i];
		break;

	case abs_int16:

		dup_short_vals = (short *)dup_vals;
		orig_short_vals = (short *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_short_vals[j]=orig_short_vals[i];
		break;

	case abs_uint32:
		dup_uint_vals = (unsigned int *)dup_vals;
		orig_uint_vals = (unsigned int *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_uint_vals[j]=orig_uint_vals[i];
		break;

	case abs_int32:

		dup_int_vals = (int *)dup_vals;
		orig_int_vals = (int *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_int_vals[j]=orig_int_vals[i];
		break;

	case abs_uint64:
		dup_ulong_vals = (unsigned long *)dup_vals;
		orig_ulong_vals = (unsigned long *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_ulong_vals[j]=orig_ulong_vals[i];
		break;

	case abs_int64:

		dup_long_vals = (long *)dup_vals;
		orig_long_vals = (long *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_long_vals[j]=orig_long_vals[i];
		break;

	case abs_float32:

		dup_float_vals = (float *)dup_vals;
		orig_float_vals = (float *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_float_vals[j]=orig_float_vals[i];
		break;

	case abs_float64:

		dup_double_vals = (double *)dup_vals;
		orig_double_vals = (double *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_double_vals[j]=orig_double_vals[i];
		break;

	case abs_string:
	
		dup_string_vals = (char **)dup_vals;
		orig_string_vals = (char **) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			strcpy(dup_string_vals[j], orig_string_vals[i]);
		break;

	default:
		/** Error: Invalid Type **/
		return -1;
		/* NOTREACHED */
		break;
  }
  return 1;
}
 
int Absff_CopyToValues(dup_vals, orig_vals, type, size, start_index, count)
 void *dup_vals, *orig_vals;
 Abs_Type type;
 long size, start_index, count;
{
 
 register int i,j;
 
 unsigned char *dup_uchar_vals, *orig_uchar_vals;
 char *dup_char_vals, *orig_char_vals;
 unsigned short *dup_ushort_vals, *orig_ushort_vals;
 short *dup_short_vals, *orig_short_vals;
 unsigned int *dup_uint_vals, *orig_uint_vals;
 int *dup_int_vals, *orig_int_vals;
 unsigned long *dup_ulong_vals, *orig_ulong_vals;
 long *dup_long_vals, *orig_long_vals;
 float *dup_float_vals, *orig_float_vals;
 double *dup_double_vals, *orig_double_vals;
 char **dup_string_vals, **orig_string_vals;
 
	
  if(dup_vals == NULL || orig_vals==NULL)
	return -1;

  if (count < 0 || count > size || start_index < 0 || start_index > (size -1) ){
	/** Error: Overflow **/
	return -1;
  }

  switch (type) {
	case abs_uchar8:
	case abs_uint8:

		dup_uchar_vals = (unsigned char *)dup_vals;
		orig_uchar_vals = (unsigned char *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_uchar_vals[i]=orig_uchar_vals[j];
		break;


	case abs_char8:
	case abs_int8:

		dup_char_vals = (char *)dup_vals;
		orig_char_vals = (char *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_char_vals[i]=orig_char_vals[j];
		break;

	case abs_uint16:

		dup_ushort_vals = (unsigned short *)dup_vals;
		orig_ushort_vals = (unsigned short *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_ushort_vals[i]=orig_ushort_vals[j];
		break;

	case abs_int16:

		dup_short_vals = (short *)dup_vals;
		orig_short_vals = (short *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_short_vals[i]=orig_short_vals[j];
		break;

	case abs_uint32:
		dup_uint_vals = (unsigned int *)dup_vals;
		orig_uint_vals = (unsigned int *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_uint_vals[i]=orig_uint_vals[j];
		break;

	case abs_int32:

		dup_int_vals = (int *)dup_vals;
		orig_int_vals = (int *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_int_vals[i]=orig_int_vals[j];
		break;

	case abs_uint64:
		dup_ulong_vals = (unsigned long *)dup_vals;
		orig_ulong_vals = (unsigned long *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_ulong_vals[i]=orig_ulong_vals[j];
		break;

	case abs_int64:

		dup_long_vals = (long *)dup_vals;
		orig_long_vals = (long *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_long_vals[i]=orig_long_vals[j];
		break;

	case abs_float32:

		dup_float_vals = (float *)dup_vals;
		orig_float_vals = (float *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_float_vals[i]=orig_float_vals[j];
		break;

	case abs_float64:

		dup_double_vals = (double *)dup_vals;
		orig_double_vals = (double *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_double_vals[i]=orig_double_vals[j];
		break;

	case abs_string:
	
		dup_string_vals = (char **)dup_vals;
		orig_string_vals = (char **) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			strcpy(dup_string_vals[i], orig_string_vals[j]);
		break;

	default:
		/** Error: Invalid Type **/
		return -1;
		/* NOTREACHED */
		break;
  }
  return 1;
}

Abs_Varshape * Absff_ConstructDefaultShape(domainname)
 char *domainname;
{
  Abs_Varshape *shape;
 
        shape = (Abs_Varshape *) malloc(1*sizeof(Abs_Varshape));
        shape->ndomains = 1;
        shape->domainname = (char **) malloc(1*sizeof(char*));
        shape->domainname[0] = Absff_CopyString(domainname);
        shape->domaintype = (byte *) malloc(1*sizeof(byte));
        shape->domaintype[0] = DYNAMICSIZECOMPONENT;
        shape->domainsize = (long *) malloc(1*sizeof(long));
        shape->domainsize[0] = 0;

        return shape;
}
                                                                  
void Absff_DestructDefaultShape(pshape)
 Abs_Varshape **pshape;
{
        free((*pshape)->domaintype);
        free((*pshape)->domainsize);
        Absff_FreeString((*pshape)->domainname[0]);
        free((*pshape)->domainname);
        free(*pshape);
}



char* Absff_CopyString(str)
 char *str;
{
 char *ret;
 int len;

 if(str == NULL) return NULL;

 len = strlen(str)+1;
 ret = (char *) malloc(len*sizeof(char));
 strcpy(ret, str);
 return ret;
}

void Absff_FreeString(str)
 char *str;
{
 if(str == NULL) return;
 free(str);
}

float Absff_Atof(s)
 char *s;
{
 float f;
 if(sscanf(s, "%f", &f) > 0)
	return f;
 else
	return 0.0;
}

double Absff_Atod(s)
 char *s;
{
 double d;
 if(sscanf(s, "%lf", &d) > 0)
	return d;
 else
	return 0.0; 
}

long Absff_Atol(s)
 char *s;
{
 long l;
 if(sscanf(s, "%ld", &l) > 0)
	return l;
 else
	return 0.0;
}

