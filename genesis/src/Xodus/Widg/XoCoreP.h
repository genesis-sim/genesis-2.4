/*
** $Id: XoCoreP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $
**
** $Log: XoCoreP.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1995/09/27 23:40:53  venkat
** Contains log messages for all revisions
**

 ----------------------------
revision 1.3    locked by: venkat;
date: 1995/03/06 19:13:24;  author: venkat;  state: Exp;  lines: +2 -3
Added XtRXoPixel resource, fg and bg
----------------------------
revision 1.2
date: 1995/02/22 23:25:08;  author: venkat;  state: Exp;  lines: +1 -1
Changed the class record name according to Xt conventions
----------------------------
revision 1.1
date: 1995/02/22 01:25:57;  author: venkat;  state: Exp;
Initial revision
=============================================================================

*/

#ifndef _XoCoreP_h
#define _XoCoreP_h

#include "XoCore.h"


/* define unique representation types not found in <X11/StringDefs.h> */

/* define new fields for class XoCore */
typedef struct {
    int empty;
} XoCoreClassPart;

typedef struct _XoCoreClassRec {
    CoreClassPart	core_class;
    XoCoreClassPart	xocore_class;
} XoCoreClassRec;

extern XoCoreClassRec xocoreClassRec;

typedef struct {
    /* resources */
   Pixel fg;
   Pixel bg;
    /* private state */
} XoCorePart;

typedef struct _XoCoreRec {
    CorePart		core;
    XoCorePart	        xocore;
} XoCoreRec;

#endif /* _XoCoreP_h */
