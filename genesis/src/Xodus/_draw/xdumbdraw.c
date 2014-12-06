/* $Id: xdumbdraw.c,v 1.2 2005/07/01 10:02:58 svitak Exp $ */
/*
 * $Log: xdumbdraw.c,v $
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
 * Revision 1.7  2000/06/12 04:25:12  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.6  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.5  1994/05/25 13:35:30  bhalla
 * Added default 80% of parent form height
 *
 * Revision 1.4  1994/02/08  22:33:37  bhalla
 *  Added xoFullName to get sensible names for the widgets
 *
 * Revision 1.3  1994/02/08  17:47:26  bhalla
 * Added call to xoGetGeom for geometry field initialization
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * eliminated soft actions
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Widg/Form.h"
#include "Draw/CoreDraw.h"
#include "Draw/DumbDraw.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"

static Gen2Xo GXconvert[] = {
	{"axis",		XtNaxis},
	{"xmin",		XtNxmin},
	{"xmax",		XtNxmax},
	{"ymin",		XtNymin},
	{"ymax",		XtNymax},
	{"zmin",		XtNzmin},
	{"zmax",		XtNzmax},
	{"drawflags",	XtNdrawflags},
};


int XDumbDraw (dumbdraw, action)
     register struct xdumbdraw_type *dumbdraw;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentForm();
	FramedWidget fW;
	int 	i;

  
  if (Debug(0) > 1)
    ActionHeader("XDumbDraw", dumbdraw, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

	if ((parentW = xoFindParentForm(dumbdraw)) == NULL) return(0);

	fW = (FramedWidget)XtVaCreateManagedWidget(
		xoFullName(dumbdraw), framedWidgetClass, parentW,
						XtVaTypedArg,XtNhGeometry,XtRString,"80%",4,
					    XtNchildClass, dumbdrawWidgetClass,
					    XtNmappedWhenManaged, False,
					    NULL);
    dumbdraw->widget = (char *)XoGetFramedChild(fW);
	XtAddCallback((Widget) dumbdraw->widget,XtNcallback,xoCallbackFn,
		(XtPointer)dumbdraw);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(dumbdraw,ac,av);
	XtMapWidget((Widget) fW);
	xoGetGeom(dumbdraw);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
    if (ac) {			/* need to set fields */
		gx_convert(dumbdraw,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(dumbdraw->widget)) return(0);
	XtDestroyWidget((Widget)dumbdraw->widget);
	break;
  case XUPDATE : /* update dumbdraw fields due to changes in widget */
		xg_convert(dumbdraw,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(dumbdraw,action,dumbdraw->script,"");
	break;
  }
  return 0;
}

