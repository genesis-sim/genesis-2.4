/* $Id: Image.c,v 1.2 2005/07/01 10:02:50 svitak Exp $ */
/*
 * $Log: Image.c,v $
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/10/06 04:47:56  mhucka
 * Reworked the handling of graphics contexts.  The previous implementation
 * assumed that the default screen visual was the same as that used for
 * windows, which is not necessarily true and can lead to BadMatch errors
 * in X.
 *
 * Revision 1.5  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  2000/05/19 18:20:18  mhucka
 * Rewrote LoadImage to use new XGIFLoad function.
 *
 * Revision 1.3  2000/05/02 06:18:08  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.2  1995/07/08 01:07:11  venkat
 * image->filename is string resource and is now freed in the genesis side
 * DELETE action for the image.
 *
 * Revision 1.1  1994/03/22  15:24:49  bhalla
 * Initial revision
 * */

/* --------------------------------------------- */
/* The init routine for setting up a image widget.
 *
 *	By Upi Bhalla April 1990.
 *  If this code does not work it is not my fault.
 *	You may hack this code if the acknowledgement
 *	remains intact.
 *	Parts of the code are taken from xgif, by
 *	John Bradley, University of Pennsylvania, who in turn got
 *	portions of code from Patrick J. Naughton. See xgifload.c
 *
 */
/* --------------------------------------------- */

/* All the necessary include files */

#include <stdio.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "ImageP.h"

Boolean ImageSetHook();
void ImageGetHook();
void ImageResize();
static void LoadImage();
static void ResetGC();

/* --------------------------------------------- */

static XtResource ImageResources[] = {
#define offset(field) XtOffset(ImageWidget, image.field)
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
	{XtNcallback, XtCCallback, XtRCallback, sizeof (XtPointer),
		offset(callbacks), XtRCallback, (XtPointer) NULL
	},
};

static Boolean ImageSetValues();
static void Destroy();
void ImageDisplay();

static char translations[] = "";

static void ImageAction();
static void ImageInit();
static XtActionsRec actions[] = {
	{"image",	ImageAction},
};

ImageClassRec imageClassRec = {
  { /* core fields */
    /* superclass       */  (WidgetClass) &widgetClassRec,
    /* class_name       */  "Image",
    /* widget_size      */  sizeof(ImageRec),
    /* class_initialize     */  NULL,
    /* class_part_initialize    */  NULL,
    /* class_inited     */  FALSE,
    /* initialize       */  ImageInit,
    /* initialize_hook      */  NULL,
    /* realize          */  XtInheritRealize,
    /* actions          */  actions,
    /* num_actions      */  XtNumber(actions),
    /* resources        */  ImageResources,
    /* num_resources        */  XtNumber(ImageResources),
    /* xrm_class        */  NULLQUARK,
    /* compress_motion      */  TRUE,
    /* compress_exposure    */  TRUE,
    /* compress_enterleave  */  TRUE,
    /* visible_interest     */  FALSE,
    /* destroy          */  Destroy,
    /* resize           */  ImageResize,
    /* expose           */  ImageResize,
    /* set_values       */  ImageSetValues,
    /* set_values_hook      */  NULL,
    /* set_values_almost    */  XtInheritSetValuesAlmost,
    /* get_values_hook      */  NULL,
    /* accept_focus     */  NULL,
    /* version          */  XtVersion,
    /* callback_private     */  NULL,
    /* tm_table         */  translations,
    /* query_geometry       */  XtInheritQueryGeometry,
    /* display_accelerator  */  XtInheritDisplayAccelerator,
    /* extension        */  NULL
  },
  { /* image fields */
    /* empty            */  0
  }
};


WidgetClass imageWidgetClass = (WidgetClass)&imageClassRec;

static void Destroy(w)
    Widget w;
{
    ImageWidget img = (ImageWidget) w;

    if (img->image.image && img->image.filename) {
	XDestroyImage(img->image.image);
    }
#ifdef PURIFY_WATCH
    purify_watch(img->image.filename);
#endif

    XtReleaseGC(XtParent(w), img->image.gc);

    /*
      This is done in the genesis side DELETE action
      if (w->image.filename) XtFree(w->image.filename); 
    */
}

static void ImageInit(request, new, args, num_args)
    Widget request, new;
    ArgList   args;
    Cardinal *num_args;
{
    ImageWidget img = (ImageWidget) new;
    img->image.image = NULL;

    /*
    ** Default width and height set by wx and wy the width cannot be
    ** changed by default since it is constrained by the framed widget on
    ** which it's based although I don't understand why it can't be
    ** overridden except in doing a SetValues.
    */
    img->core.width = img->image.wx;
    img->core.height = img->image.wy;

    ResetGC(img);
}

/*
** Reset the values in the GC.
*/
static void 
ResetGC(gif)
ImageWidget gif;
{
    Display  *display = XtDisplay(XtParent((Widget) gif));
    Drawable  drawable;
    Widget    ptr;
    XGCValues values;
    XtGCMask  mask = GCForeground;

    if (gif->image.gc) {
	XtReleaseGC(XtParent((Widget) gif), gif->image.gc);
    }

    /*
    ** When this gets called, the parent widget may not have a window
    ** defined.  However, to create the GC, we need a drawable, and it
    ** can't necessarily be the default root window because the drawable's
    ** depth and visual must match that of the widget we're creating.
    ** (Otherwise, you get BadMatch errors in some later calls.)  This
    ** is grungy, but I can't seem to find another way to get at the
    ** enclosing window except to search backward through the parent
    ** widgets and look for one.  So....
    */
    ptr = (Widget) gif;
    while (ptr->core.window == 0 && ptr->core.parent) {
	ptr = ptr->core.parent;
    }
    if (ptr->core.window != 0) {
	drawable = ptr->core.window;
    } else {
	/* Not sure what else to do. */
	drawable = XDefaultRootWindow(display);
    }

    values.foreground = XBlackPixel(display, XDefaultScreen(display));
    gif->image.gc = XCreateGC(display, drawable, mask, &values);
}


/*
** ImageResize is used as the handler for Expose events too.
*/
void ImageResize(w)
    ImageWidget	w;
{
    if (XtIsRealized((Widget) w) && w->image.image) {
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, False);
	XPutImage(XtDisplay(w), XtWindow(w), w->image.gc, w->image.image,
		  w->image.srcx, w->image.srcy,
		  w->image.dstx, w->image.dsty,
		  w->image.wx, w->image.wy);
    }
}


static Boolean ImageSetValues(current,request,new)
	ImageWidget	current;
	ImageWidget	request;
	ImageWidget	new;
{

	if (strcmp(current->image.fileformat,request->image.fileformat) != 0 ||
	strcmp(current->image.filename,request->image.filename) != 0 ||
	strcmp(current->image.storemode,request->image.storemode) != 0) {
		LoadImage(new);
	}

	if (strcmp(current->image.fileformat,request->image.fileformat) != 0 ||
	strcmp(current->image.filename,request->image.filename) != 0 ||
	strcmp(current->image.storemode,request->image.storemode) != 0 ||
	current->image.srcx != request->image.srcx ||
	current->image.srcy != request->image.srcy ||
	current->image.dstx != request->image.dstx ||
	current->image.dsty != request->image.dsty) {
		return(TRUE);
	}
	if (current->image.wx != request->image.wx ||
	    current->image.wy != request->image.wy) {


		/* Won't get entered unless ximage has a resource to allow this
		 * change to occur from a GENESIS Set command.
		 */
		printf("Image Set Image  curwx=%d curwy=%d reqwx=%d reqwy=%d\n",
		       current->image.wx, current->image.wy,
		       request->image.wx, request->image.wy);
		return(TRUE);
	}
	return(FALSE);
}


static void LoadImage(w)
ImageWidget w;
{
    XImage *XGIFLoad();

    if (w->image.image && w->image.filename) {
	XDestroyImage(w->image.image);
	w->image.image = NULL;
    }

    w->image.image = XGIFLoad(w->image.filename, (Widget) w->core.self,
			      &(w->image.wx), &(w->image.wy));

    /* If the user has not specified an explicit hgeom then the following
     * will take into affect to set the hgeom to the height of the widget.
     */
    w->core.width = w->image.wx;
    w->core.height = w->image.wy;

    /* printf("wx=%d wy=%d\n",w->image.wx, w->image.wy); */
}

static void ImageAction(w,e,str,num)
	ImageWidget w;
	XEvent	*e;
	String	*str;
	Cardinal	*num;
{
}
