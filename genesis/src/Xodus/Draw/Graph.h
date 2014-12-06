/* $Id: Graph.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: Graph.h,v $
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
#ifndef _Graph_h
#define _Graph_h

/****************************************************************
 *
 * Graph widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 axis		     Axis		char		x
 xmin		     Position		float		0.0
 ymin		     Position		float		0.0
 xmax		     Position		float		1.0
 ymax		     Position		float		1.0

*/



typedef struct _GraphClassRec*	GraphWidgetClass;
typedef struct _GraphRec*	GraphWidget;


extern WidgetClass graphWidgetClass;
extern void XoUpdatePlotAxes();

#endif /* _Graph_h */


#define XtNgxmin	"gxmin"
#define XtNgymin	"gymin"
#define XtNgxmax	"gxmax"
#define XtNgymax	"gymax"
#define XtNxoffset		"xoffset"
#define XtNyoffset		"yoffset"

#define XtNxlabel		"xlabel"
#define XtNylabel		"ylabel"
#define XtNxunits		"xunits"
#define XtNyunits		"yunits"
#ifndef XtNtitle
#define XtNtitle		"title"
#endif
