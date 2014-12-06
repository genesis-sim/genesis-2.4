/* $Id: FastPlot.c,v 1.3 2005/07/20 20:01:57 svitak Exp $ */
/*
** $Log: FastPlot.c,v $
** Revision 1.3  2005/07/20 20:01:57  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:02:50  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2000/09/11 15:57:13  mhucka
** The previous implementation did not properly handle graphics contexts
** (GCs).  The contexts were being created using information drawn from the X
** window system default window, but this is wrong: on TrueColor or other
** non-Pseudocolor visuals, GENESIS explicitly uses a Pseudocolor visual and
** creates its own private colormap, and these may not match the defaults in
** characteristics.  This lead to BadMatch errors involving X_PolySegment when
** xfastplot was used on 24-bit color Solaris systems.  The changes to
** Initialize() should now fix this problem.
**
** Revision 1.3  2000/09/06 01:19:03  mhucka
** Added XoCvtIntToString() to eliminate X warnings about
** "No type converter registered for 'Int' to 'String' conversion".
**
** Revision 1.2  1998/07/22 06:39:09  dhb
** Fixed ANSI C isms
**
 * Revision 1.1  1998/07/14 20:45:26  dhb
 * Initial revision
 * */

#include <stdio.h>
#include <stdlib.h>	/* atoi() */
#include <math.h>
#include <ctype.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include "Xo/XoDefs.h"
#include "FastPlotP.h"

#define F_ROLL 1
#define F_OSCOPE 2
#define F_RING 3
#define F_XYRING 4
#define F_WAVEPLOT 5
#define F_XYWAVEPLOT 6


/*
** This is the fastplot widget. It is an efficient widget for
** plotting data, with some limitations as compared to the real
** Plot and Graph widgets. It works well for oscilloscope-type
** displays and on-line data acquisition.
*/

static XtResource resources[] = {
#define offset(field) XtOffset(FastPlotWidget, fastplot.field)
  {XtNht, XtCPosition, XtRInt,	sizeof(int),
	 offset(ht), XtRString, "100"},
  {XtNwid, XtCPosition, XtRInt,	sizeof(int),
	 offset(wid), XtRString, "100"},
  {XtNlborder, XtCPosition, XtRInt,	sizeof(int),
	 offset(lborder), XtRString, "20"},
  {XtNrborder, XtCPosition, XtRInt,	sizeof(int),
	 offset(rborder), XtRString, "0"},
  {XtNtborder, XtCPosition, XtRInt,	sizeof(int),
	 offset(tborder), XtRString, "0"},
  {XtNbborder, XtCPosition, XtRInt,	sizeof(int),
	 offset(bborder), XtRString, "20"},
  {XtNnpts, XtCNpts, XtRInt,	sizeof(int),
	 offset(npts), XtRString, "32"},
  {XtNpts, XtCPts, XtRPointer,	sizeof(XPoint *),
	 offset(pts), XtRImmediate, (XtPointer)NULL},
  {XtNnnewpts, XtCNpts, XtRInt,	sizeof(int),
	 offset(nnewpts), XtRString, "1"},
  {XtNringpts, XtCIndex, XtRInt,	sizeof(int),
	 offset(ringpts), XtRString, "0"},
  {XtNoverlay_flag, XtCFlag, XtRInt,	sizeof(int),
	 offset(overlay_flag), XtRString, "0"},
  {XtNline_flag, XtCFlag, XtRInt,	sizeof(int),
	 offset(line_flag), XtRString, "0"},
  {XtNplotmode, XtCValue, XtRInt,	sizeof(int),
	 offset(plotmode), XtRString, "0"},
  {XtNreset_flag, XtCFlag, XtRInt,	sizeof(int),
	 offset(reset_flag), XtRString, "0"},
  {XtNcolorptr, XtCPtr, XtRPointer,	sizeof(int *),
	 offset(colorptr), XtRImmediate, (XtPointer)NULL},
  {XtNxval, XtCValue, XtRInt,	sizeof(int),
	 offset(xval), XtRString, "0"},
  {XtNyval, XtCValue, XtRInt,	sizeof(int),
	 offset(yval), XtRString, "-1"},
  {XtNxaxis_flag, XtCFlag, XtRInt,	sizeof(int),
	 offset(xaxis_flag), XtRString, "1"},
  {XtNyaxis_flag, XtCFlag, XtRInt,	sizeof(int),
	 offset(yaxis_flag), XtRString, "1"},
  {XtNtitle, XtCLabel, XtRString,	sizeof(char *),
	 offset(title), XtRImmediate,  (XtPointer)NULL},
  {XtNxtextmax, XtCLabel, XtRString,	sizeof(XtPointer),
	 offset(xtextmax), XtRImmediate,  (XtPointer)NULL},
  {XtNxtextmid, XtCLabel, XtRString,	sizeof(XtPointer),
	 offset(xtextmid), XtRImmediate,  (XtPointer)NULL},
  {XtNxtextmin, XtCLabel, XtRString,	sizeof(XtPointer),
	 offset(xtextmin), XtRImmediate,  (XtPointer)NULL},
  {XtNytextmax, XtCLabel, XtRString,	sizeof(XtPointer),
	 offset(ytextmax), XtRImmediate,  (XtPointer)NULL},
  {XtNytextmid, XtCLabel, XtRString,	sizeof(XtPointer),
	 offset(ytextmid), XtRImmediate,  (XtPointer)NULL},
  {XtNytextmin, XtCLabel, XtRString,	sizeof(XtPointer),
	 offset(ytextmin), XtRImmediate,  (XtPointer)NULL},
  {XtNcallback, XtCCallback, XtRCallback,	sizeof(XtPointer),
	 offset(callbacks), XtRCallback, (XtPointer) NULL},
#undef offset
};

/* 
** The main keyboard callback operation should be zooming and panning
*/

static void  Pan(), Restore(), Notify();
static void DoKeyPress();
/* arguments are : x,y (fractions of screen, relative) */
static void Initialize(),Destroy(), Resize();
static void Redisplay();
static Boolean SetValues();
static int blackfg;
static void XoRegisterIntToStringConverter();
static void ClassInitialize();

static XtActionsRec actions[] =
{
  {"pan",	 Pan},
  {"dokeypress", DoKeyPress},
  {"notify", Notify},
};

static char translations[] =
" <Key>Left:	pan(-1,0)	\n\
  <Key>Right:      pan(1,0)	\n\
  <Key>Down:       pan(0,-1)       \n\
  <Key>Up:   pan(0,1)      \n\
  <Key>:	dokeypress()	\n\
  <BtnDown>: notify()	\n\
";

FastPlotClassRec fastplotClassRec = {
  { /* core fields */
  /* superclass		*/	(WidgetClass) (&xocoreClassRec),
    /* class_name		*/	"FastPlot",
    /* widget_size		*/	sizeof(FastPlotRec),
    /* class_initialize		*/	ClassInitialize,
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
    /* compress_motion		*/	TRUE,  /* we may need to rubberband */
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
  { /* fastplot_class fields */
    /* empty		*/	0
  }
};

WidgetClass fastplotWidgetClass = (WidgetClass)&fastplotClassRec;

/* ARGUSED */
static void Initialize (req, new, args, num_args)
        Widget    req, new;
        ArgList   args;
        Cardinal *num_args;
{
        Display       *display = XtDisplay(XtParent((Widget) new));
        Drawable       drawable;
	Widget         ptr;
        FastPlotWidget fw = (FastPlotWidget) new;
        XGCValues      values;
        XtGCMask       mask = GCForeground;
	/*
	** When this gets called, the parent widget does not have a window
	** defined.  However, to create the GC, we need a drawable, and it
	** can't necessarily be the default root window because the drawable's
	** depth and visual must match that of the widget we're creating.
	** (Otherwise, you get BadMatch errors in some later calls.)  This
	** is grungy, but I can't seem to find another way to get at the
	** enclosing window except to search backward through the parent
	** widgets and look for one.  So....
	*/
	ptr = (Widget) fw;
	while (ptr->core.window == 0 && ptr->core.parent) {
	    ptr = ptr->core.parent;
	}
	if (ptr->core.window != 0) {
	    drawable = ptr->core.window;
	} else {
	    /* Not sure what else to do. */
	    drawable = XDefaultRootWindow(display);
	}

        blackfg = values.foreground =
	    XBlackPixel(display, XDefaultScreen(display));

        fw->fastplot.gc = XCreateGC(display, drawable, mask, &values);
        fw->fastplot.colorptr = &blackfg;
}

static void ClassInitialize()
{
	XoRegisterIntToStringConverter();
}

static Boolean SetValues(curr,req,new,args,num_args)
	Widget curr,req,new;
	ArgList	args;
	Cardinal	*num_args;
{
/* Values that need to be checked : 
	margins
	min/max (ensure that they are not equal)
*/
	FastPlotWidget fcw = (FastPlotWidget) curr;
	FastPlotWidget frw = (FastPlotWidget) req;
	FastPlotWidget fnw = (FastPlotWidget) new;

	/* Check if field is interesting */
	if ( 
		!(frw->fastplot.lborder != fcw->fastplot.lborder || 
		frw->fastplot.rborder != fcw->fastplot.rborder || 
		frw->fastplot.bborder != fcw->fastplot.tborder || 
		frw->fastplot.tborder != fcw->fastplot.bborder))
			 return(False);

	if (frw->fastplot.lborder < 0) {
	 	fnw->fastplot.lborder = 0;
		return(False);
	}
	if (frw->fastplot.rborder < 0) {
	 	fnw->fastplot.rborder = 0;
		return(False);
	}
	if (frw->fastplot.tborder < 0) {
	 	fnw->fastplot.tborder = 0;
		return(False);
	}
	if (frw->fastplot.bborder < 0) {
	 	fnw->fastplot.bborder = 0;
		return(False);
	}

	Resize(new);
	return(True);
}

/*
This function manages pix sizes in the following manner :
	let them draw themselves wherever they like!
*/
static void Resize(w)
	Widget w;
{
  FastPlotWidget dw = (FastPlotWidget) w;
  static XoEventInfo	info;
  	dw->fastplot.wid = w->core.width;
  	dw->fastplot.ht = w->core.height;

	info.event = XoUpdateFields;
	info.ret = "xgeom ygeom hgeom wgeom wid ht";
	info.x = info.y = info.z = 0.0;
	XtCallCallbackList(w, dw->fastplot.callbacks, (XtPointer)&info);
}

static void DrawTicks(w, color)
Widget w;
int color;
{
	FastPlotWidget	fw = (FastPlotWidget)w;

	if (color)
		XSetForeground(XtDisplay(w), fw->fastplot.gc,
			fw->xocore.fg);
	else
		XSetForeground(XtDisplay(w), fw->fastplot.gc,
			fw->core.background_pixel);

  if (fw->fastplot.xaxis_flag) {
  	XDrawLine(XtDisplay(w), XtWindow(w), fw->fastplot.gc, 
		fw->fastplot.xval,
		fw->fastplot.ht - fw->fastplot.bborder + 2,
		fw->fastplot.xval,
		fw->fastplot.ht - fw->fastplot.bborder + 5);
	}

  if (fw->fastplot.yaxis_flag) {
  	XDrawLine(XtDisplay(w), XtWindow(w), fw->fastplot.gc, 
		fw->fastplot.lborder - 2,
		fw->fastplot.yval,
		fw->fastplot.lborder - 5,
		fw->fastplot.yval);
	}
}


#ifdef __STDC_
static void DrawText(Widget w,char *str, int x, int y) {
#else
static void DrawText(w, str, x, y)
Widget w;
char *str;
int x;
int y;
{
#endif
	if (str && strlen(str) > 0) {
  		FastPlotWidget dw = (FastPlotWidget) w;
		XDrawString(XtDisplay(w), XtWindow(w), dw->fastplot.gc,
			x, y, str, strlen(str));
	}
}

static void Redisplay (w, e, region)
     Widget w;
     XEvent *e;
     Region region;
{
  FastPlotWidget dw = (FastPlotWidget) w;
	Display *disp = XtDisplay(w);

  if (dw->fastplot.reset_flag == 0) return;
	XSetForeground(XtDisplay(w), dw->fastplot.gc, dw->xocore.fg);
		/*
		XSetForeground(XtDisplay(w), fw->fastplot.gc,
		XBlackPixel(disp,XDefaultScreen(disp)));
		*/

  /* Draw the axes */
  if (dw->fastplot.xaxis_flag)
  	XDrawLine(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
		dw->fastplot.lborder -1 ,
		dw->fastplot.ht - dw->fastplot.bborder + 1,
		dw->fastplot.wid - dw->fastplot.rborder,
		dw->fastplot.ht - dw->fastplot.bborder + 1);

  if (dw->fastplot.yaxis_flag)
  	XDrawLine(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
		dw->fastplot.lborder - 1,
		dw->fastplot.ht - dw->fastplot.bborder,
		dw->fastplot.lborder -1,
		dw->fastplot.tborder);

  /* Draw the thresh ticks */
	DrawTicks(w, 1);

	/* Draw in the titles */
	DrawText(w, dw->fastplot.title, w->core.width / 2 - 20, 15);
	DrawText(w, dw->fastplot.xtextmin, 20, w->core.height - 2);
	DrawText(w, dw->fastplot.xtextmid, w->core.width / 2 - 20,
		w->core.height - 2);
	DrawText(w, dw->fastplot.xtextmax, w->core.width - 20,
		w->core.height - 2);
	DrawText(w, dw->fastplot.ytextmin, 2, w->core.height - 20);
	DrawText(w, dw->fastplot.ytextmid, 2, w->core.height / 2);
	DrawText(w, dw->fastplot.ytextmax, 2, 15);

	/* Draw in the plots */
	if (dw->fastplot.line_flag) {
		XDrawLines(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
			dw->fastplot.pts, dw->fastplot.npts, CoordModeOrigin);
	} else {
		XDrawPoints(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
			dw->fastplot.pts, dw->fastplot.npts, CoordModeOrigin);
	}
}

static void Destroy(w)
	Widget w;
{
}

static void XoFastPlotEnter(w,event)
	Widget	w;
	XButtonEvent	*event;
{
}

static void XoFastPlotLeave(w,event)
	Widget	w;
	XButtonEvent	*event;
{
}

static void  Zoom(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	double zx,zy;
	float mx,my,dx,dy;
	FastPlotWidget	dw = (FastPlotWidget)w;
	static XoEventInfo	info;
	if (*num_params == 2) {
/*	
		zx = atof(params[0]);
		zy = atof(params[1]);
		mx = (dw->fastplot.xmin + dw->fastplot.xmax)/2.0;
		my = (dw->fastplot.ymin + dw->fastplot.ymax)/2.0;
		dx = (dw->fastplot.xmax - dw->fastplot.xmin)/2.0;
		dy = (dw->fastplot.ymax - dw->fastplot.ymin)/2.0;
		dw->fastplot.xmin = mx - dx * zx;
		dw->fastplot.ymin = my - dy * zy;
		dw->fastplot.xmax = mx + dx * zx;
		dw->fastplot.ymax = my + dy * zy;
		info.event = XoUpdateFields;
		info.ret = "xmin xmax ymin ymax";
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->fastplot.callbacks,(XtPointer) &info);
*/
		/* Call the field updating code here */
		Resize(w);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
	}
}

static void  Pan(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	double px,py;
	float mx,my,dx,dy;
	FastPlotWidget	fw = (FastPlotWidget)w;
	static XoEventInfo	info;
	
	if (*num_params == 2) {
		px = atoi(params[0]);
		py = atoi(params[1]);

		DrawTicks(w, 0);

		fw->fastplot.xval += px;
		fw->fastplot.yval -= py;

		if (fw->fastplot.xval > fw->fastplot.wid - fw->fastplot.rborder)
			fw->fastplot.xval = fw->fastplot.wid - fw->fastplot.rborder;
		else if (fw->fastplot.xval < fw->fastplot.lborder)
			fw->fastplot.xval = fw->fastplot.lborder;

		if (fw->fastplot.yval > fw->fastplot.ht - fw->fastplot.bborder)
			fw->fastplot.yval = fw->fastplot.ht - fw->fastplot.bborder;
		else if (fw->fastplot.yval < fw->fastplot.tborder)
			fw->fastplot.yval = fw->fastplot.tborder;

		DrawTicks(w, 1);

		info.event = XoUpdateFields;
		info.ret = "xval yval";
		info.x=info.y=info.z=0.0;

		/* Call the field updating code here */
		XtCallCallbackList(w,fw->fastplot.callbacks,(XtPointer) &info);

		/* Call the script event here */
		info.event = XoKeyPress;
		XtCallCallbackList(w,fw->fastplot.callbacks,(XtPointer) &info);
	}
}

static void  Restore(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	FastPlotWidget dw = (FastPlotWidget)w;
	static XoEventInfo	info;
	dw->fastplot.xval = (dw->fastplot.wid - dw->fastplot.lborder - 
		dw->fastplot.rborder) / 2;
	dw->fastplot.yval = (dw->fastplot.ht - dw->fastplot.tborder - 
		dw->fastplot.bborder) / 2;
		info.event = XoUpdateFields;
		info.ret = "xval yval";
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->fastplot.callbacks,(XtPointer) &info);
		/* Call the field updating code here */
		/* XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True); */
}

#ifdef __STDC_
void XoUndrawFastPlot(Widget w, int start, int npts) {
#else
void XoUndrawFastPlot(w, start, npts)
Widget w;
int start;
int npts;
{
#endif
	FastPlotWidget dw = (FastPlotWidget)w;
  	if (dw->fastplot.reset_flag == 0) return;

	XSetForeground(XtDisplay(w), dw->fastplot.gc,
		dw->core.background_pixel);

/* Need to handle points going round a ring buffer */
	if (dw->fastplot.line_flag) {
		if (start >= 1) {
			npts++;
			start--;
		}
		XDrawLines(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
			dw->fastplot.pts + start, npts , CoordModeOrigin);
	} else {
		XDrawPoints(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
			dw->fastplot.pts + start, npts, CoordModeOrigin);
	}
}

#ifdef __STDC_
void XoUpdateFastPlot(Widget w, int start, int npts) {
#else
void XoUpdateFastPlot(w, start, npts)
Widget w;
int start;
int npts;
{
#endif
	FastPlotWidget dw = (FastPlotWidget)w;
  	if (dw->fastplot.reset_flag == 0) return;

	XSetForeground(XtDisplay(w), dw->fastplot.gc,
		*dw->fastplot.colorptr);

	if (dw->fastplot.line_flag) {
		if (start >= 1) {
			npts++;
			start--;
		}
		XDrawLines(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
			dw->fastplot.pts + start, npts, CoordModeOrigin);
	} else {
		XDrawPoints(XtDisplay(w), XtWindow(w), dw->fastplot.gc, 
			dw->fastplot.pts + start, npts, CoordModeOrigin);
	}
}

#ifdef __STDC_
static void DoKeyPress(Widget w, XEvent *ev,
	String *params, Cardinal *num_params) {
#else
static void DoKeyPress(w, ev, params, num_params)
Widget w;
XEvent *ev;
String *params;
Cardinal *num_params;
{
#endif
	FastPlotWidget fw = (FastPlotWidget) w;
	static XoEventInfo	info;

	if (ev->type != KeyPress) {
		/* Should issue a warning of some sort */
	} else {
		static char            keystr[2];
		KeySym          keysym;

		XLookupString((XKeyEvent *)ev, keystr, 1, &keysym, NULL);
		keystr[1] = '\0';
		if (keysym >= XK_space && keysym <= XK_ydiaeresis) {
			/* Latin 1 characters */
			info.event = XoKeyPress;
			info.key = keystr[0];
			info.ret = keystr;
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,fw->fastplot.callbacks,(XtPointer) &info);
		}
	}
}

static void Notify(w,event,params,num_params)
     Widget w;
	 XButtonEvent	*event;
	 String			*params;
	 Cardinal		*num_params;
{
  FastPlotWidget fw = (FastPlotWidget) w;
  static XoEventInfo	info;
  static long last_time;

  info.event  = XoPress;
	switch(event->button) {
		case Button1: info.event |= XoBut1 ;
			break;
		case Button2: info.event |= XoBut2 ;
			break;
		case Button3: info.event |= XoBut3 ;
			break;
	}
	if (event->time - last_time < XO_DOUBLE_CLICK_TIME)
		info.event |= XoDouble;
	else
		last_time = event->time;

	info.ret="";
	info.x = event->x;
	info.y = event->y;
	info.z = 0;

  XtCallCallbackList(w,fw->fastplot.callbacks, (XtPointer)&info);
}

static Boolean XoCvtIntToString(dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
    static char staticval[50];
    sprintf(staticval, "%d", *(int *)fromVal->addr);
    if (toVal->addr != NULL) {
	if (toVal->size < strlen(staticval) + 1) {
	    toVal->size = strlen(staticval) + 1;
	    return (Boolean) False;
	} else {
	    strcpy(toVal->addr, staticval);
	}
    } else {
	toVal->addr = (XtPointer) staticval;
    }
    toVal->size = strlen(staticval) + 1;
    return (Boolean) True;
}

static void XoRegisterIntToStringConverter ()
{
  XtSetTypeConverter (XtRInt, XtRString,
                      (XtTypeConverter) XoCvtIntToString,
		      NULL, 0,
                      XtCacheNone, NULL);
}
