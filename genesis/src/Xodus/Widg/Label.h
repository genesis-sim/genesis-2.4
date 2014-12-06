/* $Id: Label.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Label.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/07/03 18:15:27  mhucka
 * Removed extra comma to eliminate compiler warnings.
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 20:03:04  venkat
 * Included public header for superclass, XoCore
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _XoLabel_h
#define _XoLabel_h

/***********************************************************************
 *
 * Label Widget
 *
 ***********************************************************************/

#include "XoCore.h"

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 cursor		     Cursor		Cursor		None
 destroyCallback     Callback		XtCallbackList	NULL
 font		     Font		XFontStruct*	XtDefaultFont
 foreground	     Foreground		Pixel		XtDefaultForeground
 height		     Height		Dimension	text height
 internalHeight	     Height		Dimension	2
 internalWidth	     Width		Dimension	4
 justify	     Justify		XoJustify	XoJustifyCenter
 label		     Label		String		NULL
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 resize		     Resize		Boolean		True
 width		     Width		Dimension	text width
 x		     Position		Position	0
 y		     Position		Position	0

*/

#ifndef XtNforeground
#define XtNforeground       "foreground"
#endif
/*
#define XtNlabel            "label"
*/
#ifndef XtNfont
#define XtNfont             "font"
#endif
#ifndef XtNinternalWidth
#define XtNinternalWidth    "internalWidth"
#endif
#ifndef XtNinternalHeight
#define XtNinternalHeight   "internalHeight"
#endif
#ifndef XtNjustify
#define XtNjustify          "justify"
#define XtCJustify          "Justify"
#endif
typedef enum {
  XoJustifyCenter,
  XoJustifyLeft,
  XoJustifyRight
} XoJustify;
#ifndef XtNresize
#define XtNresize           "resize"
#define XtCResize           "Resize"
#endif
 
/* Class record constants */

extern WidgetClass labelWidgetClass;

typedef struct _LabelClassRec *LabelWidgetClass;
typedef struct _LabelRec      *LabelWidget;

#endif /* _XoLabel_h */
/* DON'T ADD STUFF AFTER THIS #endif */
