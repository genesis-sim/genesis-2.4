/* $Id: xdraw.c,v 1.2 2005/07/01 10:02:58 svitak Exp $ */
/*
 * $Log: xdraw.c,v $
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
 * Revision 1.9  2000/06/12 04:15:45  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.8  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.7  1994/11/07 22:13:39  bhalla
 * Added in a function AssignDrawLimits which partially fixes the
 * continuing problems with relating the legacy cx,cy,cz, wx,wy etc
 * scheme to the current xmin xmax scheme. Did corresponding
 * changes to the gxcvt table.
 *
 * Revision 1.6  1994/05/25  13:35:13  bhalla
 * Added default 80% of parent form height
 *
 * Revision 1.5  1994/03/16  16:59:27  bhalla
 * Fixed part of the SET action behaviour for xmin etc. This really
 * needs a comprehensive matrix-based conversion routine.
 *
 * Revision 1.4  1994/02/08  22:33:37  bhalla
 *  Added xoFullName to get sensible names for the widgets
 * Put in a few more checks in the wx,wy, cx, cy, cz SET options
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
#include "Draw/Draw.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"

static void AssignDrawLimits();

static Gen2Xo GXconvert[] = {
	{"transform",		XtNtransform},
	{"xmin",		XtNxmin},
	{"xmax",		XtNxmax},
	{"ymin",		XtNymin},
	{"ymax",		XtNymax},
	{"zmin",		XtNzmin},
	{"zmax",		XtNzmax},
	{"vx",			XtNvx},
	{"vy",			XtNvy},
	{"vz",			XtNvz},
	{"rv",			XtNrv},
	{"rz",			XtNrz},
	{"drawflags",	XtNdrawflags},
};


int XDraw (draw, action)
     register struct xdraw_type *draw;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentForm();
	FramedWidget fW;
	float	dr;
  
  if (Debug(0) > 1)
    ActionHeader("XDraw", draw, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

    if ((parentW = xoFindParentForm(draw)) == NULL) return(0);

	fW = (FramedWidget)XtVaCreateManagedWidget(
		xoFullName(draw), framedWidgetClass, parentW,
						XtVaTypedArg,XtNhGeometry,XtRString,"80%",4,
					    XtNchildClass, drawWidgetClass,
					    XtNmappedWhenManaged, False,
					    NULL);
    draw->widget = (char *)XoGetFramedChild(fW);
	draw->fg = "black";
	draw->xmin = 0;
	draw->ymin = 0;
	draw->zmin = 0;
	draw->xmax = 1;
	draw->ymax = 1;
	draw->zmax = 1;
	draw->cx = 0.5;
	draw->cy = 0.5;
	draw->cz = 0.5;
	draw->wx = 1;
	draw->wy = 1;
	draw->transform = "z";

	XtAddCallback((Widget) draw->widget,XtNcallback,xoCallbackFn,
		(XtPointer)draw);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(draw,ac,av);
	XtMapWidget((Widget) fW);
	xoGetGeom(draw);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (strcmp(av[0],"cx") == 0) {
		dr = Atof(av[1]) - (draw->xmax + draw->xmin)/2;
		draw->xmin += dr;
		draw->xmax += dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"cy") == 0) {
		dr = Atof(av[1]) - (draw->ymax + draw->ymin)/2;
		draw->ymin += dr;
		draw->ymax += dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"cz") == 0) {
		dr = Atof(av[1]) - (draw->zmax + draw->zmin)/2;
		draw->zmin += dr;
		draw->zmax += dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"wx") == 0) {
		dr = Atof(av[1]);
		if (dr == 0) return(1);
		draw->cx = (draw->xmin+draw->xmax)/2;
		draw->xmin = draw->cx - dr / 2;
		draw->xmax = draw->cx + dr / 2;
		draw->cy = (draw->ymin+draw->ymax)/2;
		draw->ymin = draw->cy - dr / 2;
		draw->ymax = draw->cy + dr / 2;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"wy") == 0) {
		dr = Atof(av[1]);
		if (dr == 0) return(1);
		if (strcmp(draw->transform,"z") == 0) {
			draw->cy = (draw->ymin+draw->ymax)/2;
			draw->ymin = draw->cy - dr / 2;
			draw->ymax = draw->cy + dr / 2;
		} else {
			draw->cz = (draw->zmin+draw->zmax)/2;
			draw->zmin = draw->cy - dr / 2;
			draw->zmax = draw->cy + dr / 2;
		}
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"xmin") == 0) {
		dr = Atof(av[1]);
		if (dr == draw->xmax) return(1);
		draw->xmin = dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"xmax") == 0) {
		dr = Atof(av[1]);
		if (dr == draw->xmin) return(1);
		draw->xmax = dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"ymin") == 0) {
		dr = Atof(av[1]);
		if (dr == draw->ymax) return(1);
		draw->ymin = dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"ymax") == 0) {
		dr = Atof(av[1]);
		if (dr == draw->ymin) return(1);
		draw->ymax = dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"zmin") == 0) {
		dr = Atof(av[1]);
		if (dr == draw->zmax) return(1);
		draw->zmin = dr;
		AssignDrawLimits(draw);
		return(1);
	}
	if (strcmp(av[0],"zmax") == 0) {
		dr = Atof(av[1]);
		if (dr == draw->zmin) return(1);
		draw->zmax = dr;
		AssignDrawLimits(draw);
		return(1);
	}

    if (ac) {			/* need to set fields */
		gx_convert(draw,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	return(0);
	/* NOTREACHED */
	break;
  case DELETE:
	if (!(draw->widget)) return(0);
	XtDestroyWidget(XtParent((Widget)draw->widget));
	break;
  case XUPDATE : /* update draw fields due to changes in widget */
		xg_convert(draw,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(draw,action,draw->script,"");
	break;
  }
  return 0;
}

/* This function is meant to treat the cx etc uniformly
** by ensuring that they simply get changed to the best
** combination of xmin etc, and then the xmin etc get
** set in the draw widget 
*/
static void AssignDrawLimits(draw)
    struct xdraw_type *draw;
{
	char *tav[15];
	int i;

	for(i = 0; i < 15; i++)
		tav[i] = (char *) calloc(40,sizeof(char));


	/* Assign the auxiliary variables */
	draw->cx = (draw->xmin + draw->xmax) / 2;
	draw->cy = (draw->ymin + draw->ymax) / 2;
	draw->cz = (draw->zmin + draw->zmax) / 2;
	draw->wx = draw->xmax - draw->xmin;
	draw->wy = (strcmp(draw->transform,"z") == 0) ?
		draw->ymax - draw->ymin : draw->zmax - draw->zmin;

	/* convert the relevant variables */
	i = 0;
	sprintf(tav[i],"%s","xmin"); i++;
	sprintf(tav[i],"%f",draw->xmin); i++;
	sprintf(tav[i],"%s","xmax"); i++;
	sprintf(tav[i],"%f",draw->xmax); i++;

	sprintf(tav[i],"%s","ymin"); i++;
	sprintf(tav[i],"%f",draw->ymin); i++;
	sprintf(tav[i],"%s","ymax"); i++;
	sprintf(tav[i],"%f",draw->ymax); i++;

	sprintf(tav[i],"%s","zmin"); i++;
	sprintf(tav[i],"%f",draw->zmin); i++;
	sprintf(tav[i],"%s","zmax"); i++;
	sprintf(tav[i],"%f",draw->zmax); i++;

	gx_convert(draw,GXconvert,XtNumber(GXconvert),i,tav);

	for(i = 0; i < 15; i++)
		free(tav[i]);
}
