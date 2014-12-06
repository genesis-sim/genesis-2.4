/* $Id: ViewP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: ViewP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1996/06/20 23:24:52  venkat
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
#ifndef _ViewP_h
#define _ViewP_h

#include "View.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any view are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the view gadget
** occupies. It is not necessary to restrict the view to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _ViewClassPart {
	int make_compiler_happy;
} ViewClassPart;

typedef struct _ViewClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  ViewClassPart		view_class;
} ViewClassRec;

extern ViewClassRec viewClassRec;

typedef struct {
	/* Resources */
	varShape	*shapes; /* array of the varShape structs */
	int			nshapes;
	int			shapeflag; /* to warn of changes to shapes */
	float		**view_values; /* array of values passed to view */
							 /* array has size XoViewNValues */
	int			viewmode; /* shape, graph, surface, contour... */
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

	/* fields distinct from the Var fields */
	double	*xpts; /* There is a possibility of confusion here, */
	double	*ypts; /* but I decided it is more important to keep */
	double	*zpts; /* to the convention of using xpts etc for the */
					/* arrays */
	float	**old_interps; /* An array for keeping track of the values
						** from the previous display, for undrawing */

	int		ncoords; /* This is the number of pts in xpts etc */
	XPoint	*coords; /* These are the projected coords of each var */
} ViewPart;

typedef struct _ViewRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  ViewPart		view;
} ViewRec;


#endif /* _ViewP_h */
