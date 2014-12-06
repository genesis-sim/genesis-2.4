/* $Id: Label.c,v 1.2 2005/07/01 10:02:50 svitak Exp $ */
/*
 * $Log: Label.c,v $
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.8  2000/07/03 18:14:17  mhucka
 * Added missing type casts.
 *
 * Revision 1.7  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.6  1995/07/29 00:17:24  venkat
 * The label resource is being alloced and realloced within the widget methods
 * and so the destroy method must free them.
 *
 * Revision 1.5  1995/07/08  01:06:02  venkat
 * label->label is string resource and is now freed in the genesis side
 * DELETE action for the label.
 *
 * Revision 1.4  1995/06/02  19:46:33  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.3  1995/03/06  20:00:39  venkat
 * SetValues() and GetGC() changed to determine
 * gc based on the XoCore fg and bg resources.
 * Also changed Class Initilization record to subclass
 * from XoCore
 *
 * Revision 1.2  1994/12/16  20:57:40  venkat
 * label checked for NULL before freeing
 *
 * Revision 1.1  1994/03/22  15:18:26  bhalla
 * Initial revision
 * */

/* #define JASONDEBUG */


/*
 * Label.c - Label widget
 * Search for @@JASON for functions added by Jason.
 * There are some additional changes made to Label.c that Jason
 * did not label so the best thing to do is to use this version of label.c
 *
 * Note: Labels and hence Text widgets are limited to having a maximum of 
 * 32767 characters in them.  The Text widget uses much of the label
 * widget code which deals with everything in integers.  Hence the 32767
 * limit.  Maneesh decided it, not me.
 */

#define XtStrlen(s)		((s) ? strlen(s) : 0)

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "Widg/FramedP.h"
#include "LabelP.h"

#define streq(a,b) (strcmp( (a), (b) ) == 0)

#define MULTI_LINE_LABEL 32767

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(LabelWidget, field)
static XtResource resources[] = {
	{XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(label.font), XtRString, "XtDefaultFont"},
	{XtNlabel, XtCLabel, XtRString, sizeof(String),
	offset(label.label), XtRString, NULL},
	{XtNjustify, XtCJustify, XtRJustify, sizeof(XoJustify),
	offset(label.justify), XtRImmediate, (caddr_t) XoJustifyCenter},
	{XtNinternalWidth, XtCWidth, XtRDimension, sizeof(Dimension),
	offset(label.internal_width), XtRImmediate, (caddr_t) 4},
	{XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(label.internal_height), XtRImmediate, (caddr_t) 2},
/*    {XtNbitmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
	offset(label.pixmap), XtRImmediate, (caddr_t)None},  */
	{XtNresize, XtCResize, XtRBoolean, sizeof(Boolean),
	offset(label.resize), XtRImmediate, (caddr_t) True},
};

static void Initialize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static void ClassInitialize();
static void Destroy();
static XtGeometryResult QueryGeometry();

static void XoRegisterJustifyConverter();

void  Recalculate();

#define superclass		(&xocoreClassRec)

LabelClassRec labelClassRec = {
	{
/* core_class fields */
		 /* superclass	  	 */ (WidgetClass) superclass,
		 /* class_name	  	 */ "Label",
		 /* widget_size	  	 */ sizeof(LabelRec),
		 /* class_initialize   	 */ ClassInitialize,
		 /* class_part_initialize	 */ NULL,
		 /* class_inited       	 */ FALSE,
		 /* initialize	  	 */ Initialize,
		 /* initialize_hook		 */ NULL,
		 /* realize		  	 */ XtInheritRealize,
		 /* actions		  	 */ NULL,
		 /* num_actions	  	 */ 0,
		 /* resources	  	 */ resources,
		 /* num_resources	  	 */ XtNumber(resources),
		 /* xrm_class	  	 */ NULLQUARK,
		 /* compress_motion	  	 */ TRUE,
		 /* compress_exposure  	 */ TRUE,
		 /* compress_enterleave	 */ TRUE,
		 /* visible_interest	  	 */ FALSE,
		 /* destroy		  	 */ Destroy,
		 /* resize		  	 */ Resize,
		 /* expose		  	 */ Redisplay,
		 /* set_values	  	 */ SetValues,
		 /* set_values_hook		 */ NULL,
		 /* set_values_almost	 */ XtInheritSetValuesAlmost,
		 /* get_values_hook		 */ NULL,
		 /* accept_focus	 	 */ NULL,
		 /* version			 */ XtVersion,
		 /* callback_private   	 */ NULL,
		 /* tm_table		   	 */ NULL,
		 /* query_geometry		 */ QueryGeometry,
		 /* display_accelerator	 */ XtInheritDisplayAccelerator,
		 /* extension		 */ NULL
	},
/* xocore fields */
	{
		 /* empty */ 0
	},
/* label fields */
	{
		 /* empty */ 0
	}
};
WidgetClass labelWidgetClass = (WidgetClass) & labelClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void 
ClassInitialize()
{
	XoRegisterJustifyConverter();
/*  XoRegisterBitmapConverter(); */
}				/* ClassInitialize */

/*
 * Calculate width and height of displayed text in pixels
 */

static void 
SetTextWidthAndHeight(lw)
LabelWidget lw;
{
	register XFontStruct *fs = lw->label.font;
	char *nl;
	char *index();

	/* Minimum height is 1 character. */
	lw->label.label_height = fs->max_bounds.ascent + fs->max_bounds.descent;

	/* If there is no label then set the label's length (in characters) to 0 and
	 * the label's width to 0.
	 */
	if (lw->label.label == NULL) {
		lw->label.label_len = 0;
		lw->label.label_width = 0;
	}

	/* Otherwise look for a newline */
	else if ((nl = index(lw->label.label, '\n')) != NULL) {
		char *label;


		/* If there is a newline then assume there are going to be more
		 * newlines and increment heights appropriately.
		 */

		/* set label_len to MULTI_LINE_LABEL so that the Redisplay function
		 * can know to show that there are more than 1 line of text.
		 */
		lw->label.label_len = MULTI_LINE_LABEL;
		lw->label.label_width = 0;

		/* Scan to each newline character */
		for (label = lw->label.label; nl != NULL; nl = index(label, '\n')) {

			/* Compute the width of the line */
			Dimension width = XTextWidth(fs, label, (int) (nl - label));

			/* Get the largest line width so far found. */
			if (width > lw->label.label_width)
				lw->label.label_width = width;

			/* Go to next character after newline */
			label = nl + 1;

			/* If there are more characters then increment the label height 
			 * Actually I don't think this is correct becoz if the last
			 * character was a newline then the cursor would not be visible
			 * on the next line becoz the label's height would be one character
			 * short.  So I'm going to comment the if below out.
			 * That means that if a newline is the last character in a label
			 * an extra line would be added thus making the label 1 character taller.
			 */

/* @@HJASON */
/*			if (*label) */
				lw->label.label_height +=
					fs->max_bounds.ascent + fs->max_bounds.descent;
		}
		if (*label) {
			Dimension width = XTextWidth(fs, label, strlen(label));

			if (width > lw->label.label_width)
				lw->label.label_width = width;
		}
	}

	/* If there is no newline then just compute the width as the width of
	 * the label.
	 */
	else {
		lw->label.label_len = strlen(lw->label.label);
		lw->label.label_width =
			XTextWidth(fs, lw->label.label, (int) lw->label.label_len);
	}
}

static void 
GetGC(lw)
LabelWidget lw;
{
	XGCValues values;

	
	values.foreground = lw->xocore.fg;
	values.font = lw->label.font->fid;

	lw->label.gc = XtGetGC(
			       (Widget) lw,
			 (unsigned) GCForeground | GCFont,
			       &values);
}


/* ARGSUSED */
static void 
Initialize(request, new)
Widget request, new;
{
	LabelWidget lw = (LabelWidget) new;

#ifdef JASONDEBUG
	printf("in Label Initialize\n");
#endif
	/* If label has no name then copy the core widget's name.
	 * This copying may cause memory leaks later for Text widgets
	 * because that pointer is overwritten by the text string in the
	 * text widget.  We need to make sure that this memory
	 * gets deallocated before reassignment of the label.label pointer.
	 */
	if (lw->label.label == NULL)
		lw->label.label = XtNewString(lw->core.name);
	else {
		/* If it has one then duplicate it. */
		lw->label.label = XtNewString(lw->label.label);
	}

#ifdef JASONDEBUG
	printf("at Label Initialize Label says: %s\n",lw->label.label);
#endif
	/* Get the graphics context and store it in lw->label.gc 
	 * This also gets the resources for fonts and foreground color
	 * and applies them to the GC so that the text is at the appropriate color and font.
	 */
	GetGC(lw);

	/* Determine the text width and height */
	SetTextWidthAndHeight(lw);

	/* Internal width and height is defaulted to 4 and 2 respectively by resource
	 * table.  I believe this adds a border of space around the text.
	 * By assigning this to core we can produce a core widget that's slightly larger
	 * than the text.
	 */
	if (lw->core.width == 0)
		lw->core.width = lw->label.label_width + 2 * lw->label.internal_width;
	if (lw->core.height == 0)
		lw->core.height = lw->label.label_height + 2 * lw->label.internal_height;

	/* Set the label's x & y position to zero */
	lw->label.label_x = lw->label.label_y = 0;

	/* Call the core class's resize */
	(*XtClass(new)->core_class.resize) ((Widget) lw);
	
}

/* Realize the label widget */
/* 
static void Realize(w, valueMask, attributes)
 Widget w;
 XtValueMask *valueMask;
 XSetWindowAttributes *attributes;
{
(*XtClass(w)->core_class.realize) (w, valueMask,attributes);
}

*/


/*
 * Repaint the widget window
 */

/* ARGSUSED */
static void 
Redisplay(w, event, region)
Widget w;
XEvent *event;
Region region;
{
	LabelWidget lw = (LabelWidget) w;
	GC gc;
	int len = lw->label.label_len;
	char *label = lw->label.label;
	Position y = lw->label.label_y + lw->label.font->max_bounds.ascent;
	char *index();

	if (region != NULL &&
	    XRectInRegion(region, lw->label.label_x, lw->label.label_y,
			  lw->label.label_width, lw->label.label_height)
	    == RectangleOut)
		return;

#ifdef JASONDEBUG
	printf("In Label Redisplay\n");
#endif

	gc = lw->label.gc;
	if (len == MULTI_LINE_LABEL) {
		char *nl;

		while ((nl = index(label, '\n')) != NULL) {
			XDrawString(
			XtDisplay(w), XtWindow(w), gc, lw->label.label_x,
				    y, label, (int) (nl - label));
			y += lw->label.font->max_bounds.ascent + lw->label.font->max_bounds.descent;
			label = nl + 1;
		}

/* !!!!!! Warning: Label depends greatly on a /0 at the end of the string */
		len = strlen(label);
	}
	if (len)
		XDrawString(
			XtDisplay(w), XtWindow(w), gc, lw->label.label_x,
			    y, label, len);
}

static void 
RequestResize(lw)
register LabelWidget lw;
{
	Dimension w, h, wret, hret;
	XtGeometryResult answer;

	/* If label.resize == 0, don't resize. */
	if (!lw->label.resize)
		return;

#ifdef JASONDEBUG
	printf("Label: Requesting resize \n");
#endif
	/* widht and height */
	w = lw->label.label_width + 2 * lw->label.internal_width +
		2 * lw->core.border_width;
	h = lw->label.label_height + 2 * lw->label.internal_height +
		2 * lw->core.border_width;


	/* Request parent to change child's size.
	 * wret & hret return the compromised size.  If parent is unwilling
	 * to compromise, then answer is NULL
	 */
	answer = XtMakeResizeRequest((Widget) lw, w, h, &wret, &hret);

	if (answer != XtGeometryNo) {
		/* Apply compromised size */
		XtMakeResizeRequest((Widget) lw, wret, hret, NULL, NULL);
#ifdef JASONDEBUG
		printf("Resize request OK\n");
#endif
	} else {
#ifdef JASONDEBUG
		printf("Resize request rejected\n");
#endif
		;
	}


}

static void 
_Reposition(lw, width, height, dx, dy)
register LabelWidget lw;
Dimension width, height;
Position *dx, *dy;
{
	Position newPos = 0;

	/* Check the justification */
	switch (lw->label.justify) {

	/* If left justification then set left position to internal_width */
	case XoJustifyLeft:
		newPos = lw->label.internal_width;
		break;

	/* If right justification then set left position to right justify
	 * from the core width.  The width parameter is usually the core width.
	 */
	case XoJustifyRight:
		newPos = width -
			(lw->label.label_width + lw->label.internal_width);
		break;

	case XoJustifyCenter:
		newPos = (Position)(width - lw->label.label_width) / 2;
		break;
	}

	/* Make sure that the leftmost position can get no smaller than the internal_width. */
	if (newPos < (Position) lw->label.internal_width)
		newPos = lw->label.internal_width;

	/* This is the change in label position */
	*dx = newPos - lw->label.label_x;
	lw->label.label_x = newPos;

	/* The new y position is computed as the center of the core height */
	newPos = (Position)(height - lw->label.label_height) / 2;

	*dy =  newPos - lw->label.label_y;
/*	lw->label.label_y = newPos;*/

	/* Instead of centering it, I am simply going to stick it at the top
	 * because it makes it easier for text widgets.
	 */
	lw->label.label_y = lw->label.internal_height;
	return;
}


static void 
Resize(w)
Widget w;
{
	LabelWidget lw = (LabelWidget) w;
	Position dx, dy;

#ifdef JASONDEBUG
	printf("in label Resize\n");
#endif

	/* This only repositions the label widget depending on the
	 * justification and core width and heights
	 */
	_Reposition(lw, w->core.width, w->core.height, &dx, &dy);

}

/*
 * Set specified arguments into widget
 */

#define PIXMAP 0
#define WIDTH 1
#define HEIGHT 2
#define NUM_CHECKS 3

static Boolean 
SetValues(current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
	LabelWidget curlw = (LabelWidget) current;
	LabelWidget reqlw = (LabelWidget) request;
	LabelWidget newlw = (LabelWidget) new;
	int i;
	Boolean was_resized = False, redisplay = True, checks[NUM_CHECKS];

#ifdef JASONDEBUG
	printf("in Label SetValues num_args: %d args: %s\n", *num_args, *args);
#endif

	for (i = 0; i < NUM_CHECKS; i++)
		checks[i] = FALSE;

	for (i = 0; i < *num_args; i++) {
		if (streq(XtNbitmap, args[i].name))
			checks[PIXMAP] = TRUE;
		if (streq(XtNwidth, args[i].name))
			checks[WIDTH] = TRUE;
		if (streq(XtNheight, args[i].name))
			checks[HEIGHT] = TRUE;
	}

	/* If there is no label then use the core widget's */
	if (newlw->label.label == NULL) {
		newlw->label.label = XtNewString(newlw->core.name);
	}

	/* If the label has been altered ... */
	if (curlw->label.label != newlw->label.label) {


		/* Free the current label; I don't know if this is necessary !!!!! */
		if (curlw->label.label != curlw->core.name)
			XtFree((char *) curlw->label.label);


		/* Copy the new label */
		if (newlw->label.label != newlw->core.name) {
			newlw->label.label = XtNewString(newlw->label.label);
		}
		was_resized = True;
	}

	/* If either a resize due to label change, or change in fontm or justification
	 * then compute new width and height of label.
	 */
	if (was_resized || (curlw->label.font != newlw->label.font) ||
	    (curlw->label.justify != newlw->label.justify) || checks[PIXMAP]) {

		SetTextWidthAndHeight(newlw);
		was_resized = True;
	}

	/* recalculate the window size if something has changed. */

#ifdef JASONDEBUG
	printf("core width: %d %d\n",curlw->core.width, newlw->core.width);
	printf("core height: %d %d\n",curlw->core.height, newlw->core.height);

	printf("resize flag: %d  was_received: %d\n",newlw->label.resize,
	       was_resized);


	printf("checkwidth %d  checkheight %d\n",checks[WIDTH], checks[HEIGHT]);


	printf("lable size: w=%d h=%d\n", newlw->label.label_width,
	       newlw->label.label_height);

#endif
	if (newlw->label.resize && was_resized) {


		if ((curlw->core.width == reqlw->core.width && !checks[WIDTH])) {
#ifdef JASONDEBUG
			printf("label resizing core width\n");
#endif
			newlw->core.width = (newlw->label.label_width +
					2 * newlw->label.internal_width);
		}

		if ((curlw->core.height == reqlw->core.height && !checks[HEIGHT])) {
#ifdef JASONDEBUG
			printf("label resizing core height\n");
#endif
			newlw->core.height = (newlw->label.label_height +
				       2 * newlw->label.internal_height);
		}
	}



	redisplay = True;
	/* If foreground has changed or font has changed then adjust the GC */
	if (curlw->xocore.fg != newlw->xocore.fg 
	     || curlw->label.font->fid != newlw->label.font->fid) {

		XtReleaseGC(new, curlw->label.gc);
		GetGC(newlw);
		redisplay = True;
	}

	/* If the internal sizes have changed then reposition widget */
	if ((curlw->label.internal_width != newlw->label.internal_width)
	|| (curlw->label.internal_height != newlw->label.internal_height)
	    || was_resized) {
		/* Resize() will be called if geometry changes succeed */
		Position dx, dy;

		_Reposition(newlw, curlw->core.width, curlw->core.height, &dx, &dy);
	}

	return was_resized || redisplay ||
		XtIsSensitive(current) != XtIsSensitive(new);
}

static void 
Destroy(w)
Widget w;
{
	LabelWidget lw = (LabelWidget) w;

	XtReleaseGC(w, lw->label.gc);
    	if(lw->label.label!=NULL) XtFree(lw->label.label);
}


static XtGeometryResult 
QueryGeometry(w, intended, preferred)
Widget w;
XtWidgetGeometry *intended, *preferred;
{
	register LabelWidget lw = (LabelWidget) w;

	preferred->request_mode = CWWidth | CWHeight;
	preferred->width = lw->label.label_width + 2 * lw->label.internal_width;
	preferred->height = lw->label.label_height + 2 * lw->label.internal_height;
	if (((intended->request_mode & (CWWidth | CWHeight))
	     == (CWWidth | CWHeight)) &&
	    intended->width == preferred->width &&
	    intended->height == preferred->height)
		return XtGeometryYes;
	else if (preferred->width == w->core.width &&
		 preferred->height == w->core.height)
		return XtGeometryNo;
	else
		return XtGeometryAlmost;
}


/***************************************************************************
  Utilities:
*/

static XrmQuark
       XtQleft, XtQright, XtQcenter;

static void 
XoRegisterJustifyQuarks()
{
	XtQleft = XrmStringToQuark("left");
	XtQcenter = XrmStringToQuark("center");
	XtQright = XrmStringToQuark("right");
}

/* ARGSUSED */
static Boolean
XoCvtStringToJustify(dpy, args, num_args, fromVal, toVal, destruct)
Display *dpy;			/* unused */
XrmValuePtr args;		/* unused */
Cardinal *num_args;		/* unused */
XrmValuePtr fromVal;
XrmValuePtr toVal;
XtPointer destruct;		/* unused */
{
	static XoJustify justify;
	XrmQuark q;
	char lowerName[1000];

	XoLowerName(lowerName, (char *) fromVal->addr);
	q = XrmStringToQuark(lowerName);
	if (q == XtQleft)
		justify = XoJustifyLeft;
	else if (q == XtQright)
		justify = XoJustifyRight;
	else if (q == XtQcenter)
		justify = XoJustifyCenter;
	else {
		XtStringConversionWarning(fromVal->addr, "justify");
	 	toVal->size = sizeof(XoJustify);
		return((Boolean)False);		
	}
	XoCvtDone(XoJustify,justify);
}

static void 
XoRegisterJustifyConverter()
{
	XoRegisterJustifyQuarks();
	XtSetTypeConverter(XtRString, XtRJustify,
			   (XtTypeConverter) XoCvtStringToJustify, NULL, 0,
			   XtCacheNone, NULL);
}

#if 0

/* Bitmap display isn't supported yet */

#define done(address, type) \
        { (*toVal).size = sizeof(type); (*toVal).addr = (caddr_t) address; }



/* This came from Xmu.  I changed the name to prevent collisions */
/*ARGSUSED*/
void 
XoCvtStringToBitmap(args, num_args, fromVal, toVal)
XrmValuePtr args;
Cardinal *num_args;
XrmValuePtr fromVal;
XrmValuePtr toVal;
{
	static Pixmap pixmap;	/* static for cvt magic */
	char *name = (char *) fromVal->addr;

	if (*num_args != 1)
		XtErrorMsg("wrongParameters", "cvtStringToBitmap", "XtToolkitError",
		     "String to pixmap conversion needs screen argument",
			   (String *) NULL, (Cardinal *) NULL);

	if (strcmp(name, "None") == 0) {
		pixmap = None;
		done(&pixmap, Pixmap);
		return;
	}

	if (strcmp(name, "ParentRelative") == 0) {
		pixmap = ParentRelative;
		done(&pixmap, Pixmap);
		return;
	}

	pixmap = XmuLocateBitmapFile(*((Screen **) args[0].addr), name,
				     NULL, 0, NULL, NULL, NULL, NULL);

	if (pixmap != None) {
		done(&pixmap, Pixmap);
	}
	else {
		XtStringConversionWarning(name, "Pixmap");
		return;
	}
}




void 
XoRegisterBitmapConverter()
{
	static XtConvertArgRec screenConvertArg[] = {
		{XtWidgetBaseOffset, (caddr_t) XtOffset(Widget, core.screen),
		sizeof(Screen *)}
	};

	XtAddConverter("String", "Bitmap", XoCvtStringToBitmap,
		       screenConvertArg, XtNumber(screenConvertArg));
}

#endif


/*
 * Use this function from subclasses (in particular Text) to force an
 * update of the label display.
 */

void 
Recalculate(w)
Widget w;
{
	LabelWidget lw = (LabelWidget) w;
	Position dx, dy;

	/* Determine the width and height of a label */
	SetTextWidthAndHeight(lw);

	if (lw->label.resize)
		RequestResize(lw);

	/* Reposition label based on core dimensions */
	_Reposition(lw, w->core.width, w->core.height, &dx, &dy);

#ifdef JASONDEBUG
	printf("in Label Recalculate dx= %d dy= %d\n",dx,dy);
#endif

	/* If the widget has been realized, then clear the window
	 * and cause an expose event.
	 */
	if (XtIsRealized(w)) {
		XClearWindow(XtDisplay(w), XtWindow(w));
		(*XtClass(w)->core_class.expose) (w, NULL, NULL);
	}
}


/* @@JASON : This does the same thing as recalculate but overrides the label
 * width and height values with user specified values.  This prevents the
 * label in the text widget to resize itself everytime text is added.
 * If it did there would be no point of having a scroll bar.  Also it would
 * also result in having a humungous text widget.
 */
void 
RecalculateOveride(w,wid,hei)
Widget w;
int wid,hei;
{
	LabelWidget lw = (LabelWidget) w;
	Position dx, dy;

#ifdef JASONDEBUG
	printf("recalculate overide\n");
#endif
	/* Determine the width and height of a label */
	SetTextWidthAndHeight(lw);

	if (wid >=0) lw->label.label_width = wid;
	if (hei >=0) lw->label.label_height = hei;
	if (wid >=0 )lw->core.width = wid;
	if (hei >=0)lw->core.height = hei;

	if (lw->label.resize)
		RequestResize(lw);

	/* Reposition label based on core dimensions */
	_Reposition(lw, w->core.width, w->core.height, &dx, &dy);

#ifdef JASONDEBUG
	printf("in Label Recalculate dx= %d dy= %d\n",dx,dy);
#endif

	/* If the widget has been realized, then clear the window
	 * and cause an expose event.
	 */
	if (XtIsRealized(w)) {
		XClearWindow(XtDisplay(w), XtWindow(w));
		(*XtClass(w)->core_class.expose) (w, NULL, NULL);
	}
}


