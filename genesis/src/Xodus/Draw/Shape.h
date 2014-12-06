/* $Id: Shape.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Shape.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/03/22 15:33:48  bhalla
 * eliminated transfmode
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _Shape_h
#define _Shape_h

/************************************************************
  
  Shape Object.

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

#define XtNdrawmode		"drawmode"
#define XtNlinewidth	"linewidth"
#define XtNlinestyle	"linestyle"
#define XtNjoinstyle	"joinstyle"
#define XtNcapstyle		"capstyle"
#define XtNtextmode		"textmode"
#define XtNtext			"text"
#define XtNtextcolor	"textcolor"
#define XtNtextfont		"textfont"
#define XtNxpts			"xpts"
#define XtNypts			"ypts"
#define XtNzpts			"zpts"
#define XtNnpts			"npts"

#define XtCMode			"Mode"
#define XtCNpts			"Npts"
#define XtCPts			"Pts"
#define XtCLineWidth	"LineWidth"

#define XtRDrawmode		"Drawmode"
#define XtRLinestyle	"Linestyle"
#define XtRCapstyle		"Capstyle"
#define XtRJoinstyle	"Joinstyle"
#define XtRTextmode		"Textmode"

typedef struct _ShapeClassRec*       ShapeObjectClass;
typedef struct _ShapeRec*            ShapeObject;
typedef struct _ShapeRec*            Shape;

extern WidgetClass shapeObjectClass;
#endif
