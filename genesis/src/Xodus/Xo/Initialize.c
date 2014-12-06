/* $Id: Initialize.c,v 1.3 2005/07/20 20:01:57 svitak Exp $ */
/*
** $Log: Initialize.c,v $
** Revision 1.3  2005/07/20 20:01:57  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:02:50  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2000/09/11 15:47:05  mhucka
** Trivial change: made the XODUS banner line be the same width as the
** GENESIS startup banner lines.
**
** Revision 1.5  2000/09/08 04:20:14  mhucka
** The RCS/CVS Id and Log keywords lacked trailing dollar signs,
** and hence weren't being recognized by the version control
** systems.
** 
*/

#include <stdio.h>	/* printf() */
#include <X11/Intrinsic.h>

typedef struct _XgApplicationData {
	Boolean         initialized;
	XtAppContext    app_con;
	Display        *display;
}               XgApplicationData;

static XgApplicationData application_data = {
	False,
	NULL,
	NULL
};

void            XgInitialize();
void            XgEventLoop();
XtAppContext    XgAppContext();
Display        *XgDisplay();

/* These are the default resources to use if the user does not have his own
 * X resource file.
 */
String          fallback_resources[] = {
	"*borderWidth: 2",
	"*borderColor: LightSteelBlue",
	"*background: LightSteelBlue",
	NULL
};

/* @@SECTION XgInitialize */
void 
XgInitialize()
{
	int             argc = 0;
	String          argv[1];

	if (application_data.initialized)
		return;

	printf("\n--------------------------- Starting XODUS 2.0 ---------------------------\n\n");

	/* Initialize the X Toolkit internals */
	XtToolkitInitialize();

	/* Every application must have an application context */
	application_data.app_con = XtCreateApplicationContext();

	/* Specify default set of resource values. */
	XtAppSetFallbackResources(application_data.app_con, fallback_resources);

	/* Connect a client program to an X server */
	if (!(application_data.display = XOpenDisplay(NULL))) {
		XgError("Couldn't open display.  Check DISPLAY envariable");
		application_data.display = NULL;
		return;
	}
	application_data.initialized = True;

	/* Initialize and add the display above to the application context. */
	XtDisplayInitialize(application_data.app_con, application_data.display,
			    "genesis", "Genesis", NULL, 0, &argc, argv);

	/* Set up the colorscale */
	XoInitColorscale();

	/* Set up the cursor */
	XoInitCursor();

	/*
	 * Presumably AddJob adds the function XgEventLoop as a job in the
	 * GENESIS loop.
	 */
	AddJob(XgEventLoop, 1);

}

/* @@SECTION XgEventLoop */
void 
XgEventLoop()
{
	XEvent          event;

	/* Returns non zero if there are any Xevents on the queue. */
	while (XtAppPending(application_data.app_con) != 0) {

		/* Get next event from the X event queue. */
		XtAppNextEvent(application_data.app_con, &event);

		XtDispatchEvent(&event);
	}
}

int XgEventStep(int limit)
{
	XEvent          event;
	int ret = 0;

	/* Returns non zero if there are any Xevents on the queue. */
	if (XtAppPending(application_data.app_con) != 0) {
		if ((ret = XPending(application_data.display)) > limit) {
			XSync(application_data.display, True);
			return 0;
		}

		/* Get next event from the X event queue. */
		XtAppNextEvent(application_data.app_con, &event);

		XtDispatchEvent(&event);
	}
	return ret;
}
 
/* @@SECTION XgAppContext */
/* Accessor functions */
XtAppContext XgAppContext()
{
	return application_data.app_con;
}

/* @@SECTION XgDisplay */
Display        *XgDisplay()
{
	return application_data.display;
}
