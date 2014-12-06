/* $Id: Gif.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: Gif.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/02/02 18:49:54  bhalla
 * Initial revision
 * */
#ifndef _Gif_h
#define _Gif_h

/************************************************************
  
  Gif Object.

*/


/* Resources: inherited from Pix ..

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 destroyCallback     Callback           Pointer         NULL
 selectCallback      Callback           Pointer         NULL
 sensitive           Sensitive          Boolean         True

 x                   Position           Position        0
 y                   Position           Position        0
 width               Width              Dimension       0
 height              Height             Dimension       0

 pixX	     Position		Position	0
 pixY	     Position		Position	0
 pixZ	     Position		Position	0

 fg		     Foreground         Pixel		BlackPixel

 Own resources
 pixR		      Dimension		Dimension		1
*/

#define XtNfilename		"filename"
#define XtNfileformat		"fileformat"
#define XtNstoremode		"storemode"
#define XtNrescaleable		"rescaleable"
#ifndef XtNvalue
#define XtNvalue		"value"
#endif
#define XtNwx			"wx"
#define XtNwy			"wy"
#define XtNsrcx			"srcx"
#define XtNsrcy			"srcy"
#define XtNdstx			"dstx"
#define XtNdsty			"dsty"

#define XtCFilename		"Filename"
#define XtCFileformat		"Fileformat"
#define XtCStoremode		"Storemode"
#define XtCRescaleable	"Rescaleable"
#ifndef XtCValue
#define XtCValue		"Value"
#endif
#define XtCWx			"Wx"
#define XtCWy			"Wy"
#define XtCSrcx			"Srcx"
#define XtCSrcy			"Srcy"
#define XtCDstx			"Dstx"
#define XtCDsty			"Dsty"

typedef struct _GifClassRec*       GifObjectClass;
typedef struct _GifRec*            GifObject;
typedef struct _GifRec*            Gif;

extern WidgetClass gifObjectClass;
#endif
