/* $Id: Button.c,v 1.3 2005/07/20 20:01:40 svitak Exp $ */
/*
 * $Log: Button.c,v $
 * Revision 1.3  2005/07/20 20:01:40  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:49  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.11  2000/07/03 18:14:16  mhucka
 * Added missing type casts.
 *
 * Revision 1.10  2000/06/12 04:14:04  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added type casts where appropriate.
 *
 * Revision 1.9  2000/05/02 06:18:07  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.8  1995/12/06 01:17:37  venkat
 * Changed the Notify event handler to map the poped up label child
 * for PressButtons to display undepressed even if the callback script
 * unmaps the parent form.
 *
 * Revision 1.7  1995/09/26  23:49:57  venkat
 * Changed XtVaSetValues() calls to XoXtVaSetValues()
 *
 * Revision 1.6  1995/06/02  19:46:33  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.5  1995/03/24  23:52:07  venkat
 * Changed the default values of all background resources
 * to XoDefaultBackground. This macro is defined in Xo/XoDefs.h as
 * the fallback or default background string, namely "LightSteelBlue"
 *
 * Revision 1.4  1995/03/06  19:01:54  venkat
 * Modified to be subclassed from XoComposite
 *
 * Revision 1.3  1994/04/25  18:07:44  bhalla
 * Fixed bug in SetValues which was preventing font and fg from
 * being set for the off state
 *
 * Revision 1.2  1994/03/22  15:25:56  bhalla
 * This file seems to have been Jasonized, I do not think there are any
 * code changes
 * */
#include <stdio.h>	/* needed by stdlib.h on some architectures */
#include <stdlib.h>	/* malloc() */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "Widg/FramedP.h"
#include "Widg/LabelP.h"
#include "ButtonP.h"

#include <stdio.h>

static XtResource resources[] = {
#define offset(field) XtOffset(ButtonWidget, button.field)
  { XtNtype, XtCType, XtRButtonType, sizeof (XoButtonType),
    offset(type), XtRImmediate, (XtPointer) XoPressButton },
  { XtNstate, XtCState, XtRBoolean, sizeof (Boolean), 
    offset(state), XtRImmediate, (XtPointer) False },
  { XtNonLabel, XtCLabel, XtRString, sizeof (String),
    offset(onlabel), XtRString, (XtPointer) NULL },
  { XtNoffLabel, XtCLabel, XtRString, sizeof (String),
    offset(offlabel), XtRString, (XtPointer) NULL },
  { XtNonBg, XtCBackground, XtRXoPixel, sizeof (Pixel),
    offset(onbg), XtRString, (XtPointer) XoDefaultBackground },
  { XtNoffBg, XtCBackground, XtRXoPixel, sizeof (Pixel),
    offset(offbg), XtRString, (XtPointer) XoDefaultBackground },
  { XtNonFg, XtCForeground, XtRXoPixel, sizeof (Pixel),
    offset(onfg), XtRString, (XtPointer) "XtDefaultForeground" },
  { XtNoffFg, XtCForeground, XtRXoPixel, sizeof (Pixel),
    offset(offfg), XtRString, (XtPointer) "XtDefaultForeground" },
  { XtNonFont, XtCFont, XtRFontStruct, sizeof (XFontStruct *),
    offset(onfont), XtRString, (XtPointer) "XtDefaultFont" },
  { XtNoffFont, XtCFont, XtRFontStruct, sizeof (XFontStruct *),
    offset(offfont), XtRString, (XtPointer) "XtDefaultFont" },
  { XtNradioData, XtCValue, XtRString, sizeof (String),
    offset(radiodata), XtRString, (XtPointer) "radio" },
  { XtNradioGroup, XtCRadioGroup, XtRWidget, sizeof (Widget),
    offset(radiogroup), XtRString, (XtPointer) NULL },
  { XtNcallback, XtCCallback, XtRCallback, sizeof (XtPointer),
    offset(callbacks), XtRCallback, (XtPointer) NULL},
#undef offset
};

static Boolean SetValues();
/* Utilities */
static void XoRegisterButtonTypeConverter();

/* Actions */
static void On(), Off(), Notify(), Toggle();

static XtActionsRec actions[] =
{
  {"on",	On},
  {"off",       Off},
  {"notify",    Notify},
  {"toggle",    Toggle},
};

static char press_translations[] =
  "<BtnDown>:		on()     notify() \n\
   <BtnUp>:            off()             ";

static char hold_translations[] =
  "<BtnDown>:          on()     notify() \n\
   <BtnUp>:            off()    notify() ";

static char toggle_translations[] =
  "<BtnDown>:          toggle() notify() ";

/* Methods */
static void
  ClassInitialize(), Initialize(), MapChild(), Resize(), InsertChild();
static Boolean SetValues();
#ifdef Expose
#undef Expose
#endif
static void Expose();

static XtGeometryResult 
  GeometryManager();
#define superclass  (&xocompositeClassRec)

ButtonClassRec buttonClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) superclass,
    /* class_name		*/	"Button",
    /* widget_size		*/	sizeof(ButtonRec),
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
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	Resize,
    /* expose			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	press_translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* composite fields */
    /* geometry_manager         */      GeometryManager,
    /* change_managed           */      NULL,
    /* insert_child             */      InsertChild,
    /* delete_child             */      XtInheritDeleteChild,
    /* extension                */      NULL
  },
  { /* xocomposite fields */
    /* empty			*/	0
  },
  { /* button fields */
    /* empty			*/	0
  }
};

WidgetClass buttonWidgetClass = (WidgetClass)&buttonClassRec;


/***************************************************************************
  Methods:
*/

static void ClassInitialize()
{
  XoRegisterButtonTypeConverter();
} 

/* number of Args generated locally */
#define LOCAL_ARGS 10

static void Initialize (request, new, arg_list, arg_num)
     Widget    request, new;
     ArgList   arg_list;
     Cardinal  *arg_num;
{
  ButtonWidget bw = (ButtonWidget) new;

  bw->button.on = (LabelWidget)XtVaCreateManagedWidget("on",
	framedWidgetClass, (Widget) bw,
	XtNchildClass,labelWidgetClass,
	XtNelevation,XoInsetFrame,
	XtNlabel,bw->button.onlabel,
	XtNfont,bw->button.onfont,
	NULL);

  bw->button.off = (LabelWidget)XtVaCreateManagedWidget("off",
	framedWidgetClass, (Widget) bw,
	XtNchildClass,labelWidgetClass,
	XtNelevation,XoRaisedFrame,
	XtNlabel,bw->button.offlabel,
	XtNfont,bw->button.offfont,
	NULL);

#define max(a,b) ((a)>(b)?(a):(b))
  bw->core.width = 
    max ((int)bw->button.on->core.width +
		2 * (int)bw->button.on->core.border_width,  
	 (int)bw->button.off->core.width +
		2 * (int)bw->button.off->core.border_width);
  bw->core.height = 
    max ((int)bw->button.on->core.height +
		2 * (int)bw->button.on->core.border_width,
	 (int)bw->button.off->core.height +
	 2 * (int)bw->button.off->core.border_width);
#undef max
  Resize (bw);
  MapChild (bw);

  /* install translations */
  switch (bw->button.type) {
  case XoPressButton: break;	/* translations installed by default */
  case XoHoldButton:
    bw->core.tm.translations = XtParseTranslationTable (hold_translations);
    break;
  case XoToggleButton:
    bw->core.tm.translations = XtParseTranslationTable (toggle_translations);
    break;
  }

  /* default the core border_width to zero */
  bw->core.border_width = 0;
}


static void Resize (w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w;
  Widget cw;
  static XoEventInfo	info;
  
  cw = (Widget)bw->button.on;
  XtResizeWidget (cw, bw->core.width - 2 * cw->core.border_width,
		      bw->core.height - 2 * cw->core.border_width,
		      cw->core.border_width);
  cw = (Widget)bw->button.off;
  XtResizeWidget (cw, bw->core.width - 2 * cw->core.border_width,
		      bw->core.height - 2 * cw->core.border_width,
		      cw->core.border_width);

  info.event = XoUpdateFields;
  info.ret="xgeom ygeom hgeom wgeom";
  info.x=info.y=info.z = 0.0;
  XtCallCallbackList(w,bw->button.callbacks, (XtPointer)&info);
}


static void Expose (w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w;
	if (bw->button.type == XoToggleButton)
	{
  		MapChild (bw);
	}
}



static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
    ButtonWidget curbw = (ButtonWidget) current;
    ButtonWidget newbw = (ButtonWidget) new;
	Boolean redisplay = False;

    /* printf("in ButtonWidget SetValues\n"); */

	if (curbw->button.type == XoToggleButton && 
		newbw->button.state != curbw->button.state) {
  		MapChild (newbw);
		redisplay = True;
	}
	if (newbw->button.onlabel != curbw->button.onlabel) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.on),
			XtNlabel,newbw->button.onlabel,
			NULL);
		newbw->button.onlabel = newbw->button.on->label.label;
		redisplay = True;
	}
	if (newbw->button.onbg != curbw->button.onbg) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.on),
			XtNbg,newbw->button.onbg,
			NULL);
		redisplay = True;
	}
	if (newbw->button.onfg != curbw->button.onfg) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.on),
			XtNfg,newbw->button.onfg,
			NULL);
		redisplay = True;
	}
	if (newbw->button.onfont != curbw->button.onfont) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.on),
			XtNfont,newbw->button.onfont,
			NULL);
		redisplay = True;
	}

	if (newbw->button.offlabel != curbw->button.offlabel) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.off),
			XtNlabel,newbw->button.offlabel,
			NULL);
		newbw->button.offlabel = newbw->button.off->label.label;
		redisplay = True;
	}
	if (newbw->button.offbg != curbw->button.offbg) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.off),
			XtNbg,newbw->button.offbg,
			NULL);
		redisplay = True;
	}
	if (newbw->button.offfg != curbw->button.offfg) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.off),
			XtNfg,newbw->button.offfg,
			NULL);
		redisplay = True;
	}
	if (newbw->button.offfont != curbw->button.offfont) {
		XoXtVaSetValues(XoGetFramedChild(newbw->button.off),
			XtNfont,newbw->button.offfont,
			NULL);
		redisplay = True;
	}
	return(redisplay);
}


static void InsertChild (w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w->core.parent;

  /* The two allowed children on and off are added by this widget's 
     Initialize procedure.  Thus we trust they will be the first two
     children added, and simply disallow any further procreation */
  
  if (bw->composite.num_children == 2) {
    String subs[2];
    Cardinal num_subs = 2;
 
    subs[0] = (caddr_t) bw->core.name;
    subs[1] = (caddr_t) w->core.name;
    
    XtAppErrorMsg (XtWidgetToApplicationContext (w),
                   "unplannedParenthood", "xoButton", "XoToolkitError",
                   "ButtonWidget '%s' cannot accomodate unplanned child '%s'",
                   subs, &num_subs);
    return;
  }
  
  if (bw->composite.num_children == 0) {
    bw->composite.children = (Widget *) malloc (2 * sizeof (Widget));
  }
  
  bw->composite.children[bw->composite.num_children++] = w;
}


static XtGeometryResult GeometryManager(child, request, reply)
     Widget child;
     XtWidgetGeometry *request, *reply;
{
  ButtonWidget bw = (ButtonWidget) child->core.parent;
  XtGeometryResult answer;

  /* Always deny a position request */
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
 
    w = request->width  + 2*request->border_width;
    h = request->height + 2*request->border_width;
    answer = XtMakeResizeRequest ((Widget) bw, w, h, &w, &h);
    if (answer == XtGeometryAlmost) {
      reply->width  = w - 2*request->border_width;
      reply->height = h - 2*request->border_width;
    }
    return answer;
  }
  
  /* must have been a stacking request */
 
  return XtGeometryYes;
}

    
/***************************************************************************
  Utilities:
  */


static void MapChild(w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w;

  XtSetMappedWhenManaged ((Widget) bw->button.on,    bw->button.state);
  XtSetMappedWhenManaged ((Widget) bw->button.off, ! bw->button.state);
}



static XrmQuark
  XtQpress, XtQhold, XtQtoggle;
 
static void XoRegisterButtonTypeQuarks ()
{
  XtQpress   = XrmStringToQuark("press");
  XtQtoggle = XrmStringToQuark("toggle");
  XtQhold  = XrmStringToQuark("hold");
}
 
/* ARGSUSED */
static Boolean 
  XoCvtStringToButtonType (dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
  static XoButtonType buttonType;
  XrmQuark q;
  char lowerName[1000];
  
  XoLowerName (lowerName, (char*)fromVal->addr);
  q = XrmStringToQuark(lowerName);
  if (q == XtQpress)
    buttonType = XoPressButton;
  else if (q == XtQhold)
    buttonType = XoHoldButton;
  else if (q == XtQtoggle)
    buttonType = XoToggleButton;
  else {
    XtStringConversionWarning(fromVal->addr, "buttonType");
    toVal->size = sizeof(XoButtonType);
    return ((Boolean)False);
  }
   XoCvtDone(XoButtonType, buttonType);
}
 
static void XoRegisterButtonTypeConverter ()
{
  XoRegisterButtonTypeQuarks();
  XtSetTypeConverter (XtRString, XtRButtonType, 
                      (XtTypeConverter) XoCvtStringToButtonType,
		      NULL, 0,
                      XtCacheNone, NULL);
}



/***************************************************************************
  Actions:
  */

static void On(w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w;

  bw->button.state = True;
  MapChild ((Widget) bw);
}

static void Off(w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w;

  bw->button.state = False;
  MapChild ((Widget) bw);
}

/*ARGUSED*/
static void Notify(w,event,params,num_params)
     Widget w;
	 XButtonEvent	*event;
	 String			*params;
	 Cardinal		*num_params;
{
  ButtonWidget bw = (ButtonWidget) w;
  static XoEventInfo	info;

  info.event  = 0;
	switch(event->button) {
		case Button1: info.event |= XoBut1 ;
			break;
		case Button2: info.event |= XoBut2 ;
			break;
		case Button3: info.event |= XoBut3 ;
			break;
	}
	info.event |= (bw->button.state) ? XoPress : XoRelease;
	info.ret="state";
	info.x=info.y=info.z = 0.0;

  XtCallCallbackList(w,bw->button.callbacks, (XtPointer)&info);
  if(bw->button.type == XoPressButton){
   	bw->button.state = False;
  	MapChild((Widget) bw);
  }
}

static void Toggle(w)
     Widget w;
{
  ButtonWidget bw = (ButtonWidget) w;

  bw->button.state = ! bw->button.state;
  MapChild ((Widget) bw);
}
