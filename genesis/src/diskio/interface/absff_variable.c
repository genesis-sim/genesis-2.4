/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: absff_variable.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: absff_variable.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/06/12 04:38:21  mhucka
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
 * Revision 1.3  1998/01/14 18:16:42  venkat
 * Replaced usage of no longer defined typdefs (uchar, ushort etc.,) with
 * the appropriate type names. Also replaced old variable-specific
 * function calls with the new generalized equivalents (Absff_CopyToValues(),
 * Absff_AllocDefaultValues() etc.) defined in absff_utilfunc.c.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#include "absff_ext.h"

static int Absff_ActualVariableValueIsSmaller(/*Abs_Variable *avi, Abs_ComponentValue actual, Abs_ComponentValue passed */);

void Absff_FreeVariableShape(pshape)
 Abs_Varshape **pshape;
{
 int idim, ndoms;

 if(*pshape != NULL){
	ndoms = (*pshape)->ndomains;
	if(ndoms > 0){
		for(idim = 0; idim < ndoms; idim++)
			free((*pshape)->domainname[idim]);
		free((*pshape)->domainname);
		(*pshape)->domainname=NULL;
		free((*pshape)->domainsize);
		free((*pshape)->domaintype);
	}
	free(*pshape);
	*pshape = NULL;
 }
}

int Absff_SetVariableShape(avi, shape)
 Abs_Variable *avi;
 Abs_Varshape *shape;
{
 Abs_Varshape *copyshape;
 int idom, ndoms;

 if(avi == NULL) return -1;
 Absff_FreeVariableShape(&avi->varshape);

 if(shape == NULL){
	avi->varshape = NULL; 
	return 1;
 }
 avi->varshape = (Abs_Varshape *) calloc(1, sizeof(struct Abs_Varshape_Type));
 copyshape = avi->varshape;
 ndoms = copyshape->ndomains = shape->ndomains;
 if(ndoms > 0){
 	copyshape->domainname = (char **) malloc(ndoms * sizeof(char*)); 
 	copyshape->domainsize = (long *) malloc(ndoms * sizeof(long));
	copyshape->domaintype = (byte *) malloc(ndoms * sizeof(byte));
 }
 for(idom = 0; idom < ndoms; idom++){
	copyshape->domainname[idom] = Absff_CopyString(shape->domainname[idom]);
	copyshape->domainsize[idom] = shape->domainsize[idom];
	copyshape->domaintype[idom] = shape->domaintype[idom];
 }
 return 1;
}

int Absff_SetVariableType(avi, type)
 Abs_Variable *avi;
 Abs_Type type;
{
 if(avi == NULL || avi->adi==NULL) 
	return -1;
 avi->abstype = type;
 return 1;
}

int Absff_SetVariableValuesFromString(avi, vals, start_index, count)
 Abs_Variable *avi;
 char **vals;
 long start_index[];
 long count[];
{
 Abs_Type type;
 Abs_Diskio *adi;
 int i, j, ret, ndims, start, oldsize;
 Abs_Varshape *shape;
 void *values;

 unsigned char *uchar_values;
 char *char_values;
 unsigned short *ushort_values;
 short *short_values;
 unsigned int *uint_values;
 int *int_values;
 unsigned long *ulong_values;
 long *long_values;
 float *float_values;
 double *double_values;

 if(avi == NULL || avi->adi==NULL || vals==NULL)
	return -1;

 adi =  avi->adi;
 if(!adi->is_writable){
	/** Error: Not writable **/
	return -1;
 }
 
 
 values = NULL;
 type = avi->abstype;
 shape = avi->varshape;
 ndims = shape->ndomains;

 start = 0;
 switch(type){
	case abs_int8:
	case abs_char8:
 		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
			if(values == NULL) 
				values = (char *) malloc(count[0] * sizeof(char));
			else
				values = (char *) realloc(values, oldsize*sizeof(char));	
			char_values = (char *)values;
			for(j=start; j<count[i]; j++)
				char_values[j] = vals[j][0];		
			start+=(int)count[i];
 		}
		break;

	case abs_uint8:
	case abs_uchar8:
 		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
			if(values == NULL) 
				values = (unsigned char *) malloc((int)count[0] * sizeof(unsigned char));
			else
				values = (unsigned char *) realloc(values, oldsize*sizeof(unsigned char));	
			uchar_values = (unsigned char *)values;
			for(j=start; j<count[i]; j++)
				uchar_values[j] = vals[j][0];		
			start+=(int)count[i];
 		}
		break;
		
	case abs_uint16:
 		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
			if(values == NULL) 
				values = (unsigned short *) malloc((int)count[0] * sizeof(unsigned short));
			else
				values = (unsigned short *) realloc(values, oldsize*sizeof(unsigned short));	
			ushort_values = (unsigned short *)values;
			for(j=start; j<count[i]; j++)
				ushort_values[j] = atoi(vals[j]);		
			start+=(int)count[i];
 		}
		break;
	case abs_int16:
 		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
			if(values == NULL) 
				values = (short *) malloc((int)count[0] * sizeof(short));
			else
				values = (short *) realloc(values, oldsize*sizeof(short));	
			short_values= (short *)values;
			for(j=start; j<count[i]; j++)
				short_values[j] = atoi(vals[j]);		
			start+=(int)count[i];
 		}
		break;

	case abs_uint32:
		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
                        if(values == NULL)
                                values = (unsigned int *) malloc((int)count[0] * sizeof(unsigned int));
                        else
                                values = (unsigned int *) realloc(values, oldsize * sizeof(unsigned int));
			uint_values = (unsigned int *) values;
                        for(j=start; j<count[i]; j++)
                                uint_values[j] = atoi(vals[j]);
                        start+=(int)count[i];
                }
                break;
 	
	case abs_int32:
 		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
			if(values == NULL) 
				values = (int *) malloc((int)count[0] * sizeof(int));
			else
				values = (int *) realloc(values, oldsize*sizeof(int));	
			int_values = (int *)values;
			for(j=start; j<count[i]; j++)
				int_values[j] = atoi(vals[j]);		
			start+=(int)count[i];
 		}
		break;

	case abs_int64:
 		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
			if(values == NULL) 
				values = (long *) malloc((int)count[0] * sizeof(long));
			else
				values = (long *) realloc(values, oldsize*sizeof(long));	

			long_values = (long *)values;
			for(j=start; j<count[i]; j++)
				long_values[j] = Absff_Atol(vals[j]);		
			start+=(int)count[i];
 		}
		break;
	case abs_uint64:
		
		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
                        if(values == NULL)
                                values = (unsigned long *) malloc((int)count[0] * sizeof(unsigned long));
                        else
                                values = (unsigned long *) realloc(values, oldsize * sizeof(unsigned long));
			ulong_values = (unsigned long *) values;
                        for(j=start; j<count[i]; j++)
                                ulong_values[j] = Absff_Atol(vals[j]);
                        start+=(int)count[i];
                }
                break;

	case abs_float32:
		
		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
                        if(values == NULL)
                                values = (float *) malloc((int)count[0] * sizeof(float));
                        else
                                values = (float *) realloc(values, oldsize * sizeof(float));
			float_values = (float *) values;
                        for(j=start; j<count[i]; j++)
                                float_values[j] = Absff_Atof(vals[j]);
                        start+=(int)count[i];
                }
                break;

	case abs_float64:

		for(i=0; i<ndims; i++){
			oldsize = (i==0)? count[0]:oldsize+count[i];
                        if(values == NULL)
                                values = (double *) malloc((int)count[0] * sizeof(double));
                        else
                                values = (double *) realloc(values, oldsize * sizeof(double));
			double_values = (double *) values;
                        for(j=start; j<count[i]; j++)
                                double_values[j] = Absff_Atod(vals[j]);
                        start+=(int)count[i];
                }
                break;

	case abs_string:
                /* No conversion necessary */
                return avi->SetValues(avi, vals, start_index, count);
		/* NOTREACHED */
                break;

 	default:
		/* Error: Invalid type */
		return -1;
		/* NOTREACHED */
		break;
	}
	
 ret = avi->SetValues(avi, values, start_index, count);
 free(values);
 return ret;
}

Abs_Varshape* Absff_GetVariableShape(avi)
 Abs_Variable *avi;
{
  if((avi == NULL) || (avi->adi == NULL))
	return NULL;
  return (avi->varshape);
}

Abs_Type Absff_GetVariableType(avi)
 Abs_Variable *avi;
{
  if((avi == NULL) || (avi->adi == NULL))
	return ((Abs_Type) -1);
  return (avi->abstype);
}

size_t Absff_GetVariableTypeSize(avi)
 Abs_Variable *avi;
{
  if((avi == NULL) || (avi->adi == NULL))
	return ((size_t) -1);
  switch (avi->abstype) {
	case abs_float32:
		return ((size_t) sizeof(float));
		/* NOTREACHED */
		break;

	case abs_float64:
		return ((size_t) sizeof(double));
		/* NOTREACHED */
		break;

	/* Need to fill in for other types as well */
 }

 return (size_t) 0;

}

int Absff_GetVariableValuesAsString(avi, vals, start_index, count)
/** Note that vals is appropriately allocated at the call site **/
 /** But the individual elements will be allocated here and freed at the call
        site **/
 Abs_Variable *avi;
 char **vals;
 long start_index[];
 long count[];
{
Abs_Varshape *shape;
Abs_Type type;
int i,j,k, ndims;
char buffer[50];
void *localvals;
int numvals;

 unsigned char *uchar_values;
 char *char_values;
 unsigned short *ushort_values;
 short *short_values;
 unsigned int *uint_values;
 int *int_values;
 unsigned long *ulong_values;
 long *long_values;
 float *float_values;
 double *double_values;


 if(avi == NULL ||  avi->adi == NULL || vals == NULL)
	return -1;

  shape = avi->varshape;
  ndims = shape->ndomains;
  type = avi->abstype;

  numvals = 0;
  for(i=0; i<ndims; i++)
	numvals+=count[i];

  localvals = NULL;

 switch(type){
	case abs_uchar8:
	case abs_uint8:
		localvals = (unsigned char *) malloc(numvals * sizeof(unsigned char));
		uchar_values = (unsigned char *) localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%c", uchar_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				uchar_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;

	case abs_char8:
	case abs_int8:
		localvals = (char *) malloc(numvals * sizeof(char));
		char_values = (char *) localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%c", char_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				char_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
		
	case abs_uint16:
		localvals = (unsigned short *) malloc(numvals * sizeof(unsigned short));
		ushort_values = (unsigned short *) localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%d", ushort_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				ushort_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
		
   	case abs_int16:
		localvals = (short *) malloc(numvals * sizeof(short));
		short_values = (short *)localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%d", short_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				short_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
	
 	case abs_uint32:
		localvals = (unsigned int *) malloc(numvals * sizeof(unsigned int));
		uint_values = (unsigned int *)localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%d", uint_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				uint_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
		
	case abs_int32:
		localvals = (int *) malloc(numvals * sizeof(int));
		int_values = (int *)localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0, k=0; i<ndims; i++){
				for(j=0; j<count[i]; j++, k++){
					sprintf(buffer,"%d", int_values[j]);
					vals[k]= Absff_CopyString(buffer);
				}
				int_values+=count[i];
			} 
		}
		break;

	case abs_uint64:
		localvals = (unsigned long *) malloc(numvals * sizeof(unsigned long));
		ulong_values = (unsigned long *) localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%ld", ulong_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				ulong_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
		
	case abs_int64:
		localvals = (long *) malloc(numvals * sizeof(long));
		long_values = (long *) localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%ld", long_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				long_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
		

	case abs_float32:
		localvals = (float *) malloc(numvals * sizeof(float));
		float_values = (float *) localvals;	
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%g", float_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				float_values+=count[i];
				vals+=count[i];
			} 
		}
		break;

		
	case abs_float64:
		localvals = (double *) malloc(numvals * sizeof(double));
		double_values = (double *) localvals;
		avi->GetValues(avi, localvals, start_index, count);
		if(localvals != NULL) {
			for(i=0; i<ndims; i++){
				for(j=0; j<count[i]; j++){
					sprintf(buffer,"%g", double_values[j]);
					vals[j]= Absff_CopyString(buffer);
				}
				double_values+=count[i];
				vals+=count[i];	
			} 
		}
		break;
		
#define MAX_VAR_LENGTH 100 

	case abs_string:
		
		for(i=0; i<ndims; i++){
			for(j=0; j<count[i]; j++)
				vals[j] = (char*) malloc(MAX_VAR_LENGTH * sizeof(char));
			vals+=count[i];
		}	
		avi->GetValues(avi, vals, start_index, count);
		break;
#undef MAX_VAR_LENGTH
	
	default:
		/*Error: Invalid type */
		break;
 }
 if(localvals != NULL) 
	free(localvals);
 return 1; 
}

void Absff_ConvertVariableValueFromString(avi, value, retptr)
 Abs_Variable *avi;
 char *value;
 void *retptr;
{
 Abs_Type type;
 Abs_ComponentValue *retval;

 if(avi == NULL || value == NULL || retptr == NULL) return;
	
 retval = (Abs_ComponentValue *) retptr;
 type = avi->abstype;
 
 switch(type){

 	case abs_float64:
		retval->double_value = Absff_Atod(value);
	break;

	case abs_float32:
		retval->float_value = Absff_Atof(value);
	break;

	/* 
	{
	  Need to do this for other types as well 
	}
	*/
 }
}

long Absff_GetVariableIndexForInput(avi, input)
 Abs_Variable *avi;
 char *input;
{
#define INVALID_INDEX -999

 int comparestat;
 Abs_Varshape *shape;
 char *vals[1];
 long start_index[1], count[1];
 long varsize, ret_index;
 register long i; 

 Abs_ComponentValue passed_input, actual_input;
 
shape = avi->varshape;

 if(shape->ndomains > 0)
 	varsize = shape->domainsize[0];
 else
	varsize = 0;

 Absff_ConvertVariableValueFromString(avi, input, (void *) &passed_input);
 
 ret_index = 0;
 count[0] = 1;

 for(i= 0; i< varsize; i++){
	start_index[0] = i;	
	avi->GetValuesAsString(avi, vals, start_index, count);
	Absff_ConvertVariableValueFromString(avi, vals[0], (void *) &actual_input);
	Absff_FreeString(vals[0]);
	ret_index = i;
	if(!(comparestat=Absff_ActualVariableValueIsSmaller(avi, actual_input, passed_input))) 
		break;
	else if(comparestat == -1) {
		--ret_index; /* Return the preceding time instant index
				for intermediate time specifications */
		break;
	}
	if(i == varsize-1) ret_index = INVALID_INDEX;
 }
 
 return ret_index;

#undef INVALID_INDEX

}

long Absff_FastGetVariableIndexForInput(avi, input)
 Abs_Variable *avi;
 char *input;
{
#define INVALID_INDEX -999

 int comparestat;
 Abs_Varshape *shape;
 char *vals[1];
 long start_index[1], count[1];
 long varsize; 
 register long i; 


 Abs_ComponentValue passed_input, actual_input;

 /* 
  This elementary faster version just remembers the last
  index returned and starts from there instead of from
  0 everytime.
 */ 
 
shape = avi->varshape;

 if(shape->ndomains > 0)
 	varsize = shape->domainsize[0];
 else
	varsize = 0;

 Absff_ConvertVariableValueFromString(avi, input, (void *) &passed_input);
 
 count[0] = 1;

 if(avi->last_retrieved_index == INVALID_INDEX)
	avi->last_retrieved_index = 0; /* We restart if there was an error at 
					some point */

 /* The construction of the interface sets this member to zero at start */
 /* The ResetFastVariableIndexForInput method can be used to explicitly set
    it back to zero to do the search from the beginning */

 for(i= avi->last_retrieved_index; i< varsize; i++){
	start_index[0] = i;	
	avi->GetValuesAsString(avi, vals, start_index, count);
	Absff_ConvertVariableValueFromString(avi, vals[0], (void *) &actual_input);
	Absff_FreeString(vals[0]);
	avi->last_retrieved_index = i;
	if(!(comparestat=Absff_ActualVariableValueIsSmaller(avi, actual_input, passed_input))) 
		break;
	else if(comparestat == -1) {
		--(avi->last_retrieved_index); /* Return the preceding time instant index
				for intermediate time specifications */
		break;
	}
	if(i == varsize-1) avi->last_retrieved_index = INVALID_INDEX;
 }
 
 return avi->last_retrieved_index;

#undef INVALID_INDEX

}
 
void Absff_ResetFastVariableIndexForInput(avi)
 Abs_Variable *avi;
{
 /* 
   This method is there for client (genesis) objects to send the message
    to the interface to reset the rememberance index back to 0. This 
    can happen for instance, if during a simulation, the user resets 
    the simulation in which case the updation of the output value 
    will have to happen starting with time t = 0 
 */  
 avi->last_retrieved_index = 0;
}

static int Absff_ActualVariableValueIsSmaller(avi, actual, passed)
 Abs_Variable *avi;
 Abs_ComponentValue actual;
 Abs_ComponentValue passed;
{
 Abs_Type type;
 int ret;
 
 ret = 1;
 type = avi->abstype; 

 switch(type){
	case abs_float32:
		if(actual.float_value > passed.float_value)
			ret = -1;
		else if(actual.float_value == passed.float_value)
			ret = 0;
	break;
	
	case abs_float64:
		if(actual.double_value > passed.double_value)
			ret = -1;
		else if(actual.double_value == passed.double_value)
			ret = 0;
	break;
	/*
	 {
		Need to do this for other types as well 
	 }
	*/
 }
 return ret;
}

int Absff_DestructVariable(avi)
 Abs_Variable *avi;
{
 Abs_Varshape *shape;
 int idim, iattr;
 int ndims, nattr;
 Abs_Type type;

 if((avi == NULL) ||
        (avi->adi == NULL))
        return -1;

 nattr = avi->nlocalmetadata;
 if(nattr > 0){
        for(iattr=0; iattr < nattr; iattr++)
         	if(avi->localmetadatanames[iattr] != NULL) free(avi->localmetadatanames [iattr]);
	free(avi->localmetadatanames);
	avi->nlocalmetadata = 0;
 }

 if(avi->varshape == NULL) return 1;

 Absff_FreeVariableShape(&avi->varshape); /* the varshape pointer is set to NULL sentinel
						in the routine  */
 return 1;
}
