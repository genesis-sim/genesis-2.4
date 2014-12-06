/* $Id: FormP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: FormP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:56:52  venkat
 * Added Pixel resource, bg, in the FormPart structure.
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */

/* Form widget private definitions */

#ifndef _XawFormP_h
#define _XawFormP_h

#include "Form.h"
#include <X11/ConstrainP.h>

#define XtREdgeType "EdgeType"

typedef enum {  
  LayoutPending, 
  LayoutRevisionPending, 
  LayoutInProgress, 
  LayoutRevisionInProgress,
  LayoutDone
  } LayoutState;
#define XtInheritLayout ((Boolean (*)())_XtInherit)

typedef struct {
    Boolean	(*layout)(/* FormWidget, Dimension, Dimension */);
} FormClassPart;

/*
 * Layout(
 *	FormWidget w	- the widget whose children are to be configured
 *	Dimension w, h	- bounding box of layout to be calculated
 *
 *  Stores preferred geometry in w->form.preferred_{width,height}.
 *  If w->form.resize_in_layout is True, then a geometry request
 *  may be made for the preferred bounding box if necessary.
 *
 *  Returns True if a geometry request was granted, False otherwise.
 */

typedef struct _FormClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    FormClassPart	form_class;
} FormClassRec;

extern FormClassRec formClassRec;

typedef struct _FormPart {
	int		auto_resize;		/* flag for doing auto-resizes */
	Pixel 		bg;
    /* private state */
    LayoutState layout_state;       /* how far has layout gone?          */
    int		no_refigure;	    /* no re-layout while > 0		 */
    Boolean	needs_relayout;	    /* next time no_refigure == 0	 */
    Boolean	resize_in_layout;   /* should layout() do geom request?  */
    Dimension	preferred_width, preferred_height; /* cached from layout */
	XtCallbackList	callbacks;	/* only used for geometry callbacks */
} FormPart;

typedef struct _FormRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    FormPart		form;
} FormRec;

typedef struct _FormConstraintsPart {
/*
 * Constraint Resources.
 */
  int		   x_requested,	     /* requested dimensions and position */
		   y_requested,
		   w_requested,
		   h_requested;
  XoRequestType	   x_request_type,   /* type of request */
		   y_request_type,
		   w_request_type,
		   h_request_type; 
  Widget	   x_reference,	     /* request relative to what widget */
		   y_reference,
		   w_reference,
		   h_reference;
  XoReferenceEdge  x_reference_edge, /* edge to make reference to */
                   y_reference_edge,
                   w_reference_edge,
                   h_reference_edge;
  XoGeometryRec    x_geometry,       /* user set geometry records */
                   y_geometry,
                   w_geometry,
                   h_geometry;
/*
 * Private contstraint resources.
 */

  int              x_working,	     /* working space for figuring geometry */
                   y_working,
                   w_working,
                   h_working;
  LayoutState	   layout_state;     /* temporary layout state */
} FormConstraintsPart;

typedef struct _FormConstraintsRec {
    FormConstraintsPart	form;
} FormConstraintsRec, *FormConstraints;

#endif /* _XawFormP_h */
