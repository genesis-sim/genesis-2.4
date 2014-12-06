/* $Id: GraphP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: GraphP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/03/07 00:55:57  venkat
 * Changed Class and widget part structures to subclass
 * XoComposite widget
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
#ifndef _GraphP_h
#define _GraphP_h

#include "Graph.h"
#include "CoreDrawP.h" /* the superclass private header */

#include <X11/Composite.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRGraphResource "GraphResource"

typedef struct {
    int empty;
} GraphClassPart;

typedef struct _GraphClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart xocomposite_class;
	CoreDrawClassPart	coredraw_class;
    GraphClassPart	graph_class;
} GraphClassRec;

extern GraphClassRec graphClassRec;

typedef struct {
	float xmin,xmax,ymin,ymax;
	float xoffset,yoffset;
	String	title;
	String	xlabel;
	String	ylabel;
	String	xunits;
	String	yunits;
	/* Private fields */
	Widget xaxis, yaxis, title_w;
} GraphPart;

typedef struct _GraphRec {
  CorePart		core;
  CompositePart		composite;
  XoCompositePart	xocomposite;
  CoreDrawPart		coredraw;
  GraphPart		graph;
} GraphRec;

#endif /* _GraphP_h */
