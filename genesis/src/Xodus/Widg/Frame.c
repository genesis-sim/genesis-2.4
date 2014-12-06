/* $Id: Frame.c,v 1.4 2006/02/17 06:30:33 svitak Exp $ */
/*
 * $Log: Frame.c,v $
 * Revision 1.4  2006/02/17 06:30:33  svitak
 * Fix to prevent crash if frame is resized too small.
 *
 * Revision 1.3  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.10  2000/07/03 18:14:16  mhucka
 * Added missing type casts.
 *
 * Revision 1.9  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.8  2000/05/02 06:18:08  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.7  1995/06/02 19:46:33  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.6  1995/04/05  22:02:42  venkat
 * PaintFrame() changed to update the frame widget's width and height
 * based on the child widget's width and height. This makes the frames
 * to be drawn for the entire width and height of the widget instead
 * of 80% something of the width or height.
 *
 * Revision 1.5  1995/03/06  19:44:47  venkat
 * Class Initialization modified to subclass directly from
 * XoComposite
 *
 * Revision 1.4  1995/02/24  20:20:39  venkat
 * Changed name to XoGetUsedColormap(0
 *
 * Revision 1.3  1995/02/21  01:21:12  venkat
 *  Added code in ChangeManaged() to implement short-term
 * >> workaround for the xcolorscale problem
 *
 * Revision 1.2  1994/03/22  15:19:36  bhalla
 * Jason made all sorts of comments to this file, but I dont think he
 * touched much of the code.
 * */
#include <stdio.h>
#include <stdlib.h>	/* malloc() */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "FrameP.h"

#define UndefinedPixel  ( (1 << (sizeof(long)*4-2)) - 1 )

static XtResource resources[] = {
#define offset(field) XtOffset(FrameWidget, frame.field)
  { XtNelevation, XtCElevation, XtRXoFrameElevation, 
    sizeof(XoFrameElevation),  offset(elevation), 
    XtRImmediate, (XtPointer) XoRaisedFrame },
  { XtNhighlight, XtCBorderColor, XtRXoPixel, sizeof(Pixel), 
    offset(highlight), XtRImmediate, (XtPointer)UndefinedPixel},
  { XtNshadow, XtCBorderColor, XtRXoPixel, sizeof(Pixel), 
    offset(shadow), XtRImmediate, (XtPointer)UndefinedPixel},
/*  { XtNchildClass, XtCChildClass, XtRWidgetClass, sizeof (WidgetClass),
    offset(child_class), XtRImmediate, NULL }, */
#undef offset
};

/* Methods */
static void 
  ClassInitialize(), PaintFrame(), Initialize(), InsertChild(), 
  ChangeManaged(), PlaceChild();
static Boolean
  SetValues();
static XtGeometryResult 
  GeometryManager(), QueryGeometry();

/* Utilities */
static Boolean 
  XoCvtStringToFrameElevation();

static void
  XoRegisterFrameElevationQuarks(), XoRegisterFrameElevationConverter();


FrameClassRec frameClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) (&xocompositeClassRec),
    /* class_name		*/	"Frame",
    /* widget_size		*/	sizeof(FrameRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	PlaceChild,
    /* expose			*/	PaintFrame,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	QueryGeometry,
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
  { /* xocomposite fields */
    /* empty			*/	0
  },
  { /* frame fields */
    /* empty			*/	0
  }
};

WidgetClass frameWidgetClass = (WidgetClass)&frameClassRec;

/***************************************************************************
  Methods:
  */

static void ClassInitialize()
{
  XoRegisterFrameElevationConverter();
}
    
static void Initialize (request, new, args, nargs)
     Widget request, new;
     ArgList args;
     Cardinal *nargs;
{
  FrameWidget fw = (FrameWidget) new;
  Display      *dpy;
  int          scr_number;
  XColor       flat;
  unsigned long intensity;
  XGCValues    val;
  unsigned long valmask = 0;

  dpy = XtDisplay (fw);
  scr_number = DefaultScreen (dpy);

  /* hijack the core border_width and border_pixel (color) fields */
  fw->frame.frame_width = fw->core.border_width;
  fw->frame.flat         = fw->core.border_pixel;
  fw->core.border_width = 0;

  /* figure colors */
  flat.pixel = fw->frame.flat;
  XQueryColor (dpy, DefaultColormap(dpy, scr_number), &flat);
  intensity = flat.red + flat.green + flat.blue;
  
  /* for the moment just use white and black pixel */

  if (fw->frame.shadow == UndefinedPixel) {
    fw->frame.shadow = BlackPixel(dpy, scr_number);
  }
  if (fw->frame.highlight == UndefinedPixel) {
    fw->frame.highlight = WhitePixel(dpy, scr_number);
  }
  
  /* create GCs */
  val.foreground = fw->frame.flat, valmask |= GCForeground;
  fw->frame.flat_gc = XtGetGC((Widget) fw, valmask, &val);

  val.foreground = fw->frame.shadow;
  fw->frame.shadow_gc = XtGetGC((Widget) fw, valmask, &val);

  val.foreground = fw->frame.highlight;
  fw->frame.highlight_gc = XtGetGC((Widget) fw, valmask, &val);
}



static void PaintFrame (w)
     Widget w;
{
  FrameWidget fw = (FrameWidget) w;
  Display *dpy;
  Window   win;
  GC gc;
  Dimension i;

    /* Make the frame follow the child's dimensions since the painting is 
 	done based on the frame's width and height */

    fw->core.width = fw->composite.children[0]->core.width 
                                                  + 2 * fw->frame.frame_width;
    fw->core.height = fw->composite.children[0]->core.height 
                                                  + 2 * fw->frame.frame_width;
  dpy = XtDisplay (w);
  win = XtWindow  (w);
  /* Draw top and left */
  switch (fw->frame.elevation) {
  case XoRaisedFrame: gc = fw->frame.highlight_gc; break;
  case XoFlatFrame:   gc = fw->frame.flat_gc; break;
  case XoInsetFrame:  gc = fw->frame.shadow_gc; break;
  }
  for (i = 0; i < fw->frame.frame_width; i++)
    XDrawLine (dpy, win, gc, 0, i, fw->core.width  - i, i);
  for (i = 0; i < fw->frame.frame_width; i++)
    XDrawLine (dpy, win, gc, i, 0, i, fw->core.height - i);
  
  /* Draw right and bottom */
    
  switch (fw->frame.elevation) {
  case XoRaisedFrame: gc = fw->frame.shadow_gc; break;
  case XoFlatFrame:   gc = fw->frame.flat_gc; break;
  case XoInsetFrame:  gc = fw->frame.highlight_gc; break;
  }
  for (i = 0; i < fw->frame.frame_width; i++)
    XDrawLine(dpy, win, gc, fw->core.width - 1 - i, fw->core.height - 1, 
	                    fw->core.width - 1 - i, i + 1);
  for (i = 0; i < fw->frame.frame_width; i++)
    XDrawLine(dpy, win, gc, fw->core.width - 1, fw->core.height - 1 - i,
	                    i + 1, fw->core.height - 1 - i);
	/*
	printf("painting frame %s (%d,%d) child %s is (%d,%d)\n",
		fw->core.name,fw->core.width,fw->core.height,
		fw->composite.children[0]->core.name,
		fw->composite.children[0]->core.width,
		fw->composite.children[0]->core.height);
	*/
}

static void ChangeManaged (w)
     Widget w;
{
  FrameWidget fw    = (FrameWidget) w;
  Widget       child = fw->composite.children[0];
  XtGeometryResult	ans;

  Dimension 	neww,newh;
  Dimension 	retw,reth;

  /* Terrifying call here. I do not think it should be used -- Upi */
  /*
  fw->core.width =
    child->core.width + 2*child->core.border_width + 2*fw->frame.frame_width;
  fw->core.height = 
    child->core.height + 2*child->core.border_width +2*fw->frame.frame_width;
  XtResizeWindow (fw);
  */

   neww = child->core.width + 2*child->core.border_width + 2*fw->frame.frame_width;
   newh = child->core.height + 2*child->core.border_width + 2*fw->frame.frame_width;
  ans = XtMakeResizeRequest(w,neww,newh,&retw,&reth);
  if (ans == XtGeometryAlmost)
  	XtMakeResizeRequest(w,retw,reth,&neww,&newh);
  
  PlaceChild (fw);
}

static void InsertChild (w)
     Widget w;
{
  FrameWidget fw = (FrameWidget) w->core.parent;
  
  if (fw->composite.num_children) {
    String subs[2];
    Cardinal num_subs = 2;
 
    subs[0] = (caddr_t) fw->core.name;
    subs[1] = (caddr_t) w->core.name;
    
    XtAppErrorMsg (XtWidgetToApplicationContext (w),
                   "unplannedParenthood", "xoFrame", "XoToolkitError",
                   "FrameWidget '%s' cannot accomodate unplanned child '%s'",
                   subs, &num_subs);
    return;
  }

  fw->composite.num_children = 1;
  fw->composite.children = (Widget *) malloc (sizeof (Widget));
  fw->composite.children[0] = w;
}

  

static XtGeometryResult GeometryManager(child, request, reply)
     Widget child;
     XtWidgetGeometry *request, *reply;
{
  FrameWidget fw = (FrameWidget) child->core.parent;
  XtGeometryResult answer;

  /* printf("Geometry manager for frame widget \n"); */
  /* Always deny a position request (child always at 0,0) */
  if ((request->request_mode & CWX && request->x != child->core.x) ||
      (request->request_mode & CWY && request->y != child->core.y))
    return (XtGeometryNo);
 
  /* For resize requests we simply act as a go-between */
  
  if (request->request_mode & (CWWidth | CWHeight | CWBorderWidth)){
    Dimension w, h;

    /* Make all three fields in the request valid */
    if ((request->request_mode & CWWidth) == 0)
      request->width = child->core.width;
    if ((request->request_mode & CWHeight) == 0)
      request->height = child->core.height;
    if ((request->request_mode & CWBorderWidth) == 0)
      request->border_width = child->core.border_width;

    w = request->width  + 2*request->border_width + 2*fw->frame.frame_width;
    h = request->height + 2*request->border_width + 2*fw->frame.frame_width;
    answer = XtMakeResizeRequest ((Widget) fw, w, h, &w, &h);
    if (answer == XtGeometryAlmost) {
      reply->width  = w - 2*request->border_width - 2*fw->frame.frame_width;
      reply->height = h - 2*request->border_width - 2*fw->frame.frame_width;
    /*	answer = XtMakeResizeRequest (fw, w, h, &w, &h); */
    }
	if (answer == XtGeometryYes) {
		PlaceChild(fw);
	}
    return answer;
  }
  
  /* must have been a stacking request */

  return XtGeometryYes;
}
    

/* Resize method.  Also called by ChangeManaged */

static void PlaceChild (fw)
     FrameWidget fw;
{
  Widget child;
 /* printf("PlaceChild for frame widget \n"); */

  if (! fw->composite.num_children ||
	fw->core.width - 2*fw->frame.frame_width <= 0 ||
	fw->core.height - 2*fw->frame.frame_width <= 0)
    return;

  child = fw->composite.children[0];

  XtConfigureWidget (child, fw->frame.frame_width, fw->frame.frame_width,
		            fw->core.width - 2*fw->frame.frame_width,
		            fw->core.height - 2*fw->frame.frame_width,
		            0);
}
     
  

static XtGeometryResult QueryGeometry (w, constraint, preferred)
     Widget w;
     XtWidgetGeometry *constraint, *preferred;
{
  FrameWidget fw = (FrameWidget) w;
  XtWidgetGeometry child_constraint;
  XtGeometryResult answer;

  if (fw->composite.num_children == 0)
    /* anything goes before we have any children */
    return XtGeometryYes;

  /* Just as for Geometry Manager we are only a go-between, so we query
     the child. */
  child_constraint = *constraint;
  if (child_constraint.request_mode & CWWidth)
    child_constraint.width -= 2*fw->frame.frame_width;
  if (child_constraint.request_mode & CWHeight)
    child_constraint.height -= 2*fw->frame.frame_width;
  answer = 
    XtQueryGeometry (fw->composite.children[0], &child_constraint, preferred);
  preferred->width += 2*fw->frame.frame_width;
  preferred->height += 2*fw->frame.frame_width;
  return answer;
}


static Boolean SetValues (current, request, new, args, nargs)
     Widget current, request, new;
     ArgList args;
     Cardinal *nargs;
{
  FrameWidget curfw = (FrameWidget) current;
  FrameWidget newfw = (FrameWidget) new;
  Boolean ret = False;

  /* printf("SetValues for frame widget \n"); */

  /* check for changes in border_width */

  if (newfw->core.border_width) {
    newfw->frame.frame_width = newfw->core.border_width;
    newfw->core.border_width = 0;
    PlaceChild (newfw);
    ret = True;
  } 
  /* Just checking .. nope, it doesnt work */
  /*
  if (newfw->core.width != curfw->core.width ||
	newfw->core.height != curfw->core.height) {
    newfw->frame.frame_width = newfw->core.border_width;
    newfw->core.border_width = 0;
    PlaceChild (newfw);
    ret = True;
  } 
  */

  /* check for changes in color */
  if (newfw->core.border_pixel != curfw->core.border_pixel) {
    newfw->frame.flat = newfw->core.border_pixel;
  }
    
  /* pass unrecognized args down to the child */
  if (newfw->composite.num_children) {
    XtSetValues (newfw->composite.children[0], args, *nargs);
  }

	return ret;
}

  

/**************************************************************************
  Utilities.
  */


static XrmQuark 
  XtQflat, XtQraised, XtQinset;

static void XoRegisterFrameElevationQuarks ()
{
  XtQflat       = XrmStringToQuark("flat");
  XtQraised     = XrmStringToQuark("raised");
  XtQinset      = XrmStringToQuark("inset");
}

/* ARGSUSED */
static Boolean 
  XoCvtStringToFrameElevation(dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
  static XoFrameElevation frameElevation;
  XrmQuark q;
  char lowerName[1000];
  
  XoLowerName (lowerName, (char*)fromVal->addr);
  q = XrmStringToQuark(lowerName);
  if (q == XtQflat)
    frameElevation = XoFlatFrame;
  else if (q == XtQraised)
    frameElevation = XoRaisedFrame;
  else if (q == XtQinset)
    frameElevation = XoInsetFrame;
  else {
    XtStringConversionWarning(fromVal->addr, "frameElevation");
    toVal->size = sizeof(XoFrameElevation);
    return((Boolean)False);
  }
  XoCvtDone(XoFrameElevation, frameElevation);
}

static void XoRegisterFrameElevationConverter ()
{
  XoRegisterFrameElevationQuarks();
  XtSetTypeConverter (XtRString, XtRXoFrameElevation, 
		      (XtTypeConverter) XoCvtStringToFrameElevation,
		      NULL, 0,
		      XtCacheNone, NULL);
}





/*************************************************************************
  Public functions.
  */

Widget XoChild (w)
     Widget w;
{
  CompositeWidget cw = (CompositeWidget) w;

  if ( ! XtIsComposite(w) )
    return NULL;
  
  if (cw->composite.num_children == 0)
    return NULL;

  return cw->composite.children[0];
}
