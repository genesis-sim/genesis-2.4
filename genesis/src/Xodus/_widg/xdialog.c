/* $Id: xdialog.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xdialog.c,v $
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
 * Revision 1.13  2000/06/12 04:26:02  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.12  1998/07/15 06:27:19  dhb
 * Upi update
 *
 * Revision 1.11  1995/08/09 18:42:21  venkat
 * The COPY action for the dialog does not typecast the created frame
 * to FramedWidget in the XtVaCreateManagedWidget() call.
 *
 * Revision 1.10  1995/08/05  02:37:06  venkat
 * COPY action added
 *
 * Revision 1.9  1995/08/02  01:58:17  venkat
 * Some memory leak fixes
 *
 * Revision 1.8  1995/07/29  00:08:58  venkat
 * The font is not a string member resource and it is neither set nor deallocated here.
 * Also the value member is allocated and deallocated on demand and not in the CREATE code.
 *
 * Revision 1.7  1995/07/08  01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.6  1995/02/02  00:25:57  dhb
 * Extra checking for actions called without arguments.  This can happen
 * when the user uses the call command.
 *
 * Revision 1.5  1994/05/25  13:41:19  bhalla
 * Bugfix: added break in PROCESS action
 *
 * Revision 1.4  1994/02/08  22:32:59  bhalla
 * using xoFullName for widget name during create, added xoGetGeom
 *
 * Revision 1.3  1994/01/31  17:11:24  bhalla
 * Corrected calloc statement. This is what now lives at Tech
 *
 * Revision 1.2  1994/01/31  17:09:30  bhalla
 * updating it to version I sent to Caltech
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "_xo/xo_defs.h"
#include "Widg/Framed.h"
#include "Widg/Dialog.h"
#include "widg_ext.h"

#ifndef MAX_NTARGS
#define MAX_NTARGS 20

static Gen2Xo GXconvert[] = {
	{"value",		XtNvalue},
	{"label",		XtNlabel},
	{"title",		XtNlabel},
	{"font",		XtNfont},
/*
	{"fg",			"XtNtextFg"},
	{"fg",			"XtNonFg"},
	{"fg",			"XtNoffFg"},
	{"bg",			"XtNtextBg"},
	{"bg",			"XtNonBg"},
	{"bg",			"XtNoffBg"},
	{"font",		"XtNonFont"},
	{"font",		"XtNoffFont"},
	{"font",		"XtNtextFont"},
*/
};

/*
static char text_translations[] =
"<BtnDown>:		notify()	\n\
 Ctrl<Key>a:		beginning-of-line()	\n\
 Ctrl<Key>b:    	forward-char(-1)        \n\
 Ctrl<Key>d:        delete-char()           \n\
 Ctrl<Key>e:		end-of-line()           \n\
 Ctrl<Key>f:        forward-char()          \n\
 Ctrl<Key>k:        kill-to-end-of-line()   \n\
 Ctrl<Key>l:    	recenter()              \n\
 Ctrl<Key>u:    	kill-line()              \n\
 <Key>Left:     	forward-char(-1)        \n\
 <Key>Right:		forward-char()		\n\
 <Key>BackSpace:	delete-char(-1)         \n\
 <Key>Delete:		delete-char(-1)         \n\
 <Key>Return:		notify()	\n\
 <Key>:	        	self-insert-command()	\n\
";
*/

int XDialog (dialog, action)
     register struct xdialog_type *dialog;
     Action                     *action;
{
	int		ac=action->argc;
	char**	av=action->argv;
    Widget	parentW,fW,xoFindParentForm();
	char	*savevalptr;
	MsgIn	*msg;
	struct xdialog_type *copy_dialog;
  
  if (Debug(0) > 1)
    ActionHeader("XDialog", dialog, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    MSGLOOP(dialog,msg) {
        case 0: /* INPUT */
        dialog->fvalue = MSGVALUE(msg,0);
        sprintf(dialog->value,"%g",dialog->fvalue);
        XoSetDialogValue((Widget)dialog->widget,dialog->value);
		break;
    }
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

    if (!(parentW = xoFindParentForm(dialog))) {
      XgError ("At least one ancestor must be an xform");
      return 0;
    }
    
    fW = XtVaCreateManagedWidget(
		xoFullName(dialog), framedWidgetClass, parentW,
					    XtNchildClass, dialogWidgetClass,
						XtNlabel,dialog->name,
					    XtNmappedWhenManaged, False,
					    NULL);
	dialog->widget = (char *)XoGetFramedChild(fW);
	dialog->value  = NULL;
	dialog->label  = NULL;

	XtAddCallback((Widget)dialog->widget, XtNcallback,xoCallbackFn,
		(XtPointer)dialog);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(dialog,ac,av);
	XtMapWidget(fW);
	xoGetGeom(dialog);
	return(1);
	/* NOTREACHED */
	break;
  case COPY:
	copy_dialog = (struct xdialog_type *) action->data;
    		if ((parentW= xoFindParentForm(copy_dialog))==NULL){
     		  XgError ("Ancestor must be an xform for destination element");
      		  return 0;
    		}
	/* Duplicating the dialog string */
	copy_dialog->label = CopyString(dialog->label);
	copy_dialog->value = CopyString(dialog->value);
	/* creating  a new dialog widget */
	fW = XtVaCreateManagedWidget(
		xoFullName(copy_dialog), framedWidgetClass, parentW,
					    XtNchildClass, dialogWidgetClass,
					    XtNmappedWhenManaged, False,
					    XtNlabel, copy_dialog->name,
					    NULL);
	copy_dialog->widget = (char *)XoGetFramedChild(fW);
	/* converting the rest of the values */
	gx_convert_all(copy_dialog,GXconvert,XtNumber(GXconvert));
	XtMapWidget(fW);
	xoGetGeom(copy_dialog);
	return (1);
	/* NOTREACHED */
	break;
  case SET:
    if (ac) {			/* need to set fields */
		if (ac == 2 && strcmp(av[0],"value") == 0) {
			/* for setting the value we bypass everything to keep
			** the display from flickering */
			XoSetDialogValue((Widget)dialog->widget,av[1]);
			if(dialog->value != NULL) FreeString(dialog->value);
			dialog->value = CopyString(av[1]);
			return(1); /* We do NOT want the default routine to set it*/
        } 
		else if (ac == 2 && strcmp(av[0],"title") == 0) {
		if (dialog->label != NULL) FreeString(dialog->label);
            DirectSetElement(dialog,"label",av[1]);
			savevalptr = av[1];
			av[1] = dialog->label;
            gx_convert(dialog,GXconvert,XtNumber(GXconvert),ac,av);
			av[1] = savevalptr;
            return(1);
        }
		else {
			gx_convert(dialog,GXconvert,XtNumber(GXconvert),ac,av);
			return(0);
		}
   }
	break;
  case DELETE:
	if (!(dialog->widget)) return(0);
    XtDestroyWidget(XtParent((Widget)dialog->widget));
	if(dialog->label != NULL) FreeString(dialog->label);
	if(dialog->value != NULL) FreeString(dialog->value);
	break;
  case XUPDATE : /* update dialog fields due to changes in widget */
		if(ac > 0)
			xg_convert(dialog,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	if (ac > 0) {
			if(dialog->value != NULL) FreeString(dialog->value);
			dialog->value = CopyString(av[0]);
	    		dialog->fvalue = Atof(av[0]);
	}
        xoExecuteFunction(dialog,action,dialog->script,dialog->value);
    break;
  }
  return 0;
}
#undef MAX_NTARGS /* 20 */
#endif

