/* $Id: ximage.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: ximage.c,v $
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
 * Revision 1.8  2000/06/12 04:26:11  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.7  2000/05/02 06:18:45  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.6  1995/07/08 01:34:58  venkat
 * Changes-to-allocate-and-deallocate-string-member-resources-in-the-SET-and-DELETE-actions
 *
 * Revision 1.5  1994/12/10  01:11:58  venkat
 * Fixed PAR error on DELETE action by checking
 * for NULL before freeing the filename of the image object
 *
 * Revision 1.4  1994/02/08  22:27:13  bhalla
 * using xoFullName for widget name during create, added xoGetGeom
 *
 * Revision 1.3  1994/02/08  17:36:42  bhalla
 * got rid of random comment
 * Added 'format' field and set it to 'gif'
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Enabled callbacks
 * Eliminated lots of stuff by using xoParseCreateArgs
 * Put in destroyWidget code
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "Xo/XoDefs.h"
#include "_xo/xo_defs.h"
#include "Widg/Framed.h"
#include "Widg/Frame.h"
#include "Widg/Image.h"
#include "widg_ext.h"


static Gen2Xo GXconvert[] = {
	{"filename", XtNfilename},
	{"srcx", XtNsrcx},
	{"srcy", XtNsrcy},
	{"dstx", XtNdstx},
	{"dsty", XtNdsty}
};


/*
 * The action function handles all the actions that the xodus object
 * carries out. This similar to the usual genesis object definition
 * function
 */

/* XImage seems to be predefined somewhere. So we'll use Ximage */
int Ximage(imageXObj,action)
struct ximage_type *imageXObj;
Action *action;
{
	int ac = action->argc;
	char **av = action->argv;
	Widget parentW, newWidget, xoFindParentForm();
	char *savevalptr;

	/*
	 * ActionHeader is used for debugging purposes. It will print out
	 * the header ("xobject" in this case), the pathname of the
	 * element, and the action name. Defined in: sim/sim_response.c
	 */
	if (Debug(0) > 1)
		ActionHeader("XImage", imageXObj, action);

	SELECT_ACTION(action) {
case PROCESS:

		/*
		 * Put any widget process code here. Displays that need to
		 * be updated every clock tick are an example
		 */
		break;
case RESET:

		/*
		 * Restore widget initial conditions. For example, a graph
		 * widget might wish to restore the current time to zero
		 * and clear the existing plots
		 */
		break;
case CREATE:				/* create the widget */
		/* Check that the parent is an xform element */
		if (!(parentW = xoFindParentForm(imageXObj))) {
			XgError("Ximage: At least one ancestor must be an xform");
			return 0;
		}

		/* Create the widget */
		newWidget = XtVaCreateManagedWidget(
				       xoFullName(imageXObj),framedWidgetClass, parentW,
						    XtNchildClass, imageWidgetClass,
						    XtNelevation, XoInsetFrame,
						    XtNmappedWhenManaged, False,
						    NULL);


		imageXObj->widget = (char *) XoGetFramedChild(newWidget);
		imageXObj->format = "gif";

		XtAddCallback((Widget) imageXObj->widget, XtNcallback, xoCallbackFn, (XtPointer) imageXObj);

		ac--; av++;		/* object type */
		ac--; av++;		/* path */
		xoParseCreateArgs(imageXObj,ac,av);
		XtMapWidget(newWidget);
		xoGetGeom(imageXObj);

		return (1);
		/* NOTREACHED */
		break;
case SET:
		if (ac == 2 && strcmp(av[0], "filename")==0){
			DirectSetElement(imageXObj,"filename",av[1]);
			savevalptr = av[1];
			av[1] = imageXObj->filename;
			gx_convert(imageXObj, GXconvert, XtNumber(GXconvert), ac, av);
			av[1] = savevalptr;
			return(1);
		}	
		if (ac) {		/* need to set fields */
			gx_convert(imageXObj, GXconvert, XtNumber(GXconvert), ac, av);
		}
		break;
case DELETE:				/* Get rid of the widget and all
					 * kids */
		if(imageXObj->filename!=NULL)
		free(imageXObj->filename);
		if (!(imageXObj->widget)) return(0);
		XtDestroyWidget(XtParent((Widget)imageXObj->widget));
		break;

case XUPDATE:				/* update xobject fields due to
					 * changes in widget */

		/*
		 * xg_convert does the opposite of gx_convert and is
		 * defined in sim/xo_cvt.c
		 */
		if(ac > 0)
		 xg_convert(imageXObj, GXconvert, XtNumber(GXconvert), ac, av);
		break;
	}
	return 0;
}
