/* $Id: Draw.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Draw.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/01/20 01:13:34  venkat
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
/** Not in certain systems **/
 
/*#include <X11/copyright.h>*/

/* $XConsortium: Draw.h,v 1.4 89/07/21 01:41:49 kit Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _Draw_h
#define _Draw_h

/****************************************************************
 *
 * Draw widget
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

 axis		     Transform		char		z
 xmin		     Position		float		0.0
 ymin		     Position		float		0.0
 xmax		     Position		float		1.0
 ymax		     Position		float		1.0

*/

#define XtNtransform		"transform"

#define XtCTransform		"Transform"
#define XtRTransform		"Transform"
/*
#define XtCPosition	"Position"
*/


typedef struct _DrawClassRec*	DrawWidgetClass;
typedef struct _DrawRec*	DrawWidget;


extern WidgetClass drawWidgetClass;

#endif /* _Draw_h */


#define XtNzmin		"zmin"
#define XtNzmax		"zmax"

#define XtNmatrix	"matrix"
#define XtNrv	"rv"
#define XtNrz	"rz"
#define XtNvx	"vx"
#define XtNvy	"vy"
#define XtNvz	"vz"
#define XtNax	"ax"
#define XtNay	"ay"
#define XtNaz	"az"
#define XtNperspective	"perspective"

#define XtCMatrix "Matrix"
#define XtCPerspective "Perspective"
