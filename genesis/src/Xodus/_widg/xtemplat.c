/* $Id */
/* $Log */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xo/Geometry.h>
#include <Xo/Form.h>
#include "widg_ext.h"

/*
 * * This is a template.  There is no real genesis object called xobject, *
 * but all xodus objects should be built around this model.  This is * the *
 * only commented file in the xolib directory.  Others will just refer * to
 * it. * All the argument handling is carried out in the file xg_convert.c *
 * 
 * The template consists of two sections :  first, the static table * defining
 * the correspondence between Genesis and Xodus fields for * this element.
 * Second, the action function for this object.
 */

/*
 * The GXconvert structure must be filled in for each xodus element, and
 * must have all the fields specific to that element/widget. Note that the
 * special fields (which are common to all xodus objects) are NOT entered
 * in this table. The data structure Gen2xo lives in the file xo_struct.h :
 * typedef struct {
 * 	char*	g;	** Genesis field name **
 * 	char*   x;	** Xodus  field name **
 * } Gen2xo;
 */

/* @@SECTION XObject */

#define XtNhappy "happy"

static Gen2Xo   GXconvert[] = {
	/* we put in a dummy field here to keep the compiler happy */
	{"make_compiler_happy", XtNhappy}
};


/*
 * * The action function handles all the actions that the xodus object *
 * carries out. This similar to the usual genesis object definition *
 * function
 */
XObject(xobject, action)
	struct xobject_type *xobject;
	Action         *action;
{
	int             ac = action->argc;
	char          **av = action->argv;
	Widget         parentW
	struct object_type *parentO;


	/* ActionHeader is used for debugging purposes. It will print
	 * out the header ("xobject" in this case), the pathname of the element,
	 * and the action name. Defined in: sim/sim_response.c
	 */
	if (Debug(0) > 1)
		ActionHeader("xobject", xobject, action);

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
case CREATE:			/* create the widget */
		/* Check that the parent is an xform element */
		parentO = label->parent->object;
		if (strcmp(parentO->name, "xform")) {
			XgError("Immediate parent must be an xform");
			return 0;
		}
		parentW = (Widget) (((struct xform_type *) label->parent)->widget);

		/* Create the widget */
		label->widget = (char *) XtVaCreateManagedWidget(
					  av[1], framedWidgetClass, parentW,
					    XtNchildClass, labelWidgetClass,
						XtNmappedWhenManaged, False,
								 NULL);
		ac--, av++;	/* object type */
		ac--, av++;	/* path */
		if (ac) {	/* need to set widget fields */

			/* Defined in xo/xo_cvt.c */
			/* Accepts parameters:
			 * xwidg_type, table with genesis & xodus field names,
			 * size of this list,
			 * number of elements to convert,
			 * array of argument names and values.
			 * This function puts genesis element fields into xodus widgets.
			 */
			gx_convert(xobject, GXconvert, XtNumber(GXconvert), ac, av);
		}
		break;
case SET:			/* Set widget fields */
		ac--, av++;	/* object type */
		ac--, av++;	/* path */
		if (ac) {	/* need to set fields */
			gx_convert(xobject, GXconvert, XtNumber(GXconvert), ac, av);
		}
		break;
case DELETE:			/* Get rid of the widget and all kids */
		break;
case ADDMSG:			/* Some widgets need to do things when
				 * messages * are added, eg graphs may want
				 * to add a plot */
		break;
case DELETEMSG:		/* similarly, graphs may want to delet a plot *
				 * when messages are deleted */
		break;
case MSGACTION:		/* The operation to perform when an active *
				 * message arrives. */
		break;
case XUPDATE:			/* update xobject fields due to changes in
				 * widget */
		/* xg_convert does the opposite of gx_convert and is defined
		 * in sim/xo_cvt.c
		 */
		xg_convert(xobject, GXconvert, XtNumber(GXconvert), ac, av);
		break;
	}
}
