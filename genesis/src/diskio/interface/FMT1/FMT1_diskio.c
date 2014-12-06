#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include "FMT1_ext.h"


/* Class methods */

int FMT1_IsA(filename)
 char *filename;
{
  FILE *fp;
  int status;

  if ((fp = fopen(filename, "r")) == NULL)  {
	/*
	** HACK **
	A small hack to preserve BC with genesis disk_in
	A number of scripts just create the object and set
	the filename later even in read mode - So this
	just attempts to create the file with the name
	(usually the name of the object) if it does not exist
	Does not break anything that way - But print an
	error message to that effect right here.
	if ((fp = fopen(filename, "a+")) != NULL) {
		fprintf(stderr, "File '%s' not found or has no data in it\n", filename);
		return 1;
	} else
		return 0; 
	*/
	return 0;
  }

  if (! (status = FMT1_CheckHeader(fp))) {
	fprintf(stderr, "File '%s' does not seem to be a FMT1 file \n", filename);
  	fclose(fp);
  }

  return status;
}

void FMT1_Funcinit_Diskio(fadi) 
 FMT1_Diskio *fadi;
{
 fadi->ConstructMetadata_Interface = FMT1_ConstructGlobalAttr;
 fadi->ConstructVariable_Interface = FMT1_ConstructVar;
 fadi->Flush =   FMT1_FlushFile;
 fadi->FreeChildInterface = FMT1_NOP;
 fadi->DestructChildInterfaces = FMT1_DestructChildInterfaces;
 fadi->Destruct = FMT1_DestructFile;

 fadi->IsCoordVariable = FMT1_IsCoordVar;
 fadi->ConstructCoord_Interface = FMT1_ConstructCoordVar;

 fadi->SetFile = FMT1_SetFile;
 fadi->OpenFile = FMT1_OpenFile;
 fadi->GatherInfo = FMT1_GatherFileInfo;
 fadi->ResetDataOffset = FMT1_ResetDataOffset;
 fadi->ResetFile = FMT1_ResetFile;
 fadi->CloseFile = 	 FMT1_CloseFile;
 fadi->BufferLabel = FMT1_BufferLabel;
 fadi->BufferStartTime = FMT1_BufferStartTime;
 fadi->BufferDt = FMT1_BufferDt;
 fadi->BufferFrameSize = FMT1_BufferFrameSize;
 fadi->BufferDatatype = FMT1_BufferDatatype;

 fadi->CreateIndexedName = FMT1_CreateIndexedName;
 fadi->RecreateNameAndIndex = FMT1_RecreateNameAndIndex;

 fadi->GetStartTime = FMT1_GetStartTime;
 fadi->GetDt = FMT1_GetDt;
 fadi->GetDataType = FMT1_Get_DataType;
 fadi->GetHeaderSize = FMT1_Get_HeaderSize;

 fadi->SetAppendMode = FMT1_SetAppendMode;
 fadi->SetMetadataInterface = FMT1_SetMetadataInterface;
 fadi->SetVariableInterface = FMT1_SetVariableInterface;
 fadi->SetMetadataValues = FMT1_SetMetadataValues;
 fadi->SetVariableValues = FMT1_SetVariableValues;

 fadi->FillAllVariableValues = FMT1_FillAllVariableValues;
 fadi->FillVariableValues = FMT1_FillVariableValues;
}

Abs_Diskio * FMT1_ConstructFile(fname, accessmode)
 char *fname;
 char accessmode;
{
 Abs_Diskio *adi = (struct Abs_Diskio_Type*) calloc(1, sizeof(struct FMT1_Diskio_Type));

 if(adi != NULL){
	FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *)adi;

        FMT1_Funcinit_Diskio(fadi);

        if(FMT1_OpenFile(adi, fname, accessmode) == -1) {
                 free(adi);
		 return NULL;
        }

        adi->filename = Absff_CopyString(fname);

        FMT1_GatherFileInfo(adi);
 } /* adi != NULL */

 fprintf(stderr, "Successfully opened FMT1 file '%s' in mode '%c' \n", fname, accessmode);
 return adi;
}

void FMT1_GatherFileInfo(adi)
 Abs_Diskio *adi;
{
	FILE *fp; 
	FMT1_Diskio *fadi = (FMT1_Diskio *) adi;

	fp = fadi->fp;

	/** 
		Use the FMT1 native mechanism to fill in adi->nvariables.
		and adi->nglobmetadata
	**/
	if (FMT1_inquire(fp, NULL, &adi->nvariables, &adi->nglobmetadata, NULL) == -1) {
		/** 
			Reading error
		**/
		return;
	} 
	
	if(adi->nvariables > 0){
		register int ivar;
		adi->variablenames = (char **)calloc(adi->nvariables, sizeof(char*));
		for(ivar=0; ivar<adi->nvariables; ivar++){
			adi->variablenames[ivar] = (char*) calloc(MAX_FMT1_NAME, sizeof(char));
			/** Use the FMT1  native mechanism to fill in adi->variablenames[] **/
			if (FMT1_varname(fp, ivar, adi->variablenames[ivar]) == -1){
				free(adi->variablenames[ivar]);
				adi->variablenames[ivar] = NULL;
			}
		}
	}
	if(adi->nglobmetadata > 0){
		register int imetadata;
		adi->globmetadatanames = (char **)calloc(adi->nglobmetadata, sizeof(char*));
		for(imetadata=0; imetadata<adi->nglobmetadata; imetadata++){
			adi->globmetadatanames[imetadata] = (char*) calloc(MAX_FMT1_NAME, sizeof(char));
			/** Use the FMT1  native mechanism to fill in adi->globalmetadatanames[] **/
			if (FMT1_attname(fp, imetadata, adi->globmetadatanames[imetadata]) == -1) {
				free(adi->globmetadatanames[imetadata]);
				adi->globmetadatanames[imetadata] = NULL;
			}
		}
	}
}

Abs_Variable * FMT1_ConstructVar(adi, name, type, shape)
 Abs_Diskio *adi;
 char *name;
 Abs_Type type;
 Abs_Varshape *shape;
{
#define INVALID_VAR -1
	int status = INVALID_VAR;
	Abs_Variable *avi = (struct Abs_Variable_Type *) calloc(1, sizeof(struct FMT1_Variable_Type));
	struct FMT1_Diskio_Type *fadi = (struct FMT1_Diskio_Type *)adi;
  struct FMT1_Variable_Type *favi = (struct FMT1_Variable_Type *)avi;

	if (avi != NULL) {
	
        	FMT1_Funcinit_Variable(favi);
        	avi->adi = adi;

		if (name != NULL)
			status = FMT1_varinq(fadi->fp, name, NULL, NULL, NULL);

		if (status == INVALID_VAR) {
			FMT1_DestructVar(avi);
			free(avi);
			return (NULL);
		}
			

		if (adi->is_writable) {
			/* In FMT1, the size of the domain which is 
			the same as the variable size changes dynamically.
		
			Here we flag the size so the CreateVar() code 
			allocates the vals buffer in chunks.
			*/
			shape->domainsize[0] = -1;
			if ((status = FMT1_CreateVar(avi, name, type, shape)) == -1) {
				/**
				{
					Determine error status and handle it
				}
				**/
				FMT1_DestructVar(avi);		
				free(avi);
				return (NULL);
			} else {
				Absff_SetVariableShape(avi, shape);
				Absff_SetVariableType(avi, type);
			}
		} else {
			/* Read values into memory */
			Abs_Type type;
			fmt1_type ftype;
			int ndims;
			register int idim, ilocalattr;
	
			Abs_Varshape *newshape;

			newshape = (Abs_Varshape *) malloc(1 * sizeof(struct Abs_Variable_Type));

			/* First, inquire */
			FMT1_varinq(fadi->fp, name, &ftype, &ndims, &avi->nlocalmetadata);
			newshape->ndomains = ndims;

			/* Set the shape */
			if(ndims > 0) {
				newshape->domainname = (char **) malloc(ndims * sizeof(char *));	
				newshape->domainsize = (long *) malloc(ndims * sizeof(long));
				newshape->domaintype = (byte *) calloc(ndims, sizeof(byte));
			}			
			
			for(idim = 0; idim < ndims; idim++) {
				newshape->domainname[idim] = (char *) malloc(MAX_FMT1_NAME * sizeof(char));
				FMT1_inquire(fadi->fp, newshape->domainname[idim], NULL, NULL, &newshape->domainsize[0]);
				newshape->domaintype[idim] = DYNAMICSIZECOMPONENT;
			}
			
			Absff_SetVariableShape(avi, newshape);

			/* Set the type */
			type = fmt1_type_To_Abs_Type(ftype);
			Absff_SetVariableType(avi, type);

			/* Cache the values using the FMT1 specific create call*/
			/* 
				Unlike its netcdf counterpart this create call merely
				readies its vals cache by allocating appropriately
				depending on the type and size of the variable 
			*/
		
			FMT1_CreateVar(avi, name, type, newshape); 
			Absff_FreeVariableShape(&newshape);

			/* Cache the local attribs if any */

			if(avi->nlocalmetadata > 0){
        avi->localmetadatanames = (char**) calloc(avi->nlocalmetadata, sizeof(char*));
        for(ilocalattr=0; ilocalattr < avi->nlocalmetadata; ilocalattr++){
        	avi->localmetadatanames[ilocalattr] = (char*) calloc(MAX_FMT1_NAME, sizeof(char));
         	FMT1_attname(fadi->fp, ilocalattr, avi->localmetadatanames[ilocalattr]);

        }
     	}
			
		}
	} 
	
#undef INVALID_VAR
	return avi;
}

Abs_Metadata * FMT1_ConstructGlobalAttr(adi, name, type, size)
	Abs_Diskio *adi;
	char *name;
	Abs_Type type;
	long size;
{
#define INVALID_ATT -1
	int status = INVALID_ATT;
	Abs_Metadata *amdi = (struct Abs_Metadata_Type *) calloc(1, sizeof(struct FMT1_Metadata_Type)); /* amdi->avi = NULL happens here */
	struct FMT1_Diskio_Type *fadi = (struct FMT1_Diskio_Type *)adi;
	struct FMT1_Metadata_Type *famdi = (struct FMT1_Metadata_Type *)amdi;
	
	if(amdi != NULL){
		FMT1_Funcinit_Metadata(famdi);
		amdi->adi = adi;

		/* 
		This returns -1 if the name of the attribute is not 
		one of the standard ones for FMT1
		*/
		if (name != NULL)
			status = FMT1_attinq(fadi->fp, name, NULL, NULL);

		if(status == INVALID_ATT){
			famdi->propmodified = 0;
			FMT1_DestructAttr(amdi);
			free(amdi);
			return (NULL);
		} 


		if (adi->is_writable) {
			if((status=FMT1_CreateAttr(famdi, type, size)) != -1){
			} else{
				/** {
					Determine  error status and handle it
				} **/
				famdi->propmodified = 0;
				FMT1_DestructAttr(amdi);
				free(amdi);	
				return (NULL);
			}
		} else {
			/* Read values into memory */	
			 long size;
                	Abs_Type type;
                	fmt1_type ftype;
                	void *values;

                	int accesstoggled;

                	if (FMT1_attinq(fadi->fp, name, &ftype, (int *)&size) == -1) {
				famdi->propmodified = 0;
				FMT1_DestructAttr(amdi);
				free(amdi);
				return NULL;
			}

                	/*
                	KLUDGE: The writable flag is toggled and untoggled to
                	allow the metadata methods cache the size, type and values
                	if the file accessmode is read-only
                	*/
                	accesstoggled = 0;
                	if(!fadi->is_writable){
                        	fadi->is_writable = !fadi->is_writable;
                        	accesstoggled = 1;
			}

                	Absff_SetMetadataSize(amdi, size);
                	type = fmt1_type_To_Abs_Type(ftype);
                	Absff_SetMetadataType(amdi, type);
                	values = Absff_AllocDefaultValues(type, size);
                	FMT1_attget(fadi->fp, name, ftype, values);
                	Absff_SetMetadataValues(amdi, values, 0, size);
                	famdi->propmodified = 0; /* All the sets are associated with
                                        a read. So no modification */
                	if(values != NULL) free(values);

                	if(accesstoggled)
                        	fadi->is_writable = !fadi->is_writable;

		}
	} /* amdi != NULL */

#undef INVALID_ATT
	return amdi;
}


int FMT1_OpenFile(adi, fname, accessmode)
	Abs_Diskio *adi;
	char *fname;
	char accessmode;
{
	char mode[2];  
	FMT1_Diskio *fadi = (FMT1_Diskio *) adi;				

	mode[0] = accessmode;
	mode[1] = '\0';

	switch(accessmode) {

		case 'r': 
			if ((fadi->fp = fopen(fname, mode)) == NULL) {
				/*
                          ** HACK **
                          A small hack to preserve BC with genesis disk_in
                          A number of scripts just create the object and set
                          the filename later even in read mode - So this
                          just attempts to create the file with the name
                          (usually the name of the object) if it does not exist
                          Does not break anything that way - But print an
                          error message to that effect right here.
                        */
				if ((fadi->fp = fopen(fname, "a+")) != NULL) 
					fprintf(stderr, "File '%s' not found or has no data in it\n", fname);
				else {
					fprintf(stderr, "File '%s' does not seem to be a FMT1 file \n", fname);
					return -1;
				}
			}

			if (! FMT1_CheckHeader(fadi->fp)) {
				fprintf(stderr, "File '%s' does not seem to be a FMT1 file \n", fname);
				fclose(fadi->fp);
				return -1;
			}

			adi->is_open = 1;
			adi->is_writable = 0;
		break;


		case 'w':
			if ((fadi->fp = fopen(fname, "w+")) == NULL)
				return 1;
			adi->is_open = 1;
			adi->is_writable = 1;
		break;
	
		case 'a':
			if(adi->is_open && adi->is_writable)
				return 1;	
			if((fadi->fp = fopen(fname, "a+")) == NULL) 
				return -1;

			adi->is_open = 1;
			adi->is_writable = 1;

		break;

		default:
			/** Error: Unknown accessmode */
			return -1;
			/* NOTREACHED */
		break;

	}

	fadi->writeposition = ftell(fadi->fp);
		
	return 1;
}

int FMT1_SetFile(adi, filename, accessmode)
 Abs_Diskio *adi;
 char *filename;
 char accessmode;
{
 FMT1_Diskio * fadi = (FMT1_Diskio *) adi;

 if (adi->is_open) {
        if (FMT1_DestructFile(adi) == -1)
                return -1;
        else {
                adi->is_open = 0;
        }
 }
 if (FMT1_OpenFile(adi, filename, accessmode) != -1) {
        adi->filename = Absff_CopyString(filename);
        FMT1_GatherFileInfo(adi);
        return 1;
 }
 else
        return -1;

}


char * FMT1_CreateIndexedName(adi, client_name, client_index) 
	Abs_Diskio *adi;
	char *client_name;
	int client_index;
{
 char *buf;
 char locbuf[10];
 int namesize;

 if(client_name == NULL) return NULL;
 
 namesize = strlen(client_name) + 1;
 buf = (char *) malloc(sizeof(char) * namesize);
 strcpy(buf, client_name);
 if(client_index != 0) {
        namesize += (strlen("-_")+1);
        buf = (char *) realloc(buf, namesize);
        strcat(buf, "-_");

        sprintf(locbuf, "%d", client_index);
        namesize += (strlen(locbuf)+1);
        buf = (char *) realloc(buf, namesize);
        strcat(buf, locbuf);
 }
 return buf;

}

char * FMT1_RecreateNameAndIndex(adi, nameinfile, index_val) 
	Abs_Diskio *adi;
	char *nameinfile;
	int *index_val;
{
	char* client_name;
	char index_part[10];
	char *ptr;

	if(nameinfile == NULL || index_val == NULL) return NULL;

	ptr = strstr(nameinfile,  "-_");
	if(ptr != NULL){
			*ptr = '\0'; client_name = Absff_CopyString(nameinfile);
			ptr+=2; /* go past the "-_" */
			if(ptr != NULL) {
				strcpy(index_part, ptr);
				*index_val = atoi(index_part);
			}
			else
				*index_val = 0;
 	}
	else{
		client_name = Absff_CopyString(nameinfile);
		*index_val = 0;
	}
	return client_name;
}
	

int FMT1_ResetDataOffset(adi)
 Abs_Diskio  *adi;
{
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *)adi;

 fadi->flush_frame_offset = 0;
 return 1; 
}

int FMT1_ResetFile(adi)
 Abs_Diskio *adi;
{
 register int i;
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *)adi;

 if (!fadi->is_writable)
	return 1;  

 fadi->writeposition = 0;

 /*

 for (i=0; i < fadi->framesize; i++) {
	Abs_Varshape *shape = NULL;

	if (fadi->autoSAVE_favi[i] != NULL)
		shape = FMT1_GetVarShape((Abs_Variable *)fadi->autoSAVE_favi[i]);
	if (shape != NULL)
		shape->domainsize[0] = (long) 0;	
 }

 */

 return 1;
}

int FMT1_CloseFile(adi)
 Abs_Diskio *adi;
{
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;

 fclose(fadi->fp);
 fadi->fp = NULL;
 fadi->is_open = 0; 
	
 return 1;
}

int FMT1_DestructChildInterfaces(adi)
 Abs_Diskio *adi;
{
 register int i;
 int framesize;
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;

 if(fadi == NULL) 
	return -1;
 
 framesize = FMT1_Get_FrameSize(adi);


 if (fadi->autoSAVE_favi != NULL) {
	for (i=0; i<framesize; i++) {
		if (fadi->autoSAVE_favi[i] != NULL) {
			FMT1_DestructVar((Abs_Variable *)fadi->autoSAVE_favi[i]);
			free(fadi->autoSAVE_favi[i]);
		}
	}
	free(fadi->autoSAVE_favi);
	fadi->autoSAVE_favi = NULL;
 }

 if (fadi->autoSAVE_famdi != NULL) {
	for (i=0; i<framesize; i++) {
		if (fadi->autoSAVE_famdi[i] != NULL) {
			FMT1_DestructAttr((Abs_Metadata *)fadi->autoSAVE_famdi[i]);
			free(fadi->autoSAVE_famdi[i]);
		}
	}
	free(fadi->autoSAVE_famdi);
	fadi->autoSAVE_famdi = NULL;
 }

 if (fadi->framevals != NULL) {
	free(fadi->framevals);
	fadi->framevals = NULL;
 }

 return 1;

}


int FMT1_DestructFile(adi)
 Abs_Diskio *adi;
{
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;

 fclose(fadi->fp);
 fadi->fp = NULL;

 if (fadi->label != NULL) {
	Absff_FreeString(fadi->label);
	fadi->label = NULL;
 }

 adi->is_open = 0;

 return Absff_DestructDiskio(adi);

}

int FMT1_FlushFile(adi, dataoffset)
 Abs_Diskio *adi;
 int dataoffset;
{

 int framesize, nframes;
 long frame_offset[1], count[1];
 register int i, j;
 float *floatvals;
 FMT1_Variable *favi;
 FMT1_Metadata *famdi;

 FMT1_Diskio *fadi = (FMT1_Diskio *)adi;
 if(fadi == NULL || !fadi->is_writable) 
	return -1;

 framesize = fadi->framesize;


 if (!fadi->is_appendmode || (fadi->is_appendmode && !FMT1_ReadHeader(fadi->fp))) {
	char label[100];
 	fadi->writeposition = 0;
	fadi->flush_frame_offset = 0;

	strcpy(label, fadi->label);
 	FMT1_WriteHeader(adi, label, sizeof(char), 80);
 	FMT1_WriteHeader(adi, &fadi->starttime, sizeof(float), 1);
 	FMT1_WriteHeader(adi, &fadi->dt, sizeof(float), 1);
 	FMT1_WriteHeader(adi, &framesize, sizeof(int), 1);
 	FMT1_WriteHeader(adi, &fadi->datatype, sizeof(int), 1);

 /* Write out the metadata values first */
 	if (fadi->autoSAVE_famdi != NULL) {
 		for (i= 0; i<framesize; i++) {
			famdi = fadi->autoSAVE_famdi[i];
			famdi->propmodified = 1;
			if (famdi != NULL)
				FMT1_FlushMetadataValues(famdi);
 		}
 	}
	
 }
 /* Set the works to write variables frame by frame */
 if (fadi->autoSAVE_favi != NULL) {
 	if (fadi->framevals == NULL) {
		/* should do a switch on datatype here */
		if ((fadi->framevals = (float *) malloc(framesize * sizeof(float))) == NULL) {
			/* Error */
			return -1; 
		}
 	}

	if (fadi->autoSAVE_favi[0] != NULL) {	
		Abs_Varshape *shape = FMT1_GetVarShape((Abs_Variable*) fadi->autoSAVE_favi[0]);
 		nframes = shape->domainsize[0];
	}
	else 
		nframes = 0;
 
 	count[0] = (long) 1;
	floatvals = (float *) fadi->framevals;	
 	for (i=fadi->flush_frame_offset; i<nframes; i++) {
		frame_offset[0] = i;
		for (j=0; j<framesize; j++) {
			favi = fadi->autoSAVE_favi[j];
			if (favi != NULL) 
				FMT1_GetVarValues((Abs_Variable *)favi, &floatvals[j], frame_offset, count); 
		}
		/* should do a switch on datatype here */
		fseek(fadi->fp, (long)fadi->writeposition, 0);
		if (fwrite(floatvals, sizeof(float), framesize, fadi->fp) == 0)  {
			clearerr(fadi->fp);
			return -1;  
		}
		else
			fadi->writeposition += sizeof(float) * framesize;
 	}

	if (i == nframes)
		fadi->flush_frame_offset = nframes;
	else
		return -1;
 }

 if (fflush(fadi->fp) != 0)
	return -1;

 return 1;

}

int FMT1_IsCoordVar(adi, varname)
 Abs_Diskio *adi;
 char *varname;
{
 return 0;	
}

Abs_Variable * FMT1_ConstructCoordVar(adi, coordname)
 Abs_Diskio *adi;
 char *coordname;
{
	return NULL;
}


int FMT1_BufferLabel(adi, label)
 Abs_Diskio *adi;
 char *label;
{
	FMT1_Diskio * fadi = (struct FMT1_Diskio_Type *) adi;

	if (label == NULL || fadi == NULL)
		return -1;

	if (fadi->label != NULL)
		Absff_FreeString(fadi->label);
	fadi->label = Absff_CopyString(label);

	return 1;
}

int FMT1_BufferStartTime(adi, starttime)
 Abs_Diskio *adi;
 float starttime;
{
	FMT1_Diskio * fadi = (struct FMT1_Diskio_Type *) adi;

	if (fadi == NULL)
		return -1;

	fadi->starttime = starttime;
	return 1;
}

int FMT1_BufferDt(adi, dt)
 Abs_Diskio *adi;
 float dt;
{
	FMT1_Diskio * fadi = (struct FMT1_Diskio_Type *) adi;

	if (fadi == NULL)
		return -1;

	fadi->dt = dt;
	return 1;
}

int FMT1_BufferFrameSize(adi, framesize)
 Abs_Diskio *adi;
 int framesize;
{
	FMT1_Diskio * fadi = (struct FMT1_Diskio_Type *) adi;

	if (fadi == NULL)
		return -1;

	fadi->framesize = framesize;
	return 1;
}


int FMT1_BufferDatatype(adi, datatype)
 Abs_Diskio *adi;
 int datatype;
{
	FMT1_Diskio * fadi = (struct FMT1_Diskio_Type *) adi;

	if (fadi == NULL)
		return -1;

	fadi->datatype = datatype;
	return 1;
}
 



int FMT1_Get_FrameSize(adi)
 Abs_Diskio *adi;
{
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;

 if (fadi == NULL)
	return -1;

 if (fadi->is_writable)
	return fadi->framesize;

 return FMT1_GetFrameSize(fadi->fp);

}

float FMT1_GetStartTime(adi)
 Abs_Diskio *adi;
{
 float start_time;
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;
 if (fadi == NULL)
	return (float) -1;

 if (fadi->is_writable)
	return fadi->starttime;

 if (fadi->fp == NULL)
	return (float) -1;

 if (fseek(fadi->fp, (long) (80), 0) == -1) {
	return (float) -1;
 }
 
 if (fread(&start_time, sizeof(float), 1, fadi->fp) == 0) {
	clearerr(fadi->fp);
	return (float) -1;
 }
	
 return start_time;
}

float FMT1_GetDt(adi)
 Abs_Diskio *adi;
{
 float dt;
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;

 if (fadi == NULL)
	return (float) -1;

 if (fadi->is_writable)
	return fadi->dt;

 if (fadi->fp == NULL)
	return (float) -1;

 if (fseek(fadi->fp, (long) (sizeof(float) + 80), 0) == -1) {
	return (float) -1;
 }
 
 if (fread(&dt, sizeof(float), 1, fadi->fp) == 0) {
	clearerr(fadi->fp);
	return (float) -1;
 }
	
 return dt;
}

int FMT1_Get_HeaderSize(adi)
 Abs_Diskio *adi;
{
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;
 
 if (fadi == NULL)	
	return -1;


 if (fadi->is_writable)
	return (2 * sizeof(float) + 2 * sizeof(int) + 80 +
			fadi->framesize * 3 * sizeof(float));

 return FMT1_GetHeaderSize(fadi->fp);
}

int FMT1_Get_DataType(adi)
 Abs_Diskio *adi;
{
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;
 
 if (fadi == NULL)	
	return -1;

 if (fadi->is_writable) 
	return fadi->datatype;

 return FMT1_GetDataType(fadi->fp);
}

int FMT1_SetAppendMode(adi, appendmode)
 Abs_Diskio *adi;
 short appendmode;
{
 FMT1_Diskio *fadi = (FMT1_Diskio *) adi;

 fadi->is_appendmode = appendmode;

 return 1;

}

int FMT1_SetMetadataInterface(adi, famdi_offset, amdi)
 Abs_Diskio *adi;
 int famdi_offset;
 Abs_Metadata *amdi;
{
 FMT1_Diskio *fadi = (FMT1_Diskio *) adi;
 FMT1_Metadata *famdi = (FMT1_Metadata *) amdi;

 if (fadi == NULL ||
	famdi == NULL)
	return -1; 

 if (fadi->autoSAVE_famdi == NULL) {
 	int framesize = FMT1_Get_FrameSize(adi);
	if ((fadi->autoSAVE_famdi = (FMT1_Metadata **) malloc (framesize * sizeof(struct FMT1_Metadata_Type *))) == NULL) {
	 fprintf(stderr, "Cannot allocate frame of size %d\n", framesize);
	 return -1;	
	}
 }

 fadi->autoSAVE_famdi[famdi_offset] = famdi;
 return 1;
}

int FMT1_SetMetadataValues(adi, famdi_offset, vals, start, count) 
 Abs_Diskio *adi;
 int famdi_offset;
 void *vals;
 long start;
 long count;
{
 FMT1_Metadata *famdi;
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;
 
 if (fadi == NULL ||
	fadi->autoSAVE_famdi == NULL)	
	return -1;

 famdi = fadi->autoSAVE_famdi[famdi_offset];
 
 if(famdi == NULL)
	return -1;
 
 return FMT1_SetAttrValues((Abs_Metadata *)famdi, vals, start, count);
}

int FMT1_SetVariableInterface(adi, favi_offset, avi)
 Abs_Diskio *adi;
 int favi_offset;
 Abs_Variable *avi;
{
 FMT1_Diskio *fadi = (FMT1_Diskio *) adi;
 FMT1_Variable *favi = (FMT1_Variable *) avi;

 if (fadi == NULL ||
	favi == NULL)
	return -1; 


 if (fadi->autoSAVE_favi == NULL) {
 	int framesize = FMT1_Get_FrameSize(adi);
	if ((fadi->autoSAVE_favi = (FMT1_Variable **) calloc (framesize, sizeof(struct FMT1_Variable_Type *))) == NULL) {
	 fprintf(stderr, "Cannot allocate frame of size %d\n", framesize);
	 return -1;	
	}
 }
 

 fadi->autoSAVE_favi[favi_offset] = favi;
 return 1;
}

int FMT1_SetVariableValues(adi, favi_offset, vals, start, count) 
 Abs_Diskio *adi;
 int favi_offset;
 void *vals;
 long start[];
 long count[];
{
 FMT1_Variable *favi;
 FMT1_Diskio *fadi = (struct FMT1_Diskio_Type *) adi;
 
 if (fadi == NULL ||
	fadi->autoSAVE_favi == NULL)	
	return -1;

 favi = fadi->autoSAVE_favi[favi_offset];
 
 if(favi == NULL)
	return -1;

 return FMT1_SetVarValues((Abs_Variable *)favi, vals, start, count);
}

int FMT1_FillAllVariableValues(adi)
 Abs_Diskio *adi;
{
  Abs_Varshape *shape;
  int header_size;
  long start_index[1], count[1];
  register int i, j;
  float *floatvals;
  FMT1_Diskio *fadi = (FMT1_Diskio *) adi;

  int framesize = FMT1_Get_FrameSize(adi);	
	
  /* Allocate fadi->framevals if necessary*/

  if (fadi->framevals == NULL)
  	fadi->framevals = (float *) malloc(framesize * sizeof(float));

  floatvals = (float *) fadi->framevals;

  if (fadi->autoSAVE_favi == NULL 
	|| fadi->autoSAVE_favi[0] == NULL)
	return -1;

  header_size = FMT1_Get_HeaderSize(adi);

  count[0] = (long) 1;
  shape = FMT1_GetVarShape(fadi->autoSAVE_favi[0]);

  for (i=0 ; i<shape->domainsize[0]; i++) {
  	start_index[0] = (long)i;
	if (fseek(fadi->fp, (long) (header_size + i*framesize*sizeof(float)), 0) == -1)
		return -1;
  	if (fread(floatvals, sizeof(float), framesize, fadi->fp) == 0) {
		fprintf(stderr, "Error while reading. Number read = %d\n", framesize);
		clearerr(fadi->fp);
		return -1;
	}
  	for (j=0; j<framesize; j++)
		FMT1_SetVarValues(fadi->autoSAVE_favi[j], &floatvals[j], start_index, count);
  }
  
  return 1;
}

int FMT1_FillVariableValues(adi, favi_offset)
 Abs_Diskio *adi;
 int favi_offset;
{
  Abs_Varshape *shape;
  int header_size;
  register int i;
  long start_index[1], count[1];
  float *floatvals;
  FMT1_Diskio *fadi = (FMT1_Diskio *) adi;

  int framesize = FMT1_Get_FrameSize(adi);	
	
  /* Allocate fadi->framevals if necessary*/

  if (fadi->framevals == NULL)
  	fadi->framevals = (float *) malloc(1 * sizeof(float));

  floatvals = (float *) fadi->framevals;

  if (fadi->autoSAVE_favi == NULL 
	|| fadi->autoSAVE_favi[favi_offset] == NULL)
	return -1;

  header_size = FMT1_Get_HeaderSize(adi);

  shape = FMT1_GetVarShape(fadi->autoSAVE_favi[favi_offset]);
  count[0] = (long) shape->domainsize[0];

  for (i=0 ; i<count[0]; i++) {
	long nval[1];
	nval[0] = (long) 1;

  	start_index[0] = (long)i;
	if (fseek(fadi->fp, (long) (header_size + (i*framesize+favi_offset)*sizeof(float)), 0) == -1)
		return -1;
  	if (fread(floatvals, sizeof(float), 1, fadi->fp) == 0) {
		fprintf(stderr, "Error while reading. Number read = %d\n", framesize);
		clearerr(fadi->fp);
		return -1;
	}
	FMT1_SetVarValues(fadi->autoSAVE_favi[favi_offset], &floatvals[0], start_index, nval);
  }
  
  return 1;
 
}

 int FMT1_WriteHeader(adi, header, bytesize, nbytes) 
 Abs_Diskio *adi;
 void *header;
 int bytesize;
 int nbytes;
{
 FMT1_Diskio *fadi = (FMT1_Diskio *)adi;

 fseek(fadi->fp, (long)fadi->writeposition, 0L);

 if (fwrite(header, bytesize, nbytes, fadi->fp) != nbytes) {
	clearerr(fadi->fp);
  	return -1;
 }
 
 fadi->writeposition += nbytes * bytesize;
 return 1;
}

int FMT1_NOP(adi)
 Abs_Diskio *adi;
{
 return 1;
}

