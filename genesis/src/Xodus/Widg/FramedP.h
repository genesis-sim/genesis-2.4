/* $Id: FramedP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: FramedP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:41:57  venkat
 * Modified to subclass from XoComposite
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _FramedP_h
#define _FramedP_h

#include "Framed.h"
/* include superclass private header file */
#include "Widg/FrameP.h"

/* define unique representation types not found in <X11/StringDefs.h> */

#ifndef XtRWidgetClass
#define XtRWidgetClass "WidgetClass"
#endif

typedef struct {
    int empty;
} FramedClassPart;

typedef struct _FramedClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart	xocomposite_class;
    FrameClassPart      frame_part;
    FramedClassPart	framed_class;
} FramedClassRec;

extern FramedClassRec framedClassRec;

typedef struct {
    /* resources */
    WidgetClass         child_class;
} FramedPart;

typedef struct _FramedRec {
    CorePart		core;
    CompositePart       composite;
    XoCompositePart	xocomposite;
    FramePart           frame;
    FramedPart	        framed;
} FramedRec;

#endif /* _FramedP_h */
