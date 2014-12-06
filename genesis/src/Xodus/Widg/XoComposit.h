/*
** $Id: XoComposit.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $
**
** $Log: XoComposit.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1995/09/27 23:49:09  venkat
** Contains logs of all revisions
**
----------------------------
revision 1.1
date: 1995/03/06 18:24:52;  author: venkat;  state: Exp;
Initial revision
=============================================================================

*/

#ifndef _XoComposite_h
#define _XoComposite_h

/****************************************************************
 *
 * XoComposite widget
 *
 ****************************************************************/

#include <Xo/StringDefs.h>
#include <X11/Composite.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 bg		     Background		Pixel		XtDefaultBackground
 fg		     Foreground		Pixel		XtDefaultForeground
*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNbg      "bg"
#define XtNfg      "fg"


/* declare specific XoCompositeWidget class and instance datatypes */

typedef struct _XoCompositeClassRec*	        XoCompositeWidgetClass;
typedef struct _XoCompositeRec*		XoCompositeWidget;

/* declare the class constant */

extern WidgetClass xocompositeWidgetClass;

#endif /* _XoComposite_h */
