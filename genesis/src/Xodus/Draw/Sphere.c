/* $Id: Sphere.c,v 1.2 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Sphere.c,v $
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.8  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.7  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.6  1996/06/06 20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.5  1995/07/08  00:46:36  venkat
 *  Changed some float resource default strings to reflect float values.
 *
 * Revision 1.4  1994/06/13  23:06:26  bhalla
 * Added default radius of 1
 * Fixed updating in SetValues
 * Added pixflags
 *
 * Revision 1.3  1994/03/22  15:30:12  bhalla
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
#include "SphereP.h"

#define offset(field) XtOffset(SphereObject, sphere.field)
static XtResource resources[] = {
  {XtNr, XtCPosition, XtRFloat, sizeof(float),
     offset(r), XtRString, "1.0"},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static void Expose();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();

/* class record definition */


SphereClassRec sphereClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Sphere",
    /* size               */    sizeof(SphereRec),
    /* class_initialize   */    NULL,
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
    /* select_distance */       XoInheritSelectDistance,
    /* select */           		XoInheritSelect,
    /* unselect */         		XoInheritUnSelect,
    /* motion */           		XoInheritMotion,
    /* highlight */        		XoInheritHighlight,
    /* extension */             NULL
  },
  {
	/* sphere fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass sphereObjectClass = (WidgetClass)&sphereClassRec;

static void ResetGC (sphere)
     SphereObject sphere;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground;
  
  if (sphere->pix.gc != NULL)
    XtReleaseGC (XtParent((Widget)sphere), sphere->pix.gc);

  values.foreground = sphere->pix.fg;
  sphere->pix.gc = XtGetGC(XtParent((Widget)sphere), mask, &values);
}


static void Initialize (req, new)
     Widget req, new;
{
  SphereObject sphere = (SphereObject) new;

  sphere->pix.gc=NULL;
  ResetGC(sphere);
  sphere->sphere.r = 1.0;
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  SphereObject curs = (SphereObject) curw;
  SphereObject news = (SphereObject) neww;

  Boolean ret = False;

  if (news->pix.fg != curs->pix.fg) {
    ResetGC(news);
    ret = True;
  }
  if (news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->sphere.r != curs->sphere.r) {
		Project(neww);
		XClearArea(XtDisplay(XtParent(neww)),
			XtWindow(XtParent(neww)),0,0,0,0,True);
		ret = False; /* since we have already done an update */
	}
	if (ret && !(news->pix.pixflags & XO_UPDATE_SELF_ON_SET_NOT))
		return(True);
	else
		return(False);
}
  
static void Expose (w)
     Widget w;
{
  SphereObject sw = (SphereObject) w;
 
  /* Draw the two dimensional image according to the information
     cached by the draw method.  The fact that it is, in this case, more
     convenient to use the rectangle fields is a coincidence.  Normally
     such a command would use the specially stored info. */

	if (sw->pix.pixflags & XO_VISIBLE_NOT) return;

  XoFillArc (XtDisplay(XtParent(w)), XtWindow(XtParent(w)), sw->pix.gc, 
		sw->pix.x,sw->pix.y,sw->pix.w,sw->pix.h,
		 /*
	     sw->sphere.screenx - sw->sphere.screenr, 
		 sw->sphere.screeny - sw->sphere.screenr,
	     sw->sphere.screenr + sw->sphere.screenr,
	     sw->sphere.screenr + sw->sphere.screenr,
		 */
	     0, 360 * 64);
	     
}

static void Destroy(w)
	Widget w;
{
  SphereObject sw = (SphereObject) w;
  XtReleaseGC(XtParent(w),sw->pix.gc);
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	SphereObject sw = (SphereObject)w;
	preferred->x = sw->pix.x;
	preferred->y = sw->pix.y;
	preferred->width = 2*sw->sphere.screenr;
	preferred->height = 2*sw->sphere.screenr;
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




static void Project (w)
     Widget w;
{
  int x,y,z;
  int ex,ey,ez;
  int tx,ty;
  SphereObject sw = (SphereObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int parentwid= ((CoreDrawWidget)XtParent(w))->core.width;
  int parentht= ((CoreDrawWidget)XtParent(w))->core.height;
  void (*tpt)();
  

  if (sw->pix.pixflags  & (XO_VISIBLE_NOT | XO_RESIZABLE_NOT)) return;
 
/* we would do something similar here with transformpts if we had
** multiple points to display. See Shape.c for that case */
  if (sw->pix.pixflags & XO_PIXELOFFSET) 
    tpt = PixelTransformPoint;
  else if (sw->pix.pixflags & XO_PCTOFFSET) 
    tpt = PctTransformPoint;
  else
    tpt = class->coredraw_class.transformpt;

 /* Calculate position on screen using parents TransformPoint routine */
  (tpt)(XtParent(w),sw->pix.tx,sw->pix.ty, sw->pix.tz,&x, &y,&z);

  /* Calculate position of axes on screen. Objective is to get screen
  ** radius of sphere */
  (tpt)(XtParent(w),
	sw->pix.tx+sw->sphere.r,sw->pix.ty, sw->pix.tz,&ex, &ey,&ez);
	tx = (ex-x) * (ex-x); ty = (ey-y) * (ey-y);
  (tpt)(XtParent(w),
	sw->pix.tx,sw->pix.ty + sw->sphere.r, sw->pix.tz,&ex, &ey,&ez);
	tx += (ex-x) * (ex-x); ty += (ey-y) * (ey-y);
  (tpt)(XtParent(w),
	sw->pix.tx,sw->pix.ty, sw->pix.tz + sw->sphere.r,&ex, &ey,&ez);
	tx += (ex-x) * (ex-x); ty += (ey-y) * (ey-y);

	if ((tx+ty) <= 0  ||
		(tx + ty) > (parentwid + parentht) * (parentwid + parentht))
		sw->sphere.screenr = 1;
	else
		sw->sphere.screenr = (Dimension)sqrt((double)(tx + ty));

  /* Set the rectangle geometry fields */
  sw->pix.x = x - sw->sphere.screenr;
  sw->pix.y = y - sw->sphere.screenr;
  sw->pix.w = 2*sw->sphere.screenr;
  sw->pix.h = 2*sw->sphere.screenr;
}
