/* $Id: PlotP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: PlotP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:17  mhucka
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
#ifndef _PlotP_h
#define _PlotP_h

#include "Plot.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any plot are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the plot gadget
** occupies. It is not necessary to restrict the plot to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _PlotClassPart {
	int make_compiler_happy;
} PlotClassPart;

typedef struct _PlotClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  PlotClassPart		plot_class;
} PlotClassRec;

extern PlotClassRec plotClassRec;

typedef struct {
	/* resources */
	int		npts;	/* number of points on arrays */
	double 	*xpts; /* Coordinates for pix */
	double 	*ypts;
	int		linewidth;
	int 	linestyle;
	int		frompts;
	int		topts;
	float	wx,wy;	/* window frame sizes for plot */
	float	xmin,xmax,ymin,ymax;	/* plot range */
	float	xminvalue,xmaxvalue;	/* Range of values in plot */
	float	yminvalue,ymaxvalue;	
	/* Private fields */
	XPoint	*pts;
	int		alloced_pts;
	float	sx,sy,sz;
	int		addpts;	/* flag for adding pts during projection */
	GC		backgrgc;
} PlotPart;

typedef struct _PlotRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  PlotPart		plot;
} PlotRec;

#define XO_PLOT_PTS_INCREMENT 1000
#endif /* _PlotP_h */
