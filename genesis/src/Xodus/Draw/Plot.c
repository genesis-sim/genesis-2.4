/* $Id: Plot.c,v 1.2 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Plot.c,v $
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.15  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.14  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.13  2000/05/02 06:06:42  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.12  1996/08/10 00:53:57  venkat
 * Changed SetValues to reset the plot range resources (xmaxvalue ...) if
 * the number of points == 0. The old values were retained earlier and
 * an autoscale would not provide the intended range around the plot.
 *
 * Revision 1.11  1996/06/06  20:40:25  dhb
 * Merged in 1.9.1.1.
 *
 * Revision 1.10  1995/12/06  01:06:29  venkat
 * The sx and memebers of the plot are initialized in the Initialize method.
 * They are calculated in the SetValues method only if the plot needs
 * reprojection. Unitialized  float/double values caused problems on the
 * alpha when the value is read.
 *
 * Revision 1.9.1.1  1996/06/06  20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.9  1995/07/08  01:00:11  venkat
 *  Changed some float resource default strings to reflect float values.
 *
 * Revision 1.8  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.7  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.6  1995/04/12  18:14:57  venkat
 * Made the change to calculate the sx and sy members of the plot
 * in SetValues() unconditionally. This solves the problem of
 * plots not appearing initially, but only on autoscaling.
 *
 * Revision 1.5  1994/05/26  13:45:38  bhalla
 * Added conditional for updating plots based on whether the
 * appropriate pixflags are set
 *
 * Revision 1.4  1994/04/25  17:59:55  bhalla
 * Got rid of XO_REQUEST_REDRAW and XO_ALWAYS_REFRESH and similar
 * obsolete pixflags options which were interfering with the
 * correct pixflags choices.
 * Implemented most of the pixflags options
 *
 * Revision 1.3  1994/03/22  15:30:01  bhalla
 * RCS error: no code changes
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
/* 
** The basic plot pix only needs to plot lines on the screen
** in specified colors and line thickness. At this point it assumes
** that all points will be cached, but this will change.
** At the genesis level, one would define the plot axes at two levels.
** First, the 'window' occupied by the plot on the draw
** widget, in real coords. Second, the xmin,xmax,ymin,ymax 
** for the plotted values representing the plot axes, which would occupy
** the entire area of the 'window'.
** Internally, the plot uses pix->tx,pix->ty,pix->tz and 
** plot->sx,plot->sy,plot->sz to offset and scale the data points to
** fit the real values on the screen. In more sophisticated plots
** these will be replaced by a matrix which will allow all sorts
** of awful manipulations to be done on the data points before they
** are even projected.
** Both in Genesis and internally, min and max for the point indices is
** specified.
** Finally, the whole mess has to fit within the
** screen bounding box and possibly within a 3-d box, but not in the
** initial version. 
** 
** There are 3 ways in which the gadget will need to draw to the
** screen: first, as usual, when resizing or projecting.
** second, as usual, when an expose happens
** third, which is special, is to add individual points to an existing
** plot.
** Note that for a scrolling graph the tx etc will have to change and
** the whole thing be reprojected. This may be a problem later.
** A possible fourth drawing mode would be a scrolling graph mode.
**
** As a general policy, all fancy memory handling options will be
** dealt with at the genesis object level. The only exception (which
** may justify a special pix) is when a very large plot needs to be
** done in several project-and-draw stages.
*/
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "CoreDrawP.h"
#include "PlotP.h"


#define offset(field) XtOffset(PlotObject, plot.field)
static XtResource resources[] = {
  {XtNwx, XtCWx, XtRFloat, sizeof(float),
     offset(wx), XtRString, "1.0"},
  {XtNwy, XtCWx, XtRFloat, sizeof(float),
     offset(wy), XtRString, "1.0"},
  {XtNxmin, XtCRange, XtRFloat, sizeof(float),
     offset(xmin), XtRString, "0.0"},
  {XtNxmax, XtCRange, XtRFloat, sizeof(float),
     offset(xmax), XtRString, "1.0"},
  {XtNymin, XtCRange, XtRFloat, sizeof(float),
     offset(ymin), XtRString, "0.0"},
  {XtNymax, XtCRange, XtRFloat, sizeof(float),
     offset(ymax), XtRString, "1.0"},
  {XtNxminvalue, XtCRange, XtRFloat, sizeof(float),
     offset(xminvalue), XtRString, "1e10"},
  {XtNxmaxvalue, XtCRange, XtRFloat, sizeof(float),
     offset(xmaxvalue), XtRString, "-1e10"},
  {XtNyminvalue, XtCRange, XtRFloat, sizeof(float),
     offset(yminvalue), XtRString, "1e10"},
  {XtNymaxvalue, XtCRange, XtRFloat, sizeof(float),
     offset(ymaxvalue), XtRString, "-1e10"},
  {XtNnpts, XtCNpts, XtRInt, sizeof(int),
     offset(npts), XtRString, "0"},
  {XtNfrompts, XtCNpts, XtRInt, sizeof(int),
     offset(frompts), XtRString, "0"},
  {XtNtopts, XtCNpts, XtRInt, sizeof(int),
     offset(topts), XtRString, "1000"},
  {XtNxpts, XtCPts, XtRPointer, sizeof(double*),
     offset(xpts), XtRImmediate, (XtPointer)NULL},
  {XtNypts, XtCPts, XtRPointer, sizeof(double*),
     offset(ypts), XtRImmediate, (XtPointer)NULL},
  {XtNlinewidth, XtCLineWidth, XtRInt, sizeof(int),
     offset(linewidth), XtRString, "1"},
  {XtNlinestyle, XtCLinestyle, XtRLinestyle, sizeof(int),
     offset(linestyle), XtRString, "LineSolid"},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToLinestyle();
static void ClassInitialize();
static void ResetGC();
static void Expose();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();

/* class record definition */


PlotClassRec plotClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Plot",
    /* size               */    sizeof(PlotRec),
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
    /* query_geometry     */    NULL,
    /* display_accelerator*/    NULL,
    /* extension          */    NULL
  },
  {
    /* pix Fields */
    /* project */				Project,
    /* undraw */				XoInheritUndraw,
    /* select_distance */       XoInheritSelectDistance,
    /* select */           		XoInheritSelect,
    /* unselect */         		XoInheritUnSelect,
    /* motion */         		XoInheritMotion,
    /* highlight */        		XoInheritHighlight,
    /* extension */             NULL
  },
  {
	/* plot fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass plotObjectClass = (WidgetClass)&plotClassRec;

static void ResetGC (plot)
     PlotObject plot;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground | GCLineWidth | GCLineStyle;

  
  if (plot->pix.gc != NULL)
    XtReleaseGC (XtParent((Widget)plot), plot->pix.gc);

  values.foreground = plot->pix.fg;
  values.line_width = plot->plot.linewidth;
  values.line_style = plot->plot.linestyle;
  plot->pix.gc = XtGetGC(XtParent((Widget)plot), mask, &values);

  if (plot->plot.backgrgc != NULL)
    XtReleaseGC (XtParent((Widget)plot), plot->plot.backgrgc);

  values.foreground = XtParent((Widget)plot)->core.background_pixel;
  values.line_width = plot->plot.linewidth;
  values.line_style = plot->plot.linestyle;
  plot->plot.backgrgc = XtGetGC(XtParent((Widget)plot), mask, &values);
}



static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRLinestyle,
                      XoCvtStringToLinestyle, NULL, 0 ,
                      XtCacheNone, NULL);
}

static void Initialize (req, new)
     Widget req, new;
{
  PlotObject plot = (PlotObject) new;

  /* Check that the initial values are reasonable */
	if (plot->plot.linewidth < 0 || plot->plot.linewidth > 10) 
		plot->plot.linewidth = 0;
	if (plot->plot.linestyle != LineSolid &&
		plot->plot.linestyle != LineOnOffDash &&
		plot->plot.linestyle != LineDoubleDash)
		plot->plot.linestyle = LineSolid;
		
  plot->pix.gc=NULL;
  plot->plot.backgrgc=NULL;
  ResetGC(plot);
  plot->plot.pts =
	(XPoint *) XtCalloc(XO_PLOT_PTS_INCREMENT,sizeof(XPoint));
	plot->plot.alloced_pts = XO_PLOT_PTS_INCREMENT;
  plot->pix.w = 0;	/* This ensures that the pix will not be 
						** selected */
  plot->pix.h = 0;
  plot->plot.sx = plot->plot.wx / (plot->plot.xmax - plot->plot.xmin);
  plot->plot.sy = plot->plot.wy / (plot->plot.ymax - plot->plot.ymin);
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  PlotObject curs = (PlotObject) curw;
  PlotObject news = (PlotObject) neww;
  int ret = 0;

  /* Assorted things which affect the GC */
  if (news->pix.fg != curs->pix.fg ||
	news->plot.linestyle != curs->plot.linestyle ||
	news->plot.linewidth != curs->plot.linewidth
	) {
    ResetGC(news);
	ret = 1;
  }

  /* These are the items which do not affect the projection,
  ** but do need redrawing */
  if (
	news->plot.linestyle != curs->plot.linestyle ||
	news->plot.linewidth != curs->plot.linewidth
	)
	ret = 1;

  /* This is a special case which needs memory reallocation */
  /* These are the items which do need reprojection */
  if (
  	news->plot.wx != curs->plot.wx ||
  	news->plot.wy != curs->plot.wy ||
  	news->plot.xmin != curs->plot.xmin ||
  	news->plot.xmax != curs->plot.xmax ||
  	news->plot.ymin != curs->plot.ymin ||
  	news->plot.ymax != curs->plot.ymax
  ) {
	/* calculate and check the plot frame values */
	if (news->plot.xmin == news->plot.xmax) { 
		news->plot.xmin = curs->plot.xmin;
		news->plot.xmax = curs->plot.xmax;
		return(False);
	}
	if (news->plot.ymin == news->plot.ymax) { 
		news->plot.ymin = curs->plot.ymin;
		news->plot.ymax = curs->plot.ymax;
		return(False);
	}
	if (news->plot.wx <= 0.0) { 
		news->plot.wx = curs->plot.wx;
		return(False);
	}
	if (news->plot.wy <= 0.0) { 
		news->plot.wy = curs->plot.wy;
		return(False);
	}
	news->plot.sx = news->plot.wx / (news->plot.xmax - news->plot.xmin);
	news->plot.sy = news->plot.wy / (news->plot.ymax - news->plot.ymin);
  }

  if (
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->plot.wx != curs->plot.wx ||
  	news->plot.wy != curs->plot.wy ||
  	news->plot.xmin != curs->plot.xmin ||
  	news->plot.xmax != curs->plot.xmax ||
  	news->plot.ymin != curs->plot.ymin ||
  	news->plot.ymax != curs->plot.ymax ||
  	news->plot.npts != curs->plot.npts ||
  	news->plot.xpts != curs->plot.xpts ||
  	news->plot.ypts != curs->plot.ypts
  	) {
		if (news->plot.npts != curs->plot.npts) {
			if (news->plot.npts < 0) {
  				news->plot.npts = curs->plot.npts;
			} else if (news->plot.npts == 0) {
				news->plot.xmaxvalue = -1e10;
				news->plot.xminvalue = 1e10;
				news->plot.ymaxvalue = -1e10;
				news->plot.yminvalue = 1e10;
			} else if (news->plot.npts > curs->plot.alloced_pts) {
			/* 20 is a random extra amount so that future changes
			** have a fair chance of fitting too */
				news->plot.alloced_pts =
				(1 + news->plot.npts / XO_PLOT_PTS_INCREMENT) *
				XO_PLOT_PTS_INCREMENT;
				if (news->plot.topts == curs->plot.alloced_pts)
					news->plot.topts = news->plot.alloced_pts;
				news->plot.pts =
					(XPoint *)XtRealloc((char *) news->plot.pts,
						news->plot.alloced_pts*sizeof(XPoint));
			}
		}
		if (news->plot.npts > news->plot.topts)
			news->plot.topts = news->plot.npts;
		Project(news);
		if (news->pix.pixflags & XO_UPDATE_ALL_ON_SET)
			return(True);
			/*
			XClearArea(XtDisplay(XtParent(news)),
				XtWindow(XtParent(news)), 0,0,0,0,True);
		*/
	}
	return(False);
}

  

static void Expose (w)
     Widget w;
{
  PlotObject sw = (PlotObject) w;
  XPoint	*points;

  if (sw->pix.pixflags & XO_VISIBLE_NOT) return;

	if (sw->plot.npts < 2)
		return;
	points=sw->plot.pts;
  		XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
		sw->pix.gc, points,sw->plot.npts,CoordModeOrigin); 
}

static void Destroy(w)
	Widget w;
{
  PlotObject sw = (PlotObject) w;
  if (sw->pix.gc != NULL)
  	XtReleaseGC(XtParent(w),sw->pix.gc);

  if (sw->plot.backgrgc != NULL)
    XtReleaseGC (XtParent(w), sw->plot.backgrgc);
  XtFree((char *)(sw->plot.pts));
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	PlotObject sw = (PlotObject)w;
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
*/

static void Project (w)
     Widget w;
{
  int x,y,z;
  int ox,oy,oz;
  int	nplotpts;
  double *sxpts,*sypts,*szpts;
  PlotObject sw = (PlotObject) w;
  double sx = sw->plot.sx;
  double sy = sw->plot.sy;
  double xmin = sw->plot.xmin;
  double ymin = sw->plot.ymin;
  double xmax = sw->plot.xmax;
  double ymax = sw->plot.ymax;
  double xminvalue = sw->plot.xminvalue;
  double xmaxvalue = sw->plot.xmaxvalue;
  double yminvalue = sw->plot.yminvalue;
  double ymaxvalue = sw->plot.ymaxvalue;
  double tx = sw->pix.tx;
  double ty = sw->pix.ty;
  double tz = sw->pix.tz;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int parentht= ((CoreDrawWidget)XtParent(w))->core.height;
  int	*px,*py,*pz;
  int	i,j;
  XPoint	*points;
  double temp;
  void (*tpt)();
  void (*tpts)();

  if (sw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (sw->pix.pixflags & XO_RESIZABLE_NOT)/* freeze size,position*/
  	return;
  if (sw->pix.pixflags & XO_PIXELCOORDS) { /* transform using pixels*/
        tpts = PixelTransformPoints;
  } else { /* use standard transformations */
        tpts = class->coredraw_class.transformpts;
  }
  if (sw->pix.pixflags & XO_PIXELOFFSET) { /* transform using pixels*/
        tpt = PixelTransformPoint;
  } else { /* use standard transformations */
        tpt = class->coredraw_class.transformpt;
  }
 
  if (sw->pix.pixflags & XO_PCTCOORDS) { /* transform using % of win */
        tpts = PctTransformPoints;
  }
  if (sw->pix.pixflags & XO_PCTOFFSET) { /* transform using % of win */
        tpt = PctTransformPoint;
  }

  /* we assume that the points are upto but not including npts */
  nplotpts = (sw->plot.topts > sw->plot.npts) ? sw->plot.npts :
	sw->plot.topts;
  nplotpts -= sw->plot.frompts;
  if (sw->plot.npts < 1 || nplotpts < 1)
	return;

  px = (int *) XtCalloc(nplotpts,sizeof(int));
  py = (int *) XtCalloc(nplotpts,sizeof(int));
  pz = (int *) XtCalloc(nplotpts,sizeof(int));
  sxpts = (double *) XtCalloc(nplotpts,sizeof(double));
  sypts = (double *) XtCalloc(nplotpts,sizeof(double));
  szpts = (double *) XtCalloc(nplotpts,sizeof(double));
  points = sw->plot.pts;
  
  /* Calculate origin on screen using parents TransformPoint routine */
  (tpt)(XtParent(w),
	(float)0.0,(float)0.0,(float)0.0, &ox,&oy,&oz);

  /* Calculate offset on screen using parents TransformPoint routine */
  if (!(sw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS))) {
  (tpt)(XtParent(w),
	sw->pix.tx,sw->pix.ty, sw->pix.tz,&x,&y,&z);
	sw->pix.cx = x; sw->pix.cy = y; sw->pix.cz=z;

	/* Figure out required offset for points being drawn */
	x-= ox;
	y-= oy;
	z-= oz;
  } else {
	y -= parentht; /*needed because both transforms include it*/
  }

  /* Scale up sxpts, sypts from xptx,ypts to fit in screen */
  for(i=0,j=sw->plot.frompts;i<nplotpts;i++,j++) {
	/* a quick hack to prevent points from going out of bounds.
	** Later someone can clean it up */
	temp=sw->plot.xpts[j];
	if (temp < xmin) sxpts[i] = tx;
	else if (temp > xmax) sxpts[i] = sw->plot.wx + tx;
	else sxpts[i] = sx * (temp - xmin) + tx;
	/* Keep track of value ranges for plot */
	if (xminvalue > temp) xminvalue = temp;
	else if (xmaxvalue < temp) xmaxvalue = temp;
	temp=sw->plot.ypts[j];
	if (temp < ymin) sypts[i] = ty;
	else if (temp > ymax) sypts[i] = sw->plot.wy + ty;
	else sypts[i] = sy * (temp - ymin) + ty;
	if (yminvalue > temp) yminvalue = temp;
	else if (ymaxvalue < temp) ymaxvalue = temp;
	/* szpts was already initialized to zero at calloc */
	szpts[i] = tz;
  }

  /* Do the projection */
  (tpts)(XtParent(w),
	sxpts,sypts,szpts,
		px,py,pz,nplotpts);
	/* put all the points in the cache */
	j = (sw->plot.addpts) ? sw->plot.frompts:0;
	for(i=0;i<nplotpts;i++,j++) {
		points[j].x = px[i];
		points[j].y = py[i];
	}

	/* put the value ranges back in the plot */
	/* We may at some future point wish to have a callback
	** here if the values have changed. However, this
	** update will happen very often and the callback
	** is likely to be an expensive operation so should
	** not be a default */
	sw->plot.xminvalue = xminvalue;
	sw->plot.xmaxvalue = xmaxvalue;
	sw->plot.yminvalue = yminvalue;
	sw->plot.ymaxvalue = ymaxvalue;
	
  XtFree((char *) px);
  XtFree((char *) py);
  XtFree((char *) pz);
  XtFree((char *) sxpts);
  XtFree((char *) sypts);
  XtFree((char *) szpts);
}


/* This is a utility function for adding points a small number at
** a time, without recalculating the whole mess. The added
** points are assumed to be already in the xpts,ypts arrays,
** and all that this routine needs to do is to extract them
** and add them to the local cache. If necessary, it also 
** expands the cache. We really need a callback to handle
** operations when the cache fills
*/

/* cases : adding first point - just do the transform on it, and
** put it in the cache. Do not bother with drawing it.
**	adding nth point, n>1 - do transform on new point, draw line
** to previously tranformed point
**  frompts > npts: Do not transform or draw point, just increment npts
**  topts < npts: ditto.
**  newnpts <= npts - this is a strange case. For now we will do nothing
** when this occurs.
*/

void XoAddPtsToPlot(w,newnpts)
     Widget w;
	 int	newnpts;
{
  PlotObject sw = (PlotObject) w;
  int	frompts;
  int 	apts;

  if (newnpts <= sw->plot.npts) return;

	/* Check if we have filled the old cache */
	if (newnpts > sw->plot.alloced_pts) {
		apts=(1+newnpts/XO_PLOT_PTS_INCREMENT) * XO_PLOT_PTS_INCREMENT;
		if (sw->plot.topts == sw->plot.alloced_pts)
			sw->plot.topts = apts;
		sw->plot.alloced_pts = apts;
		sw->plot.pts =
			(XPoint *)XtRealloc((char *) sw->plot.pts,
				apts*sizeof(XPoint));
	}

  if (newnpts > sw->plot.topts || newnpts <= sw->plot.frompts) {
	sw->plot.npts = newnpts;
	return;
  }

	/* do transform on all points from npts to newnpts -1 */
  	frompts = sw->plot.frompts;
	sw->plot.frompts = sw->plot.npts;
	sw->plot.npts = newnpts;
	/* Doing the projection on the subset of points */
	/* The addpts flag tells the project routine to add the new
	** points to the existing cache, rather than replace the cache */
	sw->plot.addpts = 1;
	Project(w);
	sw->plot.addpts = 0;


	/* draw lines from npts (currently in frompts) to newnpts -1 */
	if (sw->plot.frompts >=1) {
 		XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			sw->pix.gc, sw->plot.pts+sw->plot.frompts-1,
			newnpts - sw->plot.frompts + 1,
			CoordModeOrigin); 
	
		/* Flushing the display */
		if (!(sw->pix.pixflags & XO_FLUSH_NOT))
			XFlush(XtDisplay(XtParent(w)));
	}
	/* Restoring the original value for frompts */
  	sw->plot.frompts = frompts;
}

/* This function assumes that the projection has not changed at all,
** just the point values to be plotted. So it draws over the originals
** in the background color, and then redraws the new line in
** foreground 
*/
void XoUpdateWavePlot(w)
     Widget w;
{
  PlotObject sw = (PlotObject) w;

	/* draw lines from 0  to npts, in background, using cache */
	XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
		sw->plot.backgrgc,sw->plot.pts,sw->plot.npts,CoordModeOrigin); 

	/* Projecting the new plot */
	Project(w);

	/* draw lines from 0  to npts, in foreground, using cache */
	XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
		sw->pix.gc,sw->plot.pts,sw->plot.npts,CoordModeOrigin); 
	/* Flushing the display */
	if (!(sw->pix.pixflags & XO_FLUSH_NOT))
		XFlush(XtDisplay(XtParent(w)));
}





static Boolean XoCvtStringToLinestyle(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"LineSolid") == 0)
			ret = LineSolid;
		if (strcmp(fromVal->addr,"LineOnOffDash") == 0)
			ret = LineOnOffDash;
		if (strcmp(fromVal->addr,"LineDoubleDash") == 0)
			ret = LineDoubleDash;
		if (ret >=0) {
		 	XoCvtDone(int, ret);	
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRLinestyle");
		return((Boolean) False);
}
