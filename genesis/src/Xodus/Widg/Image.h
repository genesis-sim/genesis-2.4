/* $Id: Image.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Image.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/03/22 15:35:54  bhalla
 * Initial revision
 * */
/*
** Xodus : Image.h,v 1.0,  5 Feb 1990, Upi Bhalla
**
** Copyright	California Institute of Technology	1989
**
** You may hack this code as long as this notice stays in.
** It is not my fault if the code doesn't work.
*/

#ifndef _Image_h
#define _Image_h



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
#ifndef XtNcallback
#define XtNcallback		"callback"
#endif

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

/* Declare specific ImageWidget class and instance datatypes */
typedef struct	_ImageClassRec*	ImageWidgetClass;
typedef struct	_ImageRec*	ImageWidget;

/* declare the class constant */
extern WidgetClass imageWidgetClass;

#endif
