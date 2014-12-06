/* $Id: Dialog.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Dialog.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:33:37  venkat
 * Modified to subclass from XoComposite
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _Dialog_h
#define _Dialog_h

#include "Frame.h"
/****************************************************************
 *
 * Dialog widget
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


#ifndef XtNfont
#define XtNfont "font"
#endif

#ifndef XtCFont
#define XtCFont "Font"
#endif

typedef struct _DialogClassRec*		DialogWidgetClass;
typedef struct _DialogRec*		DialogWidget;

/* declare the class constant */

extern WidgetClass dialogWidgetClass;

#endif /* _Dialog_h */
