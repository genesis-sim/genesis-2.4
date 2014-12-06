/* $Id: ShapeP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: ShapeP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/03/22 15:33:39  bhalla
 * Eliminated transfmode
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _ShapeP_h
#define _ShapeP_h

#include "Shape.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any shape are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the shape gadget
** occupies. It is not necessary to restrict the shape to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _ShapeClassPart {
	int make_compiler_happy;
} ShapeClassPart;

typedef struct _ShapeClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  ShapeClassPart		shape_class;
} ShapeClassRec;

extern ShapeClassRec shapeClassRec;

typedef struct {
	/* resources */
	int		drawmode; /* mode for drawing */
	int		npts;	/* number of points on arrays */
	double 	*xpts; /* Coordinates for pix */
	double 	*ypts;
	double 	*zpts;
	int		linewidth;
	int 	linestyle;
	int		capstyle;
	int		joinstyle;
	String	text;
	Pixel	textcolor;
	int		textmode;
	XFontStruct	*textfont;
	/* Private fields */
	XPoint	*pts;
	int		alloced_pts;
	GC		textgc;
} ShapePart;

typedef struct _ShapeRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  ShapePart		shape;
} ShapeRec;


#endif /* _ShapeP_h */
