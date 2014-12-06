/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: netcdf_diskio.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: netcdf_diskio.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 05:07:02  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.2  1999/10/16 22:47:25  mhucka
 * Merged changes from Upi for supporting netCDF 3.4.
 *
 * Revision 1.1  1999/10/16 21:17:07  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  This version of diskio by
 * Venkat implemented an abstract file interface and placed the netcdf_*
 * files into the subdirectory interface/netcdf/, instead of in
 * interface/ as in the version that I started with.  So I've moved the
 * files into interface/netcdf/ now.
 *
 * Revision 1.4  1998/01/14 19:06:14  venkat
 * Definition of the netcdf subclass isA() method and the concrete
 * implementations of the new methods in the Abs_Diskio structure.
 *
 * Revision 1.3  1997/08/04 04:19:56  dhb
 * Include of strings.h doesn't work under Solaris.  Including
 * string.h should suffice.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "netcdf_ext.h"

extern int ncopts;

static int Netcdf_HelpConstructVar(); /* Helper for the variable interface constructor */

/* This is defined in accordance with the defined base interface class 
	data types (i.e. enum Abs_Type), and thus provides a one-one mapping */
nc_type netcdf_type[14] = {
	NC_BYTE,
	NC_BYTE,
	NC_INVALID,
	NC_SHORT,
	NC_INVALID,
	NC_LONG,
	NC_INVALID,
	NC_INVALID,
	NC_FLOAT,
	NC_DOUBLE,
	NC_INVALID,
	NC_CHAR,
	NC_INVALID,
	NC_INVALID
};	

/* Class methods */
 
int Netcdf_IsA(filename) 
 char *filename;
{
 /**************************************************************
	This IsA() method is called only when an existing file is
	to be deduced for format info. Since scripts in genesis
	can create a diskio object for reading without specifying 
	a filename in the create command line, we adopt the same 
	technique as in the constructor to non-destructively open the
	file. 
	
	If the file does not exist then nccreate with the NC_NOCLOBBER 
	mode will succeed thereby making sure that an error condition
	indicating the failure of creation of the element is not 
	returned.
	
	If the file exists, then NO_NOCLOBBER mode will fail in the 
	creation claiming that the file is not a netcdf file. So in
	order that a subsequent call to the function does not fail
	we safely remove the dummy file once created using the 
	system() call.
	
 *************************************************************/
	
 ncopts = 0;

 if (ncopen(filename, NC_NOWRITE) == -1)  {
 	if (nccreate(filename, NC_NOCLOBBER) != -1) {
		char cmd[100];
		fprintf(stderr, "File '%s' not found or has no data in it\n", filename);
		sprintf(cmd, "/bin/rm -f %s", filename);	
		system(cmd);
		return 1;
	} else {
		fprintf(stderr, "File '%s' does not seem to be a netcdf file\n", filename);
		return 0;
	}
 }
 else
	return 1;
}

void Netcdf_Funcinit_Diskio(nadi)
 Netcdf_Diskio *nadi;
{
 nadi->ConstructMetadata_Interface = Netcdf_ConstructGlobalAttr;
 nadi->ConstructVariable_Interface = Netcdf_ConstructVar;
 nadi->Flush =	 Netcdf_FlushFile;
 nadi->FreeChildInterface = Netcdf_FreeChildInterface;
 nadi->DestructChildInterfaces = Netcdf_NOP;
 nadi->Destruct = Netcdf_DestructFile;

 nadi->IsCoordVariable = Netcdf_IsCoordVar;
 nadi->ConstructCoord_Interface = Netcdf_ConstructCoordVar;

 nadi->SetFile = Netcdf_SetFile;
 nadi->ResetFile = Netcdf_NOP;
 nadi->OpenFile = Netcdf_OpenFile;
 nadi->CloseFile = Netcdf_CloseFile;
 nadi->GatherInfo = Netcdf_GatherFileInfo;
 nadi->SetAppendMode = Netcdf_NOP;
 nadi->ResetDataOffset = Netcdf_NOP;

 nadi->WriteHeader = Netcdf_NOP;
 nadi->BufferLabel = Netcdf_NOP;
 nadi->BufferStartTime = Netcdf_NOP;
 nadi->BufferDt = Netcdf_NOP;
 nadi->BufferFrameSize = Netcdf_NOP;
 nadi->BufferDatatype = Netcdf_NOP;
 nadi->GetStartTime = Netcdf_GetStartTime;
 nadi->GetDt = Netcdf_GetDt;
 nadi->GetDataType = Netcdf_GetDataType;
 nadi->GetHeaderSize = Netcdf_GetHeaderSize;

 nadi->SetMetadataInterface = Netcdf_NOP;
 nadi->SetMetadataValues = Netcdf_NOP;

 nadi->SetVariableInterface = Netcdf_NOP;
 nadi->SetVariableValues = Netcdf_NOP;
 nadi->FillAllVariableValues = Netcdf_NOP;
 nadi->FillVariableValues = Netcdf_NOP;

 nadi->CreateIndexedName = Netcdf_CreateIndexedName;
 nadi->RecreateNameAndIndex = Netcdf_RecreateNameAndIndex;
}

int Netcdf_NOP(adi)
 Abs_Diskio *adi;
{
 return 1;
}



Abs_Diskio * Netcdf_ConstructFile(fname, accessmode)
 char *fname;
 char accessmode;
{
 Abs_Diskio *adi = (struct Abs_Diskio_Type*) calloc(1, sizeof(struct Netcdf_Diskio_Type));
 
 if(adi != NULL){
	Netcdf_Diskio *nadi = (struct Netcdf_Diskio_Type *)adi;
	
	Netcdf_Funcinit_Diskio(nadi);
	ncopts = 0;

	if(Netcdf_OpenFile(adi, fname, accessmode) == -1) {
		 free(adi);
		 return (NULL);
	}

	adi->filename = Absff_CopyString(fname);
	
	Netcdf_GatherFileInfo(adi);
			
 } /* adi != NULL */
 fprintf(stderr, "Successfully opened netcdf file '%s' in mode '%c' \n", fname, accessmode);
 return adi;
}

void Netcdf_GatherFileInfo(adi)
 Abs_Diskio *adi;
{
 int fid; 
 Netcdf_Diskio *nadi = (Netcdf_Diskio *) adi;

	fid = nadi->file_id;

	ncinquire(fid, NULL, &adi->nvariables, &adi->nglobmetadata, NULL);		
	if(adi->nvariables > 0){
		register int ivar;
		adi->variablenames = (char **)calloc(adi->nvariables, sizeof(char*));
		for(ivar=0; ivar<adi->nvariables; ivar++){
			adi->variablenames[ivar] = (char*) calloc(MAX_NC_NAME, sizeof(char));
			ncvarinq(fid, ivar, adi->variablenames[ivar], NULL, NULL, NULL, NULL);
		}
	}
	if(adi->nglobmetadata > 0){
		register int imetadata;
		adi->globmetadatanames = (char **)calloc(adi->nglobmetadata, sizeof(char*));
		for(imetadata=0; imetadata<adi->nglobmetadata; imetadata++){
			adi->globmetadatanames[imetadata] = (char*) calloc(MAX_NC_NAME, sizeof(char));
			ncattname(fid, NC_GLOBAL, imetadata,adi->globmetadatanames[imetadata]);
		}
	}
				
}

float Netcdf_GetStartTime(adi)
 Abs_Diskio *adi;
{
 return 0.0;
}

float  Netcdf_GetDt(adi)
 Abs_Diskio *adi;
{
 return 1.0;
}

int Netcdf_GetDataType(adi)
  Abs_Diskio *adi;
{
 return 4; /* This is FLOAT in genesis */
}

int Netcdf_GetHeaderSize(adi)
   Abs_Diskio *adi;
{
 return 0; 
}



Abs_Metadata *	Netcdf_ConstructGlobalAttr(adi, name, type, size)
 Abs_Diskio *adi;
 char *name;
 Abs_Type type;
 long size;
{
  Abs_Metadata *amdi = (struct Abs_Metadata_Type *) calloc(1, sizeof(struct Netcdf_Metadata_Type)); /* amdi->avi = NULL happens here*/ 

 if(amdi != NULL){
#define NO_ATT -1
	int status = NO_ATT;
	struct Netcdf_Diskio_Type *nadi = (struct Netcdf_Diskio_Type *)adi;
	struct Netcdf_Metadata_Type *namdi = (struct Netcdf_Metadata_Type *)amdi;
	Netcdf_Funcinit_Metadata(namdi);
	amdi->adi = adi; 

	if (name != NULL)
		status = ncattinq(nadi->file_id, NC_GLOBAL, name, NULL, NULL);
	if(status == NO_ATT){
		if(!adi->is_writable) {
			/** Error: Not writable **/
			return (NULL);	
		}
		else{
			if((status=Netcdf_CreateAttr(amdi, type, size)) != -1){
				namdi->attr_name = Absff_CopyString(name);
			}
			else{
				/** {
				Determine  error status and handle it 
				} **/
				/** Destruction will be handled by code that 
					constructs **/
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
		ncattinq(nadi->file_id, NC_GLOBAL, namdi->attr_name, &ntype, (int *)&size);  

		/* 
		KLUDGE: The writable flag is toggled and untoggled to 
		allow the metadata methods cache the size, type and values 
		if the file accessmode is read-only
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
		ncattget(nadi->file_id, NC_GLOBAL, namdi->attr_name, values);
		Absff_SetMetadataValues(amdi, values, 0, size);
		namdi->propmodified = 0; /* All the sets are associated with
					a read. So no modification */
		if(values != NULL) free(values);

		if(accesstoggled)
			nadi->is_writable = !nadi->is_writable;
	}
#undef NO_ATT
 } /* amdi != NULL */

	return (amdi);
}


Abs_Variable  *	Netcdf_ConstructVar(adi, name, type, shape)
 Abs_Diskio *adi;
 char *name;
 Abs_Type type;
 Abs_Varshape *shape;
{
  int stat;
  Abs_Variable *avi = (struct Abs_Variable_Type*) calloc(1, sizeof(struct Netcdf_Variable_Type));

  if(avi != NULL)
	stat = Netcdf_HelpConstructVar(adi, avi, name, type, shape); 

  if(stat == -1){ 
  	Netcdf_DestructVar(avi);	
  	free(avi);
  	return (NULL);
  }
  return (avi);
}

static int Netcdf_HelpConstructVar(adi, avi, name, type, shape)
 Abs_Diskio *adi;
 Abs_Variable *avi;
 char *name;
 Abs_Type type;
 Abs_Varshape *shape;
{	
#define NO_VAR -1
	int status = NO_VAR;
	struct Netcdf_Diskio_Type *nadi = (struct Netcdf_Diskio_Type *)adi;
	struct Netcdf_Variable_Type *navi = (struct Netcdf_Variable_Type *)avi;
	Netcdf_Funcinit_Variable(navi);
	avi->adi = adi; 

	if (name != NULL)
		status = ncvarid(nadi->file_id, name);
	if(status == NO_VAR){
		if(!adi->is_writable) {
			{
			/** Error: Not writable **/
			}
			return -1;
		}
		else{
			if((status=Netcdf_CreateVar(avi, name, type, shape)) != -1){
				Absff_SetVariableShape(avi, shape);
				Absff_SetVariableType(avi, type);
				navi->var_id = status;
			}
			else{
				{
				/** Determine  error status and handle it **/
				}
				return -1;
			}
		}
	} /* status = NO_VAR */
	else {
		register int idim,i;
		register int ilocalattr;
		int ndims, size, unlimid;
		int dimids[MAX_VAR_DIMS];
		Abs_Type type;
		nc_type ntype;
		Abs_Varshape *shape;

		navi->var_id = status;

		/* cache  the dimension names & sizes (Varshape), the type, and local attr*/ 
 		shape = (Abs_Varshape *) malloc(1 * sizeof(struct Abs_Varshape_Type));

	{
		/* First inquire */
		ncinquire(nadi->file_id, NULL, NULL, NULL, &unlimid);
		ncvarinq(nadi->file_id, navi->var_id, NULL, &ntype, &shape->ndomains, dimids, &avi->nlocalmetadata);
	}

	{
		/* Cache the shape */

		ndims = shape->ndomains;
		if(ndims > 0){
			shape->domainname = (char **) malloc(ndims *  sizeof(char*));
			shape->domainsize = (long *) malloc(ndims * sizeof(long));
			shape->domaintype = (byte *) calloc(ndims, sizeof(byte));
		}
		for(idim =0; idim < ndims; idim++){
			shape->domainname[idim] = (char *) malloc(MAX_NC_NAME * sizeof(char));
			ncdiminq(nadi->file_id, dimids[idim], shape->domainname[idim], &shape->domainsize[idim]);
			if(unlimid == dimids[idim])
				shape->domaintype[idim] = DYNAMICSIZECOMPONENT;
		}
		Absff_SetVariableShape(avi, shape);
	}

		Absff_FreeVariableShape(&shape);

	{
		/* Cache the type */
		type = nc_type_To_Abs_Type(ntype);
		Absff_SetVariableType(avi, type);
	}

	{	
		/* cache the local attribute names */
		if(avi->nlocalmetadata > 0){
			avi->localmetadatanames = (char**) calloc(avi->nlocalmetadata, sizeof(char*));
		
			for(ilocalattr=0; ilocalattr < navi->nlocalmetadata; ilocalattr++){
				avi->localmetadatanames[ilocalattr] = (char*) calloc(MAX_NC_NAME, sizeof(char));
				ncattname(nadi->file_id, navi->var_id, ilocalattr, avi->localmetadatanames[ilocalattr]);	
			
			}	
		} 
	}


	}/* status != NO_VAR */
#undef NO_VAR
	return 1;
} 

int Netcdf_FlushFile(adi)
 Abs_Diskio *adi; 
{
 int fid;
 
 Netcdf_Diskio *nadi = (Netcdf_Diskio*)adi;
 if(nadi == NULL) return -1;
 fid = nadi->file_id; 

 if(!nadi->datamode){
	ncendef(fid);
	nadi->datamode = !nadi->datamode;
 }

 return ncsync(fid);
}

int Netcdf_CloseFile(adi)
 Abs_Diskio *adi;
{
 Netcdf_Diskio *nadi = (struct Netcdf_Diskio_Type *) adi;
 if (nadi == NULL)
	return -1;
 return ncclose(nadi->file_id);
}

int Netcdf_FreeChildInterface(adi, xface)
 Abs_Diskio *adi;
 void *xface;
{
 if (xface != NULL)
 	free(xface);
 return 1;
}

int Netcdf_DestructFile(adi)
 Abs_Diskio *adi;
{
 int status;

 Netcdf_Diskio *nadi = (struct Netcdf_Diskio_Type *) adi;
  
 status = ncclose(nadi->file_id);

 if(status == -1) return -1;

 return Absff_DestructDiskio(adi);
}


int Netcdf_IsCoordVar(adi, varname)
 Abs_Diskio *adi;
 char *varname;
{
 int ndims, fid;
 register int i;
 char dimname[MAX_NC_NAME];

 Netcdf_Diskio * nadi = (Netcdf_Diskio *)adi;

 if(nadi == NULL || varname == NULL){
	/* Error */
	return 0;
 }
 fid = nadi->file_id;
 ncinquire(fid, &ndims, NULL, NULL, NULL); 
 for(i=0; i<ndims; i++){ 
	ncdiminq(fid, i, dimname, NULL);
	if(strcmp(varname, dimname) == 0) return 1;
 }
 return 0;
}

Abs_Variable * Netcdf_ConstructCoordVar(adi, coordname)
 Abs_Diskio *adi;
 char *coordname;
{
  int stat;
  Abs_Varshape *shape = Absff_ConstructDefaultShape(coordname);
  Abs_Variable *avi = (struct Abs_Variable_Type*) calloc(1, sizeof(struct Netcdf_Variable_Type));
  Abs_Type type = abs_float64;

  if(avi != NULL)
	stat = Netcdf_HelpConstructVar(adi, avi, coordname, type, shape); 

  if(stat == -1){ 
	Absff_DestructDefaultShape(&shape);
  	Netcdf_DestructVar(avi);	
  	free(avi);
  	return (NULL);
  }
  Absff_DestructDefaultShape(&shape);
  return (avi);
}

int Netcdf_SetFile(adi, filename, accessmode)
 Abs_Diskio *adi;
 char *filename;
 char accessmode;
{
 Netcdf_Diskio * nadi = (Netcdf_Diskio *) adi;

 if (adi->is_open) {
	if (Netcdf_DestructFile(adi) == -1)
		return -1;
	else {
		adi->is_open = nadi->is_created = 0;
	}
 }
 if (Netcdf_OpenFile(adi, filename, accessmode) != -1) {
	adi->filename = Absff_CopyString(filename);		
	Netcdf_GatherFileInfo(adi);
	return 1;
 }
 else 
	return -1;
}


int Netcdf_OpenFile(adi, fname, accessmode)
  Abs_Diskio *adi;
  char *fname;
  char accessmode;
{ 
  	int fid; 
  	Netcdf_Diskio *nadi = (Netcdf_Diskio *) adi;
	switch(accessmode){
		case 'r':
			fid = ncopen(fname, NC_NOWRITE);		
			if(fid == -1) {
			/*  
			  ** HACK **
			  A small hack to preserve BC with genesis disk_in 
			  A number of scripts just create the object and set 
			  the filename later even in read mode - So this 
			  just attempts to create the file with the name 
			  (usually the name of the object) if it does not exist
			  Does not break anything that way - But print an 
			  error message to that effect right here.

			Fri Jan  9 15:24:29 PST 1998 - Venkat
				The system() function call to remove the 
				created file (usually the name of the 
				genesis diskio element) allows the 
				recreation when a deduction process
				is trigerred. The code here and in
				the FMT1_OpenFile and in the static isA()
				methods allows for the deduction of 
				the format from an existing file, if
				it is not specified or specified
				incorrectly.
			*/ 
				if ((fid = nccreate(fname, NC_NOCLOBBER)) != -1) {
					char cmd[100];
					fprintf(stderr, "File '%s' not found or has no data in it\n", fname);
					sprintf(cmd, "/bin/rm -f %s", fname);	
					system(cmd);
					nadi->is_created = 1;
				}
				else {
					fprintf(stderr, "File '%s' does not seem to be a netcdf file\n", fname);
					return -1;

				}
			}
			nadi->file_id = fid;
			nadi->datamode = !nadi->is_created;
			nadi->is_created = adi->is_open = 1;
			adi->is_writable = 0;
		break;

		case 'w':
			fid = nccreate(fname, NC_CLOBBER);
			if(fid == -1) {
				 /**Error: Creating file **/
				return -1;
			}	
			else {
				nadi->file_id = fid;
				nadi->datamode = 0;
				nadi->is_created = adi->is_open = 1;
				adi->is_writable  = 1;
			}
		break;

		case 'a':

			fid = ncopen(fname, NC_WRITE);
			if(fid == -1){
				fid=nccreate(fname, NC_CLOBBER);
				if(fid != -1) nadi->is_created = 1; 
			} else 
				nadi->is_created = 0;
			if(fid == -1){
				/**Error: Creating file **/
				return -1;
			}
			nadi->file_id = fid;
			nadi->datamode = !nadi->is_created;
			nadi->is_created = adi->is_open = 1;
			adi->is_writable  = 1;
		break;

		default:
	
			/** Error: Unknown access mode **/
			return -1;
			/* NOTREACHED */
		break;
	}

	return fid;
}

char * Netcdf_CreateIndexedName(adi, client_name, client_index)
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

char * Netcdf_RecreateNameAndIndex(adi, nameinfile, index_val)
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

