/* $Id: Axis.c,v 1.3 2005/07/20 20:01:16 svitak Exp $ */
/*
 * $Log: Axis.c,v $
 * Revision 1.3  2005/07/20 20:01:16  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:29  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.22  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.21  2000/06/12 04:11:59  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added missing type casts.
 *
 * Revision 1.20  2000/05/26 22:24:18  mhucka
 * Comment out an error message that often happens for something that
 * doesn't seem to really be a problem.
 *
 * Revision 1.19  1998/01/20 00:24:42  venkat
 * Included-stdio.h-for-prototyping-sprintf.-Caused-undefined-results-on-64-bit-Irix
 *
 * Revision 1.18  1996/06/06  20:42:17  dhb
 * Merged in 1.16.1.1 changes.
 *
 * Revision 1.17  1995/09/27  00:00:10  venkat
 * Changes-to-avoid-Alpha-FPE's-and-core-dumps
 *
 * Revision 1.16.1.1  1996/06/06  20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.16  1995/07/08  00:59:41  venkat
 *  Destroy method modified to free resources allocated by the widget
 *
 * Revision 1.15  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.14  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.13  1995/04/04  00:57:13  venkat
 * Expose() now draws single character axis units and text strings
 *
 * Revision 1.12  1995/03/24  20:33:28  venkat
 * Removed Linux ifdef from definition of expr10
 *
 * Revision 1.11  1995/03/22  18:59:18  venkat
 * SetValues() at this point sets a justification resource
 * for the x and y axes it creates. It is hard-coded at this point
 * but can be easily extended as an option for the axis justification.
 *
 * Revision 1.9  1995/03/17  20:22:51  venkat
 * Added Linux specific Upi merges.
 *
 * Revision 1.8  1994/12/21  19:28:43  dhb
 * Fixed problem in Project() where it was complaning that nlabels == 0.
 * This was attributable to two problems.  First, when axmin == axmax
 * you would get nlabels == 0.  Second, the code which makes the initial
 * esitmate of nlabels based on screen size of the axis and the label
 * width or height is based on if the axis is basically horizontal or
 * vertical.  This test wasn't correct when the computed x and/or y values
 * for the axis length were negative.
 *
 * For the first condition, put in an explicit test and set axis label and
 * tick values such that the axis is not displayed.  In the second case,
 * modified the code to use absolute values of the computed axis x and y
 * lengths.
 *
 * Revision 1.7  1994/11/07  22:25:36  bhalla
 * Cleaned up the axis drag-to-scale code. It still occasionally
 * croaks, but the scaling is much cleaner when it works.
 *
 * Revision 1.6  1994/05/26  13:41:27  bhalla
 * Major overhaul. The algorithm for placing labels is changed, and now
 * it does not impose its own ideas on the min and max limits of the axis.
 * Also major changes in how the graph widget creates and manages the
 * axis - all this is now done in the Genesis end of things.
 *
 * Revision 1.5  1994/05/25  13:46:13  bhalla
 * Got rid of dependence on XO_REQUEST_REDRAW in pixflags
 *
 * Revision 1.4  1994/04/07  13:29:02  bhalla
 * Added some stuff to avoid problems with rounding off
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * Fixed ANSI warnings - cast strlen to int where needed
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>	/* abs() */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "CoreDrawP.h"
#include "GraphP.h"
#include "AxisP.h"

#define exp10(x) exp(2.3025851 * (x))

#define DRAWLINES 0
#define DRAWSEGS 1
#define DRAWPOINTS 2
#define DRAWRECTS 3
#define DRAWARROWS 4
#define FILLRECTS 5
#define FILLPOLY 6

#define	TEXTDRAW 0
#define	TEXTFILL 1
#define	TEXTNODRAW 2
#define	TEXTRADRAW 3
#define	TEXTRAFILL 4

#define	TICKAUTO 0
#define	TICKFIFTH 1
#define	TICKTENTH 2
#define	TICKNONE 3

#define XO_DEFAULT_NLABELS 100
#define XO_LABEL_LEN 10
#define XO_DEFAULT_NTICKS 200
#define XO_MAX_TICK_PIXELS 10

#ifndef EPSILON
#define EPSILON 1e-6
#endif

#define xoffset(field) XtOffset(AxisObject, axis.field)
static XtResource resources[] = {
  {XtNlinewidth, XtCLinewidth, XtRInt, sizeof(int),
     xoffset(linewidth), XtRString, "1"},
  {XtNlinestyle, XtCLinestyle, XtRLinestyle, sizeof(int),
     xoffset(linestyle), XtRString, "LineSolid"},
  {XtNtextcolor, XtCForeground, XtRXoPixel, sizeof(Pixel),
     xoffset(textcolor), XtRString, "XtDefaultForeground"},
  {XtNaxisTextmode, XtCAxisTextmode, XtRAxisTextmode, sizeof(int),
     xoffset(textmode), XtRString, "draw"},
  {XtNtextfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     xoffset(textfont), XtRString, "XtDefaultFont"},
  {XtNunits, XtCLabel, XtRString, sizeof(String),
     xoffset(units), XtRString, (XtPointer)NULL},
  {XtNunitsjustify, XtCUnitsJustify, XtRString, sizeof(String),
     xoffset(unitsjustify), XtRString, (XtPointer)NULL},
  {XtNtitle, XtCLabel, XtRString, sizeof(String),
     xoffset(title), XtRString, (XtPointer)NULL},
  {XtNunitoffset, XtCPosition, XtRFloat, sizeof(float),
     xoffset(unitoffset), XtRString, "1"},
  {XtNtitleoffset, XtCPosition, XtRFloat, sizeof(float),
     xoffset(titleoffset), XtRString, "1"},
  {XtNlabeloffset, XtCPosition, XtRFloat, sizeof(float),
     xoffset(labeloffset), XtRString, "1"},
  {XtNrotatelabels, XtCMode, XtRInt, sizeof(int),
     xoffset(rotatelabels), XtRString, "0"},
  {XtNticklength, XtCPosition, XtRFloat, sizeof(float),
     xoffset(ticklength), XtRString, "1"},
  {XtNtickmode, XtCMode, XtRTickmode, sizeof(int),
     xoffset(tickmode), XtRString, "auto"},
  {XtNaxx, XtCPosition, XtRFloat, sizeof(float),
     xoffset(axx), XtRString, "1"},
  {XtNaxy, XtCPosition, XtRFloat, sizeof(float),
     xoffset(axy), XtRString, "0"},
  {XtNaxz, XtCPosition, XtRFloat, sizeof(float),
     xoffset(axz), XtRString, "0"},
  {XtNtickx, XtCPosition, XtRFloat, sizeof(float),
     xoffset(tickx), XtRString, "1"},
  {XtNticky, XtCPosition, XtRFloat, sizeof(float),
     xoffset(ticky), XtRString, "0"},
  {XtNtickz, XtCPosition, XtRFloat, sizeof(float),
     xoffset(tickz), XtRString, "0"},
  {XtNaxmin, XtCPosition, XtRFloat, sizeof(float),
     xoffset(axmin), XtRString, "0"},
  {XtNaxmax, XtCPosition, XtRFloat, sizeof(float),
     xoffset(axmax), XtRString, "1"},
  {XtNlabmin, XtCPosition, XtRFloat, sizeof(float),
     xoffset(labmin), XtRString, "0"},
  {XtNlabmax, XtCPosition, XtRFloat, sizeof(float),
     xoffset(labmax), XtRString, "1"},
  {XtNaxlength, XtCAxLength, XtRFloat, sizeof(float),
     xoffset(axlen), XtRString, "1"},
  {XtNrefreshCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
     xoffset(refresh_callbacks), XtRCallback, (XtPointer)NULL},
};
#undef xoffset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToTickmode();
static Boolean XoCvtStringToLinestyle();
static Boolean XoCvtStringToAxisTextmode();
static void ClassInitialize();
static void ResetGC();
static void ResetTextGC();
static void Expose();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();
static void XoAxisMotionHandler();
static void XoAxisSelect();
static void XoAxisUnSelect();
static void pad_print();

/* a bunch of definitions for handling plot update callbacks */
static void XoUpdatePlotX();
static void XoUpdatePlotY();
static void XoDeleteUpdatePlotXCallback();
static void XoDeleteUpdatePlotYCallback();
static void XoDeleteDeleteXCallback();
static void XoDeleteDeleteYCallback();

/* class record definition */


AxisClassRec axisClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Axis",
    /* size               */    sizeof(AxisRec),
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
    /* select */  	         	XoAxisSelect,
    /* unselect */           	XoAxisUnSelect,
    /* motion */ 	          	XoAxisMotionHandler,
    /* highlight */ 	       	XoInheritHighlight,
    /* extension */             NULL
  },
  {
	/* axis fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass axisObjectClass = (WidgetClass)&axisClassRec;

static void ResetGC (axis)
     AxisObject axis;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground | GCLineStyle | GCLineWidth;
  
  if (axis->pix.gc != NULL)
    XtReleaseGC (XtParent((Widget)axis), axis->pix.gc);

  values.foreground = axis->pix.fg;
  values.line_style = axis->axis.linestyle;
  values.line_width = axis->axis.linewidth;
  axis->pix.gc = XtGetGC(XtParent((Widget)axis), mask, &values);
}

static void ResetTextGC (axis)
     AxisObject axis;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCBackground | GCForeground | GCFont;
  
  if (axis->axis.textgc != NULL)
    XtReleaseGC (XtParent((Widget)axis), axis->axis.textgc);

  values.foreground = axis->axis.textcolor;
  values.background = XtParent((Widget)axis)->core.background_pixel;
  values.font = axis->axis.textfont->fid;
  axis->axis.textgc = XtGetGC(XtParent((Widget)axis), mask, &values);
}

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRTickmode,
                      XoCvtStringToTickmode, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRAxisTextmode,
                      XoCvtStringToAxisTextmode, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRLinestyle,
                      XoCvtStringToLinestyle, NULL, 0 ,
                      XtCacheNone, NULL);
}


static void Initialize (req, new)
     Widget req, new;
{
  AxisObject axis = (AxisObject) new;
  int i;

  /* Check that the initial values are reasonable */
	if (axis->axis.linewidth < 0 || axis->axis.linewidth > 10) 
		axis->axis.linewidth = 0;
	if (axis->axis.linestyle != LineSolid &&
		axis->axis.linestyle != LineOnOffDash &&
		axis->axis.linestyle != LineDoubleDash)
		axis->axis.linestyle = LineSolid;
		
	if (axis->axis.textmode != TEXTDRAW &&
		axis->axis.textmode != TEXTFILL &&
		axis->axis.textmode != TEXTRADRAW &&
		axis->axis.textmode != TEXTRAFILL &&
		axis->axis.textmode != TEXTNODRAW)
		axis->axis.textmode = TEXTDRAW;

  axis->pix.gc=NULL;
  ResetGC(axis);
  axis->axis.textgc=NULL;
  ResetTextGC(axis);
  axis->axis.ticks =
	(XSegment *) XtCalloc(XO_DEFAULT_NTICKS,sizeof(XSegment));
	axis->axis.alloced_ticks = XO_DEFAULT_NTICKS;
  axis->axis.labels = (char **)(XtCalloc(XO_DEFAULT_NLABELS,
	sizeof(char*)));
  for(i=0;i<XO_DEFAULT_NLABELS;i++)
  	axis->axis.labels[i] = (char *)(XtCalloc(XO_LABEL_LEN,sizeof(char)));
  axis->axis.labelx =
	(int *) XtCalloc(XO_DEFAULT_NLABELS,sizeof(int));
  axis->axis.labely =
	(int *) XtCalloc(XO_DEFAULT_NLABELS,sizeof(int));
	axis->axis.alloced_labels = XO_DEFAULT_NLABELS;
  axis->axis.axx = 1;
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  AxisObject curs = (AxisObject) curw;
  AxisObject news = (AxisObject) neww;
  Boolean ret = False;

  /* Assorted things which affect the GC */
  if (news->pix.fg != curs->pix.fg ||
	news->axis.linestyle != curs->axis.linestyle ||
	news->axis.linewidth != curs->axis.linewidth) {
    ResetGC(news);
	ret = True;
  }

  /* Assorted things which affect the text GC */
  if (news->axis.textcolor != curs->axis.textcolor ||
	news->axis.textfont != curs->axis.textfont) {
  	ResetTextGC(news);
	ret = True;
  }

  /* These are the items which do not affect the projection,
  ** but do need redrawing */
  if (news->axis.linestyle != curs->axis.linestyle ||
	news->axis.linewidth != curs->axis.linewidth ||
	news->axis.textcolor != curs->axis.textcolor ||
	news->axis.textfont != curs->axis.textfont
	)
	ret = True;

  /* This is a special case which needs memory reallocation */
  if (news->axis.units != curs->axis.units) {
	if (curs->axis.units != NULL) XtFree((char *) curs->axis.units);
	news->axis.units = XtNewString(news->axis.units);
	ret = True;
  }
  if (news->axis.title != curs->axis.title) {
	if (curs->axis.title != NULL) XtFree((char *) curs->axis.title);
	news->axis.title = XtNewString(news->axis.title);
	ret = True;
  }
  /* These are the items which do need reprojection */
  if (
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->axis.unitoffset != curs->axis.unitoffset ||
  	news->axis.titleoffset != curs->axis.titleoffset ||
  	news->axis.labeloffset != curs->axis.labeloffset ||
  	news->axis.rotatelabels != curs->axis.rotatelabels ||
  	news->axis.ticklength != curs->axis.ticklength ||
  	news->axis.tickmode != curs->axis.tickmode ||
  	news->axis.textmode != curs->axis.textmode ||
  	news->axis.axx != curs->axis.axx ||
  	news->axis.axy != curs->axis.axy ||
  	news->axis.axz != curs->axis.axz ||
  	news->axis.tickx != curs->axis.tickx ||
  	news->axis.ticky != curs->axis.ticky ||
  	news->axis.tickz != curs->axis.tickz ||
  	news->axis.axmin != curs->axis.axmin ||
  	news->axis.axmax != curs->axis.axmax ||
  	news->axis.axlen != curs->axis.axlen) {
		Project(news);
		ret = True;
	}
	return(False);
}

  

static void Expose (w)
     Widget w;
{
  AxisObject sw = (AxisObject) w;
  GraphWidget gw = (GraphWidget)(XtParent(w));
  XSegment	*ticks;
  int	i,j;

	/* Need to do a check on the mode before doing this */
	ticks=sw->axis.ticks;
  	XoDrawSegments(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
		sw->pix.gc, ticks,sw->axis.nticks,CoordModeOrigin); 

  if (sw->axis.labels != NULL &&
  	sw->axis.labels[0] != NULL &&
 	 (int)strlen(sw->axis.labels[0]) > 0) {
  	switch(sw->axis.textmode) {
		case TEXTDRAW:
			/* Note that labels are drawn upto and including nlabels */
			for(i=0;i<=sw->axis.nlabels;i++)
				XoDrawCenteredString(XtDisplay(XtParent(w)),
					XtWindow(XtParent(w)), sw->axis.textgc,
					sw->axis.textfont,
					sw->axis.labelx[i],
					sw->axis.labely[i],
					sw->axis.labels[i],strlen(sw->axis.labels[i]));
			break;
		case TEXTFILL:
			for(i=0;i<=sw->axis.nlabels;i++)
				XoFillCenteredString(XtDisplay(XtParent(w)),
					XtWindow(XtParent(w)), sw->axis.textgc,
					sw->axis.textfont,
					sw->axis.labelx[i],
					sw->axis.labely[i],
					sw->axis.labels[i],strlen(sw->axis.labels[i]));
			break;
		case TEXTRADRAW:
			for(i=0;i<=sw->axis.nlabels;i++)
				XoDrawRtAlignedString(XtDisplay(XtParent(w)),
					XtWindow(XtParent(w)), sw->axis.textgc,
					sw->axis.textfont,
					sw->axis.labelx[i],
					sw->axis.labely[i],
					sw->axis.labels[i],strlen(sw->axis.labels[i]));
			break;
		case TEXTRAFILL:
			for(i=0;i<=sw->axis.nlabels;i++)
				XoFillRtAlignedString(XtDisplay(XtParent(w)),
					XtWindow(XtParent(w)), sw->axis.textgc,
					sw->axis.textfont,
					sw->axis.labelx[i],
					sw->axis.labely[i],
					sw->axis.labels[i],strlen(sw->axis.labels[i]));
			break;
		case TEXTNODRAW:
			/* do not draw anything */
			break;
		default:
			break;
  	}
  }
  if (sw->axis.units != NULL &&
 	(int)strlen(sw->axis.units) > 0) {
	if (sw->axis.unitsjustify != NULL &&
	     strcmp(sw->axis.unitsjustify, "Right")==0) 
		XoDrawRtAlignedString(XtDisplay(XtParent(w)),
			XtWindow(XtParent(w)), sw->axis.textgc,
			sw->axis.textfont,
			sw->axis.unitsx,
			sw->axis.unitsy,
			sw->axis.units,strlen(sw->axis.units));
	if (sw->axis.unitsjustify != NULL &&
	    strcmp(sw->axis.unitsjustify, "Left")==0) 
		XoDrawLtAlignedString(XtDisplay(XtParent(w)),
			XtWindow(XtParent(w)), sw->axis.textgc,
			sw->axis.textfont,
			gw->core.x,
			sw->axis.unitsy,
			sw->axis.units,strlen(sw->axis.units));
  }
  if (sw->axis.title != NULL &&
 	(int)strlen(sw->axis.title) > 0) {
		if (sw->axis.rotatelabels)
			XoDrawVertString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), sw->axis.textgc,
				sw->axis.textfont,
				sw->axis.titlex,
				sw->axis.titley,
				sw->axis.title,strlen(sw->axis.title));
		else
			XoDrawCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), sw->axis.textgc,
				sw->axis.textfont,
				sw->axis.titlex,
				sw->axis.titley,
				sw->axis.title,strlen(sw->axis.title));
  }
}

static void Destroy(w)
	Widget w;
{
	int i;

  AxisObject sw = (AxisObject) w;
  XtReleaseGC(XtParent(w),sw->pix.gc);
  XtReleaseGC(XtParent(w),sw->axis.textgc);
  if(sw->axis.units != NULL) XtFree(sw->axis.units);
  if(sw->axis.title != NULL) XtFree(sw->axis.title);
  XtFree((char *)(sw->axis.ticks));
  for(i=0;i<XO_DEFAULT_NLABELS;i++)
  	XtFree((char *)(sw->axis.labels[i]));
  XtFree((char *)(sw->axis.labels));
  XtFree((char *)(sw->axis.labelx));
  XtFree((char *)(sw->axis.labely));
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	AxisObject sw = (AxisObject)w;
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


static int findmin(x1,x2,x3,x4,x5,x6)
	int	x1,x2,x3,x4,x5,x6;
{
	x1=(x1<x2) ? x1 : x2;
	x3=(x3<x4) ? x3 : x4;
	x5=(x5<x6) ? x5 : x6;

	x1=(x1<x3) ? x1 : x3;

	x1=(x1<x5) ? x1 : x5;
	return(x1);
}

static int findmax(x1,x2,x3,x4,x5,x6)
	int	x1,x2,x3,x4,x5,x6;
{
	x1=(x1>x2) ? x1 : x2;
	x3=(x3>x4) ? x3 : x4;
	x5=(x5>x6) ? x5 : x6;

	x1=(x1>x3) ? x1 : x3;

	x1=(x1>x5) ? x1 : x5;
	return(x1);
}


static void Project (w)
     Widget w;
{
  int x,y,z;
  int ox,oy,oz;
  AxisObject sw = (AxisObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int	i,j,k;
  int	nlabels;
  static char	sigstr[] = "88888888888888888888888888888888";
  double	axrange;
  double	extended_axrange;
  int	axscreenrange;
  int	nsigfigmin,nsigfigmax,nsigfig;
  int	labwidth,labheight;
  double labmin,labmax,dlabel,lab;
  int	nticks;
  double	dtick,lastick,ticklen,lt;

  if (sw->axis.alloced_ticks <= 0)
	return;

    /*
    ** Check for axis on which we can't place labels/ticks.  Set
    ** up so that nothing gets displayed.
    */
    if (sw->axis.axmin == sw->axis.axmax) {
	sw->axis.nlabels = -1;
	sw->axis.nticks = 0;
	return;
    }

  /* Calculate origin on screen using parents TransformPoint routine */
  (class->coredraw_class.transformpt)(XtParent(w),
	0.0, 0.0, 0.0, &ox,&oy,&oz);

  /* Calculate offset on screen using parents TransformPoint routine */
  (class->coredraw_class.transformpt)(XtParent(w),
	sw->pix.tx,sw->pix.ty, sw->pix.tz,&x,&y,&z);
	sw->pix.cx = x; sw->pix.cy = y; sw->pix.cz=z;

	/* Find length of axis on screen, in pixels */
  (class->coredraw_class.transformpt)(XtParent(w),
	sw->axis.axx * sw->axis.axlen,
	sw->axis.axy * sw->axis.axlen,
	sw->axis.axz * sw->axis.axlen,
	&x,&y,&z);
	x-=ox; y-=oy; z-=oz;

	if (x == 0 && y == 0)
		return;

	axscreenrange = sqrt((double)(x * x + y * y));

	/* Calculate number of sigfig needed for labels */
	axrange = fabs(sw->axis.axmax - sw->axis.axmin);
	if (axrange == 0.0) axrange = 1;
	nsigfigmin = (sw->axis.axmin == 0) ? 0 :
		(int)(1 + fabs(log10(fabs(sw->axis.axmin/axrange))));
	nsigfigmax = (sw->axis.axmax == 0) ? 0 :
		(int)(1 + fabs(log10(fabs(sw->axis.axmax/axrange))));
	nsigfig = (nsigfigmin < nsigfigmax) ? nsigfigmax : nsigfigmin;
	nsigfig = (nsigfig < 2) ? 2 : nsigfig;
	/* Here we find the number of figs before the decimal place.
	** we assume that if there are more than 5, the xx.yyezz
	** form of the %g option will take over
	** we use nsigfig as a temporary variable here
	*/
	nsigfig = (sw->axis.axmin == 0) ? 1 : 1 + fabs(log10(fabs(sw->axis.axmin)));
	if (nsigfig > 8) nsigfig = 8;
	nsigfigmin += nsigfig;
	nsigfig = (sw->axis.axmax == 0) ? 1 : 1 + fabs(log10(fabs(sw->axis.axmax)));
	if (nsigfig > 8) nsigfig = 8;
	nsigfigmax += nsigfig;

	nsigfig = (nsigfigmin < nsigfigmax) ? nsigfigmax : nsigfigmin;
	nsigfig = (nsigfig < 4) ? 4 : nsigfig;

	/* Estimate width in pixels of label with this many sigfigs */
	labwidth=XTextWidth(sw->axis.textfont,sigstr,nsigfig);
	labheight=sw->axis.textfont->max_bounds.ascent+
		sw->axis.textfont->max_bounds.descent;
	/* Estimate max number of labels that will fit on graph,
	** considering rotation etc */
	/* Note that nlabels is actually 1 less than the total number
	** of labels. It really refers to the number of label intervals */

	/* Code below fails when x and/or y have negative */
	/* values.  Use abs(x) and abs(y).  ---DHB */
	if (abs(x) > 2 * abs(y))	/* The axis is < 30deg off the horizontal, need
					** to use label width to est number */
		nlabels = axscreenrange/labwidth;
	else if (abs(y) > 2 * abs(x))
		nlabels = axscreenrange/labheight;
	else
		nlabels = axscreenrange/(labheight * 2);

	if (nlabels < 1) nlabels = 1;
	else if (nlabels > 10) nlabels = 10;
	/* We might wish to do something with fonts here, instead */

	/* Chose between multiples of 1eX, 2eX and 5eX to get nearest
	** to max no of labels. Need at least 2 */
	/* Labmin and labmax should be multiples of dlabel, which should
	** be 1, 2 or 5 into a power of 10 */


	dlabel =exp10(log10(axrange/nlabels)-floor(log10(axrange/nlabels)));
	if (dlabel < 2 + EPSILON) dlabel = 2;
	else if (dlabel < 5 + EPSILON) dlabel = 5;
	else if (dlabel < 10 + EPSILON) dlabel = 10;
	else if (dlabel > 10 - EPSILON) dlabel = 1; /* should not happen */
	else  /* This should never happen */
		dlabel = 2;
	dlabel *= exp10(floor(log10(axrange/nlabels)));
	if (dlabel == 0.0) dlabel = 1;

	labmin = ceil(sw->axis.axmin/dlabel - EPSILON) * dlabel;
	labmax = floor(sw->axis.axmax/dlabel + EPSILON) * dlabel;
	nlabels = (int)(0.001 + (labmax-labmin)/dlabel);
	if (nlabels < 1 || nlabels > sw->axis.alloced_labels) {
	        /* 2000-05-23 <mhucka@caltech.edu>  Some situations result in
		   this error, but it's basically harmless.  After some debate,
		   we decided to just suppress the warning for now.
		printf("Error in Axis.c: number of labels = %d out of range \n",nlabels);
		*/
		return;
	}

	/* Generate labels */
	for(i=0,lab=labmin;i<=nlabels;i++,lab+=dlabel) {
		/* To get rid of stupid sigfig when it should be zero */
		if (fabs(lab) < EPSILON * dlabel)
			lab = 0.0;
		sprintf(sw->axis.labels[i],"%g",lab);
	}

	/* Get first approx to number of ticks */
	nticks = (axscreenrange * ((labmax - labmin)/axrange) /
		XO_MAX_TICK_PIXELS) / nlabels;
	/* Refine it to sensible number */
	if (nticks < 1) nticks = 1;
	else if (nticks < 5) nticks = 2;
	else if (nticks < 10) nticks = 5;
	else  nticks = 10; /* In this case we want intermediate ticks too */

	/* Format subdivisions of ticks as 1eX, 2eX, 5eX etc */
	/* Generate ticks */
	k = 0; /* counting variable for tick marks */
	dtick = (labmax - labmin)/(float)(nticks * nlabels);
	/* Locate the axis */
	/* x and y are at the far end of the axis in screen coords */
	sw->axis.ticks[k].x1 = sw->pix.cx;
	sw->axis.ticks[k].y1 = sw->pix.cy;
	sw->axis.ticks[k].x2 = sw->pix.cx+x;
	sw->axis.ticks[k].y2 = sw->pix.cy+y;
	k++;
	/* Locate the labels */
	lastick = labmin - sw->axis.axmin;
	for(i=0;i<=nlabels;i++,lastick += dlabel) {
		lt = lastick / axrange;
  		(class->coredraw_class.transformpt)(XtParent(w),
			sw->pix.tx + sw->axis.axx * lt +
				sw->axis.tickx * sw->axis.labeloffset,
			sw->pix.ty + sw->axis.axy * lt +
				sw->axis.ticky * sw->axis.labeloffset,
			sw->pix.tz + sw->axis.axz * lt +
				sw->axis.tickz * sw->axis.labeloffset,
			&x,&y,&z);
		sw->axis.labelx[i] = x;
		sw->axis.labely[i] = y;
	}
	/* locate the tick marks */
	lastick = (labmin - sw->axis.axmin)/dtick;
	j = (int)lastick; /* this is the integral part */
	lastick = (lastick - j) * dtick; /* this is the fractional part */

	/* we need slightly more than axrange to handle truncation err */
	extended_axrange = axrange * (1.0 + EPSILON);
	/* note that k has already been set up to be 1 */
	for(; lastick <= extended_axrange; lastick += dtick, j++, k++) {
		lt = lastick / axrange;
		if (j >= nticks || j == 0) {
			j = 0;
			ticklen = sw->axis.ticklength;
		} else {
			ticklen = sw->axis.ticklength/2.0;
		}
		(class->coredraw_class.transformpt)(XtParent(w),
					sw->pix.tx + sw->axis.axx * lt,
					sw->pix.ty + sw->axis.axy * lt,
					sw->pix.tz + sw->axis.axz * lt,
					&x,&y,&z);
		sw->axis.ticks[k].x1 = x;
		sw->axis.ticks[k].y1 = y;
  		(class->coredraw_class.transformpt)(XtParent(w),
					sw->pix.tx + sw->axis.axx * lt +
					sw->axis.tickx * ticklen,
					sw->pix.ty + sw->axis.axy * lt +
					sw->axis.ticky * ticklen,
					sw->pix.tz + sw->axis.axz * lt +
					sw->axis.tickz * ticklen,
					&x,&y,&z);
		sw->axis.ticks[k].x2 = x;
		sw->axis.ticks[k].y2 = y;
	}
	sw->axis.nticks = k;
	sw->axis.nlabels = nlabels;
	sw->axis.labmin = labmin;
	sw->axis.labmax = labmax;

	/* Locate the units */
	if (sw->axis.rotatelabels) {
  		(class->coredraw_class.transformpt)(XtParent(w),
			sw->pix.tx +
				sw->axis.axx * sw->axis.axlen
				* (1 + sw->axis.unitoffset) +
				sw->axis.tickx * sw->axis.labeloffset,
			sw->pix.ty +
				sw->axis.axy * sw->axis.axlen
				* (1 + sw->axis.unitoffset) +
				sw->axis.ticky * sw->axis.labeloffset,
			sw->pix.tz +
				sw->axis.axz * sw->axis.axlen 
				* (1 + sw->axis.unitoffset) +
				sw->axis.tickz * sw->axis.labeloffset,
			&x,&y,&z);
	} else {
  		(class->coredraw_class.transformpt)(XtParent(w),
			sw->pix.tx +
				sw->axis.axx * sw->axis.axlen +
				sw->axis.tickx * sw->axis.unitoffset,
			sw->pix.ty +
				sw->axis.axy * sw->axis.axlen +
				sw->axis.ticky * sw->axis.unitoffset,
			sw->pix.tz +
				sw->axis.axz * sw->axis.axlen +
				sw->axis.tickz * sw->axis.unitoffset,
			&x,&y,&z);
	}
	sw->axis.unitsx = x;
	sw->axis.unitsy = y;

	/* Locate the title */
  	(class->coredraw_class.transformpt)(XtParent(w),
		sw->pix.tx + sw->axis.axx * sw->axis.axlen/2.0 +
			sw->axis.tickx * sw->axis.titleoffset,
		sw->pix.ty + sw->axis.axy * sw->axis.axlen/2.0 +
			sw->axis.ticky * sw->axis.titleoffset,
		sw->pix.tz + sw->axis.axz * sw->axis.axlen/2.0 +
			sw->axis.tickz * sw->axis.titleoffset,
		&x,&y,&z);
	sw->axis.titlex = x;
	sw->axis.titley = y;

  /* Set the rectangle geometry fields. These may need to be
  ** enlarged to account for the labels*/

  sw->pix.x=findmin(sw->axis.labelx[0],sw->axis.labelx[nlabels],
	sw->axis.unitsx,sw->axis.titlex,ox,sw->axis.ticks[k-1].x1) -
	labwidth/2;
  sw->pix.w=findmax(sw->axis.labelx[0],sw->axis.labelx[nlabels],
	sw->axis.unitsx,sw->axis.titlex,ox,sw->axis.ticks[k-1].x1) +
	labwidth - sw->pix.x;
	
  sw->pix.y=findmin(sw->axis.labely[0],sw->axis.labely[nlabels],
	sw->axis.unitsy,sw->axis.titley,oy,sw->axis.ticks[k-1].y1) -
	labheight;
  sw->pix.h=findmax(sw->axis.labely[0],sw->axis.labely[nlabels],
	sw->axis.unitsy,sw->axis.titley,oy,sw->axis.ticks[k-1].y1) +
	labheight - sw->pix.y;
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
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRLinestyle");
		return((Boolean) False);
}

static Boolean XoCvtStringToAxisTextmode(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"draw") == 0)
			ret = TEXTDRAW;
		if (strcmp(fromVal->addr,"fill") == 0)
			ret = TEXTFILL;
		if (strcmp(fromVal->addr,"radraw") == 0)
			ret = TEXTRADRAW; /* Right Aligned */
		if (strcmp(fromVal->addr,"rafill") == 0)
			ret = TEXTRAFILL;
		if (strncmp(fromVal->addr,"no",2) == 0)
			ret = TEXTNODRAW;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRAxisTextmode");
		return((Boolean) False);
}

static Boolean XoCvtStringToTickmode(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"auto") == 0)
			ret = TICKAUTO;
		if (strcmp(fromVal->addr,"fifth") == 0)
			ret = TICKFIFTH;
		if (strcmp(fromVal->addr,"tenth") == 0)
			ret = TICKTENTH;
		if (strcmp(fromVal->addr,"none") == 0)
			ret = TICKNONE;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRTickmode");
		return((Boolean) False);
}

/* What follows is a series of callback routines used for
** associating axes with plots. It gets a little messy because
** we need to remove the callbacks in the event of either of the
** plot or axis being deleted.
*/

static void XoUpdatePlotX(w,client_data,call_data)
	Widget	w;
	XtPointer	client_data, call_data;
{
	AxisObject aw = (AxisObject) w;
	Widget plot = (Widget) client_data;
	char	strmin[40];
	char	strmax[40];

	sprintf(strmin,"%g",aw->axis.labmin);
	sprintf(strmax,"%g",aw->axis.labmax);

	XoXtVaSetValues(plot,
		XtVaTypedArg,XtNxmin,XtRString,strmin,strlen(strmin)+1,
		XtVaTypedArg,XtNxmax,XtRString,strmax,strlen(strmax)+1,
		NULL);
}

static void XoUpdatePlotY(w,client_data,call_data)
	Widget	w;
	XtPointer	client_data, call_data;
{
	AxisObject aw = (AxisObject) w;
	Widget plot = (Widget) client_data;
	char	strmin[40];
	char	strmax[40];

	sprintf(strmin,"%g",aw->axis.labmin);
	sprintf(strmax,"%g",aw->axis.labmax);

	XoXtVaSetValues(plot,
		XtVaTypedArg,XtNymin,XtRString,strmin,strlen(strmin)+1,
		XtVaTypedArg,XtNymax,XtRString,strmax,strlen(strmax)+1,
		NULL);
}

/* Delete references to the plot in the
** axis update and delete callbacks */
static void XoDeleteUpdatePlotXCallback(w,client_data,call_data)
	Widget	w;
	XtPointer	client_data, call_data;
{
	Widget axis = (Widget) client_data;
	Widget plot = w;

	XtRemoveCallback(axis,XtNcallback,XoUpdatePlotX,(XtPointer)plot);
	XtRemoveCallback(axis,XtNdestroyCallback,
		XoDeleteDeleteXCallback,(XtPointer)plot);
}

static void XoDeleteUpdatePlotYCallback(w,client_data,call_data)
	Widget	w;
	XtPointer	client_data, call_data;
{
	Widget axis = (Widget) client_data;
	Widget plot = w;

	XtRemoveCallback(axis,XtNcallback,XoUpdatePlotY,(XtPointer)plot);
	XtRemoveCallback(axis,XtNdestroyCallback,
		XoDeleteDeleteYCallback,(XtPointer)plot);
}

/* Delete references to the axis in the plot delete callback */
static void XoDeleteDeleteXCallback(w,client_data,call_data)
	Widget	w;
	XtPointer	client_data, call_data;
{
	Widget plot = (Widget) client_data;
	Widget axis = w;

	XtRemoveCallback(plot,XtNdestroyCallback,
		XoDeleteUpdatePlotXCallback, (XtPointer)axis);
}

static void XoDeleteDeleteYCallback(w,client_data,call_data)
	Widget	w;
	XtPointer	client_data, call_data;
{
	Widget plot = (Widget) client_data;
	Widget axis = w;

	XtRemoveCallback(plot,XtNdestroyCallback,
		XoDeleteUpdatePlotYCallback, (XtPointer)axis);
}

void AddXAxisToPlot(axis,plot)
	Widget axis,plot;
{
	/* Add callback to axis on min/max update for update of plot */

	XtAddCallback(axis, XtNcallback, XoUpdatePlotX, (XtPointer)plot);

	/* Add callback to plot on plot delete for removal of the above
	** callback */
	XtAddCallback(plot, XtNdestroyCallback, XoDeleteUpdatePlotXCallback,
		(XtPointer)axis);

	/* Add callback to axis on axis delete for removal of the 
	** above callback */
	XtAddCallback(axis, XtNdestroyCallback, XoDeleteDeleteXCallback,
		(XtPointer)plot);

	/* Call the update function so that the plot is now scaled 
	** correctly to the axis */
	XoUpdatePlotX(axis,(XtPointer) plot,(XtPointer)NULL);
}

void AddYAxisToPlot(axis,plot)
	Widget axis,plot;
{
	/* Add callback to axis on min/max update for update of plot */

	XtAddCallback(axis, XtNcallback, XoUpdatePlotY, (XtPointer)plot);

	/* Add callback to plot on plot delete for removal of the above
	** callback */
	XtAddCallback(plot, XtNdestroyCallback, XoDeleteUpdatePlotYCallback,
		(XtPointer)axis);

	/* Add callback to axis on axis delete for removal of the 
	** above callback */
	XtAddCallback(axis, XtNdestroyCallback, XoDeleteDeleteYCallback,
		(XtPointer)plot);
	/* Call the update function so that the plot is now scaled 
	** correctly to the axis */
	XoUpdatePlotY(axis,(XtPointer) plot,(XtPointer)NULL);
}

void DropXAxisFromPlot(axis,plot)
	Widget axis,plot;
{
	XoDeleteUpdatePlotXCallback(plot,(XtPointer)axis,(XtPointer)NULL);
	XoDeleteDeleteXCallback(axis,(XtPointer)plot,(XtPointer)NULL);
}

void DropYAxisFromPlot(axis,plot)
	Widget axis,plot;
{
	XoDeleteUpdatePlotYCallback(plot,(XtPointer)axis,(XtPointer)NULL);
	XoDeleteDeleteYCallback(axis,(XtPointer)plot,(XtPointer)NULL);
}


/* This is the Select method for the axis. */

static void XoAxisSelect(w,event,info)
     Widget w;
	 XButtonEvent	*event;
	XoEventInfo    *info;
{
	AxisObject aw = (AxisObject) w;
	int i,r,minr;

	minr = 1000000;
	for(i=0;i<=aw->axis.nlabels;i++) {
		r = fabs((double)(aw->axis.labelx[i] - event->x)) + 
			fabs((double)(aw->axis.labely[i] - event->y));
		if (r < minr) {
			minr = r;
			aw->axis.clickpt = i;
		}
	}
	aw->axis.labmin = aw->axis.axmin;
	aw->axis.labmax = aw->axis.axmax;
	/* printf("Clickpt = %d\n",aw->axis.clickpt); */

	XtCallCallbackList(w,aw->pix.callbacks, (XtPointer)info);
}

static void XoAxisUnSelect(w,event)
     Widget w;
	 XButtonEvent	*event;
{
	AxisObject aw = (AxisObject) w;
	static XoEventInfo info;
	/*
	static float	x[3];
	*/

	Project(w);
#if 0
	x[0] = aw->axis.labmin;
	x[1] = aw->axis.labmax;
	x[2] = 2; /* Flag to tell the graph widget to do the update */

	XtCallCallbackList(w,aw->axis.refresh_callbacks, (XtPointer)x);
#endif
	info.event = XoUpdateFields;
	info.x = info.y = info.z = 0;
	info.ret = "axmin axmax labmin labmax";
	XtCallCallbackList(w,aw->pix.callbacks, (XtPointer)&info);
}

#define XO_AXIS_DEAD_ZONE 0.05
#define XO_AXIS_LIN_LIMIT 0.5
#define XO_AXIS_EXP_FACTOR 5

/* Calculate this one as follows:
** At the lower LIN_LIMIT (LL)
** labmin - range * LL / LL = labmin - range * NORM * exp(LL * FACTOR)
** So, NORM = exp(-LL * FACTOR)
*/
#define XO_AXIS_EXP_NORM 82.085e-3

/* The following are motion handlers for the axis pix */
static void XoAxisMotionHandler(axis,event)
	Widget	axis;
	XEvent	*event;
	/* String	*params;
	Cardinal *num_params;
	*/
{
	AxisObject aw = (AxisObject) axis;
	XMotionEvent *e = (XMotionEvent *)event;
	int	labelmax = aw->axis.nlabels;
	float	temp = 0.0;
	char	tempstr[40];
	double	frac;
	float	scale;
	static	float lastfrac = 0;
	int		temp1,temp2,absdist;

	/* Make sure that everything is nicely initialized */
	if (aw->axis.nlabels <= 1) return;

	/* Find fractional distance along axis. 0 is at min, 1 at max.
	** This is easy, using the dot product. Let r = motion pt,
	** r1 = min of axis, r2 = max of axis. Then
	** (r - r1).(r2 - r1) / |r2 - r1| 
	** this is fractional distance. hey for vector geometry ! */
	temp1 = aw->axis.ticks[0].x2 - aw->axis.ticks[0].x1;
	temp2 = aw->axis.ticks[0].y2 - aw->axis.ticks[0].y1;
	absdist = (temp1 * temp1 + temp2 * temp2);
	frac = (double)((e->x - aw->axis.ticks[0].x1) * temp1 +
			(e->y - aw->axis.ticks[0].y1) * temp2) / (float)absdist;

	/*
	printf("In axis motion handler at %d %d, frac=%f\n",e->x,e->y,frac);
	*/

	/* Note that the labmin,labmax are currently set equal to the
	** starting axmin,axmax */
	/* Switch according to start postion, scale according to
	** start postion */
	/* Lower bound: scale logarithmically if curr position is 
	** below min coord. scale linearly otherwise. Sign flip if
	** too far. Update value on lower bound */
	if (aw->axis.clickpt == 0) { /* lower bound */
		if (fabs(frac) < XO_AXIS_DEAD_ZONE) {
			temp = aw->axis.labmin;
		} else if (frac > 0) { /* can't allow it to exceed +ve bound */
			if (frac < 0.999) /* use linear scaling */
				temp = aw->axis.labmin +
					(aw->axis.labmax - aw->axis.labmin) * frac;
		} else {
			if (frac > -XO_AXIS_LIN_LIMIT) /* use linear scaling */
				temp = aw->axis.labmin +
					(aw->axis.labmax - aw->axis.labmin) * frac / 
						XO_AXIS_LIN_LIMIT;
			else /* Use exponential scaling */
				temp = aw->axis.labmin -
					(aw->axis.labmax - aw->axis.labmin) * 
					XO_AXIS_EXP_NORM *
					exp(-frac*XO_AXIS_EXP_FACTOR);
		}

		if (temp == lastfrac) return;
		pad_print(tempstr,temp,lastfrac,aw,0);
		aw->axis.axmin = lastfrac = temp;
		return;
	}
	/* Upper bound: converse of lower bound. */
	if (aw->axis.clickpt >= aw->axis.nlabels) { /* upper bd */
		frac -= 1.0;
		if (fabs(frac) < XO_AXIS_DEAD_ZONE) {
			temp = aw->axis.labmax;
		} else if (frac < 0) { /*can't allow it to exceed lower bound */
			if (frac > -0.999) /* use linear scaling */
				temp = aw->axis.labmax +
					(aw->axis.labmax - aw->axis.labmin) * frac;
		} else {
			if (frac < XO_AXIS_LIN_LIMIT) /* use linear scaling */
				temp = aw->axis.labmax +
					(aw->axis.labmax - aw->axis.labmin)
						* frac / XO_AXIS_LIN_LIMIT;
			else /* Use exponential scaling */
				temp = aw->axis.labmax +
					(aw->axis.labmax - aw->axis.labmin)*
					XO_AXIS_EXP_NORM *
					exp(frac * XO_AXIS_EXP_FACTOR);
		}
		pad_print(tempstr,temp,lastfrac,aw,labelmax);
		aw->axis.axmax = lastfrac = temp;
		return;
	}
	/* middle: shift lin/log depending on current position.
	** Keep the range fixed. update both upper and lower bound values */
	if (aw->axis.clickpt <= aw->axis.nlabels/2 + 1 && 
		aw->axis.clickpt >= aw->axis.nlabels/2 -1) { /* middle */
		frac -= 0.5;
		if (fabs(frac) < XO_AXIS_DEAD_ZONE) {
			temp = 0;
		} else if (fabs(frac) < XO_AXIS_LIN_LIMIT)  {/* linear region */
			temp = (aw->axis.labmax - aw->axis.labmin) * frac / XO_AXIS_LIN_LIMIT;
		} else {
			temp = (aw->axis.labmax - aw->axis.labmin)*
				XO_AXIS_EXP_NORM *
				exp(XO_AXIS_EXP_FACTOR * fabs(frac));
			if (frac < 0) temp = -temp;
		}

		aw->axis.axmin = aw->axis.labmin + temp;
		aw->axis.axmax = aw->axis.labmax + temp;
		pad_print(tempstr,	temp+aw->axis.labmin,
							lastfrac+aw->axis.labmin,aw,0);
		pad_print(tempstr,	temp+aw->axis.labmax,
							lastfrac+aw->axis.labmax,aw,labelmax);
		lastfrac = temp;
	}
}

static void pad_print(str,num,lastnum,aw,labelno)
	char	*str;
	float	num;
	float	lastnum;
	AxisObject	aw;
	int		labelno;
{
	char	temp[40];
	int		lastlen,len;
	int		i,j;

	sprintf(temp,"%2.3g",lastnum);
	lastlen = strlen(temp);
	sprintf(temp,"%2.3g",num);
	len = strlen(temp);
	if (aw->axis.textmode == TEXTDRAW || 
		aw->axis.textmode == TEXTFILL) {
			/* we need to pad the string to fit in the center */
		if (len >= lastlen) {
				strcpy(str,temp);
		} else {
			for(i=0;i<=lastlen && i < 40 ; i++) {
				str[i] = ' ';
				j = i - (1 + lastlen - len)/2;
				if (j >= 0 && j < len)
					str[i] = temp[j];
			}
			str[i] = '\0';
		}
		XoFillCenteredString(XtDisplay(XtParent((Widget)aw)),
			XtWindow(XtParent((Widget)aw)), aw->axis.textgc,
			aw->axis.textfont,
			aw->axis.labelx[labelno],
			aw->axis.labely[labelno],
			str,strlen(str));

	} else if (aw->axis.textmode == TEXTRADRAW || 
		aw->axis.textmode == TEXTRAFILL) {
		if (len >= lastlen) {
				strcpy(str,temp);
		} else {
			/* we need to pad the string to be right aligned */
			for(i=0;i<=lastlen && i < 40 ; i++) {
				str[i] = ' ';
				j = i - (lastlen - len);
				if (j >= 0 && j < len)
					str[i] = temp[j];
			}
			str[i] = '\0';
		}
		XoFillRtAlignedString(XtDisplay(XtParent((Widget)aw)),
			XtWindow(XtParent((Widget)aw)), aw->axis.textgc,
			aw->axis.textfont,
			aw->axis.labelx[labelno],
			aw->axis.labely[labelno],
			str,strlen(str));
	}
	XFlush(XtDisplay(XtParent((Widget)aw)));
}


#undef DRAWLINES
#undef DRAWSEGS
#undef DRAWPOINTS
#undef DRAWRECTS
#undef DRAWARROWS
#undef FILLRECTS
#undef FILLPOLY

#undef	TEXTDRAW
#undef	TEXTFILL
#undef	TEXTNODRAW
#undef	TEXTRADRAW
#undef	TEXTRAFILL

#undef	TICKAUTO
#undef	TICKFIFTH
#undef	TICKTENTH
#undef	TICKNONE
#undef XO_DEFAULT_NLABELS
#undef XO_DEFAULT_NTICKS
#undef XO_LABEL_LEN
#undef XO_MAX_TICK_PIXELS
#undef EPSILON
