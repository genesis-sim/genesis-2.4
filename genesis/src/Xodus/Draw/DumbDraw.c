/* $Id: DumbDraw.c,v 1.4 2005/07/25 19:17:14 svitak Exp $ */
/*
 * $Log: DumbDraw.c,v $
 * Revision 1.4  2005/07/25 19:17:14  svitak
 * Fixed bug in TransformPoints where *sz was getting assigned a pointer
 * instead of a value.
 *
 * Revision 1.3  2005/07/20 19:37:55  svitak
 * Removed pre-processor logic for GNUC 2.6.3 OPTIMIZE issue. Comment was added
 * to README regarding this issue.
 *
 * math.h and stdio.h added when needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.12  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.11  2000/05/02 06:06:41  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.10  1998/07/15 06:17:26  dhb
 * Upi update
 *
 * Revision 1.9  1995/07/28 23:35:17  dhb
 * Undefine __OPTIMIZE__ macro for gcc which causes incompatible
 * redefinition of functions in stdlib.h.
 *
 * Revision 1.8  1995/06/16  05:53:57  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.7  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.6  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.5  1995/03/17  20:34:38  venkat
 * Include stdlib.h for atof() usage under Linux
 * Used being_destroyed flag check in the Resize and Expose methods
 *
 * Revision 1.4  1995/03/07  01:07:20  venkat
 * Class record changed to subclass XoComposite
 *
 * Revision 1.3  1994/03/22  15:29:32  bhalla
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
#include "DumbDrawP.h"
#include "PixP.h"



/*
** This is a simple-minded widget for holding pixes. Its transforms are 
** just scalings on x,y,or z axis projections. It is a good template for
** more advanced coredraw based widgets.
*/

static XtResource resources[] = {
#define offset(field) XtOffset(DumbDrawWidget, dumbdraw.field)
  {XtNzmin, XtCPosition, XtRFloat,  sizeof(float),
	 offset(zmin), XtRString, "0"},
  {XtNzmax, XtCPosition, XtRFloat,  sizeof(float),
	 offset(zmax), XtRString, "1"},
  {XtNaxis, XtCAxis, XtRAxis,sizeof(char),
	 offset(axis), XtRString,"z"}
#undef offset
};

/* 
** The main mouse callback operation should be selection of pixes
** The main keyboard callback operation should be zooming and panning
*/
static void XoDumbDrawSelectPix();
static void XoDumbDrawUnSelectPix();
static void XoDumbDrawEnter();
static void  Zoom(), Pan(), Restore(), SetAxis();
/* arguments are : x,y (fractions of screen, relative) */
static void Print();
static void Initialize(),Destroy(), Resize();
static void Redisplay();
static void TransformPoint(),TransformPoints(),BackTransform();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static Boolean XoCvtStringToAxis();
static Boolean XoCvtAxisToString();
static void ClassInitialize();

static XtActionsRec actions[] =
{
  {"pan",	 Pan},
  {"zoom", Zoom},
  {"restore", Restore},
  {"print", Print},
  {"select", XoDumbDrawSelectPix},
  {"unselect", XoDumbDrawUnSelectPix},
  {"set_axis", SetAxis},
  {"enter", XoDumbDrawEnter},
};

static char translations[] =
" <Key>Left:	pan(0.1,0)	\n\
  <Key>Right:      pan(-0.10,0)	\n\
  <Key>Down:       pan(0,0.10)       \n\
  <Key>Up:   pan(0,-0.10)      \n\
  <Key><:	zoom(1.1,1.1)	\n\
  <Key>>:	zoom(0.9,0.9)	\n\
  <Key>u:	restore()	\n\
  <Key>x:	set_axis(\"x\")	\n\
  <Key>y:	set_axis(\"y\")	\n\
  <Key>z:	set_axis(\"z\")	\n\
  <Key>P:	print()		\n\
  <BtnDown> : select()	\n\
  <BtnUp> : unselect()	\n\
  <Enter> : enter()	\n\
";

DumbDrawClassRec dumbdrawClassRec = {
  { /* core fields */
  /* superclass		*/	(WidgetClass) &coredrawClassRec,
    /* class_name		*/	"DumbDraw",
    /* widget_size		*/	sizeof(DumbDrawRec),
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
    /* back_transform		*/	BackTransform,
	/* selectpix			*/	XoInheritSelectPix,
	/* unselectpix			*/	XoInheritUnSelectPix,
    /* extension			*/	NULL
  },
  { /* dumbdraw_class fields */
	/* empty				*/  0
  }
};

WidgetClass dumbdrawWidgetClass = (WidgetClass)&dumbdrawClassRec;

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRAxis,
                      XoCvtStringToAxis, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRAxis, XtRString,
                      XoCvtAxisToString, NULL, 0 ,
                      XtCacheNone, NULL);
}

/* ARGUSED */
static void Initialize (req, new,args, num_args)
     Widget req, new;
	 ArgList	args;
	 Cardinal	*num_args;
{
	DumbDrawWidget dw = (DumbDrawWidget) new;
	/* This is a hack. I need to figure out a way of doing it
	** properly */
	if (dw->core.width == 0)
		dw->core.width = 20;
	if (dw->core.height == 0)
		dw->core.height = 20;
	if (dw->dumbdraw.axis != 'x' ||
		dw->dumbdraw.axis != 'y' ||
		dw->dumbdraw.axis != 'z')
		dw->dumbdraw.axis = 'z';
	/* dw->dumbdraw.axis = XtCalloc(12,sizeof(char)); */
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
	DumbDrawWidget dcw = (DumbDrawWidget) curr;
	DumbDrawWidget drw = (DumbDrawWidget) req;
	DumbDrawWidget dnw = (DumbDrawWidget) new;
	/* Check if field is interesting */
	/* Note that the xmin etc fields are dealt with in the superclass */
	if (((drw->dumbdraw.zmin != dcw->dumbdraw.zmin) ||
		drw->dumbdraw.zmax != dcw->dumbdraw.zmax)) {
		/* check if the ranges are consistent */
		if (drw->dumbdraw.zmin > drw->dumbdraw.zmax) {
			dnw->dumbdraw.zmin = dcw->dumbdraw.zmin;
			dnw->dumbdraw.zmax = dcw->dumbdraw.zmax;
			return(False);
		} else {
			return(True);
		}
	}
	if (dnw->dumbdraw.axis!=dcw->dumbdraw.axis) {
		/* check for a valid axis */
		if (dnw->dumbdraw.axis == 'x' || 
			dnw->dumbdraw.axis == 'y' || 
			dnw->dumbdraw.axis == 'z') {
			return(True);
		} else {
			dnw->dumbdraw.axis=dcw->dumbdraw.axis;
			return(False);
		}
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
  DumbDrawWidget dw = (DumbDrawWidget) w;
 Widget *childP, *first_born = (Widget *)(dw->composite.children);
  int num_children = dw->composite.num_children;
  PixObjectClass class;

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
	/* expose happens automatically here */
}

static void Redisplay (w, e, region)
     Widget w;
     XEvent *e;
     Region region;
{
  DumbDrawWidget dw = (DumbDrawWidget) w;
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
	DumbDrawWidget dw=(DumbDrawWidget)w;
}

/* This is inherited from the core widget class */
static void XoDumbDrawSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	(dumbdrawClassRec.coredraw_class.selectpix)(w,event);
}

/* This is inherited from the core widget class */
static void XoDumbDrawUnSelectPix(w,event,params,num_params)
	Widget	w;
	XButtonEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	(dumbdrawClassRec.coredraw_class.unselectpix)(w,event);
}

/* This is the same as the core widget class, but is not yet
** inherited */

static void XoDumbDrawEnter(w,event)
    Widget  w;
    XButtonEvent    *event;
{
  DumbDrawWidget dw = (DumbDrawWidget) w;
  Widget pix;
  static XoEventInfo    info;

  pix = XoFindNearestPix(w,event);
  if (pix != NULL) {
        /* Call the callbacks for a possible drag-drop operation
        ** within the draw widget */
        info.event = XoEnter;
        info.ret="";
		/* If it is the draw, use 0 because dumbdraws do not 
		** worry about depth. If it is a pix, use its cz */
		info.z = (pix == w) ? 0.0 : ((PixObject)pix)->pix.cz;
		(dumbdrawClassRec.coredraw_class.backtransform)(
		   w,event->x,event->y,
		   &(info.x),&(info.y),&(info.z));
        info.key = event->time;
        /* Calling the drag callback */
        if (pix == w)
            XtCallCallbackList(w,dw->coredraw.callbacks,
                (XtPointer)&info);
        else
            XtCallCallbackList(pix,((PixObject)pix)->pix.callbacks,
                (XtPointer)&info);
  }
}




static void  SetAxis(w,event,params,num_params)
	Widget	w;
	XEvent	*event;
	String	*params;
	Cardinal	*num_params;
{
	char	axis;
	DumbDrawWidget	dw = (DumbDrawWidget)w;
	static XoEventInfo	info;
	if (*num_params == 1) {
		axis = params[0][0];
		if (strlen(params[0]) == 1 && 
			(axis == 'x' || axis == 'y' || axis == 'z')) {
			dw->dumbdraw.axis = axis;
			info.event = XoUpdateFields;
			info.ret = "axis";
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
	float zx,zy;
	float mx,my,dx,dy;
	DumbDrawWidget	dw = (DumbDrawWidget)w;
	static XoEventInfo	info;
	float	*pxmin = NULL,*pxmax = NULL,*pymin = NULL,*pymax = NULL;
	
	if (*num_params == 2) {
		zx = atof(params[0]);
		zy = atof(params[1]);
		switch(dw->dumbdraw.axis) {
			case 'x':
				pxmin = &(dw->coredraw.ymin);
				pxmax = &(dw->coredraw.ymax);
				pymin = &(dw->dumbdraw.zmin);
				pymax = &(dw->dumbdraw.zmax);
				info.ret = "ymin ymax zmin zmax";
				break;
			case 'y':
				pxmin = &(dw->coredraw.xmin);
				pxmax = &(dw->coredraw.xmax);
				pymin = &(dw->dumbdraw.zmin);
				pymax = &(dw->dumbdraw.zmax);
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
				break;
		}
		mx = (*pxmin + *pxmax)/2.0;
		my = (*pymin + *pymax)/2.0;
		dx = (*pxmax - *pxmin)/2.0;
		dy = (*pymax - *pymin)/2.0;
		*pxmin = mx - dx * zx;
		*pymin = my - dy * zy;
		*pxmax = mx + dx * zx;
		*pymax = my + dy * zy;
		info.event = XoUpdateFields;
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
	float px,py;
	float mx,my,dx,dy;
	DumbDrawWidget	dw = (DumbDrawWidget)w;
	static XoEventInfo	info;
	float	*pxmin = NULL,*pxmax = NULL,*pymin = NULL,*pymax = NULL;
	
	if (*num_params == 2) {
		px = atof(params[0]);
		py = atof(params[1]);
		switch(dw->dumbdraw.axis) {
			case 'x':
				pxmin = &(dw->coredraw.ymin);
				pxmax = &(dw->coredraw.ymax);
				pymin = &(dw->dumbdraw.zmin);
				pymax = &(dw->dumbdraw.zmax);
				info.ret = "ymin ymax zmin zmax";
				break;
			case 'y':
				pxmin = &(dw->coredraw.xmin);
				pxmax = &(dw->coredraw.xmax);
				pymin = &(dw->dumbdraw.zmin);
				pymax = &(dw->dumbdraw.zmax);
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
		info.event = XoUpdateFields;
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
	DumbDrawWidget dw = (DumbDrawWidget)w;
	static XoEventInfo	info;
		dw->coredraw.xmin = 0;
		dw->coredraw.ymin = 0;
		dw->dumbdraw.zmin = 0;
		dw->coredraw.xmax = 1;
		dw->coredraw.ymax = 1;
		dw->dumbdraw.zmax = 1;
		info.event = XoUpdateFields;
		info.ret = "ymin ymax xmin xmax zmin zmax";
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
	DumbDrawWidget	dw = (DumbDrawWidget)w;
	switch (dw->dumbdraw.axis) {
		case 'x' :
			*sx=(int)((float)dw->core.width *
			 (y-dw->coredraw.ymin)/
			(dw->coredraw.ymax - dw->coredraw.ymin));
			*sy=(int)(dw->core.height * (1.0 -
			 (z-dw->dumbdraw.zmin)/
			(dw->dumbdraw.zmax - dw->dumbdraw.zmin)));
			*sz = (int)x; /* Note that we might lose info here */
		break;
		case 'y' :
			*sx=(int)((float)dw->core.width *
			 (x-dw->coredraw.xmin)/
			(dw->coredraw.xmax - dw->coredraw.xmin));
			*sy=(int)(dw->core.height * (1.0 -
			 (z-dw->dumbdraw.zmin)/
			(dw->dumbdraw.zmax - dw->dumbdraw.zmin)));
			*sz = (int)y; /* Note that we might lose info here */
		break;
		case 'z' :
			*sx=(int)((float)dw->core.width *
			 (x-dw->coredraw.xmin)/
			(dw->coredraw.xmax - dw->coredraw.xmin));
			*sy=(int)(dw->core.height * (1.0 -
			 (y-dw->coredraw.ymin)/
			(dw->coredraw.ymax - dw->coredraw.ymin)));
			*sz = (int)z; /* Note that we might lose info here */
		break;
	}
}

static void TransformPoints(w,x,y,z,sx,sy,sz,n)
	Widget w;
	double *x,*y,*z;
	int	*sx,*sy,*sz;
	int n;
{
	DumbDrawWidget	dw = (DumbDrawWidget)w;
	int	i;
	float dx,dy;
	switch (dw->dumbdraw.axis) {
		case 'x' :
			dx=dw->coredraw.ymax-dw->coredraw.ymin;
			dy=dw->dumbdraw.zmax-dw->dumbdraw.zmin;
			for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
				*sx=(int)((float)dw->core.width *
			 	(*y-dw->coredraw.ymin)/dx);
				*sy=(int)(dw->core.height * (1.0 -
			 	(*z-dw->dumbdraw.zmin)/dy));
				*sz = (int)*x; /* Note: we might lose info here */
			}
		break;
		case 'y' :
			dx=dw->coredraw.xmax-dw->coredraw.xmin;
			dy=dw->dumbdraw.zmax-dw->dumbdraw.zmin;
			for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
				*sx=(int)((float)dw->core.width *
			 	(*x-dw->coredraw.xmin)/dx);
				*sy=(int)(dw->core.height * (1.0 -
			 	(*z-dw->dumbdraw.zmin)/dy));
				*sz = (int)*y; /* Note: we might lose info here */
			}
		break;
		case 'z' :
			dx=dw->coredraw.xmax-dw->coredraw.xmin;
			dy=dw->coredraw.ymax-dw->coredraw.ymin;
			for(i=0;i<n;i++,x++,y++,z++,sx++,sy++,sz++) {
				*sx=(int)((float)dw->core.width *
			 	(*x-dw->coredraw.xmin)/dx);
				*sy=(int)(dw->core.height * (1.0 -
			 	(*y-dw->coredraw.ymin)/dy));
				*sz = (int)*z; /* Note: we might lose info here */
			}
		break;
	}
}


static void BackTransform(w,sx,sy,x,y,z)
	Widget w;
	int	sx,sy;
	float *x,*y,*z;
{
	DumbDrawWidget	dw = (DumbDrawWidget)w;
	switch (dw->dumbdraw.axis) {
		case 'x' :
			*x = 0.0;
			*y =(1.0 - (float)sx/(float)(dw->core.width)) * 
			(dw->coredraw.ymax - dw->coredraw.ymin) +
			dw->coredraw.ymin;
			*z =(1.0 - (float)sy/(float)(dw->core.height)) * 
			(dw->dumbdraw.zmax - dw->dumbdraw.zmin) +
			dw->dumbdraw.zmin;
		break;
		case 'y' :
			*x =(1.0 - (float)sx/(float)(dw->core.width)) * 
			(dw->coredraw.xmax - dw->coredraw.xmin) +
			dw->coredraw.xmin;
			*y = 0.0;
			*z =(1.0 - (float)sy/(float)(dw->core.height)) * 
			(dw->dumbdraw.zmax - dw->dumbdraw.zmin) +
			dw->dumbdraw.zmin;
		break;
		case 'z' :
			*x =(1.0 - (float)sx/(float)(dw->core.width)) * 
			(dw->coredraw.xmax - dw->coredraw.xmin) +
			dw->coredraw.xmin;
			*y =(1.0 - (float)sy/(float)(dw->core.width)) * 
			(dw->coredraw.ymax - dw->coredraw.ymin) +
			dw->coredraw.ymin;
			*z = 0.0;
		break;
	}
}




static Boolean XoCvtStringToAxis(dpy,args,num_args,fromVal,toVal,
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
		if (ret == 'x' || ret == 'y' || ret == 'z') {
			XoCvtDone(char,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRAxis");
		return((Boolean) False);
}


static Boolean XoCvtAxisToString(dpy,args,num_args,fromVal,toVal,
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
		if (ret == 'x' || ret == 'y' || ret == 'z') {
			XoCvtDone(char,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRString");
		return((Boolean) False);
}
