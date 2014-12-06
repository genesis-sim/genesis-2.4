/* $Id: Framed.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Framed.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  2000/05/02 06:18:08  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.2  1995/03/06 19:34:42  venkat
 * Class Initialization modified to subclass from XoComposite
 *
 * Revision 1.1  1994/03/22  15:19:02  bhalla
 * Initial revision
 * */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "FramedP.h"
#include "Widg/LabelP.h"

static XtResource resources[] = {
#define offset(field) XtOffset(FramedWidget, framed.field)
  /* {name, class, type, size, offset, default_type, default_addr}, */
  { XtNchildClass, XtCChildClass, XtRWidgetClass, sizeof(WidgetClass),
    offset(child_class), XtRImmediate, (XtPointer) (&labelClassRec)},
#undef offset
};

static void 
  Initialize();
#define SuperClass (&frameClassRec)

FramedClassRec framedClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) SuperClass,
    /* class_name		*/	"Framed",
    /* widget_size		*/	sizeof(FramedRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
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
    /* set_values		*/	NULL,
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
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child       */    XtInheritInsertChild,
    /* delete_child       */    XtInheritDeleteChild,
    /* extension          */    NULL
  },
  { /* xocomposite fields */
    /* empty                    */      0
  },
  { /* frame fields */
    /* empty                    */      0
  },
  { /* framed fields */
    /* empty			*/	0
  }
};

WidgetClass framedWidgetClass = (WidgetClass)&framedClassRec;

#define LOCAL_ARGS 2
static void Initialize (request, new, arg_list, arg_num)
     Widget     request, new;
     ArgList    arg_list;
     Cardinal   *arg_num;
{
  FramedWidget fw = (FramedWidget) new;
  if (fw->framed.child_class) {
    char name[100];
    ArgList child_list;
    Cardinal child_num;
	Widget cw;
	/* Cursor XoDefaultCursor(); */

    /* Create child */
    /* This should call ChangeManaged, which calls PlaceChild */

    child_list = (ArgList) XtMalloc ((*arg_num + LOCAL_ARGS) * sizeof (Arg));
    child_num = 0;

    /* Set the args that need to be passed down */
    XtSetArg (child_list[child_num], XtNbackground, 
	      fw->core.background_pixel);
    child_num++;
    XtSetArg (child_list[child_num], XtNbackgroundPixmap,
	      fw->core.background_pixmap);
    child_num++;

    XoCopyUnknownArgs (fw, child_list, &child_num, arg_list, *arg_num);

    XoLowerName (name, fw->framed.child_class->core_class.class_name);
    cw = XtCreateManagedWidget (name, fw->framed.child_class, (Widget) fw, 
			   child_list, child_num);

    /* now that we have a child we might as well pay attention to it */
    fw->core.width = fw->composite.children[0]->core.width 
                                                  + 2 * fw->frame.frame_width;
    fw->core.height = fw->composite.children[0]->core.height 
                                                  + 2 * fw->frame.frame_width;
	/* Set the default cursor for the child */
	/*
	if (XtIsRealized(cw))
		XDefineCursor(XtDisplay(cw),XtWindow(cw),XoDefaultCursor());
	*/
	/*
    printf("creating framedw %s (%d,%d) child %s is (%d,%d)\n",
        fw->core.name,fw->core.width,fw->core.height,
        fw->composite.children[0]->core.name,
        fw->composite.children[0]->core.width,
        fw->composite.children[0]->core.height);
	*/
  }
}

Widget XoGetFramedChild(fw)
	FramedWidget	fw;
{
	/* Check the class of fw */
	/* Return the child */
	return(fw->composite.children[0]);
}


