/* $Id: Form.c,v 1.2 2005/07/01 10:02:50 svitak Exp $ */
/*
 * $Log: Form.c,v $
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.18  2001/06/29 21:09:21  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.17  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.16  2000/05/02 06:18:07  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.15  1999/10/16 22:50:12  mhucka
 * Merged in changes from Venkat for supporting "xsimplot".
 *
 * Revision 1.11.1.3  1997/06/28  00:15:22  venkat
 * Changes to accomodate embedded forms - PreferredGeometry()  routine
 * modified to calculate width of a nested form.
 *
 * Revision 1.11.1.2  1997/06/20 01:19:29  venkat
 * Calls to XoResolveReference() changed to accomodate the additional
 * arguments - See Xo/XoGeometry.c logs for description.
 *
 * Revision 1.11.1.1  1995/09/26 23:43:50  venkat
 * XtVaSetValues() calls changed to XoXtVaSetValues()
 *
 * Revision 1.11  1995/04/04  00:55:37  venkat
 * Changed the realize method to not set the color specific
 * resources. This is done once for the top level shell
 *
 * Revision 1.10  1995/03/24  23:47:24  venkat
 * Changed default value of XtNbg resource to XoDefaultBackground
 * This macro is defined in XoDefs.h as the fallback or default backgrd value for the application, which is "LightSteelBlue"
 *
 * Revision 1.9  1995/03/07  00:39:33  venkat
 * Added local function SetGlobalResources() and
 * also added realize method to the form widget
 *
 * Revision 1.8  1995/03/06  19:49:14  venkat
 * Added XtRXoPixel for background resource in the
 * form resource list.
 * Modified SetValues() to set the core XtNbackground if
 * the resource value is changed.
 *
 * Revision 1.7  1995/02/24  20:19:04  venkat
 * Changed name to XoGetUsedColormap()
 *
 * Revision 1.6  1995/02/21  23:19:20  venkat
 * Fixed bug in ChangeManaged() for the
 * looping of child widgets to set their
 * colormap resource
 *
 * Revision 1.5  1995/02/21  01:20:09  venkat
 * Added code in ChangeManaged() to implement short-term
 * workaround for the xcolorscale problem
 *
 * Revision 1.4  1995/01/20  23:44:47  venkat
 * Fixed typo in ConstraintInitialize()
 * XoGeometryIsUnset(form.w_geometry) changed to XoGeometryIsUnset(&form.w_geometry)
 *
 * Revision 1.3  1995/01/19  18:52:21  venkat
 * Changed defaults for the w_geometry in ConstraintInitialize()
 * Fixed some typos in the same function and ConstraintSetValues()
 *
 * Revision 1.2  1994/03/22  15:22:05  bhalla
 * Added Form resize callbacks.
 * Put in stuff so form resizes when new widgets are added.
 * Still havent figured out how to get the last child to be framed correctly
 * */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "FramedP.h"
#include "FormP.h"
#include "Xo/XoDefs.h"
#include <stdio.h>		/* for debugging */


/* Private Definitions */

static int
  defCoord    = -9999;
static XoRequestType 
  defRequest  = (XoRequestType) -9999;
static XoReference 
  defReference = (Widget) -9999;
static XoReferenceEdge 
  defRefEdge = XoReferToDefault;
static void DefaultGeometryProc();

#define Offset(field) XtOffset(FormConstraints, form.field)
static XtResource formConstraintResources[] = {
  {XtNxRequested, XtCGeometryRequest, XtRGeometryRequest, sizeof(int),
     Offset(x_requested), XtRGeometryRequest, (XtPointer)&defCoord},
  {XtNyRequested, XtCGeometryRequest, XtRGeometryRequest, sizeof(int),
     Offset(y_requested), XtRGeometryRequest, (XtPointer)&defCoord},
  {XtNwRequested, XtCGeometryRequest, XtRGeometryRequest, sizeof(int),
     Offset(w_requested), XtRGeometryRequest, (XtPointer)&defCoord},
  {XtNhRequested, XtCGeometryRequest, XtRGeometryRequest, sizeof(int),
     Offset(h_requested), XtRGeometryRequest, (XtPointer)&defCoord},
  {XtNxRequestType, XtCRequestType, XtRRequestType, sizeof(XoRequestType),
     Offset(x_request_type), XtRRequestType, (XtPointer)&defRequest},
  {XtNyRequestType, XtCRequestType, XtRRequestType, sizeof(XoRequestType),
     Offset(y_request_type), XtRRequestType, (XtPointer)&defRequest},
  {XtNwRequestType, XtCRequestType, XtRRequestType, sizeof(XoRequestType),
     Offset(w_request_type), XtRRequestType, (XtPointer)&defRequest},
  {XtNhRequestType, XtCRequestType, XtRRequestType, sizeof(XoRequestType),
     Offset(h_request_type), XtRRequestType, (XtPointer)&defRequest},
  {XtNxReference, XtCReference, XtRReference, sizeof(XoReference),
     Offset(x_reference), XtRReference, (XtPointer)&defReference},
  {XtNyReference, XtCReference, XtRReference, sizeof(XoReference),
     Offset(y_reference), XtRReference, (XtPointer)&defReference},
  {XtNwReference, XtCReference, XtRReference, sizeof(XoReference),
     Offset(w_reference), XtRReference, (XtPointer)&defReference},
  {XtNhReference, XtCReference, XtRReference, sizeof(XoReference),
     Offset(h_reference), XtRReference, (XtPointer)&defReference},
  {XtNxReferenceEdge, XtCReferenceEdge, XtRReferenceEdge, 
     sizeof(XoReferenceEdge), Offset(x_reference_edge),
     XtRReferenceEdge, (XtPointer)&defRefEdge},
  {XtNyReferenceEdge, XtCReferenceEdge, XtRReferenceEdge, 
     sizeof(XoReferenceEdge), Offset(y_reference_edge),
     XtRReferenceEdge, (XtPointer)&defRefEdge},
  {XtNwReferenceEdge, XtCReferenceEdge, XtRReferenceEdge, 
     sizeof(XoReferenceEdge), Offset(w_reference_edge),
     XtRReferenceEdge, (XtPointer)&defRefEdge},
  {XtNhReferenceEdge, XtCReferenceEdge, XtRReferenceEdge, 
     sizeof(XoReferenceEdge), Offset(h_reference_edge),
     XtRReferenceEdge, (XtPointer)&defRefEdge},
  {XtNxGeometry, XtCXoGeometry, XtRXoGeometry, sizeof (XoGeometryRec),
     Offset(x_geometry), XtRCallProc, (XtPointer) DefaultGeometryProc},
  {XtNyGeometry, XtCXoGeometry, XtRXoGeometry, sizeof (XoGeometryRec),
     Offset(y_geometry), XtRCallProc, (XtPointer) DefaultGeometryProc},
  {XtNwGeometry, XtCXoGeometry, XtRXoGeometry, sizeof (XoGeometryRec),
     Offset(w_geometry), XtRCallProc, (XtPointer) DefaultGeometryProc},
  {XtNhGeometry, XtCXoGeometry, XtRXoGeometry, sizeof (XoGeometryRec),
     Offset(h_geometry), XtRCallProc, (XtPointer) DefaultGeometryProc},
};
#undef Offset

#define Offset(field) XtOffset(FormWidget, form.field)
static XtResource formResources[] = {
	{ XtNauto_resize, XtCAutoResize, XtRInt, sizeof (int),
       Offset(auto_resize), XtRString, (XtPointer) "0"},
	{ XtNcallback, XtCCallback, XtRCallback, sizeof (XtPointer),
       Offset(callbacks), XtRCallback, (XtPointer) NULL},
	{ XtNbg, XtCBackground, XtRXoPixel, sizeof (Pixel),
       Offset(bg), XtRString, (XtPointer) XoDefaultBackground },
};
#undef Offset
/* Methods */
static void ClassInitialize(), ClassPartInitialize(), Initialize(), Resize();
static void ConstraintInitialize();
static Boolean SetValues(), ConstraintSetValues();
static XtGeometryResult GeometryManager(), PreferredGeometry();
static void ChangeManaged();
static void InsertChild();
static void XoVertSizeForm();
static Boolean Layout();
static void Realize();

/* Local functions */
static void SetGlobalResources();

FormClassRec formClassRec = {
  { /* core_class fields */
    (WidgetClass) &constraintClassRec, /* superclass         */    
    "Form",			       /* class_name         */    
    sizeof(FormRec),		       /* widget_size        */    
    ClassInitialize,                   /* class_initialize   */
    ClassPartInitialize,	       /* class_part_init    */
    FALSE,			       /* class_inited       */
    Initialize,			       /* initialize         */
    NULL,			       /* initialize_hook    */
    Realize,			       /* realize            */
    NULL,			       /* actions            */
    0,				       /* num_actions        */
    formResources,		       /* resources          */
    XtNumber(formResources),   /* num_resources      */
    NULLQUARK,			       /* xrm_class          */
    TRUE,			       /* compress_motion    */
    TRUE,			       /* compress_exposure  */
    TRUE,			       /* compress_enterleave*/
    FALSE,			       /* visible_interest   */
    NULL,			       /* destroy            */
    Resize,			       /* resize             */
    XtInheritExpose,		       /* expose             */
    SetValues,			       /* set_values         */
    NULL,			       /* set_values_hook    */
    XtInheritSetValuesAlmost,	       /* set_values_almost  */
    NULL,			       /* get_values_hook    */
    NULL,			       /* accept_focus       */
    XtVersion,			       /* version            */
    NULL,			       /* callback_private   */
    NULL,			       /* tm_table           */
    PreferredGeometry,	               /* query_geometry     */
    XtInheritDisplayAccelerator,       /* display_accelerator*/
    NULL			       /* extension          */
    },
  { /* composite_class fields */
    GeometryManager,                   /* geometry_manager   */   
    ChangeManaged,                     /* change_managed     */   
    InsertChild,		               /* insert_child       */   
    XtInheritDeleteChild,              /* delete_child       */   
    NULL		               /* extension          */   
    },
  { /* constraint_class fields */
    formConstraintResources,           /* subresourses       */   
    XtNumber(formConstraintResources), /* subresource_count  */   
    sizeof(FormConstraintsRec),	       /* constraint_size    */   
    ConstraintInitialize,	       /* initialize         */   
    NULL,			       /* destroy            */   
    ConstraintSetValues,	       /* set_values         */   
    NULL			       /* extension          */   
    },
  { /* form_class fields */
    Layout	                       /* layout             */   
    }
};

WidgetClass formWidgetClass = (WidgetClass)&formClassRec;
     
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/
     
     
static void ClassInitialize()
{
  XoGeomInitialize();
}

static void ClassPartInitialize(class)
     WidgetClass class;
{
  register FormWidgetClass c = (FormWidgetClass)class;
  
  if (c->form_class.layout == XtInheritLayout)
    c->form_class.layout = Layout;
}


/* ARGSUSED */
static void Initialize(request, new)
     Widget request, new;
{
  FormWidget fw = (FormWidget)new;
  fw->form.no_refigure = False;
  fw->form.needs_relayout = False;
  fw->form.resize_in_layout = True;
}


static void RefigureLocations(w)
     FormWidget w;
{
  if (w->form.no_refigure) {
    w->form.needs_relayout = True;
  }
  else {
    (*((FormWidgetClass)w->core.widget_class)->form_class.layout)
      ( w, w->core.width, w->core.height );
    w->form.needs_relayout = False;
  }
}

/* ARGSUSED */
static Boolean Layout(fw, width, height)
     FormWidget fw;
     Dimension width, height;
{
	return(_XoLayoutForm(fw));
}

static void Realize(w, valueMask, attributes)
 Widget w;
 Mask *valueMask;
 XSetWindowAttributes *attributes;
{
/*  SetGlobalResources(w); */
 (*formWidgetClass->core_class.superclass->core_class.realize)
	(w,valueMask,attributes);
}

static void Resize(w)
     Widget w;
{
  static XoEventInfo	info;
  FormWidget fw = (FormWidget)w;
  RefigureLocations (fw);

  info.event = XoUpdateFields;
  info.ret = "xgeom ygeom hgeom wgeom";
  info.x = info.y = info.z = 0.0;
  XtCallCallbackList(w,fw->form.callbacks, (XtPointer)&info);
}


static void SetGlobalResources(w)
 Widget w;
{
 XoXtVaSetValues(w,XtNcolormap,XoGetUsedColormap(),NULL);
}

/* ARGSUSED */
static XtGeometryResult GeometryManager(w, request, reply)
     Widget w;
     XtWidgetGeometry *request;
     XtWidgetGeometry *reply;	/* RETURN */
{
  /* Allow changes only if the requested constraint is XoNaturalGeometry */
  FormConstraints form = (FormConstraints)w->core.constraints;
  XtWidgetGeometry allowed;
  
  if ((request->request_mode & 
       ~(XtCWQueryOnly | CWWidth | CWHeight | CWX | CWY)))
    return XtGeometryNo;
  
  if (request->request_mode & CWX && 
      form->form.x_requested == XoNaturalGeometry)
    allowed.x = request->x;
  else
    allowed.x = w->core.x;

  if (request->request_mode & CWY && 
      form->form.y_requested == XoNaturalGeometry)
    allowed.y = request->y;
  else
    allowed.y = w->core.y;

  if (request->request_mode & CWWidth &&
      form->form.w_requested == XoNaturalGeometry)
    allowed.width = request->width;
  else
    allowed.width = w->core.width;
  
  if (request->request_mode & CWHeight &&
      form->form.h_requested == XoNaturalGeometry)
    allowed.height = request->height;
  else
     allowed.height = w->core.height;
  
  if (allowed.x == w->core.x && allowed.y == w->core.y &&
      allowed.width == w->core.width && allowed.height == w->core.height)
    return XtGeometryNo;
  
  if (!(request->request_mode & XtCWQueryOnly)) {
    XtConfigureWidget (w, allowed.x, allowed.y, allowed.width, allowed.height,
		          w->core.border_width);
    RefigureLocations( (FormWidget)w->core.parent );
    return XtGeometryDone;
  }
  return XtGeometryYes;
}



/* ARGSUSED */
static Boolean SetValues(current, request, new)
     Widget current, request, new;
{
  FormWidget curfw = (FormWidget) current;
  FormWidget newfw = (FormWidget) new;
   
  if(curfw->form.bg != newfw->form.bg){
    XoXtVaSetValues(new, XtNbackground, newfw->form.bg, NULL);
    return(TRUE);
  }
  return( FALSE );
}

/* ARGSUSED */
static void DefaultGeometryProc (widget, offset, value)
     Widget       widget;
     int          offset;
     XrmValuePtr  value;
{
  XoGeometryPtr deflt;
  

  deflt = XoGeometryMakeUnsetRecord();
  value->addr = (XtPointer) deflt;
  value->size = sizeof (XoGeometryRec);
}

/* ARGSUSED */
static void ConstraintInitialize(request, new)
     Widget request, new;
{
  FormConstraints child_constraints = (FormConstraints)new->core.constraints;
  FormConstraintsPart form;

  int refDestroyed;

  form = child_constraints->form;

  /* x: */
  /* check to see if application programmer has not set any geometry fields */
  if (form.x_requested == defCoord && form.x_request_type == defRequest &&
      form.x_reference == defReference && form.x_reference_edge == defRefEdge) {
    if ( ! XoGeometryIsUnset(&form.x_geometry)) {
      form.x_requested = form.x_geometry.request;
      form.x_request_type = form.x_geometry.requestType;
      form.x_reference = form.x_geometry.reference;
      form.x_reference_edge = form.x_geometry.referenceEdge;
    } else {
      form.x_requested = 0;
      form.x_request_type = XoAbsoluteRequest;
      form.x_reference = XoReferToParent;
      form.x_reference_edge = XoReferToDefault;
    }
  }

  if (form.x_requested == defCoord) form.x_requested = (int) new->core.x;
  if (form.x_request_type == defRequest) form.x_request_type = XoAbsoluteRequest;
  if (form.x_reference == defReference) form.x_reference = XoReferToParent;
  if (form.x_reference_edge == XoReferToDefault)
    form.x_reference_edge = 
      (XoResolveReference (new, form.x_reference, &refDestroyed, (XoReference)XoReferToParent) == new->core.parent) 
	? XoReferToLeft : XoReferToRight;


  /* y: */
  /* check to see if application programmer has not set any geometry fields */
  if (form.y_requested == defCoord && form.y_request_type == defRequest &&
      form.y_reference == defReference && form.y_reference_edge == defRefEdge) {
    if ( ! XoGeometryIsUnset(&form.y_geometry)) {
      form.y_requested = form.y_geometry.request;
      form.y_request_type = form.y_geometry.requestType;
      form.y_reference = form.y_geometry.reference;
      form.y_reference_edge = form.y_geometry.referenceEdge;
    } else {
      form.y_requested = 0;
      form.y_request_type = XoAbsoluteRequest;
      form.y_reference = XoReferToLastChild;
      form.y_reference_edge = XoReferToDefault;
    }
  }

  if (form.y_requested == defCoord) form.y_requested = (int) new->core.y;
  if (form.y_request_type == defRequest) form.y_request_type = XoAbsoluteRequest;
  if (form.y_reference == defReference) form.y_reference = XoReferToParent;
  if (form.y_reference_edge == XoReferToDefault)
    form.y_reference_edge = 
      (XoResolveReference (new, form.y_reference, &refDestroyed, (XoReference)XoReferToParent) == new->core.parent) 
	? XoReferToTop : XoReferToBottom;


  /* w: */
  /* check to see if application programmer has not set any geometry fields */
  if (form.w_requested == defCoord && form.w_request_type == defRequest &&
      form.w_reference == defReference && form.w_reference_edge == defRefEdge) {
    if ( ! XoGeometryIsUnset(&form.w_geometry) ) {
      form.w_requested = form.w_geometry.request;
      form.w_request_type = form.w_geometry.requestType;
      form.w_reference = form.w_geometry.reference;
      form.w_reference_edge = form.w_geometry.referenceEdge;
    } else {
	/* The default for the child width is to fill 
	  the entire width of the parent */
      form.w_requested = 100;
      form.w_request_type = XoRelativeRequest;
      form.w_reference = XoReferToParent;
      form.w_reference_edge = XoReferToDefault;
    }
  }

  if (form.w_requested == defCoord) form.w_requested = (int) new->core.width;
  if (form.w_request_type == defRequest) 
    form.w_request_type = XoRelativeRequest;
  if (form.w_reference == defReference) form.w_reference = XoReferToParent;
  if (form.w_reference_edge == XoReferToDefault) 
    form.w_reference_edge = XoReferToWidth;


  /* h: */
  /* check to see if application programmer has not set any geometry fields */
  if (form.h_requested == defCoord && form.h_request_type == defRequest &&
      form.h_reference == defReference && form.h_reference_edge == defRefEdge) {
    if ( ! XoGeometryIsUnset(&(child_constraints->form.h_geometry))) {
      form.h_requested = form.h_geometry.request;
      form.h_request_type = form.h_geometry.requestType;
      form.h_reference = form.h_geometry.reference;
      form.h_reference_edge = form.h_geometry.referenceEdge;
    } else {
      form.h_requested = XoNaturalGeometry;
      form.h_request_type = XoAbsoluteRequest;
      form.h_reference = XoReferToParent;
      form.h_reference_edge = XoReferToHeight;
    }
  }

  if (form.h_requested == defCoord) form.h_requested =
	 (int) new->core.height;
  if (form.h_request_type == defRequest) form.h_request_type = XoAbsoluteRequest;
  if (form.h_reference == defReference) form.h_reference = XoReferToParent;
  if (form.h_reference_edge == XoReferToDefault)
    form.h_reference_edge = XoReferToHeight;

  child_constraints->form = form;
}

int DiffGeom(x,y)
	XoGeometryRec *x,*y;
{
	if (x->request != y->request) return(1);
	if (x->requestType != y->requestType) return(1);
	if (x->reference != y->reference) return(1);
	if (x->referenceEdge != y->referenceEdge) return(1);
	return(0);
}

/* ARGSUSED */
static Boolean ConstraintSetValues(current, request, new)
     Widget current, request, new;
{
#define UPIHACK
#ifdef UPIHACK
  FormConstraints child_constraints = (FormConstraints)new->core.constraints;
  FormConstraintsPart form,currform;
  int	changed=0;
  int	vert_changed = 0;

  int refDestroyed;

  /* printf("In ConstraintSetValues\n"); */

  form = child_constraints->form;
  currform=((FormConstraints)current->core.constraints)->form;

  /* x: see if xgeom has changed */
    if (DiffGeom(&form.x_geometry,&currform.x_geometry)) {
      form.x_requested = form.x_geometry.request;
      form.x_request_type = form.x_geometry.requestType;
      form.x_reference = form.x_geometry.reference;
      form.x_reference_edge = form.x_geometry.referenceEdge;

  if (form.x_requested == defCoord) form.x_requested = (int) new->core.x;
  if (form.x_request_type == defRequest) form.x_request_type = XoAbsoluteRequest;
  if (form.x_reference == defReference) form.x_reference = XoReferToParent;
  if (form.x_reference_edge == XoReferToDefault)
    form.x_reference_edge = 
      (XoResolveReference (new, form.x_reference, &refDestroyed, (XoReference)XoReferToParent) == new->core.parent) 
	? XoReferToLeft : XoReferToRight;
	  changed = 1;
    }


  /* y: see if ygeom has changed */
    if (DiffGeom(&form.y_geometry,&currform.y_geometry)) {
      form.y_requested = form.y_geometry.request;
      form.y_request_type = form.y_geometry.requestType;
      form.y_reference = form.y_geometry.reference;
      form.y_reference_edge = form.y_geometry.referenceEdge;

  if (form.y_requested == defCoord) form.y_requested = (int) new->core.y;
  if (form.y_request_type == defRequest) form.y_request_type = XoAbsoluteRequest;
  if (form.y_reference == defReference) form.y_reference = XoReferToParent;
  if (form.y_reference_edge == XoReferToDefault)
    form.y_reference_edge = 
      (XoResolveReference (new, form.y_reference, &refDestroyed, (XoReference)XoReferToParent) == new->core.parent) 
	? XoReferToTop : XoReferToBottom;
	  changed = 1;
	  vert_changed = 1;
    } 


  /* w: see if wgeom has changed */
    if (DiffGeom(&form.w_geometry,&currform.w_geometry)) {
      form.w_requested = form.w_geometry.request;
      form.w_request_type = form.w_geometry.requestType;
      form.w_reference = form.w_geometry.reference;
      form.w_reference_edge = form.w_geometry.referenceEdge;

  if (form.w_requested == defCoord) form.w_requested = (int) new->core.width;
  if (form.w_request_type == defRequest) 
    form.w_request_type = XoRelativeRequest;
  if (form.w_reference == defReference) form.w_reference = XoReferToParent;
  if (form.w_reference_edge == XoReferToDefault) 
    form.w_reference_edge = XoReferToWidth;
	  changed = 1;
    }


  /* h: see if hgeom has changed */
    if (DiffGeom(&form.h_geometry,&currform.h_geometry)) {
      form.h_requested = form.h_geometry.request;
      form.h_request_type = form.h_geometry.requestType;
      form.h_reference = form.h_geometry.reference;
      form.h_reference_edge = form.h_geometry.referenceEdge;

  if (form.h_requested == defCoord) form.h_requested = 
    (int) new->core.height;
  if (form.h_request_type == defRequest) form.h_request_type = XoAbsoluteRequest;
  if (form.h_reference == defReference) form.h_reference = XoReferToParent;
  if (form.h_reference_edge == XoReferToDefault)
    form.h_reference_edge = XoReferToHeight;
	  changed = 1;
	  vert_changed = 1;
    }
	if (!changed)  return False;

	child_constraints->form = form; 
	RefigureLocations ( (FormWidget)new->core.parent );
	if (vert_changed &&
		((FormWidget)new->core.parent)->form.auto_resize)
		XoVertSizeForm(new->core.parent);
	return True;
#else
/*  RefigureLocations ( (FormWidget)current->core.parent ); */
  return False;
#endif
}

static void ChangeManaged(w)
     Widget w;
{
 /*
  FormWidget fw = (FormWidget)w;
  WidgetList children = fw->composite.children;
  int num_children = fw->composite.num_children;
  Widget *childP; 
  for(childP=children; childP-children < num_children; childP++){
   if(!XtIsRealized(*childP))
	XoXtVaSetValues(*childP, XtNcolormap, XoGetUsedColormap(), NULL);
  }
 */
  RefigureLocations( (FormWidget)w );
}

static void InsertChild(w)
	Widget w;
{
    FormWidget fw = (FormWidget) w->core.parent;
 
    /* realloc space for the children every 10th time */
    if (fw->composite.num_children == 0)
        fw->composite.children = NULL;
    if (fw->composite.num_children%10 == 0)
        fw->composite.children =
            (Widget *)XtRealloc((char *) fw->composite.children,
            (10 + 10 * (fw->composite.num_children/10)) *
            sizeof(Widget));
 
    fw->composite.children[fw->composite.num_children] = w;
    fw->composite.num_children++;

    RefigureLocations (fw);
/* Check if we need to resize the form */
	if (fw->form.auto_resize)
		XoVertSizeForm(fw);
}


static XtGeometryResult PreferredGeometry( widget, request, reply  )
     Widget widget;
     XtWidgetGeometry *request, *reply;
{
Dimension default_width, default_height;
  FormWidget w = (FormWidget)widget;
  FramedWidget fW = (FramedWidget)XtParent(widget);
  Widget parentW = XtParent(XtParent(widget));
  reply->request_mode = CWWidth | CWHeight;
  if (!XtIsSubclass(parentW,formWidgetClass)){
        reply->width = (Dimension) w->core.width;
        reply->height = (Dimension) w->core.height;
        if ((request->request_mode & (CWWidth | CWHeight)) ==
	    (reply->request_mode & (CWWidth | CWHeight))
	    && request->width == reply->width
	    && request->height == reply->height)
                 return XtGeometryYes;
        else
                 return XtGeometryNo;
  }
  default_width = parentW->core.width - 2*parentW->core.border_width;
  default_height = 0.5 * (parentW->core.height - 2*parentW->core.border_width);
  reply->width = default_width - 2*fW->frame.frame_width;
  reply->height = default_height - 2*fW->frame.frame_width;
  if( ((request->request_mode && (CWWidth | CWHeight))
        == (CWWidth | CWHeight)) &&
        request->width == reply->width &&
        request->height == reply->height)
    return XtGeometryYes;
  else if (reply->width == w->core.width &&
         reply->height == w->core.height)
    return XtGeometryNo;
  else
    return XtGeometryAlmost;
}


/**********************************************************************
 *
 * Public routines
 *
 **********************************************************************/

/* Toggle whether to do layout updates whenever necessary.  This is
 * useful when layout parameters are about to be set repeatedly.  If
 * updating is switched on, the widget is realized and the form needs
 * to be refigured then the layout is done.  */

void XoFormToggleUpdate(w, state)
     Widget w;
     Boolean state;
{
  FormWidget fw = (FormWidget)w;
  
  fw->form.no_refigure = !state;
  
  if ( state && fw->form.needs_relayout && XtIsRealized(w) )
    RefigureLocations( fw );
}


#define max(x,y) ((x)<(y)?(y):(x))
void XoSizeForm(w)
     Widget w;
{
  FormWidget fw = (FormWidget)w;
  int num_children = fw->composite.num_children;
  WidgetList children = fw->composite.children;
  Widget *childP;
  int maxx = 1, maxy = 1;

  RefigureLocations (fw);
  for (childP = children; childP - children < num_children; childP++) {
    maxx = max (maxx,
		(*childP)->core.x + (Position)((*childP)->core.width)
		          + 2 * (Position)((*childP)->core.border_width));
    maxy = max (maxy,
		(*childP)->core.y + (Position)((*childP)->core.height)
		          + 2 * (Position)((*childP)->core.border_width));
  }
  XtResizeWidget (w, maxx, maxy, w->core.border_width);
}

static void XoVertSizeForm(w)
     Widget w;
{
  FormWidget fw = (FormWidget)w;
  int num_children = fw->composite.num_children;
  WidgetList children = fw->composite.children;
  Widget *childP;
  int maxx = 1, maxy = 1;
  Dimension retw,reth;

  for (childP = children; childP - children < num_children; childP++) {
    maxx = max (maxx,
		(*childP)->core.x + (Position)((*childP)->core.width)
		          + 2 * (Position)((*childP)->core.border_width));
    maxy = max (maxy,
		(*childP)->core.y + (Position)((*childP)->core.height)
		          + 2 * (Position)((*childP)->core.border_width));
  }
  if (maxy > (int)w->core.height) {
  	XtMakeResizeRequest (w, w->core.width, (Dimension)maxy,&retw,&reth);
	if (retw >= w->core.width && reth >= w->core.height)
  		XtMakeResizeRequest (w, retw, reth, &retw,&reth);
  }
}
