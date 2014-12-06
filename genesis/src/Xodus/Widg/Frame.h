/* $Id: Frame.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Frame.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/07/03 18:15:27  mhucka
 * Removed extra comma to eliminate compiler warnings.
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:48:12  venkat
 * Modified to include public header of XoComposite
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _Frame_h
#define _Frame_h

#include "XoComposit.h"
/****************************************************************
 *
 * Frame widget
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

 elevation           Elevation          XoFrameElevation XoRaisedFrame
 highlight           BorderColor        Pixel           (at realize)
 shadow              BorderColor        Pixel           (at realize)
*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNelevation      "elevation"
#define XtCElevation      "Elevation"

typedef enum {
  XoFlatFrame,
  XoRaisedFrame,
  XoInsetFrame
} XoFrameElevation;

#ifndef XtNhighlight
#define XtNhighlight      "highlight"
#endif
#define XtNshadow         "shadow"


/* declare specific FrameWidget class and instance datatypes */

typedef struct _FrameClassRec*	FrameWidgetClass;
typedef struct _FrameRec*	FrameWidget;

/* declare the class constant */

extern WidgetClass frameWidgetClass;

#endif /* _Frame_h */
