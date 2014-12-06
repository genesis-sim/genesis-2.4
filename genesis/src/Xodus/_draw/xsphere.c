/* $Id: xsphere.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xsphere.c,v $
 * Revision 1.2  2005/07/01 10:02:59  svitak
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
 * Revision 1.6  2000/06/12 04:20:57  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.5  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.4  1994/06/13 23:02:41  bhalla
 * Added default radius of 1
 *
 * Revision 1.3  1994/02/08  22:33:37  bhalla
 * ci bug - I made no changes here
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Sphere.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"


static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"fg",		XtNforeground},
	{"r",		XtNr},
};


int XSphere (sphere, action)
     register struct xsphere_type *sphere;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW, xoFindParentDraw();
	SphereObject sW;

  
  if (Debug(0) > 1)
    ActionHeader("XSphere", sphere, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

	if ((parentW = xoFindParentDraw(sphere)) == NULL) return(0);
	sW = (SphereObject)XtCreateManagedWidget(
		av[1], sphereObjectClass, parentW, NULL,0);
    sphere->widget = (char *)sW;
	sphere->r = 1;
	XtAddCallback((Widget) sphere->widget, XtNcallback,xoCallbackFn,
		(XtPointer)sphere);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(sphere,ac,av);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(sphere,ac,av)) return(1);
    if (ac) {			/* need to set fields */
		gx_convert(sphere,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(sphere->widget)) return(0);
	XtDestroyWidget((Widget)sphere->widget);
	break;
  case XUPDATE : /* update sphere fields due to changes in widget */
		xg_convert(sphere,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(sphere,action,sphere->script,sphere->value);
	break;
  }
  return 0;
}

