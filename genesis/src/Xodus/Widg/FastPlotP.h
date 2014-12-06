/* $Id: FastPlotP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: FastPlotP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1998/07/14 20:45:26  dhb
 * Initial revision
 *
 * */
#ifndef _FastPlotP_h
#define _FastPlotP_h

#include "FastPlot.h"
#include "XoCoreP.h"

#include "Widg/XoComposiP.h"

/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRFastPlotResource "FastPlotResource"

typedef struct {
	int empty;
} FastPlotClassPart;

#define XoInheritTransformPoint _XtInherit
#define XoInheritTransformPoints _XtInherit
#define XoInheritBackTransform  _XtInherit
#define XoInheritSelectPix  _XtInherit
#define XoInheritUnSelectPix  _XtInherit

typedef struct _FastPlotClassRec {
    CoreClassPart	core_class;
	XoCoreClassPart xocore_class;
	FastPlotClassPart	fastplot_class;
} FastPlotClassRec;

extern FastPlotClassRec fastplotClassRec;

typedef struct {
/* resources */
	XtCallbackList	callbacks;
/* Private state */
	int ht, wid; /* Total plot height and width */
	int lborder, rborder, tborder, bborder; /* Border of plot area */
	int npts;
	XPoint *pts;
	int nnewpts;
	int ringpts;
	int overlay_flag;
	int line_flag;
	int plotmode;
	int reset_flag;
	int* colorptr;
	int xval;
	int yval;
	int xaxis_flag;
	int yaxis_flag;
	char *title;
	char *xtextmax;
	char *xtextmid;
	char *xtextmin;
	char *ytextmax;
	char *ytextmid;
	char *ytextmin;
	GC	gc;
} FastPlotPart;

typedef struct _FastPlotRec {
  CorePart		core;
  XoCorePart		xocore;
  FastPlotPart		fastplot;
} FastPlotRec;

#endif /* _FastPlotP_h */
