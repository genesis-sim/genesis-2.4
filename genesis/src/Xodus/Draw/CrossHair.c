/* $Id: CrossHair.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: CrossHair.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.5  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1996/06/06 20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.3  1994/03/22  15:28:02  bhalla
 * RCS error: no code changes
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
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
     offset(tx), XtRImmediate, "0"},
  {XtNpixY, XtCPosition, XtRFloat, sizeof(float),
     offset(ty), XtRImmediate, "0"},
  {XtNpixZ, XtCPosition, XtRFloat, sizeof(float),
     offset(tz), XtRImmediate, "0"},
    {XtNforeground, XtCForeground, XtRXoPixel, sizeof(Pixel),
     offset(fg), XtRString, "XtDefaultForeground"},
    {XtNrefresh, XtCRefresh, XtRInt, sizeof(int),
     offset(pixflags), XtRString,"51"},
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
	/* hightlight */			Highlight,
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


static void Initialize (req, new)
     Widget req, new;
{
  PixObject pix = (PixObject) new;
  CoreDrawWidget dw;
  XGCValues values;

  /*
  dw = (CoreDrawWidget)XtParent(new);
  values.foreground   = dw->coredraw.fg;
  values.background   = dw->core.background_pixel;


  pix->pix.gc = XtGetGC(req,
	(unsigned)GCForeground | GCBackground,&values);
  */
  pix->pix.gc=NULL;
  ResetGC(pix);
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  PixObject curs = (PixObject) curw;
  PixObject news = (PixObject) neww;

  if (news->pix.fg != curs->pix.fg) {
    ResetGC(news);
    return True;
  }
  if (news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz) {
		Project(neww);
		if (news->pix.w != curs->pix.w ||
			news->pix.h != curs->pix.h)
			return(True);
		else
    		return False;
	}
}
  

static void Project (w)
     Widget w;
{
  int x,y,z;
  int ex,ey,ez;
  int tx,ty;
  PixObject pw = (PixObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int parentwid= ((CoreDrawWidget)XtParent(w))->core.width;
  int parentht= ((CoreDrawWidget)XtParent(w))->core.height;
  
  /* Calculate position on screen using parents TransformPoint routine */
  (class->coredraw_class.transformpt)(XtParent(w),
	pw->pix.tx,pw->pix.ty, pw->pix.tz,&x, &y,&z);

  /* Set the rectangle geometry fields */
  pw->pix.x = x;
  pw->pix.y = y;
  pw->pix.w = 10; /* This is a hack so I can click on it */
  pw->pix.h = 10;
}

static void Expose (w)
     Widget w;
{
  PixObject pw = (PixObject) w;
 
  /* Draw a cross centered on the position of the pix */

  XoDrawLine(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
	pw->pix.gc,pw->pix.x,0,pw->pix.x,XtParent(w)->core.height);
  XoDrawLine(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
	pw->pix.gc,0,pw->pix.y,XtParent(w)->core.width,pw->pix.y);
}

static void Destroy(w)
	Widget w;
{
  PixObject pw = (PixObject) w;
  XtReleaseGC(XtParent(w),pw->pix.gc);
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

static void SelectDistance (w, x, y, dist)
     Widget w;
     Position x, y;
	 Dimension *dist;
{
  int ans;
  PixObject pw = (PixObject) w;

  /* Calculate the distance from the given point to the boundary of the 
     2 dimensional drawn image. */

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

	XtCallCallbackList(w,pw->pix.callbacks, (XtPointer)info);
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
}

/* Utility function for generating an expose event which will
** force recalculation of projections for a pix */
void XoProjectAndDrawPix(w)
	Widget	w;
{
	PixObject	pw = (PixObject)w;
	PixObjectClass	class;

	class = (PixObjectClass) pw->object.widget_class;
	(class->pix_class.project)(pw);
	(class->rect_class.expose)(pw);
}
