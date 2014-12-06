#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "FMT1_ext.h"

void FMT1_Funcinit_Metadata(famdi)
 FMT1_Metadata *famdi;
{
	famdi->Create = FMT1_CreateAttr;
	
	famdi->SetType = FMT1_SetAttrType;
	famdi->SetSize = FMT1_SetAttrSize;
	famdi->SetValues = FMT1_SetAttrValues;
	famdi->SetValuesFromString = Absff_SetMetadataValuesFromString;

	famdi->GetType = Absff_GetMetadataType;
	famdi->GetSize = Absff_GetMetadataSize;
	famdi->GetValues = Absff_GetMetadataValues;
	famdi->GetValuesAsString = Absff_GetMetadataValuesAsString;

	famdi->Rename = FMT1_RenameAttr;
	famdi->Delete = FMT1_DeleteAttr;
	famdi->Flush = FMT1_FlushAttr;

	famdi->Destruct = FMT1_DummyDestructAttr;
	
}

int FMT1_CreateAttr(amdi, type, size)
	Abs_Metadata *amdi;
	Abs_Type type;
	long size;
{
	FMT1_Metadata *famdi = (struct FMT1_Metadata_Type*)amdi;

 	fmt1_type ftype = Abs_Type_To_fmt1_type(type);

 	if(ftype == (fmt1_type)FMT1_INVALID) {
		fprintf(stderr, "Sorry:Type %d not supported by FMT1\n", (int)type);
  		return -1;
 	}
 	return Absff_CreateMetadata(amdi, type, size);
}

int FMT1_SetAttrType(amdi, type)
 Abs_Metadata *amdi;
 Abs_Type type;
{
 FMT1_Metadata *famdi = (struct FMT1_Metadata_Type *)amdi;
 fmt1_type ftype = Abs_Type_To_fmt1_type(type);

 if(ftype == (fmt1_type)FMT1_INVALID) {
  fprintf(stderr, "Sorry:Type %d not supported by FMT1\n", (int)type);
  return -1;
 }
 if(Absff_SetMetadataType(amdi, type) != -1){
        famdi->propmodified = 1;
        return 1;
 }
 return -1;

}

int FMT1_SetAttrSize(amdi, size)
 Abs_Metadata *amdi;
 long size;
{
 FMT1_Metadata *famdi = (struct FMT1_Metadata_Type *)amdi;
 if(Absff_SetMetadataSize(amdi, size) != -1){
        famdi->propmodified = 1;
        return 1;
 }
 return -1;
}


int FMT1_SetAttrValues(amdi, vals, start, count) 
 Abs_Metadata *amdi;
 void *vals;
 long start, count;
{
	FMT1_Metadata *famdi = (struct FMT1_Metadata_Type *) amdi;	
	if(Absff_SetMetadataValues(amdi, vals, start, count) != -1) {
		famdi->propmodified = 1;
		return 1;
	}
	return -1;
}

int FMT1_RenameAttr(amdi)
 Abs_Metadata *amdi;
{
	return 1;
}

int FMT1_FlushMetadataValues(amdi)
 Abs_Metadata *amdi;
{
	int status; 
	void *conv_vals, *values;
	Abs_Type type;
	fmt1_type ftype;
	long size;

	FMT1_Diskio *fadi;
	FMT1_Metadata * famdi = (struct FMT1_Metadata_Type *) amdi;
	
	if (famdi == NULL || amdi->adi == NULL)
		return -1;

	fadi = (struct FMT1_Diskio_Type *) amdi->adi;
	
	status = -1;

	if (! fadi->is_writable)
		famdi->propmodified = 0;

	if (famdi->propmodified) {
		type = Absff_GetMetadataType(amdi);
		size = Absff_GetMetadataSize(amdi);
		values = Absff_AllocDefaultValues(type, size);
		Absff_GetMetadataValues(amdi, values, 0L, size);	

	 /* Convert to the netcdf type : This routine returns null if
                the type is not supported. This condition should be taken
                care of in the Creation/SetType subclass methods itself. I
                have it here for the sake of completeness */

		conv_vals = FMT1_Convert_To_fmt1_type(values, type, (int)size);

		if(conv_vals != values) free(values);
		if(conv_vals != NULL) {
			ftype = Abs_Type_To_fmt1_type(type);
			fseek(fadi->fp, (long)fadi->writeposition, 0);
			status = FMT1_attput(&fadi->fp, ftype, (int)size, conv_vals);
			free(conv_vals);
		}
	}
	
	if(status == -1 || status == 0) {
		{
			/* Error */
		}
		return -1;
	} else {
		fadi->writeposition += status;
		famdi->propmodified = 0;
		return 1;
	}
}
 


int FMT1_FlushAttr(amdi) 
 Abs_Metadata *amdi;
{
 /*
 return FMT1_FlushMetadataValues(amdi);
 */
 return 1;
}

int FMT1_DeleteAttr(amdi)
 Abs_Metadata *amdi;
{
	return 1;
}

int FMT1_DummyDestructAttr(amdi)
 Abs_Metadata *amdi;
{
 return 1;
}

int FMT1_DestructAttr(amdi)
 Abs_Metadata *amdi;
{
 FMT1_Metadata *famdi = (FMT1_Metadata *) amdi;

 /*
 if (FMT1_FlushMetadataValues(amdi) != -1) 
 */
 return Absff_DestructMetadata(amdi);

 /*
 return -1;
 */
}
