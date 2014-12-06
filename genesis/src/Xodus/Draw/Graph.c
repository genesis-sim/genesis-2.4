/* $Id: Graph.c,v 1.3 2005/07/29 16:00:15 svitak Exp $ */
/*
 * $Log: Graph.c,v $
 * Revision 1.3  2005/07/29 16:00:15  svitak
 * Removed preprocessor logic for GCC 2.6.3 and OPTIMIZE. If still needed
 * it should be handled in some central include file.
 *
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.22  2001/06/29 21:08:12  mhucka
 * Added extra parens inside conditionals to quiet certain compiler
 * warnings.
 *
 * Revision 1.21  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.20  2000/05/02 06:06:41  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.19  1998/07/15 06:17:26  dhb
 * Upi update
 *
 * Revision 1.18  1996/10/29 00:31:23  venkat
 * Removed-parameter-type-from-declaration-of-atof-for-linux,-which-was-left-there-unintentionally-in-the-previous-check-in
 *
 * Revision 1.17  1996/10/25  19:49:19  venkat
 * Removed-ansi-compliant-function-prototypes-from-atof-declaration-and-shuffled-preprocessor-conditionals-to-include-stdlib.h-for-compilation-under-Linux/FreeBSD-with-or-without-optimization
 *
 * Revision 1.16  1996/10/07  21:45:06  dhb
 * Merged in 1.13.1.1 changes
 *
 * Revision 1.15  1996/08/08  15:43:01  dhb
 * GAutoScale() change to check the calculated x and y scale ranges.
 * If the x or y scale ranges are invalid they are fixed to a range
 * of 0.1.  The fixed x scale starts at the xmin while the fixed y
 * scale is centered at ymin.  The x and y scales are treated
 * independently.
 *
 * Revision 1.14  1995/09/27  00:00:13  venkat
 * Changes-to-avoid-Alpha-FPE's-and-core-dumps
 *
 * Revision 1.13.1.1  1996/08/21  21:08:51  venkat
 * Explicit declaration of atof() for Linux
 *
 * Revision 1.13.1.2  1996/09/13  23:59:29  venkat
 * Cleaned-up-the-preprocessor-statements-for-Linux-stdlib.h-include
 *
 * Revision 1.13.1.1  1996/08/21  21:08:51  venkat
 * Explicit declaration of atof() for Linux
 *
 * Revision 1.13  1995/07/28  23:35:17  dhb
 * Undefine __OPTIMIZE__ macro for gcc which causes incompatible
 * redefinition of functions in stdlib.h.
 *
 * Revision 1.12  1995/07/08  00:58:49  venkat
 * Destroy method modified to free resources allocated by the widget
 *
 * Revision 1.11  1995/06/16  05:53:57  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.10  1995/03/22  19:01:46  venkat
 * SetValues() hardcodes the justification resources
 * for the x and y axis children. This is for backwards
 * compatibility but can be extended to provide a justification
 * option for the axes.
 *
 * Revision 1.8  1995/03/17  20:18:05  venkat
 * Merged Upi changes:
 * Included stdlib.h for use of atof() in Linux
 * Checked for the being_destroyed flag before using child PixObjects
 * in the Resize and Expose methods.
 *
 * Revision 1.7  1995/03/07  00:52:58  venkat
 * Changed SetValues() to calculate GC based on xocomposite resources
 * for fg.
 *
 * Revision 1.6  1994/11/07  22:24:46  bhalla
 * Added lots of additional key control strokes for positioning
 * axes in the graph
 *
 * Revision 1.5  1994/05/26  13:43:11  bhalla
 * Implemented major changes in how the graph widget creates and manages the
 * axis. The creation and some of the management is now handled in the
 * Genesis end of things, which makes the Graph much simpler.
 *
 * Revision 1.4  1994/05/25  13:48:55  bhalla
 * futile attempt to get the graph to update xmin etc in the genesis structure.
 *
 * Revision 1.3  1994/03/22  15:29:42  bhalla
 * RCS error: no code changes
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "GraphP.h"
#include "PixP.h"
#include "Axis.h"
#include "Plot.h"
#include "Shape.h"


/*
** This is the graph widget. It manages plot and axis pixes.
** Each plot is associated with two axis pixes, x and y. When
** either x or y change then the plot will usually have to be
** reprojected and redrawn. 
*/

#define XO_OVERLAP 0
#define XO_STACK 1

static XtResource resources[] = {
#define offset(field) XtOffset(GraphWidget, graph.field)
  {XtNgxmin, XtCPosition, XtRFloat,	sizeof(float),
	 offset(xmin), XtRString, "0.0"},
  {XtNgymin, XtCPosition, XtRFloat,	sizeof(float),
	 offset(ymin), XtRString, "0.0"},
  {XtNgxmax, XtCPosition, XtRFloat,	sizeof(float),
	 offset(xmax), XtRString, "100.0"},
  {XtNgymax, XtCPosition, XtRFloat,	sizeof(float),
	 offset(ymax), XtRString, "100.0"},
  {XtNxoffset, XtCPosition, XtRFloat,sizeof(float),
	 offset(xoffset), XtRString, "0.0"},
  {XtNyoffset, XtCPosition, XtRFloat,sizeof(float),
	 offset(yoffset), XtRString, "0.0"},
  {XtNtitle, XtCString, XtRString,sizeof(String),
	 offset(title), XtRString, ""},
  {XtNxlabel, XtCString, XtRString,sizeof(String),
	 offset(xlabel), XtRString, ""},
  {XtNylabel, XtCString, XtRString,sizeof(String),
	 offset(ylabel), XtRString, ""},
  {XtNxunits, XtCString, XtRString,sizeof(String),
	 offset(xunits), XtRString, ""},
  {XtNyunits, XtCString, XtRString,sizeof(String),
	 offset(yunits), XtRString, ""},
#ifdef LATER
  {XtNmode, XtCMode, XtRMode,sizeof(int),
	 offset(mode), XtRString, "overlap"},
  {XtNplotLabelMode, XtCMode, XtRMode,sizeof(int),
	 offset(mode), XtRString, "yes"},
  {XtNscaleMode, XtCMode, XtRMode,sizeof(int),
	 offset(mode), XtRString, "hide"},
#endif
#undef offset
#define offset(field) XtOffset(GraphWidget, coredraw.field)
  {XtNxmin, XtCPosition, XtRFloat,	sizeof(float),
	 offset(xmin), XtRString, "-0.2"},
  {XtNymin, XtCPosition, XtRFloat,	sizeof(float),
	 offset(ymin), XtRString, "-0.2"},
  {XtNxmax, XtCPosition, XtRFloat,	sizeof(float),
	 offset(xmax), XtRString, "1.2"},
  {XtNymax, XtCPosition, XtRFloat,	sizeof(float),
	 offset(ymax), XtRString, "1.2"},
#undef offset
};

static char GraphRange[200]; 

/* 
** The main mouse callback operation should be selection of pixes
** The main keyboard callback operation should be zooming and panning
*/
static void XoGraphSelectPix();
static void XoGraphUnSelectPix();
static void XoGraphMotion();
static void XoGraphEnter();
static void  GZoom(), GPan(), GRestore(), GAutoscale();
/* arguments are : x,y (fractions of screen, relative) */
static void  XoGraphBounds();
static void Print();
static void
	Initialize(),Destroy(), Resize();
static void InsertChild();
static void Redisplay();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static XPoint rubberband[5];


/* More actions to come : mouse actions for zooming in and getting
** graph point values */
static XtActionsRec actions[] =
{
  {"pan",	 GPan},
  {"zoom", GZoom},
  {"restore", GRestore},
  {"autoscale", GAutoscale},
  {"print", Print},
  {"select", XoGraphSelectPix},
  {"unselect", XoGraphUnSelectPix},
  {"motion", XoGraphMotion},
  {"enter", XoGraphEnter},
  {"bds", XoGraphBounds},
};

static char translations[] =
" Shift Ctrl<Key>Left:     	bds(0.05,1,0)	\n\
  Shift Ctrl<Key>Right:    	bds(-0.05,1,0)	\n\
  Shift Ctrl<Key>Up:  		bds(0.05,1,1)      \n\
  Shift Ctrl<Key>Down:     	bds(-0.05,1,1)      \n\
  Ctrl<Key>Left:     	bds(0.05,0,0)	\n\
  Ctrl<Key>Right:     	bds(-0.05,0,0)	\n\
  Ctrl<Key>Up:  		bds(0.05,0,1)      \n\
  Ctrl<Key>Down:      	bds(-0.05,0,1)       \n\
  Shift<Key>Left:		zoom(1.1,1.0)	\n\
  Shift<Key>Right:      zoom(0.9,1.0)	\n\
  Shift<Key>Down:       zoom(1.0,1.1)       \n\
  Shift<Key>Up:   		zoom(1.0,0.9)       \n\
  <Key>Left:	pan(0.1,0)	\n\
  <Key>Right:      pan(-0.10,0)	\n\
  <Key>Down:       pan(0,0.10)       \n\
  <Key>Up:   pan(0,-0.10)      \n\
  <Key><:	zoom(1.1,1.1)	\n\
  <Key>>:	zoom(0.9,0.9)	\n\
  <Key>u:	restore()	\n\
  <Key>a:	autoscale()	\n\
  Ctrl<Key>P:	print()		\n\
  <BtnMotion> : motion()	\n\
  <BtnDown> : select()	\n\
  <BtnUp> : unselect()	\n\
  <Enter> : enter()	\n\
";

GraphClassRec graphClassRec = {
  { /* core fields */
  /* superclass		*/	(WidgetClass) &coredrawClassRec,
    /* class_name		*/	"Graph",
    /* widget_size		*/	sizeof(GraphRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	FALSE,  /* we may need to rubberband */
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child       */    InsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    NULL
  },
  { /* xocomposite_class fields */
    /* empty 		  */ 	0
  },
  { /* coredraw_class fields */
    /* transform_point		*/	XoInheritTransformPoint,
    /* transform_points		*/	XoInheritTransformPoints,
    /* back_transform		*/	XoInheritBackTransform,
    /* selectpix			*/	XoInheritSelectPix,
    /* unselectpix			*/	XoInheritUnSelectPix,
    /* extension			*/	NULL
  },
  {
	/* graph_class fields */	0
  }
};

WidgetClass graphWidgetClass = (WidgetClass)&graphClassRec;

static void ResetGC(gw)
	GraphWidget	gw;
{
	XGCValues values;
	XtGCMask  mask = GCForeground | GCFunction;

	if (gw->coredraw.gc != NULL)
		XtReleaseGC((Widget)gw,gw->coredraw.gc);

	values.foreground = gw->xocomposite.fg;
	values.function = GXxor;
	gw->coredraw.gc = XtGetGC((Widget)gw,mask,&values);
}

/* ARGUSED */
static void Initialize (req, new,args, num_args)
     Widget req, new;
	 ArgList	args;
	 Cardinal	*num_args;
{
	GraphWidget dw = (GraphWidget) new;
	/* This is a hack. I need to figure out a way of doing it
	** properly */
	if (dw->core.width == 0)
		dw->core.width = 1.1;
	if (dw->core.height == 0)
		dw->core.height = 1.1;
	/* Set up a commonly used string. If we had a ClassInit we should
	** do this there */
	sprintf(GraphRange,"%s %s %s %s",
		XtNgxmin,XtNgxmax,XtNgymin,XtNgymax);
	/* create the x and y axes */
#if 0
	dw->graph.xaxis = XtVaCreateManagedWidget("xaxis",
		axisObjectClass,new,
		XtVaTypedArg,XtNaxx,XtRString,"1.0",4,
		XtVaTypedArg,XtNaxy,XtRString,"0.0",4,
		XtVaTypedArg,XtNaxz,XtRString,"0.0",4,
		XtVaTypedArg,XtNaxmin,XtRString,"0.0",4,
		XtVaTypedArg,XtNaxmax,XtRString,"100.0",6,
		XtVaTypedArg,XtNaxlength,XtRString,"1.0",4,
		XtVaTypedArg,XtNtickx,XtRString,"0.0",4,
		XtVaTypedArg,XtNticky,XtRString,"-1.0",5,
		XtVaTypedArg,XtNtickz,XtRString,"0.0",4,
		XtVaTypedArg,XtNticklength,XtRString,"0.04",5,
		XtVaTypedArg,XtNlabeloffset,XtRString,"0.06",5,
		XtVaTypedArg,XtNunitoffset,XtRString,"0.10",5,
		XtVaTypedArg,XtNtitleoffset,XtRString,"0.13",5,
		XtVaTypedArg,XtNaxisTextmode,XtRString,"draw",7,
		XtVaTypedArg,XtNrotatelabels,XtRString,"0",2,
		NULL);
	XtAddCallback(dw->graph.xaxis,XtNrefreshCallback,
		XoRequestAxisUpdate, (XtPointer)dw);
	dw->graph.yaxis = XtVaCreateManagedWidget("yaxis",
		axisObjectClass,new,
		XtVaTypedArg,XtNaxx,XtRString,"0.0",4,
		XtVaTypedArg,XtNaxy,XtRString,"1.0",4,
		XtVaTypedArg,XtNaxz,XtRString,"0.0",4,
		XtVaTypedArg,XtNaxmin,XtRString,"0.0",4,
		XtVaTypedArg,XtNaxmax,XtRString,"100.0",6,
		XtVaTypedArg,XtNaxlength,XtRString,"1.0",4,
		XtVaTypedArg,XtNtickx,XtRString,"-1.0",5,
		XtVaTypedArg,XtNticky,XtRString,"0.0",4,
		XtVaTypedArg,XtNtickz,XtRString,"0.0",4,
		XtVaTypedArg,XtNticklength,XtRString,"0.02",5,
		XtVaTypedArg,XtNlabeloffset,XtRString,"0.03",5,
		XtVaTypedArg,XtNunitoffset,XtRString,"0.10",5,
		XtVaTypedArg,XtNtitleoffset,XtRString,"0.14",5,
		XtVaTypedArg,XtNaxisTextmode,XtRString,"radraw",7,
		XtVaTypedArg,XtNrotatelabels,XtRString,"1",2,
		NULL);
	XtAddCallback(dw->graph.yaxis,XtNrefreshCallback,
		XoRequestAxisUpdate, (XtPointer)dw);
	dw->graph.title_w = XtVaCreateManagedWidget("title",
		shapeObjectClass,new,
		XtVaTypedArg,XtNpixX,XtRString,"0.5",4,
		XtVaTypedArg,XtNpixY,XtRString,"1.10",5,
		XtNtext,new->core.name,
		NULL);
#endif

	/* allocate the space for assorted strings */
		if (dw->graph.title == NULL)
			dw->graph.title = XtNewString(dw->core.name);
		else
			dw->graph.title = XtNewString(dw->graph.title);

		if (dw->graph.xlabel == NULL)
			dw->graph.xlabel = XtNewString("");
		else
			dw->graph.xlabel = XtNewString(dw->graph.xlabel);

		if (dw->graph.ylabel == NULL)
			dw->graph.ylabel = XtNewString("");
		else
			dw->graph.ylabel = XtNewString(dw->graph.ylabel);

		if (dw->graph.xunits == NULL)
			dw->graph.xunits = XtNewString("");
		else
			dw->graph.xunits = XtNewString(dw->graph.xunits);

		if (dw->graph.yunits == NULL)
			dw->graph.yunits = XtNewString("");
		else
			dw->graph.yunits = XtNewString(dw->graph.yunits);

	/* Get an XOR GC for undrawing */
	ResetGC(dw);
	(*XtClass(new)->core_class.resize)((Widget)dw);
}

static Boolean SetValues(curr,req,new,args,num_args)
	Widget curr,req,new;
	ArgList	args;
	Cardinal	*num_args;
{
/* Values that need to be checked : 
	margins
	min/max (ensure that they are not equal)
	offsets
*/
	char	temp[50];
	GraphWidget dcw = (GraphWidget) curr;
	GraphWidget drw = (GraphWidget) req;
	GraphWidget dnw = (GraphWidget) new;
	int		i;
	int		set_xoff = 0;
	int		set_yoff = 0;

	/* Check if field is interesting */
	if ( 
		!(drw->graph.ymin != dcw->graph.ymin || 
		drw->graph.ymax != dcw->graph.ymax || 
		drw->graph.xmin != dcw->graph.xmin || 
		drw->graph.xmax != dcw->graph.xmax || 
		drw->graph.xoffset != dcw->graph.xoffset || 
		drw->graph.yoffset != dcw->graph.yoffset || 
		drw->graph.xlabel != dcw->graph.xlabel || 
		drw->graph.ylabel != dcw->graph.ylabel || 
		drw->graph.xunits != dcw->graph.xunits || 
		drw->graph.yunits != dcw->graph.yunits || 
		drw->graph.title != dcw->graph.title || 
		drw->xocomposite.bg != dcw->xocomposite.bg))
			 return(False);

	if (drw->xocomposite.fg != dcw->xocomposite.fg) {
		ResetGC(dnw);
		return(True);
	}
	if ((dnw->graph.title_w = XtNameToWidget((Widget) dnw,"title"))
		&& drw->graph.title != dcw->graph.title) {
		if (dcw->graph.title != dcw->core.name &&
			dcw->graph.title != NULL && drw->graph.title != NULL) {
			XtFree(dcw->graph.title);
			dnw->graph.title = XtNewString(drw->graph.title);
			XoXtVaSetValues(dnw->graph.title_w,
				XtNtext,dnw->graph.title,
				NULL);
		}
	}
	if (drw->graph.xlabel != dcw->graph.xlabel) {
		if (dcw->graph.xlabel != NULL)
			XtFree((char *)dcw->graph.xlabel);
		dnw->graph.xlabel = XtNewString(dnw->graph.xlabel);
		if ((dnw->graph.xaxis = XtNameToWidget((Widget) dnw,"x_axis")))
			XoXtVaSetValues(dnw->graph.xaxis,
				XtNtitle,dnw->graph.xlabel,
				NULL);
	}
	if (drw->graph.ylabel != dcw->graph.ylabel) {
		if (dcw->graph.ylabel != NULL)
			XtFree((char *)dcw->graph.ylabel);
		dnw->graph.ylabel = XtNewString(dnw->graph.ylabel);
		if ((dnw->graph.yaxis = XtNameToWidget((Widget) dnw,"y_axis")))
			XoXtVaSetValues(dnw->graph.yaxis,
				XtNtitle,dnw->graph.ylabel,
				NULL);
	}
	if (drw->graph.xunits != dcw->graph.xunits) {
		if (dcw->graph.xunits != NULL)
			XtFree((char *)dcw->graph.xunits);
		dnw->graph.xunits = XtNewString(dnw->graph.xunits);
		if ((dnw->graph.xaxis = XtNameToWidget((Widget) dnw,"x_axis")))
			XoXtVaSetValues(dnw->graph.xaxis,
				XtNunits,dnw->graph.xunits,
				XtNunitsjustify,"Right",
				NULL);
	}
	if (drw->graph.yunits != dcw->graph.yunits) {
		if (dcw->graph.yunits != NULL)
			XtFree((char *)dcw->graph.yunits);
		dnw->graph.yunits = XtNewString(dnw->graph.yunits);
		if ((dnw->graph.yaxis = XtNameToWidget((Widget) dnw,"y_axis")))
			XoXtVaSetValues(dnw->graph.yaxis,
				XtNunits,dnw->graph.yunits,
				XtNunitsjustify,"Left",
				NULL);
	}
	 if (drw->graph.xmin > drw->graph.xmax) {
		 dnw->graph.xmin = dcw->graph.xmin;
		dnw->graph.xmax = dcw->graph.xmax;
		/*
		XtCallCallbacks(); 
		*/
		/* Call the field updating code here */
		return(False);
	}
	if (drw->graph.ymin > drw->graph.ymax) {
		dnw->graph.ymin = dcw->graph.ymin;
		dnw->graph.ymax = dcw->graph.ymax;
		/*
		XtCallCallbacks();
		*/
		/* Call the field updating code here */
		return(False);
	}
	if ((dnw->graph.xaxis = XtNameToWidget((Widget) dnw,"x_axis"))) {
		/* For some reason the floats are not getting set
		** correctly. For now I will do a quick and dirty
		** and use strings */
		if (dnw->graph.xmin != dcw->graph.xmin) {
			sprintf(temp,"%g",dnw->graph.xmin);
			XoXtVaSetValues(dnw->graph.xaxis,
				XtVaTypedArg,XtNaxmin,XtRString,temp,strlen(temp)+1,
				NULL);
			/* At this point the axis update routine calls back to set
			** the graph xmin and xmax to the final values chosen by
			** the axis widget. The callback does NOT tell the graph
			** to update in this case, so we will do so explicitly by
			** means of the set_xoff and set_yoff flags */
			set_xoff = 1;
		}
		if (dnw->graph.xmax != dcw->graph.xmax) {
			sprintf(temp,"%g",dnw->graph.xmax);
			XoXtVaSetValues(dnw->graph.xaxis,
				XtVaTypedArg,XtNaxmax,XtRString,temp,strlen(temp)+1,
				NULL);
			set_xoff = 1;
		}
	}
	if ((dnw->graph.yaxis = XtNameToWidget((Widget) dnw,"y_axis"))) {
		if (dnw->graph.ymin != dcw->graph.ymin) {
			sprintf(temp,"%g",dnw->graph.ymin);
			XoXtVaSetValues(dnw->graph.yaxis,
				XtVaTypedArg,XtNaxmin,XtRString,temp,strlen(temp)+1,
				NULL);
			set_yoff = 1;
		}
		if (dnw->graph.ymax != dcw->graph.ymax) {
			sprintf(temp,"%g",dnw->graph.ymax);
			XoXtVaSetValues(dnw->graph.yaxis,
				XtVaTypedArg,XtNaxmax,XtRString,temp,strlen(temp)+1,
				NULL);
			set_yoff = 1;
		}
	}
	if (dnw->graph.xoffset != dcw->graph.xoffset) 
		set_xoff = 1;
	if (dnw->graph.yoffset != dcw->graph.yoffset) 
		set_yoff = 1;
	XoUpdatePlotAxes(dnw,set_xoff,set_yoff);
	return(True);
}

void XoUpdatePlotAxes(dnw,set_xoff,set_yoff)
	GraphWidget dnw;
	int	set_xoff,set_yoff;
{
	float	curr_xoffset;
	float	curr_yoffset;
	char	temp[50];
	char	temp2[50];
	char	temp3[50];
	char	temp4[50];
	char	temp5[50];
	char	temp6[50];
	int		i;

	curr_yoffset = curr_xoffset = 0.0;
	for(i=2;i<dnw->composite.num_children;i++) {
		if (XtIsSubclass(dnw->composite.children[i],
			plotObjectClass)) {
			sprintf(temp,"%g",curr_xoffset/
				(dnw->graph.xmax - dnw->graph.xmin));
			sprintf(temp2,"%g",dnw->graph.xmin);
			sprintf(temp3,"%g",dnw->graph.xmax);
			sprintf(temp4,"%g",curr_yoffset/
				(dnw->graph.ymax - dnw->graph.ymin));
			sprintf(temp5,"%g",dnw->graph.ymin);
			sprintf(temp6,"%g",dnw->graph.ymax);
			XoXtVaSetValues(dnw->composite.children[i],
				XtVaTypedArg,XtNpixX,XtRString,temp,strlen(temp)+1,
				XtVaTypedArg,XtNxmin,XtRString,temp2,strlen(temp2)+1,
				XtVaTypedArg,XtNxmax,XtRString,temp3,strlen(temp3)+1,
				XtVaTypedArg,XtNpixY,XtRString,temp4,strlen(temp4)+1,
				XtVaTypedArg,XtNymin,XtRString,temp5,strlen(temp5)+1,
				XtVaTypedArg,XtNymax,XtRString,temp6,strlen(temp6)+1,
				NULL);
			curr_xoffset += dnw->graph.xoffset;
			curr_yoffset += dnw->graph.yoffset;
		}
	}
}



/*
This function manages pix sizes in the following manner :
	let them draw themselves wherever they like!
*/
static void Resize(w)
	Widget w;
{
	/* need to resize all the child pixes, by calling their Project
	** methods */
  GraphWidget dw = (GraphWidget) w;
 Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;
  PixObjectClass class;

	if (!XtIsRealized((Widget) dw))
		return;

	/* XoUpdatePlotAxes(dw,1,1); */
	/* handle children */
  	for (childP = first_born;
		childP - first_born < num_children; childP++) {
		if (((PixObject)*childP)->object.being_destroyed)
                       continue;
		class =
			(PixObjectClass) ((PixObject)*childP)->object.widget_class;
     	(class->pix_class.project)(*childP);

	}
	/* expose happens automatically here */
}

static void Redisplay (w, e, region)
     Widget w;
     XEvent *e;
     Region region;
{
  GraphWidget dw = (GraphWidget) w;
 Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;
 /* static int redno = 0; */

	if ((dw->coredraw.drawflags & XO_DRAW_REFRESHABLE_NOT) ||
		!XtIsRealized((Widget) dw))
		return;

/* printf("Redisp %d\n",redno++); */
	/* handle children */
  for (childP = first_born;childP - first_born < num_children;childP++){
	if (((PixObject)*childP)->object.being_destroyed)
                         continue;
    if (region == NULL ||
	((*childP)->core.width == 0 &&
		(*childP)->core.height == 0) ||
	(XRectInRegion(region, (*childP)->core.x, (*childP)->core.y,
		       (*childP)->core.width, (*childP)->core.height) 
	   != RectangleOut))
      (*(XtClass(*childP))->core_class.expose)((*childP), e, region);
	/* Handle axes */
  }
}

static XtGeometryResult GeometryManager(w,request,reply)
	Widget w;
	XtWidgetGeometry	*request;
	XtWidgetGeometry	*reply;	/* RETURN */
{
	return XtGeometryYes;
}

static void InsertChild(w)
	Widget w;
{
	GraphWidget gw = (GraphWidget) w->core.parent;

	if (XtIsSubclass(w,pixObjectClass)) {
		(coredrawClassRec.composite_class.insert_child)(w);
		/*
		gw->composite.children[gw->composite.num_children] = w;
		gw->composite.num_children++;
		*/
		/*
		if (gw->composite.num_children > 2 && 
			XtIsSubclass(w,plotObjectClass)) {
			AddXAxisToPlot(gw->composite.children[0],w);
			AddYAxisToPlot(gw->composite.children[1],w);
		}
		*/
	}
}


static void Destroy(w)
	Widget w;
{
	GraphWidget gw=(GraphWidget)w;
	if(gw->graph.xunits != NULL) XtFree(gw->graph.xunits);
	if(gw->graph.yunits != NULL) XtFree(gw->graph.yunits);
	if(gw->graph.xlabel != NULL) XtFree(gw->graph.xlabel);
	if(gw->graph.ylabel != NULL) XtFree(gw->graph.ylabel);
	if(gw->graph.title != NULL)  XtFree(gw->graph.title);
}


/* Note that the pixes figure out the dist in real coords by using
** the back-transform of the parent
** Actually the pixes have already found their little patch of 2-d
** space and then go ahead and do the back transform if they want to
*/
static void XoGraphSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	int i;
	CoreDrawWidgetClass class = (CoreDrawWidgetClass)(XtClass(w));

	(class->coredraw_class.selectpix)(w,event);
	if (((GraphWidget)w)->coredraw.selected == w) {
		for(i=0;i<5;i++) {
			rubberband[i].x = event->x;
			rubberband[i].y = event->y;
		}
	}
}

/* Here we do something different from the CoreDraw widget, so
** we use the inherited unselect for only a small part of the function*/
static void XoGraphUnSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	float x1,y1,x2,y2,z;
	char  temp1[20],temp2[20];
	GraphWidget gw =  (GraphWidget)w;
	PixObjectClass	class;
	CoreDrawWidgetClass dclass = (CoreDrawWidgetClass)(XtClass(w));
  	static XoEventInfo    info;

	if (gw->coredraw.selected == w) {
	/* This is where we figure out what the rubberbanded region was */

		z = 0.0;
		(dclass->coredraw_class.backtransform)(
				w,(int)rubberband[0].x,(int)rubberband[0].y,&x1,&y1,&z);
		(dclass->coredraw_class.backtransform)(
				w,(int)rubberband[2].x,(int)rubberband[2].y,&x2,&y2,&z);
		if (x1 > x2) {
			z = x1; 
			x1 = x2;
			x2 = z;
		}
		if (y1 > y2) {
			z = y1; 
			y1 = y2;
			y2 = z;
		}
		/* Here we assume that of the window in 3d-space that the graph
		** widget displays (ie, from gw->coredraw.xmin .. xmax etc)
		** the portion from 0 to 1 exactly is where the plot is drawn.
		** So the only scaling on x1 etc relates to the plot coords
		** ie, gw->graph.xmin ...
		*/
		if (x1 != x2 && y1 != y2) {
			x1 = gw->graph.xmin + x1 *
				(gw->graph.xmax - gw->graph.xmin);
			x2 = gw->graph.xmin + x2 *
				(gw->graph.xmax - gw->graph.xmin);
			sprintf(temp1,"%f",x1);
			sprintf(temp2,"%f",x2);
			if ((gw->graph.xaxis = XtNameToWidget((Widget) gw,"x_axis")))
			XoXtVaSetValues(gw->graph.xaxis,
				XtVaTypedArg,XtNaxmin,XtRString,temp1,1+strlen(temp1),
				XtVaTypedArg,XtNaxmax,XtRString,temp2,1+strlen(temp2),
				NULL);
			y1 = gw->graph.ymin + y1 *
				(gw->graph.ymax - gw->graph.ymin);
			y2 = gw->graph.ymin + y2 *
				(gw->graph.ymax - gw->graph.ymin);
			sprintf(temp1,"%f",y1);
			sprintf(temp2,"%f",y2);
			if ((gw->graph.yaxis = XtNameToWidget((Widget) gw,"y_axis")))
			XoXtVaSetValues(gw->graph.yaxis,
				XtVaTypedArg,XtNaxmin,XtRString,temp1,1+strlen(temp1),
				XtVaTypedArg,XtNaxmax,XtRString,temp2,1+strlen(temp2),
				NULL);
			gw->graph.xmin = x1;
			gw->graph.xmax = x2;
			gw->graph.ymin = y1;
			gw->graph.ymax = y2;
			XoUpdatePlotAxes(gw,1,1);
			info.event = XoUpdateFields;
			info.ret = GraphRange;
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,gw->coredraw.callbacks,
				(XtPointer) &info);
		}
	}
	/* Use the coredraw unselect action to deal with pixes and
	** clearing the pointer to the widget */
	(dclass->coredraw_class.unselectpix)(w,event);
	/* Resize(w); */
	XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
}

static void XoGraphMotion(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	GraphWidget	gw = (GraphWidget)w;
	PixObject	pix = (PixObject)gw->coredraw.selected;
  	PixObjectClass class;
	int do_redraw = 0;
	
	/* Set a minimum movement of XO_MINIMUM_MOVE pixels before it is
	** recognized as motion, to avoid confusion with
	** double clicks */
	if (rubberband[2].x == rubberband[0].x && 
		rubberband[2].y == rubberband[0].y && 
		fabs((double)(event->x - rubberband[0].x)) < XO_MINIMUM_MOVE &&
		fabs((double)(event->y - rubberband[0].y)) < XO_MINIMUM_MOVE)
		return;
	
	if (pix){
		if (XtIsSubclass((Widget) pix,pixObjectClass)) {
			class = (PixObjectClass) pix->object.widget_class;
		/* call the pix motion handler */
     		(class->pix_class.motion)((Widget)pix,event);
		} else {
		/* call the draw motion handler */
			/* printf("Calling draw motion handler\n"); */
			/* Undraw the rubberband. Since XOR is used, we only need
			** the original GC */
			do_redraw = 1;
			XoDrawLines(XtDisplay(w),XtWindow(w),gw->coredraw.gc,
				rubberband,5,CoordModeOrigin);
		}
	}
	/* Set the new rubberband points */
	rubberband[1].x = rubberband[2].x = event->x;
	rubberband[3].y = rubberband[2].y = event->y;
	if (do_redraw) {
		/* Redraw the rubberband */
		XoDrawLines(XtDisplay(w),XtWindow(w),gw->coredraw.gc,
			rubberband,5,CoordModeOrigin);
		XFlush(XtDisplay(w));
	}
}


static void XoGraphEnter(w,event)
    Widget  w;
    XButtonEvent    *event;
{
  GraphWidget gw = (GraphWidget) w;
  Widget pix;
  static XoEventInfo    info;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)(XtClass(w));

  pix = XoFindNearestPix(w,event);
  if (pix != NULL) {
        /* Call the callbacks for a possible drag-drop operation
        ** within the graph widget */
        info.event = XoEnter;
        info.ret="";
		info.z = 0.0;
		(class->coredraw_class.backtransform)(
			w,event->x,event->y,
			&(info.x),&(info.y),&(info.z));
        info.key = event->time;
        /* Calling the drag callback */
        if (pix == w)
            XtCallCallbackList(w,gw->coredraw.callbacks,
                (XtPointer)&info);
        else
            XtCallCallbackList(pix,((PixObject)pix)->pix.callbacks,
                (XtPointer)&info);
  }
}


static void  GZoom(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	float zx,zy;
	float mx,my,dx,dy;
	GraphWidget	dw = (GraphWidget)w;
	static XoEventInfo	info;
	char	minstr[40];
	char	maxstr[40];
	
	if (*num_params == 2) {
		zx = atof(params[0]);
		zy = atof(params[1]);
		mx = (dw->graph.xmin + dw->graph.xmax)/2.0;
		my = (dw->graph.ymin + dw->graph.ymax)/2.0;
		dx = (dw->graph.xmax - dw->graph.xmin)/2.0;
		dy = (dw->graph.ymax - dw->graph.ymin)/2.0;
		dw->graph.xmin = mx - dx * zx;
		dw->graph.ymin = my - dy * zy;
		dw->graph.xmax = mx + dx * zx;
		dw->graph.ymax = my + dy * zy;

		/* Update the axes */
		sprintf(minstr,"%g",dw->graph.xmin);
		sprintf(maxstr,"%g",dw->graph.xmax);
		if ((dw->graph.xaxis = XtNameToWidget((Widget) dw,"x_axis")))
		XoXtVaSetValues(dw->graph.xaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);
		sprintf(minstr,"%g",dw->graph.ymin);
		sprintf(maxstr,"%g",dw->graph.ymax);
		if ((dw->graph.yaxis = XtNameToWidget((Widget) dw,"y_axis")))
		XoXtVaSetValues(dw->graph.yaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);

		/* Call the field updating code here */
		/* Resize(w); */
		XoUpdatePlotAxes(dw,1,1);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		info.event = XoUpdateFields;
		info.ret = GraphRange;
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
	}
}

static void  GPan(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	float px,py;
	float mx,my,dx,dy;
	GraphWidget	dw = (GraphWidget)w;
	static XoEventInfo	info;
	char	minstr[40];
	char	maxstr[40];
	
	if (*num_params == 2) {
		px = atof(params[0]);
		py = atof(params[1]);
		mx = (dw->graph.xmin + dw->graph.xmax)/2.0;
		my = (dw->graph.ymin + dw->graph.ymax)/2.0;
		dx = (dw->graph.xmax - dw->graph.xmin)/2.0;
		dy = (dw->graph.ymax - dw->graph.ymin)/2.0;
		dw->graph.xmin = mx - dx +  dx * px;
		dw->graph.ymin = my - dy +  dy * py;
		dw->graph.xmax = mx + dx +  dx * px;
		dw->graph.ymax = my + dy +  dy * py;
		/* Update the axes */
		sprintf(minstr,"%g",dw->graph.xmin);
		sprintf(maxstr,"%g",dw->graph.xmax);
		if ((dw->graph.xaxis = XtNameToWidget((Widget) dw,"x_axis")))
		XoXtVaSetValues(dw->graph.xaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);
		sprintf(minstr,"%g",dw->graph.ymin);
		sprintf(maxstr,"%g",dw->graph.ymax);
		if ((dw->graph.yaxis = XtNameToWidget((Widget) dw,"y_axis")))
		XoXtVaSetValues(dw->graph.yaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);

		/* update the external fields */
		/* Call the field updating code here */
		/* Resize(w); */
		/* Update the plots */
		XoUpdatePlotAxes(dw,1,1);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		info.event = XoUpdateFields;
		info.ret = GraphRange;
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
	}
}

static void  GRestore(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	GraphWidget dw = (GraphWidget)w;
	static XoEventInfo	info;
	char	minstr[40];
	char	maxstr[40];

		dw->graph.xmin = 0;
		dw->graph.ymin = 0;
		dw->graph.xmax = 100;
		dw->graph.ymax = 100;

		/* Update the axes */
		sprintf(minstr,"%g",dw->graph.xmin);
		sprintf(maxstr,"%g",dw->graph.xmax);
		if ((dw->graph.xaxis = XtNameToWidget((Widget) dw,"x_axis")))
		XoXtVaSetValues(dw->graph.xaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);
		sprintf(minstr,"%g",dw->graph.ymin);
		sprintf(maxstr,"%g",dw->graph.ymax);
		if ((dw->graph.yaxis = XtNameToWidget((Widget) dw,"y_axis")))
		XoXtVaSetValues(dw->graph.yaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);

		/* Call the field updating code here */
		/* Resize(w); */
		XoUpdatePlotAxes(dw,1,1);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		info.event = XoUpdateFields;
		info.ret = GraphRange;
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
}


static void  GAutoscale(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	GraphWidget dw = (GraphWidget)w;
	static XoEventInfo	info;
	char	minstr[40];
	char	maxstr[40];
	float	xmin,xmax,ymin,ymax;
	float 	gxrange = dw->graph.xmax - dw->graph.xmin;
	float 	gyrange = dw->graph.ymax - dw->graph.ymin;
	float	xoff;
	float	yoff;
	int		firstloop = 1;
	int		i;

	for(i=2;i<dw->composite.num_children;i++) {
		if (XtIsSubclass(dw->composite.children[i],
			plotObjectClass)) {
			XtVaGetValues(dw->composite.children[i],
				XtNxminvalue,&xmin,
				XtNyminvalue,&ymin,
				XtNxmaxvalue,&xmax,
				XtNymaxvalue,&ymax,
				XtNpixX,&xoff,
				XtNpixY,&yoff,
				NULL);
			/* Check that the plots have been inited */
			if (xmin > xmax || ymin > ymax)
				continue;
			xmin += xoff * gxrange;
			ymin += yoff * gyrange;
			xmax += xoff * gxrange;
			ymax += yoff * gyrange;
			if (firstloop) {
				dw->graph.xmin = xmin;
				dw->graph.ymin = ymin;
				dw->graph.xmax = xmax;
				dw->graph.ymax = ymax;
				firstloop = 0;
			} else {
				if (dw->graph.xmin > xmin) dw->graph.xmin = xmin;
				if (dw->graph.ymin > ymin) dw->graph.ymin = ymin;
				if (dw->graph.xmax < xmax) dw->graph.xmax = xmax;
				if (dw->graph.ymax < ymax) dw->graph.ymax = ymax;
			}
		}
	}
	if (firstloop) /* there were no child plots */
		return;

		/*
		** Check the new axis limits, ensure the x and y ranges are
		** greater than zero.
		**
		** POLICY: default to x or y range of 0.1, x scale starts
		** at the min x and y scale is centered at the min y.  X
		** and y scales are treated independently.
		*/

		if (dw->graph.xmax <= dw->graph.xmin)
		    dw->graph.xmax = dw->graph.xmin + 0.1;

		if (dw->graph.ymax <= dw->graph.ymin)
		  {
		    dw->graph.ymax = dw->graph.ymin + 0.05;
		    dw->graph.ymin = dw->graph.ymin - 0.05;
		  }

		/* Update the plots */
		XoUpdatePlotAxes(dw,1,1);
	

		/* Update the axes */
		sprintf(minstr,"%g",dw->graph.xmin);
		sprintf(maxstr,"%g",dw->graph.xmax);
		if ((dw->graph.xaxis = XtNameToWidget((Widget) dw,"x_axis")))
		XoXtVaSetValues(dw->graph.xaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);
		sprintf(minstr,"%g",dw->graph.ymin);
		sprintf(maxstr,"%g",dw->graph.ymax);
		if ((dw->graph.yaxis = XtNameToWidget((Widget) dw,"y_axis")))
		XoXtVaSetValues(dw->graph.yaxis,
			XtVaTypedArg,XtNaxmin,XtRString,minstr,strlen(minstr)+1,
			XtVaTypedArg,XtNaxmax,XtRString,maxstr,strlen(maxstr)+1,
			NULL);

		/* Call the field updating code here */
		Resize(w);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		info.event = XoUpdateFields;
		info.ret = GraphRange;
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
}

static void  XoGraphBounds(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	float increment;
	int		is_upbd;
	int		is_yax;
	GraphWidget	dw = (GraphWidget)w;
	static XoEventInfo	info;
	
	if (*num_params == 3) {
		increment = atof(params[0]);
		is_upbd = atoi(params[1]);
		is_yax = atoi(params[2]);
		if (is_upbd && is_yax) {
			dw->coredraw.ymax -= increment;
		} else if (!is_upbd && is_yax) {
			dw->coredraw.ymin -= increment;
		} else if (is_upbd && !is_yax) {
			dw->coredraw.xmax += increment;
		} else if (!is_upbd && !is_yax) {
			dw->coredraw.xmin += increment;
		}
		/* Call the field updating code here */
		Resize(w);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		info.event = XoUpdateFields;

		/* Note that these names are derived from the XtNXmin etc */
		info.ret = "xmin xmax ymin ymax";
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
	}
}

static void  Print(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	/* Need to figure out what Region is like */
	Region	region = (Region)NULL;
	XoSetPostscript(XtDisplay(w),XtWindow(w));
	Redisplay(w,event,region);
	XoSetX();
}


#if 0
static void XoRequestAxisUpdate(w,client_data,call_data)
	Widget w;
	XtPointer	client_data,call_data;
{
	GraphWidget gw = (GraphWidget) client_data;
	/* The x array has the min and max values respectively */
	float	*x;
	static XoEventInfo    info;

	x = (float *)call_data;

	/* we check which axis is returning */
	if (gw->graph.xaxis == w) {
		gw->graph.xmin = x[0];
		gw->graph.xmax = x[1];
	} else {
		gw->graph.ymin = x[0];
		gw->graph.ymax = x[1];
	}
	XoUpdatePlotAxes(gw,1,1);
	if (x[2] > 1) {
		/*
		XoUpdateGraphOffsets(gw,1,1);
		*/
		XClearArea(XtDisplay((Widget)gw),XtWindow((Widget)gw),
			0,0,0,0,True);
	}
	info.event = XoUpdateFields;
	info.ret = GraphRange;
	info.x=info.y=info.z=0.0;
	XtCallCallbackList(w,gw->coredraw.callbacks,(XtPointer) &info);

	/* Redisplay ((Widget)gw, NULL, NULL); */
}
#endif
