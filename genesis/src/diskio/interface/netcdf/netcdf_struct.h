/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: netcdf_struct.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: netcdf_struct.h,v $
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
 * Revision 1.3  1998/01/14 18:36:28  venkat
 * Addition of concrete method declarations that implement the new
 * methods added to the Abs_Diskio structure for concurrent support of
 * FMT1 and netcdf fileformats.
 *
 * Revision 1.2  1997/07/26  00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#ifndef NETCDF_STRUCT_H
#define NETCDF_STRUCT_H

typedef struct Netcdf_Diskio_Type{
	ABS_DISKIO_TYPE 
	/* Netcdf specific members */
	int	file_id;
	short 	is_created;
	short 	datamode; /* 0=define, 1=data */
}Netcdf_Diskio;

typedef struct Netcdf_Metadata_Type{
	ABS_METADATA_TYPE
	/* Netcdf specific members */
	char *attr_name;
	short propmodified;/* Flag to signal whether the destructor should
			write out to the file - for the netcdf attributes*/
}Netcdf_Metadata;

typedef struct Netcdf_Variable_Type{
	ABS_VARIABLE_TYPE
	/* Netcdf specific members */
	int var_id;
}Netcdf_Variable;

extern int 			Netcdf_NOP();

/*Netcdf_Diskio Methods */
extern Abs_Metadata*            Netcdf_ConstructGlobalAttr(/* char *name, Abs_Type type, long size */);
extern Abs_Variable*            Netcdf_ConstructVar(/* char *name, Abs_Type type, Abs_Varshape *shape */);
/*
extern int                      Netcdf_SetAccessmode();
extern int                      Netcdf_GetAccessmode();
*/
extern int 			Netcdf_FlushFile();
extern int			Netcdf_FreeChildInterface(/* void *xface */);
extern int			Netcdf_DestructFile();

extern int 			Netcdf_IsCoordVar(/*char *varname */);
extern Abs_Variable*		Netcdf_ConstructCoordVar(/* char *coordname */);

extern int 			Netcdf_SetFile(/*char *fname, char mode */);
extern int 			Netcdf_OpenFile(/*char *fname, char mode */);
extern int 			Netcdf_CloseFile();
extern void 			Netcdf_GatherFileInfo();

extern float			Netcdf_GetStartTime();
extern float			Netcdf_GetDt();
extern int			Netcdf_GetDataType();
extern int			Netcdf_GetHeaderSize();

extern char*			Netcdf_CreateIndexedName(/*char *clientname, int clientindex*/);
extern char*			Netcdf_RecreateNameAndIndex(/*char *nameinfile, int *index */);

/*Netcdf_Metadata Methods */
/*private: */
extern int                      Netcdf_CreateAttr(/* char *name, Abs_Type type, long size, void *vals */);
/*public: */
extern int                      Netcdf_SetAttrType(/* Abs_Type type */);
extern int                      Netcdf_SetAttrSize(/* long size */);
extern int                      Netcdf_SetAttrValues(/* void *vals, long start_index, long count */);

extern Abs_Type                 Netcdf_GetAttrType();
extern long                     Netcdf_GetAttrSize();
extern int                      Netcdf_GetAttrValues(/*void *vals, long start_index, long size */);

extern int                      Netcdf_RenameAttr(/* char *newname */);
extern int                      Netcdf_DeleteAttr();
extern int                      Netcdf_FlushAttr();
extern int                      Netcdf_DestructAttr();

/*Netcdf_Variable Methods */
extern Abs_Metadata*            Netcdf_ConstructLocalAttr(/* char *name, Abs_Type type, long size */);
/*private: */
extern int                      Netcdf_CreateVar(/*char *name, Abs_Type type, Abs_Varshape * */);
extern int			Netcdf_IsVarName(/*char *name*/);

/*public: */
extern int                      Netcdf_SetVarShape(/* Abs_Varshape* shape */);
extern int                      Netcdf_SetVarType(/* Abs_Type type */);
extern int                      Netcdf_SetVarValues(/* void *vals, long start_index[], long count[] */);

extern Abs_Varshape*            Netcdf_GetVarShape();
extern Abs_Type                 Netcdf_GetVarType();
extern size_t 			Netcdf_GetVarTypeSize();
extern int                   	Netcdf_GetVarValues(/* void *vals, long start_index[], long count[]*/);
extern long 			Netcdf_GetVarIndexForInput(/*char *input*/);
extern long 			Netcdf_FastGetVarIndexForInput(/*char *input*/);

extern int                      Netcdf_RenameVar(/* char *newname */);
extern int                      Netcdf_DeleteVar();
extern int                      Netcdf_FlushVar();
extern int			Netcdf_FreeChildAttrInterface(/* void *xface */);
extern int                      Netcdf_DestructVar();

/** Some netcdf helper functions **/
extern void* Netcdf_MakeDefaultVarValues();
extern void* Netcdf_MakeDefaultAttrValues();
extern int Netcdf_CopyVarValues();
extern int Netcdf_CopyAttrValues();

extern void* Netcdf_AllocValues();
extern void* Netcdf_ReallocValues();

/* Type conversion functions */
extern nc_type Abs_Type_To_nc_type();
extern Abs_Type nc_type_To_Abs_Type();
extern void* Netcdf_Convert_To_nc_type();

/* Global array of netcdf types */
extern nc_type netcdf_type[];

/* Notification for an unsupported type */
#define NC_INVALID (nc_type)-100
#endif
