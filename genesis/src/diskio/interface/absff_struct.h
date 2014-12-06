/*********************************************************************

** This library uses the netcdf - version 3.4 which is
** Copyright 1993-1997 University Corporation for Atmospheric Research/Unidata

** The netcdf library is provided as per the terms of the
** UCAR/Unidata license.

** See netcdf-3.4/copyright.html for the full notice.

********************************************************************/
/* $Id: absff_struct.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $ */
/*
 * $Log: absff_struct.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.9  2000/06/12 05:07:02  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.8  1999/10/16 21:53:59  mhucka
 * Merged Upi's changes for supporting netCDF 3.4.  The changes in most files
 * only involve the copyright notice.  I also fixed up the comments slightly
 * to reflect the location of the actual netCDF copyright file.
 *
 * Revision 1.7  1999/10/16 21:25:06  mhucka
 * Venkat had a version of the diskio code in his home directory that he
 * appears to have been working on, and that did not make it into the
 * source tree that I had originally used as the starting point of my
 * GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
 * changes into the distribution, I'm reverting to his version, then will
 * update his version to incorporate recent changes by Upi for netCDF 3.4.
 *
 * Revision 1.5  1998/01/14 18:01:32  venkat
 * Addition of methods to support FMT1 in the various data structures.
 * Also removed the usage of typenames such as uchar, ulong, ushort etc. Was
 * causing problems in certain systems and needed conditional declaration
 * of typdefs. This has been totally done away with in this version.
 *
 * Revision 1.4  1997/08/08 00:30:52  dhb
 * Added aix as one of the systems to avoid uchar definition on.
 *
 * Revision 1.3  1997/08/03 20:04:02  dhb
 * Added additional ifdefs for ulong and uchar typedefs to avoid
 * similar standard typedefs on Paragon and T3E.
 *
 * Revision 1.2  1997/07/26 00:15:24  venkat
 * Added-Copyright-notice-and-RCS-headers
 * */

#ifndef ABS_STRUCT_H
#define ABS_STRUCT_H

/* Maybe useful typedef for  generic 8-bit data type */

typedef char byte;

/* Enum that defines the basic data types used by fileformats */

typedef enum {
	/*8-bit signed & unsigned */
	abs_uint8=0,
	abs_int8,
	/*16-bit unsigned & signed*/
	abs_uint16,	
	abs_int16,
	/*32-bit unsigned & signed */
	abs_uint32,
	abs_int32,
	/*64-bit unsigned & signed */
	abs_uint64,
	abs_int64,
	/** 32 and 64-bit floating points **/
	abs_float32,
	abs_float64,
	/** 8-bit signed and unsigned char, unicode(16-bit char) and strings(??) **/
	abs_uchar8,
	abs_char8,
	abs_char16,
	abs_string
}Abs_Type;

/* A union of the various allowed types - Could be useful in certain cases */
typedef union{
	unsigned char uchar_value;
	char char_value;
	unsigned short ushort_value;
	short short_value;
	unsigned int uint_value;
	int int_value;
	unsigned long ulong_value;
	long long_value;
	float float_value;
	double double_value;
	/* Need to add something for 16-bit chars here */
	char *string_value;
}Abs_ComponentValue;

/* Typedefs for the essential interface structs */
typedef struct Abs_Diskio_Type 		Abs_Diskio;
typedef struct Abs_Metadata_Type 	Abs_Metadata;
typedef struct Abs_Variable_Type 	Abs_Variable;


/* Structure that defines the shape of a variable in terms of 
	domains or dimensions */
typedef struct Abs_Varshape_Type {
	int ndomains;
	char **domainname;
	long *domainsize;
	byte *domaintype; /* UNLIMITED or not */
}Abs_Varshape;

/** The Abstract DiskIO structure **/
/**
  This interfaces between client code and the physical file on disk.
    At the moment the works are not there to deal with the file in many
    ways other than opening/creating it in a particular mode and writing/reading 
    from it and eventually closing it. Features such as dynamically 
    switching between files, changing the mode of access and physically deleting
    the file will probably be added in future.

    One important issue that needs to be addressed well in future is the 
    coordinate variable business. To the library (for Netcdf) this is just 
    another variable but with the same name as a dimension in the file. So
    no special constructors/destructors are provided for it but the diskio 
    interface struct has handles to an array (size=MAX_COORD_VAR) of these
    variable interfaces that are allocated in the client code. (CREATE action for
    the gen_variable object in Genesis). Support is therfore provided in this interface for any
    number of  multi-dimensional variables  but for no more than MAX_COORD_VAR coord variables 
**/

#define ABS_DISKIO_TYPE \
	char			*filename; \
	short			is_open; \
	short 			is_writable; \
\
	int 			nglobmetadata; \
	char 			**globmetadatanames; \
	int 			nvariables; \
	char 			**variablenames; \
\
/* Methods to be overridden by individual file format diskios*/ \
	Abs_Metadata*		(*ConstructMetadata_Interface) (/* char *name, Abs_Type type, long size */); \
	Abs_Variable*		(*ConstructVariable_Interface) (/* char *name, Abs_Type type, Abs_Varshape* */); \
	/* \
	int			(*SetAccessmode) (); \
	int			(*GetAccessmode) (); \
	*/ \
	int			(*Flush) (); \
	int			(*Destruct)(); \
	int			(*FreeChildInterface) (); \
	int			(*DestructChildInterfaces) (); \
\
/* Public methods to deal with coordinate variables in particular - overriden by specific file formats */ \
	int			(*IsCoordVariable)(); /* args=char *varname */ \
	Abs_Variable*		(*ConstructCoord_Interface)(/*char *coordvarname*/); \
\
/* Methods added to deal with backwards compatibility with disk_out/disk_in */ \
	int			(*SetFile)(); /*args=char *filename, char accessmode */ \
	int			(*ResetDataOffset)();  \
	int			(*ResetFile)();  \
	int			(*CloseFile)();  \
	int			(*BufferLabel)(); \
	int			(*BufferStartTime)(); \
	int			(*BufferDt)(); \
	int			(*BufferFrameSize)(); \
	int			(*BufferDatatype)(); \
	float 			(*GetStartTime)(); \
	float 			(*GetDt)(); \
	int 			(*GetDataType)(); \
	int 			(*GetHeaderSize)(); \
	int 			(*SetAppendMode)(); \
	int 			(*SetMetadataInterface)(); \
	int 			(*SetMetadataValues)(); \
	int			(*SetVariableInterface)(); \
	int			(*SetVariableValues)(); \
	int			(*FillAllVariableValues)(); \
	int			(*FillVariableValues)(/*int variable_xface_offset*/); \
/* Private methods */ \
	int 			(*OpenFile)();/*args=char *filename, char accessmode */	\
	void			(*GatherInfo)(); \
\
/* These methods are generic interface methods pertaining to the format in general \
   rather than any component in the format - should be overridden by \
   subclass methods */	\
	char* 			(*CreateIndexedName)(); /* args=char *clientname, int index */ \
	char*			(*RecreateNameAndIndex)(); /*args= char *nameinfile, int *index	*/ \
/* Private Methods */ \
	int			(*WriteHeader)() /* args= char *header, int bytesize, int nvalues */; 

struct Abs_Diskio_Type {
	ABS_DISKIO_TYPE
};

/* Abs_Diskio methods */
/*public: */
extern int Absff_DestructDiskio();

/* The abstract metadata structure */

#define ABS_METADATA_TYPE \
/* Handles to the parent components in the file format */ \
	Abs_Diskio		*adi; \
	Abs_Variable		*avi; \
/* value addr is cached to deal with non-garbage-collecting file formats */ \
	void 			*vals; \
/* type and size are cached so genesis string args can be converted easily */ \
	Abs_Type		abstype; \
	long 			size; \
/* These are private methods called from the metadata constructors */ \
	int			(*Create)(); /* args = Abs_Type type, long size */ \
/* Public methods to be overridden by individual metadata objects */ \
	int			(*SetType)(); /* arg=Abs_Type type*/ \
	int			(*SetSize)(); /* arg=long size */ \
	int			(*SetValues)(); /*args=void *vals,long start_index,long count */ \
	int			(*SetValuesFromString)(); /*args=char **vals, long start_index, long count*/ \
	Abs_Type		(*GetType)(); \
	long			(*GetSize)(); \
	int			(*GetValues)();/*args=void *vals,long start_index,long count */ \
	int			(*GetValuesAsString)(); /*args=char **vals, long start_index, long count*/ \
\
	int			(*Rename)(); /*args=char *newname */ \
	int			(*Delete)(); \
	int			(*Flush) (); \
	int			(*Destruct)();

struct Abs_Metadata_Type {
	ABS_METADATA_TYPE
};

/* Abs_Metadata methods */
/*private: */
extern int Absff_CreateMetadata(/* Abs_Type type, int size */);
/*public: */
extern int Absff_SetMetadataType(/* Abs_Type type */);
extern int Absff_SetMetadataSize(/* int size */);
extern int Absff_SetMetadataValues(/*void *vals, int start, int count */);
extern int Absff_SetMetadataValuesFromString(/*char **vals, int start, int count */);

extern Abs_Type Absff_GetMetadataType();
extern long Absff_GetMetadataSize();
extern int Absff_GetMetadataValues(/*void *vals, int start, int count */);
extern int Absff_GetMetadataValuesAsString(/* char **vals, int start, int count */);

#define ABS_VARIABLE_TYPE \
/* Handle to the parent component in the file format */ \
	Abs_Diskio 		*adi;	 \
\
	int			nlocalmetadata; \
	char 			**localmetadatanames; \
\
/* The shape of the var is cached to avoid inquiring the library often */ \
	Abs_Varshape 		*varshape; \
/* The type is cached so genesis string args can be converted easily */ \
	Abs_Type		abstype; \
/* These are private methods */ \
	int			(*Create)();/*args=char *name, Abs_Type type, Abs_Varshape *shape */ \
/* Methods that are overridden by individual file format variable objects */ \
	Abs_Metadata*		(*ConstructMetadata_Interface)(); /*Constructor for local attributes*/ \
\
	int			(*SetShape)(); /* arg=Abs_Varshape *shape */ \
	int			(*SetType)(); /* arg=Abs_Type type */ \
	int			(*SetValues)(); /*args=void *vals,long start_index[],long count[]*/ \
	int			(*SetValuesFromString)(); /*args=char **vals, long start_index[], long count[]*/ \
\
	Abs_Varshape*		(*GetShape)(); \
	Abs_Type		(*GetType)(); \
	size_t			(*GetTypeSize)(); \
	int			(*GetValues)(); /* args=void *vals, long start_index[], long count[] */ \
	int			(*GetValuesAsString)(); /*args=char **vals, long start_index[], long count[]*/ \
\
/* The following fields and methods here are relevant only to coord variables*/ \
\
/* This remembers the last index to the coord so the next search is faster */ \
	long			last_retrieved_index; \
\
	long			(*GetIndexForInput)(); /*args=char *input */ \
	long			(*FastGetIndexForInput)(); /*args=char *input */ \
	void			(*ResetFastIndexForInput)(); \
\
	int			(*IsName)();/*args=char *name*/ \
	int			(*Rename)(); /*args=char *newname */ \
	int			(*Delete)(); \
	int			(*Flush) (); \
	int			(*FreeChildInterface) (); \
	int			(*Destruct)(); 

struct Abs_Variable_Type {
	ABS_VARIABLE_TYPE
};

/* Abs_Variable methods */
/*public: */
extern int Absff_SetVariableType(/* Abs_Type type */);
extern int Absff_SetVariableShape(/* Abs_Varshape *shape */);
extern int Absff_SetVariableValues(/*void *vals, long start[], long count[] */);
extern int Absff_SetVariableValuesFromString(/* char **vals, long start[], long count[] */);

extern Abs_Type Absff_GetVariableType();
extern size_t Absff_GetVariableTypeSize();
extern Abs_Varshape* Absff_GetVariableShape();
extern int Absff_GetVariableValues(/*void *vals, long start[], long count[] */);
extern int Absff_GetVariableValuesAsString(/* char **vals, long start[], long count[] */);
extern long Absff_GetVariableIndexForInput(/* char *input */);
extern long Absff_FastGetVariableIndexForInput(/* char *input */);
extern void Absff_ResetFastVariableIndexForInput();

/* Some utility functions */
extern Abs_Varshape * Absff_ConstructDefaultShape(/* char *domainname */);
extern void Absff_DestructDefaultShape(/*Abs_Varshape **shape*/);
extern void Absff_FreeVariableShape(/*Abs_Varshape *shape */);
extern void* Absff_AllocDefaultValues(/* Abs_Type type, int size */);
extern void* Absff_ReallocDefaultValues(/*void *vals, Abs_Type type, int size */);
extern int Absff_CopyFromValues(/*void *dupvals, void *origvals, Abs_Type type, long size, long start_index, long count */);
extern int Absff_CopyToValues(/* void *dupvals, void *origvals, Abs_Type type, long size, long start_index, long count */);
extern void Absff_ConvertVariableValueFromString(/*char *value*/);
extern char* Absff_CopyString(/*char * str */);
extern void Absff_FreeString(/*char *str*/);
extern double Absff_Atod(/*char *str*/);
extern float Absff_Atof(/*char *str*/);
extern long Absff_Atol(/*char *str*/);

#endif
