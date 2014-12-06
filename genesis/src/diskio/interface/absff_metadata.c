/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: absff_metadata.c,v 1.2 2005/07/12 06:11:06 svitak Exp $ */
/*
 * $Log: absff_metadata.c,v $
 * Revision 1.2  2005/07/12 06:11:06  svitak
 * Removed malloc.h since it's included from stdlib.h. Added stdio before
 * stdlib if needed since some OSs require it.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.8  2000/07/12 06:12:33  mhucka
 * Added #include of string.h and malloc.h.
 *
 * Revision 1.7  2000/06/12 05:04:25  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added missing typecast in call to Absff_SetMetadataValues().
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
 * Revision 1.3  1998/01/14 18:14:25  venkat
 * Replaced usage of no longer defined typdefs (uchar, ushort etc.,) with
 * the appropriate type names. Also replaced old metadata-specific
 * function calls with the new generalized equivalents (Absff_CopyToValues(),
 * Absff_AllocDefaultValues() etc.) defined in absff_utilfunc.c.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>

#include "absff_ext.h"


/** Errors: -1 for library error or NULL params, non-negative for success **/


 
 

int Absff_DestructMetadata(amdi)
 Abs_Metadata *amdi;
{
 if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

 if(amdi->vals != NULL) {
	free(amdi->vals);
	amdi->vals = NULL;
 }

 return 1;
}
 
int Absff_CreateMetadata(amdi, type, size)
 Abs_Metadata *amdi;
 Abs_Type type;
 long size;
{
 if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

 if(amdi->vals == NULL)
	amdi->vals = Absff_AllocDefaultValues(type, size); 

 if(amdi->vals == NULL) return -1;
 amdi->abstype = type;
 amdi->size = size; 
 return 1;
}

int Absff_SetMetadataType(amdi, type)
 Abs_Metadata *amdi;
 Abs_Type type;
{
 Abs_Diskio *adi;
 if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

 adi = (struct Abs_Diskio_Type *) amdi->adi;

  if(!adi->is_writable){
		/** Error: Not writable **/
		return -1;
  }
  
  if(amdi->vals != NULL){
	free(amdi->vals);
	amdi->vals = NULL;
  }
  amdi->vals = Absff_AllocDefaultValues(type, amdi->size);
  amdi->abstype = type;
  return 1;
}

int Absff_SetMetadataSize(amdi, size)
 Abs_Metadata *amdi;
 long size;
{
 Abs_Type type;
 long 	  old_size;
 Abs_Diskio *adi;

  if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

  adi = (struct Abs_Diskio_Type *) amdi->adi;

  if(!adi->is_writable){
		/** Error: Not writable **/
		return -1;
  }
  

  old_size =  amdi->size;
  type =  amdi->abstype;

  if(amdi->vals == NULL)
	amdi->vals = Absff_AllocDefaultValues(type, size);
  else{
	if(size > old_size)
		 amdi->vals = Absff_ReallocDefaultValues(amdi->vals, type, size);
  }

  amdi->size = size;

  return 1;
}

int Absff_SetMetadataValues(amdi, vals, start_index, count)
 Abs_Metadata *amdi;
 void *vals;
 long start_index;
 long count; 
{ 

 Abs_Type type;
 long 	  size;
 Abs_Diskio *adi;

 if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

  adi = amdi->adi; 

  if(!adi->is_writable){
		/** Error: Not writable **/
		return -1;
  }
  

  size =  amdi->size;
  type =  amdi->abstype;

  if(count < 0 || count > size || start_index < 0 || start_index > (size -1) ){
	/** Error: Invalid start_index or size **/
	 return -1;
  }

  if(amdi->vals == NULL)
	amdi->vals = Absff_AllocDefaultValues(type, size);
  if(vals != amdi->vals)
  	Absff_CopyToValues(amdi->vals, vals, type, size, start_index, count);

  return 1;
}

int Absff_SetMetadataValuesFromString(amdi, vals, start_index, count)
 Abs_Metadata *amdi;
 char **vals;
 long start_index, count;
{
 Abs_Type type;
 Abs_Diskio *adi;
 int i, ret; 
 long size;
 void *values;

 unsigned char *uchar_values; 
 char *char_values;
 unsigned short  *ushort_values;
 short *short_values;
 unsigned int *uint_values;
 int *int_values;
 unsigned long *ulong_values; 
 long *long_values;
 float *float_values;
 double *double_values;
 
 
 if(amdi == NULL ||
	amdi->adi == NULL || vals == NULL) 
	return -1;

  adi = amdi->adi;
  if(!adi->is_writable){
		/** Error: Not writable **/
		return -1;
  }
  

  size =  amdi->size;
  type =  amdi->abstype;

  if(count < 0 || count > size ){
	/** Error: Invalid size **/
	 return -1;
  }

  switch (type){
	case abs_uint8:
	case abs_uchar8:
		values = (unsigned char *) malloc(count * sizeof(unsigned char));
		uchar_values = (unsigned char*) values;
  		for(i=0; i< count; i++)
			uchar_values[i] = vals[i][0];
		break;

	case abs_int8:
	case abs_char8:
		values = (char *) malloc(count * sizeof(char));
		char_values = (char*) values;
  		for(i=0; i< count; i++)
			char_values[i] = vals[i][0];
		break;

	case abs_uint16:
		values = (unsigned short *) malloc(count * sizeof(unsigned short));
		ushort_values = (unsigned short  *)values;
  		for(i=0; i< count; i++)
			ushort_values[i] = atoi(vals[i]);
		break;

	case abs_int16:
		values = (short*) malloc(count * sizeof(short));
		short_values = (short *)values;
  		for(i=0; i< count; i++)
			short_values[i] = atoi(vals[i]);
		break;

	case abs_uint32:
		values = (unsigned int *) malloc(count * sizeof(unsigned int));
		uint_values = (unsigned int *) values;
  		for(i=0; i< count; i++)
			uint_values[i] = atoi(vals[i]);
		break;

	case abs_int32:
		values = (int *) malloc(count * sizeof(int));
		int_values = (int*) values;
  		for(i=0; i< count; i++)
			int_values[i] = atoi(vals[i]);
		break;

	case abs_uint64:
		values = (unsigned long *) malloc(count * sizeof(unsigned long));
		ulong_values = (unsigned long*) values;
  		for(i=0; i< count; i++)
			ulong_values[i] = Absff_Atol(vals[i]);
		break;
	
	case abs_int64:
		values = (long *) malloc(count * sizeof(long));
		long_values = (long *) values;
  		for(i=0; i< count; i++)
			long_values[i] = Absff_Atol(vals[i]);
		break;
	
	case abs_float32:
		values = (float *) malloc(count * sizeof(float));
		float_values = (float *) values;
		for(i=0; i<count; i++)
			float_values[i] = Absff_Atof(vals[i]);
		break;

	case abs_float64:
		values = (double *) malloc(count * sizeof(double));
		double_values = (double *) values;
		for(i=0; i<count; i++)
			double_values[i] = Absff_Atod(vals[i]);
		break;

	case abs_string:
		/* No conversion necessary */
  		return Absff_SetMetadataValues(amdi, (void *) vals, start_index, count);
		/* NOTREACHED */
		break;

	default:
		/* Error: Invalid type */
		return -1;
		
  }
  
  ret = Absff_SetMetadataValues(amdi, values, start_index, count);
  free(values);
  return (ret);
}

/** Errors: -1 for NULL params or other library errors**/

Abs_Type Absff_GetMetadataType(amdi)
 Abs_Metadata *amdi;
{
 if(amdi == NULL || amdi->adi == NULL)
	return (Abs_Type)-1;

 return amdi->abstype;
}

long Absff_GetMetadataSize(amdi)
 Abs_Metadata *amdi;
{
 if(amdi == NULL || amdi->adi == NULL)
	return -1;

 return amdi->size;
}

int Absff_GetMetadataValues(amdi, vals, start_index, count)
 /** Note that vals is appropriately allocated at the call site **/
 Abs_Metadata 	*amdi;
 void 		*vals;
 long		start_index;
 long 		count;
{
 long size;
 Abs_Type type;

 if((amdi == NULL) ||
	(amdi->adi == NULL) || (amdi->vals == NULL))
	return -1;


  size = amdi->size;
  type = amdi->abstype;

  if (count < 0 || count > size || start_index < 0 || start_index > (size -1) ){
	/** Error: Overflow **/
	return -1;
  }


  if(vals != amdi->vals)
	Absff_CopyFromValues(vals, amdi->vals, type, size, start_index, count);

  return 1;
}

int Absff_GetMetadataValuesAsString(amdi, vals, start_index, count)
 /** Note that vals is appropriately allocated at the call site **/
 /** But the individual elements will be allocated here and freed at the call
	site **/
 Abs_Metadata 	*amdi;
 char 		**vals;
 long		start_index;
 long 		count;
{
 long size;
 Abs_Type type;
 int i, j;

 char buffer[50];

 unsigned char *uchar_values;
 char *char_values;
 unsigned short *ushort_values;
 short *short_values;
 unsigned int  *uint_values;
 int *int_values;
 unsigned long *ulong_values;
 long *long_values;
 float *float_values;
 double *double_values;
 char **string_values; 

 if((amdi == NULL) ||
	(amdi->adi == NULL) || (amdi->vals == NULL) || vals == NULL)
	return -1;


  size = amdi->size;
  type = amdi->abstype;

  if (count < 0 || count > size || start_index < 0 || start_index > (size -1) ){
	/** Error: Overflow **/
	return -1;
  }

  switch (type){
	case abs_uint8:
	case abs_uchar8:
		uchar_values = (unsigned char *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%c", uchar_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
	 	break;
		
	case abs_int8:
	case abs_char8:
		char_values = (char *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%c", char_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
	 	break;
	
	case abs_uint16:
		ushort_values = (unsigned short *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%d", ushort_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
	 	break;
		
	case abs_int16:
		short_values = (short *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%d", short_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
	 	break;
		
	case abs_uint32:
		uint_values = (unsigned int  *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%d", uint_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
	 	break;

	case abs_int32:
		int_values = (int *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%d", int_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
	 	break;

	case abs_uint64:
		ulong_values = (unsigned long *)amdi->vals;	
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%ld", ulong_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
		break;

	case abs_int64:

		long_values = (long *)amdi->vals;	
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%ld", long_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
		break;

	case abs_float32:
		float_values = (float *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%g", float_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
		break;

	case abs_float64:
		double_values = (double *)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			sprintf(buffer, "%g", double_values[i]);
			vals[j] = Absff_CopyString(buffer);
		} 
		break;

	case abs_string:
		string_values = (char **)amdi->vals;
		for(i=start_index, j=0; j<count; i++, j++){
			vals[j]=(char *) malloc((strlen(string_values[i])+1) * sizeof(char));
			strcpy(vals[j], string_values[i]);
		} 
		break;
	
	default:
		/* Error: Invalid Type */
		return -1;
  }
  return 1;
}

