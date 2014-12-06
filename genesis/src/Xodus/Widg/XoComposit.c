static char rcsid[] = "$Id: XoComposit.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $";
/*
** $Log: XoComposit.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  1995/09/28 00:40:51  venkat
** Recovered RCS log and Id tags
**
revision 1.5
date: 1995/09/26 23:56:45;  author: venkat;  state: Exp;  lines: +2 -2
Changed-XtVaSetValues()-calls-to-XoXtVaSetValues()
----------------------------
revision 1.4
date: 1995/04/05 17:07:30;  author: venkat;  state: Exp;  lines: +3 -1
The colormap resource is not set here again.
It is done once for the top level shell.
----------------------------
revision 1.3
date: 1995/03/24 23:58:20;  author: venkat;  state: Exp;  lines: +1 -1
 Changed default value of XtNbg resource to XoDefaultBackground.
>> This macro is defined in Xo/XoDefs.h as the default or fallback
>> value for the application, namely "LightSteelBlue"
>> Doing this ensures the match of the XtNbg resource with
>> the default for the Core widget class's XtNbackground resource
----------------------------
revision 1.2
date: 1995/03/06 20:13:13;  author: venkat;  state: Exp;  lines: +1 -1
NULL initialize method in class record initialization
----------------------------
revision 1.1
date: 1995/03/06 18:25:36;  author: venkat;  state: Exp;
Initial revision
=============================================================================
*/
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include "Xo/XoDefs.h"
#include "XoComposiP.h"




static XtResource resources[] = {
#define offset(field) XtOffset(XoCompositeWidget, xocomposite.field)
  { XtNbg, XtCBackground, XtRXoPixel, sizeof (Pixel),
    offset(bg), XtRString, (XtPointer) XoDefaultBackground },
  { XtNfg, XtCForeground, XtRXoPixel, sizeof (Pixel),
    offset(fg), XtRString, (XtPointer) "XtDefaultForeground" },
#undef offset
};


/* Methods */
static Boolean SetValues();
static void Realize();


XoCompositeClassRec xocompositeClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) (&compositeClassRec),
    /* class_name		*/	"XoComposite",
    /* widget_size		*/	sizeof(XoCompositeRec),
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
  { /* composite_class fields */
    /* geometry_manager         */      XtInheritGeometryManager,
    /* change_managed           */      XtInheritChangeManaged,
    /* insert_child             */      XtInheritInsertChild,
    /* delete_child             */      XtInheritDeleteChild,
    /* extension                */      NULL
  },
  { /* xocomposite fields */
    /* empty			*/	0
  }
};

WidgetClass xocompositeWidgetClass = (WidgetClass)&xocompositeClassRec;

/** Local function declarations **/
static void SetGlobalResources(/*Widget*/);

/***************************************************************************
  Methods:
***************************************************************************/



/*ARGSUSED*/

static Boolean SetValues(current, request, new, args, num_args)
    Widget current, request, new;
    ArgList args;
    Cardinal *num_args;
{
	Boolean ret = False;
	XoCompositeWidget currxow = (XoCompositeWidget)current;
 	XoCompositeWidget newxow = (XoCompositeWidget)new;

	if(currxow->xocomposite.bg != newxow->xocomposite.bg){
		
	 XoXtVaSetValues(new,XtNbackground,
			 newxow->xocomposite.bg, NULL); 
		
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
/*  SetGlobalResources(w); */
 (*xocompositeWidgetClass->core_class.superclass->core_class.realize)
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
 
