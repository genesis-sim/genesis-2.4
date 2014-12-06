/* $Id: CoreDraw.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 *
*   !$Log: CoreDraw.h,v $
*   !Revision 1.1.1.1  2005/06/14 04:38:33  svitak
*   !Import from snapshot of CalTech CVS tree of June 8, 2005
*   !
*   !Revision 1.6  2000/06/12 04:28:16  mhucka
*   !Removed nested comments within header, to make compilers happy.
*   !
 * Revision 1.5  1995/03/07 00:44:58  venkat
 * Included XoComposit.h as superclass public header.
 *
 * Revision 1.4  1995/01/20  01:08:38  venkat
 * X11/CopyRight.h not included
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

/* $XConsortium: CoreDraw.h,v 1.4 89/07/21 01:41:49 kit Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _CoreDraw_h
#define _CoreDraw_h

#include "Widg/XoComposit.h"

/****************************************************************
 *
 * CoreDraw widget
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

 xmin		     Position		float		0.0
 ymin		     Position		float		0.0
 xmax		     Position		float		1.0
 ymax		     Position		float		1.0

*/

#define XtNxmin		"xmin"
#define XtNxmax		"xmax"
#define XtNymin		"ymin"
#define XtNymax		"ymax"
#define XtCDrawflags		"Drawflags"
#define XtNdrawflags		"drawflags"

typedef struct _CoreDrawClassRec*	CoreDrawWidgetClass;
typedef struct _CoreDrawRec*	CoreDrawWidget;


extern WidgetClass coredrawWidgetClass;

extern Widget XoFindNearestPix();

extern void PixelTransformPoint();
extern void PixelTransformPoints();
extern void PctTransformPoint();
extern void PctTransformPoints();


/* These defines are not mean to be generally accessible at this
** point, unlike the corresponding pixflags. */

#define XO_DRAW_REFRESHABLE_NOT		0x01
#define XO_DRAW_RESIZABLE_NOT		0x02
#define XO_DRAW_CLICKABLE_NOT		0x04


#endif /* _CoreDraw_h */
