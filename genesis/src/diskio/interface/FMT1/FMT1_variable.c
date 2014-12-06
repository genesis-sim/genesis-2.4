#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "FMT1_ext.h"

void FMT1_Funcinit_Variable(favi)
 FMT1_Variable *favi;
{
  favi->ConstructMetadata_Interface = FMT1_ConstructLocalAttr;
  favi->Create = FMT1_CreateVar;

  favi->SetShape = FMT1_SetVarShape;
  favi->SetType = FMT1_SetVarType;
  favi->SetValues = FMT1_SetVarValues;
  favi->SetValuesFromString = Absff_SetVariableValuesFromString;

  favi->GetShape = FMT1_GetVarShape;
  favi->GetType = FMT1_GetVarType;
  favi->GetTypeSize = FMT1_GetVarTypeSize;
  favi->GetValues = FMT1_GetVarValues;
  favi->GetValuesAsString = Absff_GetVariableValuesAsString;
  favi->GetIndexForInput = Absff_GetVariableIndexForInput;
  favi->FastGetIndexForInput = Absff_FastGetVariableIndexForInput;
  favi->ResetFastIndexForInput = Absff_ResetFastVariableIndexForInput;

  favi->IsName = FMT1_IsVarName;
  favi->Rename = FMT1_RenameVar;
  favi->Delete = FMT1_DeleteVar;
  favi->Flush =  FMT1_FlushVar;
  favi->FreeChildInterface = FMT1_NOP;
  favi->Destruct = FMT1_DummyDestructVar;
}

Abs_Metadata *FMT1_ConstructLocalAttr(avi, name, type, size) 
 Abs_Variable *avi;
 char *name;
 Abs_Type type;
 long size;
{
 /* No non-global attribs possible for FMT1 at the moment */
 return NULL;
}

int FMT1_CreateVar(avi, name, type, shape)
 Abs_Variable *avi;
 char *name;
 Abs_Type type;
 Abs_Varshape *shape;
{
	fmt1_type ftype;
	int nallocations;
	FMT1_Variable *favi = (FMT1_Variable *)avi;

	if((avi == NULL) ||
		(avi->adi == NULL) ||
			(name == NULL) ||
				(shape == NULL))
	return -1;

	if((shape->ndomains != 1) ||
		(strcmp(shape->domainname[0], "SimulationTime") != 0)) 
		return -1;

	ftype = Abs_Type_To_fmt1_type(type);

	if (ftype == (fmt1_type)FMT1_INVALID) {
		fprintf(stderr, "Sorry:Type %d not supported by FMT1\n", (int)type);
		return -1;

	}

		
	if (shape->domainsize[0] == -1) {
		nallocations = FMT1_FRAME_INCREMENT;
		shape->domainsize[0] = 0;
	} else
		nallocations = shape->domainsize[0];
		
	if (favi->vals == NULL) 
		favi->vals = Absff_AllocDefaultValues(type, nallocations);

	if(favi->vals == NULL)
		return -1;

	favi->nvalallocations = nallocations;

	return 1;
}

int FMT1_SetVarValues(avi, vals, start_index, count)
 Abs_Variable *avi;
 void *vals;
 long start_index[];
 long count[];
{
 	/* In FMT1, we have only one dimension */

	Abs_Varshape * shape;		
	int size;
	Abs_Type type;
	FMT1_Diskio *fadi; 
	FMT1_Variable * favi = (FMT1_Variable *) avi;

	if (avi == NULL || avi->adi == NULL || 
		vals == NULL || favi->vals == NULL) 
		return -1;


	fadi = (FMT1_Diskio *) avi->adi;

	size = favi->nvalallocations;
	if (start_index[0] < 0 || count[0] > size)
		return -1;
	type = Absff_GetVariableType(avi);

	if (start_index[0] >= favi->nvalallocations) {
		size += FMT1_FRAME_INCREMENT;
		favi->vals = Absff_ReallocDefaultValues(favi->vals, type, size);
		if (favi->vals != NULL) 
			favi->nvalallocations = size;
	}


	if(vals != favi->vals)
		Absff_CopyToValues(favi->vals, vals, type, (long)size, start_index[0], count[0]);

	if (fadi->is_writable) {
		shape = Absff_GetVariableShape(avi);
		if (start_index[0] == shape->domainsize[0])
			shape->domainsize[0] += count[0];
		favi->propmodified = 1;
	}

	
	return 1;
}

int FMT1_SetVarShape(avi, shape)
 Abs_Variable *avi;
 Abs_Varshape *shape;
{
	{
	/** Error - Not supported in FMT1 **/
	}
	return -1;
}

int FMT1_SetVarType(avi, type)
 Abs_Variable *avi;
 Abs_Type type;
{
 {
 /** Error - Not supported in FMT1 **/
 }
 return -1;
}

int FMT1_IsVarName(avi, varname)
 Abs_Variable *avi;
 char *varname;
{
 return 1;
}

Abs_Type FMT1_GetVarType(avi)
 Abs_Variable *avi;
{
 return Absff_GetVariableType(avi);
}

size_t FMT1_GetVarTypeSize(avi)
 Abs_Variable *avi;
{
 return Absff_GetVariableTypeSize(avi);
}

Abs_Varshape* FMT1_GetVarShape(avi)
 Abs_Variable *avi;
{
 return Absff_GetVariableShape(avi);
}

int FMT1_GetVarValues(avi, vals, start_index, count)
 Abs_Variable *avi;
 void *vals;
 long start_index[];
 long count[];
{
 	/* In FMT1, we have only one dimension */

	Abs_Varshape * shape;		
	int size;
	Abs_Type type;
	FMT1_Variable * favi = (FMT1_Variable *) avi;

	if (avi == NULL ||
		vals == NULL || favi->vals == NULL) 
		return -1;

	shape = Absff_GetVariableShape(avi);
	if (shape == NULL || start_index[0] < 0)
		return -1;
	size = shape->domainsize[0];
	type = Absff_GetVariableType(avi);

	if (start_index[0] < 0 || count[0] > size)
		return -1;

	if(vals != favi->vals)
		Absff_CopyFromValues(vals, favi->vals, type, (long)size, start_index[0], count[0]);

	
	return 1;
}

int FMT1_RenameVar(avi)
 Abs_Variable *avi;
{
 return 1;
}

static int FMT1_FlushDataValues(avi)
 Abs_Variable *avi;
{
  int status;
        void *conv_vals, *values;
        Abs_Type type;
        fmt1_type ftype;
	Abs_Varshape *shape;
        long size;

        FMT1_Diskio *fadi;
        FMT1_Variable * favi = (struct FMT1_Variable_Type *) avi;

        if (favi == NULL || avi->adi == NULL)
                return -1;

        fadi = (struct FMT1_Diskio_Type *) avi->adi;

        status = -1;

        if (! fadi->is_writable)
                favi->propmodified = 0;

	if (favi->propmodified) {
                type = Absff_GetVariableType(avi);
                shape = Absff_GetVariableShape(avi);
		size = shape->domainsize[0];

         /* Convert to the netcdf type : This routine returns null if
                the type is not supported. This condition should be taken
                care of in the Creation/SetType subclass methods itself. I
                have it here for the sake of completeness */

                conv_vals = FMT1_Convert_To_fmt1_type(favi->vals, type, (int)size);

                if(conv_vals != NULL) {
                        ftype = Abs_Type_To_fmt1_type(type);
			fseek(fadi->fp, (long)fadi->writeposition, 0);
                        status = FMT1_varput(&fadi->fp, ftype, (int)size, conv_vals);
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
                favi->propmodified = 0;
                return 1;
        }
}

int FMT1_FlushVar(avi)
 Abs_Variable *avi;
{

/*
  return FMT1_FlushDataValues(avi);
*/
 return 1;

}

int FMT1_DeleteVar(avi)
 Abs_Variable *avi;
{
 return 1;
}

int FMT1_DummyDestructVar(avi)
 Abs_Variable *avi;
{
 return 1;
}

int FMT1_DestructVar(avi)
 Abs_Variable *avi;
{
 FMT1_Variable *favi = (FMT1_Variable *) avi;
/*
 if (FMT1_FlushDataValues(avi) != -1) {
*/
 if(favi->vals != NULL)
 	free(favi->vals);
 return Absff_DestructVariable(avi);
/*
 }
 return -1;
*/
}
