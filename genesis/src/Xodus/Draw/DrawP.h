/* $Id: DrawP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: DrawP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/03/07 01:01:08  venkat
 * Changed class part and widget part structures
 * to subclass XoComposite
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
#ifndef _DrawP_h
#define _DrawP_h

#include "Draw.h"
#include "CoreDrawP.h" /* the superclass private header */

#include <X11/Composite.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRDrawResource "DrawResource"

typedef struct {
    int empty;
} DrawClassPart;

typedef struct _DrawClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart xocomposite_class;
	CoreDrawClassPart	coredraw_class;
    DrawClassPart	draw_class;
} DrawClassRec;

extern DrawClassRec drawClassRec;

typedef struct {
	char transform; /* x,y,z,o,p,X,Y,Z */
	float zmin,zmax;
	float **matrix;
	float	vx,vy,vz; /* The viewpoint vector */
	float	ax,ay,az; /* The axis vector */
	float rv; /* Dist of viewpoint from viewplane for perspective */
	float rz; /* position of viewplane */
	int perspective;
} DrawPart;

typedef struct _DrawRec {
  CorePart		core;
  CompositePart		composite;
  XoCompositePart	xocomposite;
  CoreDrawPart		coredraw;
  DrawPart		draw;
} DrawRec;

#endif /* _DrawP_h */
