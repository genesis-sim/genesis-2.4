/* $Id: Cell.c,v 1.2 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Cell.c,v $
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.16  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.15  2000/06/12 04:12:38  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.14  2000/05/02 06:06:41  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.13  1996/06/27 23:36:15  venkat
 * Changed type of the colorscale resource and any local variables attempting
 * to get the list of colortable indices, from int to long. On the alpha these
 * types are different sizes and information is lost in the scale-down process
 * leading to unpredictable color index values and behaviour.
 *
 * Revision 1.12  1996/06/06  20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.11  1995/11/02  20:03:18  venkat
 * XoUpdateCell() did not declare its widget argument and this was causing
 * problems on the alpha where the int and pointer sizes differ.
 *
 * Revision 1.10  1995/08/02  22:58:56  dhb
 * The screen width for the soma was not being calculated resulting in
 * the soma not being displayed.  Added this to Project().
 *
 * Revision 1.9  1995/07/21  20:45:44  dhb
 * Merged in 1.6.3.2 changes.
 *
 * Revision 1.8  1995/06/10  01:25:51  dhb
 * Partial changes for correct implementation of positive diarange
 * values.
 *
 * Revision 1.7  1995/05/12  22:18:35  dhb
 * Changed interpretation of positive diarange values.  Now it scales
 * the displayed thickness of the compartments by diarange.  For example,
 * diarange == 1 gives a true display of the diameters; diarange == 2
 * doubles the thickness of the compartments; etc.
 *
 * Revision 1.6.3.2  1995/07/21  20:43:07  dhb
 * Merged in 1.6.1.4 changes.
 *
 * Revision 1.6.3.1  1995/07/17  20:42:05  dhb
 * Changed implementation of positive diarange values to
 * draw realistic widths.  Positive diarange values are
 * now a scaling factor on the realistic widths.
 *
 * Revision 1.8  1995/06/10  01:25:51  dhb
 * Partial changes for correct implementation of positive diarange
 * values.
 *
 * Revision 1.7  1995/05/12  22:18:35  dhb
 * Changed interpretation of positive diarange values.  Now it scales
 * the displayed thickness of the compartments by diarange.  For example,
 * diarange == 1 gives a true display of the diameters; diarange == 2
 * doubles the thickness of the compartments; etc.
 *
 * Revision 1.6.1.4  1995/07/08  03:11:28  dhb
 * Changed diarange resource default to match genesis field
 * default value.
 *
 * Revision 1.6.1.3  1995/06/16  05:44:50  dhb
 * Merged in 1.6.2.1 changes.
 *
 * Revision 1.6.1.2  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.6.1.1  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.6.2.1  1995/06/16  05:38:18  dhb
 * Fixes for arithmetic errors in color lookups, by Upi Bhalla.
 *
 * Upi June 6 95: Changed color calculation to avoid arithmetic
 * overflows if the displayed value was too big for an int.
 *
 * Revision 1.6  1995/03/17  20:25:26  venkat
 * ls
 * Upi's inclusions and changes.
 *
 * Revision 1.6  1995/02/20  04:58:52  bhalla
 * 1. Some minor fixes to the updating/reprojection code in SetValues
 * so that the xcell redisplays when things like diameter are changed.
 * 2. Changed the return system to match that in Tree and View, where
 * the info->ret now points to the index of the selected compartment.
 *
 * Revision 1.5  1995/01/23  02:55:59  bhalla
 * Tried to fix problem with projections shrinking as they rotate. This
 * change seems to actually make things worse.
 *
 * Revision 1.4  1994/03/22  15:27:29  bhalla
 * Added code to check if dendrites are on screen before redrawing them
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * Changed cell.cell.selected to cell.pix.selected as part of
 * general enhancement of selection mechanism to allow subparts of
 * gadgets to be selected and dragged between
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */

/* 
**
** The Cell pix is one of the more complex at the Genesis level, but
** it will be kept simple at the X level. All it has to do is to 
** draw lines of specified thickness and color between pairs of points.
** As a slight elaboration, it will sort the line segments by depth 
** before doing the draw. As an option, text may be drawn at the 
** appropriate points. The final option is to draw a filled circle in
** the appropriate color at one of the points (to represent the soma).
** 
** The required information is in the following arrays:
** doubles: x,y,z	** coords in space of points
** doubles: dia 	** dia may represent other things, but it always
			** specifies the line thickness. +ve dia gets
			** scaled in space, -ve dia is sign flipped 
			** and used directly as screen coords
** doubles : color	** a +ve color value is scaled according to
			** the colorscale limits. -ve is used directly
** ints : fromlist,tolist ** specifies endpts of compt line segments
**	
*/

#include <math.h>
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "CoreDrawP.h"
#include "DrawP.h"
#include "CellP.h"
#include "DrawUtil.h"

#define XO_CELL_SEL_DIST 20

#define offset(field) XtOffset(CellObject, cell.field)
static XtResource resources[] = {
  {XtNcolmin, XtCRange, XtRFloat, sizeof(float),
     offset(colmin), XtRString, "0"},
  {XtNcolmax, XtCRange, XtRFloat, sizeof(float),
     offset(colmax), XtRString, "1"},
  {XtNdiamin, XtCRange, XtRFloat, sizeof(float),
     offset(diamin), XtRString, "0"},
  {XtNdiamax, XtCRange, XtRFloat, sizeof(float),
     offset(diamax), XtRString, "1"},
  {XtNdiarange, XtCRange, XtRFloat, sizeof(float),
     offset(diarange), XtRString, "-20"},
  {XtNnpts, XtCNpts, XtRInt, sizeof(int),
     offset(npts), XtRString, "0"},
  {XtNsoma, XtCSoma, XtRInt, sizeof(int),
     offset(soma), XtRString, "0"},
  {XtNxpts, XtCPts, XtRPointer, sizeof(double*),
     offset(xpts), XtRImmediate, (XtPointer)NULL},
  {XtNypts, XtCPts, XtRPointer, sizeof(double*),
     offset(ypts), XtRImmediate, (XtPointer)NULL},
  {XtNzpts, XtCPts, XtRPointer, sizeof(double*),
     offset(zpts), XtRImmediate, (XtPointer)NULL},
  {XtNnames, XtCPts, XtRPointer, sizeof(String *),
     offset(names), XtRImmediate, (XtPointer)NULL},
  {XtNdia, XtCPts, XtRPointer, sizeof(double*),
     offset(dia), XtRImmediate, (XtPointer)NULL},
  {XtNcolor, XtCPts, XtRPointer, sizeof(double*),
     offset(color), XtRImmediate, (XtPointer)NULL},
  {XtNcolorscale, XtCColorscale, XtRPointer, sizeof(unsigned long*),
     offset(colorscale), XtRImmediate, (XtPointer)NULL},
  {XtNncols, XtCRange, XtRInt, sizeof(int),
     offset(ncols), XtRString, "0"},
  {XtNfromlist, XtCFromlist, XtRPointer, sizeof(int*),
     offset(fromlist), XtRImmediate, (XtPointer)NULL},
  {XtNtolist, XtCFromlist, XtRPointer, sizeof(int*),
     offset(tolist), XtRImmediate, (XtPointer)NULL},
  {XtNshapelist, XtCFromlist, XtRPointer, sizeof(int*),
     offset(shapelist), XtRImmediate, (XtPointer)NULL},
  {XtNnlist, XtCNpts, XtRInt, sizeof(int),
     offset(nlist), XtRString, "0"},
  {XtNcellmode, XtCCellmode, XtRCellmode, sizeof(int),
     offset(cellmode), XtRString, "endcoord"},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToCellmode();
static void Expose();
static void ClassInitialize();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();
static void indexsort();
static void	draw_dend_shape();
static void 	draw_end_dend_shape();
static void Select();
static void SelectDistance();
static void Highlight();

/* class record definition */


CellClassRec cellClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Cell",
    /* size               */    sizeof(CellRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initialize*/  NULL,
    /* Class init'ed      */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    NULL,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* resource_count     */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    FALSE,
    /* compress_exposure  */    FALSE,
    /* compress_enterleave*/    FALSE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    NULL,
    /* expose             */    Expose,
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* intrinsics version */    XtVersion,
    /* callback offsets   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */    QueryGeometry,
    /* display_accelerator*/    NULL,
    /* extension          */    NULL
  },
  {
    /* pix Fields */
    /* project */				Project,
    /* undraw */				XoInheritUndraw,
    /* select_distance */       SelectDistance,
    /* select */           		Select,
    /* unselect */         		XoInheritUnSelect,
    /* motion */         		XoInheritMotion,
    /* highlight */        		Highlight,
    /* extension */             NULL
  },
  {
	/* cell fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass cellObjectClass = (WidgetClass)&cellClassRec;

static void ClassInitialize ()
{
	XtSetTypeConverter (XtRString, XtRCellmode,
						XoCvtStringToCellmode, NULL, 0,
						XtCacheNone, NULL);
}

static void Initialize (req, new)
     Widget req, new;
{
  CellObject cell = (CellObject) new;
  CoreDrawWidget dw;
  Display	*disp = XtDisplay(XtParent(new));
  Drawable	d = XtWindow(XtParent(new));
  XGCValues values;
  XtGCMask mask = GCForeground;
  values.foreground = XBlackPixel(disp,XDefaultScreen(disp));
  /*
  XtGCMask mask = GCForeground | GCLineWidth | GCCapStyle;
  values.foreground = XBlackPixel(disp,XDefaultScreen(disp));
  values.background = XWhitePixel(disp,XDefaultScreen(disp));
  */

  /* Check that the initial values are reasonable */
		
  cell->pix.gc = XCreateGC(disp,d,mask, &values);
  /* overriding the default GC from the pix widget */
  mask |= GCFunction;
  values.function = GXxor;
  cell->pix.hlgc = XCreateGC(disp,d,mask, &values);
  cell->cell.x =
	(int *) XtCalloc(XO_CELL_PTS_INCREMENT,sizeof(int));
  cell->cell.y =
	(int *) XtCalloc(XO_CELL_PTS_INCREMENT,sizeof(int));
  cell->cell.z =	
	(int *) XtCalloc(XO_CELL_PTS_INCREMENT,sizeof(int));
  cell->cell.w =	
	(int *) XtCalloc(XO_CELL_PTS_INCREMENT,sizeof(int));
  					/* this specifies the depth, which determines
					** the order of display of the line segments */
  cell->cell.order =
	(int *) XtCalloc(XO_CELL_PTS_INCREMENT,sizeof(int));
  cell->cell.allocated_pts = XO_CELL_PTS_INCREMENT;
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  CellObject curs = (CellObject) curw;
  CellObject news = (CellObject) neww;
  Boolean ret = False;
  Boolean do_proj = False;

  /* These are the items which do need reprojection */
  if (
  	news->cell.colmin != curs->cell.colmin ||
  	news->cell.colmax != curs->cell.colmax ||
  	news->cell.diarange != curs->cell.diarange ||
  	news->cell.diamin != curs->cell.diamin ||
  	news->cell.diamax != curs->cell.diamax
  ) {
	/* calculate and check the cell ranges */
	if (news->cell.colmin == news->cell.colmax) { 
		news->cell.colmin = curs->cell.colmin;
		news->cell.colmax = curs->cell.colmax;
		return(False);
	}
	if (news->cell.diamin == news->cell.diamax) { 
		news->cell.diamin = curs->cell.diamin;
		news->cell.diamax = curs->cell.diamax;
		return(False);
	}
	ret = True;
	do_proj = True;
  }
  if (
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->cell.npts != curs->cell.npts ||
  	news->cell.nlist != curs->cell.nlist
  	) {
  /* This is a special case which needs memory reallocation */
		if (news->cell.npts != curs->cell.npts) {
			if (news->cell.npts < 0) {
  				news->cell.npts = curs->cell.npts;
			} else {
				if (news->cell.npts > curs->cell.allocated_pts) {
					news->cell.allocated_pts =
					(1 + news->cell.npts / XO_CELL_PTS_INCREMENT) *
					XO_CELL_PTS_INCREMENT;
					news->cell.x = (int *)XtRealloc((char *) news->cell.x,
						news->cell.allocated_pts*sizeof(int));
					news->cell.y = (int *)XtRealloc((char *) news->cell.y,
						news->cell.allocated_pts*sizeof(int));
					news->cell.z = (int *)XtRealloc((char *) news->cell.z,
						news->cell.allocated_pts*sizeof(int));
					news->cell.w = (int *)XtRealloc((char *) news->cell.w,
						news->cell.allocated_pts*sizeof(int));
					news->cell.order=(int *)XtRealloc((char *) news->cell.order,
						news->cell.allocated_pts*sizeof(int));
				}
			}
		}
		ret = True;
		do_proj = True;
	}
	if (do_proj)
		Project(news);
	return(ret);
}

  

/* This routine does all the redrawing and handles the color
** scaling etc. Should be able to deal with the updates
** directly here.
** 
** The description of the cell is in terms of compartment segments.
** Each of these is drawn as two parts, one with the dia and color
** of the near compartment, and the other half with the dia and
** color of the far compartment. There are a few messy cases
** arising from termini and somas, but the drawing takes no more
** time. This arrangement involves twice as many drawing operations
** as the old cell widget, but enables one to handle cases where the
** soma is not known, and also multiple unconnected cells or
** parts thereof, handled by the same instance of the widget.
** There may be a problem with compartments which are shorter
** than their radius, as I use a circular cap on each line 
** segment. This problem will not arise in most cell models.
**
** Later update: I have made it possible to use the old method
** for drawing the cell, and made that the default because
** everyone is used to it and it really does work better. 
*/
static void Expose (widg)
     Widget widg;
{
  CellObject sw = (CellObject) widg;
  Widget	pa = XtParent(widg);
  int		*x = sw->cell.x;
  int		*y = sw->cell.y;
  int		*w = sw->cell.w;
  int		*fromlist = sw->cell.fromlist;
  int		*tolist = sw->cell.tolist;
  int		*shapelist = sw->cell.shapelist;
  int		i,j;
  int		fj,tj;
  unsigned long	*colscale = sw->cell.colorscale;
  double	col_lookup =
	sw->cell.ncols / (sw->cell.colmax - sw->cell.colmin);
  int		maxwid = XtParent(widg)->core.width/2;
  double	dcol;
  int		fcol = 0,tcol = 0,fwid,twid;

	if (sw->pix.pixflags & XO_VISIBLE_NOT) return;
	if (sw->cell.nlist < 1) return;
	if (sw->cell.cellmode == XoCellEndCoordMode) {
		/* This is the old way of displaying the cell */
		for(i=0;i<sw->cell.nlist;i++) {
			/* j = sw->cell.order[i]; */
			/* this order business will need some more thought */
			j = i;
			fj = fromlist[j];
	
			tj = tolist[j];
			if (sw->cell.ncols > 0) {
				dcol = (sw->cell.color[tj] - sw->cell.colmin) * col_lookup;
				if (dcol<0)
					tcol = 0;
				else if (dcol > sw->cell.ncols)
					tcol = sw->cell.ncols-1;
				else
					tcol = (int)dcol;
				/*
				tcol = (sw->cell.color[tj] - sw->cell.colmin) * col_lookup;
				if (tcol<0) tcol = 0;
				else if (tcol > sw->cell.ncols) tcol = sw->cell.ncols-1;
				*/
			}
			if (w[tj] < 0) twid = 0;
			else if (w[tj] > maxwid) twid = maxwid;
			else twid = w[tj];

			/* This function assumes that the tolist has the ends of
			** the dend branch that is further from the soma
			*/
			draw_end_dend_shape(pa,sw->pix.gc,x[tj],y[tj],x[fj],y[fj],
				shapelist[j] & XO_TO_MASK,
				twid,colscale[tcol]);

		}
		/* Now draw the soma */
		if (sw->cell.soma >= 0 && sw->cell.soma < sw->cell.npts ) {
			tj = sw->cell.soma;
			if (sw->cell.ncols > 0) {
				dcol = (sw->cell.color[tj] - sw->cell.colmin) * col_lookup;

				if (dcol<0)
					tcol = 0;
				else if (dcol > sw->cell.ncols)
					tcol = sw->cell.ncols-1;
				else
					tcol = (int)dcol;
				/*
				if (tcol<0) tcol = 0;
				else if (tcol > sw->cell.ncols) tcol = sw->cell.ncols-1;
				*/
			}
			if (w[tj] < 0) twid = 0;
			else if (w[tj] > maxwid) twid = maxwid;
			else twid = w[tj];
			if (twid > 0)
				draw_end_dend_shape(pa,sw->pix.gc,x[tj],y[tj],x[tj],y[tj],
					XO_CELL_SOMA, twid,colscale[tcol]);
				/*
				XoFillArc(XtDisplay(pa),XtWindow(pa), sw->pix.gc,
					x[tj]-twid/2,y[tj]-twid/2,
					twid,twid,0,23040);
				*/
		}
	} else if (sw->cell.cellmode == XoCellMidCoordMode) {
		for(i=0;i<sw->cell.nlist;i++) {
			/* j = sw->cell.order[i]; */
			/* this order business will need some more thought */
			j = i;
			fj = fromlist[j];
	
			if (sw->cell.ncols > 0) {
				fcol = (sw->cell.color[fj] - sw->cell.colmin) * col_lookup;
				if (fcol<0) fcol = 0;
				else if (fcol >= sw->cell.ncols) fcol = sw->cell.ncols-1;
			}
			if (w[fj] < 0) fwid = 0;
			else if (w[fj] > maxwid) fwid = maxwid;
			else fwid = w[fj];
	
			tj = tolist[j];
			if (sw->cell.ncols > 0) {
				dcol = (sw->cell.color[tj] - sw->cell.colmin) * col_lookup;
				if (dcol<0)
					tcol = 0;
				else if (dcol > sw->cell.ncols)
					tcol = sw->cell.ncols-1;
				else
					tcol = (int)dcol;
				/*
				if (tcol<0) tcol = 0;
				else if (tcol > sw->cell.ncols) tcol = sw->cell.ncols-1;
				*/
			}
			if (w[tj] < 0) twid = 0;
			else if (w[tj] > maxwid) twid = maxwid;
			else twid = w[tj];
	
			draw_dend_shape(pa,sw->pix.gc,x[fj],y[fj],x[tj],y[tj],
				(shapelist[j] & XO_FROM_MASK) >> XO_FROM_SHIFT,
				fwid,colscale[fcol]);
			draw_dend_shape(pa,sw->pix.gc,x[tj],y[tj],x[fj],y[fj],
				shapelist[j] & XO_TO_MASK,
				twid,colscale[tcol]);
	
		}
	}
	/* In due course this will be done too
	if (sw->cell.labelmode == DO_LABELS) {
		for(i=0;i<sw->cell.npts;i++) {
			XoDrawCenteredString(disp,d,sw->pix.gc,sw->pix.gc,font,
			x[i],y[i],
			sw->cell.names[i],1 + strlen(sw->cell.names[i]));
		}
	}
	*/
}

/* This routine actually draws the cell component */
/* x1,y1 is the end of the dend, x2,y2 is the parent dend */
static void	draw_end_dend_shape(w,gc,x1,y1,x2,y2,shape,wid,col)
	Widget w;
	GC	gc;
	int	x1,x2,y1,y2,shape,wid;
	unsigned long col;
{
  	Display	*disp=XtDisplay(w);
  	Drawable	d=XtWindow(w);
	int	screenwid = w->core.width;
	int screenht = w->core.height;

		XSetForeground(disp,gc,col);
		switch(shape) {
			case XO_CELL_DEND: /* the usual branch along the dendrite */
				break;
			case XO_CELL_TIP: /* the tip of the dendrite */
				break;
			case XO_CELL_SOMA:	/* the soma. */
				XoFillArc(disp,d,gc,x1-wid/2,y1-wid/2,
					wid,wid,0,23040);
				return;
				/* NOTREACHED */
				break;
			case XO_CELL_SDEND: /* dend at x1 attached to soma at x2 */
				break;
			case XO_CELL_STIP: /* tip at x1 attached to soma at x2 */
				break;
			default:
				return;
		}
		/* Check that the line is at least partly on the screen.
		** A major problem
		** arises when trying to draw a huge cell that is way off
		** scale */
	if ((x1 > 0 && y1 > 0 && x1 < screenwid && y1 < screenht) ||
		(x2 > 0 && y2 > 0 && x2 < screenwid && y2 < screenht)) {

		/* Decide what cap to use for the line. If the width is
			** larger than the length, then the line should not be
		** rounded or it will overlap too much */
		if ((int)(fabs((double)(x2-x1)) + fabs((double)(y2-y1))) < wid)
			XSetLineAttributes(disp,gc,wid,LineSolid,CapButt,JoinBevel);
		else
			XSetLineAttributes(disp,gc,wid,LineSolid,CapRound,
				JoinBevel);
		XoDrawLine(disp,d,gc,x1,y1,x2,y2);
	}
}


/* This routine actually draws the cell component */
static void	draw_dend_shape(w,gc,x1,y1,x2,y2,shape,wid,col)
	Widget w;
	GC	gc;
	int	x1,x2,y1,y2,shape,wid;
	unsigned long col;
{
  	Display	*disp=XtDisplay(w);
  	Drawable	d=XtWindow(w);
	int	len;
	int	tx,ty;
	int	screenwid = w->core.width;
	int screenht = w->core.height;

		XSetForeground(disp,gc,col);
		switch(shape) {
			case XO_CELL_DEND: /* the usual branch along the dendrite */
				x2 = (x1 + x2) / 2;
				y2 = (y1 + y2) / 2;
				break;
			case XO_CELL_TIP: /* the tip of the dendrite */
				x2 = (x1 + x2) / 2;
				y2 = (y1 + y2) / 2;
				x1 += x1 - x2;
				y1 += y1 - y2;
				break;
			case XO_CELL_SOMA:	/* the soma. */
				XoFillArc(disp,d,gc,x1-wid/2,y1-wid/2,
					wid,wid,0,23040);
				return;
				/* NOTREACHED */
				break;
			case XO_CELL_SDEND: /* dend at x1 attached to soma at x2 */
				len = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
				if (len > 0) len = sqrt((double)len);
					/* find the position of the surface of the soma */
				if (len > 0) {
					x2 = x2 + ((x1-x2) * wid) / len;
					y2 = y2 + ((y1-y2) * wid) / len;
				}
				break;
			case XO_CELL_STIP: /* tip at x1 attached to soma at x2 */
				tx = x1 + (x1 - x2)/2;
				ty = y1 + (y1 - y2)/2;
				len = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
				if (len > 0) len = sqrt((double)len);
				/* find the position of the surface of the soma */
				if (len > 0) {
					x2 = x2 + ((x1-x2) * wid) / len;
					y2 = y2 + ((y1-y2) * wid) / len;
					x1 = tx;
					y1 = ty;
				}
				break;
			default:
				return;
		}
		/* Check that the line is at least partly on the screen.
		** A major problem
		** arises when trying to draw a huge cell that is way off
		** scale */
	if ((x1 > 0 && y1 > 0 && x1 < screenwid && y1 < screenht) ||
		(x2 > 0 && y2 > 0 && x2 < screenwid && y2 < screenht)) {

		/* Decide what cap to use for the line. If the width is
			** larger than the length, then the line should not be
		** rounded or it will overlap too much */
		if ((int)(fabs(x2-x1) + fabs(y2-y1)) < wid)
			XSetLineAttributes(disp,gc,wid,LineSolid,CapButt,JoinBevel);
		else
			XSetLineAttributes(disp,gc,wid,LineSolid,CapRound,
				JoinBevel);
		XoDrawLine(disp,d,gc,x1,y1,x2,y2);
	}
}

static void Destroy(w)
	Widget w;
{
  CellObject sw = (CellObject) w;
  XtReleaseGC(XtParent(w),sw->pix.gc);
  XtFree((char *)(sw->cell.x));
  XtFree((char *)(sw->cell.y));
  XtFree((char *)(sw->cell.z));
  XtFree((char *)(sw->cell.w));
  XtFree((char *)(sw->cell.order));
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	CellObject sw = (CellObject)w;
	preferred->x = sw->pix.x;
	preferred->y = sw->pix.y;
	preferred->width = sw->pix.w;
	preferred->height = sw->pix.h;
	if (intended->request_mode & (CWX | CWY | CWWidth | CWHeight)) {
		if (preferred->x == intended->x &&  
			preferred->y == intended->y &&  
			preferred->width == intended->width &&  
			preferred->height == intended->height)
			return(XtGeometryYes);
	} else {
		if (preferred->x == sw->pix.x &&  
			preferred->y == sw->pix.y &&  
			preferred->width == sw->pix.w &&  
			preferred->height == sw->pix.h)
			return(XtGeometryYes);
	}
	return(XtGeometryAlmost);
}



/* 
** This routine takes the points in xpts,ypts, scales them,
** and transforms them to the screen coords
**
** cell.width_scale is the max allowed diam of a compt, in pixels
*/

static void Project (widg)
     Widget widg;
{
  int x,y,z;
  int ox,oy,oz;
  CellObject sw = (CellObject) widg;
  double *xpts = sw->cell.xpts;
  double *ypts = sw->cell.ypts;
  double *zpts = sw->cell.zpts;
  double tx = sw->pix.tx;
  double ty = sw->pix.ty;
  double tz = sw->pix.tz;
  int *tolist = sw->cell.tolist;
  int *fromlist = sw->cell.fromlist;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(widg));
  int parentwid= ((CoreDrawWidget)XtParent(widg))->core.width;
  int parentht= ((CoreDrawWidget)XtParent(widg))->core.height;
  int	i;
  int	maxx=0,maxy=0,minx=5000,miny=5000;
  int	maxwid;
  float	wx,wy,wz;
  double wxpts[2], wypts[2], wzpts[2];
  int	iwxpts[2], iwypts[2], iwzpts[2];
  void (*tpt)();
  void (*tpts)();

	if (sw->cell.npts == 0 ||
		sw->pix.pixflags & XO_VISIBLE_NOT ||
		sw->pix.pixflags & XO_RESIZABLE_NOT)
		return;

	/* I think that the fancy coord options are not quite right,
	** but I hardly expect them to be used */
	if (sw->pix.pixflags & XO_PIXELCOORDS)
		tpts = PixelTransformPoints;
	else if (sw->pix.pixflags & XO_PCTCOORDS)
		tpts = PctTransformPoints;
  	else
  		tpts = class->coredraw_class.transformpts;

	if (sw->pix.pixflags & XO_PIXELOFFSET)
		tpt = PixelTransformPoint;
	else if (sw->pix.pixflags & XO_PCTOFFSET)
		tpt = PctTransformPoint;
  	else
  		tpt = class->coredraw_class.transformpt;

  /* Calculate origin on screen using parents TransformPoint routine
  ** if it is usual transforms */
  /* if (!(sw->pix.pixflags & (XO_PIXELCOORDS | XOPCTCOORDS))) */
  	(tpt)(XtParent(widg),
		(float)0.0,(float)0.0,(float)0.0, &ox,&oy,&oz);

  /* Calculate offset on screen using parents TransformPoint routine */
  (tpt)(XtParent(widg),
	sw->pix.tx,sw->pix.ty, sw->pix.tz,&x,&y,&z);
	sw->pix.cx = x; sw->pix.cy = y; sw->pix.cz=z;

	/* Do the offsets */
	for(i=0;i<sw->cell.npts;i++) {
		xpts[i] +=tx;
		ypts[i] +=ty;
		zpts[i] +=tz;
	}
  /* Do the projection */
  (tpts)(XtParent(widg),
		xpts,ypts,zpts,
		sw->cell.x,sw->cell.y,sw->cell.z,sw->cell.npts);
	/* undo the offsets */
	for(i=0;i<sw->cell.npts;i++) {
		xpts[i] -=tx;
		ypts[i] -=ty;
		zpts[i] -=tz;
	}
	/* sort the points in order of increasing z */
	indexsort(sw->cell.npts,sw->cell.z-1,sw->cell.order-1);
	/* putting the order array back into zero-base form */
	for(i=0;i<sw->cell.npts;i++)
		sw->cell.order[i]--;


	/*
	** Determine the width to display for each compartment
	*/

	if (sw->cell.diarange >= 0) {
	    /*
	    ** Widths are realistic widths scaled by diarange
	    */
	    double	pvx, pvy, pvz;

	    /* Get the view vector */
	    pvx = ((DrawWidget) XtParent(widg))->draw.vx;
	    pvy = ((DrawWidget) XtParent(widg))->draw.vy;
	    pvz = ((DrawWidget) XtParent(widg))->draw.vz;

	    /*
	    ** Now for each compartment we calculate the unit cross
	    ** product of the view vector with a vector along the
	    ** compartment, scale this unit vector by the compartment
	    ** diameter times the diarange scale factor, transform
	    ** pairs of coordinates giving the location of the
	    ** compartment and the location offset by the width vector
	    ** to screen coords and finally calculate the screen width
	    ** as the distance between the transformed points.
	    */

	    maxwid = 0;
	    for (i = 0; i < sw->cell.nlist; i++) {
		int	tj, fj;
		double	scale;

		tj = tolist[i];
		fj = fromlist[i];
		scale = sw->cell.dia[tj]*sw->cell.diarange;

		XoUnitCross(pvx, pvy, pvz,
		    xpts[tj]-xpts[fj], ypts[tj]-ypts[fj], zpts[tj]-zpts[fj],
		    &wx, &wy, &wz);

		wxpts[0] = xpts[tj] + tx;
		wxpts[1] = xpts[tj] + tx + wx*scale;
		wypts[0] = ypts[tj] + ty;
		wypts[1] = ypts[tj] + ty + wy*scale;
		wzpts[0] = zpts[tj] + tz;
		wzpts[1] = zpts[tj] + tz + wz*scale;

		/*
		** Transform points, then calculate the screen
		** width from the distance between the point pair.
		*/

		(tpts)(XtParent(widg),
		    wxpts, wypts, wzpts, iwxpts, iwypts, iwzpts, 2);

		sw->cell.w[tj] = (int)XoVLength((float)(iwxpts[0] - iwxpts[1]),
						(float)(iwypts[0] - iwypts[1]),
						(float)(iwzpts[0] - iwzpts[1]));

		if (sw->cell.w[tj] > maxwid)
		    maxwid = sw->cell.w[tj];
	    }

	    if (sw->cell.soma >= 0 && sw->cell.soma < sw->cell.npts) {
		int	tj;
		double	scale;

		tj = sw->cell.soma;
		scale = sw->cell.dia[tj]*sw->cell.diarange;

		/*
		** soma doesn't have a length so use a vector
		** not parallel to view vector.
		*/

		XoUnitCross(pvx, pvy, pvz,
		    pvx+1, pvy+3, pvz+5,
		    &wx, &wy, &wz);

		wxpts[0] = xpts[tj] + tx;
		wxpts[1] = xpts[tj] + tx + wx*scale;
		wypts[0] = ypts[tj] + ty;
		wypts[1] = ypts[tj] + ty + wy*scale;
		wzpts[0] = zpts[tj] + tz;
		wzpts[1] = zpts[tj] + tz + wz*scale;

		/*
		** Transform points, then calculate the screen
		** width from the distance between the point pair.
		*/

		(tpts)(XtParent(widg),
		    wxpts, wypts, wzpts, iwxpts, iwypts, iwzpts, 2);

		sw->cell.w[tj] = (int)XoVLength((float)(iwxpts[0] - iwxpts[1]),
						(float)(iwypts[0] - iwypts[1]),
						(float)(iwzpts[0] - iwzpts[1]));

		if (sw->cell.w[tj] > maxwid)
		    maxwid = sw->cell.w[tj];
	    }

	    /*
	    ** do we still need a value for width_scale so the pick code
	    ** will work?
	    */

	    sw->cell.width_scale = maxwid;
	} else {
	    /*
	    ** Widths are scaled by diameter with diamax displaying
	    ** with a width of |diarange| pixels.
	    */

	    double	ws;

	    ws = sw->cell.width_scale = -sw->cell.diarange;

	    for (i = 0; i < sw->cell.nlist; i++) {
		int	tj;

		tj = tolist[i];
		sw->cell.w[tj] = (int)
		    ws * (sw->cell.dia[tj] - sw->cell.diamin) /
			 (sw->cell.diamax - sw->cell.diamin);
	    }

	    if (sw->cell.soma >= 0 && sw->cell.soma < sw->cell.npts) {
		int	tj;

		tj = sw->cell.soma;
		sw->cell.w[tj] = (int)
		    ws * (sw->cell.dia[tj] - sw->cell.diamin) /
			 (sw->cell.diamax - sw->cell.diamin);
	    }
	}

	if (sw->cell.width_scale > (parentwid + parentht)/4)
		sw->cell.width_scale = (parentwid + parentht)/4;
	if (sw->cell.width_scale < 0) sw->cell.width_scale = 0;

	/* find the extent of the cell on the screen */
	for(i=0;i<sw->cell.npts;i++) {
		if (maxx < sw->cell.x[i])
			maxx = sw->cell.x[i];
		if (minx > sw->cell.x[i])
			minx = sw->cell.x[i];
		if (maxy < sw->cell.y[i])
			maxy = sw->cell.y[i];
		if (miny > sw->cell.y[i])
			miny = sw->cell.y[i];
	}
	sw->pix.x = minx - 10; /* some margin for error */
	sw->pix.y = miny - 10;
	sw->pix.w = maxx -minx + 20; /* some margin for error */
	sw->pix.h = maxy -miny + 20;
}


/* This is a heapsort routine based on the one in 
** Numerical Recipes in C, Press et al. 
*/
static void indexsort(n,arrin,indx)
	int	n,*arrin,*indx;
{
	int k,j,ir,indxt,i;
	float q;

	for(j=1;j<=n;j++) indx[j] = j;
	/* It looks like evil things will happen if n == 1 */
	if (n <= 1) return;
	k=n/2+1;
	ir = n;
	for(;;) {
		if (k > 1) {
			q = arrin[(indxt=indx[--k])];
		} else {
			q = arrin[(indxt=indx[ir])];
			indx[ir]=indx[1];
			if (--ir == 1) {
				indx[1]=indxt;
				return;
			}
		}
		i=k;
		j = k + k;
		while(j<=ir) {
			if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
			if (q < arrin[indx[j]]) {
				indx[i] = indx[j];
				i=j;
				j += j;
			} else {
				j = ir + 1;
			}
		}
		indx[i] = indxt;
	}
}

/* a utility function for dealing with updates */
void XoUpdateCell(w)
 Widget w;
{
	Expose(w);
}

/* Note that this routine does need to return a sensible value in
** cz, to provide click-selection routines with a good value for z
** in three-d images
*/
static void SelectDistance (w, x, y, dist)
     Widget w;
     Position x, y;
	 Dimension *dist;
{
  float ans;
  int	i;
  int neari = -1;
  float nearr;
  CellObject cw = (CellObject) w;
  float	tx,ty;
  float	fx,fy;
  	int		*fromlist = cw->cell.fromlist;
  	int		*tolist = cw->cell.tolist;
  	double	dfx, dfy, dx, dy, dp, sqrdf;

  if (cw->pix.pixflags & XO_CLICKABLE_NOT) {
	*dist = XO_MAX_SEL_DIST;
	return;
  }

	/* Start out by setting the dist equal to the dist from the soma */
	i = cw->cell.soma;
	if (i >= 0 && i < cw->cell.npts) {
		tx = cw->cell.x[i] - x;
		ty = cw->cell.y[i] - y;
		nearr = tx * tx + ty * ty;
		neari = i;
		ans=0.5 * (cw->cell.dia[i] - cw->cell.diamin)
			* cw->cell.width_scale / (cw->cell.diamax - cw->cell.diamin);
		if (ans * ans > nearr) { /* the click point was within the soma */
			cw->pix.selected = neari;
			/* what is this about ??*/
			cw->pix.cz = cw->cell.zpts[neari];
			/* get the ans */
			if (nearr <= 0)
				ans = 0;
			else /* we need to take the sqrt of nearr */
				ans = sqrt((double)nearr);
			*dist = (Dimension)(ans > 0 ? ans : 0);
			return;
		}
	} else {
  		/* bigger than any screen, squared */
  		nearr = XO_MAX_SEL_DIST * XO_MAX_SEL_DIST;
  		neari = -1;
	}

	/* Scan all the remaining compts in terms of their line segments.
	** a point may be close to one end of the compt,
	** but the compt index (neari) may be the other.
	*/
	for(i=0;i<cw->cell.nlist;i++) {
			fx = cw->cell.x[fromlist[i]];
			fy = cw->cell.y[fromlist[i]];
			tx = cw->cell.x[tolist[i]];
			ty = cw->cell.y[tolist[i]];
			/* Some 2-d coord geom here: what is the closest distance from
			** x,y to a line defined by the endpoints fx,fy  tx,ty
			*/
			dfx = fx -tx;
			dfy = fy -ty; 
			dx = x - tx;
			dy = y - ty;
			
			dp = (dx * dfx + dy * dfy);
			sqrdf = (dfx * dfx + dfy * dfy);
			if (sqrdf > 0 && dp >= 0 && dp < sqrdf) {
				double tempx, tempy;
				tempx = dfx * dp / sqrdf - dx;
				tempy = dfy * dp / sqrdf - dy;
				ans = tempx * tempx + tempy * tempy;
				if (ans < nearr) {
					nearr = ans;
					neari = tolist[i];
				}
			}
	}
  /* Check that we found at least one point */
	if (neari < 0) {
		*dist = XO_MAX_SEL_DIST;
		cw->pix.selected = -1;
		return;
	}

	/* get the ans */
	if (nearr <= 0)
		ans = 0;
	else /* we need to take the sqrt of nearr */
		ans = sqrt((double)nearr);

	*dist = (Dimension) (ans > 0 ? ans : 0);
	if (*dist < XO_CELL_SEL_DIST) {
		cw->pix.selected = neari;
		cw->pix.cz = cw->cell.zpts[neari];
	} else {
		*dist = XO_MAX_SEL_DIST;
		cw->pix.selected = -1;
	}
}

/* The Select op must return the selected compt name */
static void Select(w,event,info)
     Widget w;
	 XButtonEvent	*event;
	 XoEventInfo    *info;
{
	CellObject cw = (CellObject) w;
	PixObjectClass class = (PixObjectClass) cw->object.widget_class;
	static int ret;

	if (cw->pix.selected < 0 || cw->pix.selected >= cw->cell.npts)
		return;
	(class->pix_class.highlight)(w,event);

    /* info->ret=cw->cell.names[cw->pix.selected]; */
    ret = cw->pix.selected;
    info->ret = (XtPointer)(&ret);
	XtCallCallbackList(w,cw->pix.callbacks, (XtPointer)info);
}

static void Highlight(w,event)
     Widget w;
	 XButtonEvent	*event;
{
	CellObject cw = (CellObject) w;
  	int		*x = cw->cell.x;
  	int		*y = cw->cell.y;
  	Display	*disp=XtDisplay(XtParent(w));
	Widget	pa = XtParent(w);
  	double	width_scale = 
		cw->cell.width_scale / (cw->cell.diamax - cw->cell.diamin);
	int i;
	int	fj,tj;
	int fwid;
	double width;
	int		maxwid = XtParent(w)->core.width/2;
	static int selected = -1;

  	/* XBlackPixel(disp,XDefaultScreen(disp)); */

    if (cw->pix.pixflags & XO_IS_SELECTED) {
        cw->pix.pixflags &= ~XO_IS_SELECTED;
		/* selected is static so it keeps last value */
    } else {
        cw->pix.pixflags |= XO_IS_SELECTED;
		selected = cw->pix.selected;
	}
	if (cw->pix.pixflags & XO_HLT1_NOT) /* do not do highlighting */
		return;
	if (selected < 0 || selected >= cw->cell.npts) return;
	width=(cw->cell.dia[selected] - cw->cell.diamin) * width_scale;
	if (width < 0) fwid = 5;
	else if (width > maxwid) fwid = maxwid+5;
	else fwid = width + 5;
	fj = selected;
	if (fj >= 0 && fj < cw->cell.npts) {
		if (cw->cell.cellmode == XoCellMidCoordMode) {
			for(i = 0 ; i < cw->cell.nlist; i++) {
				if (cw->cell.fromlist[i] == selected) {
					fj = selected;
					tj = cw->cell.tolist[i];
						draw_dend_shape(pa,cw->pix.hlgc,
							x[fj],y[fj], x[tj],y[tj],
						(cw->cell.shapelist[i] & XO_FROM_MASK) >> XO_FROM_SHIFT,
	  						fwid, XBlackPixel(disp,XDefaultScreen(disp)) );
				} else if (cw->cell.tolist[i] == selected) {
					tj = selected;
					fj = cw->cell.fromlist[i];
					draw_dend_shape(pa,cw->pix.hlgc,
						x[tj],y[tj],x[fj],y[fj],
						cw->cell.shapelist[i] & XO_TO_MASK,
	  					fwid,XBlackPixel(disp,XDefaultScreen(disp)) );
				}
			}
		} else {
			if (selected == cw->cell.soma && selected >= 0 && 
				selected < cw->cell.npts) {
				tj = selected;
				draw_end_dend_shape(pa,cw->pix.hlgc,
					x[tj],y[tj],0,0,
					XO_CELL_SOMA,
	  				fwid,XBlackPixel(disp,XDefaultScreen(disp)) );
				return;
			}
			for(i = 0 ; i < cw->cell.nlist; i++) {
				if (cw->cell.tolist[i] == selected) {
					tj = selected;
					fj = cw->cell.fromlist[i];
					draw_end_dend_shape(pa,cw->pix.hlgc,
						x[tj],y[tj],x[fj],y[fj],
						cw->cell.shapelist[i] & XO_TO_MASK,
	  					fwid,XBlackPixel(disp,XDefaultScreen(disp)) );
					return;
				}
			}
		}
	}
}

static void UnSelect(w,event)
     Widget w;
	 XButtonEvent	*event;
{
}

static void Undraw(w,event)
     Widget w;
	 XButtonEvent	*event;
{
}


static void Motion(w,event)
     Widget w;
	 XButtonEvent	*event;
{
}


static Boolean XoCvtStringToCellmode(dpy,args,num_args,fromVal,toVal,
	destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
	static int ret;
	ret = -1;

	if (fromVal->addr != NULL) {
		if (strncmp(fromVal->addr,"e",1) == 0)
			ret = XoCellEndCoordMode;
		if (strncmp(fromVal->addr,"m",1) == 0)
			ret = XoCellMidCoordMode;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRCellmode");
		return((Boolean) False);
}

