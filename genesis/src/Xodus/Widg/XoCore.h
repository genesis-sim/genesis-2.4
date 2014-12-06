/*
** $Id: XoCore.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $
**
** $Log: XoCore.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1995/09/27 23:46:23  venkat
** Contains logs of all revisions
**

 revision 1.2
 date: 1995/03/06 19:12:48;  author: venkat;  state: Exp;  lines: +4 -2
Defined resource constants for foreground and background
----------------------------
revision 1.1
date: 1995/02/22 01:25:14;  author: venkat;  state: Exp;
Initial revision
=============================================================================
*/

#ifndef _XoCore_h
#define _XoCore_h

/****************************************************************
 *
 * XoCore widget
 *
 ****************************************************************/

#include <Xo/StringDefs.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 bg		     Background		Pixel		XtDefaultBackground
 fg		     Foreground		Pixel		XtDefaultForeground
*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNbg      "bg"
#define XtNfg      "fg"


/* declare specific XoCoreWidget class and instance datatypes */

typedef struct _XoCoreClassRec*	        XoCoreWidgetClass;
typedef struct _XoCoreRec*		XoCoreWidget;

/* declare the class constant */

extern WidgetClass xocoreWidgetClass;

#endif /* _XoCore_h */
