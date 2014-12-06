/* $Id: CoreDrawP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: CoreDrawP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/03/07 00:47:22  venkat
 * Removed fg resource since it now finds its place
 * in the base XoComposite class of type XtRXoPixel
 * in addition to the background
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * Added selected_pix field to hold pix->pix.selected field. This is
 * part of enhancement of selection mechanisms for complex pixes.
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _CoreDrawP_h
#define _CoreDrawP_h

#include "CoreDraw.h"

#include "Widg/XoComposiP.h"
/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRCoreDrawResource "CoreDrawResource"

typedef struct {
	void (*transformpt)(/* CoreDrawWidget dw, float x,y,z,
		int *sx,*sy,*sz */);
	void (*transformpts)(/* CoreDrawWidget dw, float x,y,z,
		int *sx,*sy,*sz, int npts */);
	void (*backtransform)(/* CoreDrawWidget dw, int  sx,sy,
		float *x,*y,*z */);
	void (*selectpix)(/* CoreDrawWidget w, XButtonEvent *event */);
	void (*unselectpix)(/* CoreDrawWidget w, XButtonEvent *event */);
	char	*extension;
} CoreDrawClassPart;

#define XoInheritTransformPoint _XtInherit
#define XoInheritTransformPoints _XtInherit
#define XoInheritBackTransform  _XtInherit
#define XoInheritSelectPix  _XtInherit
#define XoInheritUnSelectPix  _XtInherit

typedef struct _CoreDrawClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart xocomposite_class;
	CoreDrawClassPart	coredraw_class;
} CoreDrawClassRec;

extern CoreDrawClassRec coredrawClassRec;

typedef struct {
/* resources */
	XtCallbackList	callbacks;
/* Private state */
	float xmin,ymin,xmax,ymax; /* Define the border of the draw */
	Widget	selected;
	/* Needed to handle pixes which may have separate sub-parts
	** which get selected by mouse events */
	int		pix_selected;
	GC	gc;
	int drawflags;
} CoreDrawPart;

typedef struct _CoreDrawRec {
  CorePart		core;
  CompositePart		composite;
  XoCompositePart	xocomposite;
  CoreDrawPart		coredraw;
} CoreDrawRec;

#endif /* _CoreDrawP_h */
