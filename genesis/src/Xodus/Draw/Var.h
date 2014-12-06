/* $Id: Var.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Var.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
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
#ifndef _Var_h
#define _Var_h

/************************************************************
  
  Var Object.

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


#define XtNvarShapes	"varShapes"
#define XtNnshapes		"nshapes"
#define XtNshapeflag	"shapeflag"
#define XtNvarValues	"varValues"
#define XtNvarmode		"varmode"

#define XtNvarColorVal	"varColorVal"
#define XtNvarMorphVal	"varMorphVal"
#define XtNvarXOffsetVal	"varXOffsetVal"
#define XtNvarYOffsetVal	"varYOffsetVal"
#define XtNvarZOffsetVal	"varZOffsetVal"
#define XtNvarTextVal		"varTextVal"
#define XtNvarTextColorVal	"varTextColorVal"
#define XtNvarLineThicknessVal	"varLineThicknessVal"

#define XtCVarShapes		"VarShapes"
#define XtCNshapes			"Nshapes"
#define XtCShapeflag		"Shapeflag"
#define XtCVarValues		"VarValues"
#define XtCVarSelect		"VarSelect"
#define XtCVarmode			"Varmode"

#define XtRVarmode		"Varmode"

typedef struct _VarClassRec*       VarObjectClass;
typedef struct _VarRec*            VarObject;
typedef struct _VarRec*            Var;

extern WidgetClass varObjectClass;
#endif

