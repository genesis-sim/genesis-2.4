/*
** $Id: XoComposiP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $
**
** $Log: XoComposiP.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1995/09/27 23:44:02  venkat
** Contains log messages of all revisions
**
----------------------------
revision 1.1    locked by: venkat;
date: 1995/03/06 18:23:10;  author: venkat;  state: Exp;
Initial revision
=============================================================================

*/

#ifndef _XoCompositeP_h
#define _XoCompositeP_h

#include "XoComposit.h"
#include <X11/CompositeP.h>

/* define new fields for class XoComposite */
typedef struct {
    int empty;
} XoCompositeClassPart;

typedef struct _XoCompositeClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    XoCompositeClassPart	xocomposite_class;
} XoCompositeClassRec;

extern XoCompositeClassRec xocompositeClassRec;

typedef struct {
    /* resources */
   Pixel fg;
   Pixel bg;
    /* private state */
} XoCompositePart;

typedef struct _XoCompositeRec {
    CorePart		core;
    CompositePart	composite;
    XoCompositePart	xocomposite;
} XoCompositeRec;

#endif /* _XoCompositeP_h */
