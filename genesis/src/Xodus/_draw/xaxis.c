/* $Id: xaxis.c,v 1.2 2005/07/01 10:02:51 svitak Exp $ */
/*
 * $Log: xaxis.c,v $
 * Revision 1.2  2005/07/01 10:02:51  svitak
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
 * Revision 1.9  2000/06/12 04:26:33  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.8  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.7  1995/03/22 19:06:02  venkat
 * Same as 1.5. The additional resources are removed and the
 * old resource retained. The justification resource added into the
 * Xodus Axis widget is not a genesis xaxis element option at this point
 * but is to be extended to one.
 *
 * Revision 1.5  1995/03/17  20:51:38  venkat
 * Got rid of unnecessary create_interpol() function declaration.
 *
 * Revision 1.4  1994/05/26  13:46:43  bhalla
 * Added call to xo_graph_update_axis in the XUPDATE action. This finally
 * makes it possible to retrieve changes to the axis limits and put them
 * into the genesis fields. Much larger changes have taken place on
 * the X side of things.
 *
 * Revision 1.3  1994/02/08  22:33:37  bhalla
 * buggy ci - there was no change here
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated softactions
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated softactions
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Axis.h"
/*#include "olf_struct.h" */
#include "draw_ext.h"
#include "_widg/xform_struct.h"


static Gen2Xo GXconvert[] = {
	{"fg",		XtNforeground},
    {"tx",      XtNpixX},
    {"ty",      XtNpixY},
    {"tz",      XtNpixZ},
	{"linewidth",		XtNlinewidth},
	{"linestyle",		XtNlinestyle},
	{"textcolor",		XtNtextcolor},
	{"textmode",		XtNaxisTextmode},
	{"textfont",		XtNtextfont},
	{"units",		XtNunits},
	{"title",		XtNtitle},
	{"unitoffset",		XtNunitoffset},
	{"titleoffset",		XtNtitleoffset},
	{"ticklength",		XtNticklength},
	{"tickmode",		XtNtickmode},
	{"labeloffset",		XtNlabeloffset},
	{"rotatelabels",		XtNrotatelabels},
	{"axx",		XtNaxx},
	{"axy",		XtNaxy},
	{"axz",		XtNaxz},
	{"axmin",		XtNaxmin},
	{"axmax",		XtNaxmax},
	{"axlength",		XtNaxlength},
	{"tickx",		XtNtickx},
	{"ticky",		XtNticky},
	{"tickz",		XtNtickz},
	{"labmin",		XtNlabmin},
	{"labmax",		XtNlabmax},
};


int XAxis (axis, action)
     register struct xaxis_type *axis;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentDraw();
	AxisObject sW;

  
  if (Debug(0) > 1)
    ActionHeader("XAxis", axis, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */
	if ((parentW = xoFindParentDraw(axis)) == NULL) return(0);
	sW = (AxisObject)XtCreateManagedWidget(
		av[1], axisObjectClass, parentW, NULL,0);
    axis->widget = (char *)sW;
	/* copying over the table pointers */
	XtAddCallback((Widget) axis->widget, XtNcallback,xoCallbackFn,
		(XtPointer)axis);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(axis,ac,av);
	return(1);
	/* NOTREACHED */
	break;
  case SET: /* pleasantly boring */
	if (xoSetPixFlags(axis,ac,av)) return(1);
    if (ac) {			/* need to set fields */
		gx_convert(axis,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(axis->widget)) return(0);
	XtDestroyWidget((Widget)axis->widget);
	break;
  case XUPDATE : /* update axis fields due to changes in widget */
		xg_convert(axis,GXconvert,XtNumber(GXconvert),ac,av);
		xo_graph_update_axis(axis,axis->update_parent,
				axis->axmin,axis->axmax);
			
	break;
  default: /* generic script action functions here */
	xoExecuteFunction(axis,action,axis->script,axis->value);
	break;
  }
  return 0;
}
