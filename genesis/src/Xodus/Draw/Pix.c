/* $Id: Pix.c,v 1.2 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Pix.c,v $
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.11  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.10  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.9  2000/05/02 06:06:42  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.8  1996/06/06 20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.7  1995/07/08  03:08:10  dhb
 * Merged 1.5.1.1
 *
<<<<<<< 1.6
 * Revision 1.6  1995/07/08  00:46:03  venkat
 *  Changed some float resource default strings to reflect float values.
 *
=======
 * Revision 1.5.1.1  1995/07/08  03:06:51  dhb
 * Replaced XClearArea() calls with XoDrawForceExpose().
 *
>>>>>>> 1.5.1.1
 * Revision 1.5  1995/03/07  02:01:56  venkat
 * Changed ResetGC to correctly access
 * the xocomposite fg and bg resources for short-term
 * fix.
 *
 * Revision 1.4  1994/06/13  23:05:30  bhalla
 * Fixed click location
 * Added pixflags options
 * Handle SetValues for tx etc
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * Changed Initialize to zero pix.selected. Used for complex pixes with
 * subparts that can be selected
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "CoreDrawP.h"
#include "PixP.h"

#define offset(field) XtOffset(PixObject, pix.field)
static XtResource resources[] = {
  {XtNpixX, XtCPosition, XtRFloat, sizeof(float),
     offset(tx), XtRString, "0.0"},
  {XtNpixY, XtCPosition, XtRFloat, sizeof(float),
     offset(ty), XtRString, "0.0"},
  {XtNpixZ, XtCPosition, XtRFloat, sizeof(float),
     offset(tz), XtRString, "0.0"},
    {XtNforeground, XtCForeground, XtRXoPixel, sizeof(Pixel),
     offset(fg), XtRString, "XtDefaultForeground"},
    {XtNpixflags, XtCPixflags, XtRInt, sizeof(int),
     offset(pixflags), XtRString,"0"},
	{ XtNcallback, XtCCallback, XtRCallback, sizeof (XtPointer),
	offset(callbacks), XtRCallback, (XtPointer) NULL},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static void Project();
static void Expose();
static void Initialize();
static Boolean SetValues();
static void SelectDistance();
static void Select();
static void UnSelect();
static void Motion();
static void Undraw();
static void Highlight();
void XoUndrawByClearing();
static void Destroy();
static XtGeometryResult QueryGeometry();
static void ClassPartInitialize();
static void ClassInitialize();

#define sqr(x) ((x)*(x))

/* class record definition */


PixClassRec pixClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &rectObjClassRec,
    /* class_name         */    "Pix",
    /* size               */    sizeof(PixRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initialize*/  ClassPartInitialize,
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
  },{
    /* Pix Fields */
    /* project */				Project,
	/* undraw */				Undraw,
    /* select_distance */       SelectDistance,
    /* select */           		Select,
	/* unselect */				UnSelect,
	/* motion */				Motion,
	/* highlight */				Highlight,
    /* extension */             NULL
  }
};

WidgetClass pixObjectClass = (WidgetClass)&pixClassRec;

static void ResetGC (pix)
     PixObject pix;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground;
  
  if (pix->pix.gc != NULL)
    XtReleaseGC (XtParent((Widget)pix), pix->pix.gc);

  values.foreground = pix->pix.fg;
  pix->pix.gc = XtGetGC(XtParent((Widget)pix), mask, &values);
}

static void ResetHLGC (pix)
     PixObject pix;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground | GCFunction;
  Display *disp = XtDisplay(XtParent((Widget)pix));

  if (pix->pix.hlgc != NULL)
    XtReleaseGC (XtParent((Widget)pix), pix->pix.hlgc);

  values.foreground = XBlackPixel(disp,XDefaultScreen(disp));
  values.function = GXxor;
  pix->pix.hlgc = XtGetGC(XtParent((Widget)pix), mask, &values);
}



/* At this point the only thing we initialize is the hlgc */
static void Initialize (req, new)
     Widget req, new;
{
  PixObject pix = (PixObject) new;
  CoreDrawWidget dw;
  XGCValues values;

  pix->pix.hlgc = NULL;
  ResetHLGC(pix);

	pix->pix.selected = 0;

	if (XtClass(new) == pixObjectClass) {

  	dw = (CoreDrawWidget)XtParent(new);
  	values.foreground   = dw->xocomposite.fg;
  	values.background   = dw->xocomposite.bg;


  /*
  pix->pix.gc = XtGetGC(req,
	(unsigned)GCForeground | GCBackground,&values);
  */
  	pix->pix.gc=NULL;
  	ResetGC(pix);
  }
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
	PixObject curp = (PixObject) curw;
	PixObject newp = (PixObject) neww;
	Boolean ret = False;

	if (newp->pix.fg != curp->pix.fg) {
		ResetGC(newp);
		ret = True;
	}
	if (XtClass(neww) == pixObjectClass) {
		if (newp->pix.tx != curp->pix.tx ||
			newp->pix.ty != curp->pix.ty ||
			newp->pix.tz != curp->pix.tz) {
			(((PixObjectClass)
				(newp->object.widget_class))->pix_class.project(neww));
			/*  This may be needed later if we put in occlusion
			ResizeDraws(XtParent(neww));
			*/
			/* force a refresh of the entire draw */
			XoDrawForceExpose(XtParent(neww));
			ret = False;
		}
	}

	if (newp->pix.pixflags != curp->pix.pixflags &&
		(newp->pix.pixflags ^ curp->pix.pixflags) & 
		(XO_AFFECTS_DISPLAY)){
		(((PixObjectClass)
			(newp->object.widget_class))->pix_class.project(neww));
		XoDrawForceExpose(XtParent(neww));
	}
	if  (ret && !(newp->pix.pixflags & XO_UPDATE_SELF_ON_SET_NOT))
		return(True);
	else
		return False;
}
  

static void Project (w)
     Widget w;
{
  int x,y,z;
  PixObject pw = (PixObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  void (*tpt)();
  
  /* Calculate position on screen using parents TransformPoint routine */

  if (pw->pix.pixflags & (XO_VISIBLE_NOT | XO_RESIZABLE_NOT)) return;

  if (pw->pix.pixflags & XO_PIXELOFFSET) 
	tpt = PixelTransformPoint;
  else if (pw->pix.pixflags & XO_PCTOFFSET) 
	tpt = PctTransformPoint;
  else
	tpt = class->coredraw_class.transformpt;

  (tpt)(XtParent(w),
	pw->pix.tx,pw->pix.ty, pw->pix.tz,&x, &y,&z);

  /* Set the rectangle geometry fields */
  pw->pix.x = x-10; /* This is a hack so I can click on it */
  pw->pix.y = y-10;
  pw->pix.w = 20;
  pw->pix.h = 20;
}

static void Expose (w)
     Widget w;
{
  PixObject pw = (PixObject) w;
 
  if (pw->pix.pixflags & XO_VISIBLE_NOT) return;
  /* Draw a cross centered on the position of the pix */

  XoDrawLine(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
	pw->pix.gc,pw->pix.x+10,0,pw->pix.x+10,XtParent(w)->core.height);
  XoDrawLine(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
	pw->pix.gc,0,pw->pix.y+10,XtParent(w)->core.width,pw->pix.y+10);
	if (pw->pix.pixflags & XO_IS_SELECTED) { /* Do the highlight */
		pw->pix.pixflags &= ~XO_IS_SELECTED;
		Highlight(pw,(XButtonEvent *)NULL);
	}
}

static void Destroy(w)
	Widget w;
{
  /*
  PixObject pw = (PixObject) w;
  if (pw->pix.gc != NULL)
  	XtReleaseGC(XtParent(w),pw->pix.gc);
	*/
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	PixObject pw = (PixObject)w;
	preferred->x = pw->pix.x;
	preferred->y = pw->pix.y;
	preferred->width = pw->pix.w;
	preferred->height = pw->pix.h;
	if (intended->request_mode & (CWX | CWY | CWWidth | CWHeight)) {
		if (preferred->x == intended->x &&  
			preferred->y == intended->y &&  
			preferred->width == intended->width &&  
			preferred->height == intended->height)
			return(XtGeometryYes);
	} else {
		if (preferred->x == pw->pix.x &&  
			preferred->y == pw->pix.y &&  
			preferred->width == pw->pix.w &&  
			preferred->height == pw->pix.h)
			return(XtGeometryYes);
	}
	return(XtGeometryAlmost);
}

/* In more complex pixes, SelectDistance is supposed to place in cz the
** z coord in 3-d of the nearest point on the pix, for example, the
** z of the nearest dendrite. In the default case we leave cz alone.
*/
static void SelectDistance (w, x, y, dist)
     Widget w;
     Position x, y;
	 Dimension *dist;
{
  int ans;
  PixObject pw = (PixObject) w;

  /* Calculate the distance from the given point to the boundary of the 
     2 dimensional drawn image. */
	if (pw->pix.pixflags & XO_CLICKABLE_NOT) {
		*dist = XO_MAX_SEL_DIST;
		return;
	}

  ans = (int) sqrt ( (double) sqr (x - pw->pix.x) +
		     (double) sqr (y - pw->pix.y) ); 
	*dist = (Dimension) (ans > 0 ? ans : 0);
}

static void Select(w,event,info)
     Widget w;
	 XButtonEvent	*event;
	 XoEventInfo    *info;
{
	PixObject pw = (PixObject) w;
	PixObjectClass class = (PixObjectClass) pw->object.widget_class;

	/* 
	** The original code below is pointless because pix.w and pix.h are
	** unsigned quantities.  Removing the tests.
	** wid = (pw->pix.w < 0) ? 10 : pw->pix.w + 10;
	** h = (pw->pix.h < 0) ? 10 : pw->pix.h + 10;
	*/
	XoCopyCursor(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
		pw->pix.gc,
		pw->pix.x, pw->pix.y, pw->pix.w, pw->pix.h);
	(class->pix_class.highlight)(w,event);
	XtCallCallbackList(w,pw->pix.callbacks, (XtPointer)info);
}

static void UnSelect(w,event)
     Widget w;
	 XButtonEvent	*event;
{
	PixObject pw = (PixObject) w;
	PixObjectClass class = (PixObjectClass) pw->object.widget_class;

	XoRestoreCursor(XtDisplay(XtParent(w)),XtWindow(XtParent(w)));
	(class->pix_class.highlight)(w,event);
}

/* Depending on the state of the widget, the highlight has to
** decide what to do next. For the standard pix it just redraws
** an XORed rectangle and flips the 'is_selected' flag */
static void Highlight(w,event)
     Widget w;
	 XButtonEvent	*event;
{
	PixObject pw = (PixObject) w;
	unsigned int wid,h;

	/* Here we are just flipping the flag. In other cases we
	** might make the drawing operation depend on the flag */
	if (pw->pix.pixflags & XO_IS_SELECTED)
		pw->pix.pixflags &= ~XO_IS_SELECTED;
	else
		pw->pix.pixflags |= XO_IS_SELECTED;

	if (!(pw->pix.pixflags & XO_HLT1_NOT)) {
	    /* 
	    ** The original code below is pointless because the results
	    ** unsigned quantities.  Removing the tests.
	    ** wid = (pw->pix.w < 0) ? 10 : pw->pix.w + 10;
	    ** h = (pw->pix.h < 0) ? 10 : pw->pix.h + 10;
	    */
	    wid = pw->pix.w + 10;
	    h =  pw->pix.h + 10;

	/* Draw the rectangle using an XOR GC so it alternates on and off*/
		XFillRectangle(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			pw->pix.hlgc,
			pw->pix.x - 5,pw->pix.y - 5,wid,h);
	}
	if (pw->pix.pixflags & XO_HLT2) {
			XDrawImageString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)),
				pw->pix.hlgc,
				pw->pix.x + pw->pix.w/2,
				pw->pix.y + pw->pix.h/2,
				"*",1);
	}
}

static void Undraw(w,event)
     Widget w;
	 XButtonEvent	*event;
{
}

/* This special option is really to provide a pointer for the parent
** draw widget to check for */
void XoUndrawByClearing(w,event)
     Widget w;
	 XButtonEvent	*event;
{
}

static void Motion(w,event)
     Widget w;
	 XButtonEvent	*event;
{
}

static void ClassInitialize()
{
}

static void ClassPartInitialize(wc)
	WidgetClass wc;
{
	PixObjectClass poc = (PixObjectClass) wc;

	if (poc->pix_class.project == XoInheritProject)
		poc->pix_class.project = Project;
	if (poc->pix_class.undraw == XoInheritUndraw)
		poc->pix_class.undraw = Undraw;
	if (poc->pix_class.select_distance == XoInheritSelectDistance)
		poc->pix_class.select_distance = SelectDistance;
	if (poc->pix_class.select == XoInheritSelect)
		poc->pix_class.select = Select;
	if (poc->pix_class.unselect == XoInheritUnSelect)
		poc->pix_class.unselect = UnSelect;
	if (poc->pix_class.motion == XoInheritMotion)
		poc->pix_class.motion = Motion;
	if (poc->pix_class.highlight == XoInheritHighlight)
		poc->pix_class.highlight = Highlight;
}

/* Utility function for generating an expose event which will
** force recalculation of projections for a pix */
void XoProjectAndDrawPix(w,do_refresh)
	Widget	w;
	int		do_refresh;
{
	PixObject	pw = (PixObject)w;
	PixObjectClass	class;

	class = (PixObjectClass) pw->object.widget_class;
	(class->pix_class.project)(pw);
	if (do_refresh)
		XoDrawForceExpose(XtParent(w));
}
