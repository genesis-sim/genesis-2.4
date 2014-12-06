/* $Id */
/*
 * $Log: xo_cmds.c,v $
 * Revision 1.3  2006/02/03 21:21:59  svitak
 * Fix for newer window managers which won't allow windows to be raised
 * to the top of the stack under the guise of "focus stealing prevention".
 *
 * Revision 1.2  2005/07/01 10:03:00  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:52  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.18  2001/06/29 21:12:48  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.17  2001/04/25 17:16:58  mhucka
 * Misc. small changes to improve portability and address compiler warnings.
 *
 * Revision 1.16  2000/06/12 04:28:22  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.15  2000/05/02 06:19:48  mhucka
 * Added #include of Xo_ext.h.
 *
 * Revision 1.14  1999/10/16 22:50:22  mhucka
 * Merged in changes from Venkat for supporting "xsimplot".
 *
 * Revision 1.12.1.4  1998/01/19 03:08:55  venkat
 * Hardcoded inclusion of diskio interface headers no longer done. Taken
 * care of through the -I option in Makefile.
 *
 * Revision 1.12.1.3  1998/01/14 22:09:15  venkat
 * Implementation of xosimplot() and utility functions to support
 * the xsimplot command.
 *
 * Revision 1.12.1.2  1997/07/16 21:26:43  dhb
 * Changed getopt() name to avoid conflicts with unix version
 *
 * Revision 1.12.1.1  1997/06/28 00:07:40  venkat
 * Defined functions xoIsNestedForm() and xoIsNestedOptionSet() that
 * determine if a form is embedded or not.
 *
 * Revision 1.12  1995/08/03  01:04:27  venkat
 * Added function xogetstat() for the xgetstat command and avoided initializing
 * Xodus in xinit() if genesis was invoked with -nox
 *
 * Revision 1.11  1995/05/02  22:00:06  venkat
 * xocolorscale() returns with error message if DISPLAY is not set.
 *
 * Revision 1.10  1995/04/12  18:19:07  venkat
 * Hiding and showing forms also gives the problem of floating forms.
 * So introduced the hack in the CREATE and SET actions for the form
 * in xo_show_on_top() too.
 *
 * Revision 1.9  1995/03/21  01:17:37  venkat
 * Checked for color being supported before attempting to allocate
 * colorscales in xocolorscale()
 *
 * Revision 1.8  1994/12/15  17:33:06  dhb
 * Changed xoFindParentForm() and xoFindParentDraw() which check the
 * object type of an element to check the BaseObject() type of the
 * element.  This allows extended xform and xdraw based objects.
 *
 * Revision 1.7  1994/12/07  18:13:03  venkat
 * Same as 1.6; Just entering log
 * Fixed xmap and xtextload core dumps
 *
 * Revision 1.6  1994/12/06  00:36:04  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.5  1994/05/26  13:49:09  bhalla
 * Added ResizeDraws to the xupdate command so it forces a reprojection
 * of the contents of the draw
 *
 * Revision 1.4  1994/02/08  22:24:44  bhalla
 * Added xoFullName utility func to return element name with index if
 * nonzero, for giving widgets sensible names during create
 *
 * Revision 1.3  1994/02/02  20:26:05  bhalla
 * *** empty log message ***
 * */

#include <string.h>
#include "sim_ext.h"
#include "shell_func_ext.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "xo_ext.h"
#include "_widg/widg_struct.h"
#include <Widg/Form.h>
#include <Draw/CoreDraw.h>
#include <Xo/Xo_ext.h>

#include <stdlib.h>
#include "absff_ext.h"

static void xoCleanUp_SimplotDiskioInterfaces();
/* 
 * Commands.  Most of these are frontends to the corresponding Xg functions
 */

void xinit_defunct()
{
 fprintf(stderr, "xinit is no longer necessary in GENESIS 2.0\n");
}

/* @@SECTION xinit */
void xinit (argc, argv)
     int argc;
     char **argv;
{
  extern int GENOPT_NoX;
 if (!GENOPT_NoX)
   XgInitialize();
  /* Add the xodus actions to GENESIS*/
  /* Spiff: I wish I knew what these actions mean. */

    AddAction("B1DOWN",B1DOWN);
    AddAction("B2DOWN",B2DOWN);
    AddAction("B3DOWN",B3DOWN);
    AddAction("ANYBDOWN",ANYBDOWN);
  
    AddAction("B1UP",B1UP);
    AddAction("B2UP",B2UP);
    AddAction("B3UP",B3UP);
    AddAction("ANYBUP",ANYBUP);
  
    AddAction("B1DOUBLE",B1DOUBLE);
    AddAction("B2DOUBLE",B2DOUBLE);
    AddAction("B3DOUBLE",B3DOUBLE);
    AddAction("ANYBDOUBLE",ANYBDOUBLE);
  
    AddAction("NOBMOVE",NOBMOVE);
    AddAction("B1MOVE",B1MOVE);
    AddAction("B2MOVE",B2MOVE);
    AddAction("B3MOVE",B3MOVE);
    AddAction("ANYBMOVE",ANYBMOVE);
  
    AddAction("NOBENTER",NOBENTER);
    AddAction("B1ENTER",B1ENTER);
    AddAction("B2ENTER",B2ENTER);
    AddAction("B3ENTER",B3ENTER);
    AddAction("ANYBENTER",ANYBENTER);
  
    AddAction("NOBLEAVE",NOBLEAVE);
    AddAction("B1LEAVE",B1LEAVE);
    AddAction("B2LEAVE",B2LEAVE);
    AddAction("B3LEAVE",B3LEAVE);
    AddAction("ANYBLEAVE",ANYBLEAVE);
  
    AddAction("KEYPRESS",KEYPRESS);
    AddAction("UPDATEFIELDS",UPDATEFIELDS);
  
    AddAction("XUPDATE",XUPDATE);
    AddAction("XODRAG",XODRAG);
    AddAction("XODROP",XODROP);
    AddAction("XOWASDROPPED",XOWASDROPPED);
    AddAction("XOCOMMAND",XOCOMMAND);
}

/* @@SECTION xverify */

/* This is private and should not be accessed by scripts */
/* utility function to go through the arg checking */
struct xwidg_type *xverify(argc,argv)
     int argc;
     char **argv;
{
	struct xwidg_type	*widg;

	initopt(argc, argv, "widget-element");
	if (G_getopt(argc, argv) != 0) {
		printoptusage(argc, argv);
		return((struct xwidg_type *)NULL);
	}


	widg = (struct xwidg_type *)GetElement(argv[1]);
	if (!widg) {
		printf("error in %s: could not find widget-element %s\n",	
			argv[0],argv[1]);
		return((struct xwidg_type *)NULL);
	}

	/* Check to see if the object is of the widget class */
	if (!CheckClass(widg,ClassID("widget"))) {
		printf("error in %s: element %s is not a widget\n",	
			argv[0],argv[1]);
		return((struct xwidg_type *)NULL);
	}
	return(widg);
}

/* @@SECTION xoraise	 */
void xoraise(argc,argv)
     int argc;
     char **argv;
{
	struct xwidg_type	*widg;
	Widget w;
	Boolean xoIsNestedForm(), xoIsNestedOptionSet();
	
	if ((widg=xverify(argc,argv))) {
		w=(Widget)(widg->widget);
		if (XtIsSubclass(w,formWidgetClass) && ! (xoIsNestedForm(widg) && xoIsNestedOptionSet(widg)) )
			w=XtParent(XtParent(XtParent(XtParent(w))));
		else
			w=XtParent(w);
		XRaiseWindow(XtDisplay(w),XtWindow(w));
	}
}

/* @@SECTION xolower */
void xolower(argc,argv)
     int argc;
     char **argv;
{
	struct xwidg_type	*widg;
	Widget w;
	Boolean xoIsNestedForm(), xoIsNestedOptionSet();
	
	if ((widg=xverify(argc,argv))) {
		w=(Widget)(widg->widget);
		if (XtIsSubclass(w,formWidgetClass) && !(xoIsNestedForm(widg) &&xoIsNestedOptionSet(widg)))
			w=XtParent(XtParent(XtParent(XtParent(w))));
		else
			w=XtParent(w);
		XLowerWindow(XtDisplay(w),XtWindow(w));
	}
}

void xoshow_on_top(argc,argv)
     int argc;
     char **argv;
{
	struct xwidg_type	*widg;
	struct xform_type	*form;
	Widget w;
	Boolean xoIsNestedForm(), xoIsNestedOptionSet();
	
	if ((widg=xverify(argc,argv))) {
		w=(Widget)(widg->widget);
		if (XtIsSubclass(w,formWidgetClass) && !(xoIsNestedForm(widg) && xoIsNestedOptionSet(widg))){
			form = (struct xform_type *) widg;
			xoGetGeom(form);
			XoWMPositionHack((Widget)form->shell,(Position) atoi(form->xgeom),(Position)atoi(form->ygeom));
			w=XtParent(XtParent(XtParent(XtParent(w))));
		}	
		else
			w=XtParent(w);
		XtMapWidget(w);
		XRaiseWindow(XtDisplay(w),XtWindow(w));
	}
}

/* Extend this so it sets the pixflags for a child pix */
void xoshow(argc,argv)
     int argc;
     char **argv;
{
	struct xwidg_type	*widg;
	Widget w;
	Boolean xoIsNestedForm(), xoIsNestedOptionSet();
	
	if ((widg=xverify(argc,argv))) {
		w=(Widget)(widg->widget);
		if (XtIsSubclass(w,formWidgetClass) && !(xoIsNestedForm(widg) && xoIsNestedOptionSet(widg)))
			w=XtParent(XtParent(XtParent(XtParent(w))));
		else
			w=XtParent(w);
		XtMapWidget(w);
	}
}

/* Extend this so it sets the pixflags for a child pix */
void xohide(argc,argv)
     int argc;
     char **argv;
{
	struct xwidg_type	*widg;
	Widget w;
	Boolean xoIsNestedForm(), xoIsNestedOptionSet();
	
	if ((widg=xverify(argc,argv))) {
		w=(Widget)(widg->widget);
		if (XtIsSubclass(w,formWidgetClass) && !(xoIsNestedForm(widg) && xoIsNestedOptionSet(widg)))
			w=XtParent(XtParent(XtParent(XtParent(w))));
		else
			w=XtParent(w);
		XtUnmapWidget(w);
	}
}

 
void xocolorscale(argc,argv)
     int argc;
     char **argv;
{
    char    *name;
    char    *value;
 
    if(argc < 2){
        fprintf(stderr, "usage: %s colorscale\n",argv[0]);
        fprintf(stderr, "   colorscales : hot rainbow grey\n");
        return;
    }

    if(!XgDisplay()){
	fprintf(stderr, "Xodus cannot be supported in this display\n");
	fprintf(stderr, "Check DISPLAY env variable and restart genesis\n");
	return;
    }	

    if(!XoColorSupported()){
	fprintf(stderr, "Color not supported in this display\n");
	fprintf(stderr,"Not loading colorscale: %s\n", argv[1]);
        return;
    } 
    XoMakeCmap(argv[1], FALSE);
}
 

/**************************************************************

	Implementation of the xsimplot command which allows the 2-D graph
	plotting of variables in a file created by the diskio element. 
	The diskio library now supports FMT1 files in addition to netcdf.

 ****************************************************************/

int xosimplot(argc, argv)
 int argc;
 char **argv;
{
 char *plotname;
 char plotpath[200];
 int plotargc;
 char *plotargv[5];
 Element *overlayelm;

 struct xgraph_type *graph;
 char *pathtofile;
 
 int cell = 0;
 int overlay = 0;
 float overlayoffset = 0.0;
 int skip = 1;
 char *color = "Black";
 
 int optstatus;
 Abs_Variable *coordi, *avi;
 Abs_Diskio *adi;

 extern Element * xoAcquireOverlay(/* Element * */);
 extern char * xoSimplotName(/* int cell, int overlay */);
 
 /****************************************************
	
 The plots plotted by invocation of the xsimplot command 
 will exist under an overlay neutral element below the 
 parent graph in the element hierarchy. The xoAcquireOverlay() 
 function allows the reuse of the overlay neutral which exists 
 in the event of the graphs->overlay field being set.

 The name of the plot is determined by the xoSimplotName() function
 which uses the offset into the file (given by -cell option) and 
 the overlay of the plot (given by the -overlay option) in the context 
 of the xsimplot command.

 Note that this overlay option to xsimplot is not the same
 as the functionality associated with the overlay field of the 
 graph element. The commonality stops at the use of the overlay 
 neutral element to hold the resulting plots.

 *****************************************************/
 

 if (initopt(argc, argv, "graphelm pathtofile -cell cellnum -skip skipby -overlay overlaynum -offset overlayoffset -color col -gain gainval -old") == -1 || optargc != 3) {
	printoptusage(argc, argv);
	return 0;
 }
	
 graph = (struct xgraph_type *) GetElement(optargv[1]);
 if (graph == NULL || strcmp(BaseObject(graph)->name, "xgraph") != 0) {
	fprintf(stderr, "Error in '%s': %s is not a graph \n", argv[0],
				optargv[1]);
	return 0;
 }
  
 pathtofile = optargv[2]; 

 /* Process command-line options and store in local variables */

 while((optstatus = G_getopt(argc, argv))) {
	if (optstatus == -1) {
		fprintf(stderr, "%s: Error while processing option %s\n",
				argv[0], G_optopt);
		printoptusage(argc, argv);
		return 0;
	}
	
	if (strcmp(G_optopt, "-cell") == 0) 
		cell = atoi(optargv[1]);
	else if (strcmp(G_optopt, "-skip") == 0)
		skip = atoi(optargv[1]);
	else if (strcmp(G_optopt, "-overlay") == 0)
		overlay = atoi(optargv[1]);
	else if (strcmp(G_optopt, "-offset") == 0)
		overlayoffset = Atof(optargv[1]);
	else if (strcmp(G_optopt, "-color") == 0)
		color = optargv[1];
	else if (strcmp(G_optopt, "-gain") == 0)
		fprintf(stderr, "%s: %s option is not implemented\n", argv[0], 
					optargv[0]);
 } 	 

 /*
 Create the overlay neutral (if needed) and plots under the graph and pass the 
 points to be plotted.
 */

 overlayelm = (Element *) xoAcquireOverlay((Element *)graph);
 plotargc = 5;
 plotargv[0] = "create";
 plotargv[1] = "xplot";

 plotname = xoSimplotName(cell, overlay);
 strcpy(plotpath, "");
 strcat(plotpath, Pathname(overlayelm));
 strcat(plotpath, "/");
 strcat(plotpath, plotname);
 plotargv[2] = plotpath;

 plotargv[3] = "-fg";
 plotargv[4] = color;

 do_create(plotargc, plotargv);

/****************************************************************
  BEGIN SECTION:

	This section deals with the creation of the diskio abstract 
        interface objects, invocation of their methods to obtain
	the relevant data based on the command-line options to 
	xsimplot and the passing off of the data to the 
  	created plot for plotting. 

 ***************************************************************/
	

 {
 
 /*
  Use the diskio abstract interfaces and the command-line options to 
  buffer the needed data points 
 */ 
 
 float t, dt;
 char *yvals[1];
 int i, numvals;
 Abs_Varshape *shape;

 char *formatname = CopyString(" ");
 adi = Absff_CreateDiskio_Interface(pathtofile, 'r', &formatname);

 if (adi == NULL) {
	/* Error : file not found */
	return 0;
 }
 
 coordi = adi->ConstructCoord_Interface(adi, "SimulationTime");
	 
 if (coordi != NULL) {
	/************************************************************ 
	Coordinate variables are characteristic of file formats such 
	as netcdf. These are special variables which do not store data 
	per se but store the basis of any histogram of values. For example,
	in Genesis simulations, "time" is the basis of most histograms. 
	Currently "SimulationTime" is the only coordinate variable allowed 		in the diskio/netcdf framework. 
	**********************************************************/

	long start_index[1], count[1];
	char *vals[2];
	start_index[0] = 0L; count[0] = 2L;
	coordi->GetValuesAsString(coordi, vals, start_index, count);
	t = Atof(vals[0]);
	dt = Atof(vals[1]) - Atof(vals[0]);
	FreeString(vals[0]);
	FreeString(vals[1]);
 } else {
	t = adi->GetStartTime(adi);
	dt = adi->GetDt(adi);
 }

  if (coordi != NULL) {
	/******************************************************
	HACK:
	____
		For file formats supporting the use of coordinate
		variables, the variablenames array maintained by
		the diskio interface also holds the names of the
		coordinate variables. Since at this time, only the
		"Simulation Time" coordinate variable can exist, I add 
		one before indexing the appropriate variable name for 
		construction of the variable interface, to account for
		the extra coordinate variable. 

		Something more elegant would be to change the interface
		to hold coordinate variable names in an array seperate 
		from the oridinary variable array.
	*******************************************************/

	if (adi->nvariables <= (cell + 1) || cell < 0) {
		fprintf(stderr, "xsimplot error: invalid cell '%d' specified\n", cell); 
		xoCleanUp_SimplotDiskioInterfaces(adi, coordi, NULL);
		return 0;
	}
  	avi = adi->ConstructVariable_Interface(adi, adi->variablenames[cell+1], abs_float32, NULL);
  }
  else {
	if (adi->nvariables <= cell || cell < 0) {
		fprintf(stderr, "xsimplot error: invalid cell '%d' specified\n", cell); 
		xoCleanUp_SimplotDiskioInterfaces(adi, coordi, NULL);
		return 0;
	}
  	avi = adi->ConstructVariable_Interface(adi, adi->variablenames[cell], abs_float32, NULL);
  }

  if (avi == NULL) {
	/* Error */
	xoCleanUp_SimplotDiskioInterfaces(adi, coordi, NULL);
	return 0;
  }

  /* These are NOPs in netcdf but do buffering in the case of FMT1 */
  adi->SetVariableInterface(adi, cell, avi);
  adi->FillVariableValues(adi, cell);



 /* 
   We should ideally allocate vals based on the avi->GetType(avi)
   Also, the concrete implementation of GetValues() should be changed
   to not make it necessary to allocate vals from the heap. This means
   conversion from the abstract to the concrete type should not 
   be implemented by mallocing for the appropriate type which obviates
   the need to free the vals pointer and avoid memory leaks.

   See: Netcdf_Convert_To_nc_type to understand what I mean!
*/
  

  shape = avi->GetShape(avi);
  if (shape == NULL) {	
	/* Error */
	xoCleanUp_SimplotDiskioInterfaces(adi, coordi, avi);
	return 0;
  }
  if (shape->ndomains == 0)
	numvals = 1;
  else
	numvals = shape->domainsize[0];

  for (i=0; i<numvals; i+=skip) {
	Element *GetElement();
	long start_index[1], count[1];
	char xval[60], yval[60];

	start_index[0] = (long)i; count[0] = 1L;

	sprintf(xval, "%g", t); 
	avi->GetValuesAsString(avi, yvals, start_index, count);
	sprintf(yval, "%g", Atof(yvals[0])+overlayoffset);
	FreeString(yvals[0]);
	xoAddPtsToPlot(GetElement(plotpath), xval, yval);
	t += dt*skip;
  }

 }
/****************************************************************
  END SECTION: 
	diskio interface method invocation for data gathering 
	and plotting.
 ***************************************************************/

	
	/* Destroy the interfaces */
  xoCleanUp_SimplotDiskioInterfaces(adi, coordi, avi);

	return 0;
}

 /****************************************************** 
  	Destroy the interfaces created in xosimplot(). This function 
	encapsulates the order of invocation of the interface destruction 
 	methods which is important since the implementation and construction
  	of interface structures for different file formats is different.
 ********************************************************/

static void xoCleanUp_SimplotDiskioInterfaces(adi, coordi, avi)
 Abs_Diskio *adi;
 Abs_Variable *coordi, *avi;
{
  if (coordi != NULL) {
	coordi->Destruct(coordi);
	free(coordi);
  }

  if (avi != NULL) {
  	avi->Destruct(avi);
  	adi->FreeChildInterface(adi, avi);
  }

  if (adi != NULL) {
  	adi->DestructChildInterfaces(adi);
  	adi->Destruct(adi);
  	free(adi);
  }
}

char * xoSimplotName(cell, overlay)
 int cell;
 int overlay;
{
  static char buf[80]; 
  char locbuf[10];

 strcpy(buf, "");

 sprintf(locbuf, "%d", cell);
 strcat(buf, locbuf);

 strcat(buf, " ");

 sprintf(locbuf, "%d", overlay);
 strcat(buf, locbuf);

 return buf;

}

void xoupdate(argc,argv)
     int argc;
     char **argv;
{
    struct xwidg_type   *widg;
    Widget w;
 
    if ((widg=xverify(argc,argv))) {
        w=(Widget)(widg->widget);
        if (XtIsSubclass(w,coredrawWidgetClass)) {
			/* The core_class is not recognized - I need headers */
			/*  (*XtClass(w)->core_class.resize)(w); */
			ResizeDraws(w);
        }
        XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
    }
}
 
void xops(argc,argv)
     int argc;
     char **argv;
{
    XoSetPSparms(argc,argv);
}
 
void xoflushevents(argc,argv)
     int argc;
     char **argv;
{
    /* Just call the event loop to get rid of the backlogs */
    XgEventLoop();
}


Widget xoFindParentForm(elm)
	Element	*elm;
{
	Element *parent;

	for(parent = elm->parent; parent ;parent = parent->parent) {
		if (strcmp(BaseObject(parent)->name, "xform") == 0) {
			return((Widget)((struct xwidg_type *)parent)->widget);
		}
	}
	XgError("Could not find any ancestor of type xform");
	return((Widget)NULL);
}

Boolean xoIsNestedForm(elm)
 Element *elm;
{
	Element *parent;

        for(parent = elm->parent; parent != NULL ;parent = parent->parent) {
                if (strcmp(BaseObject(parent)->name, "xform") == 0) {
                        return(True);
                }
        }
        return(False);
}

Boolean xoIsNestedOptionSet(elm)
        Element *elm;
{
        register struct xform_type *form = (struct xform_type *)elm;
        return(form->nested?True:False);
}

 
Widget xoFindParentDraw(elm)
    Element *elm;
{
    Element *parent;
    for(parent = elm; parent ;parent = parent->parent) {
        if (strcmp(BaseObject(parent)->name, "xcoredraw") == 0 ||
            strcmp(BaseObject(parent)->name, "xdumbdraw") == 0 ||
            strcmp(BaseObject(parent)->name, "xdraw") == 0 ||
            strcmp(BaseObject(parent)->name, "xgraph") == 0) {
            Widget w=(Widget)((struct xwidg_type *)parent)->widget;
            if (XtIsSubclass(w,coredrawWidgetClass)) {
                return(w);
            }
        }
    }
    XgError("Could not find any ancestor subclassed from CoreDraw");
    return((Widget)NULL);
}
 
void xoUpdateParentDraw(elm)
    Element *elm;
{
    Widget w = xoFindParentDraw(elm);
    if (w)
        XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
}


/* This function handles the <widget> syntax for backwards
** compatibility. This syntax should be considered deprecated and its
** use is discouraged */
void xoParseScriptArg(elm,cur_arg,value)
	Element	*elm;
	char	*cur_arg;
	char	*value;
{
	static char	newval[200];
	char	*temp,*temp2;

	if ((temp =strstr(value,"<widget>")) == NULL) {
		DirectSetElement(elm,cur_arg,value);
	} else {
		strcpy(newval,value);
		temp2=Pathname(elm);
		sprintf(&(newval[temp-value]),
			"%s%s",temp2,temp+strlen("<widget>"));
		free(temp2);
		DirectSetElement(elm,cur_arg,newval);
	}
}

void xoRedraw(elm)
	Element	*elm;
{
	struct xwidg_type *widg = (struct xwidg_type *)elm;
	Widget w;

	if (!CheckClass(widg,ClassID("widget"))) {
		printf("error in xoRedraw: element %s is not a widget\n",	
			Pathname(elm));
			return;
	}
	w = (Widget)widg->widget;
	if (w)
		XClearArea(XtDisplay(w),XtWindow(w),0,0,0,0,True);
}

int xogetstat()
{
 extern int GENOPT_NoX;
 if (GENOPT_NoX || XgDisplay() == NULL)
	return(0);
 else
	return(1);
}

char *xoFullName(elm)
	Element *elm;
{
	static char	str[100];
	if (!elm)
		return("default");
	if (elm->index != 0)
		sprintf(str,"%s[%d]",elm->name,elm->index);
	else
		sprintf(str,"%s",elm->name);
	return(str);
}
