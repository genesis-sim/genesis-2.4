/* $Id: Dialog.c,v 1.2 2005/07/01 10:02:50 svitak Exp $ */
/*
 * $Log: Dialog.c,v $
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.9  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.8  2000/05/02 06:18:07  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.7  1995/09/26 23:56:40  venkat
 * Changed-XtVaSetValues()-calls-to-XoXtVaSetValues()
 *
 * Revision 1.6  1995/04/06  23:54:00  venkat
 * Changed text_translations to revert to 1.4
 * behaviour for mouse button clicks on the text part.
 *
 * Revision 1.5  1995/04/04  00:54:48  venkat
 * SetValues() changed to handle bg based on xocomposite
 * resources.
 *
 * Revision 1.4  1995/03/06  19:30:04  venkat
 * Modified class initialization to subclass from XoComposite
 * Foreground and background resources used from XoComposite
 *
 * Revision 1.3  1994/06/30  21:52:39  bhalla
 * Fixed a really difficult bug with the previous keypress-related change,
 * which was mutilating the XUPDATE events and causing all sorts of misery.
 *
 * Revision 1.2  1994/06/30  14:56:58  bhalla
 * Added code to handle individual key presses, and also deal with
 * changes to the Text widget
 *
 * Revision 1.1  1994/03/22  15:24:10  bhalla
 * Initial revision
 * */
/* THERE ARE SOME MINOR CHANGES THAT HAVE BEEN MADE TO THIS FILE.
 * Search for @@JASON for details.
 */



#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Widg/Form.h>
#include <Widg/Button.h>
#include <Widg/Framed.h>
#include <Widg/Text.h>
#include "DialogP.h"
#include "Xo/XoDefs.h"



static XtResource resources[] = {
#define offset(field) XtOffset(DialogWidget, dialog.field)
  {XtNcallback, XtCCallback, XtRCallback,   sizeof(XtPointer),
     offset(callbacks), XtRCallback, (XtPointer) NULL},
  {XtNvalue, XtCString, XtRString,   sizeof(XtPointer),
     offset(value), XtRString, (XtPointer) NULL},
  {XtNlabel, XtCString, XtRString,   sizeof(XtPointer),
     offset(label), XtRString, (XtPointer) NULL},
  {XtNfont, XtCFont, XtRFontStruct,   sizeof(XFontStruct *),
     offset(font), XtRString, (XtPointer) "XtDefaultFont"},
#undef offset
};


/* Methods */
static void Initialize();
static Boolean SetValues();
 
static void XoCallDialogCallbackFn();

/* actions */
static void 
  Notify();

static XtActionsRec actions[] =
{
    {"notify",	Notify},
};

static char translations[] =
"<BtnDown>:		notify()	\n\
";

static char text_translations[] =
"<Btn1Down>:		mouse-button1()	\n\
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

/*
static char text_translations[] = 
"<BtnDown>:		notify()	\n\
 <Key>Return:		notify()	\n\
";
*/

DialogClassRec dialogClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &frameClassRec,
    /* class_name		*/	"Dialog",
    /* widget_size		*/	sizeof(DialogRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
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
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* composite fields */
    /* geometry_manager         */      XtInheritGeometryManager,
    /* change_managed           */      XtInheritChangeManaged,
    /* insert_child             */      XtInheritInsertChild,
    /* delete_child             */      XtInheritDeleteChild,
    /* extension                */      NULL
  },
  { /* xocomposite fields */
    /* empty                    */      0
  },
  { /* frame fields */
    /* empty                    */      0
  },
  { /* dialog fields */
    /* empty			*/	0
  }
};

WidgetClass dialogWidgetClass = (WidgetClass)&dialogClassRec;

static void Initialize (request, new, arg_list, arg_num)
     Widget     request, new;
     ArgList    arg_list;
     Cardinal   *arg_num;
{
  DialogWidget dw = (DialogWidget) new;
  char	*label;


  dw->dialog.form = 
    XtVaCreateManagedWidget ("form", formWidgetClass, (Widget) dw,
			     XtNborderWidth, 0,
			     NULL);

  label = (dw->dialog.label != NULL ) ? dw->dialog.label: dw->core.name;
  dw->dialog.label = XtNewString(label);
  dw->dialog.button = 
    XtVaCreateManagedWidget ("button", buttonWidgetClass, dw->dialog.form,
			     XtNonLabel, XtNewString(label),
			     XtNoffLabel, XtNewString(label),
			     XtNwRequested, XoNaturalGeometry,
			     XtNhRequested, XoNaturalGeometry,
				 XtNonFont, dw->dialog.font,
				 XtNoffFont, dw->dialog.font,
				 XtNonFg, dw->xocomposite.fg,
				 XtNoffFg, dw->xocomposite.fg,
				 XtNonBg, dw->xocomposite.bg,
				 XtNoffBg, dw->xocomposite.bg,
			     NULL);


/* CHANGED BY JASON */
  dw->dialog.text_framed =
    XtVaCreateManagedWidget ("text", framedWidgetClass, dw->dialog.form,
			     XtNchildClass, textWidgetClass,
			     XtNeditable, True,		/* Make the text widget an editable one (@@JASON) */
			     XtNelevation, XoInsetFrame,
			     XtNxRequested, 4,
			     XtNxReference, dw->dialog.button,
			     XtNxReferenceEdge, XoReferToRight,
			     XtNyReference, XoReferToParent,
			     XtNwRequested, 4,
			     XtNwReference, XoReferToParent,
			     XtNwReferenceEdge, XoReferToRight,
			     XtNhRequested, XoNaturalGeometry,
			     XtNjustify, XoJustifyLeft,
				 XtNfont, dw->dialog.font,
				 XtNfg, dw->xocomposite.fg,
				 XtNbg, dw->xocomposite.bg,
			     NULL);
			    /* XtNwRequested, XoNaturalGeometry, */
				 
  
  XoSizeForm (dw->dialog.form);

  /* set the dialog size */
  dw->core.width = dw->dialog.form->core.width + 2 * dw->frame.frame_width;
  dw->core.height = dw->dialog.form->core.height + 2 * dw->frame.frame_width;
  dw->dialog.value =
	XoGetTextValue(XoGetFramedChild(dw->dialog.text_framed));

  XtSetKeyboardFocus(dw->dialog.form, XoGetFramedChild(dw->dialog.text_framed));
  /* printf("adding callback to button\n"); */
  XtAddCallback(dw->dialog.button,
	XtNcallback,XoCallDialogCallbackFn,(XtPointer)dw);
  /*
  XtAddCallback(XoGetFramedChild(dw->dialog.button),
	XtNcallback,XoCallDialogCallbackFn,(XtPointer)dw);
	*/

  /* printf("adding callback to text\n"); */
  XtAddCallback(XoGetFramedChild(dw->dialog.text_framed),
	XtNcallback,XoCallDialogCallbackFn,(XtPointer)dw);

  /*
  XtParseTranslationTable(text_translations);
  */
  XoXtVaSetValues(XoGetFramedChild(dw->dialog.text_framed),
	XtNtranslations, XtParseTranslationTable(text_translations),
	NULL);
  /*
  XtParseTranslationTable(text_translations);
  XtAugmentTranslations(XoGetFramedChild(dw->dialog.text_framed),
	text_translations);
	*/
}

static Boolean SetValues(curr,req,new,args,num_args)
	Widget curr,req,new;
	ArgList args;
	Cardinal    *num_args;
{
	DialogWidget dnw = (DialogWidget) new;
	DialogWidget dcw = (DialogWidget) curr;

	if (dnw->dialog.value != dcw->dialog.value) {
		dnw->dialog.value =
			XoSetTextValue(XoGetFramedChild(dnw->dialog.text_framed),
			dnw->dialog.value,0);
			/* We want the widget to resize every time the text
			** changes, so the do_resize flag is set to 1 */
	}
	if (dnw->dialog.label != dcw->dialog.label) {
		if (dcw->dialog.label != dcw->core.name)
			XtFree( (char *)dcw->dialog.label);
		if (dnw->dialog.label != dnw->core.name)
			dnw->dialog.label = XtNewString(dnw->dialog.label);
		XoXtVaSetValues(dnw->dialog.button,
			XtNonLabel,dnw->dialog.label,
			XtNoffLabel,dnw->dialog.label,
			NULL);
		return(True);
	}

	if (dnw->xocomposite.fg != dcw->xocomposite.fg) {
		XoXtVaSetValues(XoGetFramedChild(dnw->dialog.text_framed),
			XtNfg, dnw->xocomposite.fg,
			 NULL);
		XoXtVaSetValues(dnw->dialog.button,
			 XtNonFg, dnw->xocomposite.fg,
			 XtNoffFg, dnw->xocomposite.fg,
			 NULL);
		return(True);
	}

	if (dnw->dialog.font != dcw->dialog.font) {
		XoXtVaSetValues(XoGetFramedChild(dnw->dialog.text_framed),
			XtNfont, dnw->dialog.font,
			 NULL);
		XoXtVaSetValues(dnw->dialog.button,
			 XtNonFont, dnw->dialog.font,
			 XtNoffFont, dnw->dialog.font,
			 NULL);
		return(True);
	}

	if (dnw->xocomposite.bg != dcw->xocomposite.bg) {
		XoXtVaSetValues(XoGetFramedChild(dnw->dialog.text_framed),
			 XtNbg, dnw->xocomposite.bg,
			 NULL);
		XoXtVaSetValues(dnw->dialog.button,
			 XtNonBg, dnw->xocomposite.bg,
			 XtNoffBg, dnw->xocomposite.bg,
			 NULL);
		return(True);
	}
	return(False);
}


#include <X11/keysym.h>
 
/* This function routes all the callbacks from the text widget and
** button etc into the dialog callback */
static void XoCallDialogCallbackFn(w,client_data,call_data)
    Widget w;
    XtPointer   client_data,call_data;
{
    XoEventInfo     *info = (XoEventInfo *) call_data;
    DialogWidget    dw  = (DialogWidget)client_data;
 
    dw->dialog.value= 
        XoGetTextValue(XoGetFramedChild(dw->dialog.text_framed));
 
	/* temporary hack: the 0 is when a mouse click occurred */
	if ((info->event & XoKeyPress) && 
		(info->key == XK_Return || info->key == 0)) {
        	info->event = XoPress | XoBut1;
		/* brutally hacking it to force it to
                            ** behave like a mouse button press */
	}
    if (info->event & XoPress) {
    	info->ret=dw->dialog.value;
    	info->x=info->y=info->z = 0.0;
	}
#if 0
    if ((info->event & (XoBut1 | XoBut2 | XoBut3)) == 0)
        info->event |= XoBut1; /* Hack to pass off all returns as
                                ** a mouse click */
#endif
 
    XtCallCallbackList((Widget) dw,dw->dialog.callbacks,(XtPointer)info);
}


/*ARGUSED*/
static void Notify(w,event,params,num_params)
     Widget w;
	 XButtonEvent	*event;
	 String			*params;
	 Cardinal		*num_params;
{
  DialogWidget dw = (DialogWidget) w;
  static XoEventInfo	info;

  info.event  = 0;
	switch(event->button) {
		case Button1: info.event |= XoBut1 ;
			break;
		case Button2: info.event |= XoBut2 ;
			break;
		case Button3: info.event |= XoBut3 ;
			break;
	}
	info.event |= XoPress;
	info.ret=dw->dialog.value;
	info.x=info.y=info.z = 0.0;

	XtCallCallbackList((Widget) dw,dw->dialog.callbacks,(XtPointer)&info);
}

void XoSetDialogValue(w,str)
     Widget w;
	 char	*str;
{
  DialogWidget dw = (DialogWidget) w;

	dw->dialog.value =
  		XoSetTextValue(XoGetFramedChild(dw->dialog.text_framed),str,0);
}
