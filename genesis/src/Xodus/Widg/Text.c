/* $Id: Text.c,v 1.6 2005/07/29 16:17:27 svitak Exp $ */
/*
 * $Log: Text.c,v $
 * Revision 1.6  2005/07/29 16:17:27  svitak
 * Various changes to address MIPSpro compiler warnings.
 *
 * Revision 1.5  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.4  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.3  2005/06/18 18:50:29  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.2  2005/06/17 17:36:54  svitak
 * Deprecated varargs.h replaced by stdarg.h. Old-style argument lists updated.
 * These changes were part of varargs-genesis.patch. Systems without stdarg.h
 * will no longer compile.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.14  2001/06/29 21:09:37  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.13  2000/09/21 19:26:46  mhucka
 * It doesn't seem necessary to add a NULL at the end of the empty varargs
 * calls like "CallAction(Update, w, NULL, NULL)", and what's more, the IRIX
 * compiler flags the NULL as being of the wrong datatype, so I'm going to try
 * removing the trailing NULL altogether and see what happens.
 *
 * Revision 1.12  2000/06/12 04:14:34  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added type casts where appropriate.
 *
 * Revision 1.11  2000/05/02 06:18:08  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.10  1995/06/08 20:29:48  venkat
 * Fixed a minor bug in Redisplay relating to
 * display of dialog values
 *
 * Revision 1.9  1995/06/08  17:49:25  venkat
 * Added code in Redisplay and included function indexn to
 * accomodate reading of tab characters from files. This fix
 * does not allow recognition of tab chars if the text widget is
 * editable.
 *
 * Revision 1.8  1995/05/25  22:28:34  venkat
 * Bounding rect used in checking for the Redisplay event changed from
 * label.label_width and label.label_height to text.actualTextWidth and text.actualTextHeight
 *
 * Revision 1.7  1995/03/06  20:06:44  venkat
 * Changed the class initialization record to subclass XoCore
 *
 * Revision 1.6  1994/12/19  22:46:36  venkat
 * Modified XoSetTextValue() and GetTextWidthAndHeight() to update
 * label and label_len when text buffer is modified
 *
 * Revision 1.5  1994/12/16  20:59:07  venkat
 * label made to follow text buffer in Destroy()
 *
 * Revision 1.4  1994/06/30  21:51:44  bhalla
 * A few bugfixes relating to memory allocation and checking.
 *
 * Revision 1.3  1994/06/29  19:06:07  bhalla
 * Added callback to call the KEYPRESS action after each keystroke
 *
 * Revision 1.2  1994/03/22  15:23:36  bhalla
 * All sorts of changes by Jason and myself.
 * */
/* #define JASONDEBUG */


/* Text Widget code originally by Maneesh Sahani.
 * Updated widget code (and not to mention all the comments!) by Jason Leigh 8/11/93 
 */

#include <stdarg.h>
#include <stdio.h>	/* needed by stdlib.h on some archs */
#include <stdlib.h>	/* atoi(), [cm(re)]alloc() */
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "TextP.h"
#include "Xo/XoDefs.h"


/* Memory allocated to the text buffer is in 4 byte chunks. */
#define TEXT_ALLOC_INCREMENT 4

/* Used to indicate that a text label has more than 1 line delimited by newline characters */
#define MULTI_LINE_LABEL 32767	/* from Label.c */

/* This is the x position where the text label can begin to be drawn. */
#define SCROLL_BAR_MARGIN 20

/* This is the actual width of the margin to be drawn. */
#define SCROLL_BAR_WIDTH SCROLL_BAR_MARGIN-5


/* The margin to leave when there is no scroll bar */
#define NOSCROLL_BAR_MARGIN 5

/* This is the width of the triangular cursor */
#define CURSOR_WIDTH 6

static int      defaultPoint = -9999;
static int 	defaultHeight = 0;

static XtResource resources[] = {
#define offset(field) XtOffset(TextWidget, text.field)
	/* {name, class, type, size, offset, default_type, default_addr}, */

	/* Point determines the location in the text buffer where the editing cursor is
	 * currently pointing.
	 */
	{XtNpoint, XtCpoint, XtRInt, sizeof(int),
	offset(point), XtRInt, (XtPointer) & defaultPoint},

	/* This is the initial text of the text buffer.  This is
	 * separate and is copied to the text buffer.
	 */
	{XtNinitialText, XtCInitialText, XtRString, sizeof(String),
	offset(initial_text), XtRString, ""},

	/* Editable is a flag to indicate whether to allow the text
	 * to be editable by the user
	 */
	{XtNeditable, XtCEditable, XtRBoolean, sizeof(Boolean),
        offset(editable), XtRBoolean, False},

	{XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(callbacks), XtRCallback, (XtPointer) NULL},

	/* Allows changing of the initial height of the text widget */
	{XtNtextHeight, XtCTextHeight, XtRInt, sizeof(int),
        offset(initialHeight), XtRInt, (XtPointer) &defaultHeight}

#undef offset
};

/* methods */
static void Initialize(), Redisplay();


static Boolean SetValues();

/* actions */
static void Update(), SelfInsertCommand(), Insert(), Recenter(), Destroy(),
DeleteChar(), ForwardChar(), BeginningOfLine(),
EndOfLine(), KillToEndOfLine(), KillLine(), Notify(),
SelfInsertReturnCommand(), MouseButton1(),UpdateWithResize(),
GrabScrollMove(), GrabScrollEstablish(), ScrollBarMove();


/* utilities */
static void
                IncrementTextMemory(), CallAction(void (*action) (), Widget widget, XEvent *event, ...), InsertString();
void
                Recalculate();	/* from Label.c */
void
                RecalculateOveride();	/* from Label.c */
void		SetScrollPoint(), SetScrollBarHeight();
static void FollowCursor();
static void  GetTextWidthAndHeight();
static char	*indexn(); /* A variation of the std index(). This is index() on a
				substr */

static XtActionsRec actions[] =
{
	{"self-insert-command", SelfInsertCommand},
	{"self-insert-return-command", SelfInsertReturnCommand},
	{"forward-char", ForwardChar},
	{"recenter", Recenter},
	{"delete-char", DeleteChar},
	{"beginning-of-line", BeginningOfLine},
	{"end-of-line", EndOfLine},
	{"kill-to-end-of-line", KillToEndOfLine},
	{"kill-line", KillLine},
	{"notify", Notify},
	{"mouse-button1",MouseButton1},
	{"grab-scroll-move", GrabScrollMove},
	{"grab-scroll-establish", GrabScrollEstablish},
	{"scroll-bar-move", ScrollBarMove}
};

/* Event translations. */
static char     translations[] =
"Ctrl<Key>a:		beginning-of-line()	\n\
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
 <Key>Return:        	self-insert-return-command()	\n\
 <Key>:	        	self-insert-command()	\n\
 <Btn2Motion>:		grab-scroll-move() \n\
 <Btn2Down>:		grab-scroll-establish() \n\
 <Btn1Down>:		mouse-button1() \n\
 <Btn1Motion>:		scroll-bar-move() \n\
";

/* Text class record */
TextClassRec    textClassRec = {
	{			/* core fields */
		 /* superclass		 */ (WidgetClass) & labelClassRec,
		 /* class_name		 */ "Text",
		 /* widget_size		 */ sizeof(TextRec),
		 /* class_initialize		 */ NULL,
		 /* class_part_initialize	 */ NULL,
		 /* class_inited		 */ FALSE,
		 /* initialize		 */ Initialize,
		 /* initialize_hook		 */ NULL,
		 /* realize			 */ XtInheritRealize,
		 /* actions			 */ actions,
		 /* num_actions		 */ XtNumber(actions),
		 /* resources		 */ resources,
		 /* num_resources		 */ XtNumber(resources),
		 /* xrm_class		 */ NULLQUARK,
		 /* compress_motion		 */ TRUE,
		 /* compress_exposure	 */ TRUE,
		 /* compress_enterleave	 */ TRUE,
		 /* visible_interest		 */ FALSE,
		 /* destroy			 */ Destroy,
		 /* resize			 */ XtInheritResize,
		 /* expose			 */ Redisplay,
		 /* set_values		 */ SetValues,
		 /* set_values_hook		 */ NULL,
		 /* set_values_almost	 */ XtInheritSetValuesAlmost,
		 /* get_values_hook		 */ NULL,
		 /* accept_focus		 */ NULL,
		 /* version			 */ XtVersion,
		 /* callback_private		 */ NULL,
		 /* tm_table			 */ translations,
		 /* query_geometry		 */ XtInheritQueryGeometry,
		 /* display_accelerator	 */ XtInheritDisplayAccelerator,
		 /* extension		 */ NULL
	},
	{			/* xocore fields */
		 /* empty                    */ 0
	},
	{			/* label fields */
		 /* empty                    */ 0
	},
	{			/* text fields */
		 /* empty			 */ 0
	}
};

WidgetClass     textWidgetClass = (WidgetClass) & textClassRec;


/* Initialize the widget */
static void
Initialize(req, new)
	Widget          req, new;
{
	TextWidget      tnew = (TextWidget) new;
	int             savepoint;
	char newlin[2];


	newlin[0] = (char) 10;
	newlin[1] = '\0';

#ifdef JASONDEBUG
	printf("In Text Initialize\n");
#endif

	/* Allocate the first block of memory for the text buffer. */
	tnew->text.text_memory = TEXT_ALLOC_INCREMENT;
	tnew->text.text = (char *) malloc(sizeof(char) * TEXT_ALLOC_INCREMENT);
	*(tnew->text.text) = '\0';
	tnew->text.text_size = 0;


	/* Location in text memory the cursor is pointing to. We save it away so that
	 * if it is altered by a resource by a user, we can use this to jump the cursor
	 * directly to the chosen spot in the text.
	 */
	savepoint = tnew->text.point;

	/* We set the point to zero so that the insertion of the initial text
	 * will begin at the beginning of the buffer.
	 */
	tnew->text.point = 0;

	/* Location of scroll bar and vertical offset to start drawing text. */
	tnew->text.textStartDrawOffset = 0;
	tnew->text.mouseStartX = 0;
	tnew->text.mouseStartY = 0;
	tnew->text.scrollingAccessible = False;

	/* Insert initial text into text buffer. */
	/* Make very sure we do not later free a non-allocated string */
	if (tnew->text.initial_text) {
		char *temp = tnew->text.initial_text;
		tnew->text.initial_text = (char *)calloc((int)strlen(temp) + 1,
			sizeof(char));
		strcpy(tnew->text.initial_text,temp);
		CallAction(Insert, (Widget)tnew, NULL, tnew->text.initial_text, NULL);
	} else {
		tnew->text.initial_text = (char *)calloc(TEXT_ALLOC_INCREMENT,
			sizeof(char));
	}

	/* The default point is -9999.  If a new location was indeed specified
	 * by the point resource then savepoint will no longer be -9999
	 * hence we restore the point to this saved point.
	 * Otherwise we leave point alone so that it can assume the position
	 * at the last character of the text buffer.
	 */
	if (savepoint != defaultPoint)
		tnew->text.point = savepoint;

	/* Assign the label to this text and update the label widget */
	tnew->label.label = tnew->text.text;

	/* Updating involves recalculating the size of the label and then drawing it.
	 * But note I have forced the default size of the text widget to be 200 by 200.
	 * THIS MUST BE CHANGED TO ALLOW SUCH SIZE SPECIFICATIONS TO BE MADE FROM
	 * WITHIN AN OPTION IN THE CREATION PHASE.
	 */

	if (tnew->text.initialHeight)
	RecalculateOveride((Widget) tnew, 200,tnew->text.initialHeight);
	else
        Recalculate((Widget) tnew);


	/* Because of some unexplainable glitch somewhere, it is generally best to assume
	 * that all text will be multi line. (@@WEIRD)
	 */
	tnew->label.label_len = MULTI_LINE_LABEL;


	CallAction(Update, (Widget)tnew, NULL, NULL);
}


/* Draws the text, cursor, and scrollbar in the label widget */
static void
Redisplay(w, event, region)
	Widget          w;
	XEvent         *event;
	Region          region;
{
	TextWidget      tw = (TextWidget) w;
	char	*index(); /* the SYSV headers do not have this */

	GC              gc;

	/* len is the length in characters of the label. */
	int             len = tw->label.label_len;

	/* Grab the address of the character array of the label.
	 * This should be the same as the text buffer's array.
	 */
	/* nl is used to hold the position of a new line character */
	char           *label = tw->label.label, *nl;

	/* tab holds the tabchar position */
	char		*tab;	

	/* Start off by placing the y position where the label's y position
	 * is plus the logical extent above the baseline.
	 */
	Position        y = tw->label.label_y + tw->label.font->max_bounds.ascent;

	/* The x position (in pixels)is needed for accomodating tab characters. When a 
	   tabchar is encountered the drawing of the following text is based on this x pos
	*/

	Position x = 0;

	/* Grab the address of the label's font structure. */
	XFontStruct    *fs = tw->label.font;

	Position        pointx = 0, pointy = 0;


	/* Determine if the label rectangle resides in the region.
	 * If the rectangle is completely out of the region, then simply
	 * return and do no redisplay.  This is presumably to avoid
	 * unnecessary redraws.
	 */

	y += tw->text.textStartDrawOffset;

	if (region != NULL &&

	    XRectInRegion(region, tw->label.label_x, tw->label.label_y,
			  tw->text.actualTextWidth+CURSOR_WIDTH, tw->text.actualTextHeight)
	    == RectangleOut)
		return;

#ifdef JASONDEBUG
	printf("In Text Redisplay\n");
#endif

	/* Grab the graphics context of the label */
	gc = tw->label.gc;


	/* If the text might contain multiple text lines. */
	if (len == MULTI_LINE_LABEL) {


		/* Look for each newline */
          while ((nl = index(label, '\n')) != NULL) {
		if(tw->text.editable==0){
		/*
		    This part of the code looks for tabs in the current line
		    At this point we only recognize tabs when reading from files
		    Not for editable text widgets 
		    We index through the line
		    (not the entire multi-line string) , look for the first tab and draw
		    only that part of the line before the tab + whatever space is left before the next tab stop (default = 8). Then we
		    update the label and tempstr to point to the character after the tab 
		    and repeat the above proc **/

		   if(tw->text.showScrollBar)
			x = SCROLL_BAR_MARGIN;
		   else 
			x = NOSCROLL_BAR_MARGIN;
		/** indexn is an index function that indexes only part of
			a string **/
		 while ((tab = indexn(label, '\t',(int)(nl-label))) != NULL){
		 	/** Draw the first line upto the first tab character. */

			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, x,
				    y, label, (int) (tab - label));

			/** Update the x position for the next set chars before the
			    subsequent tab on the same line in pixels. 8 chars is the
			    default tab-stop **/

			 x += (8-((int)(tab - label)%8)+(int)(tab-label)) * (fs->max_bounds.width);

			/** Point the label to the char after the tab **/
			 label = tab +1;
		 }
			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, x,
				    y, label, (int) (nl - label));
		} /* only if text is not editable */

			/* If the text is editable we do not recognize tabs
				Draw the label up to the newline 
			 If there is more text than the widget can show then the widget
			 * must be a scrollable one.  For a scrollable widget, the text
			 * is displayed slightly more to the right.
			 */
		else{
			
			if (tw->text.showScrollBar)
			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, SCROLL_BAR_MARGIN,
				    y, label, (int) (nl - label));
			else
			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, NOSCROLL_BAR_MARGIN,
				    y, label, (int) (nl - label));
		}
			
			

			/* We want to figure out where to draw the cursor
			 * based on when the text pointed to by 'point'
			 * is encountered.
			 */
			if (pointy == 0 && nl - tw->label.label >= tw->text.point) {

				/* nl - tw->label.label determines how many characters
				 * into the label this newline is at.  If this newline
				 * is greater than or equal to 'point' then the cursor
				 * should be placed at the current y position; hence
				 * pointy = y;
				 */
				pointy = y;

				/* Now figure out what the pointx is. To do this we
				 * start with the x position which is now updated after
				 * processing all the tabs in the line and add the text 
				 * width in pixels of the string, from the beginning of
				 * the line up to the character 'point' is pointing at.
				 *
				 *	label.label
				 *	 |
				 *	 V
				 *	---------------------------------
				 *	|				|
				 *	---------------------------------
				 *	---------------------------------
				 *	|	tab	X		|
				 *	---------------------------------
				 *	 ^	^	^
				 *	 |	|	|
				 *	label	x	text.point
				 */
			if(tw->text.editable!=0)
			 if (tw->text.showScrollBar)
			pointx = SCROLL_BAR_MARGIN +
				XTextWidth(fs, label, tw->text.point - (label - tw->label.label));
			 else
			pointx = NOSCROLL_BAR_MARGIN +
		                XTextWidth(fs, label, tw->text.point - (label - tw->label.label));
			else
					pointx = x +
				      XTextWidth(fs, label,tw->text.point - (label - tw->label.label));
			 
			
		       }

			/* Increment y based on the space above and below the baseline.
			 * This basically moves the text cursor to the next line.
			 */
			y += fs->max_bounds.ascent + fs->max_bounds.descent;

			/* Goto to next part of the label after the newline. */
			label = nl + 1;
		}
		/* Find out what the len of the label is.  By now there should be a single
		 * line of text left with no newline.
		 */
		len = tw->label.label + tw->text.text_size - label;
	}

	/* If there is something left then draw what's left */
	if (len!=0) {
	 if(tw->text.editable==0){
		if (tw->text.showScrollBar)
			x = SCROLL_BAR_MARGIN;
		else
			x = NOSCROLL_BAR_MARGIN;
			while ((tab = index(label, '\t')) != NULL){
		 	/** Draw the label upto the first tab character. */

			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, x,
				    y, label, (int) (tab - label));

			/** Update the x position for the next set chars before the
			    subsequent tab on the same line in pixels. 8 chars is the
			    default tab-stop **/

			 x += (8-((int)(tab - label)%8)+(int)(tab-label)) * (fs->max_bounds.width);

			/** Point the label to the char after the tab **/

			 label = tab +1;
			}
			XDrawString(
				    XtDisplay(w), XtWindow(w), gc, x,
				    y, label,(int)(tw->label.label+tw->text.text_size-label));
	}
	else{
			
			if (tw->text.showScrollBar)
			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, SCROLL_BAR_MARGIN,
				    y, label, (int)(tw->label.label+tw->text.text_size-label));
			else
			XDrawString(
			   XtDisplay(w), XtWindow(w), gc, NOSCROLL_BAR_MARGIN,
				    y, label, (int)(tw->label.label+tw->text.text_size-label));
	}

		/* Figure out where the newline point is in this last line. */
		nl = label + len;

		/* Again if the cursor position has not already been determined above,
		 * do it now.  If pointy == 0 that means the cursor position has
		 * not been determined yet.
		 */
		if (pointy == 0 && nl - tw->label.label >= tw->text.point) {
			pointy = y;
		 if(tw->text.editable!=0)	 
			if (tw->text.showScrollBar)
			pointx = SCROLL_BAR_MARGIN +
				XTextWidth(fs, label, tw->text.point - (label - tw->label.label));
			else
			pointx = NOSCROLL_BAR_MARGIN +
		                XTextWidth(fs, label, tw->text.point - (label - tw->label.label));
		else	
					pointx = x+
				      XTextWidth(fs, label,tw->text.point - (label - tw->label.label));
			
		}
	} else {

		/* Finally if the cursor was not on the last line then just set the
		 * the cursor to the beginning of the first line.
		 */
		if (pointy == 0) {
			pointy = y;
			if (tw->text.showScrollBar)
			pointx = SCROLL_BAR_MARGIN;
			else pointx = NOSCROLL_BAR_MARGIN;
		}
	}



	/* Only draw the cursor if this widget is an editable one */
	if (tw->text.editable) {
		/* Draw the ugly triangular cursor */
		XDrawLine(XtDisplay(w), XtWindow(w), gc, pointx, pointy,
			  pointx - 3, pointy + 3);
		XDrawLine(XtDisplay(w), XtWindow(w), gc, pointx, pointy,
			  pointx + 3, pointy + 3);
		XDrawLine(XtDisplay(w), XtWindow(w), gc, pointx -3, pointy + 3,
			  pointx + 3, pointy + 3);
		tw->text.cursorY = pointy;
	}

	/* Draw slider in scroll bar */

	if (tw->text.showScrollBar) {
		int barLocation = tw->text.barLocation;
		int halfSliderHeight = tw->text.halfSliderHeight;

		/* Draw the scroll bar */
		XDrawLine(XtDisplay(w), XtWindow(w), gc, SCROLL_BAR_WIDTH, 0,
			  SCROLL_BAR_WIDTH, w->core.height);


		/* Draw the slider */
		XDrawRectangle(XtDisplay(w), XtWindow(w), gc, 
			       2, barLocation - halfSliderHeight,
			       SCROLL_BAR_WIDTH-4 , halfSliderHeight * 2);

	}


#ifdef JASONDEBUG
	printf("TEXT ******************************************\n");
	printf("%s\n",tw->text.text);
	printf("INIT###########################################\n");
	printf("%s\n",tw->text.initial_text);
	printf("-----------------------------------------------\n");
	printf("point: %d size: %d memory %d\n", tw->text.point,
	       tw->text.text_size, tw->text.text_memory);
#endif


}



/* Increment the memory for the text buffer to accomodate 1 more character.
 * Memory allocation is done 4 bytes at a time.
 */
static void
IncrementTextMemory(w)
	TextWidget      w;
{
	/* Allocate more memory for 1 additional text character */
	w->text.text = (char *) realloc(w->text.text, sizeof(char) *
			     (w->text.text_memory += TEXT_ALLOC_INCREMENT));
	/*
	 * make sure the label field follows the text field in case it
	 * changed
	 */
	w->label.label = w->text.text;
}


/*
 * Actions:
 * 
 * These deliberately resemble emacs editing commands where possible.
 */

/* VARARGS */
static void
CallAction(void (*action) (), Widget widget, XEvent *event, ...)
{
	va_list         args;
	String          argv[100];
	Cardinal        argc = 0;

	va_start(args, event);
	while ((argv[argc++] = va_arg(args, String)));
	argc--;
	va_end(args);
	(*action) (widget, event, argv, &argc);
}

/* I have no idea why this function has been redefined from my
** previous working version. I do not want to mess with it at this
** time, only fix an egregrious bug  -- Upi 
*/

/* This updates information on the scroll bar and then redraws the text */
static void
Update(w)
	Widget          w;
{

	TextWidget tw = (TextWidget) w;


	/* This calls Recalculate in Label.c which forces a change in the label widget
	 * to the new size and then redisplays the label.  Redisplaying is done by
	 * putting an Expose event in the event list.
	 */
/*	Recalculate(w);*/
	/* Upi's bugfix */
	if (tw->label.label_len != MULTI_LINE_LABEL)
		tw->label.label_len = tw->text.text_size;

	/* Compute new actual text height and width */

#ifdef JASONDEBUG
	printf("In Update: core width %d height %d\n", w->core.width,
	       w->core.height);
#endif

	GetTextWidthAndHeight(tw, &tw->text.actualTextWidth,
			      &tw->text.actualTextHeight);

	if (tw->text.actualTextHeight >= (int)tw->core.height)
               tw->text.showScrollBar = True;
	else
               tw->text.showScrollBar = False;

	SetScrollPoint(tw, tw->text.mouseStartY);
	SetScrollBarHeight(tw);

	if (XtIsRealized(w)) {
		XClearWindow(XtDisplay(w), XtWindow(w));
		(*XtClass(w)->core_class.expose) (w, NULL, NULL);
	}

}


/* This is the same as update except a call to Recalculate is done first. */
static void
UpdateWithResize(w)
	Widget          w;
{

	/* This calls Recalculate in Label.c which forces a change in the label widget
	 * to the new size and then redisplays the label.  Redisplaying is done by
	 * putting an Expose event in the event list.
	 */
	Recalculate(w);
#ifdef JASONDEBUG
	printf("In UpdateWResize: core width %d height %d\n", w->core.width,
	       w->core.height);
#endif

	Update(w);
}


static void
Recenter(w)
	Widget          w;
{
	TextWidget tw = (TextWidget) w;

	if (tw->text.editable == False) return;

	CallAction(Update, w, NULL);
	FollowCursor(w);
}

static void
InsertString(w, insert)
	Widget          w;
	String          insert;
{
	TextWidget      tw = (TextWidget) w;
	int             i, insert_length;
	static XoEventInfo info;


	/* Length of text to insert */
	insert_length = strlen(insert);

	/* for each character in the string, increment the memory 4 bytes at
	 * a time until we have enough memory to store the string we wish
	 * to insert.
	 */

/* !!!!! @@JASON Added 1 here to try and keep the \0 This is important because a number of
 * funtions in Label.c depend on the \0 at the end of the string
 */

	while (tw->text.text_size + insert_length + 1 > tw->text.text_memory)
		IncrementTextMemory(tw);

	/* Move all existing text after the insert point further down
	 * to make space for the string we're about to insert.
	 */
	for (i = tw->text.text_size; i >= tw->text.point; i--)
		tw->text.text[i + insert_length] = tw->text.text[i];

	/* Write each character of the string at location determined by 'point' and
	 * increment point at each insertion.
	 */
	for (i = 0; i < insert_length; i++)
		tw->text.text[tw->text.point++] = insert[i];

	/* Increment the text size; presumably the number of characters in the text buffer
	 * as opposed to the amount of memory occupied by the text buffer. 
	 */
	tw->text.text_size += insert_length;


	info.event = XoKeyPress;
	info.key = insert[0];
	info.ret = tw->text.text;
	info.x = info.y = info.z = 0.0;

	XtCallCallbackList(w, tw->text.callbacks, (XtPointer) & info);
}

/* Insert text lots of text array entries into the text buffer */
static void
Insert(w, ev, argv, argc)
	Widget          w;
	XEvent         *ev;
	String         *argv;
	Cardinal       *argc;
{

	while ((*argc)--)
		InsertString(w, *(argv++));


	CallAction(Update, w, NULL);
}

#define XK_LATIN1
#include <X11/keysym.h>

/* Insert key press characters into text buffer */
static void
SelfInsertCommand(w, ev)
	Widget          w;
	XEvent         *ev;
{

	TextWidget tw = (TextWidget) w;

	if (tw->text.editable == False) return;

	if (ev->type != KeyPress) {
		/* Should issue a warning of some sort */
#ifdef JASONDEBUG
		printf("Not a keypress\n");
#endif
	} else {
		char            keystr[2];
		KeySym          keysym;

		XLookupString((XKeyEvent *) ev, keystr, 1, &keysym, NULL);
		keystr[1] = '\0';
		if (keysym >= XK_space && keysym <= XK_ydiaeresis)
			/* Latin 1 characters */
			CallAction(Insert, w, NULL, keystr, NULL);
		FollowCursor(w);
	}

}

/* If you're in editing mode, and you're typing something,
 * and the cursor is not in the window, scroll the text so
 * that the cursor is visible.  Note you must call Insert once
 * before you call this because the first time you need to
 * compute where the cursor is, then the second time it can
 * figure out where to redraw the screen so that it is visible.
 */
static void
FollowCursor(w)
Widget w;

{
	TextWidget tw = (TextWidget) w;
	int textHeight;
	XFontStruct *fs = tw->label.font;


	textHeight = fs->max_bounds.ascent + fs->max_bounds.descent;

	if (tw->text.cursorY >= (int)tw->core.height) {
		tw->text.textStartDrawOffset -= (tw->text.cursorY - tw->core.height) + textHeight;
		CallAction(Update,w,NULL);
	} else
	if (tw->text.cursorY < 0) {
		tw->text.textStartDrawOffset += (-tw->text.cursorY) + textHeight;
		CallAction(Update,w,NULL);
	}

}

/* Translate carriage returns to newlines and add them to the text buffer */
static void 
SelfInsertReturnCommand(w, ev)
	Widget          w;
	XEvent         *ev;
{

	TextWidget tw = (TextWidget) w;


	if (tw->text.editable == False) return;
#ifdef JASONDEBUG
	printf("in Text.c SelfInsertReturnCommand\n");
#endif
	if (ev->type != KeyPress) {
		/* Should issue a warning of some sort */
#ifdef JASONDEBUG
		printf("Not a keypress\n");
#endif

	} else {
		char            keystr[2];
		KeySym          keysym;

		XLookupString((XKeyEvent *) ev, keystr, 1, &keysym, NULL);
		keystr[1] = '\0';
/*		if (keysym >= XK_space && keysym <= XK_ydiaeresis) {*/
			/* Latin 1 characters */
			keystr[0] = '\n';
			CallAction(Insert, w, NULL, keystr, NULL);
/*		}*/
		FollowCursor(w);
	}

}


/* Move the text cursor forward 1 character */
static void
ForwardChar(w, e, argv, argc)
	Widget          w;
	XEvent         *e;
	String         *argv;
	Cardinal       *argc;
{
	TextWidget      tw = (TextWidget) w;
	int             n = 1;


	if (tw->text.editable == False) return;

	if (*argc)
		n = atoi(*argv);
	if ((tw->text.point + n) < 0)
		tw->text.point = 0;
	else if ((tw->text.point + n) > tw->text.text_size)
		tw->text.point = tw->text.text_size;
	else
		tw->text.point += n;
	CallAction(Update, w, NULL);
	FollowCursor(w);
}

static void
BeginningOfLine(w)
	Widget          w;
{
	TextWidget      tw = (TextWidget) w;
	char           *nl = tw->text.text;
	int             new_point = 0;
	char			*index();

	if (tw->text.editable == False) return;

	do {
		new_point = nl - tw->text.text;
		nl = index(nl, '\n');
	} while (nl && (nl++ - tw->text.text) < tw->text.point);
	tw->text.point = new_point;
	CallAction(Update, w, NULL);
	FollowCursor(w);
}

static void
EndOfLine(w)
	Widget          w;
{
	TextWidget      tw = (TextWidget) w;
	char           *nl;
	char			*index();

	if (tw->text.editable == False) return;

	nl = index(tw->text.text + tw->text.point, '\n');
	tw->text.point = nl ? nl - tw->text.text : tw->text.text_size;
	CallAction(Update, w, NULL);
	FollowCursor(w);
}


static void
KillToEndOfLine(w)
	Widget          w;
{
	TextWidget      tw = (TextWidget) w;

	if (tw->text.editable == False) return;

	/*
	 * nl = index (tw->text.text + tw->text.point, '\n'); tw->text.point
	 * = nl ? nl - tw->text.text : tw->text.text_size;
	 */

	if (tw->text.point < tw->text.text_size) {
		tw->text.text_size = tw->text.point;
		tw->text.text[tw->text.point] = '\0';
	}
	CallAction(Update, w, NULL);
	FollowCursor(w);
}

static void
DeleteChar(w, e, argv, argc)
	Widget          w;
	XEvent         *e;
	String         *argv;
	Cardinal       *argc;
{
	TextWidget      tw = (TextWidget) w;
	int             i, n = 1;


	if (tw->text.editable == False) return;

	if (*argc)
		n = atoi(*argv);
	if (n == 0)
		return;
	if (n > 0) {
		if (tw->text.point + n < tw->text.text_size) {
			tw->text.text_size -= n;
		} else {
			n = tw->text.text_size - tw->text.point;
			tw->text.text_size = tw->text.point;
		}
	} else {
		n = -n;
		if (tw->text.point - n > 0) {
			tw->text.point -= n;
			tw->text.text_size -= n;
		} else {
			tw->text.text_size -= tw->text.point;
			n = tw->text.point;
			tw->text.point = 0;
		}
	}
	for (i = tw->text.point; i <= tw->text.text_size; i++)
		tw->text.text[i] = tw->text.text[i + n];

	CallAction(Update, w, NULL);
	FollowCursor(w);
}


static void
KillLine(w)
	Widget          w;
{
	TextWidget      tw = (TextWidget) w;
	char           *nl = tw->text.text;
	int             new_point = 0;
	char			*index();

	if (tw->text.editable == False) return;

	do {
		new_point = nl - tw->text.text;
		nl = index(nl, '\n');
	} while (nl && (nl++ - tw->text.text) < tw->text.point);
	tw->text.point = new_point;

	if (tw->text.point < tw->text.text_size) {
		tw->text.text_size = tw->text.point;
		tw->text.text[tw->text.point] = '\0';
	}
	CallAction(Update, w, NULL);
		FollowCursor(w);
}

/* ARGUSED */
static void
Notify(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	TextWidget      tw = (TextWidget) w;
	static XoEventInfo info;
	Modifiers       modifiers;

#ifdef JASONDEBUG
	printf("in text notify\n");
#endif

	info.event = XoKeyPress;
	info.key = XtGetActionKeysym(event, &modifiers);
	info.ret = tw->text.text;
	info.x = info.y = info.z = 0.0;

	XtCallCallbackList(w, tw->text.callbacks, (XtPointer) & info);
}


/* Check of a click of the mouse button within the text widget.  This
 * function will move the text.point cursor to wherever the mouse pointer
 * is.
 */

static void
MouseButton1(w, event, params, num_params)
     Widget w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{

	TextWidget tw = (TextWidget) w;
	char	*index();

#ifdef JASONDEBUG
	/* Width and height of label in pixels */
	int widthOfLabel = tw->label.label_width;
	int heightOfLabel = tw->label.label_height;
#endif

	/* Top left hand position of label. */
/*	int labelXPosition = tw->label.label_x;*/
	int labelXPosition;
	int labelYPosition = tw->label.label_y;

	/* Here start of label is set to the actual beginning of label which may not
	 * be the case if the window were scrollable and we saw only a small subwindow
	 */
	char *startOfLabel = tw->label.label;

	/* labelPointer acts as an accumulator that iterates through the label. */
	char *labelPointer = startOfLabel;

	/* Mouse X and Y position relative to the text widget. */
	int mouseX = ((XButtonEvent *)event)->x;
	int mouseY = ((XButtonEvent *)event)->y;

	/* Length of the label from startOfLabel in characters. */
	int labelLength =  (tw->label.label + tw->text.text_size) -  startOfLabel;

	/* Pointer to each newline character found in label. */
	char *newLinePointer;

	/* Pointer to font structure. Needed to compute the width of characters
	 * in the label.
	 */
	XFontStruct *fs = tw->label.font;

	/* This is accumulated through the search phase where we are looking
	 * for the character the mouse pointer is pointing at.  The value
	 * stored in scanningPoint is the eventual value passed to text.point
	 * which tells the widget which part of the text the cursor is at.
	 */
	int scanningPoint = 0;

	/* Iterators for the X and Y position during scanning. */
	Position runningPointerX, runningPointerY;

	/* Left and right limits for a character on a line.  It is used to
	 * determine if the mouse is in between them.
	 */
	int leftX, rightX;

	/* Pointer to each character in a line of a label. */
	char *eachChar;

	/* Width of a character in pixels. */
	int charWidth;

	/* Label X position begins at different places depending on whether there is
	 * to be a scroll bar or not.
	 */
	if (tw->text.showScrollBar)
	labelXPosition =SCROLL_BAR_MARGIN;
	else
	labelXPosition = NOSCROLL_BAR_MARGIN;

#ifdef JASONDEBUG
	printf("Mouse button 1 pressed x=%d y=%d\n",mouseX,mouseY);
	printf("Label: x=%d y=%d\n",labelXPosition, labelYPosition);
	printf("Label: w=%d h=%d\n",widthOfLabel, heightOfLabel);
#endif

	/* Get the mouse's starting position to enable scrolling
	 * if user has clicked inside the scroll bar
	 */
	tw->text.mouseStartX = mouseX;
	tw->text.mouseStartY = mouseY;

	/* If mouse is inside scroll bar and slider then make scrolling possible */
	if (tw->text.showScrollBar) {
		if (mouseX < SCROLL_BAR_WIDTH) {
			if ((mouseY >= tw->text.barLocation - tw->text.halfSliderHeight)  &&
			    (mouseY <= tw->text.barLocation + tw->text.halfSliderHeight)) {
				tw->text.scrollingAccessible = True;
			} else
		      tw->text.scrollingAccessible = False;
			return;
		}
		else	tw->text.scrollingAccessible = False;
	}

	/* RunningPointerY begins with the Y label position and moves down line by line */
	runningPointerY = labelYPosition + tw->text.textStartDrawOffset;
	runningPointerX = labelXPosition;

	/* Scan to each newline character */
	while((newLinePointer = index(labelPointer,'\n')) != NULL) {

#ifdef JASONDEBUG
		printf("runningY: %d\n",runningPointerY);
#endif

		/* Check if mouseY is on this current line. */
		if ((mouseY >= runningPointerY) &&
		    (mouseY <= (runningPointerY + fs->max_bounds.ascent +
				fs->max_bounds.descent))) {

			/* If so figure out where the cursor is along the line. */

			leftX = runningPointerX;

			/* If the mouse is on the far left of a line, always set point
			 * to the beginning of that line.
			 */

			if (tw->text.showScrollBar) {
				if ((mouseX <= leftX) && (mouseX >= SCROLL_BAR_WIDTH)) {
					tw->text.point = scanningPoint;
					/* Update the screen */
					CallAction(Update, w, NULL);
					return;				
				}
			} else {
				if (mouseX <= leftX) {
					tw->text.point = scanningPoint;
					/* Update the screen */
					CallAction(Update, w, NULL);
					return;				
				}
			}

			/* If mouse is on the far right of a line, always set point to end
			 * of that line.
			 */
			if (mouseX > 
			    (leftX + XTextWidth(fs,labelPointer, (newLinePointer - labelPointer)))) {
				tw->text.point = scanningPoint + (newLinePointer - labelPointer);
				/* Update the screen */
				CallAction(Update, w, NULL);
				return;				
			}

			/* Scan one character at a time up to the newline. */
			for(eachChar = labelPointer; eachChar < newLinePointer; eachChar++) {

				/* Compute the left and right bounds of a character by its width. */
				charWidth = XTextWidth(fs, eachChar,1);
				rightX = charWidth + leftX;

				/* If mouse is within these bounds then set text.point
				 * to indicate which character it is that the mouse is pointing
				 * at.
				 */
				if ((mouseX >= leftX) && (mouseX <= rightX)) {
					/* Found it */
#ifdef JASONDEBUG
					printf("X found\n");
#endif
					tw->text.point = scanningPoint;

					/* Update the screen */
					CallAction(Update, w, NULL);
					return;
				}

				/* Move the limits along */
				leftX = rightX;

				/* Move to next character */
				scanningPoint++;
			}
		}

		/* If on the otherhand mouseY is not on the same line, move to next
		 * line.
		 */
		scanningPoint += (newLinePointer - labelPointer + 1);
		runningPointerY += fs->max_bounds.ascent + fs->max_bounds.descent;
		labelPointer = newLinePointer + 1;

	} /* Keep doing this until all lines are scanned. */

	/* There will always be one line left after the last new line */

	/* Figure out where the cursor is on this line. */
	leftX = runningPointerX;

	/* At this point the newLinePointer should just point to the last character in the label */
	newLinePointer = startOfLabel + labelLength;
	
	/* If the mouse is on the far left of a line, always set point
	 * to the beginning of that line.
	 */

	if (tw->text.showScrollBar) {
		if ((mouseX <= leftX) && (mouseX >= SCROLL_BAR_WIDTH)) {
			tw->text.point = scanningPoint;
			/* Update the screen */
			CallAction(Update, w, NULL);
			return;				
		}
	} else {
		if (mouseX <= leftX) {
			tw->text.point = scanningPoint;
			/* Update the screen */
			CallAction(Update, w, NULL);
			return;				
		}
	}

	/* If mouse is on the far right of a line, always set point to end
	 * of that line.
	 */
	if (mouseX > 
	    (leftX + XTextWidth(fs,labelPointer, (newLinePointer - labelPointer)))) {
		tw->text.point = scanningPoint + (newLinePointer - labelPointer);
		/* Update the screen */
		CallAction(Update, w, NULL);
		return;				
	}

	/* Go through each character looking for the one the mouse is pointing at. */
	for(eachChar = labelPointer; eachChar < newLinePointer; eachChar++) {
		charWidth = XTextWidth(fs, eachChar,1);
		rightX = charWidth + leftX;
		if ((mouseX >= leftX) && (mouseX <= rightX)) {
			/* Found it */

#ifdef JASONDEBUG
			printf("X found\n");
#endif

			tw->text.point = scanningPoint;
			CallAction(Update, w, NULL);
			return;
		}
		leftX = rightX;
		scanningPoint++;
	}
	CallAction(Update, w, NULL);
	

}


/* Establish a starting X and Y mouse position to begin grab scrolling. */
static void
GrabScrollEstablish(w, event, params, num_params)
     Widget w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{

	void GetTextWidthAndHeight();

	TextWidget tw = (TextWidget) w;
	int mouseX = ((XButtonEvent *)event)->x;
	int mouseY = ((XButtonEvent *)event)->y;
	tw->text.mouseStartX = mouseX;
	tw->text.mouseStartY = mouseY;
	tw->text.scrollingAccessible = True;
	GetTextWidthAndHeight(tw, &tw->text.actualTextWidth,
			      &tw->text.actualTextHeight);
}


/* Determine the size and location of the scroll bar and set those
 * values in the text widget
 */
void SetScrollBarHeight(tw)
TextWidget tw;

{
	if (tw->text.showScrollBar) {
		int barHeight;
		int barLocation;

		/* Bar height is determined by the percentage the window
		 * occupies the entire text label.
		 */
		int halfSliderHeight = (int) (((float) tw->core.height /
					(float) tw->text.actualTextHeight) *
					   (float)tw->core.height / 2.0);

		/* The actual scroll bar height is shorter than core.height
		 * because there needs to be room to fit the slider inside
		 * the scroll bar.  Hence we subtract the size of the slider
		 * below.
		 */
		barHeight  = tw->core.height - 2 * halfSliderHeight;

		/* Rescale drawing offset in terms of bar height */ 
		barLocation = (int) ((((float) (-tw->text.textStartDrawOffset) /
				       (float) (tw->text.actualTextHeight - (int)tw->core.height)) *
				      (float) barHeight) + (float) halfSliderHeight);

		/* Store away these computed values so we don't have to keep computing
		 * them.  They only need to be recomputed if the text label increases
		 * in size.
		 */
		tw->text.barHeight = barHeight;
		tw->text.halfSliderHeight = halfSliderHeight;
		tw->text.barLocation = barLocation;
	}

}

/* Determine at what Y offset to begin redisplaying the text
 * in the Redisplay function.  By changing this offset
 * we are simulating the scroll.  The offset will always either be negative
 * or zero because a scroll always involves displaying text higher up
 * in the widget window (by higher up I mean visually higher up; coordinate-
 * wise that means more negative).
 */
void SetScrollPoint(tw, newMouseY)
TextWidget tw;
int newMouseY;
{
	void GetTextWidthAndHeight();

	/* Compute the amount to offset the text drawing based on the change
	 * in the y position of the mouse.
	 */
	tw->text.textStartDrawOffset += newMouseY - tw->text.mouseStartY;

	/* If offset > 0 then we're already at the top of the text so do not
	 * go any further
	 */
	if (tw->text.textStartDrawOffset > 0) tw->text.textStartDrawOffset = 0;


	/* Prevent over scrolling at the bottom */

	/* No more scrolling down possible if actual text height is smaller
	 * than core height
	 */
	if (tw->text.actualTextHeight < (int)tw->core.height) tw->text.textStartDrawOffset = 0;

	/* Otherwise check the lower limit to scroll */
	else
	if ((-tw->text.textStartDrawOffset) > 
	    (tw->text.actualTextHeight - (int)tw->core.height)) {
		tw->text.textStartDrawOffset =
			- (tw->text.actualTextHeight - (int)tw->core.height);
	}

	tw->text.mouseStartY = newMouseY;

}


/* After a grab with the middle mouse button any movements are handled
 * here.  This basically simulates a hand grabbing the text widget screen
 * and moving it.
 */
static void
GrabScrollMove(w, event, params, num_params)
     Widget w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{


	TextWidget tw = (TextWidget) w;

	int newMouseY = ((XButtonEvent *) event)->y;

	/* If scrolling is in-accessible then do not allow scrolling */
	if (tw->text.scrollingAccessible == False) return;

	/* Set the vertical offset for redraw based on this new mouse position */
	SetScrollPoint(tw, newMouseY);

	CallAction(Update, w, NULL);
	
}


/* Get the actual text width and height without setting any of
 * the parameters. This is actually an exact copy of
 * SetTextWidthAndHeight in Label.c but will not change any of the
 * widget's parameters- instead the label sizes are returned
 */
static void 
GetTextWidthAndHeight(tw, labelWidth, labelHeight)
TextWidget tw;
int *labelWidth, *labelHeight;
{
	char *nl;
	char *index();
	LabelWidget lw = (LabelWidget) tw;
	register XFontStruct *fs = lw->label.font;

	/* Minimum height is 1 character. */
	*labelHeight = fs->max_bounds.ascent + fs->max_bounds.descent;

	/* If there is no label then set the label's length (in characters) to 0 and
	 * the label's width to 0.
	 */
	if (lw->label.label == NULL) {
		tw->label.label_len=0; 
		*labelWidth = 0;
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
		tw->label.label_len=MULTI_LINE_LABEL; 

		/* Initialize the labelwidth and add it for every char encountered */

		*labelWidth = 0;

		/* Scan to each newline character */
		for (label = lw->label.label; nl != NULL; nl = index(label, '\n')) {

			/* Compute the width of the line */
			int width = XTextWidth(fs, label, (int) (nl - label));

			/* Get the largest line width so far found. */
			if (width > *labelWidth)
				*labelWidth = width;

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

			*labelHeight += fs->max_bounds.ascent + fs->max_bounds.descent;
		}
		if (*label) {
			int width = XTextWidth(fs, label, strlen(label));

			if (width > *labelWidth)
				*labelWidth = width;
		}
	}

	/* If there is no newline then just compute the width as the width of
	 * the label.
	 */
	else {
		int len = strlen(lw->label.label);

		*labelWidth =
			XTextWidth(fs, lw->label.label, len);
	}
}

/* A click in the scroll bar has occurred so see if we can scroll the
 * window using the slider in the scroll bar.  This is different from
 * grabbing the window and sliding it in the sense that, it allows
 * greater jumps through the text.
 */
static void
ScrollBarMove(w, event, params, num_params)
     Widget w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	TextWidget tw = (TextWidget) w;
	int newMouseY = ((XButtonEvent *) event)->y;
	int diffY;

	/* If there is no scroll bar then obviously no scrolling */
	if (!tw->text.showScrollBar)  return;

	/* If the user has not explicitly clicked in the slider of
	 * the scroll bar then sorry, no scrolling allowed either.
	 */
	if (tw->text.scrollingAccessible == False) return;

#ifdef JASONDEBUG
	printf("ScrollBarMove\n");
#endif

	/* Need to translate mouse movements into window movements */
	diffY = newMouseY - tw->text.mouseStartY;

	tw->text.textStartDrawOffset += (int) -(((float) diffY / (float) tw->core.height) *
						(float) tw->text.actualTextHeight);

	CallAction(Update, w, NULL);

	/* Need to do this so that we are always measuring delta mouse moves at each
	 * call to this function.
	 */
	tw->text.mouseStartY = newMouseY;
}

/* Utility functions for accessing the text string */
char           *
XoGetTextValue(w)
	Widget          w;
{
	TextWidget      tw = (TextWidget) w;
	return (tw->text.text);
}

/* This sets the text attribute of the text widget */
char           *
XoSetTextValue(w, str, do_multiline)
	Widget          w;
	char           *str;
	int             do_multiline;
{
	TextWidget      tw = (TextWidget) w;
	char			*index();
	char           *nl;


	if (!str)
		return (NULL);
	/* Free up all memory in the text buffer and allocate the first block */
	if (tw->text.text_memory > 0 && tw->text.text){
		free(tw->text.text);
		tw->label.label=NULL;
	 /** Updating this is a mistake because of the @@WEIRD glitch 
             This is always initialized to MULTI_LINE_LABEL and not used anywhere 
	     other than the Redisplay code	**/
			tw->label.label_len=0; 
	}
	tw->text.text_memory = TEXT_ALLOC_INCREMENT;
	tw->text.text = (char *) malloc(sizeof(char) * TEXT_ALLOC_INCREMENT);
	*(tw->text.text) = '\0';
	tw->text.text_size = 0;

	/* Free up the text in the initial_text buffer and reassign it to the size of the
	 * new string.
	 */
	if (tw->text.initial_text)
		free(tw->text.initial_text);
	tw->text.initial_text = (char *) malloc(sizeof(char) *
						(strlen(str)+1));

	/* Copy this string into the buffer */
	strcpy(tw->text.initial_text, str);

	/* Now insert the string into the text buffer proper */
	tw->text.point = 0;
	defaultPoint = -9999;
	CallAction(Insert, w, NULL, str, NULL);

	/* Assign label to point to text buffer as usual. */
	tw->label.label = tw->text.text;

	/* Compute new text size */
	if (!do_multiline)
		tw->label.label_len = tw->text.text_size;
	else
		tw->label.label_len = MULTI_LINE_LABEL;

	/* Go to end of line */
	nl = index(tw->text.text + tw->text.point, '\n');
	tw->text.point = nl ? nl - tw->text.text : tw->text.text_size;

	/* Update the text widget */
	CallAction(Update, w, NULL);
	return (tw->text.text);
}


#define XO_TEXT_MAXLINE_LEN 200
static Boolean
SetValues(current, request, new, args, num_args)
	Widget          current, request, new;
	ArgList         args;
	Cardinal       *num_args;
{
	TextWidget    curtw = (TextWidget) current;
	TextWidget    reqtw = (TextWidget) request;
	TextWidget    newtw = (TextWidget) new;
	Boolean         redisplay = False;

#ifdef JASONDEBUG
	printf("in TextWidget SetValues\n");


	printf("text num args: %d\n", *num_args);
	printf("text args: %s\n", *args);


	printf("text core width: %d %d\n",curtw->core.width, newtw->core.width);
	printf("text core height: %d %d\n",curtw->core.height, newtw->core.height);

	printf("text label size: w=%d h=%d\n", newtw->label.label_width,
	       newtw->label.label_height);

#endif
	/* If the current text is different from the new initial text and
	 * the two initial texts are different then an attemp to change
	 * the initial text has occurred so carry that change out.
	 * This screening is necessary because when the x or ygeom's are
	 * set this function is called and we don't want the initial text
	 * to change because of a geom change.
	 */
       if(curtw->text.text!=NULL){	
	if ((strcmp(curtw->text.text, reqtw->text.initial_text)) &&
	    (strcmp(curtw->text.initial_text, reqtw->text.initial_text))) {
		/* Make the changes */

		XoSetTextValue((Widget) newtw, reqtw->text.initial_text, TRUE);

#if 0
		/* Search for @@WEIRD for an explanation of this */
		newtw->label.label_len = MULTI_LINE_LABEL;
#endif

		redisplay = False;

#ifdef JASONDEBUG
		printf("CUR @@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("%s\n",curtw->text.initial_text);
		printf("REQ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("%s\n",reqtw->text.initial_text);
		printf("NEW @@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("%s\n",newtw->text.initial_text);
#endif

	}
       }
#ifdef NOTUSED
	/* Upi addition, to enable display of text files */
	if (reqtw->text.filename && curtw->text.filename && 
		&& strlen(reqtw->text.filename) > 0 && 
		strcmp(curtw->text.filename, reqtw->text.filename) != 0) {
		FILE *fp; *fopen();
		if (!(fp = fopen(reqtw->text.filename,"r"))) {
			redisplay = False;
			printf("Warning: Text file %s not found\n",
				reqtw->text.filename);
		} else {
			XoSetTextValue(newtw, "", TRUE);
			while (fgets(newtext,XO_TEXT_MAXLINE_LEN,fp)) {
				/* be sure that the string is terminated */
				newtext[XO_TEXT_MAXLINE_LEN - 1] = '\0';
				InsertString(newtw,newtext);
			}
			fclose(fp);
			redisplay = TRUE
		}
	}
#endif
	return (redisplay);
}



static void Destroy(w)
Widget w;
{
	TextWidget wid = (TextWidget) w;
	if (wid->text.initial_text) XtFree(wid->text.initial_text);
	if (wid->text.text){
		 XtFree(wid->text.text);
          /* To avoid freeing again in the Destroy() of the Label
		 widget */
		 wid->label.label=NULL; 
	}
}

static char *indexn(str,ch,numchar)
char *str;
char ch;
int numchar;
{
 char *ptr;
 if(str == NULL) return(NULL);
 ptr = str;
 while((ptr!=NULL) && (ptr <= str+numchar)){
	if (*ptr++ == ch) return(--ptr);
	else continue;		
 }
 return(NULL);
}
