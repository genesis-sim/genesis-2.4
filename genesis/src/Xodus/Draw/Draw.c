/* $Id: Draw.c,v 1.3 2005/07/29 16:00:15 svitak Exp $ */
/*
 * $Log: Draw.c,v $
 * Revision 1.3  2005/07/29 16:00:15  svitak
 * Removed preprocessor logic for GCC 2.6.3 and OPTIMIZE. If still needed
 * it should be handled in some central include file.
 *
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.21  2000/06/12 04:05:00  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) XoDrawLeave used variable "pix" before setting value.
 * 3) Fixed obvious case of using "==" when "=" was meant.
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
 * Revision 1.16  1996/10/07  22:13:55  dhb
 * Fix for missing atof() return type on Linux/FreeBSD systems resulting
 * in screwy pan and zoom behaviour.
 *
 * Revision 1.15  1996/06/28  00:48:04  venkat
 * stdlib.h was not getting included with a -g compilation.
 * So changed the preprocessor conditional to do the include with ot
 * without __OPTIMIZE__
 *
 * Revision 1.14.1.2  1996/09/13  23:59:29  venkat
 * Cleaned-up-the-preprocessor-statements-for-Linux-stdlib.h-include
 *
 * Revision 1.14.1.1  1996/08/21  21:05:52  venkat
 * Upi's fix for Pan/Zoom bug in Linux-ELF: Explicit declaration of atof
 * for the Linux/FreeBSD case
 *
 * Revision 1.14  1995/07/28  23:35:17  dhb
 * Undefine __OPTIMIZE__ macro for gcc which causes incompatible
 * redefinition of functions in stdlib.h.
 *
 * Revision 1.13  1995/07/17  20:42:05  dhb
 * Moved general vector functions to DrawUtil.c as these are
 * now also used by Cell.c.
 *
 * Revision 1.12  1995/07/08  03:04:12  dhb
 * Added XoDrawForceExpose() which will compress multiple calls
 * into a single XClearArea() to avoid multiple expose events.
 *
 * Revision 1.11  1995/06/30  17:41:22  dhb
 * Merged 1.10.1.2 onto main trunk.
 *
 * Revision 1.10.1.2  1995/06/16  05:53:57  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.10.1.1  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.10.2.2  1995/07/15  01:30:51  dhb
 * Mostly done diarange changes.  Need to merge onto trunk for testing
 * to include XoDrawForceExpose() which is referenced elsewhere.
 *
 * Revision 1.10.2.1  1995/06/30  17:36:56  dhb
 * Moved partial changes involving Drawlib vector code to RCS branch.
 *
 * Revision 1.11  1995/06/10  01:24:05  dhb
 * Partial changes to move generic Drawlib vector handling code to
 * DrawUtil.[ch] so it can be used in Cell.c as well as Draw.c
 *
 * Revision 1.10  1995/06/07  19:16:32  venkat
 * The LoadTransfMatrix call moved form the Initialize to the
 * Resize method. This needs to be done every time the draw is resized
 * so the child pixes in it get rescaled
 *
 * Revision 1.9  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.8  1995/03/17  20:36:49  venkat
 * Upi Changes: Include stdlib.h for atof() under Linux
 * Used being_destroyed flag in Resize and Expose methods
 * Added function XoDrawLeave()
 *
 * Revision 1.7  1995/03/07  01:00:29  venkat
 * Class record changed to hold XoComposite class field
 *
 * Revision 1.6  1994/06/29  14:21:12  bhalla
 * in SetValues: changing transform now forces update
 * in SetTransf: put in correct field name for callback XoUpdateFields
 *
 * Revision 1.5  1994/05/25  13:47:29  bhalla
 * in XoCvtStringToTransform: bugfix.
 *
 * Revision 1.4  1994/03/22  15:29:09  bhalla
 * Fixes for back transform for x and y transforms
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * Did some fixing of BackTransform to handle xy plane, others are still
 * a problem.
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
#include "DrawP.h"
#include "PixP.h"
#include "DrawUtil.h"

#define	XO_DRAW_FORCE_EXPOSE	0x100



/*
** This is a simple-minded widget for holding pixes. Its transforms are 
** just scalings on x,y,or z axis projections. It is a good template for
** more advanced coredraw based widgets.
*/

static XtResource resources[] = {
#define offset(field) XtOffset(DrawWidget, draw.field)
  {XtNzmin, XtCPosition, XtRFloat,  sizeof(float),
	 offset(zmin), XtRString, "0"},
  {XtNzmax, XtCPosition, XtRFloat,  sizeof(float),
	 offset(zmax), XtRString, "1"},
  {XtNperspective, XtCPerspective, XtRInt, sizeof(int),
	 offset(perspective), XtRString, "0"},
  {XtNtransform, XtCTransform, XtRTransform, sizeof(char),
	 offset(transform), XtRString, "z"},
  {XtNrv, XtCPosition, XtRFloat, sizeof(float),
	 offset(rv), XtRString, "1"},
  {XtNrz, XtCPosition, XtRFloat, sizeof(float),
	 offset(rz), XtRString, "0"},
  {XtNvx, XtCPosition, XtRFloat, sizeof(float),
	 offset(vx), XtRString, "1"},
  {XtNvy, XtCPosition, XtRFloat, sizeof(float),
	 offset(vy), XtRString, "1"},
  {XtNvz, XtCPosition, XtRFloat, sizeof(float),
	 offset(vz), XtRString, "1"},
 /*
  {XtNax, XtCPosition, XtRFloat, sizeof(float),
	 offset(ax), XtRString, "1"},
  {XtNay, XtCPosition, XtRFloat, sizeof(float),
	 offset(ay), XtRString, "1"},
  {XtNaz, XtCPosition, XtRFloat, sizeof(float),
	 offset(az), XtRString, "0"},
 */
  {XtNmatrix, XtCMatrix, XtRPointer, sizeof(XtPointer),
	 offset(matrix), XtRImmediate, (XtPointer)NULL},
#undef offset
};

/* 
** The main mouse callback operation should be selection of pixes
** The main keyboard callback operation should be zooming and panning
*/
static void XoDrawSelectPix();
static void XoDrawUnSelectPix();
static void XoDrawEnter();
static void XoDrawLeave();
#if defined(Linux) || defined(__FreeBSD__)
/*
XtActionProc Zoom, Pan, Restore, SetTransf,Rotate,PZoom,Print;
*/
/* static void  Zoom(w,event,params,num_params) */
/* static void  Zoom(Widget,XEvent *,String *,Cardinal *); */
static void  Zoom(Widget w,XEvent *event,String *params,Cardinal *num_params);
static void  Pan(Widget,XEvent *,String *,Cardinal *);
static void  Restore(Widget,XEvent *,String *,Cardinal *);
static void  SetTransf(Widget,XEvent *,String *,Cardinal *);
static void  Rotate(Widget,XEvent *,String *,Cardinal *);
static void  PZoom(Widget,XEvent *,String *,Cardinal *);
static void  Print(Widget,XEvent *,String *,Cardinal *);
#else
static void  Zoom(), Pan(), Restore(), SetTransf();
/* arguments are : x,y (fractions of screen, relative) */
static void Rotate();
static void PZoom();
static void Print();
#endif
static void Initialize(),Destroy(), Resize();
static void Redisplay();
static void TransformPoint(),TransformPoints(),BackTransform();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static Boolean XoCvtStringToTransform();
static Boolean XoCvtTransformToString();
static void ClassInitialize();
static int LoadTransfMatrix();
/* static void Rotate(); */

static XtActionsRec actions[] =
{
  {"pan",	 Pan},
  {"zoom", Zoom},
  {"pzoom", PZoom},
  {"rotate", Rotate},
  {"restore", Restore},
  {"print", Print},
  {"select", XoDrawSelectPix},
  {"unselect", XoDrawUnSelectPix},
  {"set_transf", SetTransf},
  {"enter", XoDrawEnter},
  {"leave", XoDrawLeave},
};

static char translations[] =
" Shift<Key>Left:	rotate(0,-0.1) \n\
  Shift<Key>Right:	rotate(0,0.1) \n\
  Shift<Key>Down:	rotate(-0.1,0) \n\
  Shift<Key>Up:		rotate(0.1,0) \n\
  <Key>a:			rotate(-0.1,0) \n\
  <Key>q:			rotate(0.1,0) \n\
  <Key>Left:		pan(0.1,0)	\n\
  <Key>Right:       pan(-0.10,0)	\n\
  <Key>Down:        pan(0,0.10)       \n\
  <Key>Up:   		pan(0,-0.10)      \n\
  <Key><:	zoom(1.05)	\n\
  <Key>>:	zoom(0.95)	\n\
  Shift<Key>R:	pzoom(1.1)	\n\
  Shift<Key>X:	set_transf(\"X\")	\n\
  Shift<Key>Y:	set_transf(\"Y\")	\n\
  Shift<Key>Z:	set_transf(\"Z\")	\n\
  Ctrl<Key>P:	print()		\n\
  <Key>r:	pzoom(0.9)	\n\
  <Key>[:	pzoom(0.8)	\n\
  <Key>]:	pzoom(1.2)	\n\
  <Key>u:	restore()	\n\
  <Key>x:	set_transf(\"x\")	\n\
  <Key>y:	set_transf(\"y\")	\n\
  <Key>z:	set_transf(\"z\")	\n\
  <Key>o:	set_transf(\"o\")	\n\
  <Key>p:	set_transf(\"p\")	\n\
  <BtnDown> : select()	\n\
  <BtnUp> : unselect()	\n\
  <Enter> : enter()	\n\
";

DrawClassRec drawClassRec = {
  { /* core fields */
  /* superclass		*/	(WidgetClass) &coredrawClassRec,
    /* class_name		*/	"Draw",
    /* widget_size		*/	sizeof(DrawRec),
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
    /* insert_child       */    XtInheritInsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    NULL
  },
  { /* xocomposite_class fields */
	/* empty				*/  0
  },
  { /* coredraw_class fields */
    /* transform_point		*/	TransformPoint,
    /* transform_points		*/	TransformPoints,
    /* backtransform		*/	BackTransform,
	/* selectpix			*/	XoInheritSelectPix,
	/* unselectpix			*/	XoInheritUnSelectPix,
    /* extension			*/	NULL
  },
  { /* draw_class fields */
	/* empty				*/  0
  }
};

WidgetClass drawWidgetClass = (WidgetClass)&drawClassRec;

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRTransform,
                      XoCvtStringToTransform, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRTransform, XtRString,
                      XoCvtTransformToString, NULL, 0 ,
                      XtCacheNone, NULL);
}

/* ARGUSED */
static void Initialize (req, new,args, num_args)
     Widget req, new;
	 ArgList	args;
	 Cardinal	*num_args;
{
	DrawWidget dw = (DrawWidget) new;
	Cursor XoDefaultCursor();
	int i;

	/* This is a hack. I need to figure out a way of doing it
	** properly */
	if (dw->core.width == 0)
		dw->core.width = 20;
	if (dw->core.height == 0)
		dw->core.height = 20;
	if (dw->draw.matrix == NULL) {
		dw->draw.matrix = (float **)calloc(4,sizeof(float *));
		for (i = 0; i < 4; i++) {
			dw->draw.matrix[i] = (float *)calloc(4,sizeof(float));
		}
	}
	dw->draw.ax = dw->draw.vx;
	dw->draw.ay = dw->draw.vy;
	dw->draw.az = 0;
	XoVNormalize(&dw->draw.ax,&dw->draw.ay,&dw->draw.az);

	if (dw->draw.rv <= 0)
		dw->draw.rv = 1;

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
*/
	int redo_transf = 0;
	DrawWidget dcw = (DrawWidget) curr;
	DrawWidget drw = (DrawWidget) req;
	DrawWidget dnw = (DrawWidget) new;

	/* Check if field is interesting */
	/* Note that the xmin etc fields are dealt with in the superclass,
	** but we do need to redo the transf matrix */
	if (((drw->coredraw.xmin != dcw->coredraw.xmin) ||
		drw->coredraw.xmax != dcw->coredraw.xmax)) {
		/* check if the ranges are consistent */
		if (drw->coredraw.xmin > drw->coredraw.xmax) {
			dnw->coredraw.xmin = dcw->coredraw.xmin;
			dnw->coredraw.xmax = dcw->coredraw.xmax;
			return(False);
		} else {
			redo_transf = 1;
		}
	}

	if (((drw->coredraw.ymin != dcw->coredraw.ymin) ||
		drw->coredraw.ymax != dcw->coredraw.ymax)) {
		/* check if the ranges are consistent */
		if (drw->coredraw.ymin > drw->coredraw.ymax) {
			dnw->coredraw.ymin = dcw->coredraw.ymin;
			dnw->coredraw.ymax = dcw->coredraw.ymax;
			return(False);
		} else {
			redo_transf = 1;
		}
	}

	if (((drw->draw.zmin != dcw->draw.zmin) ||
		drw->draw.zmax != dcw->draw.zmax)) {
		/* check if the ranges are consistent */
		if (drw->draw.zmin > drw->draw.zmax) {
			dnw->draw.zmin = dcw->draw.zmin;
			dnw->draw.zmax = dcw->draw.zmax;
			return(False);
		} else {
			redo_transf = 1;
		}
	}

	if (drw->draw.vx != dcw->draw.vx ||
		drw->draw.vy != dcw->draw.vy ||
		drw->draw.vz != dcw->draw.vz) {
		if (XoVLength(drw->draw.vx,drw->draw.vy,0.0) > 0) {
			dnw->draw.ax = drw->draw.vx;
			dnw->draw.ay = drw->draw.vy;
			dnw->draw.az = 0;
			XoVNormalize(&(dnw->draw.ax),&(dnw->draw.ay),&(dnw->draw.az));
			redo_transf = 1;
		} else {
			dnw->draw.ax = 0.0;
			dnw->draw.ay = 1.0;
			redo_transf = 1;
		}
	}
	if (drw->draw.rv != dcw->draw.rv ||
		drw->draw.rz != dcw->draw.rz) {
		if (drw->draw.rv <= 0) {
			drw->draw.rv = 1;
		}
		redo_transf = 1;
	}
	if (drw->draw.transform != dcw->draw.transform)
		redo_transf = 1;

	/* Setting aw is illegal in this version */
	if (dnw->draw.matrix!=dcw->draw.matrix) {
		if (dnw->draw.matrix == NULL) {
			dnw->draw.matrix = dcw->draw.matrix;
			return(False);
		}
		return(True);
	}
	if (redo_transf) {
		LoadTransfMatrix(dnw);
		Resize(dnw);
		return(True);
	}
	return(False);
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
  DrawWidget dw = (DrawWidget) w;
 Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;
  PixObjectClass class;

	if (!XtIsRealized((Widget) dw))
		return;

	/** This call has to preceed the actual projection of the
	    child pixes since the resizing of the parent form or the
	    draw has to rescale the pixes as well **/

	LoadTransfMatrix(dw);

	/* handle children */
  	for (childP = first_born;
		childP - first_born < num_children; childP++) {
	 	/* Avoid the horrible situation of trying to display a pix
                ** that is slated for destruction. This can happen as
                ** the actual process of getting rid of the child is the
                ** last thing that happens before an event returns. In
                ** the meantime, however, there could be a call to a display
                ** routine which would end up here.
                */
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
  DrawWidget dw = (DrawWidget) w;
  Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;

  dw->coredraw.drawflags &= ~XO_DRAW_FORCE_EXPOSE;

	if ((dw->coredraw.drawflags & XO_DRAW_REFRESHABLE_NOT) ||
		!XtIsRealized((Widget) dw))
		return;

	/* handle children */
  for (childP = first_born;childP - first_born < num_children;childP++){
	if (((PixObject)*childP)->object.being_destroyed)
                        continue;
    if (region == NULL ||
	((*childP)->core.width == 0 &&
		(*childP)->core.height == 0) ||
	(XRectInRegion(region, (*childP)->core.x, (*childP)->core.y,
		       (*childP)->core.width, (*childP)->core.height) 
	   != RectangleOut)){
/*	   if (!dw->draw.perspective || ((*(Pix *)childP))->pix.tz < 0) */
      	(*(XtClass(*childP))->core_class.expose)((*childP), e, region);
    }
   }
}

static XtGeometryResult GeometryManager(w,request,reply)
	Widget w;
	XtWidgetGeometry	*request;
	XtWidgetGeometry	*reply;	/* RETURN */
{
	return XtGeometryYes;
}

static void Destroy(w)
	Widget w;
{
	DrawWidget dw=(DrawWidget)w;
}

/* This is inherited from the core widget class */
static void XoDrawSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(w);
	(class->coredraw_class.selectpix)(w,event);
}

/* This is inherited from the core widget class */
static void XoDrawUnSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(w);
	(class->coredraw_class.unselectpix)(w,event);
}

/* This is the same as the core widget class, but is not yet
** inherited */

static void XoDrawEnter(w,event)
    Widget  w;
    XButtonEvent    *event;
{
  DrawWidget dw = (DrawWidget) w;
  Widget pix;
  static XoEventInfo    info;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(w);

	/*
	if (!XtIsRealized(dw))
		return;
	XDefineCursor(XtDisplay(w),XtWindow(w),XoDefaultCursor());
	*/

  pix = XoFindNearestPix(w,event);
  if (pix != NULL) {
        /* Call the callbacks for a possible drag-drop operation
        ** within the draw widget */
        info.event = XoEnter;
        info.ret="";
        info.z = (pix == w) ? 0.0 : ((PixObject)pix)->pix.cz;
		(class->coredraw_class.backtransform)(
			w,event->x,event->y,
			&(info.x),&(info.y),&(info.z));
        info.key = event->time;
        /* Calling the drag callback */
        if (pix == w) {
	    dw->coredraw.pix_selected = -1;
            XtCallCallbackList(w,dw->coredraw.callbacks,
                (XtPointer)&info);
        } else {
	    if (((PixObject)pix)->pix.selected >= 0)
		info.ret = (char *)(&((PixObject)pix)->pix.selected);
	    dw->coredraw.pix_selected = ((PixObject)pix)->pix.selected;
            XtCallCallbackList(pix,((PixObject)pix)->pix.callbacks,
                (XtPointer)&info);
	}
  }
}

static void XoDrawLeave(w,event)
    Widget  w;
    XButtonEvent    *event;
{
    DrawWidget dw = (DrawWidget) w;
    Widget pix;
    static XoEventInfo info;

    /* call the callbacks for a possible drag-drop operation
    ** within the draw widget */
    info.event = XoLeave;
    info.ret = "";
    info.x= event->x;
    info.y = event->y;
    info.z = 0;
 
    /* 2000-06-10 <mhucka@bbb.caltech.edu> I don't know whether the
    ** following line is correct, but previously, pix was never assigned,
    ** so something needed to be done here.  XoDrawEnter() uses this,
    ** so let's try it here too.
    */
    pix = XoFindNearestPix(w,event);
    /* calling the drag callback */
    if (pix == w)
	XtCallCallbackList(w,dw->coredraw.callbacks,
			   (XtPointer)&info);
}
 



static int LoadTransfMatrix(dw)
	DrawWidget	dw;
{
	float **tm = dw->draw.matrix;
	int i,j;

		switch(dw->draw.transform) {
			case 'x':
			case 'X':
				for(i = 0; i < 4; i++)
					for(j = 0; j < 4; j++) 
						tm[i][j] = 0;
				tm[0][1] = (float)(dw->core.width) / 
					(dw->coredraw.xmax - dw->coredraw.xmin);
				tm[0][3] = -(float)(dw->core.width) * dw->coredraw.xmin/
					(dw->coredraw.xmax - dw->coredraw.xmin);

				/* because of the flipped axes in XWindows, we need
				** to do some juggling for the Y axis */
				tm[1][2] = -(float)(dw->core.height) / 
					(dw->coredraw.ymax - dw->coredraw.ymin);
				tm[1][3] = (float)(dw->core.height) *
					(1.0 + dw->coredraw.ymin/
					(dw->coredraw.ymax - dw->coredraw.ymin));

				tm[2][0] = 1.0 / (dw->draw.zmax - dw->draw.zmin);
				tm[2][3] = -dw->draw.zmin;
				break;
			case 'y':
			case 'Y':
				for(i = 0; i < 4; i++)
					for(j = 0; j < 4; j++) 
						tm[i][j] = 0;
				tm[0][0] = (float)(dw->core.width) / 
					(dw->coredraw.xmax - dw->coredraw.xmin);
				tm[0][3] = -(float)(dw->core.width) * dw->coredraw.xmin/
					(dw->coredraw.xmax - dw->coredraw.xmin);

				/* because of the flipped axes in XWindows, we need
				** to do some juggling for the Y axis */
				tm[1][2] = -(float)(dw->core.height) / 
					(dw->coredraw.ymax - dw->coredraw.ymin);
				tm[1][3] = (float)(dw->core.height) *
					(1.0 + dw->coredraw.ymin/
					(dw->coredraw.ymax - dw->coredraw.ymin));

				tm[2][1] = 1.0/(dw->draw.zmax - dw->draw.zmin);
				tm[2][3] = -dw->draw.zmin;
				break;
			case 'z':
			case 'Z':
				for(i = 0; i < 4; i++)
					for(j = 0; j < 4; j++) 
						tm[i][j] = 0;
				tm[0][0] = (float)(dw->core.width) / 
					(dw->coredraw.xmax - dw->coredraw.xmin);
				tm[0][3] = -(float)(dw->core.width) * dw->coredraw.xmin/
					(dw->coredraw.xmax - dw->coredraw.xmin);

				/* because of the flipped axes in XWindows, we need
				** to do some juggling for the Y axis */
				tm[1][1] = -(float)(dw->core.height) / 
					(dw->coredraw.ymax - dw->coredraw.ymin);
				tm[1][3] = (float)(dw->core.height) *
					(1.0 +  dw->coredraw.ymin/
					(dw->coredraw.ymax - dw->coredraw.ymin));

				tm[2][2] = 1.0 / (dw->draw.zmax - dw->draw.zmin);
				tm[2][3] = -dw->draw.zmin;
				break;
			case 'o':
			case 'p':
				XoUnitCross(0.0,0.0,1.0,
					dw->draw.ax,dw->draw.ay,dw->draw.az,
					&tm[0][0],&tm[0][1],&tm[0][2]);
				for(i = 0; i < 3; i++)
					tm[0][i] *= (float)(dw->core.width)/
						(dw->coredraw.xmax - dw->coredraw.xmin);
				tm[0][3] = -(float)(dw->core.width) * dw->coredraw.xmin/
					(dw->coredraw.xmax - dw->coredraw.xmin);

				XoUnitCross(dw->draw.vx,dw->draw.vy,dw->draw.vz,
					tm[0][0],tm[0][1],tm[0][2],
					&tm[1][0],&tm[1][1],&tm[1][2]);
				for(i = 0; i < 3; i++)
					tm[1][i] *= -(float)(dw->core.height)/
						(dw->coredraw.ymax - dw->coredraw.ymin);
				tm[1][3] = (float)(dw->core.height) *
					(1.0 +  dw->coredraw.ymin/
					(dw->coredraw.ymax - dw->coredraw.ymin));

				XoUnitCross(tm[0][0],tm[0][1],tm[0][2],
					tm[1][0],tm[1][1],tm[1][2],
					&tm[2][0],&tm[2][1],&tm[2][2]);
				for(i = 0; i < 3; i++)
					tm[2][i] *= 1.0/
						(dw->draw.zmax - dw->draw.zmin);
				tm[2][3] = -dw->draw.zmin;
				break;
			default:
				return(0);
		}
		dw->draw.perspective = 0;
		if (dw->draw.transform == 'Z' || dw->draw.transform == 'Y' ||
			dw->draw.transform == 'X' || dw->draw.transform == 'p')
			dw->draw.perspective = 1;
	return(1); /* success */
}

static void  SetTransf(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	char	transf;
	DrawWidget	dw = (DrawWidget)w;
	static XoEventInfo	info;

	if (*num_params == 1) {
		transf = params[0][0];
		dw->draw.transform = transf;
	
		if (LoadTransfMatrix(dw) == 1) {/* successful transformation */
			info.event = XoUpdateFields;
			/*
			ret[0] = transf;
			ret[1] = '\0';
			*/
			info.ret = "transform";
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,dw->coredraw.callbacks,
				(XtPointer) &info);
			Resize(w);
			XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		}
	}
}

static void  Zoom(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	float zx;
	float mx,my,mz,dx,dy,dz;
	DrawWidget	dw = (DrawWidget)w;
	static XoEventInfo	info;
	
	if (*num_params == 1) {
		zx = atof(params[0]);
		info.ret = "ymin ymax xmin xmax zmin zmax";
		mx = (dw->coredraw.xmin + dw->coredraw.xmax)/2.0;
		my = (dw->coredraw.ymin + dw->coredraw.ymax)/2.0;
		mz = (dw->draw.zmin + dw->draw.zmax)/2.0;
		dx = (dw->coredraw.xmax - dw->coredraw.xmin)/2.0;
		dy = (dw->coredraw.ymax - dw->coredraw.ymin)/2.0;
		dz = (dw->draw.zmax - dw->draw.zmin)/2.0;
		dw->coredraw.xmin = mx - dx * zx;
		dw->coredraw.ymin = my - dy * zx;
		dw->draw.zmin = mz - dz * zx;
		dw->coredraw.xmax = mx + dx * zx;
		dw->coredraw.ymax = my + dy * zx;
		dw->draw.zmax = my + dz * zx;
		if (LoadTransfMatrix(dw) == 1) {
			info.event = XoUpdateFields;
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,dw->coredraw.callbacks,
				(XtPointer) &info);
			/* Call the field updating code here */
			Resize(w);
			XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		}
	}
}

static void  Pan(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	float px,py;
	float mx,my,dx,dy;
	DrawWidget	dw = (DrawWidget)w;
	static XoEventInfo	info;
	float	*pxmin,*pxmax,*pymin,*pymax;
	
	if (*num_params == 2) {
		px = atof(params[0]);
		py = atof(params[1]);
		switch(dw->draw.transform) {
			case 'x':
				pxmin = &(dw->coredraw.ymin);
				pxmax = &(dw->coredraw.ymax);
				pymin = &(dw->draw.zmin);
				pymax = &(dw->draw.zmax);
				info.ret = "ymin ymax zmin zmax";
				break;
			case 'y':
				pxmin = &(dw->coredraw.xmin);
				pxmax = &(dw->coredraw.xmax);
				pymin = &(dw->draw.zmin);
				pymax = &(dw->draw.zmax);
				info.ret = "xmin xmax zmin zmax";
				break;
			case 'z':
				pxmin = &(dw->coredraw.xmin);
				pxmax = &(dw->coredraw.xmax);
				pymin = &(dw->coredraw.ymin);
				pymax = &(dw->coredraw.ymax);
				info.ret = "ymin ymax xmin xmax";
				break;
			default:
				pxmin = &(dw->coredraw.xmin);
				pxmax = &(dw->coredraw.xmax);
				pymin = &(dw->coredraw.ymin);
				pymax = &(dw->coredraw.ymax);
				info.ret = "ymin ymax xmin xmax";
				break;
		}
		mx = (*pxmin + *pxmax)/2.0;
		my = (*pymin + *pymax)/2.0;
		dx = (*pxmax - *pxmin)/2.0;
		dy = (*pymax - *pymin)/2.0;
		*pxmin = mx - dx +  dx * px;
		*pymin = my - dy +  dy * py;
		*pxmax = mx + dx +  dx * px;
		*pymax = my + dy +  dy * py;
		if (LoadTransfMatrix(dw) == 1) {
			info.event = XoUpdateFields;
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,dw->coredraw.callbacks,
				(XtPointer) &info);
			/* Call the field updating code here */
			Resize(w);
			XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		}
	}
}

static void  Restore(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	DrawWidget dw = (DrawWidget)w;
	static XoEventInfo	info;
		dw->coredraw.xmin = 0;
		dw->coredraw.ymin = 0;
		dw->draw.zmin = 0;
		dw->coredraw.xmax = 1;
		dw->coredraw.ymax = 1;
		dw->draw.zmax = 1;
		if (LoadTransfMatrix(dw) == 1) {
			info.event = XoUpdateFields;
			info.ret = "ymin ymax xmin xmax zmin zmax";
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,dw->coredraw.callbacks,
				(XtPointer) &info);
			/* Call the field updating code here */
			Resize(w);
			XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		}
}

static void  Rotate(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	DrawWidget dw = (DrawWidget)w;
	static XoEventInfo	info;
	float vvec[3];
	float tx,ty,tz;
	float px,py,pz;
	float dphi,dtheta;
	double len;

	if (*num_params == 2) {
		vvec[0] = dw->draw.vx;
		vvec[1] = dw->draw.vy;
		vvec[2] = dw->draw.vz;
		dphi = atof(params[0]);
		dtheta = atof(params[1]);
		len = XoLength(vvec);
		/* get the dtheta vector */
		/* ax is the axis vector ie, the unit projection of vx onto the
		** xy plane. It retains its location when vx flips upside down*/
		XoUnitCross(0.0,0.0,1.0, 
			dw->draw.ax,dw->draw.ay,dw->draw.az,
			&tx,&ty,&tz);
		/* Get the dphi vector */
		XoUnitCross(dw->draw.vx,dw->draw.vy,dw->draw.vz,
			tx,ty,tz,&px,&py,&pz);

			dw->draw.ax += dtheta * tx;
			dw->draw.ay += dtheta * ty;
			dw->draw.az = 0;
		XoVNormalize(&(dw->draw.ax),&(dw->draw.ay),&(dw->draw.az));

		/* increment the position of the viewpoint */
			dw->draw.vx += dphi * px;
			dw->draw.vy += dphi * py;
			dw->draw.vz += dphi * pz;
		XoVNormalize(&(dw->draw.vx),&(dw->draw.vy),&(dw->draw.vz));
			/* find the length of the projection of v along a */
			len = (1.0 - dw->draw.vz * dw->draw.vz);
			if (len > 0)
				len = sqrt(len);
			else 
				len = 0;
			len = copysign(len,dw->draw.vx * dw->draw.ax +
				dw->draw.vy * dw->draw.ay);
			dw->draw.vx = dw->draw.ax * len;
			dw->draw.vy = dw->draw.ay * len;

		if (LoadTransfMatrix(dw) == 1) {
			info.event = XoUpdateFields;
			info.ret = "vx vy vz";
			info.x=info.y=info.z=0.0;
			XtCallCallbackList(w,dw->coredraw.callbacks,
				(XtPointer) &info);
			/* Call the field updating code here */
			Resize(w);
			XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
		}
	}
}

/* Change perspective viewing distance */
static void  PZoom(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	DrawWidget dw = (DrawWidget)w;
	static XoEventInfo	info;
	float zx;

	if (*num_params == 1) {
		zx = atof(params[0]);
		if (zx > 0) {
			dw->draw.rv *= zx;
			if (LoadTransfMatrix(dw) == 1) {
				info.event = XoUpdateFields;
				info.ret = "rv";
				info.x=info.y=info.z=0.0;
				XtCallCallbackList(w,dw->coredraw.callbacks,
					(XtPointer) &info);
				/* Call the field updating code here */
				Resize(w);
				XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
			}
		}
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

static void TransformPoint(w,x,y,z,sx,sy,sz)
	Widget w;
	float x,y,z;
	int	*sx,*sy,*sz;
{
	DrawWidget	dw = (DrawWidget)w;
	float **tm = dw->draw.matrix;
	/* Straightforward: we just do the transforms that are set by
	** the matrix */
	float tx,ty,tz;
	float rv; /* distance of viewpoint from center */
	float iz;
	float cx,cy;

	tx=	tm[0][0] * x + 
		tm[0][1] * y + 
		tm[0][2] * z + 
		tm[0][3];

	ty=	tm[1][0] * x + 
		tm[1][1] * y + 
		tm[1][2] * z + 
		tm[1][3];

	tz=	tm[2][0] * x + 
		tm[2][1] * y + 
		tm[2][2] * z + 
		tm[2][3];

	/* Perspectives involve a little juggling */
	if (dw->draw.perspective) {
		cx = (float)dw->core.width / 2.0;
		cy = (float)dw->core.height / 2.0;
		rv = dw->draw.rv ;
	/* Do not worry about cutoffs here. That is the job of the
	** expose routines */
		iz = fabs(rv + dw->draw.rz - tz);
		if (iz > 0.01 * rv) { /* to restrict magnification to 100x */
			iz = rv/iz;
			tx = (tx - cx) * iz + cx;
			ty = (ty - cy) * iz + cy;
		} else {
			tx = ty = 0;
		}
	}
	*sx = (int)tx;
	*sy = (int)ty;
	*sz = (int)tz;
}

static void TransformPoints(w,x,y,z,sx,sy,sz,n)
	Widget w;
	double *x,*y,*z;
	int	*sx,*sy,*sz;
	int n;
{
	DrawWidget	dw = (DrawWidget)w;
	float **tm = dw->draw.matrix;
	int i;
	float rv,iz;
	float rvby100;
	float tx,ty,tz;
	float rsum;
	float cx,cy;

	/* Straightforward: we just do the transforms that are set by
	** the matrix */

	/* Perspectives involve a little juggling */
	if (dw->draw.perspective) {
		rv = dw->draw.rv ;
		rvby100 = rv/100.0;
		rsum = rv + dw->draw.rz;
		cx = (float)dw->core.width / 2.0;
		cy = (float)dw->core.height / 2.0;
		for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
			tx =
			tm[0][0] * *x +
			tm[0][1] * *y +
			tm[0][2] * *z + 
			tm[0][3];
	
			ty =
			tm[1][0] * *x + 
			tm[1][1] * *y + 
			tm[1][2] * *z + 
			tm[1][3];
	
			tz = 
			tm[2][0] * *x + 
			tm[2][1] * *y + 
			tm[2][2] * *z + 
			tm[2][3];

			iz = fabs(rsum - tz);
			if (iz > rvby100) { /* to restrict magnification to 100x */
				iz = rv/iz;
				tx = (tx - cx) * iz + cx;
				ty = (ty - cy) * iz + cy;
			}
			*sx = (int)tx;
			*sy = (int)ty;
			*sz = (int)tz;
		}
	} else {
		for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
			*sx = (int)(
				tm[0][0] * *x + 
				tm[0][1] * *y + 
				tm[0][2] * *z + 
				tm[0][3]);
		
			*sy = (int)(
				tm[1][0] * *x + 
				tm[1][1] * *y + 
				tm[1][2] * *z + 
				tm[1][3]);
		
			*sz = (int)(
				tm[2][0] * *x + 
				tm[2][1] * *y + 
				tm[2][2] * *z + 
				tm[2][3]);
		}
	}
}

/* This uses the estimate of the z coord passed in the z argument */
static void BackTransform(w,sx,sy,x,y,z)
	Widget w;
	int	sx,sy;
	float *x,*y,*z;
{
	DrawWidget	dw = (DrawWidget)w;
	float **tm = dw->draw.matrix;
	/* We require that *z have a sensible value for use in
	** back-transforms. Usually this comes from the cz field 
	** of the pix, otherwise it should be zero */
	float tx=sx;
	float ty=sy;
	float tz = *z;
	float cx,cy;
	float iz,rv;
	float rx,ry;
	float eig;

	if (dw->draw.perspective) {
		/* unperspectivize the point if possible */
		cx = (float)dw->core.width / 2.0;
		cy = (float)dw->core.height / 2.0;
		rv = dw->draw.rv ;

		iz = fabs(rv + dw->draw.rz - tz);

		if (iz > 0.01 * rv) { /* to restrict magnification to 100x */
			iz = iz/rv;
			tx = (tx - cx) * iz + cx;
			ty = (ty - cy) * iz + cy;
		} else {
			tx = ty = 0;
		}
	}
	/* transform back the offsets */
	/*
	tx -= tm[0][3];
	ty -= tm[1][3];
	tz -= tm[2][3];
	*/
	/* Do the rotations and stretchings */
	eig = tm[0][0] * tm[1][1] - tm[0][1] * tm[1][0];
	if (eig != 0) {
		rx = tx - tm[0][3] - tz * tm[0][2];
		ry = ty - tm[1][3] - tz * tm[1][2];
		*x = (tm[1][1] * rx - tm[0][1] * ry) / eig;
		/* oops, sign flip here
		*y = (tm[1][0] * rx - tm[0][0] * ry) / eig;
		*/
		*y = (tm[0][0] * ry - tm[1][0] * rx) / eig;
	} else if (dw->draw.transform == 'x' || dw->draw.transform == 'X') {
		*x = tz;
		*y = (tx - tm[0][3])/tm[0][1];
		*z = (ty - tm[1][3])/tm[1][2];
	} else if (dw->draw.transform == 'y' || dw->draw.transform == 'Y') {
		*x = (tx - tm[0][3])/tm[0][0];
		*y = tz;
		*z = (ty - tm[1][3])/tm[1][2];
	} else {
		*x = 0.0;
		*y = 0.0;
	}
}


static Boolean XoCvtStringToTransform(dpy,args,num_args,fromVal,toVal,
	destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
	static char ret;

	if (fromVal->addr != NULL) {
		ret = fromVal->addr[0];
		if (ret == 'x' || ret == 'y' || ret == 'z' ||
			ret == 'X' || ret == 'Y' || ret == 'Z' ||
			ret == 'o' || ret == 'p'
			) {
			XoCvtDone(char,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRTransform");
		return((Boolean) False);
}


static Boolean XoCvtTransformToString(dpy,args,num_args,fromVal,toVal,
	destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
	static char ret;

	if (fromVal->addr != NULL) {
		ret = *fromVal->addr;
		if (ret == 'x' || ret == 'y' || ret == 'z' ||
			ret == 'X' || ret == 'Y' || ret == 'Z' ||
			ret == 'o' || ret == 'p'
			) {
		 if(toVal->addr != NULL){
		   if(toVal->size < 2){
			toVal->size = 2;
			return((Boolean)False);
		   }
		   else{
			toVal->addr[0] = ret;
			toVal->addr[1] = '\0';
		   }
		 }
		 else {
		 	static char staticval[2];
			staticval[0] = ret;
			staticval[1] = '\0';
			toVal->addr = (XtPointer) staticval;
		 }
			toVal->size = 2;
			return((Boolean) True);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRString");
		return((Boolean) False);
}


void XoDrawForceExpose(w)

Widget	w;

{

	DrawWidget	dw;

	dw = (DrawWidget) w;

	/* check and make sure w is a draw widget!!! */



	/* allow only one expose event per call */
	if (!(dw->coredraw.drawflags&XO_DRAW_FORCE_EXPOSE)) {
	    XClearArea(XtDisplay(dw), XtWindow(dw), 0, 0, 0, 0, True);
	    dw->coredraw.drawflags |= XO_DRAW_FORCE_EXPOSE;
	}
}
