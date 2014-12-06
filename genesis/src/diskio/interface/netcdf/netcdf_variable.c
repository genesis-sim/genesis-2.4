/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: netcdf_variable.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: netcdf_variable.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 05:07:03  mhucka
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
 * Revision 1.3  1998/01/14 18:45:31  venkat
 * Definition and assigment of the new Absff_Variable methods.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "netcdf_ext.h"

static void Netcdf_FreeVarValues(vals, type, shape)
 void *vals;
 Abs_Type type;
 Abs_Varshape *shape;
{
 int idim, ndims; 
 nc_type ntype;
 char **char_vals;
 nclong **long_vals;
 short **short_vals;
 float **float_vals;
 double **double_vals;

 if(vals == NULL || shape==NULL) return;

 ndims = shape->ndomains;
 ntype = Abs_Type_To_nc_type(type);

 switch(ntype){
	case NC_BYTE:
	case NC_CHAR:
		char_vals = (char **) vals;		
 		if(ndims == 0){
			free(char_vals[0]);
			free(char_vals);
			return;
 		} 
	
		for(idim=0; idim < ndims; idim++)
			if(char_vals[idim] != NULL) free(char_vals[idim]);
 		free(char_vals);
		break;

	case NC_SHORT:
		short_vals = (short **) vals;
		if(ndims == 0){
                        free(short_vals[0]);
                        free(short_vals);
                        return;
                }

                for(idim=0; idim < ndims; idim++)
                        if(short_vals[idim] != NULL) free(short_vals[idim]);
                free(short_vals);
                break;

	case NC_LONG:
                long_vals = (nclong **) vals;
                if(ndims == 0){
                        free(long_vals[0]);
                        free(long_vals);
                        return;
                }

                for(idim=0; idim < ndims; idim++)
                        if(long_vals[idim] != NULL) free(long_vals[idim]);
                free(long_vals);
                break;

	case NC_FLOAT:
		float_vals = (float **) vals;
                if(ndims == 0){
                        free(float_vals[0]);
                        free(float_vals);
                        return;
                }

                for(idim=0; idim < ndims; idim++)
                        if(float_vals[idim] != NULL) free(float_vals[idim]);
                free(float_vals);
                break;

	case NC_DOUBLE:
		double_vals = (double **) vals;
                if(ndims == 0){
                        free(double_vals[0]);
                        free(double_vals);
                        return;
                }

                for(idim=0; idim < ndims; idim++)
                        if(double_vals[idim] != NULL) free(double_vals[idim]);
                free(double_vals);
                break;

 }
 return; 
}

int Netcdf_CopyVarValues(dup_vals, orig_vals, type, shape)
 void *dup_vals, *orig_vals;
 Abs_Type type;
 Abs_Varshape *shape;
{
 int idim, ndims, i;
 nc_type ntype;
 char **dup_char_val, *orig_char_val;
 short **dup_short_val, *orig_short_val;
 nclong **dup_long_val, *orig_long_val;
 float **dup_float_val, *orig_float_val;
 double **dup_double_val, *orig_double_val;

 if(shape==NULL || dup_vals==NULL || orig_vals==NULL) 
	return -1;

 ndims = shape->ndomains;
 ntype = Abs_Type_To_nc_type(type);

 switch(ntype){
	case NC_BYTE:
	case NC_CHAR:
		dup_char_val = (char **) dup_vals;
		orig_char_val = (char *) orig_vals;
		for(idim=0; idim <= ndims; idim++){
			if(ndims != 0 && idim == ndims) break;
			if(ndims==0){
				*(char *) &dup_char_val[0][0] = *(char *) &orig_char_val[0];
                                break;
                        }
                        for(i=0; i<shape->domainsize[idim]; i++)
                                *(char *)(&dup_char_val[idim][i]) = *(char *) &orig_char_val[i];
                        (orig_char_val) += shape->domainsize[idim];
                }
		break;

	case NC_LONG:
		dup_long_val = (nclong **) dup_vals;
		orig_long_val = (nclong *) orig_vals;
		for(idim=0; idim <= ndims; idim++){
			if(ndims != 0 && idim == ndims) break;
                        if(ndims==0){
                                *(nclong *) &dup_long_val[0][0] = *(nclong *) &orig_long_val[0];
                                break;
                        }
                        for(i=0; i<shape->domainsize[idim]; i++)
                                *(nclong *) &dup_long_val[idim][i] = *(nclong *) &orig_long_val[i];
                        (orig_long_val) += shape->domainsize[idim];
                }
		break;

	case NC_SHORT:
		dup_short_val = (short  **) dup_vals;
		orig_short_val = (short *) orig_vals;
		for(idim=0; idim <= ndims; idim++){
			if(ndims != 0 && idim == ndims) break;
                        if(ndims==0){
                                *(short *) &dup_short_val[0][0] = *(short *) &orig_short_val[0];
                                break;
                        }
                        for(i=0; i<shape->domainsize[idim]; i++)
                                *(short *) &dup_short_val[idim][i] = *(short *) &orig_short_val[i];
                        orig_short_val += shape->domainsize[idim];
                }
		break;

	case NC_FLOAT:
		dup_float_val = (float **) dup_vals;
		orig_float_val = (float *) orig_vals;
		for(idim=0; idim <= ndims; idim++){
			if(ndims != 0 && idim == ndims) break;
                        if(ndims==0){
                                *(float *) &dup_float_val[0][0] = *(float *) &orig_float_val[0];
                                break;
                        }
                        for(i=0; i<shape->domainsize[idim]; i++)
                                *(float *) &dup_float_val[idim][i] = *(float *) &orig_float_val[i];
                        (orig_float_val) += shape->domainsize[idim];
                }
		break;
	
	case NC_DOUBLE:

		dup_double_val = (double **) dup_vals;
		orig_double_val = (double *) orig_vals;
		for(idim=0; idim <= ndims; idim++){
			if(ndims != 0 && idim == ndims) break;
                        if(ndims==0){
                                *(double *) &dup_double_val[0][0] = *(double *) &orig_double_val[0];
                                break;
                        }
                        for(i=0; i<shape->domainsize[idim]; i++)
                                *(double *) &dup_double_val[idim][i] = *(double *) &orig_double_val[i];
                        (orig_double_val) += shape->domainsize[idim];
                }
		break;
	default:
		/** Error: Invalid type **/
		return -1;
		/* NOTREACHED */
		break;
 }
 return 1;
}

void* Netcdf_MakeDefaultVarValues(type, shape)
 Abs_Type type;
 Abs_Varshape *shape;
{
 nc_type  ntype;
 int ndims, idim; 
 char **char_val;
 nclong **long_val;
 short **short_val;
 float **float_val;
 double **double_val;
 int *dimsize = NULL;
 void *def_val = NULL;

 if(shape == NULL) return NULL;

/** 
 Allow no more than 1 dims at present : Need to find a way for 
	more dimensions so the vals cache can be appropriately defined and 
   	allocated. 

 if(ndims > 1 || ndims < 0){
	return NULL;
 }
**/
 ntype = Abs_Type_To_nc_type(type);

 ndims = shape->ndomains;
 if(ndims > 0)
	dimsize = (int *) malloc(ndims * sizeof(int));

 for(idim=0; idim<= ndims; idim++){
 	/** The LE condition is because we want to allocate for 0 dims as well **/
	if(idim!=0 && idim>=ndims) break;
	if (ndims>0) dimsize[idim] = shape->domainsize[idim];


 switch(ntype){
	/** Determine default value based on type and size **/		

	case NC_BYTE:
	case NC_CHAR:

		if(ndims == 0 || dimsize[idim]==DYNAMICSIZECOMPONENT){
			if(def_val == NULL){
				def_val = (char **)calloc(1, sizeof(char*));
		 		char_val = (char **) def_val;
			}	
			char_val[idim] = (char *) calloc(1, sizeof(char));
		}
		else{
			if(def_val == NULL){
				def_val = (char **)calloc(ndims, sizeof(char*));
				char_val = (char **) def_val;
			}
			char_val[idim] = (char *) calloc(dimsize[idim], sizeof(char));
		}
		break;

	case NC_SHORT:

		if(ndims == 0 || dimsize[idim]==DYNAMICSIZECOMPONENT){
			if(def_val == NULL){
				def_val = (short **)calloc(1, sizeof(short*));
				short_val = (short **) def_val;
			}
			short_val[idim] = (short *) calloc(1, sizeof(short));
		}
		else{
			if(def_val == NULL){
				def_val = (short **)calloc(ndims, sizeof(short*));
				short_val = (short **) def_val;
			}
			short_val[idim] = (short *) calloc(dimsize[idim], sizeof(short));
		}
		break;

	case NC_LONG:

		if(ndims == 0 || dimsize[idim]==DYNAMICSIZECOMPONENT){
			if(def_val == NULL){
				def_val = (nclong **)calloc(1, sizeof(nclong*));
				long_val = (nclong **) def_val;
			}
			long_val[idim] = (nclong *) calloc(1, sizeof(nclong));
		}
		else{
			if(def_val == NULL){
				def_val = (nclong **)calloc(ndims, sizeof(nclong*));
				long_val = (nclong **) long_val;
			}
			long_val[idim] = (nclong *) calloc(dimsize[idim], sizeof(nclong));
		}
		break;
	
	case NC_FLOAT:

		if(ndims == 0 || dimsize[idim]==DYNAMICSIZECOMPONENT){
			if(def_val == NULL){
				def_val = (float **)calloc(1, sizeof(float*));
				float_val = (float **) def_val;
			}
			float_val[idim] = (float *) calloc(1, sizeof(float));
		}
		else{
			if(def_val == NULL){
				def_val = (float **)calloc(ndims, sizeof(float*));
				float_val = (float **) def_val;
			}
	    		float_val[idim] = (float *) calloc(dimsize[idim], sizeof(float));
		}
                break;

	case NC_DOUBLE:
		
		if(ndims == 0 || dimsize[idim]==DYNAMICSIZECOMPONENT){
			if(def_val == NULL){
				def_val = (double **)calloc(1, sizeof(double*));
				double_val = (double **) def_val;
			}
			double_val[idim] = (double *) calloc(1, sizeof(double));
		}
		else{
			if(def_val == NULL){
				def_val = (double **)calloc(ndims, sizeof(double*));
				double_val = (double **) def_val;
			}
			double_val[idim] = (double *) calloc(dimsize[idim], sizeof(double));
		}
		break;

        default:
		/** Error: Invalid type **/
		break;
		
        }
 }
 if(dimsize != NULL)
	free(dimsize);
 return def_val;
}

static int Netcdf_UpdateDimensionSize(avi, input)
 Abs_Variable *avi;
 char *input;
{
 /* 
This utility function serves to update the shape->dimensionsize[] 
 */ 

 int idim, ndims, fid, varid, dimids[MAX_VAR_DIMS];
 int status;
 Abs_Varshape *shape;
 Netcdf_Variable *navi = (Netcdf_Variable *) avi;
 Netcdf_Diskio *nadi = (Netcdf_Diskio *) avi->adi; 

 fid = nadi->file_id;
 varid = navi->var_id;

 shape = avi->GetShape(avi);
 if(shape == NULL) return -1;

 ncvarinq(fid, varid, NULL, NULL, &shape->ndomains, dimids, NULL);
 ndims = shape->ndomains;

 status = 1;
 if(ndims > 0){
	for(idim=0; idim<ndims; idim++)	
	/* We care only about the new size along the dimension */
		status = ncdiminq(fid, dimids[idim], NULL, &shape->domainsize[idim]);
 }

 return status;
}
	
void Netcdf_Funcinit_Variable(navi)
 Netcdf_Variable *navi;
{

  navi->ConstructMetadata_Interface = Netcdf_ConstructLocalAttr;
  navi->Create = Netcdf_CreateVar;
  
  navi->SetShape = Netcdf_SetVarShape;
  navi->SetType = Netcdf_SetVarType;
  navi->SetValues = Netcdf_SetVarValues;
  navi->SetValuesFromString = Absff_SetVariableValuesFromString;

  navi->GetShape = Netcdf_GetVarShape;
  navi->GetType = Netcdf_GetVarType;
  navi->GetTypeSize = Netcdf_GetVarTypeSize;
  navi->GetValues = Netcdf_GetVarValues;
  navi->GetValuesAsString = Absff_GetVariableValuesAsString;
  navi->GetIndexForInput = Netcdf_GetVarIndexForInput;
  navi->FastGetIndexForInput = Netcdf_FastGetVarIndexForInput;
  navi->ResetFastIndexForInput = Absff_ResetFastVariableIndexForInput;
  
  navi->IsName = Netcdf_IsVarName;
  navi->Rename = Netcdf_RenameVar;
  navi->Delete = Netcdf_DeleteVar;
  navi->Flush =  Netcdf_FlushVar;
  navi->FreeChildInterface = Netcdf_FreeChildAttrInterface;
  navi->Destruct = Netcdf_DestructVar;
}



Abs_Metadata *Netcdf_ConstructLocalAttr(avi, name, type, size)
 Abs_Variable *avi;
 char *name;
 Abs_Type type;
 long size;
{
Abs_Metadata *amdi = (struct Abs_Metadata_Type *) calloc(1, sizeof(struct Netcdf_Metadata_Type)); /* amdi->avi = NULL happens here*/
 
 if(amdi != NULL){
#define NO_ATT -1
 	int status;
        struct Netcdf_Variable_Type *navi = (struct Netcdf_Variable_Type *)avi;
        struct Netcdf_Metadata_Type *namdi = (struct Netcdf_Metadata_Type *)amdi;
        struct Netcdf_Diskio_Type *nadi = (struct Netcdf_Diskio_Type *)avi->adi;
	
        Netcdf_Funcinit_Metadata(namdi);
        amdi->avi = avi;
	amdi->adi = avi->adi;

	status = ncattinq(nadi->file_id, navi->var_id, name, NULL, NULL);
	if(status == NO_ATT){
		if(!nadi->is_writable) {
			/** Error: Not writable **/
			return (NULL);
		}
		else{
                        if((status=Netcdf_CreateAttr(amdi, type, size))!= -1){
                                namdi->attr_name = Absff_CopyString(name);
                        }
                        else{
				{
                                /** Determine  error status and handle it **/
				}
                                return (NULL);
        		}
                }
        } /* status = NO_ATT */
        else {
		long size;
		Abs_Type type;
		nc_type ntype;
		void *values;

		int accesstoggled;

                namdi->attr_name = Absff_CopyString(name);
		ncattinq(nadi->file_id, navi->var_id, namdi->attr_name, &ntype, (int *) &size);

		/* 
		KLUDGE: The writable flag is toggled and untoggled to 
		let the metadata methods cache the size, type and values 
		*/
		accesstoggled = 0;
		if(!nadi->is_writable){
			nadi->is_writable = !nadi->is_writable;
			accesstoggled = 1;
		}

		Absff_SetMetadataSize(amdi, size);
		type = nc_type_To_Abs_Type(ntype);
		Absff_SetMetadataType(amdi, type);
		values = Absff_AllocDefaultValues(type, size);
		ncattget(nadi->file_id, navi->var_id, namdi->attr_name, values);
		Absff_SetMetadataValues(amdi, values, 0, size);
		namdi->propmodified = 0; /* All the Sets are associated with 
					a read. So no modification */
		if(values != NULL) free(values);

		if(accesstoggled)
			nadi->is_writable = !nadi->is_writable;
        }
#undef NO_ATT
 } /* amdi != NULL */

        return (amdi);
}

/** Errors: -1 for library error or NULL params, positive or zero for success **/
int Netcdf_CreateVar(avi, name, type, shape)
 Abs_Variable *avi;
 char *name;
 Abs_Type type;
 Abs_Varshape *shape;
{
 Netcdf_Diskio *nadi;
 int fid;
 nc_type ntype;
 int *dimids, unlimid;
 int dimsize;

 int status, i;


 if((avi == NULL) ||
        (avi->adi == NULL) ||
                (name == NULL) ||
                        (shape == NULL))
        return -1;

 nadi = (struct Netcdf_Diskio_Type *) avi->adi;
 fid = nadi->file_id; 

 ntype = Abs_Type_To_nc_type(type);
 if(ntype == (nc_type)NC_INVALID){
	fprintf(stderr, "Sorry: This type %d not supported in Netcdf\n", (int)type);
	return -1;
 }
 if(nadi->datamode){
        ncredef(nadi->file_id);
        nadi->datamode=!(nadi->datamode);
 }

#define NO_DIM -1
 if(shape->ndomains == 0) 
	dimids = NULL;
 else
 	dimids = (int *) malloc(shape->ndomains * sizeof(int));
 for (i=0; i<shape->ndomains; i++){
	dimids[i] = ncdimid(fid, shape->domainname[i]);
	if(dimids[i] == NO_DIM){
		if(shape->domaintype[i] == DYNAMICSIZECOMPONENT)
			dimsize = NC_UNLIMITED;
		else
			dimsize = shape->domainsize[i];
		dimids[i] = ncdimdef(fid, shape->domainname[i], dimsize);
	}
	else{
		ncinquire(fid, NULL, NULL, NULL, &unlimid);
		if(unlimid == dimids[i])
			shape->domaintype[i] = DYNAMICSIZECOMPONENT;
		else
			shape->domaintype[i] = STATICSIZECOMPONENT;

		ncdiminq(fid, dimids[i], NULL, &shape->domainsize[i]);
	}
 }
 status = ncvardef(fid, name, ntype, shape->ndomains, dimids);
 if(dimids != NULL) free(dimids);
#undef NO_DIM
 return status;
}

int Netcdf_SetVarValues(avi, vals, start_index, count)
 Abs_Variable *avi;
 void *vals;
 long start_index[];
 long count[];
{
 
 Netcdf_Diskio *nadi;
 Netcdf_Variable *navi;
 int fid, varid;

 Abs_Varshape *shape;
 Abs_Type type;
 int status;
 int idim, i;
 long mindex[1];
 void *conv_vals;
 int numvals;

 if((avi == NULL) ||
        (avi->adi == NULL))
   return -1;

  nadi  = (struct Netcdf_Diskio_Type *) avi->adi;
  navi  = (struct Netcdf_Variable_Type *) avi;


  if(!nadi->is_writable){
                /** Error: Not writable **/
                return -1;
  }

  
  shape = Netcdf_GetVarShape(avi);
  if(shape == NULL){
	{
	/* Error: Invalid shape */
	}
	 return -1;
  }
	

  for(idim=0; idim<shape->ndomains; idim++){
	if(start_index[idim] < 0 || count[idim] < 0)
		return -1;
	if(shape->domaintype[idim] == DYNAMICSIZECOMPONENT)
		continue;
	if(count[idim] > shape->domainsize[idim] || start_index[idim] > (shape->domainsize[idim] - 1)){
		{
		/** Error: invalid index or size **/
		}
		return -1;
	}
  }	

  fid = nadi->file_id;
  varid = navi->var_id;

  if(!nadi->datamode){
        ncendef(fid);
        nadi->datamode = !(nadi->datamode);
  }

  type = Netcdf_GetVarType(avi);
  numvals = 0;
  for(i=0; i< shape->ndomains; i++)
	numvals +=count[i];

  conv_vals = Netcdf_Convert_To_nc_type(vals, type, numvals);
  if(conv_vals != NULL){
  	if(shape->ndomains == 0) {
		mindex[0] = (long)0;
		status = ncvarput1(fid, varid, mindex, conv_vals);
  	}
  	else
  		status = ncvarput(fid, varid, start_index, count, conv_vals);
  }
	
  if(conv_vals != vals && conv_vals != NULL) free(conv_vals);
  
  /* May be needed */
  /* ncsync(fid); */

  if(status != -1){
	int dimids[MAX_VAR_DIMS];
	int ndims;
#if 0
	int unlimid;
	int size;
	int ndims;
    	/* Note that the shape could have changed - e.g values along unlimited
		dimension could have been set. So we need to get and set 
		the new shape cache  */
	shape = NULL;
	shape = (Abs_Varshape *) malloc(1 * sizeof(struct Abs_Varshape_Type));			
	ncinquire(fid, NULL, NULL, NULL, &unlimid);
	ncvarinq(fid, varid, NULL, NULL, &shape->ndomains, dimids, NULL);	
	ndims = shape->ndomains;

	{

	 /* Realloc the shape components */
	if(ndims > 0){
		shape->domainname = (char **) malloc(ndims * sizeof(char *));
		shape->domainsize = (long *) malloc(ndims * sizeof(long));
		shape->domaintype = (byte *) malloc(ndims * sizeof(byte));
	}

	}

	{

	/* Inquire the size and names of the domains */
	for(idim=0; idim < ndims; idim++){
		shape->domainname[idim] = (char *) malloc(MAX_NC_NAME * sizeof(char));
		ncdiminq(fid, dimids[idim], shape->domainname[idim], &shape->domainsize[idim]);
		if(dimids[idim] == unlimid)
			shape->domaintype[idim] = DYNAMICSIZECOMPONENT;
	}

	}

	Absff_SetVariableShape(avi, shape);

	{

	/* Free the local shape variable  */
	Absff_FreeVariableShape(&shape);

	}
#endif
	/* Just get the new size */
	ncvarinq(fid, varid, NULL, NULL, NULL, dimids, NULL);	
	ndims = shape->ndomains;
	for(idim=0; idim < ndims; idim++)
		ncdiminq(fid, dimids[idim], NULL, &shape->domainsize[idim]);
	
	return 1;
  }
  return -1;
}

int Netcdf_IsVarName(avi, varname)
 Abs_Variable *avi;
 char *varname;
{
 int varid, fid;
 char namebuf[MAX_NC_NAME];
 Netcdf_Diskio *nadi;
 Netcdf_Variable * navi = (Netcdf_Variable *) avi; 

 if(avi == NULL || avi->adi == NULL || varname == NULL) {
	/** Error */
	return 0;
 } 
  
 nadi = (Netcdf_Diskio *) avi->adi;
 fid = nadi->file_id;
 varid = navi->var_id; 

 ncvarinq(fid, varid, namebuf, NULL, NULL, NULL, NULL);
 if (strcmp(namebuf, varname) == 0) 
	return 1;
 else
	return 0;
}

long Netcdf_GetVarIndexForInput(avi, input)
 Abs_Variable *avi;
 char *input;
{
 /* 
  This placeholder serves to update the shape-dimensionsize[] since 
  the interface to the coordinate variable is not shared by client objects.
  The construction of the interface would take care of the name and type, 
  and possibly the number of dimensions as well, but the size will not 
  be updated in all the interfaces when values are written using one 
  client object's coordinate interface
 */

#define INVALID_INDEX -999

 if (Netcdf_UpdateDimensionSize(avi, input) == -1)
	return INVALID_INDEX;

#undef INVALID_INDEX

 /* 
    Then call the base class method that does the actual searching 
    This base class method is a slow version that checks 
    through the values starting with index 0 every time 
 */
  
 return Absff_GetVariableIndexForInput(avi, input); 

}

long Netcdf_FastGetVarIndexForInput(avi, input)
 Abs_Variable *avi;
 char *input;
{
 /* 
  This placeholder serves to update the shape-dimensionsize[] since 
  the interface to the coordinate variable is not shared by client objects.
  The construction of the interface would take care of the name and type, 
  and possibly the number of dimensions as well, but the size will not 
  be updated in all the interfaces when values are written using one 
  client object's coordinate interface
 */

#define INVALID_INDEX -999

 if (Netcdf_UpdateDimensionSize(avi, input) == -1)
	return INVALID_INDEX;

#undef INVALID_INDEX

 /* 
    Then call the base class method that does the actual searching
    This base class method is a faster version than the one that checks 
    through the values starting with index 0 every time 
 */
 return Absff_FastGetVariableIndexForInput(avi, input); 

}

  
int Netcdf_RenameVar(avi, newname)
 Abs_Variable *avi;
 char *newname;
{
 Netcdf_Diskio *nadi;
 Netcdf_Variable *navi;
 int fid, varid;
 int status;


 if((avi == NULL) ||
        (avi->adi == NULL))
        return -1;

  nadi  = (struct Netcdf_Diskio_Type *) avi->adi;
  navi  = (struct Netcdf_Variable_Type *) avi;

  fid = nadi->file_id;
  varid = navi->var_id;

 
  if(!nadi->is_writable){
                /** Error: Not writable **/
                return -1;
  }

  if(nadi->datamode){
        ncredef(fid);
        nadi->datamode = !(nadi->datamode);
  }

  status = ncvarrename(fid, varid, newname);
  ncsync(fid);
  if(status == -1)
  	return -1;
  else
	return 1;
}

int Netcdf_FlushVar(avi)
 Abs_Variable *avi;
{
 Netcdf_Diskio *nadi;

 if((avi == NULL) ||
        (avi->adi == NULL))
        return -1;

 nadi  = (struct Netcdf_Diskio_Type *) avi->adi;

 if(!nadi->is_writable) 
	return -1;
 else
 	return 1;
}

int Netcdf_FreeChildAttrInterface(avi, xface)
 Abs_Variable *avi;
 void *xface;
{
 free(xface);
 return 1;
}

int Netcdf_DestructVar(avi)
 Abs_Variable *avi;
{
 return Absff_DestructVariable(avi); 
}

Abs_Varshape* Netcdf_GetVarShape(avi)
 Abs_Variable *avi;
{
 return Absff_GetVariableShape(avi);
}

Abs_Type Netcdf_GetVarType(avi)
 Abs_Variable *avi;
{
 return Absff_GetVariableType(avi);
}

size_t Netcdf_GetVarTypeSize(avi)
 Abs_Variable *avi;
{
 return Absff_GetVariableTypeSize(avi);
}

int Netcdf_GetVarValues(avi, vals, start_index, count)
 /** Note that vals should be allocated appropriately at call site **/
 Abs_Variable *avi;
 void *vals;
 long start_index[];
 long count[];
{
 int idim;
 int ndims;
 long mindex[1];
 Abs_Varshape *shape;
 Abs_Type type;
 int numvals, i, status;
 void *conv_vals;
 Netcdf_Variable *navi = (struct Netcdf_Variable_Type *)avi;
 Netcdf_Diskio *nadi = (struct Netcdf_Diskio_Type *) avi->adi;

 if(navi==NULL || nadi==NULL || avi->varshape==NULL || vals==NULL) 
	return -1;

 status = -1;
 shape = Netcdf_GetVarShape(avi);
 ndims = shape->ndomains;

 type = Netcdf_GetVarType(avi);
 numvals = 0;
 for(i=0; i<ndims; i++)
	numvals+=count[i];
 conv_vals = Netcdf_Convert_To_nc_type(vals, type, numvals);
 if(vals != conv_vals){
	free(vals);
	vals = conv_vals;
 }

 if(!nadi->datamode){
	ncendef(nadi->file_id);
	nadi->datamode = !(nadi->datamode);
 }
 
 if(ndims == 0 && conv_vals!=NULL){
	mindex[0] = 0; 
	status = ncvarget1(nadi->file_id, navi->var_id, mindex, conv_vals);
 }

 for(idim=0; idim < ndims; idim++) {
	if(count[idim] < 0 || start_index[idim] < 0)
		return -1;
	if(shape->domaintype[idim] == DYNAMICSIZECOMPONENT)
		continue;
	if(count[idim] > shape->domainsize[idim] || start_index[idim] > (shape->domainsize[idim] - 1)){
		return -1;
	}
 }
 if(ndims > 0 && conv_vals != NULL)
 	status = ncvarget(nadi->file_id, navi->var_id, start_index, count, conv_vals);
 
 if(status == -1)
 	return -1;
 else
	return 1;
}

int Netcdf_SetVarType(avi, type)
 Abs_Variable *avi;
 Abs_Type type;
{
 {
 /** Error - Not supported in netcdf **/
 }
 return -1; 
}

int Netcdf_SetVarShape(avi, shape)
 Abs_Variable *avi;
 Abs_Varshape *shape;
{
 {
 /** Error - Not supported in netcdf **/
 }
 return -1;
}


int Netcdf_DeleteVar(avi)
 Abs_Variable *avi;
{
 {
 /**Error: Not supported by netcdf **/
 }
 return -1;
}

