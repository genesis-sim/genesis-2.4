/******************************************************************************
* In order to make life a little bit easier when using the GIF file format,   *
* this library was written, and which does all the dirty work...	      *
*									      *
*					Written by Gershon Elber,  Jun. 1989  *
*					Hacks by Eric S. Raymond,  Sep. 1992  *
*                                             and Jareth Hein,     Jan. 1998  *
*******************************************************************************
* History:								      *
* 14 Jun 89 - Version 1.0 by Gershon Elber.				      *
*  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names). *
* 15 Sep 90 - Version 2.0 by Eric S. Raymond (Changes to suoport GIF slurp)   *
* 26 Jun 96 - Version 3.0 by Eric S. Raymond (Full GIF89 support)             *
* 19 Jan 98 - Version 3.1 by Jareth Hein (Support for user-defined I/O).      *
******************************************************************************/

#ifndef GIF_LIB_H
#define GIF_LIB_H

#define	GIF_ERROR	0
#define GIF_OK		1

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#ifndef NULL
#define NULL		0
#endif /* NULL */

#define GIF_FILE_BUFFER_SIZE 16384  /* Files uses bigger buffers than usual. */

typedef	int		GifBooleanType;
typedef	unsigned char	GifPixelType;
typedef unsigned char *	GifRowType;
typedef unsigned char	GifByteType;

#ifdef SYSV
#define VoidPtr char *
#else
#define VoidPtr void *
#endif /* SYSV */

typedef struct GifColorType {
    GifByteType Red, Green, Blue;
} GifColorType;

typedef struct ColorMapObject
{
    int	ColorCount;
    int BitsPerPixel;
    GifColorType *Colors;		/* on malloc(3) heap */
}
ColorMapObject;

typedef struct GifImageDesc {
    int Left, Top, Width, Height,	/* Current image dimensions. */
	Interlace;			/* Sequential/Interlaced lines. */
    ColorMapObject *ColorMap;		/* The local color map */
} GifImageDesc;

/* I/O operations.  If you roll your own, they need to be semantically equivilent to
   fread/fwrite, with an additional paramater to hold data local to your method. */
typedef size_t (*Gif_rw_func)(GifByteType *buffer, size_t size, VoidPtr method_data);
/* Finish up stream. Non-zero return indicates failure */
typedef int (*Gif_close_func)(VoidPtr close_data);
/* Error handling function */
typedef void (*Gif_error_func)(const char *string, VoidPtr error_data);

typedef struct GifFileType {
    int SWidth, SHeight,		/* Screen dimensions. */
	SColorResolution, 		/* How many colors can we generate? */
	SBackGroundColor;		/* I hope you understand this one... */
    ColorMapObject *SColorMap;		/* NULL if it doesn't exist. */
    int ImageCount;			/* Number of current image */
    GifImageDesc Image;			/* Block describing current image */
    struct SavedImage *SavedImages;	/* Use this to accumulate file state */
    VoidPtr Private;	  		/* Don't mess with this! */
    VoidPtr GifIO;			/* Contains all information for I/O */
} GifFileType;

typedef enum {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE,		/* Begin with ',' */
    EXTENSION_RECORD_TYPE,		/* Begin with '!' */
    TERMINATE_RECORD_TYPE		/* Begin with ';' */
} GifRecordType;

/******************************************************************************
*  GIF89 extension function codes                                             *
******************************************************************************/

#define COMMENT_EXT_FUNC_CODE		0xfe	/* comment */
#define GRAPHICS_EXT_FUNC_CODE		0xf9	/* graphics control */
#define PLAINTEXT_EXT_FUNC_CODE		0x01	/* plaintext */
#define APPLICATION_EXT_FUNC_CODE	0xff	/* application block */

/******************************************************************************
* IO related routines.  Defined in gif_io.c                                   *
******************************************************************************/
GifFileType *GifSetup(void);
void GifFree(GifFileType *GifFile);
void GifSetReadFunc (GifFileType *GifFile, Gif_rw_func func, VoidPtr data);
void GifSetWriteFunc(GifFileType *GifFile, Gif_rw_func func, VoidPtr data);
void GifSetCloseFunc(GifFileType *GifFile, Gif_close_func func, VoidPtr data);

/******************************************************************************
* O.K., here are the routines one can access in order to decode GIF file:     *
******************************************************************************/

void DGifOpenFileName(GifFileType *GifFile, const char *GifFileName);
void DGifOpenFileHandle(GifFileType *GifFile, int GifFileHandle);
void DGifInitRead(GifFileType *GifFile);
void DGifSlurp(GifFileType *GifFile);
void DGifGetScreenDesc(GifFileType *GifFile);
void DGifGetRecordType(GifFileType *GifFile, GifRecordType *GifType);
void DGifGetImageDesc(GifFileType *GifFile);
void DGifGetLine(GifFileType *GifFile, GifPixelType *GifLine, int GifLineLen);
void DGifGetPixel(GifFileType *GifFile, GifPixelType GifPixel);
void DGifGetComment(GifFileType *GifFile, char *GifComment);
void DGifGetExtension(GifFileType *GifFile, int *GifExtCode,
						GifByteType **GifExtension);
void DGifGetExtensionNext(GifFileType *GifFile, GifByteType **GifExtension);
void DGifGetCode(GifFileType *GifFile, int *GifCodeSize,
						GifByteType **GifCodeBlock);
void DGifGetCodeNext(GifFileType *GifFile, GifByteType **GifCodeBlock);
void DGifGetLZCodes(GifFileType *GifFile, int *GifCode);
int DGifCloseFile(GifFileType *GifFile);

#define	D_GIF_ERR_OPEN_FAILED	101		/* And DGif possible errors. */
#define	D_GIF_ERR_READ_FAILED	102
#define	D_GIF_ERR_NOT_GIF_FILE	103
#define D_GIF_ERR_NO_SCRN_DSCR	104
#define D_GIF_ERR_NO_IMAG_DSCR	105
#define D_GIF_ERR_NO_COLOR_MAP	106
#define D_GIF_ERR_WRONG_RECORD	107
#define D_GIF_ERR_DATA_TOO_BIG	108
#define GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED	110
#define D_GIF_ERR_NOT_READABLE	111
#define D_GIF_ERR_IMAGE_DEFECT	112
#define D_GIF_ERR_EOF_TOO_SOON	113

/******************************************************************************
* O.K., here are the error routines 					      *
******************************************************************************/
extern void GifSetErrorFunc(GifFileType *GifFile, Gif_error_func func, VoidPtr data);
extern void GifSetWarningFunc(GifFileType *GifFile, Gif_error_func func, VoidPtr data);
extern void GifInternError(GifFileType *GifFile, int errnum);
extern void GifInternWarning(GifFileType *GifFile, int errnum);
extern void GifError(GifFileType *GifFile, const char *err_str);
extern void GifWarning(GifFileType *GifFile, const char *err_str);

/*****************************************************************************
 *
 * Everything below this point is new after version 1.2, supporting `slurp
 * mode' for doing I/O in two big belts with all the image-bashing in core.
 *
 *****************************************************************************/

/******************************************************************************
* Support for the in-core structures allocation (slurp mode).		      *
******************************************************************************/

/* This is the in-core version of an extension record */
typedef struct {
    int		    ByteCount;
    GifByteType	*Bytes;		/* on malloc(3) heap */
} ExtensionBlock;

/* This holds an image header, its unpacked raster bits, and extensions */
typedef struct SavedImage {
    GifImageDesc	ImageDesc;

    GifPixelType	*RasterBits;		/* on malloc(3) heap */

    int			Function;
    int			ExtensionBlockCount;
    ExtensionBlock	*ExtensionBlocks;	/* on malloc(3) heap */
} SavedImage;

extern void ApplyTranslation(SavedImage *Image, GifPixelType Translation[]);

extern void MakeExtension(SavedImage *New, int Function);
extern int AddExtensionBlock(SavedImage *New, int Length, GifByteType *data);
extern void FreeExtension(SavedImage *Image);

extern SavedImage *MakeSavedImage(GifFileType *GifFile, SavedImage *CopyFrom);
extern void FreeSavedImages(GifFileType *GifFile);

/*   Common defines used by encode/decode functions */

#define GIF_STAMP	"GIFVER"	 /* First chars in file - GIF stamp. */
#define GIF_STAMP_LEN	sizeof(GIF_STAMP) - 1
#define GIF_VERSION_POS	3		/* Version first character in stamp. */
#define GIF87_STAMP	"GIF87a"         /* First chars in file - GIF stamp. */
#define GIF89_STAMP	"GIF89a"         /* First chars in file - GIF stamp. */

#define LZ_MAX_CODE	4095		/* Biggest code possible in 12 bits. */
#define LZ_BITS		12

#define FILE_STATE_READ		0x01
#define FILE_STATE_WRITE	0x01
#define FILE_STATE_SCREEN	0x02
#define FILE_STATE_IMAGE	0x04

#define FLUSH_OUTPUT		4096    /* Impossible code, to signal flush. */
#define FIRST_CODE		4097    /* Impossible code, to signal first. */
#define NO_SUCH_CODE		4098    /* Impossible code, to signal empty. */

#define IS_READABLE(Private)	(!(Private->FileState & FILE_STATE_READ))
#define IS_WRITEABLE(Private)	(Private->FileState & FILE_STATE_WRITE)

typedef struct GifFilePrivateType {
    int FileState,
	BitsPerPixel,	    /* Bits per pixel (Codes uses at list this + 1). */
	ClearCode,				       /* The CLEAR LZ code. */
	EOFCode,				         /* The EOF LZ code. */
	RunningCode,		    /* The next code algorithm can generate. */
	RunningBits,/* The number of bits required to represent RunningCode. */
	MaxCode1,  /* 1 bigger than max. possible code, in RunningBits bits. */
	LastCode,		        /* The code before the current code. */
	CrntCode,				  /* Current algorithm code. */
	StackPtr,		         /* For character stack (see below). */
	CrntShiftState;		        /* Number of bits in CrntShiftDWord. */
    unsigned long CrntShiftDWord;     /* For bytes decomposition into codes. */
    unsigned long PixelCount;		       /* Number of pixels in image. */
    GifByteType Buf[256];	       /* Compressed input is buffered here. */
    GifByteType Stack[LZ_MAX_CODE];	 /* Decoded pixels are stacked here. */
    GifByteType Suffix[LZ_MAX_CODE+1];	       /* So we can trace the codes. */
    unsigned int Prefix[LZ_MAX_CODE+1];
} GifFilePrivateType;

typedef struct GifIODataType {
    Gif_rw_func ReadFunc, WriteFunc;	/* Pointers to the functions that will do the I/O */
    Gif_close_func CloseFunc;    
    VoidPtr ReadFunc_data;		/* data to be passed to the read function */
    VoidPtr WriteFunc_data;		/* data to be passed to the write function */
    VoidPtr CloseFunc_data;		/* data to be passed to the close function */
    Gif_error_func ErrorFunc;	/* MUST NOT RETURN (use lng_jmp or exit)!  */
    Gif_error_func WarningFunc;	/* For warning messages (can be ignored) */
    VoidPtr ErrorFunc_data;
    VoidPtr WarningFunc_data;
} GifIODataType;

typedef struct GifStdIODataType {
  FILE *File;
  int FileHandle;
} GifStdIODataType;

/* Install StdIO funcs on FILE into GifFile */
void GifStdIOInit(GifFileType *GifFile, FILE *file, int filehandle);

/* Error checking reads, writes and closes */
void GifRead(GifByteType *buf, size_t size, GifFileType *GifFile);
void GifWrite(GifByteType *buf, size_t size, GifFileType *GifFile);
int GifClose(GifFileType *GifFile);

/* The default Read and Write functions for files */
size_t GifStdRead(GifByteType *buf, size_t size, VoidPtr method_data);
size_t GifStdWrite(GifByteType *buf, size_t size, VoidPtr method_data);
int GifStdFileClose(VoidPtr method_data);

ColorMapObject *MakeMapObject(int ColorCount, GifColorType *ColorMap);
void FreeMapObject(ColorMapObject *Object);


#endif /* GIF_LIB_H */
