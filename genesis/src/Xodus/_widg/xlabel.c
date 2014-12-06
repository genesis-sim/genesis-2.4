/* $Id: xlabel.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xlabel.c,v $
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
 * Revision 1.13  2000/06/12 04:22:04  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.12  1998/07/15 06:27:19  dhb
 * Upi update
 *
 * Revision 1.11  1995/09/26 22:41:09  venkat
 * The label->label is not assigned the name of the label element by
 * default. This was causing an FUM error and a core dump on a deleteall -f, specifically
 * on Solaris. The label is allocated thro CopyString only when the label field is
 * set explicitly from the command line and then freed when it is destoyed.
 *
 * Revision 1.10  1995/08/05  02:36:18  venkat
 * The COPY action does not depend on ac and the if statement checking for
 * ac > 0  is removed.
 *
 * Revision 1.9  1995/07/29  00:06:02  venkat
 * The font is not a tring member resource and it is neither set nor deallocated here.
 *
 * Revision 1.8  1995/07/08  01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.7  1994/12/09  02:26:01  venkat
 * removed redundant check on dest element and
 * changed error message on COPY action meaningfully
 *
 * Revision 1.6  1994/12/05  23:54:07  dhb
 * Bug fixes by Venkat
 *
 * Revision 1.5.1.1  1994/12/02  00:11:47  venkat
 * Side branch created to override lock set on 1.5 by bhalla
 * Fixed error message reporting for COPY action
 *
 * Revision 1.5  1994/05/25  13:41:38  bhalla
 * Added COPY action
 *
 * Revision 1.4  1994/02/08  22:27:13  bhalla
 * Added default label name
 *  using xoFullName for widget name during create
 * added xoGetGeom
 *
 * Revision 1.3  1994/02/08  17:36:42  bhalla
 * Replaced create arg handling with xoParseCreateArgs
 * Put sensible arg handling in the SET action
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Added font option
 * Eliminated some comments
 * Added deletion code
 * */
#include <X11/Intrinsic.h>
#include "Widg/Framed.h"
#include "Widg/Label.h"
#include <X11/StringDefs.h>
#include "widg_ext.h"

static Gen2Xo GXconvert[] = {
	{"label",		XtNlabel},
	{"title",		XtNlabel},
	{"font",		XtNfont},
};



int XLabel (label, action)
     register struct xlabel_type *label;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentForm();
	FramedWidget fW;
	char	*savevalptr;
	struct xlabel_type *copy_label;
  
  if (Debug(0) > 1)
    ActionHeader("XLabel", label, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

    if (!(parentW = xoFindParentForm(label))) {
      XgError ("At least one ancestor must be an xform");
      return 0;
    }


	fW = (FramedWidget)XtVaCreateManagedWidget(
		xoFullName(label), framedWidgetClass, parentW,
					    XtNchildClass, labelWidgetClass,
					    XtNmappedWhenManaged, False,
					    XtNlabel, label->name,
					    NULL);
    label->widget = (char *)XoGetFramedChild(fW);
/* 	label->label = label->name; */ /* a default name */
    ac--, av++;			/* object type */
    ac--, av++;			/* path */

	xoParseCreateArgs(label,ac,av);
	XtMapWidget((Widget)fW);
	xoGetGeom(label);
	return(1);
	/* NOTREACHED */
	break;
  case COPY:
	copy_label = (struct xlabel_type *) action->data;
    		if ((parentW= xoFindParentForm(copy_label))==NULL){
     		  XgError ("Ancestor must be an xform for destination element");
      		  return 0;
    		}
	/* Duplicating the label string */
	copy_label->label = CopyString(label->label);
	/* creating  a new label widget */
	fW = (FramedWidget)XtVaCreateManagedWidget(
		xoFullName(copy_label), framedWidgetClass, parentW,
					    XtNchildClass, labelWidgetClass,
					    XtNmappedWhenManaged, False,
					    XtNlabel, copy_label->name,
					    NULL);
	copy_label->widget = (char *)XoGetFramedChild(fW);
	/* converting the rest of the values */
		gx_convert_all(copy_label,GXconvert,XtNumber(GXconvert));
	XtMapWidget((Widget)fW);
	xoGetGeom(copy_label);
	return (1);
	/* NOTREACHED */
	break;
  case SET:
	if (ac == 2) {
		if (strcmp(av[0],"title") == 0 ||
			strcmp(av[0],"label") ==  0) {
			DirectSetElement(label,"label",av[1]);
			savevalptr = av[1];
			av[1] = label->label;
			gx_convert(label,GXconvert,XtNumber(GXconvert),ac,av);
			av[1] = savevalptr;
			return(1);
		}
		
	}
    if (ac) {			/* need to set fields */
		gx_convert(label,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if(label->label != NULL) free(label->label);
	if (label->widget == NULL) return 0;
		XtDestroyWidget(XtParent((Widget)label->widget));
	break;
  case XUPDATE : /* update label fields due to changes in widget */
		if(ac > 0)
		xg_convert(label,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  }
  return 0;
}
