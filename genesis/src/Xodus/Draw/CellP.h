/* $Id: CellP.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: CellP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.6  1996/06/27 23:46:40  venkat
 * Changed the type of the colorscale resource to long (unsigned).
 *
 * Revision 1.5  1995/07/17  20:42:05  dhb
 * Added w field for compartment screen widths to cell widget.
 *
 * Revision 1.4  1995/03/17  20:27:41  venkat
 * Upi's inclusion of cellmode resource
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _CellP_h
#define _CellP_h

#include "Cell.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any cell are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the cell gadget
** occupies. It is not necessary to restrict the cell to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _CellClassPart {
	int make_compiler_happy;
} CellClassPart;

typedef struct _CellClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  CellClassPart		cell_class;
} CellClassRec;

extern CellClassRec cellClassRec;

typedef struct {
	/* resources */
	float	colmin;
	float	colmax;
	float	diamin;
	float	diamax;
	float	diarange; /* should be screen_diamin, screen_diamax */
	int		npts;	/* number of points on arrays */
	int		soma;	/* index of soma */
	double 	*xpts; /* Coordinates for pix */
	double 	*ypts;
	double 	*zpts;
	double 	*dia;
	double 	*color;
	String	*names;
	unsigned long	*colorscale;
	int		ncols;
	int 	*fromlist;
	int 	*tolist;
	int 	*shapelist;
	int		nlist;
	int		cellmode; /* endcoords, midcoords */
	/* Private fields */
	int		*x,*y,*z,*w;
	int		*order;
	int		allocated_pts;
	double	width_scale; /* max number of pixels for a compt diam */
	int		selected;
} CellPart;

typedef struct _CellRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  CellPart		cell;
} CellRec;

#define XO_CELL_PTS_INCREMENT 100
#endif /* _CellP_h */
