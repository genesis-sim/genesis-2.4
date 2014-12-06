/* $Id: Plot.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: Plot.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/12/06 00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _Plot_h
#define _Plot_h

/************************************************************
  
  Plot Object.

*/


/* Resources: inherited from Pix ..

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 destroyCallback     Callback           Pointer         NULL
 selectCallback      Callback           Pointer         NULL
 sensitive           Sensitive          Boolean         True

 x                   Position           Position        0
 y                   Position           Position        0
width               Width              Dimension       0
 height              Height             Dimension       0

 pixX	     Position		Position	0
 pixY	     Position		Position	0
 pixZ	     Position		Position	0

 fg		     Foreground         Pixel		BlackPixel

 Own resources
 linewidth		  Linewidth int		1
 linestyle		  Linestyle int		1
 npts		      Npts		int		0
 frompts	      Npts		int		0
 topts		      Npts		int		0
 xpts		      Xpts		Pointer		NULL
 ypts		      Xpts		Pointer		NULL
*/

#ifndef XtNlinewidth
#define XtNlinewidth	"linewidth"
#define XtCLineWidth	"LineWidth"
#endif

#ifndef XtNlinestyle
#define XtNlinestyle	"linestyle"
#define XtRLinestyle	"Linestyle"
#define XtCLinestyle	"Linestyle"
#endif

#ifndef XtNxpts
#define XtNxpts			"xpts"
#define XtNypts			"ypts"
#define XtCPts			"Pts"
#endif

#ifndef XtNnpts
#define XtNnpts			"npts"
#define XtCNpts			"Npts"
#endif

#ifndef XtNwx
#define XtNwx			"wx"
#define XtNwy			"wy"
#define XtCWx			"Wx"
#endif

#ifndef XtNxmin
#define	XtNxmin			"xmin"
#define	XtNxmax			"xmax"
#define	XtNymin			"ymin"
#define	XtNymax			"ymax"
#endif

#define	XtNxminvalue			"xminvalue"
#define	XtNxmaxvalue			"xmaxvalue"
#define	XtNyminvalue			"yminvalue"
#define	XtNymaxvalue			"ymaxvalue"

#ifndef XtCRange
#define XtCRange		"Range"
#endif

#define XtNsx			"sx"
#define XtNsy			"sy"
#define XtCScale		"Scale"

#define XtNfrompts			"frompts"
#define XtNtopts			"topts"

typedef struct _PlotClassRec*       PlotObjectClass;
typedef struct _PlotRec*            PlotObject;
typedef struct _PlotRec*            Plot;

extern WidgetClass plotObjectClass;

#define XO_PLOT_PTS_INCREMENT 1000
#endif
