/* $Id: Form.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Form.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:58:15  venkat
 * Included definition for the background resource
 * XtNbg
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _XoForm_h
#define _XoForm_h

#include <X11/Constraint.h>
#include <Xo/Geometry.h>

/***********************************************************************
 *
 * Form Widget
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 defaultDistance     Thickness		int		4
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	computed at realize
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	computed at realize
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* Constraint parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 xRequested          GeometryRequest    int             x
 yRequested          GeometryRequest    int             y
 wRequested          GeometryRequest    int             width
 hRequested          GeometryRequest    int             height
 xRequestType        RequestType        XoRequestType   XoAbsoluteRequest
 yRequestType        RequestType        XoRequestType   XoAbsoluteRequest
 wRequestType        RequestType        XoRequestType   XoAbsoluteRequest
 hRequestType        RequestType        XoRequestType   XoAbsoluteRequest
 xReference          Widget             Widget          NULL (parent)
 yReference          Widget             Widget          NULL (parent)
 wReference          Widget             Widget          NULL (parent)
 hReference          Widget             Widget          NULL (parent)
 xReferenceEdge      ReferenceEdge      XoReferenceEdge XoReferToLeft
 yReferenceEdge      ReferenceEdge      XoReferenceEdge XoReferToTop
 wReferenceEdge      ReferenceEdge      XoReferenceEdge XoReferToWidth
 hReferenceEdge      ReferenceEdge      XoReferenceEdge XoReferToHeight
*/



#define XtNw               XtNwidth
#define XtNh               XtNheight
#define XtNxRequested	   "xRequested"
#define XtNyRequested	   "yRequested"
#define XtNwRequested	   "wRequested"
#define XtNhRequested	   "hRequested"
#define XtNxRequestType	   "xRequestType"
#define XtNyRequestType	   "yRequestType"
#define XtNwRequestType	   "wRequestType"
#define XtNhRequestType	   "hRequestType"
#define XtNxReference	   "xReference"
#define XtNyReference	   "yReference"
#define XtNwReference	   "wReference"
#define XtNhReference	   "hReference"
#define XtNxReferenceEdge  "xReferenceEdge"
#define XtNyReferenceEdge  "yReferenceEdge"
#define XtNwReferenceEdge  "wReferenceEdge"
#define XtNhReferenceEdge  "hReferenceEdge"
#define XtNxGeometry       "xGeometry"
#define XtNyGeometry       "yGeometry"
#define XtNwGeometry       "wGeometry"
#define XtNhGeometry       "hGeometry"

#define XtCGeometryRequest "GeometryRequest"
#define XtCRequestType     "RequestType"
#define XtCReference       "Reference"
#define XtCWidget          "Widget"
#define XtCReferenceEdge   "ReferenceEdge"
#define XtCXoGeometry      "XoGeometry"

#define XtNauto_resize		"auto_resize"
#define XtCAutoResize		"AutoResize"
#define XtNbg		"bg"

typedef struct _FormClassRec	*FormWidgetClass;
typedef struct _FormRec		*FormWidget;

extern WidgetClass formWidgetClass;


extern void XoFormToggleUpdate();	/* widget, state */
    /* Widget widget */
    /* Boolean state */

extern void XoSizeForm();

#endif /* _Xo_Form_h */

