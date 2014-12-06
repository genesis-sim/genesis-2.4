#ifndef FMT1_STRUCT_H
#define FMT1_STRUCT_H

typedef struct FMT1_Metadata_Type  FMT1_Metadata;
typedef struct FMT1_Variable_Type  FMT1_Variable;

typedef struct FMT1_Diskio_Type{
	ABS_DISKIO_TYPE 
	/* FMT1 specific members */
	FILE	*fp;
	char 	*label;
	float	starttime;
	float 	dt;
	int 	framesize;
	int 	datatype;
	void 	*framevals;	
	FMT1_Metadata **autoSAVE_famdi;
	FMT1_Variable **autoSAVE_favi;	
	int 	writeposition;
	int 	flush_frame_offset;
	short	is_appendmode;
}FMT1_Diskio;

extern int			FMT1_NOP();

/*FMT1_Diskio Methods */
extern Abs_Metadata*            FMT1_ConstructGlobalAttr(/* char *name, Abs_Type type, long size */);
extern Abs_Variable*            FMT1_ConstructVar(/* char *name, Abs_Type type, Abs_Varshape *shape */);
extern int 			FMT1_FlushFile();
extern int			FMT1_DestructChildInterfaces();
extern int			FMT1_DestructFile();

extern int 			FMT1_IsCoordVar(/*char *varname */);
extern Abs_Variable*		FMT1_ConstructCoordVar(/* char *coordname */);

extern int 			FMT1_SetFile(/*char *fname, char mode */);
extern int 			FMT1_OpenFile(/*char *fname, char mode */);
extern int			FMT1_ResetDataOffset();
extern int 			FMT1_ResetFile();
extern int 			FMT1_CloseFile();
extern void 			FMT1_GatherFileInfo();
extern int			FMT1_BufferLabel();
extern int			FMT1_BufferStartTime();
extern int			FMT1_BufferDt();
extern int			FMT1_BufferFrameSize();
extern int			FMT1_BufferDatatype();
extern int			FMT1_Get_HeaderSize();
extern int			FMT1_Get_DataType();
extern float			FMT1_GetStartTime();
extern float			FMT1_GetDt();

extern int			FMT1_SetMetadataInterface(/* int famdi_offset, Abs_Metadata *amdi */);
extern int			FMT1_SetAppendMode(/* short appendmode */);
extern int			FMT1_SetVariableInterface(/* int favi_offset, Abs_Variable *avi */);
extern int 			FMT1_SetMetadataValues(/* int famdi_offset, void *vals, long start, long count */);
extern int 			FMT1_SetVariableValues(/* int favi_offset, void *vals, long start[], long count[] */);
extern int 			FMT1_FillAllVariableValues();
extern int 			FMT1_FillVariableValues(/* int favi_offset */);

extern char*			FMT1_CreateIndexedName(/*char *clientname, int clientindex*/);
extern char*			FMT1_RecreateNameAndIndex(/*char *nameinfile, int *index */);

/* Private Methods */
extern int			FMT1_WriteHeader();
extern int 			FMT1_Get_FrameSize();

struct FMT1_Metadata_Type{
	ABS_METADATA_TYPE
	/* FMT1 specific members */
	short propmodified;
};

/*FMT1 Metadata Methods */
/*private: */
extern int                      FMT1_CreateAttr(/* Abs_Type type, long size*/);
/*public: */
extern int                      FMT1_SetAttrType(/* Abs_Type type */);
extern int                      FMT1_SetAttrSize(/* long size */);
extern int                      FMT1_SetAttrValues(/* void *vals, long start_index, long count */);

extern int			FMT1_RenameAttr(/* char *newname */);
extern int                      FMT1_FlushAttr();
extern int			FMT1_DeleteAttr();
extern int                      FMT1_DummyDestructAttr();
extern int                      FMT1_DestructAttr();

extern int 			FMT1_FlushMetadataValues();

struct FMT1_Variable_Type{
	ABS_VARIABLE_TYPE
	/* Netcdf specific members */
	void *vals;
	int 	nvalallocations;
	short propmodified;
};


/*FMT1 Variable Methods */
extern Abs_Metadata*            FMT1_ConstructLocalAttr(/* char *name, Abs_Type type, long size */);
/*private: */
extern int                      FMT1_CreateVar(/*char *name, Abs_Type type, Abs_Varshape * */);
extern int			FMT1_IsVarName(/*char *name*/);

/*public: */
extern int                      FMT1_SetVarShape(/* Abs_Varshape* shape */);
extern int                      FMT1_SetVarType(/* Abs_Type type */);
extern int                      FMT1_SetVarValues(/* void *vals, long start_index[], long count[] */);

extern Abs_Varshape*            FMT1_GetVarShape();
extern Abs_Type                 FMT1_GetVarType();
extern size_t 			FMT1_GetVarTypeSize();
extern int                   	FMT1_GetVarValues(/* void *vals, long start_index[], long count[]*/);

extern int                      FMT1_RenameVar(/* char *newname */);
extern int                      FMT1_DeleteVar();
extern int                      FMT1_FlushVar();
extern int                      FMT1_DummyDestructVar();
extern int                      FMT1_DestructVar();


/* Global array of FMT1 types */
extern fmt1_type FMT1_type[];

/* Type conversion functions */
extern fmt1_type Abs_Type_To_fmt1_type();
extern Abs_Type fmt1_type_To_Abs_Type();
extern void* FMT1_Convert_To_fmt1_type();

/** Some FMT1 helper functions **/
extern void FMT1_FillIndexedAttrName();
extern void FMT1_FillIndexedVarName();

/* Notification for an unsupported type */
#define FMT1_INVALID (fmt1_type)-100
#define FMT1_FRAME_INCREMENT 20
#endif
