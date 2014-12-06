/* $Id: CoreDraw.c,v 1.2 2005/07/29 16:00:14 svitak Exp $ */
/*
 * $Log: CoreDraw.c,v $
 * Revision 1.2  2005/07/29 16:00:14  svitak
 * Removed preprocessor logic for GCC 2.6.3 and OPTIMIZE. If still needed
 * it should be handled in some central include file.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.20  2000/07/03 18:14:16  mhucka
 * Added missing type casts.
 *
 * Revision 1.19  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.18  2000/05/02 06:06:41  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.17  1998/07/15 06:17:26  dhb
 * Upi update
 *
 * Revision 1.16  1997/07/25 00:41:38  venkat
 * Fixed bug in XoCoreDrawLeave() which was failing to initialize the pix
 * local Widget variable before comparing it with the Coredraw widget and
 * invoking its callbacks.
 *
 * Revision 1.15  1996/10/29  00:31:23  venkat
 * Removed-parameter-type-from-declaration-of-atof-for-linux,-which-was-left-there-unintentionally-in-the-previous-check-in
 *
 * Revision 1.14  1996/10/25  19:49:19  venkat
 * Removed-ansi-compliant-function-prototypes-from-atof-declaration-and-shuffled-preprocessor-conditionals-to-include-stdlib.h-for-compilation-under-Linux/FreeBSD-with-or-without-optimization
 *
 * Revision 1.13  1996/10/07  22:18:32  dhb
 * Added explicit definition for atof() return values; some Linux/FreeBSD
 * machines define an atof() macro to use strtod() which doesn't get
 * defined as returning double.
 *
 * Revision 1.12.1.2  1996/09/13  23:59:29  venkat
 * Cleaned-up-the-preprocessor-statements-for-Linux-stdlib.h-include
 *
 * Revision 1.12.1.1  1996/08/21  21:10:16  venkat
 * Explicit declaration of atof() for Linux
 *
 * Revision 1.12  1995/07/28  23:35:17  dhb
 * Undefine __OPTIMIZE__ macro for gcc which causes incompatible
 * redefinition of functions in stdlib.h.
 *
 * Revision 1.11  1995/07/08  00:43:28  venkat
 * Changed some float resource default strings to reflect float values.
 *
 * Revision 1.10  1995/06/16  05:53:57  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.9  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.8  1995/03/17  20:29:52  venkat
 * Included stdlib.h for atof() usage in Linux
 * Resize() changed to call Project() of PixObject
 * children based on being_destroyed flag.
 *
 * Revision 1.7  1995/03/07  00:42:41  venkat
 * Changed code in Class record initialization to
 * subclass directly from XoComposite.
 *
 * Revision 1.6  1994/05/26  13:44:19  bhalla
 * Added utility function ResizeDraws for calling the Resize action
 *
 * Revision 1.5  1994/05/25  13:46:40  bhalla
 * in SetValues: forces resize now.
 *
 * Revision 1.4  1994/04/07  13:29:49  bhalla
 * Fixed code relating to selection in drops (enter event)
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * General enhancement of selection mechanism (Select and Unselect funcs)
 * to allow subparts of gadgets to be selected and dragged between
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
#include "CoreDrawP.h"
#include "PixP.h"


/*
** This is the coredraw widget. It doesnt do much good on its own,
** but has the basic fields and methods for the draw subclasses
** include CoreDraw, Graph, and Draw.
*/

static XtResource resources[] = {
#define offset(field) XtOffset(CoreDrawWidget, coredraw.field)
  {XtNxmin, XtCPosition, XtRFloat,	sizeof(float),
	 offset(xmin), XtRString, "0.00"},
  {XtNymin, XtCPosition, XtRFloat,	sizeof(float),
	 offset(ymin), XtRString, "0.00"},
  {XtNxmax, XtCPosition, XtRFloat,	sizeof(float),
	 offset(xmax), XtRString, "1.00"},
  {XtNymax, XtCPosition, XtRFloat,	sizeof(float),
	 offset(ymax), XtRString, "1.00"},
  {XtNdrawflags, XtCDrawflags, XtRInt,	sizeof(int),
	 offset(drawflags), XtRString, "0"},
  {XtNcallback, XtCCallback, XtRCallback,	sizeof(XtPointer),
	 offset(callbacks), XtRCallback, (XtPointer) NULL},
#undef offset
};

/* 
** The main mouse callback operation should be selection of pixes
** The main keyboard callback operation should be zooming and panning
*/
static void XoRegisterFloatToStringConverter();
static Boolean XoCvtFloatToString();
static void ClassInitialize();
static void Realize();
static void XoCoreDrawSelectPix();
static void XoCoreDrawUnSelectPix();
static void XoCoreDrawEnter();
static void XoCoreDrawLeave();
Widget XoFindNearestPix();
static void SelectPix();
static void UnSelectPix();
static void  Zoom(), Pan(), Restore();
/* arguments are : x,y (fractions of screen, relative) */
static void Print();
static void
	Initialize(),Destroy(), Resize();
static void Redisplay();
static void TransformPoint(),TransformPoints(),BackTransform();
static Boolean SetValues();
static void ClassPartInitialize();
static void ChangeManaged();
static void InsertChild();
static XtGeometryResult GeometryManager();
/*
Here we need :
* click for selection of  a particular pix
* Key events for these, for printing, for undoing transformations.
*/

CompositeClassExtensionRec extension_rec = {
	/* next extension */	NULL,
	/* record_type */	NULLQUARK,
	/* version */		XtCompositeExtensionVersion,
	/* record_size */	sizeof(CompositeClassExtensionRec),
	/* accepts_objects */	TRUE,
};

static XtActionsRec actions[] =
{
  {"pan",	 Pan},
  {"zoom", Zoom},
  {"restore", Restore},
  {"print", Print},
  {"select", XoCoreDrawSelectPix},
  {"unselect", XoCoreDrawUnSelectPix},
  {"enter", XoCoreDrawEnter},
  {"leave", XoCoreDrawLeave},
};

static char translations[] =
" <Key>Left:	pan(0.1,0)	\n\
  <Key>Right:      pan(-0.10,0)	\n\
  <Key>Down:       pan(0,0.10)       \n\
  <Key>Up:   pan(0,-0.10)      \n\
  <Key><:	zoom(1.1,1.1)	\n\
  <Key>>:	zoom(0.9,0.9)	\n\
  <Key>u:	restore()	\n\
  <Key>P:	print()		\n\
  <BtnDown> : select()	\n\
  <BtnUp> : unselect()	\n\
  <Enter> : enter()	\n\
  <Leave> :	leave()	\n\
";

CoreDrawClassRec coredrawClassRec = {
  { /* core fields */
  /* superclass		*/	(WidgetClass) &xocompositeClassRec,
    /* class_name		*/	"CoreDraw",
    /* widget_size		*/	sizeof(CoreDrawRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
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
    /* change_managed     */    ChangeManaged,
    /* insert_child       */    InsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    NULL
  },
  { /* xocomposite_class fields */
    /* empty */			0
  },
  { /* coredraw_class fields */
    /* transform_point		*/	TransformPoint,
    /* transform_points		*/	TransformPoints,
    /* backtransform		*/	BackTransform,
    /* selectpix			*/	SelectPix,
    /* unselectpix			*/	UnSelectPix,
    /* extension			*/	NULL
  }
};

WidgetClass coredrawWidgetClass = (WidgetClass)&coredrawClassRec;

static void ClassInitialize()
{
	XoRegisterFloatToStringConverter();
}

/* ARGUSED */
static void Initialize (req, new,args, num_args)
     Widget req, new;
	 ArgList	args;
	 Cardinal	*num_args;
{
	CoreDrawWidget dw = (CoreDrawWidget) new;
	/* This is a hack. I need to figure out a way of doing it
	** properly */
	if (dw->core.width == 0)
		dw->core.width = 20;
	if (dw->core.height == 0)
		dw->core.height = 20;
	(*XtClass(new)->core_class.resize)((Widget)dw);
	dw->coredraw.selected = NULL;
	dw->coredraw.pix_selected = -1;
}

static void Realize(w,value_mask,attributes)
     Widget w;
	 XtValueMask	*value_mask;
	 XSetWindowAttributes	*attributes;
{
	Cursor	XoDefaultCursor();

	attributes->cursor = XoDefaultCursor();
	*value_mask |= CWCursor; /* this lets me change the cursor */
	XtCreateWindow(w,CopyFromParent,CopyFromParent,*value_mask,
		attributes);
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
	CoreDrawWidget dcw = (CoreDrawWidget) curr;
	CoreDrawWidget drw = (CoreDrawWidget) req;
	CoreDrawWidget dnw = (CoreDrawWidget) new;
	/* Check if field is interesting */
	if ( 
		!(drw->coredraw.ymin != dcw->coredraw.ymin || 
		drw->coredraw.ymax != dcw->coredraw.ymax || 
		drw->coredraw.xmin != dcw->coredraw.xmin || 
		drw->coredraw.xmax != dcw->coredraw.xmax || 
		drw->xocomposite.bg != dcw->xocomposite.bg))
			 return(False);
	 if (drw->coredraw.xmin > drw->coredraw.xmax) {
		 dnw->coredraw.xmin = dcw->coredraw.xmin;
		dnw->coredraw.xmax = dcw->coredraw.xmax;
		return(False);
	}
	if (drw->coredraw.ymin > drw->coredraw.ymax) {
		dnw->coredraw.ymin = dcw->coredraw.ymin;
		dnw->coredraw.ymax = dcw->coredraw.ymax;
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
	/* need to resize all the child pixes, by calling their Project
	** methods */
  CoreDrawWidget dw = (CoreDrawWidget) w;
 Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;
  PixObjectClass class;
  /* static XoEventInfo    info; */

	if (!XtIsRealized((Widget) dw))
		return;

	/* handle children */
  	for (childP = first_born;
		childP - first_born < num_children; childP++) {
		if (((PixObject)*childP)->object.being_destroyed)
                        continue;
		class =
			(PixObjectClass) ((PixObject)*childP)->object.widget_class;
     	(class->pix_class.project)(*childP);

	}
/*
	info.event = XoUpdateFields;
    info.ret="";
    info.x= info.y= info.z =0;
	XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer)&info);
*/
	/* expose happens automatically here */
}

static void Redisplay (w, e, region)
     Widget w;
     XEvent *e;
     Region region;
{
  CoreDrawWidget dw = (CoreDrawWidget) w;
 Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;

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
	   != RectangleOut))
      (*(XtClass(*childP))->core_class.expose)((*childP), e, region);
	/* Handle axes */
  }
}

static void ChangeManaged(w)
	Widget w;
{
	/* the coredraw widget leaves the individual gadget settings
	alone */
}

static XtGeometryResult GeometryManager(w,request,reply)
	Widget w;
	XtWidgetGeometry	*request;
	XtWidgetGeometry	*reply;	/* RETURN */
{
	return XtGeometryYes;
}

#ifdef FANCY
#endif /* FANCY */
/* This may be useful for extra error checking if we really want
** to be sure that pixes are the only things put in a draw
** widget, but for now we will inherit it
*/
static void InsertChild(w)
	Widget w;
{
	CoreDrawWidget dw = (CoreDrawWidget) w->core.parent;
	PixObjectClass class =
			(PixObjectClass) ((PixObject)w)->object.widget_class;

	/* realloc space for the children every 10th time */
	if (dw->composite.num_children == 0)
		dw->composite.children = NULL;
	if (dw->composite.num_children%10 == 0)
		dw->composite.children =
			(Widget *)XtRealloc((char *) dw->composite.children,
			(10 + 10 * (dw->composite.num_children/10)) *
			sizeof(Widget));

	/* I should really do widget checking here ..
	if (XoSubclassIsSubclass(class,&pixClassRec)) {
	*/
		dw->composite.children[dw->composite.num_children] = w;
		dw->composite.num_children++;
     	(class->pix_class.project)(w);
	/*
	}
	*/
}


static void Destroy(w)
	Widget w;
{
	CoreDrawWidget dw=(CoreDrawWidget)w;
}

static void ClassPartInitialize(wc)
	WidgetClass wc;
{
	CoreDrawWidgetClass ddwc = (CoreDrawWidgetClass) wc;

	extension_rec.next_extension = ddwc->composite_class.extension;
	ddwc->composite_class.extension = (XtPointer) &extension_rec;
	if (ddwc->coredraw_class.transformpt == XoInheritTransformPoint)
		ddwc->coredraw_class.transformpt =TransformPoint;
	if (ddwc->coredraw_class.transformpts == XoInheritTransformPoints)
		ddwc->coredraw_class.transformpts =TransformPoints;
	if (ddwc->coredraw_class.backtransform == XoInheritBackTransform)
		ddwc->coredraw_class.backtransform =BackTransform;
	if (ddwc->coredraw_class.selectpix == XoInheritSelectPix)
		ddwc->coredraw_class.selectpix = SelectPix;
	if (ddwc->coredraw_class.unselectpix == XoInheritUnSelectPix)
		ddwc->coredraw_class.unselectpix = UnSelectPix;
	if (ddwc->core_class.realize == XtInheritRealize)
		ddwc->core_class.realize = Realize;
}



Widget XoFindNearestPix(w,event)
	Widget	w;
	XButtonEvent	*event;
{
  CoreDrawWidget dw = (CoreDrawWidget) w;
  PixObject *childP, *first_born = (PixObject *)dw->composite.children;
  PixObject nearestP = (PixObject ) NULL;
  PixObjectClass class;
  int num_children = dw->composite.num_children;
  int min_dist = XO_MAX_SEL_DIST;
  Dimension dist=0;
  int x = event->x;	/* the events are dimensions, but we need to */
  int y = event->y;	/* compare with ints, so we convert here */
  int px,py,pw,ph;

	/* Check if the event was within the draw at all */
	
	if (dw->core.x > event->x ||
		dw->core.x + (int)dw->core.width < event->x ||
		dw->core.y > event->y ||
		dw->core.y + (int)dw->core.height < event->y) {
		return(NULL);
	}
	

  for (childP = first_born;
	(childP - first_born) < num_children; childP++) {
		px = (*childP)->pix.x;
		py = (*childP)->pix.y;
		pw = (*childP)->pix.w;
		ph = (*childP)->pix.h;
  	if (x > px && y > py && x < px + pw && y < py + ph) {
			class = (PixObjectClass) (*childP)->object.widget_class;
			(class->pix_class.select_distance)(
				(Widget) *childP, event->x,event->y,&dist);
			/* This cannot happen since dist is a dimension
			if (dist < 0)
				(class->pix_class.select)((Widget) *childP,event);
			*/
			if ((int)dist < min_dist) {
				min_dist=dist;
				nearestP = *childP;
			}
		}
  }
  if (nearestP != NULL &&
	min_dist < XO_MAX_SEL_DIST && min_dist >= 0) {
		return((Widget)nearestP);
	} else { /* do the draw's own event */
		return(w);
	}
}

/* Note that the pixes figure out the dist in real coords by using
** the back-transform of the parent
** Actually the pixes have already found their little patch of 2-d
** space and then go ahead and do the back transform if they want to
*/
static void XoCoreDrawSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	SelectPix(w,event);
}

static void XoCoreDrawUnSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	UnSelectPix(w,event);
}

static void UnSelectPix(w,event)
	Widget	w;
	XButtonEvent	*event;
{
	Widget	pix,pix2;
	PixObjectClass class;
	CoreDrawWidget dw = (CoreDrawWidget) w;
  	static XoEventInfo    info;
	CoreDrawWidgetClass dclass = (CoreDrawWidgetClass)XtClass(w);

	/* find the nearest pix to the release point */
	pix2 = XoFindNearestPix(w,event);
	pix = dw->coredraw.selected;
	/* Check if the release point is on a different pix */
	if (pix != NULL && (pix != pix2 ||
		(pix == pix2 && pix != w &&
		dw->coredraw.pix_selected != ((PixObject)pix)->pix.selected))) {
		/* Call the callbacks for a possible drag-drop operation 
		** within the draw widget */
		info.event = XoDrag;
		/* Set up the ret value for drag-drops, specially if it is
		** within the same pix */
		if (dw->coredraw.pix_selected >= 0) /* check if it is valid */
    		info.ret=(char *)(&(dw->coredraw.pix_selected));
		else 
    		info.ret="";
		info.key = event->time;
		info.z = (pix2 != NULL && pix2 != w) ?
			((PixObject)pix2)->pix.cz : 0.0;
		(dclass->coredraw_class.backtransform)(
			w,event->x,event->y,
			&(info.x),&(info.y),&(info.z));
		/* Calling the drag callback */
		if (pix == w)
  			XtCallCallbackList(w,dw->coredraw.callbacks,
				(XtPointer)&info);
		else
  			XtCallCallbackList(pix,((PixObject)pix)->pix.callbacks,
				(XtPointer)&info);
		/* Calling the drop callback */
		if (pix2 != NULL) {
			info.event = XoDrop;
			if (pix2 == w) {
				info.ret = "";
  				XtCallCallbackList(w,dw->coredraw.callbacks,
					(XtPointer)&info);
			} else {
		/* Set up the ret value for drag-drops, specially if it is
		** within the same pix */
    			if (((PixObject)pix2)->pix.selected >= 0)
    				info.ret=(char *)(&((PixObject)pix2)->pix.selected);
				else
					info.ret = "";
  				XtCallCallbackList(pix2,
					((PixObject)pix2)->pix.callbacks,
					(XtPointer)&info);
			}
		}
	}

	/* Finish up by unselecting the original pix */
	pix = dw->coredraw.selected;
	if (pix != NULL && pix != w) {
	/* Invoke the pix UnSelect option */
		class = (PixObjectClass)((PixObject)pix)->object.widget_class;
		(class->pix_class.unselect)(pix, event);
	}
	/* Make sure the event actually happens */
	XFlush(XtDisplay(w));
	/* Get rid of the previously selected widget or pix */
	dw->coredraw.selected = (Widget)NULL;
	dw->coredraw.pix_selected = -1;
}

static void SelectPix(w,event)
	Widget	w;
	XButtonEvent	*event;
{
  CoreDrawWidget dw = (CoreDrawWidget) w;
  PixObject nearestP;
  PixObjectClass class;
  static XoEventInfo	info;
  int		nclicks = 1;
  CoreDrawWidgetClass dclass = (CoreDrawWidgetClass)XtClass(w);

  nearestP = (PixObject)XoFindNearestPix(w,event);
  if (nearestP != NULL) {
	/* save the selected pix to speed up motion events */
	dw->coredraw.selected = (Widget)nearestP;
  	if (((Widget)nearestP) != w) /* it is a pix event */
		dw->coredraw.pix_selected = nearestP->pix.selected;
	else
		dw->coredraw.pix_selected = -1;
	/* keep track of the time to check if it is a double-click.
	** For some reason the intrinsics cannot handle this case
	** properly */
	if (event->time - info.key < XO_DOUBLE_CLICK_TIME)
		nclicks = 2;
	else
		info.key = event->time;
	/* Fill out the info stuff for the draw. The pix can use or
	** modify this as it pleases should the event be for a pix */
 	info.event  = 0;
 	switch(event->button) {
     	case Button1: info.event |= XoBut1 ;
       		break;
      	case Button2: info.event |= XoBut2 ;
       		break;
      	case Button3: info.event |= XoBut3 ;
        	break;
 	}
	/*
	if (event->state & ShiftMask)
		info.event |= XoShift;
	else if (event->state & ControlMask)
		info.event |= XoControl;
	*/
	if (nclicks == 1)
		info.event |= XoPress; /* single click event */
	else
		info.event |= XoDouble; /* double click event */
   	info.ret="";
  	info.z = (((Widget)nearestP) == w) ?  0.0 : nearestP->pix.cz;
	(dclass->coredraw_class.backtransform)(
		w,event->x,event->y,
		&(info.x),&(info.y),&(info.z));

  	if (((Widget)nearestP) != w) { /* it is a pix event */
		class = (PixObjectClass) (nearestP)->object.widget_class;
		(class->pix_class.select)((Widget) nearestP,event,&info);
		XFlush(XtDisplay(w));
	} else { /* do the draw's own event */
  		XtCallCallbackList(w,dw->coredraw.callbacks, (XtPointer)&info);
	}
  }
}

static void XoCoreDrawEnter(w,event)
	Widget	w;
	XButtonEvent	*event;
{
  CoreDrawWidget dw = (CoreDrawWidget) w;
  Widget pix;
  static XoEventInfo	info;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(w);

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
    			info.ret=(char *)(&((PixObject)pix)->pix.selected);
			dw->coredraw.pix_selected = ((PixObject)pix)->pix.selected;
  			XtCallCallbackList(pix,((PixObject)pix)->pix.callbacks,
				(XtPointer)&info);
		}
  }
}

static void XoCoreDrawLeave(w,event)
	Widget	w;
	XButtonEvent	*event;
{
  CoreDrawWidget dw = (CoreDrawWidget) w;
  Widget pix;
  static XoEventInfo	info;

	pix = XoFindNearestPix(w, event);
	/* Call the callbacks for a possible drag-drop operation 
	** within the draw widget */
	info.event = XoLeave;
   	info.ret="";
   	info.x=event->x;
   	info.y=event->y;
   	info.z = 0;
	/* Calling the drag callback */
	if (pix == w)
  		XtCallCallbackList(w,dw->coredraw.callbacks,
			(XtPointer)&info);
}

static void  Zoom(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	double zx,zy;
	float mx,my,dx,dy;
	CoreDrawWidget	dw = (CoreDrawWidget)w;
	static XoEventInfo	info;
	
	if (*num_params == 2) {
		zx = atof(params[0]);
		zy = atof(params[1]);
		mx = (dw->coredraw.xmin + dw->coredraw.xmax)/2.0;
		my = (dw->coredraw.ymin + dw->coredraw.ymax)/2.0;
		dx = (dw->coredraw.xmax - dw->coredraw.xmin)/2.0;
		dy = (dw->coredraw.ymax - dw->coredraw.ymin)/2.0;
		dw->coredraw.xmin = mx - dx * zx;
		dw->coredraw.ymin = my - dy * zy;
		dw->coredraw.xmax = mx + dx * zx;
		dw->coredraw.ymax = my + dy * zy;
		info.event = XoUpdateFields;
		info.ret = "xmin xmax ymin ymax";
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
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
	CoreDrawWidget	dw = (CoreDrawWidget)w;
	static XoEventInfo	info;
	
	if (*num_params == 2) {
		px = atof(params[0]);
		py = atof(params[1]);
		mx = (dw->coredraw.xmin + dw->coredraw.xmax)/2.0;
		my = (dw->coredraw.ymin + dw->coredraw.ymax)/2.0;
		dx = (dw->coredraw.xmax - dw->coredraw.xmin)/2.0;
		dy = (dw->coredraw.ymax - dw->coredraw.ymin)/2.0;
		dw->coredraw.xmin = mx - dx +  dx * px;
		dw->coredraw.ymin = my - dy +  dy * py;
		dw->coredraw.xmax = mx + dx +  dx * px;
		dw->coredraw.ymax = my + dy +  dy * py;
		info.event = XoUpdateFields;
		info.ret = "xmin xmax ymin ymax";
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
		/* Call the field updating code here */
		Resize(w);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
	}
}

static void  Restore(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	CoreDrawWidget dw = (CoreDrawWidget)w;
	static XoEventInfo	info;
		dw->coredraw.xmin = 0;
		dw->coredraw.ymin = 0;
		dw->coredraw.xmax = 1;
		dw->coredraw.ymax = 1;
		info.event = XoUpdateFields;
		info.ret = "xmin xmax ymin ymax";
		info.x=info.y=info.z=0.0;
		XtCallCallbackList(w,dw->coredraw.callbacks,(XtPointer) &info);
		/* Call the field updating code here */
		Resize(w);
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
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
	CoreDrawWidget	dw = (CoreDrawWidget)w;

	*sx=(int)((float)dw->core.width *
	 (x-dw->coredraw.xmin)/
	(dw->coredraw.xmax - dw->coredraw.xmin));
	*sy=(int)((float)dw->core.height * (1.0 -
	 (y-dw->coredraw.ymin)/
	(dw->coredraw.ymax - dw->coredraw.ymin)));
	*sz = (int)z; /* Note that we might lose info here */
}

static void TransformPoints(w,x,y,z,sx,sy,sz,n)
	Widget w;
	double *x,*y,*z;
	int	*sx,*sy,*sz;
	int n;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;
	int	i;
	float dx,dy;

	dx=dw->coredraw.xmax-dw->coredraw.xmin;
	dy=dw->coredraw.ymax-dw->coredraw.ymin;
	for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
		*sx=(int)((float)dw->core.width *
	 	(*x-dw->coredraw.xmin)/dx);
		*sy=(int)((float)dw->core.height * (1.0 -
	 	(*y-dw->coredraw.ymin)/dy));
		*sz = (int)*z; /* Note that we might lose info here */
	}
}


static void BackTransform(w,sx,sy,x,y,z)
	Widget w;
	int	sx,sy;
	float *x,*y,*z;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;

			*x =(float)sx/(float)(dw->core.width) * 
			(dw->coredraw.xmax - dw->coredraw.xmin) +
			dw->coredraw.xmin;
			*y =(1.0 - (float)sy/(float)(dw->core.height)) * 
			(dw->coredraw.ymax - dw->coredraw.ymin) +
			dw->coredraw.ymin;
}

static Boolean XoCvtFloatToString(dpy,args,num_args,fromVal,toVal,
	destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
   static char staticval[50];
   sprintf(staticval,"%g", *(float *)fromVal->addr);
   if(toVal->addr != NULL){
	if (toVal->size < strlen(staticval)+1) {
		toVal->size = strlen(staticval)+1;
		return((Boolean) False);
	} else {
	 	strcpy(toVal->addr, staticval);
	}
   }
   else{
	toVal->addr = (XtPointer) staticval;
   }
   toVal->size = strlen(staticval)+1;
   return((Boolean)True);
}

static void XoRegisterFloatToStringConverter ()
{
  XtSetTypeConverter (XtRFloat, XtRString,
                      (XtTypeConverter) XoCvtFloatToString,
		      NULL, 0,
                      XtCacheNone, NULL);
}

/* These additional transforms are used for doing direct
** pixel mapping of coords */
void PixelTransformPoint(w,x,y,z,sx,sy,sz)
	Widget w;
	float x,y,z;
	int	*sx,*sy,*sz;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;

	*sx= (int)x;
	*sy= dw->core.height - (int)y;
	*sz= (int)z;
}

void PixelTransformPoints(w,x,y,z,sx,sy,sz,n)
	Widget w;
	double *x,*y,*z;
	int	*sx,*sy,*sz;
	int n;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;
	int	i;

	for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
		*sx=(int)*x;
		*sy=(int)(dw->core.height - *y);
		*sz = (int)*z; /* Note that we might lose info here */
	}
}


void PixelBackTransform(w,sx,sy,x,y,z)
	Widget w;
	int	sx,sy;
	float *x,*y,*z;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;

	*x =(float)sx;
	*y =(float)((int)dw->core.height - sy);
}

/* These additional transforms are used for doing coords in terms
** of percent of screen size */
void PctTransformPoint(w,x,y,z,sx,sy,sz)
	Widget w;
	float x,y,z;
	int	*sx,*sy,*sz;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;

	*sx= (int)(x * (float)dw->core.width/100.0);
	*sy= dw->core.height - (int)(y * (float)dw->core.height/100.0);
	*sz= (int)z;
}

void PctTransformPoints(w,x,y,z,sx,sy,sz,n)
	Widget w;
	double *x,*y,*z;
	int	*sx,*sy,*sz;
	int n;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;
	int	i;

	for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
		*sx=(int)(*x * (float)dw->core.width/100.0);
		*sy=(int)((float)dw->core.height -
			*y * (float)dw->core.width/100.0);
		*sz = (int)*z; /* Note that we might lose info here */
	}
}

void PctBackTransform(w,sx,sy,x,y,z)
	Widget w;
	int	sx,sy;
	float *x,*y,*z;
{
	CoreDrawWidget	dw = (CoreDrawWidget)w;

	*x =(float)sx * 100.0/(float)dw->core.width;
	*y =(float)(((float)dw->core.height - sy) * 100.0 /
		(float)dw->core.height);
}

/* Utility function for forcing reprojection of any draw class
** and all pixes */
void ResizeDraws(w)
	Widget w;
{
	(*XtClass(w)->core_class.resize)(w);
}
