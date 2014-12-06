/* $Id: FormLayout.c,v 1.2 2005/07/01 10:02:50 svitak Exp $ */
/*
 * $Log: FormLayout.c,v $
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/07/03 18:18:21  mhucka
 * Removed extra function declaration that was annoying the compiler.
 *
 * Revision 1.6  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  1997/06/20 01:17:58  venkat
 * Calls to XoResolveReference() changed to accomodate the additional
 * arguments - See Xo/XoGeometry.c logs for description.
 *
 * Revision 1.4  1995/01/31 18:52:55  venkat
 * Changed _XoCalculateW() and _XoCalculateH() to calculate the
 * working width and height as the max(value, 1) which honours a
 * width/height request of 0 pixels.
 *
 * Revision 1.3  1995/01/17  23:08:19  venkat
 * Fixed _XoCalculateY() with y_working calculated in terms of
 * the height of the reference rather than the width.
 *
 * Revision 1.2  1994/12/16  20:49:37  venkat
 * Added _XoParamLoopWarning() to make genesis just print warning
 * message and not exit on x-geometry references of a child widget to another
 *
 * Revision 1.1  1994/03/22  15:25:06  bhalla
 * Initial revision
 * */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "FormP.h"

#define max(x,y) ((x)<(y)?(y):(x))

/* This file contains the layout routine for the xodus form widget.  It has *\
 * been moved here so as to make the code in the actual widget file Form.c  *
\* somewhat more readable.                                                  */


static void _XoCalculateX  ();
static void _XoCalculateY  ();
static void _XoCalculateW  ();
static void _XoCalculateH  ();
static void _XoLayoutChild ();


Boolean _XoLayoutForm (fw)
     FormWidget fw;
     /* Calculate the positions for all children in the form.  Since widgets
	may be chained so as to make a horizontal dimension or coordinate of 
	one depend on a vertical dimension or coordinate of another, we have
	to refigure the positions of all the children at each and every layout.
	We are careful to only call XtConfigureWidget() if the widget has
	changed, though. */
{
  int num_children = fw->composite.num_children;
  WidgetList children = fw->composite.children;
  Widget *childP;
  Boolean ret_val = False;

  /* printf("In _XoLayoutForm\n"); */
  

  for (childP = children; childP - children < num_children; childP++) {
    FormConstraints form = (FormConstraints)(*childP)->core.constraints;
    form->form.layout_state = LayoutPending;
  }

  fw->form.layout_state = LayoutInProgress;

  for (childP = children; childP - children < num_children; childP++) {
    if (XtIsManaged(*childP)) {
      _XoLayoutChild(*childP);
    }
  }
  return ret_val;
}


/*****************************
  _XoLayoutChild():

  Functions to calculate the position of a particular child widget
  from the geometry specifications.
*/


/* Use up the top three shorts for special values.  
   No one wants a parameter like 32767 anyway ! */

#define XoUnsetParameter    ((1 << sizeof (Position) * 8 - 1) - 1)
#define XoPendingParameter  ((1 << sizeof (Position) * 8 - 1) - 2) 
#define XoParameterLoop     ((1 << sizeof (Position) * 8 - 1) - 3)


/* The following is basically a huge list of special cases.  While I have
   tried to pare it down as far as possible (in particular, through judicious
   use of the ?: construct) most of the ugliness is unavoidable */



static void _XoParamLoopError (child)
     Widget child;
{
  String subs[2];
  Cardinal num_subs = 2;
  
  subs[0] = child->core.name;
  subs[1] = child->core.parent->core.name;
  XtAppErrorMsg (XtWidgetToApplicationContext(child),
		 "parameterLoop", "xoFormLayout","XoToolkitError",
		 "parameter loop detected while laying out child '%s' in '%s'",
		 subs, &num_subs);
}  

static void  _XoParamLoopWarning (child)
     Widget child;
{
  String subs[2];
  Cardinal num_subs = 2;
  
  subs[0] = child->core.name;
  subs[1] = child->core.parent->core.name;
  XtAppWarningMsg(XtWidgetToApplicationContext(child),
		  "parameterLoop","xoFormLayout","XoToolkitError",
		  "parameter loop detected while laying out child '%s' in '%s'",
		  subs, &num_subs);
}

static void _XoConstraintLoopWarning (child)
     Widget child;
{
  String subs[2];
  Cardinal num_subs = 2;
  
  subs[0] = child->core.name;
  subs[1] = child->core.parent->core.name;
  XtAppWarningMsg(XtWidgetToApplicationContext(child),
		  "constraintLoop","xoFormLayout","XoToolkitError",
		  "constraint loop detected while laying out child '%s' in '%s'",
		  subs, &num_subs);
}


static void _XoCalculateX (child)
     Widget child;
{
  int                req;       
  XoRequestType      reqType;  
  XoReferenceEdge    refEdge; 
  Boolean            referToParent;
  Boolean            relativeReq; 

  FormConstraints constraints = (FormConstraints)child->core.constraints;
  Widget ref       = constraints->form.x_reference;

  int refDestroyed;

  ref = XoResolveReference (child, ref, &refDestroyed, (XoReference) XoReferToParent);

  if (refDestroyed) {
  	XoGeometryRec	geometryRec; 	
  	geometryRec.request=0;
  	geometryRec.requestType=XoAbsoluteRequest;
  	geometryRec.reference = XoReferToParent;
  	geometryRec.referenceEdge = XoReferToDefault;
  	XoXtVaSetValues(child, XtNxGeometry, &geometryRec, NULL);
  }

  constraints = (FormConstraints)child->core.constraints;
  req       = constraints->form.x_requested;
  reqType   = constraints->form.x_request_type;
  refEdge   = constraints->form.x_reference_edge;
  relativeReq   = (reqType == XoRelativeRequest);


/*  printf("Calculating x : req=%d,relativeReq=%d\n",
	req,relativeReq); */
  switch (constraints->form.x_working) {
  case XoUnsetParameter:
    constraints->form.x_working = XoPendingParameter;
    break;
  case XoPendingParameter:
   /* _XoParamLoopError (child);*/
    _XoParamLoopWarning (child);
    constraints->form.x_working = XoParameterLoop;
    return;
  default:
    return;
  }

  if (req == XoNaturalGeometry) {
    XtWidgetGeometry pref;
    XtQueryGeometry (child, NULL, &pref);
    constraints->form.x_working = pref.x;
    return;
  }




  referToParent = (ref == child->core.parent);

  if (! referToParent) 
    _XoLayoutChild (ref);
  
  req = relativeReq ? req * (int)ref->core.width / 100 : req;

  switch (refEdge) {

  case XoReferToLeft:

    if (referToParent)
      constraints->form.x_working = req;
    else {
      _XoCalculateW (child);
      constraints->form.x_working = ref->core.x - req
	- constraints->form.w_working - 2 * child->core.border_width;
    }
    break;

  case XoReferToRight:

    if (referToParent) {
      _XoCalculateW (child);
      constraints->form.x_working = ref->core.width - req 
	- constraints->form.w_working - 2 * child->core.border_width;
    } else 
      constraints->form.x_working = req 
	+ ref->core.x + ref->core.width + 2 * ref->core.border_width;
    break;

  case XoReferToBottom:
  case XoReferToTop:
  default:
    break;
  }
}

static void _XoCalculateY (child)
     Widget child;
{
  int                req;       
  XoRequestType      reqType;  
  XoReferenceEdge    refEdge; 
  Boolean            referToParent;
  Boolean            relativeReq; 

  FormConstraints constraints = (FormConstraints)child->core.constraints;
  Widget             ref       = constraints->form.y_reference;
	
  int refDestroyed;

  ref = XoResolveReference (child, ref, &refDestroyed, (XoReference)XoReferToLastChild);
  if (refDestroyed) {
  	XoGeometryRec	geometryRec; 	
	geometryRec.request=0;
	geometryRec.requestType=XoAbsoluteRequest;
	geometryRec.reference = XoReferToLastChild;
	geometryRec.referenceEdge = XoReferToDefault;
  	XoXtVaSetValues(child, XtNyGeometry, &geometryRec, NULL);
  }

  constraints = (FormConstraints)child->core.constraints;
  req       = constraints->form.y_requested;
  reqType   = constraints->form.y_request_type;
  refEdge   = constraints->form.y_reference_edge;
  relativeReq   = (reqType == XoRelativeRequest);


  switch (constraints->form.y_working) {
  case XoUnsetParameter:
    constraints->form.y_working = XoPendingParameter;
    break;
  case XoPendingParameter:
    _XoParamLoopError (child);
    constraints->form.y_working = XoParameterLoop;
    return;
  default:
    return;
  }

  if (req == XoNaturalGeometry) {
    XtWidgetGeometry pref;
    XtQueryGeometry (child, NULL, &pref);
    constraints->form.y_working = pref.y;
    return;
  }



  referToParent = ref == child->core.parent;

  if (! referToParent) 
    _XoLayoutChild (ref);
  
  req = relativeReq ? req * (int)ref->core.height/ 100 : req;

  switch (refEdge) {

  case XoReferToTop:

    if (referToParent)
      constraints->form.y_working = req;
    else {
      _XoCalculateH (child);
      constraints->form.y_working = ref->core.y - req
	- constraints->form.h_working - 2 * child->core.border_width;
    }
    break;

  case XoReferToBottom:

    if (referToParent) {
      _XoCalculateH (child);
      constraints->form.y_working = ref->core.height - req 
	- constraints->form.h_working - 2 * child->core.border_width;
    } else 
      constraints->form.y_working = req 
	+ ref->core.y + ref->core.height + 2 * ref->core.border_width;
    break;

  case XoReferToRight:
  case XoReferToLeft:
  default:
    break;
  }
}

static void _XoCalculateW (child)
     Widget child;
{
  int                req;       
  XoRequestType      reqType;  
  XoReferenceEdge    refEdge; 
  Boolean            referToParent;
  Boolean            relativeReq; 

  FormConstraints constraints = (FormConstraints) child->core.constraints;
  Widget ref = constraints->form.w_reference;

  int refDestroyed;

  ref = XoResolveReference (child, ref, &refDestroyed, (XoReference)XoReferToParent);
  if (refDestroyed) {
  	XoGeometryRec	geometryRec; 	
	geometryRec.request=100;
	geometryRec.requestType=XoRelativeRequest;
	geometryRec.reference = XoReferToParent;
	geometryRec.referenceEdge = XoReferToDefault;
  	XoXtVaSetValues(child, XtNwGeometry, &geometryRec, NULL);
  }

  constraints = (FormConstraints) child->core.constraints;
  req       = constraints->form.w_requested;
  reqType   = constraints->form.w_request_type;
  refEdge   = constraints->form.w_reference_edge;
  relativeReq   = (Boolean) (reqType == XoRelativeRequest);
  
  switch (constraints->form.w_working) {
  case XoUnsetParameter:
    constraints->form.w_working = XoPendingParameter;
    break;
  case XoPendingParameter:
    _XoParamLoopError (child);
    constraints->form.w_working = XoParameterLoop;
    return;
  default:
    return;
  }

  if (req == XoNaturalGeometry) {
    XtWidgetGeometry pref;
    XtQueryGeometry (child, NULL, &pref);
    constraints->form.w_working = pref.width;
    return;
  }


  referToParent = ref == child->core.parent;

  if (!referToParent) 
    _XoLayoutChild (ref);

  switch (refEdge) {

  case XoReferToWidth:
    constraints->form.w_working =
		relativeReq ? (int)ref->core.width * req / 100:
                      (referToParent ? 0 : ref->core.width) + req;
    break;

  case XoReferToHeight:
    constraints->form.w_working =
		relativeReq ? (int)ref->core.height * req / 100:
        ref->core.height + (referToParent ? -1 : 1) * req;
    break;

  case XoReferToLeft:
    req = relativeReq ? req * (int)ref->core.width / 100 : req;
    _XoCalculateX (child);
    constraints->form.w_working = (referToParent ? req : ref->core.x - req)
	    - constraints->form.x_working - 2 * child->core.border_width;
    break;

  case XoReferToRight:
    req = relativeReq ? req * (int)ref->core.width / 100 : req;
    _XoCalculateX (child);
    constraints->form.w_working = ref->core.width + 
      (referToParent ? -req :
		ref->core.x + 2 * ref->core.border_width + req)
		- constraints->form.x_working - 2 * child->core.border_width;
    break;

  case XoReferToTop:
  case XoReferToBottom:
  default:
    break;
  }
  /* To deal with a 0 or 0% pixel_width */
   constraints->form.w_working = max(constraints->form.w_working, 1);
}


static void _XoCalculateH (child)
     Widget child;
{
  int                req; 
  XoRequestType      reqType;
  XoReferenceEdge    refEdge;
  Boolean            referToParent;
  Boolean	     relativeReq;

  FormConstraints constraints = (FormConstraints) child->core.constraints;
  Widget ref = constraints->form.h_reference;

  int refDestroyed;

  ref = XoResolveReference (child, ref, &refDestroyed, (XoReference)XoReferToParent);

  if (refDestroyed) {
  	XoGeometryRec	geometryRec; 	
	geometryRec.request=XoNaturalGeometry;
	geometryRec.requestType=XoAbsoluteRequest;
	geometryRec.reference = XoReferToParent;
	geometryRec.referenceEdge = XoReferToDefault;
  	XoXtVaSetValues(child, XtNhGeometry, &geometryRec, NULL);
  }
  
  constraints = (FormConstraints) child->core.constraints;
  req       = constraints->form.h_requested;
  reqType   = constraints->form.h_request_type;
  refEdge   = constraints->form.h_reference_edge;
  relativeReq   = (Boolean) (reqType == XoRelativeRequest);
  
  switch (constraints->form.h_working) {
  case XoUnsetParameter:
    constraints->form.h_working = XoPendingParameter;
    break;
  case XoPendingParameter:
    _XoParamLoopError (child);
    constraints->form.h_working = XoParameterLoop;
    return;
  default:
    return;
  }

  if (req == XoNaturalGeometry) {
    XtWidgetGeometry pref;
    XtQueryGeometry (child, NULL, &pref);
    constraints->form.h_working = pref.height;
    return;
  }


  referToParent = ref == child->core.parent;

  if (! referToParent) 
    _XoLayoutChild (ref);

  switch (refEdge) {

  case XoReferToHeight:
    constraints->form.h_working =
		relativeReq ? (int)ref->core.height * req / 100:
                               (referToParent ? 0 : ref->core.height) + req;
                                                
    break;

  case XoReferToWidth:
    constraints->form.h_working =
		relativeReq ? (int)ref->core.width * req / 100:
                             ref->core.width + (referToParent ? -1 : 1) * req;
                                                
    break;

  case XoReferToTop:
    req = relativeReq ? req * (int)ref->core.height / 100 : req;
    _XoCalculateY (child);
    constraints->form.h_working = (referToParent ? req : ref->core.y - req)
	    - constraints->form.y_working - 2 * child->core.border_width;
    break;

  case XoReferToBottom:
    req = relativeReq ? req * (int)ref->core.height / 100 : req;
    _XoCalculateY (child);
    constraints->form.h_working = ref->core.height + 
      (referToParent ? -req : ref->core.y + 2 * ref->core.border_width + req)
	    - constraints->form.y_working - 2 * child->core.border_width;
    break;

  case XoReferToLeft:
  case XoReferToRight:
  default:
    break;
  }
  /* To deal with a 0 or 0% pixel_height */
   constraints->form.h_working = max(constraints->form.h_working, 1);
}


static void _XoLayoutChild(child)
     Widget child;
{
  FormConstraints form = (FormConstraints)child->core.constraints;
  /* printf("In _XoLayoutChild\n"); */

  switch (form->form.layout_state) {
  case LayoutPending:
    form->form.layout_state = LayoutInProgress;
    break;
  case LayoutRevisionPending:
    form->form.layout_state = LayoutRevisionInProgress;
    break;
  case LayoutDone:
    return;
  case LayoutInProgress:
  case LayoutRevisionInProgress:
    _XoConstraintLoopWarning (child);
    return;
  }

  form->form.x_working = form->form.y_working = form->form.w_working = 
    form->form.h_working = XoUnsetParameter;

  _XoCalculateX (child);
  _XoCalculateY (child);
  _XoCalculateW (child);
  _XoCalculateH (child);

  if (child->core.x      != form->form.x_working ||
      child->core.y      != form->form.y_working ||
      child->core.width  != form->form.w_working ||
      child->core.height != form->form.h_working   )
    XtConfigureWidget (child, 
		       form->form.x_working, form->form.y_working, 
		       form->form.w_working, form->form.h_working, 
		       child->core.border_width);

  form->form.layout_state = LayoutDone;
  /* printf("Finished _XoLayoutChild\n"); */
}


