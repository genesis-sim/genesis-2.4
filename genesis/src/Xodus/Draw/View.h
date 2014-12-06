/* $Id: View.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: View.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/12/06 00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _View_h
#define _View_h

/************************************************************
  
  View Object.

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
 mode		      Mode		String		"DrawLine"
 npts		      Npts		int		0
 xpts		      Xpts		Pointer		NULL
 ypts		      Xpts		Pointer		NULL
 zpts		      Xpts		Pointer		NULL
*/

#include "VarShape.h" /* defines the varShape struct */

#ifndef XtNcolorscale
#define	XtNcolorscale	"colorscale"
#define	XtNncols	"ncols"
#define XtCColorscale	"colorscale"
#define XtCNcols	"Ncols"
#endif


#ifndef XtNvarShapes
#define XtNvarShapes	"varShapes"
#define XtCVarShapes	"VarShapes"
#endif

#ifndef XtNnshapes
#define XtNnshapes		"nshapes"
#define XtCNshapes			"Nshapes"
#endif

#ifndef XtNshapeflag
#define XtNshapeflag	"shapeflag"
#define XtCShapeflag		"Shapeflag"
#endif

#ifndef XtNviewValues
#define XtNviewValues	"viewValues"
#define XtCViewValues		"ViewValues"
#endif

#ifndef XtNviewmode
#define XtNviewmode		"viewmode"
#define XtCViewmode			"Viewmode"
#define XtRViewmode		"Viewmode"
#endif

#define XtNviewColorVal	"viewColorVal"
#define XtNviewMorphVal	"viewMorphVal"
#define XtNviewXOffsetVal	"viewXOffsetVal"
#define XtNviewYOffsetVal	"viewYOffsetVal"
#define XtNviewZOffsetVal	"viewZOffsetVal"
#define XtNviewTextVal		"viewTextVal"
#define XtNviewTextColorVal	"viewTextColorVal"
#define XtNviewLineThicknessVal	"viewLineThicknessVal"

#ifndef XtNncoords
#define XtNncoords	"ncoords"
#define XtCNcoords	"Ncoords"
#endif

#ifndef XtNxpts
#define XtNxpts	"xpts"
#endif

#ifndef XtCXpts
#define XtCXpts "Xpts"
#endif

#ifndef XtNypts
#define XtNypts	"ypts"
#endif

#ifndef XtCYpts
#define XtCYpts "Ypts"
#endif

#ifndef XtNzpts
#define XtNzpts	"zpts"
#endif

#ifndef XtCZpts
#define XtCZpts "Zpts"
#endif

#define XtCViewSelect		"ViewSelect"


typedef struct _ViewClassRec*       ViewObjectClass;
typedef struct _ViewRec*            ViewObject;
typedef struct _ViewRec*            View;

extern WidgetClass viewObjectClass;
#endif

