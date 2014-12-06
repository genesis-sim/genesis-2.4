/* $Id: Text.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Text.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/03/22 15:35:54  bhalla
 * Initial revision
 * */
#ifndef _Text_h
#define _Text_h

/****************************************************************
 *
 * Text widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

#include "Widg/Label.h"

#define XtNinitialText     "initialText"
#define XtCInitialText     "InitialText"

#define XtNpoint           "point"
#define XtCpoint           "Point"

#define XtNtextHeight	"textHeight"
#define XtCTextHeight	"TextHeight"

#define XtNeditable	   "editable"
#define XtCEditable	   "Editable"

typedef struct _TextClassRec*	TextWidgetClass;
typedef struct _TextRec*	TextWidget;

extern WidgetClass textWidgetClass;
extern char *XoGetTextValue();
extern char *XoSetTextValue();

#ifdef NOTUSED
#ifndef XtNfilename
#define XtNfilename "filename"
#endif

#ifndef XtCFilename
#define XtCFilename "Filename"
#endif
#endif

#endif /* _Text_h */

