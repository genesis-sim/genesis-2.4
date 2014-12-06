/* $Id: xcoredraw.c,v 1.2 2005/07/01 10:02:58 svitak Exp $ */
/*
 * $Log: xcoredraw.c,v $
 * Revision 1.2  2005/07/01 10:02:58  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.8  2000/06/12 04:24:59  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.7  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.6  1995/09/27 00:09:20  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.5  1994/05/25  13:34:46  bhalla
 * Added default 80% of parent form height
 *
 * Revision 1.4  1994/02/08  22:33:37  bhalla
 * Added xoFullName to get sensible names for the widgets
 *
 * Revision 1.3  1994/02/08  17:47:26  bhalla
 * Added call to xoGetGeom for geometry field initialization
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions.
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Widg/Form.h"
#include "Draw/CoreDraw.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"

static Gen2Xo GXconvert[] = {
	{"xmin",		XtNxmin},
	{"xmax",		XtNxmax},
	{"ymin",		XtNymin},
	{"ymax",		XtNymax},
	{"drawflags",	XtNdrawflags},
};


#ifndef MAX_NTARGS
#define MAX_NTARGS 20
#endif

int XCoreDraw (coredraw, action)
     register struct xcoredraw_type *coredraw;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentForm();
	FramedWidget fW;
	static int xupdating = 0;
  
  if (Debug(0) > 1)
    ActionHeader("XCoreDraw", coredraw, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

	if ((parentW = xoFindParentForm(coredraw)) == NULL) return(0);
	fW = (FramedWidget)XtVaCreateManagedWidget(
		xoFullName(coredraw), framedWidgetClass, parentW,
						XtVaTypedArg,XtNhGeometry,XtRString,"80%",4,
					    XtNchildClass, coredrawWidgetClass,
					    XtNmappedWhenManaged, False,
					    NULL);
    coredraw->widget = (char *)XoGetFramedChild(fW);
	XtAddCallback((Widget) coredraw->widget,XtNcallback,xoCallbackFn,
		(XtPointer)coredraw);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(coredraw,ac,av);
	XtMapWidget((Widget) fW);
	xoGetGeom(coredraw);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
    if (ac && !xupdating) {			/* need to set fields */
		gx_convert(coredraw,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(coredraw->widget)) return(0);
	XtDestroyWidget((Widget)coredraw->widget);
	break;
  case XUPDATE : /* update coredraw fields due to changes in widget */
		xupdating = 1;
		xg_convert(coredraw,GXconvert,XtNumber(GXconvert),ac,av);
		xupdating = 0;
	break;
  default:
    xoExecuteFunction(coredraw,action,coredraw->script,"");
	break;
  }
  return 0;
}


 
void xoSetDrawRefresh(w,state)
    Widget w;
    int state;
{
    int drawflags;
    XtVaGetValues(w, XtNdrawflags, &drawflags, NULL);
 
    if (state == 0) {
        if (drawflags & XO_DRAW_RESIZABLE_NOT)
            return;
        XoXtVaSetValues(w,
            XtNdrawflags,drawflags | XO_DRAW_RESIZABLE_NOT,
            NULL);
    } else {
        if (!(drawflags & XO_DRAW_RESIZABLE_NOT))
            return;
        XoXtVaSetValues(w,
            XtNdrawflags,drawflags & ~XO_DRAW_RESIZABLE_NOT,
            NULL);
    }
}



#undef MAX_NTARGS /* 20 */

