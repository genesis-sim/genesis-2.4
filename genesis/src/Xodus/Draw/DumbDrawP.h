/* $Id: DumbDrawP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: DumbDrawP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/03/07 01:07:48  venkat
 * Changed widget and class structure to subclass
 * XoComposite
 *
 * Revision 1.3  1994/12/06  00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _DumbDrawP_h
#define _DumbDrawP_h

#include "DumbDraw.h"
#include "CoreDrawP.h" /* the superclass private header */

#include <X11/Composite.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRDumbDrawResource "DumbDrawResource"

typedef struct {
    int empty;
} DumbDrawClassPart;

typedef struct _DumbDrawClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart xocomposite_class;
	CoreDrawClassPart	coredraw_class;
    DumbDrawClassPart	dumbdraw_class;
} DumbDrawClassRec;

extern DumbDrawClassRec dumbdrawClassRec;

typedef struct {
	char axis;
	float zmin,zmax;
} DumbDrawPart;

typedef struct _DumbDrawRec {
  CorePart		core;
  CompositePart		composite;
  XoCompositePart	xocomposite;
  CoreDrawPart		coredraw;
  DumbDrawPart		dumbdraw;
} DumbDrawRec;

#endif /* _DumbDrawP_h */
