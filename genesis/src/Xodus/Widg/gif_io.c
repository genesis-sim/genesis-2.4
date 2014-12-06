#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gifrlib.h"

/******************************************************************************
* Set up the GifFileType structure for use.  This must be called first in any *
* client program.  Then, if custom IO or Error functions are desired, call    *
* GifSetIOFunc/GifSetErrorFunc, then call EGifInitWrite.  Else call           *
* EGifOpenFileName or EGifOpenFileHandle for standard IO functions.           *
* If setup fails, a NULL pointer is returned.                                 *
******************************************************************************/
GifFileType *GifSetup(void)
{
    GifIODataType *GifIO;
    GifFileType *GifFile;

    if ((GifFile = (GifFileType *) malloc(sizeof(GifFileType))) == NULL)
	return NULL;
    memset(GifFile, '\0', sizeof(GifFileType));
    if ((GifIO = (GifIODataType *) malloc(sizeof(GifIODataType))) == NULL) {
	free((char *) GifFile);
	return NULL;
    }
    memset(GifIO, '\0', sizeof(GifIODataType));
    GifFile->GifIO = GifIO;    
    return GifFile;
}

void GifFree(GifFileType *GifFile)
{
    GifFilePrivateType *Private;

    if (GifFile == NULL) return;

    Private = (GifFilePrivateType *) GifFile->Private;

    if (GifFile->SavedImages)
	FreeSavedImages(GifFile);
    if (GifFile->Image.ColorMap)
	FreeMapObject(GifFile->Image.ColorMap);
    if (GifFile->SColorMap)
	FreeMapObject(GifFile->SColorMap);
    if (Private)
    {
	free(Private);
    }
    if (GifFile->GifIO)
	free(GifFile->GifIO);
    free(GifFile);
}

/****************************************************************************
* Install the specified ReadFunction into the GifFile specified.            *
****************************************************************************/
void GifSetReadFunc(GifFileType *GifFile, Gif_rw_func ReadFunc, VoidPtr data)
{
    GifIODataType *GifIO = (GifIODataType *)GifFile->GifIO;
    GifIO->ReadFunc = ReadFunc;
    GifIO->ReadFunc_data = data;
}

/****************************************************************************
* Install the specified WriteFunction into the GifFile specified.           *
****************************************************************************/
void GifSetWriteFunc(GifFileType *GifFile, Gif_rw_func WriteFunc, VoidPtr data)
{
    GifIODataType *GifIO = (GifIODataType *)GifFile->GifIO;
    GifIO->WriteFunc = WriteFunc;
    GifIO->WriteFunc_data = data;
}

/****************************************************************************
* Install the specified CloseFunction into the GifFile specified.           *
****************************************************************************/
void GifSetCloseFunc(GifFileType *GifFile, Gif_close_func CloseFunc, VoidPtr data)
{
    GifIODataType *GifIO = (GifIODataType *)GifFile->GifIO;
    GifIO->CloseFunc = CloseFunc;
    GifIO->CloseFunc_data = data;
}

/****************************************************************************
* Install the standard IO funcs into the GifFile, including the FILE info   *
****************************************************************************/
void GifStdIOInit(GifFileType *GifFile, FILE *file, int filehandle)
{
    GifStdIODataType *IOData;
    
    if ((IOData = (GifStdIODataType*)malloc(sizeof(GifStdIODataType))) == NULL)
	GifInternError(GifFile, GIF_ERR_NOT_ENOUGH_MEM);
    IOData->File = file;
    IOData->FileHandle = filehandle;
    GifSetReadFunc(GifFile, GifStdRead, IOData);
    GifSetWriteFunc(GifFile, GifStdWrite, IOData);
    GifSetCloseFunc(GifFile, GifStdFileClose, IOData);
}

size_t GifStdRead(GifByteType *buf, size_t size, VoidPtr method_data)
{
  GifStdIODataType *IOtype = (GifStdIODataType*)method_data;
  return (fread(buf, 1, size, IOtype->File));
}

size_t GifStdWrite(GifByteType *buf, size_t size, VoidPtr method_data)
{
  GifStdIODataType *IOtype = (GifStdIODataType*)method_data;
  return (fwrite(buf, 1, size, IOtype->File));  
}

int GifStdFileClose(VoidPtr method_data)
{
  int ret;
  GifStdIODataType *IOtype = (GifStdIODataType*)method_data;
  ret = fclose(IOtype->File);
  if (ret == 0 && IOtype->FileHandle != -1)
    ret = close(IOtype->FileHandle);
  return ret;
}

void GifRead(GifByteType *buf, size_t size, GifFileType *GifFile)
{
  GifIODataType *GifIO = (GifIODataType*)GifFile->GifIO;
  if ((*(GifIO->ReadFunc))(buf, size, GifIO->ReadFunc_data) != size)
    GifError(GifFile, "Read error!");
}

void GifWrite(GifByteType *buf, size_t size, GifFileType *GifFile)
{
  GifIODataType *GifIO = (GifIODataType*)GifFile->GifIO;
  if ((*(GifIO->WriteFunc))(buf, size, GifIO->WriteFunc_data) != size)
    GifError(GifFile, "Write error!");
}

int GifClose(GifFileType *GifFile) 
{
  GifIODataType *GifIO = (GifIODataType*)GifFile->GifIO;
  return ((*(GifIO->CloseFunc))(GifIO->CloseFunc_data));
}

static char *GifErrorString[14] = {
  "Failed to open given file",			/* D_GIF_ERR_OPEN_FAILED */
  "Failed to read from given file",		/* D_GIF_ERR_READ_FAILED */
  "Given file is NOT a GIF file",		/* D_GIF_ERR_NOT_GIF_FILE */
  "No Screen Descriptor detected",		/* D_GIF_ERR_NO_SCRN_DSCR */
  "No Image Descriptor detected",		/* D_GIF_ERR_NO_IMAG_DSCR */
  "No global or local color map",		/* D_GIF_ERR_NO_COLOR_MAP */
  "Wrong record type detected",			/* D_GIF_ERR_WRONG_RECORD */
  "#Pixels bigger than Width * Height",		/* D_GIF_ERR_DATA_TOO_BIG */
  "Fail to allocate required memory",		/* D_GIF_ERR_NOT_ENOUGH_MEM */
  "Failed to close given file",			/* D_GIF_ERR_CLOSE_FAILED */
  "Given file was not opened for read",		/* D_GIF_ERR_CLOSE_FAILED */
  "Image is defective, decoding aborted",	/* D_GIF_ERR_IMAGE_DEFECT */
  "Image EOF detected before image complete",	/* D_GIF_ERR_EOF_TOO_SOON */
  "Undefined error!",
};

const char *GetGifError(int error);

/*****************************************************************************
* Get the last GIF error in human-readable form.			     *
*****************************************************************************/
const char *GetGifError(int error)
{
    char *Err;

    switch(error) {
	case D_GIF_ERR_OPEN_FAILED:
	    Err = GifErrorString[0];
	    break;
	case D_GIF_ERR_READ_FAILED:
	    Err = GifErrorString[1];
	    break;
	case D_GIF_ERR_NOT_GIF_FILE:
	    Err = GifErrorString[2];
	    break;
	case D_GIF_ERR_NO_SCRN_DSCR:
	    Err = GifErrorString[3];
	    break;
	case D_GIF_ERR_NO_IMAG_DSCR:
	    Err = GifErrorString[4];
	    break;
	case D_GIF_ERR_NO_COLOR_MAP:
	    Err = GifErrorString[5];
	    break;
	case D_GIF_ERR_WRONG_RECORD:
	    Err = GifErrorString[6];
	    break;
	case D_GIF_ERR_DATA_TOO_BIG:
	    Err = GifErrorString[7];
	    break;
	case D_GIF_ERR_NOT_ENOUGH_MEM:
	    Err = GifErrorString[8];
	    break;
	case D_GIF_ERR_CLOSE_FAILED:
	    Err = GifErrorString[9];
	    break;
	case D_GIF_ERR_NOT_READABLE:
	    Err = GifErrorString[10];
	    break;
	case D_GIF_ERR_IMAGE_DEFECT:
	    Err = GifErrorString[11];
	    break;
	case D_GIF_ERR_EOF_TOO_SOON:
	    Err = GifErrorString[12];
	    break;
	default:
	    Err = GifErrorString[13];
	    break;
    }
    return Err;
}

/******************************
* These are called internally *        
******************************/
void GifError(GifFileType *GifFile, const char *err_str)
{
  GifIODataType *GifIO = (GifIODataType*)GifFile->GifIO;
  if (GifIO->ErrorFunc)
    (*(GifIO->ErrorFunc))(err_str, GifIO->ErrorFunc_data);
  else
    fprintf(stderr, "GIF FATAL ERROR: %s", err_str);
  exit(-10);
}

void GifWarning(GifFileType *GifFile, const char *err_str)
{
  GifIODataType *GifIO = (GifIODataType*)GifFile->GifIO;
  if (GifIO->WarningFunc) 
    (*(GifIO->WarningFunc))(err_str, GifIO->WarningFunc_data);
}

void GifInternError(GifFileType *GifFile, int error_num)
{
  const char *ErrStr = GetGifError(error_num);
  GifError(GifFile, ErrStr);
}

void GifInternWarning(GifFileType *GifFile, int error_num)
{
  const char *ErrStr = GetGifError(error_num);
  GifWarning(GifFile, ErrStr);
}

void GifSetErrorFunc(GifFileType *GifFile, Gif_error_func ErrorFunc, VoidPtr data)
{
    GifIODataType *GifIO = (GifIODataType *)GifFile->GifIO;
    GifIO->ErrorFunc = ErrorFunc;
    GifIO->ErrorFunc_data = data;  
}

void GifSetWarningFunc(GifFileType *GifFile, Gif_error_func WarningFunc, VoidPtr data)
{
    GifIODataType *GifIO = (GifIODataType *)GifFile->GifIO;
    GifIO->WarningFunc = WarningFunc;
    GifIO->WarningFunc_data = data;
}
