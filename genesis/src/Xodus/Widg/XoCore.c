static char rcsid[] = "$Id: XoCore.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $";

/*
** $Log: XoCore.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  1995/09/27 23:31:30  venkat
** This now has log messages of all revisions
**
 * Revision 1.7  1995/09/27  23:19:38  venkat
 * Recovered RCS ID and Log tags
 *
  ----------------------------
revision 1.6
date: 1995/09/26 23:56:50;  author: venkat;  state: Exp;  lines: +2 -2
Changed-XtVaSetValues()-calls-to-XoXtVaSetValues()
----------------------------
revision 1.5
date: 1995/04/05 17:08:33;  author: venkat;  state: Exp;  lines: +1 -1
The colormap resource is not set here again.
 It is done once for the top level shell.
----------------------------
revision 1.4
date: 1995/03/24 23:53:45;  author: venkat;  state: Exp;  lines: +1 -1
Changed default value of XtNbg resource to XoDefaultBackground.
This macro is defined in Xo/XoDefs.h as the default or fallback
value for the application, namely "LightSteelBlue"
Doing this ensures the match of the XtNbg resource with
the default for the Core widget class's XtNbackground resource
----------------------------
revision 1.3
date: 1995/03/06 19:07:22;  author: venkat;  state: Exp;  lines: +36 -31
Realize method sets the global resources instead of Initialize
----------------------------
revision 1.2
date: 1995/02/23 00:09:16;  author: venkat;  state: Exp;  lines: +1 -1
Made realize inherited from core class
----------------------------
revision 1.1
date: 1995/02/22 01:24:47;  author: venkat;  state: Exp;
Initial revision
=============================================================================

*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "XoCoreP.h"

#include <stdio.h>

static XtResource resources[] = {
#define offset(field) XtOffset(XoCoreWidget, xocore.field)
  { XtNbg, XtCBackground, XtRXoPixel, sizeof (Pixel),
    offset(bg), XtRString, (XtPointer) XoDefaultBackground },
  { XtNfg, XtCForeground, XtRXoPixel, sizeof (Pixel),
    offset(fg), XtRString, (XtPointer) "XtDefaultForeground" },
#undef offset
};


/* Methods */
static Boolean SetValues();
static void Realize();


XoCoreClassRec xocoreClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) (&coreClassRec),
    /* class_name		*/	"XoCore",
    /* widget_size		*/	sizeof(XoCoreRec),
    /* class_initialize		*/	NULL,	
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	False,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	XtInheritResize,
    /* expose			*/	XtInheritExpose,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* xocore fields */
    /* empty			*/	0
  }
};

WidgetClass xocoreWidgetClass = (WidgetClass)&xocoreClassRec;

/** Local function declarations **/
static void SetGlobalResources(/*Widget*/);

/***************************************************************************
  Methods:
*/



/*ARGSUSED*/
static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
	Boolean ret = False;
	XoCoreWidget currxow = (XoCoreWidget)current;
 	XoCoreWidget newxow = (XoCoreWidget)new;

	if(currxow->xocore.bg != newxow->xocore.bg){ 
	 XoXtVaSetValues(new, XtNbackground, 
			  newxow->xocore.bg, NULL);
	 ret = True;
	}
	return(ret);
}

/*ARGSUSED*/

static void Realize(w, valueMask, attributes)
 Widget w;
 Mask *valueMask;
 XSetWindowAttributes *attributes;
{
 /* 	SetGlobalResources(w); */
 	(*xocoreWidgetClass->core_class.superclass->core_class.realize)
        	(w,valueMask,attributes);
}


    
/***************************************************************************
  Local function definitions
  */

static void SetGlobalResources(w)
Widget w;
{
  XoXtVaSetValues(w,XtNcolormap,XoGetUsedColormap(),NULL);
}
 
