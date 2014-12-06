/******************************************************************************
*   "Gif-Lib" - Yet another gif library.				      *
*									      *
* Written by:  Gershon Elber			IBM PC Ver 1.1,	Aug. 1990     *
*******************************************************************************
* The kernel of the GIF Decoding process can be found here.		      *
*******************************************************************************
* History:								      *
* 16 Jun 89 - Version 1.0 by Gershon Elber.				      *
*  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names). *
* 19 Feb 98 - Version 1.2 by Jareth Hein (Support for user specified I/O)     *
******************************************************************************/

#ifdef __MSDOS__
#include <io.h>
#include <alloc.h>
#include <stdlib.h>
#include <sys\stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif /* __MSDOS__ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "gifrlib.h"

#define PROGRAM_NAME	"GIFLIB"


static void DGifGetWord(GifFileType *GifFile, int *Word);
static void DGifSetupDecompress(GifFileType *GifFile);
static void DGifDecompressLine(GifFileType *GifFile, GifPixelType *Line,
								int LineLen);
static int DGifGetPrefixChar(unsigned int *Prefix, int Code, int ClearCode);
static void DGifDecompressInput(GifFileType *GifFile, int *Code);
static void DGifBufferedInput(GifFileType *GifFile, GifByteType *NextByte);

/******************************************************************************
*   Open a new gif file for read, given by its name.			      *
*   Returns GifFileType pointer dynamically allocated which serves as the gif *
* info record. 								      *
******************************************************************************/
void DGifOpenFileName(GifFileType *GifFile, const char *FileName)
{
    FILE *f;

    if ((f = fopen(FileName, 
#ifdef __MSDOS__
			     "rb"
#else
		             "r"
#endif /* __MSDOS__ */
			                     )) == NULL)
	GifInternError(GifFile, D_GIF_ERR_OPEN_FAILED);

    GifStdIOInit(GifFile, f, -1);
    DGifInitRead(GifFile);
}

/******************************************************************************
*   Update a new gif file, given its file handle.			      *
*   Returns GifFileType pointer dynamically allocated which serves as the gif *
* info record.								      *
******************************************************************************/
void DGifOpenFileHandle(GifFileType *GifFile, int FileHandle)
{
    FILE *f;

#ifdef __MSDOS__
    setmode(FileHandle, O_BINARY);	  /* Make sure it is in binary mode. */
    f = fdopen(FileHandle, "rb");		   /* Make it into a stream: */
    setvbuf(f, NULL, _IOFBF, GIF_FILE_BUFFER_SIZE);/* And inc. stream buffer.*/
#else
    f = fdopen(FileHandle, "r");		   /* Make it into a stream: */
#endif /* __MSDOS__ */

    GifStdIOInit(GifFile, f, -1);
    DGifInitRead(GifFile);
}

/******************************************************************************
*   Update a new gif file, given its file handle.			      *
*   Returns GifFileType pointer dynamically allocated which serves as the gif *
* info record. _GifError is cleared if succesfull.			      *
******************************************************************************/
void DGifInitRead(GifFileType *GifFile)
{
    GifByteType Buf[GIF_STAMP_LEN+1];
    GifFilePrivateType *Private;

    if ((Private = (GifFilePrivateType *) malloc(sizeof(GifFilePrivateType)))
	== NULL) {
	GifInternError(GifFile, D_GIF_ERR_NOT_ENOUGH_MEM);
    }
    memset(Private, '\0', sizeof(GifFilePrivateType));
    GifFile->Private = (VoidPtr) Private;

    Private->FileState = 0;   /* Make sure bit 0 = 0 (File open for read). */

    /* Lets see if this is a GIF file: */
    GifRead(Buf, GIF_STAMP_LEN, GifFile);

    /* The GIF Version number is ignored at this time. Maybe we should do    */
    /* something more useful with it.					     */
    Buf[GIF_STAMP_LEN] = 0;
    if (strncmp(GIF_STAMP, (const char *) Buf, GIF_VERSION_POS) != 0) {
	GifInternError(GifFile, D_GIF_ERR_NOT_GIF_FILE);
    }

    DGifGetScreenDesc(GifFile);
}

/******************************************************************************
*   This routine should be called before any other DGif calls. Note that      *
* this routine is called automatically from DGif file open routines.	      *
******************************************************************************/
void DGifGetScreenDesc(GifFileType *GifFile)
{
    int i, BitsPerPixel;
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType*) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    /* Put the screen descriptor into the file: */
    DGifGetWord(GifFile, &GifFile->SWidth);
    DGifGetWord(GifFile, &GifFile->SHeight);

    GifRead(Buf, 3, GifFile);
    GifFile->SColorResolution = (((Buf[0] & 0x70) + 1) >> 4) + 1;
    BitsPerPixel = (Buf[0] & 0x07) + 1;
    GifFile->SBackGroundColor = Buf[1];
    if (Buf[0] & 0x80) {		     /* Do we have global color map? */

	GifFile->SColorMap = MakeMapObject(1 << BitsPerPixel, NULL);

	/* Get the global color map: */
	for (i = 0; i < GifFile->SColorMap->ColorCount; i++) {
	    GifRead(Buf, 3, GifFile);
	    GifFile->SColorMap->Colors[i].Red = Buf[0];
	    GifFile->SColorMap->Colors[i].Green = Buf[1];
	    GifFile->SColorMap->Colors[i].Blue = Buf[2];
	}
    } else {
	/* We should always have a colormap */
	GifFile->SColorMap = MakeMapObject(2, NULL);
	GifFile->SColorMap->Colors[0].Red = 0;
	GifFile->SColorMap->Colors[0].Green = 0;
	GifFile->SColorMap->Colors[0].Blue = 0;
	GifFile->SColorMap->Colors[1].Red = 0xff;
	GifFile->SColorMap->Colors[1].Green = 0xff;
	GifFile->SColorMap->Colors[1].Blue = 0xff;
    }
}

/******************************************************************************
*   This routine should be called before any attemp to read an image.         *
******************************************************************************/
void DGifGetRecordType(GifFileType *GifFile, GifRecordType *Type)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;
    
    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    GifRead(&Buf, 1, GifFile);

    switch (Buf) {
	case ',':
	    *Type = IMAGE_DESC_RECORD_TYPE;
	    break;
	case '!':
	    *Type = EXTENSION_RECORD_TYPE;
	    break;
	case ';':
	    *Type = TERMINATE_RECORD_TYPE;
	    break;
	default:
	    *Type = UNDEFINED_RECORD_TYPE;
	    GifInternError(GifFile, D_GIF_ERR_WRONG_RECORD);
    }
}

/******************************************************************************
*   This routine should be called before any attemp to read an image.         *
*   Note it is assumed the Image desc. header (',') has been read.	      *
******************************************************************************/
void DGifGetImageDesc(GifFileType *GifFile)
{
    int i, BitsPerPixel;
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    DGifGetWord(GifFile, &GifFile->Image.Left);
    DGifGetWord(GifFile, &GifFile->Image.Top);
    DGifGetWord(GifFile, &GifFile->Image.Width);
    DGifGetWord(GifFile, &GifFile->Image.Height);

    GifRead(Buf, 1, GifFile);
    BitsPerPixel = (Buf[0] & 0x07) + 1;
    GifFile->Image.Interlace = (Buf[0] & 0x40);
    if (Buf[0] & 0x80) {	    /* Does this image have local color map? */

	if (GifFile->Image.ColorMap && GifFile->SavedImages == NULL)
	    FreeMapObject(GifFile->Image.ColorMap);

	GifFile->Image.ColorMap = MakeMapObject(1 << BitsPerPixel, NULL);
    
	/* Get the image local color map: */
	for (i = 0; i < GifFile->Image.ColorMap->ColorCount; i++) {
	    GifRead(Buf, 3, GifFile);
	    GifFile->Image.ColorMap->Colors[i].Red = Buf[0];
	    GifFile->Image.ColorMap->Colors[i].Green = Buf[1];
	    GifFile->Image.ColorMap->Colors[i].Blue = Buf[2];
	}
    }

    if (GifFile->SavedImages) {
	SavedImage	*sp;

	if ((GifFile->SavedImages = (SavedImage *)realloc(GifFile->SavedImages,
		    sizeof(SavedImage) * (GifFile->ImageCount + 1))) == NULL) {
	    GifInternError(GifFile, D_GIF_ERR_NOT_ENOUGH_MEM);
	}

	sp = &GifFile->SavedImages[GifFile->ImageCount];
	memcpy(&sp->ImageDesc, &GifFile->Image, sizeof(GifImageDesc));
	if (GifFile->Image.ColorMap)
	  {
	    sp->ImageDesc.ColorMap =
	      MakeMapObject (GifFile->Image.ColorMap->ColorCount,
			     GifFile->Image.ColorMap->Colors);
	  }
	sp->RasterBits = (GifPixelType *)NULL;
	sp->ExtensionBlockCount = 0;
	sp->ExtensionBlocks = (ExtensionBlock *)NULL;
    }

    GifFile->ImageCount++;

    Private->PixelCount = (long) GifFile->Image.Width *
			    (long) GifFile->Image.Height;

    DGifSetupDecompress(GifFile);  /* Reset decompress algorithm parameters. */
}

/******************************************************************************
*  Get one full scanned line (Line) of length LineLen from GIF file.	      *
******************************************************************************/
void DGifGetLine(GifFileType *GifFile, GifPixelType *Line, int LineLen)
{
    GifByteType *Dummy;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    if (!LineLen) LineLen = GifFile->Image.Width;

#if defined(__MSDOS__) || defined(__GNUC__)
    if ((Private->PixelCount -= LineLen) > 0xffff0000UL)
#else
    if ((Private->PixelCount -= LineLen) > 0xffff0000)
#endif /* __MSDOS__ */
    {    
	GifInternError(GifFile, D_GIF_ERR_DATA_TOO_BIG);
    }

    DGifDecompressLine(GifFile, Line, LineLen);
    if (Private->PixelCount == 0) {
	/* We probably would not be called any more, so lets clean 	     */
	/* everything before we return: need to flush out all rest of    */
	/* image until empty block (size 0) detected. We use GetCodeNext.*/
	do
	    DGifGetCodeNext(GifFile, &Dummy);
	while (Dummy != NULL);
    }
}

/******************************************************************************
* Put one pixel (Pixel) into GIF file.					      *
******************************************************************************/
void DGifGetPixel(GifFileType *GifFile, GifPixelType Pixel)
{
    GifByteType *Dummy;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

#if defined(__MSDOS__) || defined(__GNUC__)
    if (--Private->PixelCount > 0xffff0000UL)
#else
    if (--Private->PixelCount > 0xffff0000)
#endif /* __MSDOS__ */
    {
	GifInternError(GifFile, D_GIF_ERR_DATA_TOO_BIG);
    }

    DGifDecompressLine(GifFile, &Pixel, 1);
    if (Private->PixelCount == 0) {
	/* We probably would not be called any more, so lets clean 	     */
	/* everything before we return: need to flush out all rest of    */
	/* image until empty block (size 0) detected. We use GetCodeNext.*/
	do
	    DGifGetCodeNext(GifFile, &Dummy);
	while (Dummy != NULL);
    }
}

/******************************************************************************
*   Get an extension block (see GIF manual) from gif file. This routine only  *
* returns the first data block, and DGifGetExtensionNext shouldbe called      *
* after this one until NULL extension is returned.			      *
*   The Extension should NOT be freed by the user (not dynamically allocated).*
*   Note it is assumed the Extension desc. header ('!') has been read.	      *
******************************************************************************/
void DGifGetExtension(GifFileType *GifFile, int *ExtCode,
						    GifByteType **Extension)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    GifRead(&Buf, 1, GifFile);
    *ExtCode = Buf;

    DGifGetExtensionNext(GifFile, Extension);
}

/******************************************************************************
*   Get a following extension block (see GIF manual) from gif file. This      *
* routine sould be called until NULL Extension is returned.		      *
*   The Extension should NOT be freed by the user (not dynamically allocated).*
******************************************************************************/
void DGifGetExtensionNext(GifFileType *GifFile, GifByteType **Extension)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    GifRead(&Buf, 1, GifFile);
    if (Buf > 0) {
	*Extension = Private->Buf;           /* Use private unused buffer. */
	(*Extension)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
	GifRead(&((*Extension)[1]), Buf, GifFile);
    }
    else
	*Extension = NULL;
}

/******************************************************************************
*   This routine should be called second to last, to close the GIF file.      *
******************************************************************************/
int DGifCloseFile(GifFileType *GifFile)
{
    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;

    if (GifFile == NULL) return -1;

    if (!IS_READABLE(Private))
    {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    if (GifClose (GifFile))
    {
	GifInternError(GifFile, D_GIF_ERR_CLOSE_FAILED);
    }
    return 0;
}

/******************************************************************************
*   Get 2 bytes (word) from the given file:				      *
******************************************************************************/
static void DGifGetWord(GifFileType *GifFile, int *Word)
{
    unsigned char c[2];

    GifRead(c, 2, GifFile);

    *Word = (((unsigned int) c[1]) << 8) + c[0];
}

/******************************************************************************
*   Get the image code in compressed form.  his routine can be called if the  *
* information needed to be piped out as is. Obviously this is much faster     *
* than decoding and encoding again. This routine should be followed by calls  *
* to DGifGetCodeNext, until NULL block is returned.			      *
*   The block should NOT be freed by the user (not dynamically allocated).    *
******************************************************************************/
void DGifGetCode(GifFileType *GifFile, int *CodeSize, GifByteType **CodeBlock)
{
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    *CodeSize = Private->BitsPerPixel;

    DGifGetCodeNext(GifFile, CodeBlock);
}

/******************************************************************************
*   Continue to get the image code in compressed form. This routine should be *
* called until NULL block is returned.					      *
*   The block should NOT be freed by the user (not dynamically allocated).    *
******************************************************************************/
void DGifGetCodeNext(GifFileType *GifFile, GifByteType **CodeBlock)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    GifRead(&Buf, 1, GifFile);

    if (Buf > 0) {
	*CodeBlock = Private->Buf;	       /* Use private unused buffer. */
	(*CodeBlock)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
	GifRead(&((*CodeBlock)[1]), Buf, GifFile);
    }
    else {
	*CodeBlock = NULL;
	Private->Buf[0] = 0;		   /* Make sure the buffer is empty! */
	Private->PixelCount = 0;   /* And local info. indicate image read. */
    }

}

/******************************************************************************
*   Setup the LZ decompression for this image:				      *
******************************************************************************/
static void DGifSetupDecompress(GifFileType *GifFile)
{
    int i, BitsPerPixel;
    GifByteType CodeSize;
    unsigned int *Prefix;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    GifRead(&CodeSize, 1, GifFile);    /* Read Code size from file. */
    BitsPerPixel = CodeSize;

    Private->Buf[0] = 0;			      /* Input Buffer empty. */
    Private->BitsPerPixel = BitsPerPixel;
    Private->ClearCode = (1 << BitsPerPixel);
    Private->EOFCode = Private->ClearCode + 1;
    Private->RunningCode = Private->EOFCode + 1;
    Private->RunningBits = BitsPerPixel + 1;	 /* Number of bits per code. */
    Private->MaxCode1 = 1 << Private->RunningBits;     /* Max. code + 1. */
    Private->StackPtr = 0;		    /* No pixels on the pixel stack. */
    Private->LastCode = NO_SUCH_CODE;
    Private->CrntShiftState = 0;	/* No information in CrntShiftDWord. */
    Private->CrntShiftDWord = 0;

    Prefix = Private->Prefix;
    for (i = 0; i <= LZ_MAX_CODE; i++) Prefix[i] = NO_SUCH_CODE;
}

/******************************************************************************
*   The LZ decompression routine:					      *
*   This version decompress the given gif file into Line of length LineLen.   *
*   This routine can be called few times (one per scan line, for example), in *
* order the complete the whole image.					      *
******************************************************************************/
static void DGifDecompressLine(GifFileType *GifFile, GifPixelType *Line,
								int LineLen)
{
    int i = 0, j, CrntCode, EOFCode, ClearCode, CrntPrefix, LastCode, StackPtr;
    GifByteType *Stack, *Suffix;
    unsigned int *Prefix;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    StackPtr = Private->StackPtr;
    Prefix = Private->Prefix;
    Suffix = Private->Suffix;
    Stack = Private->Stack;
    EOFCode = Private->EOFCode;
    ClearCode = Private->ClearCode;
    LastCode = Private->LastCode;

    CrntPrefix = 0;
    if (StackPtr != 0) {
	/* Let pop the stack off before continueing to read the gif file: */
	while (StackPtr != 0 && i < LineLen) Line[i++] = Stack[--StackPtr];
    }

    while (i < LineLen) {			    /* Decode LineLen items. */
	DGifDecompressInput(GifFile, &CrntCode);

	if (CrntCode == EOFCode) {
	    /* Note however that usually we will not be here as we will stop */
	    /* decoding as soon as we got all the pixel, or EOF code will    */
	    /* not be read at all, and DGifGetLine/Pixel clean everything.   */
	    if (i != LineLen - 1 || Private->PixelCount != 0) {
		GifInternError(GifFile, D_GIF_ERR_EOF_TOO_SOON);
	    }
	    i++;
	}
	else if (CrntCode == ClearCode) {
	    /* We need to start over again: */
	    for (j = 0; j <= LZ_MAX_CODE; j++) Prefix[j] = NO_SUCH_CODE;
	    Private->RunningCode = Private->EOFCode + 1;
	    Private->RunningBits = Private->BitsPerPixel + 1;
	    Private->MaxCode1 = 1 << Private->RunningBits;
	    LastCode = Private->LastCode = NO_SUCH_CODE;
	}
	else {
	    /* Its regular code - if in pixel range simply add it to output  */
	    /* stream, otherwise trace to codes linked list until the prefix */
	    /* is in pixel range:					     */
	    if (CrntCode < ClearCode) {
		/* This is simple - its pixel scalar, so add it to output:   */
		Line[i++] = CrntCode;
	    }
	    else {
		/* Its a code to needed to be traced: trace the linked list  */
		/* until the prefix is a pixel, while pushing the suffix     */
		/* pixels on our stack. If we done, pop the stack in reverse */
		/* (thats what stack is good for!) order to output.	     */
		if (Prefix[CrntCode] == NO_SUCH_CODE) {
		    /* Only allowed if CrntCode is exactly the running code: */
		    /* In that case CrntCode = XXXCode, CrntCode or the	     */
		    /* prefix code is last code and the suffix char is	     */
		    /* exactly the prefix of last code!			     */
		    if (CrntCode == Private->RunningCode - 2) {
			CrntPrefix = LastCode;
			Suffix[Private->RunningCode - 2] =
			Stack[StackPtr++] = DGifGetPrefixChar(Prefix,
							LastCode, ClearCode);
		    }
		    else {
			GifInternError(GifFile, D_GIF_ERR_IMAGE_DEFECT);
		    }
		}
		else
		    CrntPrefix = CrntCode;

		/* Now (if image is O.K.) we should not get an NO_SUCH_CODE  */
		/* During the trace. As we might loop forever, in case of    */
		/* defective image, we count the number of loops we trace    */
		/* and stop if we got LZ_MAX_CODE. obviously we can not      */
		/* loop more than that.					     */
		j = 0;
		while (j++ <= LZ_MAX_CODE &&
		       CrntPrefix > ClearCode &&
		       CrntPrefix <= LZ_MAX_CODE) {
		    Stack[StackPtr++] = Suffix[CrntPrefix];
		    CrntPrefix = Prefix[CrntPrefix];
		}
		if (j >= LZ_MAX_CODE || CrntPrefix > LZ_MAX_CODE) {
		    GifInternError(GifFile, D_GIF_ERR_IMAGE_DEFECT);
		}
		/* Push the last character on stack: */
		Stack[StackPtr++] = CrntPrefix;

		/* Now lets pop all the stack into output: */
		while (StackPtr != 0 && i < LineLen)
		    Line[i++] = Stack[--StackPtr];
	    }
	    if (LastCode != NO_SUCH_CODE) {
		Prefix[Private->RunningCode - 2] = LastCode;

		if (CrntCode == Private->RunningCode - 2) {
		    /* Only allowed if CrntCode is exactly the running code: */
		    /* In that case CrntCode = XXXCode, CrntCode or the	     */
		    /* prefix code is last code and the suffix char is	     */
		    /* exactly the prefix of last code!			     */
		    Suffix[Private->RunningCode - 2] =
			DGifGetPrefixChar(Prefix, LastCode, ClearCode);
		}
		else {
		    Suffix[Private->RunningCode - 2] =
			DGifGetPrefixChar(Prefix, CrntCode, ClearCode);
		}
	    }
	    LastCode = CrntCode;
	}
    }

    Private->LastCode = LastCode;
    Private->StackPtr = StackPtr;
}

/******************************************************************************
* Routine to trace the Prefixes linked list until we get a prefix which is    *
* not code, but a pixel value (less than ClearCode). Returns that pixel value.*
* If image is defective, we might loop here forever, so we limit the loops to *
* the maximum possible if image O.k. - LZ_MAX_CODE times.		      *
******************************************************************************/
static int DGifGetPrefixChar(unsigned int *Prefix, int Code, int ClearCode)
{
    int i = 0;

    while (Code > ClearCode && i++ <= LZ_MAX_CODE) Code = Prefix[Code];
    return Code;
}

/******************************************************************************
*   Interface for accessing the LZ codes directly. Set Code to the real code  *
* (12bits), or to -1 if EOF code is returned.				      *
******************************************************************************/
void DGifGetLZCodes(GifFileType *GifFile, int *Code)
{
    GifByteType *CodeBlock;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_READABLE(Private)) {
	/* This file was NOT open for reading: */
	GifInternError(GifFile, D_GIF_ERR_NOT_READABLE);
    }

    DGifDecompressInput(GifFile, Code);

    if (*Code == Private->EOFCode) {
	/* Skip rest of codes (hopefully only NULL terminating block): */
	do
	    DGifGetCodeNext(GifFile, &CodeBlock);
	while (CodeBlock != NULL);

	*Code = -1;
    }
    else if (*Code == Private->ClearCode) {
	/* We need to start over again: */
	Private->RunningCode = Private->EOFCode + 1;
	Private->RunningBits = Private->BitsPerPixel + 1;
	Private->MaxCode1 = 1 << Private->RunningBits;
    }
}

/******************************************************************************
*   The LZ decompression input routine:					      *
*   This routine is responsable for the decompression of the bit stream from  *
* 8 bits (bytes) packets, into the real codes.				      *
*   Returns GIF_OK if read succesfully.					      *
******************************************************************************/
static void DGifDecompressInput(GifFileType *GifFile, int *Code)
{
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;
    GifByteType NextByte;
    static unsigned int CodeMasks[] = {
	0x0000, 0x0001, 0x0003, 0x0007,
	0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff,
	0x0fff
    };

    while (Private->CrntShiftState < Private->RunningBits) {
	/* Needs to get more bytes from input stream for next code: */
	DGifBufferedInput(GifFile, &NextByte);
	Private->CrntShiftDWord |=
		((unsigned long) NextByte) << Private->CrntShiftState;
	Private->CrntShiftState += 8;
    }
    *Code = Private->CrntShiftDWord & CodeMasks[Private->RunningBits];

    Private->CrntShiftDWord >>= Private->RunningBits;
    Private->CrntShiftState -= Private->RunningBits;

    /* If code cannt fit into RunningBits bits, must raise its size. Note */
    /* however that codes above 4095 are used for special signaling.      */
    if (++Private->RunningCode > Private->MaxCode1 &&
	Private->RunningBits < LZ_BITS) {
	Private->MaxCode1 <<= 1;
	Private->RunningBits++;
    }
}

/******************************************************************************
*   This routines read one gif data block at a time and buffers it internally *
* so that the decompression routine could access it.			      *
*   The routine returns the next byte from its internal buffer (or read next  *
* block in if buffer empty)						      *
******************************************************************************/
static void DGifBufferedInput(GifFileType *GifFile, GifByteType *NextByte)
{
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;
    GifByteType *Buf = Private->Buf;

    if (Buf[0] == 0) {
	/* Needs to read the next buffer - this one is empty: */
	GifRead(Buf, 1, GifFile);
	GifRead((Buf + 1), Buf[0], GifFile);
	*NextByte = Buf[1];
	Buf[1] = 2;	   /* We use now the second place as last char read! */
	Buf[0]--;
    }
    else {
	*NextByte = Buf[Buf[1]++];
	Buf[0]--;
    }
}

/******************************************************************************
* This routine reads an entire GIF into core, hanging all its state info off  *
* the GifFileType pointer.  Call DGifOpenFileName() or DGifOpenFileHandle()   *
* first to initialize I/O.  Its inverse is EGifSpew().			      *
******************************************************************************/
void DGifSlurp(GifFileType *GifFile)
{
    int ImageSize;
    GifRecordType RecordType;
    SavedImage *sp;
    GifByteType *ExtData;

    /* Some versions of malloc dislike 0-length requests */
    GifFile->SavedImages = (SavedImage *)malloc(sizeof(SavedImage));
    memset(GifFile->SavedImages, 0, sizeof(SavedImage));
    sp = &GifFile->SavedImages[0];

    do {
	DGifGetRecordType(GifFile, &RecordType);

	switch (RecordType) {
	    case IMAGE_DESC_RECORD_TYPE:
		DGifGetImageDesc(GifFile);

		sp = &GifFile->SavedImages[GifFile->ImageCount-1];
		ImageSize = sp->ImageDesc.Width * sp->ImageDesc.Height;

		sp->RasterBits
		    = (GifPixelType*) malloc(ImageSize * sizeof(GifPixelType));

		DGifGetLine(GifFile, sp->RasterBits, ImageSize);
		break;

	    case EXTENSION_RECORD_TYPE:
		DGifGetExtension(GifFile,&sp->Function,&ExtData);
		
		do {
    		    if (AddExtensionBlock(sp, ExtData[0], ExtData+1) == GIF_ERROR)
			GifInternError(GifFile, D_GIF_ERR_NOT_ENOUGH_MEM);
		    DGifGetExtensionNext(GifFile, &ExtData);
		} while (ExtData != NULL);
		break;

	    case TERMINATE_RECORD_TYPE:
		break;

	    default:	/* Should be trapped by DGifGetRecordType */
		break;
	}
    } while (RecordType != TERMINATE_RECORD_TYPE);
}

/******************************************************************************
* Extension record functions						      *
******************************************************************************/

void MakeExtension(SavedImage *New, int Function)
{
    New->Function = Function;
    /*
     * Someday we might have to deal with multiple extensions.
     */
}

int AddExtensionBlock(SavedImage *New, int Len, GifByteType *data)
{
    int size;
    ExtensionBlock *ep;

    if (New->ExtensionBlocks == NULL)
	New->ExtensionBlocks = (ExtensionBlock *)malloc(sizeof(ExtensionBlock));
    else
	New->ExtensionBlocks =
	    (ExtensionBlock *)realloc(New->ExtensionBlocks,
		      sizeof(ExtensionBlock) * (New->ExtensionBlockCount + 1));

    if (New->ExtensionBlocks == NULL)
	return(GIF_ERROR);

    ep = &New->ExtensionBlocks[New->ExtensionBlockCount++];
    ep->ByteCount = Len;
    size = Len * sizeof(GifByteType);
    ep->Bytes = (GifByteType *)malloc(size);
    memcpy(ep->Bytes, data, size);
    return(GIF_OK);
}

void FreeExtension(SavedImage *Image)
{
    ExtensionBlock	*ep;

    for (ep = Image->ExtensionBlocks;
	 ep < Image->ExtensionBlocks + Image->ExtensionBlockCount;
	 ep++)
	(void) free((char *)ep->Bytes);
    free((char *)Image->ExtensionBlocks);
    Image->ExtensionBlocks = NULL;
}

/******************************************************************************
* Image block allocation functions					      *
******************************************************************************/
/*
 * Append an image block to the SavedImages array  
 */
SavedImage *
MakeSavedImage(GifFileType *GifFile, SavedImage *CopyFrom)
{
    SavedImage	*sp;

    if (GifFile->SavedImages == NULL)
	GifFile->SavedImages = (SavedImage *)malloc(sizeof(SavedImage));
    else
	GifFile->SavedImages = (SavedImage *)realloc(GifFile->SavedImages,
				sizeof(SavedImage) * (GifFile->ImageCount+1));

    if (GifFile->SavedImages == NULL)
	return((SavedImage *)NULL);
    else
    {
	sp = &GifFile->SavedImages[GifFile->ImageCount++];
	memset((char *)sp, '\0', sizeof(SavedImage));

	if (CopyFrom)
	{
	    memcpy((char *)sp, CopyFrom, sizeof(SavedImage));

	    /*
	     * Make our own allocated copies of the heap fields in the
	     * copied record.  This guards against potential aliasing
	     * problems.
	     */

	    /* first, the local color map */
	    if (sp->ImageDesc.ColorMap)
		sp->ImageDesc.ColorMap =
		    MakeMapObject(CopyFrom->ImageDesc.ColorMap->ColorCount,
				  CopyFrom->ImageDesc.ColorMap->Colors);

	    /* next, the raster */
	    sp->RasterBits = (GifPixelType*)malloc(sizeof(GifPixelType)
				* CopyFrom->ImageDesc.Height
				* CopyFrom->ImageDesc.Width);
	    memcpy(sp->RasterBits,
		   CopyFrom->RasterBits,
		   sizeof(GifPixelType)
			* CopyFrom->ImageDesc.Height
			* CopyFrom->ImageDesc.Width);

	    /* finally, the extension blocks */
	    if (sp->ExtensionBlocks)
	    {
		sp->ExtensionBlocks
		    = (ExtensionBlock*)malloc(sizeof(ExtensionBlock)
					      * CopyFrom->ExtensionBlockCount);
		memcpy(sp->ExtensionBlocks,
		   CopyFrom->ExtensionBlocks,
		   sizeof(ExtensionBlock)
		   	* CopyFrom->ExtensionBlockCount);

		/*
		 * For the moment, the actual blocks can take their
		 * chances with free().  We'll fix this later. 
		 */
	    }
	}

	return(sp);
    }
}

void FreeSavedImages(GifFileType *GifFile)
{
    SavedImage	*sp;

    for (sp = GifFile->SavedImages;
	 sp < GifFile->SavedImages + GifFile->ImageCount;
	 sp++)
    {
	if (sp->ImageDesc.ColorMap)
	    FreeMapObject(sp->ImageDesc.ColorMap);

	if (sp->RasterBits)
	    free((char *)sp->RasterBits);

	if (sp->ExtensionBlocks)
	    FreeExtension(sp);
    }
    free((char *) GifFile->SavedImages);
}

/******************************************************************************
* Miscellaneous utility functions					      *
******************************************************************************/

static int BitSize(int n)
/* return smallest bitfield size n will fit in */
{
    register	int i;

    for (i = 1; i <= 8; i++)
	if ((1 << i) >= n)
	    break;
    return(i);
}

/******************************************************************************
* Color map object functions						      *
******************************************************************************/

ColorMapObject *MakeMapObject(int ColorCount, GifColorType *ColorMap)
/*
 * Allocate a color map of given size; initialize with contents of
 * ColorMap if that pointer is non-NULL.
 */
{
    ColorMapObject *Object;

    if (ColorCount != (1 << BitSize(ColorCount)))
	return((ColorMapObject *)NULL);

    Object = (ColorMapObject *)malloc(sizeof(ColorMapObject));
    if (Object == (ColorMapObject *)NULL)
	return((ColorMapObject *)NULL);

    Object->Colors = (GifColorType *)calloc(ColorCount, sizeof(GifColorType));
    if (Object->Colors == (GifColorType *)NULL)
	return((ColorMapObject *)NULL);

    Object->ColorCount = ColorCount;
    Object->BitsPerPixel = BitSize(ColorCount);

    if (ColorMap)
	memcpy((char *)Object->Colors,
	       (char *)ColorMap, ColorCount * sizeof(GifColorType));

    return(Object);
}

void FreeMapObject(ColorMapObject *Object)
/*
 * Free a color map object
 */
{
    free(Object->Colors);
    free(Object);
}
