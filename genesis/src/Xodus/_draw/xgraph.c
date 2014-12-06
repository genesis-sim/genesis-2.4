/* $Id: xgraph.c,v 1.3 2006/01/09 16:31:06 svitak Exp $ */
/*
 * $Log: xgraph.c,v $
 * Revision 1.3  2006/01/09 16:31:06  svitak
 * Fixed bug where destination of strcpy was smaller than source.
 *
 * Revision 1.2  2005/07/01 10:02:58  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.26  2001/06/29 21:11:45  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.25  2000/06/12 04:10:20  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added missing type casts.
 *
 * Revision 1.24  2000/05/19 19:24:04  mhucka
 * Added #include of strings.h, to declare functions like index().
 *
 * Revision 1.23  1998/01/14 22:00:26  venkat
 * Implemented function xoAcquireOverlay(graph) which returns the overlay
 * neutral under the graph, if it exists or creates one to return if
 * it doesnt exist.
 *
 * Revision 1.22  1997/06/03 19:33:11  venkat
 * Clean-up of overlay code. There is now only 1 xoverlay neutral under
 * which all plot copies are placed.
 *
 * Revision 1.21  1996/10/05  17:22:45  dhb
 * Added USECLOCK action: sets child elements to use the same
 * clock as the xgraph.
 *
 * New plots created by a PLOT message to xgraph will use the
 * same clock as the xgraph.
 *
 * Revision 1.20  1995/09/27  00:09:22  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.19  1995/07/08  01:08:49  venkat
 * The ADDMSGIN and the XUPDATE actions now check for the number of
 * args to avoid coredumps in the case of explicit calls.
 *
 * Revision 1.18  1995/06/26  23:27:29  dhb
 * Merged in 1.17.1.1
 *
 * Revision 1.17.1.1  1995/06/12  17:08:42  venkat
 * xoZapOverlays() no longer does list management. DeleteElement() takes care of this
 * anyway.
 *
 * Revision 1.17  1995/04/12  23:00:01  dhb
 * Changes from Upi which changes the behaviour of plots on RESET.
 * They no longer reset their color and label from the message slot
 * data on a RESET, rather these values are initialized at message
 * setup time only.
 *
 * Revision 1.16  1995/03/21  21:52:52  venkat
 * PROCESS action sets the processed flag.
 * RESET checks for the flag and handles the overlays.
 * If it zaps the overlays, then the overlay_no is reset to 0.
 *
 * Revision 1.15  1995/03/17  20:43:15  venkat
 * Fixed bug in xoMakeNewPlot() which was wrongly checking the name of
 * the BaseObject with "xoverlay" instead of the child->name itself
 * Changed call in xoZapOverlays() to FreeTree() to DeleteElement()
 * This was done in consequence to the fact that FreeTree() was no longer
 * calling the DELETE action. Also fixed the list management procedure in`
 * xoZapOverlays().
 *
 * Revision 1.14  1995/03/02  18:32:34  dhb
 * Replaced use of regular messages in auto creation of plots
 * to use message forwarding.  (This was implemented before
 * and appears to have been lost when other changes were
 * merged in.  I hope this isn't a problem with RCS merging
 * or how I'm using it!)
 *
 * Revision 1.13  1995/01/05  19:31:49  venkat
 * Changed strcmp(BaseObject(child)->name, "xoverlay") back to
 * strcmp(child->name, "xoverlay") since the object->name is not
 * compared here.
 *
 * Revision 1.12  1995/01/04  17:23:31  venkat
 * ElementIsA() changed to BaseObject() form
 *
 * Revision 1.11  1994/12/20  00:57:02  dhb
 * Added missing change comments for 1.9 revision.
 *
 * Revision 1.10  1994/12/20  00:53:41  dhb
 * Merged in 1.8.1.1 changes
 *
 * revision 1.9
 * date: 1994/12/20 00:45:33;  author: dhb;  state: Exp;  lines: +14 -3
 * Replaced duplication of PLOT messages from source element to
 * xplot element with message forwarding.
 * 
 * Added a break statement in sreach for xplot to delete in the
 * DELETEMSGIN action.  Once we've found the xplot to delete we
 * need not continue the search.
 *
 * Revision 1.8.1.1  1994/12/17  00:36:28  venkat
 * Changed strcmp(elm->obj->name, "OBJNAME") to ElementIsA() format
 *
 * Revision 1.8  1994/11/07  22:11:14  bhalla
 * Two changes:
 * Added in fields cdxmin, cdymin etc for the draw xmin ymin etc. In
 * other words, these provide control over the total graph region, not
 * just the region within the axes.
 *
 * Also added in code for deleting plots using the DELETMSGIN option.
 * This works, but is hampered by the fact that the current version of
 * the Genesis base code itself does bad things when deleting messages.
 *
 * Revision 1.7  1994/05/26  13:47:36  bhalla
 * Major overhaul. xgraph now manages the creation of axes and the title,
 * which exist as the usual accessible genesis elements. Also
 * added the xo_graph_update_axis function to do updates when the
 * axis limits are changed
 *
 * Revision 1.6  1994/04/25  17:51:28  bhalla
 * Dealt with most of the msg bugs that caused trouble when elements
 * were deleted. Mainly replaced AddMsgIn with AddMsg. There is still
 * a problem with deleteall -f but it does not seem to be fatal
 *
 * Revision 1.5  1994/03/16  17:08:56  bhalla
 * Added default geometry for height = 80%
 * Changed the call to Attach to reflect changes in args of Attach.
 *
 * Revision 1.4  1994/02/08  22:33:37  bhalla
 *  Added xoFullName to get sensible names for the widgets
 *
 * Revision 1.3  1994/02/08  17:47:26  bhalla
 * Added call to xoGetGeom for geometry field initialization
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 * Added stuff for VERSION2 compatibility
 * */

#include <strings.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Widg/Form.h"
#include "Draw/CoreDraw.h"
#include "Draw/Graph.h"
#include "Draw/Pix.h"
#include "Draw/Shape.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"

static Gen2Xo GXconvert[] = {
	{"xmin",		XtNgxmin},
	{"xmax",		XtNgxmax},
	{"ymin",		XtNgymin},
	{"ymax",		XtNgymax},
	{"xoffset",		XtNxoffset},
	{"yoffset",		XtNyoffset},
	{"title",		XtNtitle},
	{"xlabel",		XtNxlabel},
	{"xunits",		XtNxunits},
	{"XUnits",		XtNxunits},
	{"ylabel",		XtNylabel},
	{"yunits",		XtNyunits},
	{"YUnits",		XtNyunits},
	{"drawflags",	XtNdrawflags},
	{"cdxmin",		XtNxmin},
	{"cdxmax",		XtNxmax},
	{"cdymin",		XtNymin},
	{"cdymax",		XtNymax},
};


#ifndef MAX_NTARGS
#define MAX_NTARGS 20
static int xoZapOverlays();
static int xoCountPlots();
static int xoMakeNewPlot();
static int xoNewIndex();
static void xoUpdateChildClocks();

static void xoCreateOverlay(/* Element *graph */);
static Element * xoGetOverlay(/* Element *graph */);
static int xoCopyToOverlay(/* Element *overlayelm, Element *plottocopy */);
static int xoGetOverlayTrialNumber(/* Element *overlayelm, char *nameofplot, int graphoverlaynumber */);
static char * xoStripToOverlayName(/* char *overlaytrialname, int *trialnumber */);

/* This function moves old plots to the neutral "xoverlay" element*/
void xoHandleOverlays(graph) 
    struct xgraph_type *graph;
{
	Element	*child;
	Element	*overlay;
	

	 /* if ((float)SimulationTime() == 0.0)
		return 0; */

	/*
		Tue Jun  3 10:48:55 PDT 1997 - Venkat 
		Since this is called only if the overlay field is set, we 
		can be certain there would be the neutral xoverlay element			under the graph. So we just get it.	
	*/

	overlay = xoGetOverlay(graph);


	for(child = graph->child; child; child = child->next) {
#ifdef VERSION2
		if (strcmp(BaseObject(child)->name, "xplot")==0 &&
			child->msgin &&
			(child->msgin->type == 0 || child->msgin->type == 1))
			/* PLOT == 0; PLOTSCALE == 1 */
#else
		if (strcmp(BaseObject(child)->name, "xplot") == 0 &&
			child->msg_in &&
			child->msg_in->type == 0)
#endif
			{ /* PLOT msg */
			/* Copy the old plots to the reused xoverlay neutral */

			if (! xoCopyToOverlay(overlay, child))
		 		printf("could not copy %s[%d] to %s[%d]\n",
					child->name,child->index,
					overlay->name,overlay->index);
		}
	}
	graph->overlay_no++;
}

int XGraph (graph, action)
     register struct xgraph_type *graph;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentForm();
	FramedWidget fW;
	int i;
	MsgIn	*msg;
	Element	*child;
	Action	newaction;
	char	*newargv[50];
	int	update_graph_offsets = 0;
  
  if (Debug(0) > 1)
    ActionHeader("XGraph", graph, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    /** This flag used to avoid creating multiple overlay labels
	on repeated RESETS **/
    graph->processed = 1;
    break;
  case RESET:
	/* Disable all refreshes in graph */
	xoSetDrawRefresh((Widget)graph->widget,0);	
	if (graph->overlay) {
		if(graph->processed) xoHandleOverlays(graph);
	} else { /* otherwise clear the graph of old plots. If there
			 ** were any old plots we will need to update offsets */
		update_graph_offsets = xoZapOverlays(graph);
		graph->overlay_no=0; /** Re-initialize the overlay number count **/
		/* xoRedraw(graph); */
	}
	XoUpdatePlotAxes((GraphWidget)graph->widget,
			graph->xoffset != 0.0,
			graph->yoffset != 0.0);
	xoSetDrawRefresh((Widget)graph->widget,1);	
	xoRedraw(graph);
	graph->processed = 0;
    break;
  case ADDMSGIN:
	/* move the relevant msg (which is passed in action->data)
	* onto the new plot */
	if((MsgIn *) (action->data) == NULL)
		break;
	 if (xoMakeNewPlot(graph,(MsgIn *)(action->data)) == 1)
		/* made a new plot, so the msg was handled internally */
		return(1);
	return(0);
	/* NOTREACHED */
	break;
  case DELETEMSGIN: { /*
					** This is an awful hack to kill a plot
					** when the message to the graph is deleted
					** I would like to disclaim all responsibility
					** for the idea, which I opposed. -- Upi Bhalla.
					*/
	char	name[80];
	char	*ptr;
	int		index;
	char	*GetStringMsgData();
	Element *overlayelm; 

	msg = (MsgIn *)action->data;
	if (!msg)
		break;
	strcpy(name,GetStringMsgData(msg,1));
	index = 0;
	if ((ptr = strchr(name,'['))) {
		*ptr = '\0';
		index = atoi(ptr +1 );
	}

	if (graph->overlay)
		overlayelm = xoGetOverlay(graph);
	else
		overlayelm = NULL;

	if (msg->type != 2 && msg->type != 3) { /* X or WAVEPLOT msg */
		for(child = graph->child; child ; child = child->next) {
			if (strcmp(child->name,name) == 0 &&
				child->index == index) {
				if (overlayelm != NULL)
					xoCopyToOverlay(overlayelm, child);
				DeleteElement(child);
			}
		}
	}
	xoRedraw(graph);
	}
	break;
  case USECLOCK:
    /* xgraph's clock is changing; change all children to match */
    xoUpdateChildClocks(graph, * (int*) action->data);
    return(1);
  case CREATE:
    /* arguments are: object_type name [field value] ... */
    
    if ((parentW = xoFindParentForm(graph)) == NULL) return(0);
	fW = (FramedWidget)XtVaCreateManagedWidget(
		xoFullName(graph), framedWidgetClass, parentW,
						XtVaTypedArg,XtNhGeometry,XtRString,"80%",4,
					    XtNchildClass, graphWidgetClass,
					    XtNmappedWhenManaged, False,
					    NULL);
    graph->widget = (char *)XoGetFramedChild(fW);
	graph->cdxmin = -0.2;
	graph->cdymin = -0.2;
	graph->cdxmax = 1.2;
	graph->cdymax = 1.2;

	graph->ymax = graph->xmax = 100;
	XtAddCallback((Widget)(graph->widget),XtNcallback,xoCallbackFn,
		(XtPointer)graph);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */

	newaction.type = CREATE;
	newaction.name = "CREATE";
	newaction.data = (char *)graph;
	newaction.argv = newargv;

/* Creating the x axis */
	i = 0;
	newargv[i] = "xaxis"; i++;
	newargv[i] = "x_axis"; i++;
	newargv[i] = "-axx"; i++;
	newargv[i] = "1.0"; i++;
	newargv[i] = "-axy"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-axz"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-axmin"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-axmax"; i++;
	newargv[i] = "100"; i++;
	newargv[i] = "-axlength"; i++;
	newargv[i] = "1"; i++;
	newargv[i] = "-tickx"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-ticky"; i++;
	newargv[i] = "-1"; i++;
	newargv[i] = "-tickz"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-ticklength"; i++;
	newargv[i] = "0.04"; i++;
	newargv[i] = "-labeloffset"; i++;
	newargv[i] = "0.06"; i++;
	newargv[i] = "-unitoffset"; i++;
	newargv[i] = "0.1"; i++;
	newargv[i] = "-titleoffset"; i++;
	newargv[i] = "0.13"; i++;
	newargv[i] = "-textmode"; i++;
	newargv[i] = "draw"; i++;
	newargv[i] = "-rotatelabels"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-update_parent"; i++;
	newargv[i] = "1"; i++;
	newaction.argc = i;
	Create("xaxis","x_axis",graph,&newaction,0);


/* Creating the y axis */
	i = 0;
	newargv[i] = "xaxis"; i++;
	newargv[i] = "y_axis"; i++;
	newargv[i] = "-axx"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-axy"; i++;
	newargv[i] = "1"; i++;
	newargv[i] = "-axz"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-axmin"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-axmax"; i++;
	newargv[i] = "100"; i++;
	newargv[i] = "-axlength"; i++;
	newargv[i] = "1"; i++;
	newargv[i] = "-tickx"; i++;
	newargv[i] = "-1"; i++;
	newargv[i] = "-ticky"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-tickz"; i++;
	newargv[i] = "0"; i++;
	newargv[i] = "-ticklength"; i++;
	newargv[i] = "0.02"; i++;
	newargv[i] = "-labeloffset"; i++;
	newargv[i] = "0.03"; i++;
	newargv[i] = "-unitoffset"; i++;
	newargv[i] = "0.1"; i++;
	newargv[i] = "-titleoffset"; i++;
	newargv[i] = "0.14"; i++;
	newargv[i] = "-textmode"; i++;
	newargv[i] = "radraw"; i++;
	newargv[i] = "-rotatelabels"; i++;
	newargv[i] = "1"; i++;
	newargv[i] = "-update_parent"; i++;
	newargv[i] = "2"; i++;
	newaction.argc = i;
	Create("xaxis","y_axis",graph,&newaction,0);


/* Creating the graph title */
	i = 0;
	newargv[i] = "xshape"; i++;
	newargv[i] = "title"; i++;
	newargv[i] = "-tx"; i++;
	newargv[i] = "0.5"; i++;
	newargv[i] = "-ty"; i++;
	newargv[i] = "1.1"; i++;
	newargv[i] = "-text"; i++;
	newargv[i] = graph->name; i++;
	newaction.argc = i;
	Create("xshape","title",graph,&newaction,0);

	graph->xaxis_w = (char *)XtNameToWidget((Widget)(XoGetFramedChild(fW)),
		"xaxis");
	graph->yaxis_w = (char *)XtNameToWidget((Widget)(XoGetFramedChild(fW)),
		"yaxis");


	xoParseCreateArgs(graph,ac,av);
	XtMapWidget((Widget)fW);
	xoGetGeom(graph);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
    if (ac) {			/* need to set fields */
	/*
		if (strcmp(av[0],"xunits") == 0) {
			DirectSetElement(graph,"XUnits",av[1]);
			gx_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
		if (strcmp(av[0],"yunits") == 0) {
			DirectSetElement(graph,"YUnits",av[1]);
			gx_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
	*/
		if (strcmp(av[0],"title") == 0) {
			Element *kid;
			if ((kid = GetChildElement(graph,"title",0))) {
				SetElement(kid,"text",av[1]);
				xoRedraw(graph);
			}
			return(0);
		}
		if (strcmp(av[0],"range") == 0) {
			float xmin,ymin,xmax,ymax;

			if (sscanf(av[1],"[%f,%f,%f,%f]",&xmin,&ymin,&xmax,&ymax)
				== 4) { /* The conversion worked */
				char	*tav[2];
				char	value[20];

				graph->xmin = xmin;
				graph->ymin = ymin;
				graph->xmax = xmax;
				graph->ymax = ymax;
				tav[1] = value;

				tav[0] = "xmin"; sprintf(value,"%g",xmin);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
				tav[0] = "ymin"; sprintf(value,"%g",ymin);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
				tav[0] = "xmax"; sprintf(value,"%g",xmax);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
				tav[0] = "ymax"; sprintf(value,"%g",ymax);
				gx_convert(graph,GXconvert,XtNumber(GXconvert),2,tav);
			}
			return(1);
		}

		/*
		Tue Jun  3 10:43:34 PDT 1997 - Venkat 

		Deal with the setting of the overlay field by creating the 
		nuetral called xoverlay, if necessary
		*/ 

		if (strcmp(av[0], "overlay") == 0) {
			int overlayset;
			sscanf(av[1], "%d", &overlayset);
			if(overlayset && !graph->overlay)
				xoCreateOverlay(graph);
			return 0;
		}
		gx_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(graph->widget)) return(0);
	XtDestroyWidget(XtParent((Widget)graph->widget));
	break;
  case XUPDATE : /* update graph fields due to changes in widget */
		if (ac > 0)
		xg_convert(graph,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(graph,action,graph->script,"");
	break;
  }
  return 0;
}



/* Utility function that returns the xoverlay neutral child if it exists,
   NULL otherwise */

static Element* xoGetOverlay(graph)
struct xgraph_type *graph;
{
	Element *child; 

	for (child = graph->child; child != NULL; child=child->next) {

		if(strcmp (BaseObject(child)->name, "neutral") == 0 &&
			strcmp(child->name, "xoverlay") == 0 ){
				return child;
		}
	}
	
	return NULL;
}

/* Utility function that creates the xoverlay neutral element as a child
   of the passed graph element parameter */

static void xoCreateOverlay(graphelm)
	Element *graphelm;
{
	Action	tempaction;
	char	*targs[MAX_NTARGS];


	struct xgraph_type * graph = (struct xgraph_type *) graphelm;

	tempaction.argc = 2;
	targs[0] = "neutral";
	targs[1] = "xoverlay";
	tempaction.argv = targs;
	tempaction.type = CREATE;
	tempaction.name = "CREATE";
	tempaction.data = (char *)graph;


	Create("neutral","xoverlay",graph,&tempaction,0);

}

Element *xoAcquireOverlay(graphelm)
 Element *graphelm;
{
 Element *overlay;

 if ((overlay = xoGetOverlay((struct xgraph_type *) graphelm)) == NULL)
	xoCreateOverlay(graphelm);	

 if (overlay == NULL)
	overlay = xoGetOverlay((struct xgraph_type *) graphelm);

 return overlay;
 
}

/*
	Tue Jun  3 11:02:17 PDT 1997 - Venkat 

		
	* Overlays are now name-encoded to associate a trial number 
	  with each addition/removal of the plot on the fly (without
	  resetting) as in Neurokit.  

	* In addition, they encode the overlay number of the graph, as before.

	* One other change is that there is now only a single "xoverlay" 
	  neutral element under which all the uniquely named copies of 
	  the plots are placed.

	E.g:

		Supposing the Ca channel concentration is plotted by 
		placing a recording electrode on a compartment, and the 
		simulation stepped, with the overlay flag on, for the graph.
		
		Now if the electrode is dropped using the GUI/otherwise without 
		resetting the simulation, the old plot remains on screen with
		the name:

			<plotname>#<trialnumber>.<overlaynumber>

		Both the trial and overlay numbers start at 0. 
		So the initial overlay name is:

			<plotname>#0.0

		On a subsequent addition & removal of the plot 
		(without resetting) the trial number is bumped one. The new
		name is:
	
			<plotname>#1.0 

		
		Now if a RESET is done on the simulation with the overlay 
		flag still on, subsequent plots will be named with the 
		trialnumber starting again at 0 but with the overlay number 
		bumped as before. In other words, the new names would be:

			<plotname>#0.1
			<plotname>#1.1 
				.
				.		
				.
			

		
	The following 2 local functions, 

		xoStripToOverlayname() 
		xoGetOverlayTrialNumber() 

	implement the above strategy.
*/


/* 
 This function takes the entire name (including the trial
  and the overlay numbers) as an argument and returns only the 
  <plotname>.<overlaynumber> part for comparison. The memory for the 
  returned string is allocated on the heap here and freed at the calling 
  function, xoGetOverlayTrialNumber()

  The trialnumber argument just serves as a int placeholder for 
  the trialnumber part of the name 
	<plotname>#<trialnumber>.<overlaynumber> 
*/

static char * xoStripToOverlayName(overlaytrialname, trialnumber)
 char *overlaytrialname;
 int *trialnumber;
{
	char *overlayname;
	char *ptr;
	
	if ((overlaytrialname == NULL) ||
		(trialnumber == NULL))
		return NULL;
	
	*trialnumber = 0;
	overlayname = NULL;

	if ((ptr = index(overlaytrialname, '#')) != NULL) {
		char plotpart[100];
		char extension[50];
		int plotnamelen;
		
		plotnamelen = (int) (ptr - overlaytrialname);
		strncpy(plotpart, overlaytrialname, plotnamelen); 
		plotpart[plotnamelen] = '\0';
		strcpy(extension, ptr);
		if ((ptr = index(extension, '.')) != NULL) {
			strcat(plotpart, ptr);
			*ptr = '\0';
			if ((extension + 1) != NULL) 
				sscanf(extension + 1, "%d", trialnumber);
		}
		overlayname = CopyString(plotpart);
	}
	return overlayname;
}

/*
	
Takes the neutral xoverlay element, the plotname part alone (in 
<plotname>#<trialnum>.<overlaynum>) and the graph->overlay_no as 
as arguments. Uses them to compare with the existing child plot
copies under the overlay element to determine and return a unique
trialnumber or rolled back to 0 if the graph->overlay_no itself 
got bumped in the xoHandleOverlays() code

*/

static int xoGetOverlayTrialNumber(overlay, plotname, overlaynumber)
 Element *overlay;
 char *plotname;
 int overlaynumber;
{
	Element *child;
	Element *graphparent;

	char overlayname[512];
	int overlaytrialnumber;

	if (overlay == NULL || plotname == NULL || 
		(graphparent = overlay->parent) == NULL)
		return -1;

	if (!((strcmp(BaseObject(overlay)->name, "neutral")==0) &&
		(strcmp(overlay->name, "xoverlay")==0)))
		return 0;
	
	sprintf(overlayname, "%s.%d", plotname, overlaynumber);
	overlaytrialnumber = 0;
	
	for (child=overlay->child; child != NULL; child=child->next) {
		char *childoverlayname = (char*) xoStripToOverlayName(child->name, &overlaytrialnumber); 
		if (childoverlayname != NULL && strcmp (overlayname, childoverlayname) == 0) {
			overlaytrialnumber++;
		} else {
			overlaytrialnumber = 0;
		}
		FreeString(childoverlayname);
	}
	
	return overlaytrialnumber;
}

/* Utility function that returns 1 if copy is successful, 0 if not */

static int xoCopyToOverlay(overlay, plot)
 Element *overlay;
 Element *plot;
{
	Element *newchild;
	Element *parent;
	struct xgraph_type *graphparent;
	char	str[200];

	/* Error check and initilize the parent local variables */
	parent = plot->parent;
	if (parent==NULL || strcmp (BaseObject(parent)->name, "xgraph") != 0)
		return 0;
	graphparent = (struct xgraph_type *) parent;

	/*

	Tue Jun  3 12:15:16 PDT 1997 - Venkat
	This avoids the unnecessary generation of a unique 
	overlayname if the plot hasnt been processed after the
	reset/creation.
	
	*/
	
	if (! ((struct xplot_type *) plot)->processed )
		return 1;


	/* We use a very restricted copy arrangement here,
		** to avoid duplicating further elements that may have
		** been put on top of the plots 
	*/


	newchild = CopyElement(plot);
	if (newchild != NULL) {
		int overlaytrialnumber;
		
		overlaytrialnumber = xoGetOverlayTrialNumber(overlay, newchild->name, graphparent->overlay_no);
		sprintf(str, "%s#%d.%d", newchild->name, overlaytrialnumber, 
					graphparent->overlay_no);
		newchild->name = CopyString(str);
		Attach(overlay,newchild);
		CopyAction(plot,newchild);
	} else {
		return 0;
	}
	return 1;
}

/* This function gets rid of old overlays */
 
static int xoZapOverlays(graph)
    struct xgraph_type *graph;
{
	Element	*child,*nextchild;
	/* Element	**current; */
	int		ret = 0;

	for(child=graph->child; child;child=nextchild) {
		nextchild = child->next;
		if (strcmp(BaseObject(child)->name,"neutral") == 0 &&
			strcmp(child->name,"xoverlay") == 0) {
			DeleteElement(child);
			ret = 1;
		} 
	}
	return(ret);
}

/* This should largely go into xplot.c, and we will add a new
** function, CREATEWITHMSG  to handle this situation
*/
static  int xoMakeNewPlot(graph,msg)
    struct xgraph_type *graph;
	MsgIn	*msg;
{
	Element	*child = NULL;
    /* struct xplot_type *pchild; */
	char	*targs[MAX_NTARGS];
	Action	tempaction;
	char	temp0[512];
	int 	plotno;
	static char	name[512];
	char	*ptr;
	char	*GetStringMsgData();

	if (!(msg->type == 0 || msg->type == 1 || msg->type == 2 ||
		msg->type == 3))
			return(0); /* assorted legal xplot msgs */

	/* find the number of plots on the graph */
	/* Later I will write a version of Create which automatically
	** assigns a unique index */
		
	strcpy(name,GetStringMsgData(msg,1));
	if ((ptr = strchr(name,'[')))
		*ptr = '\0';

	/* if you really want to get this right, you should go
	** directly to the plots */
	if (msg->type == 2 || msg->type == 3) { /* X or WAVEPLOT msg */
		for(child = graph->child; child ; child = child->next) {
			if (strcmp(child->name,name) == 0) {
				/* I worry that maybe there will be double freeing
				** of the msg slots when deleted */
				  AddForwMsg(child,msg->src,msg->type,MSGSLOT(msg),msg->nslots);
				break;
			}
		}
	}
	if (msg->type == 2 && !child) {
		Warning();
		printf("X msg to %s invalid: no plot present named %s\n",
			name,name);
		return 0;
	}
	if (msg->type == 0 || msg->type == 1 || (msg->type == 3 && !child)){
	plotno = xoNewIndex(graph,name);
	tempaction.argc = 2;
	targs[0] = "xplot";
	sprintf(temp0,"%s",name); /* note that the name does not
							 ** include the index part */
	targs[1] = temp0;
	tempaction.argv = targs;
	tempaction.type = CREATE;
	tempaction.name = "CREATE";
	tempaction.data = (char *)graph;
	child=Create("xplot",name,graph,&tempaction,plotno);
	if (child == NULL)
	    return(0);
	AssignElementClock(child, Clock(graph));
	/* I worry that maybe there will be double freeing
	** of the msg slots when deleted */
	 AddForwMsg(child,msg->src,msg->type,MSGSLOT(msg),msg->nslots);
	}
	/* This is a bit of a hack. The idea is to make the new plot
	** immediately accessible for drawing, which means that it has
	** to be added to the schedule table. It has already been reset
	** directly, so this should be OK */
	Reschedule();
	return(1);
}

static void xoUpdateChildClocks(graph, clock)
    struct xgraph_type *graph;
    int clock;
{
	Element *child;

	for (child = graph->child; child != NULL; child = child->next)
		AssignElementClock(child, clock);
}

static int xoCountPlots(graph)
    struct xgraph_type *graph;
{
	Element	*child;
	int		ret = 0;

	for(child=graph->child; child;child=child->next) {
		if (strcmp(BaseObject(child)->name, "xplot") == 0)
		ret++;
	}
	return(ret);
}

int xoUpdatePlotLabels(plot)
	Element *plot;
{
    Element *graph;
	Element	*child;
	Element	*overlaychild;
	int		i = 0;
	Widget	w;
	char temp[20];
	Dimension ht;
	/* int ht = ((GraphWidget)(graph->widget))->core.height; */
	/*float dht = XoFontHt */
	float dht;

	for(graph = plot->parent; graph; graph = graph->parent)
		if (strcmp(BaseObject(graph)->name, "xgraph") == 0)
			break;
	if (graph == NULL) return(-1);

	XtVaGetValues((Widget)((struct xgraph_type *)graph)->widget,
		XtNheight,&ht,
		NULL);
	dht = 1.0 *  /* screen ht in graph coords */
				20.0 /  /* ht of each letter, guessed */
				(float)ht;
	for(child=graph->child; child;child=child->next) {
		if (strcmp(BaseObject(child)->name, "xplot") == 0){
			w = (Widget)(((struct xplot_type *)child)->labelpix);
			if (w) {
				sprintf(temp,"%f",1.0 - dht * (float)i); 
				XoXtVaSetValues(w,
					XtVaTypedArg,XtNpixY,XtRString,temp,1+strlen(temp),
					NULL);
				i++;
			}
		} else if (strcmp(BaseObject(child)->name,"neutral") == 0) {
			for(overlaychild=child->child; overlaychild;
				overlaychild=overlaychild->next) {
				if (strcmp(BaseObject(overlaychild)->name,"xplot") == 0) {
					w =(Widget)
						(((struct xplot_type *)overlaychild)->labelpix);
					if (w) {
						sprintf(temp,"%f",1.0 - dht * (float)i); 
						XoXtVaSetValues(w,
							XtVaTypedArg,XtNpixY,XtRString,
							temp,1+strlen(temp),
							NULL);
						i++;
					}
				}
			}
		}
	}
	return(i);
}

static int xoNewIndex(graph,name)
    struct xgraph_type *graph;
	char	*name;
{
	Element *child;
	int nnames = 0;
	int i,j;
	int	childindex[500];

	for(child = graph->child; child; child = child->next) {
		if (strcmp(child->name,name) == 0) {
			childindex[nnames] = child->index;
			nnames++;
		} 

	}
	/* look for an unused index, starting at nnames to keep it
	** simple */
	for(i = nnames; ; i++) {
		for(j = 0; j < nnames; j++) {
			if (childindex[j] == i) break;
		}
		if (j >= nnames) return(i);
	}
}

int xo_graph_update_axis(elm,axis,labmin,labmax)
	Element *elm;
	int	axis;
	float labmin,labmax;
{
	Element *parent;
	char *tav[5];
	char val1[20];
	char val2[20];

	tav[1] = val1;
	tav[3] = val2;

	if (axis != 1 && axis != 2)
		return(0);
	for (parent = elm->parent; parent; parent = parent->parent) {
		if (strcmp(BaseObject(parent)->name,"xgraph") == 0) {
			if (axis == 1) {
				((struct xgraph_type *)parent)->xmin = labmin;
				((struct xgraph_type *)parent)->xmax = labmax;
				tav[0] = "xmin"; sprintf(tav[1],"%g",labmin);
				tav[2] = "xmax"; sprintf(tav[3],"%g",labmax);
				gx_convert(parent,GXconvert,XtNumber(GXconvert),4,tav);
			}
			if (axis == 2) {
				((struct xgraph_type *)parent)->ymin = labmin;
				((struct xgraph_type *)parent)->ymax = labmax;
				tav[0] = "ymin"; sprintf(tav[1],"%g",labmin);
				tav[2] = "ymax"; sprintf(tav[3],"%g",labmax);
				gx_convert(parent,GXconvert,XtNumber(GXconvert),4,tav);
			}
			/*
			XoUpdatePlotAxes(
				(Widget)((struct xgraph_type *)parent)->widget,1,1);
			ResizeDraws((Widget)((struct xgraph_type *)parent)->widget);
			*/
			break;
		}
	}
	return(0);
}


#undef MAX_NTARGS /* 20 */
#endif
