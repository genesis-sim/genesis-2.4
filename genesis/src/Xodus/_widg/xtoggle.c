/* $Id: xtoggle.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xtoggle.c,v $
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
 * Revision 1.8  2000/06/12 04:22:27  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.7  1998/07/15 06:27:19  dhb
 * Upi update
 *
 * Revision 1.6  1995/07/08 01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.5  1994/06/29  14:20:11  bhalla
 * Put in option to use 'title' as well as 'label' for setting
 * both on and off labels
 *
 * Revision 1.4  1994/04/25  18:05:17  bhalla
 * Added a few extra aliases to for label, fg etc.
 * Got rid of vestigial parsing for command line and replaced it
 * with the xoParseScriptArg function
 *
 * Revision 1.3  1994/02/08  22:27:13  bhalla
 * using xoFullName for widget name during create, added xoGetGeom
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Moved a lot of action stuff into the default action
 * xoExecuteFunction
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Moved a lot of action stuff into the default action
 * xoExecuteFunction
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

static Gen2Xo GXconvert[] = {
	{"onlabel",		XtNonLabel},
	{"label1",		XtNonLabel},
	{"label",		XtNonLabel},
	{"title",		XtNonLabel},
	{"offlabel",	XtNoffLabel},
	{"label0",		XtNoffLabel},
	{"label",		XtNoffLabel},
	{"title",		XtNoffLabel},
	{"onfg",		XtNonFg},
	{"offfg",		XtNoffFg},
	{"fg",			XtNonFg},
	{"fg",			XtNoffFg},
	{"onbg",		XtNonBg},
	{"offbg",		XtNoffBg},
	{"bg",			XtNonBg},
	{"bg",			XtNoffBg},
	{"onfont",		XtNonFont},
	{"offfont",		XtNoffFont},
	{"font",		XtNonFont},
	{"font",		XtNoffFont},
	{"state",		XtNstate}
};

int XToggle (toggle, action)
     register struct xtoggle_type *toggle;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,fW,xoFindParentForm();
  
  if (Debug(0) > 1)
    ActionHeader("XToggle", toggle, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

    if (!(parentW = xoFindParentForm(toggle))) {
      XgError ("At least one ancestor must be an xform");
      return 0;
    }



    fW = XtVaCreateManagedWidget(
		xoFullName(toggle), framedWidgetClass, parentW,
					    XtNchildClass, buttonWidgetClass,
					    XtNtype, XoToggleButton,
					    XtNonLabel, toggle->name,
					    XtNoffLabel, toggle->name,
					    XtNmappedWhenManaged, False,
					    NULL);
	toggle->widget = (char *)XoGetFramedChild(fW);
	XtAddCallback((Widget)toggle->widget, XtNcallback,xoCallbackFn,
		(XtPointer)toggle);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */

	xoParseCreateArgs(toggle, ac,av);
	XtMapWidget(fW);
	xoGetGeom(toggle);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
      if(ac == 2){	
	char *savevalptr;
    	if (strcmp(av[0],"title") == 0 ||
			strcmp(av[0],"label") == 0) {
        	DirectSetElement(toggle,"onlabel",av[1]);
        	DirectSetElement(toggle,"offlabel",av[1]);
		savevalptr  = av[1];
		av[1] = toggle->onlabel;
		gx_convert(toggle,GXconvert,XtNumber(GXconvert),ac,av);
		av[1] = savevalptr;
		return(1);
		}
    	if (strcmp(av[0],"label0") == 0) {
        	DirectSetElement(toggle,"offlabel",av[1]);
		savevalptr  = av[1];
		av[1] = toggle->offlabel;
		gx_convert(toggle,GXconvert,XtNumber(GXconvert),ac,av);
		av[1] = savevalptr;
		return(1);
    	}
    	if (strcmp(av[0],"label1") == 0) {
        	DirectSetElement(toggle,"onlabel",av[1]);
		savevalptr  = av[1];
		av[1] = toggle->onlabel;
		gx_convert(toggle,GXconvert,XtNumber(GXconvert),ac,av);
		av[1] = savevalptr;
		return(1);
    	}
      }
      if(ac > 0){
		gx_convert(toggle,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
      }
	break;
  case DELETE:
	if(toggle->offlabel != NULL ) free(toggle->offlabel);
	if(toggle->onlabel != NULL ) free(toggle->onlabel);
	if (toggle->widget != NULL)
    	XtDestroyWidget(XtParent((Widget)toggle->widget));
	break;
  case ANYBDOWN:
  case B1DOWN:
  case B2DOWN:
  case B3DOWN:
	toggle->state = 1;
	xoExecuteFunction(toggle,action,toggle->script,"1");
	break;
  case ANYBUP:
  case B1UP:
  case B2UP:
  case B3UP:
	/* We do the same thing, perhaps we may wish to call a different
	** script in some future version */
	toggle->state = 0;
    xoExecuteFunction(toggle,action,toggle->script,"0");
	break;
  case XUPDATE : /* update toggle fields due to changes in widget */
		if(ac > 0)
		xg_convert(toggle,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  }
  return(0);
}
#undef MAX_NTARGS /* 20 */
#endif
