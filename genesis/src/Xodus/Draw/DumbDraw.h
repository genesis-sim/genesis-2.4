/* $Id: DumbDraw.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: DumbDraw.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/01/20 01:11:20  venkat
 * <X11/copyright.h> not included
 *
 * Revision 1.3  1994/12/06  00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
/** This file not available in certain systems **/
/* #include <X11/copyright.h> */

/* $XConsortium: DumbDraw.h,v 1.4 89/07/21 01:41:49 kit Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _DumbDraw_h
#define _DumbDraw_h

/****************************************************************
 *
 * DumbDraw widget
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

 axis		     Axis		char		x
 xmin		     Position		float		0.0
 ymin		     Position		float		0.0
 xmax		     Position		float		1.0
 ymax		     Position		float		1.0

*/

#define XtNaxis		"axis"

#define XtCAxis		"Axis"
#define XtRAxis		"Axis"
/*
#define XtCPosition	"Position"
*/


typedef struct _DumbDrawClassRec*	DumbDrawWidgetClass;
typedef struct _DumbDrawRec*	DumbDrawWidget;


extern WidgetClass dumbdrawWidgetClass;

#endif /* _DumbDraw_h */


#define XtNzmin		"zmin"
#define XtNzmax		"zmax"
