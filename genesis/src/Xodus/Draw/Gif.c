/* $Id: Gif.c,v 1.3 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Gif.c,v $
 * Revision 1.3  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 18:50:24  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/09/11 15:44:16  mhucka
 * The previous implementation did not properly handle graphics contexts
 * (GCs).  The contexts were being created using information drawn from the X
 * window system default window, but this is wrong: on TrueColor or other
 * non-Pseudocolor visuals, GENESIS explicitly uses a Pseudocolor visual and
 * creates its own private colormap, and these may not match the defaults in
 * characteristics.  This lead to BadMatch errors in XPutImage on 24-bit color
 * Solaris systems.  This should be now fixed.
 *
 * Revision 1.6  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  2000/05/19 18:19:51  mhucka
 * 1) Minor code reformatting so I can read it more easily.
 * 2) Minor cleanup of unneeded code.
 * 3) Rewrote LoadImage to use new XGIFLoad function.
 * 4) Used new Xo/Xo_ext.h file to declare functions from other files.
 *
 * Revision 1.4  2000/05/02 06:06:41  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.3  1995/04/27 21:40:03  venkat
 * Commented call to free the filename resource in the Destroy
 * method. XtDestroyWidget() takes care of freeing the resources
 * that are allocated by calls to XtSetValues() etc.,
 *
 * Revision 1.2  1995/03/17  20:39:57  venkat
 * Merged
 *
 * Revision 1.1  1994/02/02  18:49:54  bhalla
 * Initial revision
 * */
/* This was a start on the GIF pix.  I stopped working on it to
 * work on the standalone image widget.
 */

#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "Xo/Xo_ext.h"
#include "CoreDrawP.h"
#include "GifP.h"
/* #define PURIFY_RUN */


#define offset(field) XtOffset(GifObject, gif.field)
static XtResource resources[] = {

    {XtNfilename, XtCFilename, XtRString, sizeof(char *),
		offset(filename), XtRString, "image.gif"
    },
    {XtNfileformat, XtCFileformat, XtRString, sizeof(char *),
        offset(fileformat), XtRString, "gif"
    },
    {XtNstoremode, XtCStoremode, XtRString, sizeof(char *),
        offset(storemode), XtRString, "mem"
    },
    {XtNrescaleable, XtCRescaleable, XtRBoolean, sizeof(int),
        offset(rescaleable), XtRString, "FALSE"
    },
    {XtNvalue, XtCValue, XtRString, sizeof(char *),
        offset(value), XtRString, "none"
    },
    {XtNsrcx, XtCSrcx, XtRInt, sizeof(int),
        offset(srcx), XtRString, "0"
    },
    {XtNsrcy, XtCSrcy, XtRInt, sizeof(int),
        offset(srcy), XtRString, "0"
    },
    {XtNdstx, XtCDstx, XtRInt, sizeof(int),
        offset(dstx), XtRString, "0"
    },
    {XtNdsty, XtCDsty, XtRInt, sizeof(int),
        offset(dsty), XtRString, "0"
    },
    {XtNwx, XtCWx, XtRInt, sizeof(int),
        offset(wx), XtRString, "100"
    },
    {XtNwy, XtCWy, XtRInt, sizeof(int),
        offset(wy), XtRString, "100"
    },
};

#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static void ClassInitialize();
static void ResetGC();
static void Expose();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();
static void LoadImage();

/* class record definition */


GifClassRec gifClassRec = {
	{			/* RectObj (Core) Fields */

		 /* superclass         */ (WidgetClass) & pixClassRec,
		 /* class_name         */ "Gif",
		 /* size               */ sizeof(GifRec),
		 /* class_initialize   */ ClassInitialize,
		 /* class_part_initialize */ NULL,
		 /* Class init'ed      */ FALSE,
		 /* initialize         */ Initialize,
		 /* initialize_hook    */ NULL,
		 /* realize            */ NULL,
		 /* actions            */ NULL,
		 /* num_actions        */ 0,
		 /* resources          */ resources,
		 /* resource_count     */ XtNumber(resources),
		 /* xrm_class          */ NULLQUARK,
		 /* compress_motion    */ FALSE,
		 /* compress_exposure  */ FALSE,
		 /* compress_enterleave */ FALSE,
		 /* visible_interest   */ FALSE,
		 /* destroy            */ Destroy,
		 /* resize             */ NULL,
		 /* expose             */ Expose,
		 /* set_values         */ SetValues,
		 /* set_values_hook    */ NULL,
		 /* set_values_almost  */ XtInheritSetValuesAlmost,
		 /* get_values_hook    */ NULL,
		 /* accept_focus       */ NULL,
		 /* intrinsics version */ XtVersion,
		 /* callback offsets   */ NULL,
		 /* tm_table           */ NULL,
		 /* query_geometry     */ QueryGeometry,
		 /* display_accelerator */ NULL,
		 /* extension          */ NULL
	},
	{
		/* pix Fields */
		 /* project */ 		Project,
		 /* undraw */		XoInheritUndraw,
		 /* select_distance */ XoInheritSelectDistance,
		 /* select */ 		XoInheritSelect,
		 /* unselect */		XoInheritUnSelect,
		 /* motion */		XoInheritMotion,
		 /* highlight */	XoInheritHighlight,
		 /* extension */ NULL
	},
	{
		/* gif fields */
		 /* make_compiler_happy */ 0
	}
};

WidgetClass gifObjectClass = (WidgetClass) & gifClassRec;


/*
** This primarily needs to initialize the GC.
*/
static void 
Initialize (req, new, args, num_args)
    Widget    req, new;
    ArgList   args;
    Cardinal *num_args;
{
    GifObject gif = (GifObject) new;

    gif->gif.image = NULL;
#ifdef PURIFY_RUN
    purify_watch(&(gif->gif.filename));
#endif

    ResetGC(gif);
}


/*
** Reset the values in the GC.
*/
static void 
ResetGC(gif)
GifObject gif;
{
    Display  *display = XtDisplay(XtParent((Widget) gif));
    Drawable  drawable = XtWindow(XtParent((Widget) gif));
    XGCValues values;
    XtGCMask  mask = GCForeground | GCFunction;

    if (gif->pix.gc) {
	XtReleaseGC(XtParent((Widget) gif), gif->pix.gc);
    }

    values.foreground = gif->pix.fg;
    values.function = GXcopy;
    gif->pix.gc = XCreateGC(display, drawable, mask, &values);
}


static void 
ClassInitialize()
{

}

static Boolean 
SetValues(curw, reqw, neww)
Widget curw, reqw, neww;
{
    GifObject curg = (GifObject) curw;
    GifObject newg = (GifObject) neww;
    Boolean ret = False;

    if (strcmp(newg->gif.filename, curg->gif.filename)) {
/*        printf("Load up file: %s\n", newg->gif.filename); */
	LoadImage(newg);
	ret = True;
    }
    /* Any of these changes requires a reprojection of the pix
    ** and a redrawing of the entire draw.
    */
    if (newg->pix.tx != curg->pix.tx ||
	newg->pix.ty != curg->pix.ty ||
	newg->pix.tz != curg->pix.tz) {
	Project(newg);
	ret = True;
    }

    return (ret);
}


static void 
Expose(w)
Widget w;
{
    GifObject gw = (GifObject) w;
   
    if (XtIsRealized(w) && gw->gif.image) {
	XPutImage((Display *)XgDisplay(), XtWindow(gw->object.parent),
		  gw->pix.gc, gw->gif.image,
		  /* src coords */
		  gw->gif.srcx, gw->gif.srcy, 
		  /* dest coords */
		  gw->pix.cx - gw->gif.wx/2, gw->pix.cy - gw->gif.wy/2,
		  /* width and height */
		  gw->gif.wx, gw->gif.wy);
    }
}

static void 
Destroy(w)
Widget w;
{
    GifObject gw = (GifObject) w;

    XtReleaseGC(XtParent(w), gw->pix.gc);

    if (gw->gif.image) {
	XtFree((char *) gw->gif.image);
    }
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult 
QueryGeometry(w, intended, preferred)
Widget w;
XtWidgetGeometry *intended, *preferred;
{
    GifObject gw = (GifObject) w;

    preferred->x = gw->pix.x;
    preferred->y = gw->pix.y;
    preferred->width = gw->pix.w;
    preferred->height = gw->pix.h;
    if (intended->request_mode & (CWX | CWY | CWWidth | CWHeight)) {
	if (preferred->x == intended->x &&
	    preferred->y == intended->y &&
	    preferred->width == intended->width &&
	    preferred->height == intended->height)
	    return (XtGeometryYes);
    }
    else {
	if (preferred->x == gw->pix.x &&
	    preferred->y == gw->pix.y &&
	    preferred->width == gw->pix.w &&
	    preferred->height == gw->pix.h)
	    return (XtGeometryYes);
    }
    return (XtGeometryAlmost);
}


static void 
Project(w)
Widget w;
{
    GifObject pw = (GifObject)w;
    CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
    void  (*tpt)();
    int x,y,z;

    if (pw->pix.pixflags & XO_VISIBLE_NOT) return;
    if (pw->pix.pixflags & XO_RESIZABLE_NOT){ /*Freeze size and position*/
        return;
    }
    if (pw->pix.pixflags & XO_PIXELOFFSET) { /* transform using pixels*/
        tpt = PixelTransformPoint;
    } else { /* use standard transformations */
        tpt = class->coredraw_class.transformpt;
    }
 
    /* Calculate offset on screen using parents TransformPoint routine */
    (tpt)(XtParent(w),
	  pw->pix.tx,pw->pix.ty, pw->pix.tz,&x,&y,&z);
    pw->pix.cx = x; pw->pix.cy = y; pw->pix.cz=z;

    /* Figure out where the coords for clicks should be */
    pw->pix.w = pw->gif.wx;
    pw->pix.h = pw->gif.wy;
    pw->pix.y = y - pw->gif.wy/2;
    pw->pix.x = x - pw->gif.wx/2;
/* if we were to put x,y at one corner, it would be: */
    /* pw->pix.y = y + pw->gif.wy/2; */
    /*
      pw->pix.x = x;
      pw->pix.y = y;
    */
}


static void LoadImage(w)
GifObject w;
{
    extern XImage *XGIFLoad();
    Widget widg = (Widget) (w->object.self);

    w->gif.image = XGIFLoad(w->gif.filename, widg, &(w->gif.wx), &(w->gif.wy));
}
