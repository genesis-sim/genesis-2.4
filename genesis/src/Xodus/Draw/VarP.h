/* $Id: VarP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: VarP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1996/06/20 23:23:40  venkat
 * Changed the type of the colorscale resource to long (unsigned).
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
#ifndef _VarP_h
#define _VarP_h

#include "Var.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any var are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the var gadget
** occupies. It is not necessary to restrict the var to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _VarClassPart {
	int make_compiler_happy;
} VarClassPart;

typedef struct _VarClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  VarClassPart		var_class;
} VarClassRec;

extern VarClassRec varClassRec;

typedef struct {
	/* Resources */
	varShape	*shapes; /* array of the varShape structs */
	int			nshapes;
	int			shapeflag; /* to warn of changes to shapes */
	double		*var_values; /* array of values being passed to var */
							 /* array has size XoVarNValues */
	int			varmode; /* shape, graph, surface, contour... */
	unsigned long*		colorscale;	/* color lookup table */
	int			ncols;
	int			color_val;
	int			morph_val;
	int			xoffset_val;
	int			yoffset_val;
	int			zoffset_val;
	int			text_val;
	int			textcolor_val;
	int			linethickness_val;

	/* Private fields, duplicating many of the Shape fields */
	Pixel	bg;
	int		npts;
	int		drawmode;
	String	text;
	Pixel	textcolor;
	int		textmode;
	XFontStruct *textfont;
	XPoint	*pts;
	int		alloced_pts;
	GC		textgc;
} VarPart;

typedef struct _VarRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  VarPart		var;
} VarRec;


#endif /* _VarP_h */
