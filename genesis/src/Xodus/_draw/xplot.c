/* $Id: xplot.c,v 1.3 2005/07/20 20:01:57 svitak Exp $ */
/*
 * $Log: xplot.c,v $
 * Revision 1.3  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:59  svitak
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
 * Revision 1.27  2000/06/12 04:20:27  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.26  1998/01/14 22:02:24  venkat
 * Implemented utility function xoAddPtsToPlot() which adds a pair
 * of points at a time. Must be enhanced/replaced with a more efficient
 * routine that deals with any number of points.
 *
 * Revision 1.25  1997/07/25 01:00:51  venkat
 * Initialized local variable 'k' in XPlot() to silence warnings on
 * certain compilers
 *
 * Revision 1.24  1997/07/18  23:55:56  dhb
 * Updated ShowInterpol() calls to add dimension arg
 *
 * Revision 1.23  1997/06/03 19:37:25  venkat
 * The processed field is set and reset in the PROCESS and RESET actions.
 * This is used in the graph code to determine if overlaying is at all
 * necessary.
 *
 * Revision 1.22  1996/08/02  21:50:45  venkat
 * Fixed bug in revised xoExpandPlot which was doing the reallocation and initialization
 * on the interpols conditionally. This routine just expands the interpols and the
 * decision on whether the expansion is to be done or not is to be taken at the call site.
 *
 * Revision 1.21  1996/07/02  18:07:10  venkat
 * Cleaned up xoExpandPlot() to do an initialization of any extra interpol elems
 * after a realloc call
 *
 * Revision 1.20  1996/06/06  21:55:49  venkat
 * DELETE action modified to set the npts=0 and to set its widget
 * counterpart resource to the same value. Was causing FMR and ABR errors
 * in the Project method and core dumps on the DEC/alpha and Linux.
 *
 * Revision 1.19  1995/12/06  23:44:07  dhb
 * Merged in 1.17.1.1 changes.
 *
<<<<<<< 1.18
 * Revision 1.18  1995/09/26  22:11:20  dhb
 * Fix from Upi Bhalla to correctly expand plots when they exceed
 * 200 data points.
 *
=======
 * Revision 1.17.1.1  1995/12/06  23:39:30  dhb
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
>>>>>>> 1.17.1.1
 * Revision 1.17  1995/06/26  23:31:52  dhb
 * Merged in 1.16.1.1
 *
 * Fixed Log messages
 * */

#include <math.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Plot.h"
#include "Draw/Shape.h"
#include "Draw/Graph.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"

#ifndef MAX_NTARGS
#define MAX_NTARGS 20
#endif
#define AUTOYSQUISH 0x10
#define AUTOXSQUISH 0x20
#define ROLL_INCREMENT 0.75


static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"wx",		XtNwx},
	{"wy",		XtNwy},
	{"xmin",	XtNxmin},
	{"xmax",	XtNxmax},
	{"ymin",	XtNymin},
	{"ymax",	XtNymax},
	{"fg",		XtNforeground},
	{"npts",			XtNnpts},
	{"linewidth",		XtNlinewidth},
	{"linestyle",		XtNlinestyle},
};

static void xoHandleMemory();


void xoExpandPlot(plot)
	struct	xplot_type *plot;
{
  Interpol *ip;
  int newallocation;

 /** The decision as to whether the plot->table has to be expanded with points is 
	done at the calling routine, not here. This is a utility function that 
	expands the interpol table by XO_PLOT_PTS_INCREMENT, and it also takes the 
	responsibility of setting the allocated_pts field. It also does an additional
  	task of setting the widget resources but this is not good design. But 
	this function is called from a number of places and changing the code 
	to do the resource settings at the call sites would be a pain.**/

	newallocation = plot->npts+XO_PLOT_PTS_INCREMENT;

	/** Reallocate the interpols to include the min & max of the range - So the 
		additional +1. Then zero out any additional table elements as realloc
		wont do it and then set the xdivs field of the interpol. **/

	((Interpol *)(plot->xpts))->table =
		(double *)realloc((char *)((Interpol *)(plot->xpts))->table,
		(newallocation+1) * sizeof(double));
	ip = (Interpol *)plot->xpts;
	xomemzero((char*) &ip->table[plot->allocated_pts+1], sizeof(double) * ((newallocation+1) - (plot->allocated_pts+1))); 
	((Interpol *)(plot->xpts))->xdivs= newallocation;

	((Interpol *)(plot->ypts))->table =
		(double *)realloc((char *)((Interpol *)(plot->ypts))->table,
		(newallocation+1) * sizeof(double));
	ip = (Interpol *)plot->ypts;
	xomemzero((char*) &ip->table[plot->allocated_pts+1], sizeof(double) * ((newallocation+1) - (plot->allocated_pts+1))); 
	((Interpol *)(plot->ypts))->xdivs= newallocation;

	/** Set the allocated_pts field **/

	plot->allocated_pts = plot->npts + XO_PLOT_PTS_INCREMENT;

	/* copying over the table pointers i.e Set the resources*/

	XoXtVaSetValues((Widget)plot->widget,
		XtNxpts,(XtPointer)((Interpol *)plot->xpts)->table,
		XtNypts,(XtPointer)((Interpol *)plot->ypts)->table,
		XtNnpts,plot->npts,
		NULL);
}

int XPlot (plot, action)
     register struct xplot_type *plot;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW, xoFindParentDraw();
    struct xplot_type *copy_plot;
	PlotObject sW;
	int		i=0,k=0;
	char    *targs[MAX_NTARGS];
	Interpol	*create_interpol(), *ip;
	MsgIn	*msg;
	float	y;
	char	*labelname;
	char	*GetStringMsgData();
	double	dt;
  
  if (Debug(0) > 1)
    ActionHeader("XPlot", plot, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	/* PLOT, XYPLOT msgs */
	i = 0;
	dt = Clockrate(plot);
	MSGLOOP(plot,msg) {
		case 0: /* PLOT */
			/* Deal with various memory handling schemes here */
			if (plot->npts >= plot->allocated_pts)
				xoHandleMemory(plot);
			if (plot->do_slope) {
				y=MSGVALUE(msg,0);
				plot->ypts->table[plot->npts] = (y - plot->input)/dt;
				plot->input = y;
			} else {
				plot->input = plot->ypts->table[plot->npts] = 
					MSGVALUE(msg,0);
			}
			if (plot->notxpts) {
				plot->xpts->table[plot->npts]=simulation_time;
				plot->npts++;
				XoAddPtsToPlot(plot->widget,plot->npts);
			}
			break;
		case 1: /* PLOTSCALE: obsolete to my knowledge, but lets do it*/
			if (plot->npts >= plot->allocated_pts)
				xoHandleMemory(plot);
			if (plot->do_slope) {
				y = MSGVALUE(msg,0) * MSGVALUE(msg,3) + MSGVALUE(msg,4);
				plot->ypts->table[plot->npts] = (y - plot->input)/dt;
				plot->input = y;
			} else {
				plot->input = plot->ypts->table[plot->npts] = 
					MSGVALUE(msg,0) * MSGVALUE(msg,3) + MSGVALUE(msg,4);
			}
			if (plot->notxpts) {
				plot->xpts->table[plot->npts]=simulation_time;
				plot->npts++;
				XoAddPtsToPlot(plot->widget,plot->npts);
			}
			break;
		/* This is a dangerous case. It needs to be placed in 
		** its own MSGLOOP to be sure that it is after the ypts,
		** but then there is a problem with speed. We should
		** instead do a CHECK on reset, or something similar
		** to be sure that all conditions are satisfied */
		case 2: /* X : provides an x for the y above, which is always
				evaluated first */
			/* A safety precaution here */
			if (plot->notxpts) break;
			plot->xpts->table[plot->npts] = MSGVALUE(msg,0);
			plot->npts++;
			XoAddPtsToPlot(plot->widget,plot->npts);
			break;
		case 3: /* WAVEPLOT : Change all graph points simultaneously */
			plot->ypts->table[i] = MSGVALUE(msg,0);
			i++;
			if (i == plot->npts) 
				XoUpdateWavePlot((Widget)plot->widget);
			break;
	}
	plot->processed = 1;
    break;
  case ADDMSGIN:
  	/* The message in question is passed in as action->data */
  	msg = (MsgIn *)(action->data);
	if (strcmp(BaseObject(plot->parent)->name,"xgraph") == 0) {
		plot->xmin = ((struct xgraph_type *)plot->parent)->xmin;
		plot->xmax = ((struct xgraph_type *)plot->parent)->xmax;
		plot->ymin = ((struct xgraph_type *)plot->parent)->ymin;
		plot->ymax = ((struct xgraph_type *)plot->parent)->ymax;
	}
	switch (msg->type) {
		case 0: /* PLOT */
			y = 0;
		case 1: /* PLOTSCALE */
			k++;
			plot->npts = 0;
			plot->compressed_pts = 0;
			targs[0] = "npts";
			targs[1] = "0";
			gx_convert(plot,GXconvert,XtNumber(GXconvert),
				2,targs);
			if (plot->fg) {
				if (strcmp(plot->fg,GetStringMsgData(msg,2)) != 0)
				free(plot->fg);
			}
			plot->fg = CopyString(GetStringMsgData(msg,2));
			if (plot->labelpix != NULL) /* we have a labelpix */
				XoXtVaSetValues((Widget)(plot->labelpix),
					XtVaTypedArg,XtNtextcolor,XtRString,
					plot->fg,1+strlen(plot->fg),
					NULL);
			targs[0] = "fg";
			targs[1] = plot->fg;
			gx_convert(plot,GXconvert,XtNumber(GXconvert),
				2,targs);
			break;
		case 2: /* X  using both x */
			plot->notxpts = 0;
			break;
		case 3: /* WAVEPLOT : Change all graph points simultaneously */
			i++;
			/* only the first msg is relevant for fg */
			if (i > 1) break; 
			if (plot->fg) {
				if (strcmp(plot->fg,GetStringMsgData(msg,2)) != 0)
				free(plot->fg);
			}
			plot->fg = CopyString(GetStringMsgData(msg,2));
			if (plot->labelpix != NULL) /* we have a labelpix */
				XoXtVaSetValues((Widget)(plot->labelpix),
					XtVaTypedArg,XtNtextcolor,XtRString,
					plot->fg,1+strlen(plot->fg),
					NULL);
			targs[0] = "fg";
			targs[1] = plot->fg;
			gx_convert(plot,GXconvert,XtNumber(GXconvert),
				2,targs);
			break;
	}
  	break;

  case RESET:
	y = 1; /* we use this to test if we had msg type 0 */
	i = 0;
	k = 0; /* we use this to test if there is a PLOT or PLOTSCALE */
	plot->notxpts = 1;
	MSGLOOP(plot,msg) {
		case 0: /* PLOT */
			plot->input = MSGVALUE(msg,0);
			y = 0;
		case 1: /* PLOTSCALE */
			k++;
			if (y > 0.5)
				plot->input =
					MSGVALUE(msg,0) * MSGVALUE(msg,3) + MSGVALUE(msg,4);
			plot->npts = 0;
			plot->compressed_pts = 0;
			targs[0] = "npts";
			targs[1] = "0";
			gx_convert(plot,GXconvert,XtNumber(GXconvert),
				2,targs);
			break;
		case 2: /* X  using both x */
			plot->notxpts = 0;
			break;
		case 3: /* WAVEPLOT : Change all graph points simultaneously */
			i++;
			/* only the first msg is relevant for fg */
			if (i > 1) break; 
			break;
	}
	if (k > 1) {
		Warning();
		printf("Can only have one incoming plot msg in xplot\n");
	}
	if (plot->notxpts == 0 && k == 0) {
		Warning();
		printf("X msg must have a corresponding PLOT or PLOTSCALE\n");
		plot->notxpts = 1;
	}
	if (i > 0) { /* it was a waveplot */
		while (plot->allocated_pts < i) /* most unlikely */
			xoExpandPlot(plot);
		plot->npts = i;
		plot->xmax = i;
		for(i = 0;i<plot->npts;i++)
			plot->xpts->table[i] = i;
		targs[0] = "npts";
		targs[1] = itoa(i);
		targs[2] = "xmax";
		targs[3] = itoa(i);
		gx_convert(plot,GXconvert,XtNumber(GXconvert),
			4,targs);
    	if ((parentW = xoFindParentDraw(plot)) == NULL) return(0);
		XoUpdatePlotAxes(parentW,1,1);
	}
	plot->processed = 0;
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */
    if ((parentW = xoFindParentDraw(plot)) == NULL) return(0);

	plot->xpts = create_interpol(XO_PLOT_PTS_INCREMENT,(float)0.0,(float)1.0);
	plot->ypts = create_interpol(XO_PLOT_PTS_INCREMENT,(float)0.0,(float)1.0);
	plot->allocated_pts = XO_PLOT_PTS_INCREMENT;

	plot->memory_mode = "expand";
	plot->labelpix = NULL;
	plot->memory_mode = "squish";
	plot->auto_mode = AUTOYSQUISH; 	/* autosquish */
	plot->linestyle = "LineSolid";
    
	sW = (PlotObject)XtCreateManagedWidget(
		av[1], plotObjectClass, parentW, NULL,0);
    plot->widget = (char *)sW;
	/* copying over the table pointers */
	XoXtVaSetValues((Widget)sW,
		XtNxpts,(XtPointer)((Interpol *)plot->xpts)->table,
		XtNypts,(XtPointer)((Interpol *)plot->ypts)->table,
		NULL);
	/* if (strcmp(parentO->name, "xgraph") == 0) */
	/* Set up the default axis limits */
	if (strcmp(BaseObject(plot->parent)->name,"xgraph") == 0) {
		plot->xmin = ((struct xgraph_type *)plot->parent)->xmin;
		plot->xmax = ((struct xgraph_type *)plot->parent)->xmax;
		plot->ymin = ((struct xgraph_type *)plot->parent)->ymin;
		plot->ymax = ((struct xgraph_type *)plot->parent)->ymax;
		plot->flags = ((struct xgraph_type *)plot->parent)->flags;
		plot->wx = 1.0;
		plot->wy = 1.0;
	}
	if (XtIsSubclass(parentW,graphWidgetClass)) {
		if (ac > 2 && 
			strcmp(av[2],"-value") ==  0 &&
			(int)strlen(av[3]) > 0 &&
			strcmp(plot->name,"plot") == 0)
			labelname = av[3];
		else
			labelname = plot->name;
		/* Make the plot label which also handles actions for plot */
		plot->labelpix = (char *)XtCreateManagedWidget(labelname,
			shapeObjectClass,parentW,NULL,0);
		XoXtVaSetValues((Widget)(plot->labelpix),
			XtVaTypedArg,XtNpixX,XtRString,"1.05",5,
			XtVaTypedArg,XtNpixY,XtRString,"1.0",4,
			XtNtext,labelname,
			NULL);
		XtAddCallback((Widget)(plot->labelpix),XtNcallback,xoCallbackFn,
			(XtPointer)plot);
	} else {
		XtAddCallback((Widget)(plot->widget), XtNcallback,xoCallbackFn,
			(XtPointer)plot);
	}
	XoUpdatePlotAxes(parentW,1,1);
	xoUpdatePlotLabels(plot);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(plot,ac,av);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(plot,ac,av)) return(1); 
    if (ac) {			/* need to set fields */
		/* Special case : setting the interpol arrays. */
		if (SetTable(plot,ac,av,action->data,"xpts ypts")) {
			if (plot->npts > plot->xpts->xdivs + 1)
				plot->npts = plot->xpts->xdivs + 1;
			if (plot->npts > plot->ypts->xdivs + 1)
				plot->npts = plot->ypts->xdivs + 1;

			/*
			plot->npts = (plot->xpts->xdivs < plot->ypts->xdivs) ?
				plot->xpts->xdivs + 1 : plot->ypts->xdivs + 1;
			*/
			XoXtVaSetValues((Widget)plot->widget,
				XtNxpts,(XtPointer)((Interpol *)plot->xpts)->table,
				XtNypts,(XtPointer)((Interpol *)plot->ypts)->table,
				XtNnpts,plot->npts,
				NULL);
			return(1);
		}
		if (strncmp(av[0]+1,"pts->table",10) == 0) {
		/* need to set points and update */
			DirectSetElement(plot,av[0],av[1]);
			XoProjectAndDrawPix((PixObject)plot->widget,1);
			return(1);
		}
		/* Need to reallocate storage for tables */
		if (strcmp(av[0],"npts") == 0) {
			i=atoi(av[1]);
			if (i < 0) {
				printf("error in %s: npts (%d) must be >= 0\n",
					Pathname(plot),i);
				return(1); /* Do nothing - bad value */
			}
			plot->npts=i;
			if (i > ((Interpol *)(plot->xpts))->xdivs) {
				xoExpandPlot(plot);
				return(1);
			}
			/* if a more usual change to npts, update as usual */
			gx_convert(plot,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
		if (strcmp(av[0],"fg") == 0) {
			if (plot->labelpix != NULL) /* we have a labelpix */
				XoXtVaSetValues((Widget)(plot->labelpix),
					XtVaTypedArg,XtNtextcolor,XtRString,
					av[1],1+strlen(av[1]),
					NULL);
		}
		/* If the user sets the ysquish then it ceases to be automatic*/
		if (strcmp(av[0],"ysquish") == 0) {
			plot->auto_mode = 0;
		}
		gx_convert(plot,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(plot->widget)) return(0);
	plot->npts=0;
	XoXtVaSetValues((Widget)plot->widget, XtNnpts, plot->npts, NULL);
	XtDestroyWidget((Widget)plot->widget);
	if (plot->labelpix)
		XtDestroyWidget((Widget)plot->labelpix);
	ip = (Interpol *)plot->xpts;
	if (ip->allocated) free(ip->table); free(ip);
	ip = (Interpol *)plot->ypts;
	if (ip->allocated) free(ip->table); free(ip);
	break;
  case COPY:
	copy_plot = (struct xplot_type *)action->data;
	if ((parentW = xoFindParentDraw(copy_plot)) == NULL) return(0);
	/* Duplicate the interpols */
	one_tab_dup_func(&(copy_plot->xpts));
	one_tab_dup_func(&(copy_plot->ypts));
	/* Create a new Widget */
	sW = (PlotObject)XtCreateManagedWidget(
		copy_plot->name, plotObjectClass, parentW, NULL,0);
	copy_plot->widget = (char *)sW;
	/* copy over the table pointers and pixflags */
	XoXtVaSetValues((Widget)sW,
		 XtNxpts,(XtPointer)((Interpol *)copy_plot->xpts)->table,
		 XtNypts,(XtPointer)((Interpol *)copy_plot->ypts)->table,
		 XtNpixflags,copy_plot->pixflags,
		 NULL);
	if (XtIsSubclass(parentW,graphWidgetClass)) {
		/* Make the plot label which also handles actions for plot */
		copy_plot->labelpix =
			(char *)XtCreateManagedWidget(copy_plot->name,
			shapeObjectClass,parentW,NULL,0);
		XoXtVaSetValues((Widget)(copy_plot->labelpix),
			XtVaTypedArg,XtNpixX,XtRString,"1.05",5,
			XtVaTypedArg,XtNpixY,XtRString,"1.0",4,
			XtVaTypedArg,XtNtextcolor,XtRString,
				copy_plot->fg,1+strlen(copy_plot->fg),
			XtNtext,copy_plot->name,
			NULL);
		XtAddCallback((Widget)(copy_plot->labelpix),
			XtNcallback,xoCallbackFn,
			(XtPointer)copy_plot);
	} else {
		XtAddCallback((Widget)(copy_plot->widget), XtNcallback,xoCallbackFn,
			(XtPointer)copy_plot);
	}
	XoUpdatePlotAxes(parentW,1,1);
	xoUpdatePlotLabels(copy_plot);
	gx_convert_all(copy_plot,GXconvert,XtNumber(GXconvert));
	return(1);
	/* NOTREACHED */
	break;
  case ADDPTS:
  	/* I guess something like the argopts stuff should happen here */
  	/* Arguments: numbers representing points to be added. # of args
  	** should be multiples of two, as they are taken in pairs.
  	** This routine is inefficient and is not meant to be used
  	** with large numbers of coords */
  	if (ac >= 2) {
  		int i;
  		for (i = 0; i < (ac - 1) ; i++) {
			if (plot->npts >= plot->allocated_pts)
				xoHandleMemory(plot);
			plot->xpts->table[plot->npts] = Atof(av[i]);
			i++;
			plot->input = plot->ypts->table[plot->npts] = Atof(av[i]);
			plot->npts++;
			XoAddPtsToPlot(plot->widget,plot->npts);
		}
  	}
  	break;
  case SHOW:
		if (ShowInterpol(action,"xpts",plot->xpts,1,200)) return(1);
		return(ShowInterpol(action,"ypts",plot->ypts,1,200));
	/* NOTREACHED */
  break;
  case DUMP:
    if (ac == 1) {
        if (strcmp(av[0],"pre") == 0) {
            return(0);
        }
        if (strcmp(av[0],"post") == 0) {
            FILE *fp = (FILE *)action->data ;
            DumpInterpol(fp,plot,plot->xpts,"xpts");
            DumpInterpol(fp,plot,plot->ypts,"ypts");
            return(0);
        }
    }
  break;
  case XUPDATE : /* update plot fields due to changes in widget */
		xg_convert(plot,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(plot,action,plot->script,plot->value);
	break;
  }
  return 0;
}

void xoSetNewNpts(plot)
	struct	xplot_type *plot;
{
	XoXtVaSetValues((Widget)(plot->widget),
		XtNnpts,plot->npts,
		NULL);
}

static void xoHandleMemory(plot)
     struct xplot_type *plot;
{
	int		i,k;
	int		lasti;
	float	y;

		switch (plot->memory_mode[0]) {
			case 'l':	/* limit */
				if (plot->npts > 0)
					plot->npts--;
				return;
				/* NOTREACHED */
				break;
			case 'r':	/* roll - like stripchart */
				for(k=0,i=plot->npts * ROLL_INCREMENT;
					i<plot->npts;i++,k++) {
					plot->ypts->table[k] = plot->ypts->table[i];
					plot->xpts->table[k] = plot->xpts->table[i];
				}
				xoSetNewNpts(plot);
				break;
			case 'o':	/* oscilloscope */
				plot->npts = 0;
				xoSetNewNpts(plot);
				break;
			case 's':	/* squish: lossy compression */
				/* k represents pt being added to squish list*/
				/* y represents squish baseline */

				lasti = plot->compressed_pts;

				if (plot->auto_mode & AUTOYSQUISH) { /*autosquish on y*/
					double ymin,ymax;
					/* find range of plot values */
					ymin = ymax = plot->ypts->table[lasti];
					for(i=lasti + 1 ; i<plot->npts;i++) {
						if (ymin > plot->ypts->table[i])
							ymin = plot->ypts->table[i];
						if (ymax < plot->ypts->table[i])
							ymax = plot->ypts->table[i];
					}
					/* set ysquish to 100/range */
					plot->ysquish = fabs(ymax - ymin)/100;
				}

				y = plot->ypts->table[lasti];
				for(k=i=lasti+1; i<plot->npts;i++) {
					/* dy is larger than ysquish */
					if (fabs(y - plot->ypts->table[i]) > 
						plot->ysquish) {
						/* We need to store the previous 
						** point if dx is > 1 step */
						if (lasti < i - 1) {
							y = plot->ypts->table[k] =
								plot->ypts->table[i-1];
							plot->xpts->table[k] =
								plot->xpts->table[i-1];
							k++;
							lasti = i -1;
						}
					}
					/* check if dy is still > ysquish */
					if (fabs(y - plot->ypts->table[i]) > 
						plot->ysquish) {
						/* save this point too */
						y = plot->ypts->table[k] =
							plot->ypts->table[i];
						plot->xpts->table[k] =
							plot->xpts->table[i];
						k++;
						lasti = i;
					}
				}
				/* clean up final point */
				if (lasti != i-1) {
					plot->ypts->table[k] =
						plot->ypts->table[i-1];
					plot->xpts->table[k] =
						plot->xpts->table[i-1];
					k++;
				}
				plot->compressed_pts = plot->npts = k;
				if (plot->allocated_pts - plot->npts < 
					XO_PLOT_PTS_INCREMENT) {
					xoExpandPlot(plot);
				} else { /* just set the new value for npts */
					xoSetNewNpts(plot);
				}
				break;
			case 'h':	/* halve: halve the # of x axis pts */
				for(k=0,i=0; i<plot->npts;i+=2,k++) {
					plot->ypts->table[k] = plot->ypts->table[i];
					plot->xpts->table[k] = plot->xpts->table[i];
				}
				plot->npts = k;
				if (plot->allocated_pts - plot->npts < 
					XO_PLOT_PTS_INCREMENT) {
					xoExpandPlot(plot);
				} else { /* just set the new npts */
					xoSetNewNpts(plot);
				}
				break;
			case 'f':	/* file */
			case 'e':	/* expand */
			default:
				xoExpandPlot(plot);
			break;
		}
}

void xoAddPtsToPlot(plot, xval, yval)
	struct	xplot_type *plot;
	char *xval, *yval;
{
	if (plot->npts >= plot->allocated_pts)
		xoHandleMemory(plot);
	plot->xpts->table[plot->npts] = Atof(xval);
	plot->input = plot->ypts->table[plot->npts] = Atof(yval);
	plot->npts++;
	XoAddPtsToPlot(plot->widget,plot->npts);
}

#undef MAX_NTARGS
