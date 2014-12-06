/* $Id: xgif.c,v 1.2 2005/07/01 10:02:58 svitak Exp $ */
/*
 * $Log: xgif.c,v $
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
 * Revision 1.8  2000/10/10 16:23:11  mhucka
 * Removed needless definition of "XtNhappy".
 *
 * Revision 1.7  2000/06/12 04:25:31  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.6  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.5  1995/07/08 01:10:30  venkat
 * filename member and string resource allocation and deallocation is done in the
 * genesis-side SET and DELETE actions
 *
 * Revision 1.4  1994/05/25  13:36:09  bhalla
 * Put in zero return after gx_convert
 *
 * Revision 1.3  1994/03/22  15:11:45  bhalla
 *  RCS error in previous check in. No code changes
 *
 * Revision 1.2  1994/02/08  17:47:26  bhalla
 * Added 'format' field and initialized it to "gif"
 *
 * Revision 1.2  1994/02/08  17:47:26  bhalla
 * Added 'format' field and initialized it to "gif"
 *
 * Revision 1.1  1994/02/02  20:04:53  bhalla
 * Initial revision
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Gif.h"
/*#include "olf_struct.h" */
#include "draw_ext.h"
#include "_widg/xform_struct.h"


static Gen2Xo GXconvert[] = {
    {"tx",      XtNpixX},
    {"ty",      XtNpixY},
    {"tz",      XtNpixZ},
	{"filename", XtNfilename},
	{"srcx", XtNsrcx},
	{"srcy", XtNsrcy},
	{"dstx", XtNdstx},
	{"dsty", XtNdsty}
};


int XGif(gif, action)
register struct xgif_type *gif;
Action *action;
{
	int ac = action->argc;
	char **av = action->argv;
	Widget parentW,xoFindParentDraw();
	GifObject sW;

	if (Debug(0) > 1)
		ActionHeader("XGif", gif, action);

	SELECT_ACTION(action) {
case INIT:
		break;
case PROCESS:
		break;
case RESET:
		break;
case CREATE:
		/* arguments are: object_type name [field value] ... */
		if ((parentW = xoFindParentDraw(gif)) == NULL) return(0);

		parentW = (Widget) (((struct xcoredraw_type *) gif->parent)->widget);
		sW = (GifObject) XtCreateManagedWidget(
				av[1], gifObjectClass, parentW, NULL, 0);
		gif->widget = (char *) sW;
		gif->format = "gif";

		XtAddCallback((Widget) gif->widget, XtNcallback,xoCallbackFn,
			(XtPointer)gif);


		ac--, av++;		/* object type */
		ac--, av++;		/* path */
		xoParseCreateArgs(gif,ac,av);
		return (1);
		/* NOTREACHED */
		break;
case SET:
		if (xoSetPixFlags(gif,ac,av)) return(1);
		if (ac == 2){
			char *savevalptr;
			if(strcmp(av[0], "filename")==0){
			 DirectSetElement(gif,"filename",av[1]);
			 savevalptr = av[1];
			 av[1] = gif->filename;
			 gx_convert(gif, GXconvert, XtNumber(GXconvert), ac, av);
			 av[1] = savevalptr;
			 return(1);
			}
		}
		if (ac) {		/* need to set fields */
			gx_convert(gif, GXconvert, XtNumber(GXconvert), ac, av);
			return(0);
		}
		break;
case DELETE:
		if (gif->filename != NULL) XtFree(gif->filename);
		if (!(gif->widget)) return(0);
		XtDestroyWidget((Widget)gif->widget);
		break;
case XUPDATE:	/* update gif fields due to changes in widget */
		if(ac > 0)
		xg_convert(gif, GXconvert, XtNumber(GXconvert), ac, av);
		break;
default:
		xoExecuteFunction(gif,action,gif->script,gif->value);
		break;
	}
	return 0;
}

