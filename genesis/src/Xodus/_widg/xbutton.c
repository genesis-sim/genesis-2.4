/* $Id: xbutton.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xbutton.c,v $
 * Revision 1.2  2005/07/01 10:02:59  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/06/12 04:21:43  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.6  1998/07/15 06:27:19  dhb
 * Upi update
 *
 * Revision 1.5  1995/07/08 01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.4  1994/06/29  14:20:43  bhalla
 * Put in option to use 'title' as well as 'label' for setting
 * both on and off labels
 *
 * Revision 1.3  1994/02/08  22:27:13  bhalla
 * using xoFullName for widget name during create, added xoGetGeom
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Eliminated old comments.
 * Replaced create line option parsing with xoParseCreateArgs
 * Moved actions into default xoExecuteFunction
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Eliminated old comments.
 * Replaced create line option parsing with xoParseCreateArgs
 * Moved actions into default xoExecuteFunction
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "_xo/xo_defs.h"
#include "Widg/Framed.h"
#include "Widg/Button.h"
#include "widg_ext.h"

#ifndef MAX_NTARGS
#define MAX_NTARGS 20

static Gen2Xo   GXconvert[] = {
	{"onlabel", XtNonLabel},
	{"label", XtNonLabel},
	{"title", XtNonLabel},
	{"offlabel", XtNoffLabel},
	{"title", XtNoffLabel},
	{"label", XtNoffLabel},
	{"onfg", XtNonFg},
	{"offfg", XtNoffFg},
	{"onbg", XtNonBg},
	{"offbg", XtNoffBg},
	{"onfont", XtNonFont},
	{"offfont", XtNoffFont}
};

int XButton(button, action)
	register struct xbutton_type *button;
	Action         *action;
{
	int             ac = action->argc;
	char          **av = action->argv;
	Widget          parentW, fW, xoFindParentForm();
	char           *savevalptr;

	if (Debug(0) > 1)
		ActionHeader("XButton", button, action);

	SELECT_ACTION(action) {
case INIT:
		break;
case PROCESS:
		break;
case RESET:
		break;
case CREATE:
		/* arguments are: object_type name [field value] ... */

		if (!(parentW = xoFindParentForm(button))) {
			XgError("At least one ancestor must be an xform");
			return 0;
		}
		fW = XtVaCreateManagedWidget(xoFullName(button),
					   framedWidgetClass, parentW,
					   XtNchildClass, buttonWidgetClass,
					     XtNtype, XoPressButton,
					     XtNonLabel, button->name,
					     XtNoffLabel, button->name,
					     XtNmappedWhenManaged, False,
					     NULL);


		button->widget = (char *) XoGetFramedChild(fW);
		XtAddCallback((Widget)button->widget, XtNcallback, xoCallbackFn,
			      (XtPointer) button);
		ac--; av++;	/* object type */
		ac--; av++;	/* path */
		xoParseCreateArgs(button,ac,av);
		XtMapWidget(fW);
		xoGetGeom(button);
		return (1);
		/* NOTREACHED */
		break;
case SET:
    	if (ac == 2) {
        	if (strcmp(av[0],"title") == 0 ||
            	strcmp(av[0],"label") == 0) {
            	DirectSetElement(button,"onlabel",av[1]);
            	DirectSetElement(button,"offlabel",av[1]);
		savevalptr = av[1];
		av[1] = button->onlabel;
            	gx_convert(button,GXconvert,XtNumber(GXconvert),ac,av);
		av[1] = savevalptr;
            	return(1);
        	}
    	}
		if (ac) {	/* need to set fields */
			gx_convert(button, GXconvert, XtNumber(GXconvert), ac, av);
			return (0);
		}
		break;
case DELETE:
	if (button->onlabel != NULL) free (button->onlabel);
	if (button->offlabel != NULL) free (button->offlabel);
    	if (button->widget == NULL) return(0);;
    	XtDestroyWidget(XtParent((Widget)button->widget));
		break;
case XUPDATE:			/* update button fields due to changes in
				 * widget */
		if(ac > 0)
		xg_convert(button, GXconvert, XtNumber(GXconvert), ac, av);
		break;
default:
        xoExecuteFunction(button,action,button->script,"");
    break;

	}
	return (0);
}
#undef MAX_NTARGS
#endif
