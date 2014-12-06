/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: netcdf_metadata.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: netcdf_metadata.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:40:44  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Fixed errors in fprintf format string.
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
 * Revision 1.3  1998/01/14 18:47:57  venkat
 * Replaced the use of the old metadata-specific utility function calls with
 * the new generic ones defined in src/diskio/interface/absff_utilfunc.c.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "netcdf_ext.h"

int Netcdf_CopyAttrValues(dup_vals, orig_vals, type, size, start_index, count)
 void *dup_vals, *orig_vals;
 Abs_Type type;
 long size, start_index, count;
{
 
 register int i,j;
 nc_type ntype;
 char *dup_char_vals, *orig_char_vals;
 short *dup_short_vals, *orig_short_vals;
 nclong *dup_long_vals, *orig_long_vals;
 float *dup_float_vals, *orig_float_vals;
 double *dup_double_vals, *orig_double_vals;
 
	
  if(dup_vals == NULL || orig_vals==NULL)
	return -1;

  if (count < 0 || count > size || start_index < 0 || start_index > (size -1) ){
	/** Error: Overflow **/
	return -1;
  }

  ntype = Abs_Type_To_nc_type(type);
  switch (ntype) {
	case NC_BYTE:
	case NC_CHAR:

		dup_char_vals = (char *)dup_vals;
		orig_char_vals = (char *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_char_vals[i]=orig_char_vals[j];
		break;

	case NC_SHORT:

		dup_short_vals = (short *)dup_vals;
		orig_short_vals = (short *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_short_vals[i]=orig_short_vals[j];
		break;

	case NC_LONG:

		dup_long_vals = (nclong *)dup_vals;
		orig_long_vals = (nclong *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_long_vals[i]=orig_long_vals[j];
		break;

	case NC_FLOAT:

		dup_float_vals = (float *)dup_vals;
		orig_float_vals = (float *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_float_vals[i]=orig_float_vals[j];
		break;

	case NC_DOUBLE:

		dup_double_vals = (double *)dup_vals;
		orig_double_vals = (double *) orig_vals;
		for(i=start_index, j=0; j<count; i++,j++)
			dup_double_vals[i]=orig_double_vals[j];
		break;

	default:
		/** Error: Invalid Type **/
		return -1;
		/* NOTREACHED */
		break;
  }
  return 1;
}
 

void * Netcdf_MakeDefaultAttrValues(type, size)
 Abs_Type type;
 long size;
{
	void *def_val=NULL;
        nc_type ntype = Abs_Type_To_nc_type(type);
        switch(ntype){
            /* Determine default value based on type and size*/

		case NC_BYTE:
		case NC_CHAR:

			def_val = (char *) calloc((int)size, sizeof(char));
			break;

		case NC_SHORT:

			def_val = (short *) calloc((int)size, sizeof(short));
			break;

		case NC_LONG:

			def_val = (nclong *) calloc((int)size, sizeof(nclong));
			break;

		case NC_FLOAT:

			def_val = (float *) calloc((int)size, sizeof(float));
			break;

		case NC_DOUBLE:

			def_val = (double *) calloc((int)size, sizeof(double));
			break;

		default:
			/** Error: Invalid Type **/
			break;

        }
	return def_val;
}
 

void Netcdf_Funcinit_Metadata(namdi)
Netcdf_Metadata *namdi;
{
 
 namdi->Create = Netcdf_CreateAttr;

 namdi->SetType = Netcdf_SetAttrType;
 namdi->SetSize = Netcdf_SetAttrSize;
 namdi->SetValues = Netcdf_SetAttrValues;
 namdi->SetValuesFromString = Absff_SetMetadataValuesFromString;
 
 namdi->GetType = Netcdf_GetAttrType;
 namdi->GetSize = Netcdf_GetAttrSize;
 namdi->GetValues = Netcdf_GetAttrValues;
 namdi->GetValuesAsString = Absff_GetMetadataValuesAsString;

 namdi->Rename = Netcdf_RenameAttr;
 namdi->Delete = Netcdf_DeleteAttr;
 namdi->Flush = Netcdf_FlushAttr;

 namdi->Destruct = Netcdf_DestructAttr;
}

int Netcdf_CreateAttr(amdi, type, size)
 Abs_Metadata *amdi;
 Abs_Type type;
 long size;
{
 Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type*)amdi;
 nc_type ntype = Abs_Type_To_nc_type(type);
	
 if(ntype == (nc_type)NC_INVALID) {
  fprintf(stderr, "Sorry:Type %d not supported by Netcdf\n", (int)type);
  return -1;
 }
 if(Absff_CreateMetadata(amdi, type, size) != -1){
	namdi->propmodified = 1;
	return 1;
 }
 return -1;
}

int Netcdf_SetAttrType(amdi, type)
 Abs_Metadata *amdi;
 Abs_Type type;
{
 Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type*)amdi;
 nc_type ntype = Abs_Type_To_nc_type(type);
	
 if(ntype == (nc_type)NC_INVALID) {
  fprintf(stderr, "Sorry:Type %d not supported by Netcdf\n", (int)type);
  return -1;
 }
 if(Absff_SetMetadataType(amdi, type) != -1){
	namdi->propmodified = 1;
	return 1;
 }
 return -1;
}

int Netcdf_SetAttrSize(amdi, size)
 Abs_Metadata *amdi;
 long size;
{
 Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type*)amdi;
 if(Absff_SetMetadataSize(amdi, size) != -1){
	namdi->propmodified = 1;
	return 1;
 }
 return -1;
}

int Netcdf_SetAttrValues(amdi, vals, start, count)
 Abs_Metadata *amdi;
 void *vals;
 long start, count;
{
 Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type*)amdi;
 if (Absff_SetMetadataValues(amdi, vals, start, count) != -1){
	namdi->propmodified = 1;
	return 1;
 }
 return -1;
}

Abs_Type Netcdf_GetAttrType(amdi)
 Abs_Metadata *amdi;
{
 return Absff_GetMetadataType(amdi);
}

long Netcdf_GetAttrSize(amdi)
 Abs_Metadata *amdi;
{
 return Absff_GetMetadataSize(amdi);
}

int Netcdf_GetAttrValues(amdi, vals, start, count)
 Abs_Metadata *amdi;
 void *vals;
 long start, count;
{
 return Absff_GetMetadataValues(amdi, vals, start, count); 
}


int Netcdf_RenameAttr(amdi, newname)
 Abs_Metadata *amdi;
 char *newname;
{
 
 Netcdf_Diskio *nadi;
 Netcdf_Variable *navi; 
 int fid, varid;
 char *attrname;

 int status;

  if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

  nadi  = (struct Netcdf_Diskio_Type *) amdi->adi;
  navi  = (struct Netcdf_Variable_Type *) amdi->avi;

  fid = nadi->file_id;
  if(navi == NULL) 
	varid = NC_GLOBAL;
  else 
	varid = navi->var_id;

  if(!nadi->is_writable){
		/** Error: Not writable **/
		return -1;
  }
  
  if(nadi->datamode){
	ncredef(nadi->file_id);
	nadi->datamode = !(nadi->datamode);
  }
    
  attrname = ((struct Netcdf_Metadata_Type *)amdi)->attr_name;
  if ((status=ncattrename(fid, varid, attrname, newname)) == -1)
	return -1;
  else{
	Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type *)amdi;
	if(namdi -> attr_name != NULL) Absff_FreeString(namdi->attr_name);
	namdi->attr_name = Absff_CopyString(newname); 
  }
  if(!nadi->datamode){
	ncendef(nadi->file_id);
	nadi->datamode = !(nadi->datamode);
  }
  status = ncsync(nadi->file_id);

  if(status == -1)
	return -1;
  else
	return 1;
}

int Netcdf_DeleteAttr(amdi)
 Abs_Metadata *amdi;
{
 
 Netcdf_Diskio *nadi;
 Netcdf_Variable *navi; 
 int fid, varid;
 char *attrname;

  int status;

  if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

  nadi  = (struct Netcdf_Diskio_Type *) amdi->adi;
  navi  = (struct Netcdf_Variable_Type *) amdi->avi;

  fid = nadi->file_id;
  if(navi == NULL) 
	varid = NC_GLOBAL;
  else 
	varid = navi->var_id;

  if(!nadi->is_writable){
		/** Error: Not writable **/
		return -1;
  }
  
  if(nadi->datamode){
	ncredef(nadi->file_id);
	nadi->datamode = !(nadi->datamode);
  }
    
  attrname = ((struct Netcdf_Metadata_Type *)amdi)->attr_name;
  if ((status = ncattdel(fid, varid, attrname)) == -1)
	return -1;
  else{
	Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type *)amdi;
	namdi->propmodified = 0; /* To avoid destructor attempting to 
					commit changes to file */
	Netcdf_DestructAttr(amdi);
  }
  if(!nadi->datamode){
	ncendef(nadi->file_id);
	nadi->datamode = !(nadi->datamode);
  }
  status = ncsync(nadi->file_id);
  if(status == -1)
	return -1;
  else
  	return 1;
}

int Netcdf_FlushAttr(amdi)
 Abs_Metadata *amdi;
{
 Netcdf_Diskio *nadi;
 Netcdf_Variable *navi;
 int fid, varid;
 int status;
 Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type *)amdi;

  if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

  nadi = (struct Netcdf_Diskio_Type *) amdi->adi;
  navi = (struct Netcdf_Variable_Type *) amdi->avi; 


  fid = nadi->file_id;
  if(navi == NULL)
	varid = NC_GLOBAL;
  else
	varid = navi->var_id;
  
  status = 0;

  if(nadi->is_writable) {
	Abs_Type type;
	nc_type ntype;
	void *conv_values;
	void *values;
	long size;
 	char *attrname;
	
  	attrname = namdi->attr_name;
	if(attrname == NULL) return -1;
	
	if(nadi->datamode){
		ncredef(fid);
		nadi->datamode = !nadi->datamode;
	}
	ncattdel(fid, varid, attrname); 

	/** Just to make sure the deletion took place, we sync the file - HACK */
	if(!nadi->datamode){
		ncendef(fid);
		nadi->datamode = !nadi->datamode;
	}
	ncsync(fid);

	if(nadi->datamode){
		ncredef(fid);
		nadi->datamode = !nadi->datamode;
	}
	type = Absff_GetMetadataType(amdi);
	size = Absff_GetMetadataSize(amdi);
	values = Absff_AllocDefaultValues(type, size);
	Absff_GetMetadataValues(amdi, values, 0, size);

	/* Convert to the netcdf type : This routine returns null if 
		the type is not supported. This condition should be taken 
		care of in the Creation/SetType subclass methods itself. I
		have it here for the sake of completeness */
	conv_values = Netcdf_Convert_To_nc_type(values, type, (int)size); 

	if(conv_values != values) free(values);
	if(conv_values != NULL){
		ntype = Abs_Type_To_nc_type(type);	
		status = ncattput(fid, varid, attrname, ntype, (int)size, conv_values);	
		free(conv_values);
	}
  }
  if(status == -1 || !nadi->is_writable)
  	return -1; /* Disallow any writing to file */
  else {
	namdi->propmodified = 0; /* Optimize by not writing file when destroyed */
	return 1;
  }
}

int Netcdf_DestructAttr(amdi)
 Abs_Metadata *amdi;
{
 
 Netcdf_Diskio *nadi;
 Netcdf_Variable *navi;
 int fid, varid;
 int status;
 Netcdf_Metadata *namdi = (struct Netcdf_Metadata_Type *)amdi;

  if((amdi == NULL) ||
	(amdi->adi == NULL))
	return -1;

  nadi = (struct Netcdf_Diskio_Type *) amdi->adi;
  navi = (struct Netcdf_Variable_Type *) amdi->avi; 

  if (!nadi->is_writable)
	namdi->propmodified = 0; /* Disallow any writing to file */

  fid = nadi->file_id;
  if(navi == NULL)
	varid = NC_GLOBAL;
  else
	varid = navi->var_id;
  
  status = 0;

  if(namdi->propmodified){
	Abs_Type type;
	nc_type ntype;
	void *conv_values;
	void *values;
	long size;
 	char *attrname;
	
  	attrname = namdi->attr_name;
	if(attrname == NULL) return -1;
	
	if(nadi->datamode){
		ncredef(fid);
		nadi->datamode = !nadi->datamode;
	}
	status = ncattdel(fid, varid, attrname); 
	if(status == -1){
		/** Error **/
	}
	if(!nadi->datamode){
		ncendef(fid);
		nadi->datamode = !nadi->datamode;
	}
	ncsync(fid);
	if(nadi->datamode){
		ncredef(fid);
		nadi->datamode = !nadi->datamode;
	}
	type = Absff_GetMetadataType(amdi);
	size = Absff_GetMetadataSize(amdi);
	values = Absff_AllocDefaultValues(type, size);
	Absff_GetMetadataValues(amdi, values, 0, size);

	/* Convert to the netcdf type : This routine returns null if 
		the type is not supported. This condition should be taken 
		care of in the Creation/SetType subclass methods itself. I
		have it here for the sake of completeness */
	conv_values = Netcdf_Convert_To_nc_type(values, type, (int)size); 

	if(conv_values != values) free(values);
	if(conv_values != NULL){
		ntype = Abs_Type_To_nc_type(type);	
		status = ncattput(fid, varid, attrname, ntype, (int)size, conv_values);	
		free(conv_values);
	}
  }
  if(status == -1){
	/** {
		Error 
		}**/

	return -1;
  }

  if(namdi->attr_name != NULL) {
	 Absff_FreeString(namdi->attr_name);
	 namdi->attr_name = NULL;
  }
  return Absff_DestructMetadata(amdi);
}
