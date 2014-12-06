/* $Id: Framed.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Framed.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  2000/05/02 06:18:08  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.2  1995/03/06 19:37:50  venkat
 * Included public header of superclass "Frame"
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _Framed_h
#define _Framed_h

#include "Frame.h"
/****************************************************************
 *
 * Framed widget
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

 elevation           Elevation          XoFrameElevation XoRaisedFrame
 highlight           BorderColor        Pixel           (at realize)
 shadow              BorderColor        Pixel           (at realize)

 childClass          ChildClass         WidgetClass     labelWidgetClass
*/

#define XtNchildClass "childClass"
#define XtCChildClass "ChildClass"

typedef struct _FramedClassRec*	        FramedWidgetClass;
typedef struct _FramedRec*		FramedWidget;

/* declare the class constant */

extern WidgetClass framedWidgetClass;

/* Function declarations. */

Widget XoGetFramedChild();

#endif /* _Framed_h */
