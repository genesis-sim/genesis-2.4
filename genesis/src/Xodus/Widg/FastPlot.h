/* $Id: FastPlot.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: FastPlot.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1998/07/14 20:45:26  dhb
 * Initial revision
 * */

#ifndef _FastPlot_h
#define _FastPlot_h

/****************************************************************
 *
 * FastPlot widget
 *
 ****************************************************************/

#define XtNht		"ht"
#define XtNwid		"wid"
#define XtNlborder		"lborder"
#define XtNrborder		"rborder"
#define XtNtborder		"tborder"
#define XtNbborder		"bborder"
#define XtNnpts		"npts"
#define XtNpts		"pts"
#define XtNnnewpts		"nnewpts"
#define XtNringpts		"ringpts"
#define XtNplotmode		"plotmode"
#define XtNoverlay_flag		"overlay_flag"
#define XtNline_flag		"line_flag"
#define XtNreset_flag		"reset_flag"
#define XtNcolorptr		"colorptr"
#define XtNxval		"xval"
#define XtNyval		"yval"
#define XtNxaxis_flag		"xaxis_flag"
#define XtNyaxis_flag		"yaxis_flag"
#define XtNtitle		"title"
#define XtNxtextmax		"xtextmax"
#define XtNxtextmid		"xtextmid"
#define XtNxtextmin		"xtextmin"
#define XtNytextmax		"ytextmax"
#define XtNytextmid		"ytextmid"
#define XtNytextmin		"ytextmin"

#define XtCNpts		"Npts"
#define XtCPts		"Pts"
#define XtCPtr		"Ptr"
#define XtCFlag		"Flag"
#define XtCFoo		"Foo"


typedef struct _FastPlotClassRec*	FastPlotWidgetClass;
typedef struct _FastPlotRec*	FastPlotWidget;


extern WidgetClass fastplotWidgetClass;

#endif /* _FastPlot_h */
