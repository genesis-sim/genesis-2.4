/* $Id: xfastplot.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
** $Log: xfastplot.c,v $
** Revision 1.2  2005/07/01 10:02:59  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1  2005/06/17 21:23:51  svitak
** This file was relocated from a directory with the same name minus the
** leading underscore. This was done to allow comiling on case-insensitive
** file systems. Makefile was changed to reflect the relocations.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/18 22:39:35  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.3  2000/06/12 04:26:51  mhucka
** 1) Removed nested comments in RCS/CVS log lines, to quiet down the
**    IRIX cc compiler.
** 2) Added NOTREACHED comments where appropriate.
**
** Revision 1.2  1998/07/22 06:27:46  dhb
** Fixed ANSI C isms.
**
 * Revision 1.1  1998/07/14 20:20:50  dhb
 * Initial revision
 *
 * Revision 1.1  1998/06/02 05:34:53  bhalla
 * Initial revision
 * */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Widg/Form.h"
#include "Widg/FastPlot.h"
#include "widg_ext.h"
#include "_widg/xform_struct.h"
#include "Xo/Xo_ext.h"

static Gen2Xo GXconvert[] = {
	{"ht",			XtNht},
	{"wid",			XtNwid},
	{"lborder",		XtNlborder},
	{"rborder",		XtNlborder},
	{"tborder",		XtNtborder},
	{"bborder",		XtNbborder},
	{"npts",		XtNnpts},
	{"pts",			XtNpts},
	{"nnewpts",		XtNnnewpts},
	{"ringpts",		XtNringpts},
	{"overlay_flag",	XtNoverlay_flag},
	{"line_flag",	XtNline_flag},
	{"plotmode",	XtNplotmode},
	{"reset_flag",	XtNreset_flag},
	{"colorptr",	XtNcolorptr},
	{"sxval",		XtNxval},
	{"syval",		XtNyval},
	{"xaxis_flag",	XtNxaxis_flag},
	{"yaxis_flag",	XtNyaxis_flag},
	{"title",		XtNtitle},
	{"xtextmax",	XtNxtextmax},
	{"xtextmid",	XtNxtextmid},
	{"xtextmin",	XtNxtextmin},
	{"ytextmax",	XtNytextmax},
	{"ytextmid",	XtNytextmid},
	{"ytextmin",	XtNytextmin},
};


#ifndef MAX_NTARGS
#define MAX_NTARGS 30
#endif

#define F_NPLOTMSG 1
#define F_WAVEPLOTMSG 2
#define F_XPLOTMSG 4

#define F_ROLL 1
#define F_OSCOPE 2
#define F_RING 3
#define F_XYRING 4
#define F_WAVEPLOT 5
#define F_XYWAVEPLOT 6

extern double DoubleMessageData();
extern double ShortMessageData();


/* Returns 1 if trigger happens. Also returns index of trig point
** in 'start' */
int short_oscope_trig(plot, start)
struct xfastplot_type *plot;
int *start;
{
	int i;
	double t;

	*start = 0;

	if (plot->trig_flag) return 1;	/* already triggered */

	if (plot->trig_sign == 0) {		/* Auto_trig */
		plot->trig_flag = 1;
		if (!plot->overlay_flag)
			XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
		plot->ringpts = 0;
		return 1;
	}

	if (!plot->ac_trig_flag)		/* Not AC-coupled triggering */
		plot->ac_trig_buf = 0.0;

	for (i = 0; i < plot->nnewpts; i++) { /* Check for trig event */
		t = *((short *)(plot->yptr) + i + plot->ptindex);
	/* Multiplying both sides by the sign lets us check +ve 
	** and -ve triggers at the same time */
		if (plot->trig_sign * (t - plot->ac_trig_buf) >
			plot->trig_sign * plot->trig_thresh) {
				plot->trig_flag = 1;
				*start = i;
				if (!plot->overlay_flag)
					XoUndrawFastPlot((Widget)plot->widget,
						0, plot->npts);
				plot->ringpts = 0;
				return 1;
		}
		if (plot->ac_trig_flag)
			plot->ac_trig_buf = t;
	}

	return 0;	/* No trigger */
}

#ifdef __STDC__
int double_oscope_trig(struct xfastplot_type *plot, int *start) {
#else
int double_oscope_trig(plot, start)
struct xfastplot_type *plot;
int *start;
{
#endif
	int i;
	double t;

	*start = 0;

	if (plot->trig_flag) return 1;	/* already triggered */

	if (plot->trig_sign == 0) {		/* Auto_trig */
		plot->trig_flag = 1;
		if (!plot->overlay_flag)
			XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
		plot->ringpts = 0;
		return 1;
	}

	if (!plot->ac_trig_flag)		/* Not AC-coupled triggering */
		plot->ac_trig_buf = 0.0;

	for (i = 0; i < plot->nnewpts; i++) { /* Check for trig event */
		t = *((double *)(plot->yptr) + i + plot->ptindex);
	/* Multiplying both sides by the sign lets us check +ve 
	** and -ve triggers at the same time */
		if (plot->trig_sign * (t - plot->ac_trig_buf) >
			plot->trig_sign * plot->trig_thresh) {
				plot->trig_flag = 1;
				*start = i;
				if (!plot->overlay_flag)
					XoUndrawFastPlot((Widget)plot->widget,
						0, plot->npts);
				plot->ringpts = 0;
				return 1;
		}
		if (plot->ac_trig_flag)
			plot->ac_trig_buf = t;
	}

	return 0;	/* No trigger */
}

#ifdef __STDC__
static void short_plot(struct xfastplot_type *plot) {
#else
static void short_plot(plot)
struct xfastplot_type *plot;
{
#endif
	XPoint	*pts = (XPoint *)(plot->pts);
	double t;
	int start = 0;
	int i;
	int db = plot->ht - plot->bborder;
	short *yptr;

  	switch (plot->plotmode) {
		case F_XYWAVEPLOT: /* plot fixed no of XY points from short */
			if (!plot->overlay_flag)
				XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
			for (i = 0; i < plot->npts; i++) {
				t = (*((short *)(plot->xptr) + i)
					- plot->xoffset) * plot->xscale;
				pts[i].x =
		(t > plot->wid - plot->rborder) ? plot->wid - plot->rborder :
		((t < plot->lborder) ? plot->lborder : t);
			} /* Fall through to the PLOT case for the y points */
		case F_WAVEPLOT: /* plot fixed number of points from a short */
			if (!plot->overlay_flag && plot->plotmode != F_XYWAVEPLOT)
				XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
			for (i = 0; i < plot->npts; i++) {
				t = (*((short *)(plot->yptr) + i)
					- plot->yoffset) * plot->yscale;
				pts[i].y = (t > db) ? db :
				((t < plot->tborder) ? plot->tborder : t);
			}
			XoUpdateFastPlot((Widget)plot->widget, 0, plot->npts);
		break;
		case F_ROLL:/* Shift old points left, add new points to right */
			/* overlay is irrelevant here */
			XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
			for (i = plot->nnewpts; i < plot->npts; i++) {
				pts[i - plot->nnewpts].y = pts[i].y;
			}
			yptr = ((short *)(plot->yptr)) + plot->ptindex;
			for (i = 0; i < plot->nnewpts; i++) {
				t = (yptr[i] - plot->yoffset) * plot->yscale;
				pts[plot->npts - plot->nnewpts + i].y =
					(t > db) ? db :
					((t < plot->tborder) ? plot->tborder : t);
			}
			XoUpdateFastPlot((Widget)plot->widget, 0, plot->npts);
		break;
		case F_OSCOPE: /* Start on left, add new points to right */
		/* The oscope_trig function checks if a new trig has happened,
		** and wipes the old plot unless overlay_flag is on */
			if (short_oscope_trig(plot, &start)) {
				yptr = ((short *)(plot->yptr)) + plot->ptindex;
				for (i = start; i < plot->nnewpts; i++) {
					if (plot->ringpts + i - start >= plot->npts) {
						plot->ac_trig_buf = yptr[plot->npts - 1];
						plot->trig_flag = 0;
						break;
					}
					t = (yptr[i] - plot->yoffset) * plot->yscale;
					pts[plot->ringpts + i - start].y =
						(t > db) ? db :
						((t < plot->tborder) ? plot->tborder : t);
				}
				XoUpdateFastPlot((Widget)plot->widget, plot->ringpts,
					i - start);
				plot->ringpts += i - start;
			}
		break;
		case F_XYRING: /* Like RING in xy mode. Good for scatter plot */
			if (!plot->overlay_flag)
				XoUndrawFastPlot((Widget)plot->widget,
					plot->ringpts, plot->nnewpts);
			for (i = 0; i < plot->nnewpts; i++) {
				t = (*((short *)(plot->xptr) + i + plot->ptindex)
					- plot->xoffset) * plot->xscale;
				pts[(plot->ringpts + i) % plot->npts].x =
		(t > plot->wid - plot->rborder) ? plot->wid - plot->rborder :
		((t < plot->lborder) ? plot->lborder : t);
			} /* Fall through to the RING case for the y points */
		case F_RING:	/* Start on left, add new points to right,
					** go round ring updating points */
			if (!plot->overlay_flag && plot->plotmode != F_XYRING)
				XoUndrawFastPlot((Widget)plot->widget,
					plot->ringpts, plot->nnewpts);
			yptr = ((short *)(plot->yptr)) + plot->ptindex;
			for (i = 0; i < plot->nnewpts; i++) {
				t = (yptr[i] - plot->yoffset) * plot->yscale;
				pts[(plot->ringpts + i) % plot->npts].y =
					(t > db) ? db :
					((t < plot->tborder) ? plot->tborder : t);
			}
			XoUpdateFastPlot((Widget)plot->widget, plot->ringpts,
					plot->nnewpts);
			plot->ringpts =(plot->nnewpts + plot->ringpts) % plot->npts;
		break;
	}
}

#ifdef __STDC__
static void double_plot(struct xfastplot_type *plot) {
#else
static void double_plot(plot)
struct xfastplot_type *plot;
{
#endif
	XPoint	*pts = (XPoint *)(plot->pts);
	double t;
	int start = 0;
	int i;
	int db = plot->ht - plot->bborder;

  	switch (plot->plotmode) {
		case F_XYWAVEPLOT: /* plot fixed no of XY points from double */
			if (!plot->overlay_flag)
				XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
			for (i = 0; i < plot->npts; i++) {
				t = (*((double *)(plot->xptr) + i)
					- plot->xoffset) * plot->xscale;
				pts[i].x =
		(t > plot->wid - plot->rborder) ? plot->wid - plot->rborder :
		((t < plot->lborder) ? plot->lborder : t);
			} /* Fall through to the PLOT case for the y points */
		case F_WAVEPLOT: /* plot fixed number of points from a double */
			if (!plot->overlay_flag && plot->plotmode != F_XYWAVEPLOT)
				XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
			for (i = 0; i < plot->npts; i++) {
				t = (*((double *)(plot->yptr) + i + plot->ptindex)
					- plot->yoffset) * plot->yscale;
				pts[i].y =
					(t > db) ? db :
					((t < plot->tborder) ? plot->tborder : t);
			}
			XoUpdateFastPlot((Widget)plot->widget, 0, plot->npts);
		break;
		case F_ROLL:/* Shift old points left, add new points to right */
			/* overlay is irrelevant here */
			XoUndrawFastPlot((Widget)plot->widget, 0, plot->npts);
			for (i = plot->nnewpts; i < plot->npts; i++) {
				pts[i - plot->nnewpts].y = pts[i].y;
			}
			for (i = 0; i < plot->nnewpts; i++) {
				t = (*((double *)(plot->yptr) + i + plot->ptindex)
					- plot->yoffset) * plot->yscale;
				pts[plot->npts - plot->nnewpts + i].y =
					(t > db) ? db :
					((t < plot->tborder) ? plot->tborder : t);
			}
			XoUpdateFastPlot((Widget)plot->widget, 0, plot->npts);
		break;
		case F_OSCOPE: /* Start on left, add new points to right */
		/* The oscope_trig function checks if a new trig has happened,
		** and wipes the old plot unless overlay_flag is on */
			if (double_oscope_trig(plot, &start)) {
				for (i = start; i < plot->nnewpts; i++) {
					if (plot->ringpts + i - start >= plot->npts) {
						plot->ac_trig_buf = 
							*((double *)(plot->yptr) +
								plot->npts - 1 + plot->ptindex);
						plot->trig_flag = 0;
						break;
					}
					t = (*((double *)(plot->yptr) + i + plot->ptindex)
						- plot->yoffset) * plot->yscale;
					pts[plot->ringpts + i - start].y =
						(t > db) ? db :
						((t < plot->tborder) ? plot->tborder : t);
				}
				XoUpdateFastPlot((Widget)plot->widget, plot->ringpts,
					i - start);
				plot->ringpts += i - start;
			}
		break;
		case F_XYRING: /* Like RING in xy mode. Good for scatter plot */
			if (!plot->overlay_flag)
				XoUndrawFastPlot((Widget)plot->widget,
					plot->ringpts, plot->nnewpts);
			for (i = 0; i < plot->nnewpts; i++) {
				t = (*((double *)(plot->xptr) + i + plot->ptindex)
					- plot->xoffset) * plot->xscale;
				pts[(plot->ringpts + i) % plot->npts].x =
		(t > plot->wid - plot->rborder) ? plot->wid - plot->rborder :
		((t < plot->lborder) ? plot->lborder : t);
			} /* Fall through to the RING case for the y points */
		case F_RING:	/* Start on left, add new points to right,
					** go round ring updating points */
			if (!plot->overlay_flag && plot->plotmode != F_XYRING)
				XoUndrawFastPlot((Widget)plot->widget,
					plot->ringpts, plot->nnewpts);
			for (i = 0; i < plot->nnewpts; i++) {
				t = (*((double *)(plot->yptr) + i + plot->ptindex)
					- plot->yoffset) * plot->yscale;
				pts[(plot->ringpts + i) % plot->npts].y =
					(t > db) ? db :
					((t < plot->tborder) ? plot->tborder : t);
			}
			XoUpdateFastPlot((Widget)plot->widget, plot->ringpts,
					plot->nnewpts);
			plot->ringpts =(plot->nnewpts + plot->ringpts) % plot->npts;
		break;
	}
}

#ifdef __STDC__
static void change_npts(int npts, struct xfastplot_type *plot) {
#else
static void change_npts(npts, plot)
int npts;
struct xfastplot_type *plot;
{
#endif
	int i;

	if (npts < 1 || npts == plot->npts) return;

	if (npts > plot->alloced_npts) {
		free(plot->pts);
		plot->pts = (char *)calloc(npts, sizeof(XPoint));
		plot->alloced_npts = npts;
	}

	plot->npts = npts;
	for (i = 0; i < npts; i++)
		((XPoint *)plot->pts)[i].y = 0;
	XoXtVaSetValues((Widget)(plot->widget),
		XtNnpts, plot->npts, 
		XtNpts,(XtPointer)(plot->pts),
		NULL);
}

/* If any of the xmin, xmax, ymin, ymax, xval, yval are changed, 
** then a variety of things need to be recalculated and sent back
** to the widget */
#ifdef __STDC__
static void do_range(struct xfastplot_type *plot) {
#else
static void do_range(plot)
struct xfastplot_type *plot;
{
#endif
	double t;
	plot->xscale =
		(double)(plot->wid - plot->lborder - plot->rborder) /
		(plot->xmax - plot->xmin);
	plot->xoffset = plot->xmin - plot->lborder / plot->xscale;
	plot->yscale =
		(double)(plot->ht - plot->tborder - plot->bborder) /
		(plot->ymin - plot->ymax);
	plot->yoffset = plot->ymax - plot->tborder / plot->yscale;

	if (plot->xval > plot->xmax) plot->xval = plot->xmax;
	if (plot->xval < plot->xmin) plot->xval = plot->xmin;
	if (plot->yval > plot->ymax) plot->yval = plot->ymax;
	if (plot->yval < plot->ymin) plot->yval = plot->ymin;

	if (plot->reset_flag) {
		t = (plot->xval - plot->xoffset) * plot->xscale + 0.5;
		plot->sxval =
		(t > plot->wid - plot->rborder) ? plot->wid - plot->rborder :
			((t < plot->lborder) ? plot->lborder : t);
		t = (plot->yval - plot->yoffset) * plot->yscale + 0.5;
		plot->syval =
		(t > plot->ht - plot->bborder) ? plot->ht - plot->bborder :
			((t < plot->tborder) ? plot->tborder : t);
	}

	XoXtVaSetValues((Widget)(plot->widget),
		XtNxval, plot->sxval, 
		XtNyval, plot->syval, 
		NULL);
}

void setup_plotptrs(plot)
struct xfastplot_type *plot;
{
	MsgIn	*msg;
	int dblmsg = 0;
	int shortmsg = 0;

	plot->nnewpts_slot.func = NULL;
	plot->nptr = &(plot->nnewpts);
  	plot->pm = 0;
	plot->ptindex = 0;

  	MSGLOOP(plot,msg) {
		case 0:	/* NPLOT */
			plot->yptr = (short *)(MSGPTR(msg, 0));
			plot->nnewpts_slot.data = (char *)(MSGPTR(msg, 1));
			plot->nnewpts_slot.func = MSGSLOT(msg)[1].func;
			/* plot->nptr = (int *)(MSGPTR(msg, 1)); */

			plot->index_slot.data = (char *)(MSGPTR(msg, 2));
			plot->index_slot.func = MSGSLOT(msg)[2].func;

			if (MSGSLOT(msg)[0].func == DoubleMessageData)
				dblmsg |= F_NPLOTMSG;
			if (MSGSLOT(msg)[0].func == ShortMessageData)
				shortmsg |= F_NPLOTMSG;
			plot->pm |= F_NPLOTMSG;
		break;
		case 1: /* WAVEPLOT */
			plot->yptr = (short *)(MSGPTR(msg, 0));
			plot->nnewpts_slot.data = (char *)(MSGPTR(msg, 1));
			plot->nnewpts_slot.func = MSGSLOT(msg)[1].func;
			if (plot->nnewpts_slot.func)
				plot->nnewpts = (int)
					((plot->nnewpts_slot.func)(&plot->nnewpts_slot));
			change_npts(plot->nnewpts, plot);

			if (MSGSLOT(msg)[0].func == DoubleMessageData)
				dblmsg |= F_WAVEPLOTMSG;
			if (MSGSLOT(msg)[0].func == ShortMessageData)
				shortmsg |= F_WAVEPLOTMSG;
			plot->pm |= F_WAVEPLOTMSG;
		break;
		case 2:	/* XPLOT */
			plot->xptr = (short *)(MSGPTR(msg, 0));
			if (MSGSLOT(msg)[0].func == DoubleMessageData)
				dblmsg |= F_XPLOTMSG;
			if (MSGSLOT(msg)[0].func == ShortMessageData)
				shortmsg |= F_XPLOTMSG;
			plot->pm |= F_XPLOTMSG;
		break;
		case 3: /* COLOR */
			plot->color_slot.data = (char *)(MSGPTR(msg, 0));
			plot->color_slot.func = MSGSLOT(msg)[0].func;
		break;
		case 4: /* UPDATE_DT */
		break;
	}

	if (dblmsg == plot->pm)
		plot->double_flag = 1;
	else if (shortmsg == plot->pm) {
		plot->double_flag = 0;
		if (plot->shortptr_flag == 1) {
			/* This is a workaround for limitations with GENESIS msgs */
			plot->double_flag = 0;
			plot->yptr = *(short **)plot->yptr;
			if (plot->pm & F_XPLOTMSG)
				plot->xptr = *(short **)plot->xptr;
		}
	} else {
		Error();
		printf("Mixed types of messages to %s not allowed\n",
			Pathname(plot));
		plot->reset_flag = 0;
		return;
	}

	plot->nnewpts = 1;

	if (plot->pm == F_NPLOTMSG) {
		if (plot->roll_flag)
			plot->plotmode = F_ROLL;
		else if (plot->oscope_flag)
			plot->plotmode = F_OSCOPE;
		else
			plot->plotmode = F_RING;
	} else if (plot->pm == F_WAVEPLOTMSG) {
		plot->plotmode = F_WAVEPLOT;
	} else if (plot->pm == (F_WAVEPLOTMSG | F_XPLOTMSG)) {
		plot->plotmode = F_XYWAVEPLOT;
	} else if (plot->pm == (F_NPLOTMSG | F_XPLOTMSG)) {
		plot->plotmode = F_XYRING;
	} else {
		Error();
		printf("Illegal combination of messages to %s\n",
			Pathname(plot));
		printf("Use NPLOT or WAVEPLOT alone or with XPLOT\n");
		return;
	}
	if ((plot->plotmode != F_ROLL &&  plot->plotmode != F_OSCOPE) &&
		(plot->roll_flag || plot->oscope_flag)) {
			Warning();
			printf("Roll and oscope modes need NPLOT messages to %s\n",
			Pathname(plot));
	}
}

int XFastPlot (plot, action)
struct xfastplot_type *plot;
Action *action;
{
	int		ac=action->argc;
	char**	av=action->argv;
    Widget	parentW,xoFindParentForm();
	Widget fW;
	double t;
	int i;
	static int xupdating = 0;
	XPoint	*pts = (XPoint *)(plot->pts);
  
  if (Debug(0) > 1)
    ActionHeader("XFastPlot", plot, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
  	if (plot->reset_flag == 0 || plot->enabled == 0)
		return 0;

	if (plot->nnewpts_slot.func)
		plot->nnewpts =
			(int)((plot->nnewpts_slot.func)(&plot->nnewpts_slot));
	else
		plot->nnewpts = *(plot->nptr);

	if (plot->nnewpts <= 0)
		return 0;

	if (plot->color_slot.func)
		plot->color = (int)((plot->color_slot.func)(&plot->color_slot));

	if (plot->index_slot.func)
		plot->ptindex =
			(int)((plot->index_slot.func)(&plot->index_slot));

	if (plot->double_flag)
		double_plot(plot);
	else
		short_plot(plot);

    break;
  case RESET: {
	plot->reset_flag = 0;
	plot->ringpts = 0;
	plot->color_slot.func = NULL;
	XoXtVaSetValues((Widget)(plot->widget),
		XtNreset_flag, plot->reset_flag, 
		XtNcolorptr, plot->colorptr,
		NULL);

	setup_plotptrs(plot);

	plot->xscale = (double)(plot->wid - plot->lborder - plot->rborder) /
		(plot->xmax - plot->xmin);
	plot->xoffset = plot->xmin - plot->lborder / plot->xscale;

	plot->yscale = (double)(plot->ht - plot->tborder - plot->bborder) /
		(plot->ymin - plot->ymax);
	/* This is what it would be if X used lower left for 0,0 */
	/* plot->yoffset = plot->ymin + plot->screenymin / plot->yscale; */
	plot->yoffset = plot->ymax - plot->tborder / plot->yscale;

	if (plot->plotmode != F_XYWAVEPLOT && plot->plotmode != F_XYRING) {
		t = ((double)(plot->wid - plot->lborder - plot->rborder)) /
			(double)plot->npts;
		pts = (XPoint *)(plot->pts);
		for (i = 0; i < plot->npts; i++) {
			pts[i].x = plot->lborder + t * (double)i;
		}
	}
	for (i = 0; i < plot->npts; i++)
		pts[i].y = plot->ht - plot->bborder;
	
	if (plot->xval > plot->xmax) plot->xval = plot->xmax;
	if (plot->xval < plot->xmin) plot->xval = plot->xmin;
	t = (plot->xval - plot->xoffset) * plot->xscale + 0.5; /* Rounding*/
	plot->sxval =
	(t > plot->wid - plot->rborder) ? plot->wid - plot->rborder :
		((t < plot->lborder) ? plot->lborder : t);

	if (plot->yval > plot->ymax) plot->yval = plot->ymax;
	if (plot->yval < plot->ymin) plot->yval = plot->ymin;
	t = (plot->yval - plot->yoffset) * plot->yscale + 0.5;
	plot->syval =
	(t > plot->ht - plot->bborder) ? plot->ht - plot->bborder :
		((t < plot->tborder) ? plot->tborder : t);

	plot->reset_flag = 1;
		XoXtVaSetValues((Widget)(plot->widget),
			XtNreset_flag, plot->reset_flag, 
			XtNxval, plot->sxval, 
			XtNyval, plot->syval, 
			XtNnpts, plot->npts, 
			XtNpts,(XtPointer)(plot->pts),
			NULL);
  	XClearArea(XtDisplay((Widget)plot->widget),
		XtWindow((Widget)plot->widget), 0, 0, 0, 0, True);
  } break;
  case CREATE:
  	plot->npts = 32;
  	plot->alloced_npts = 32;
	plot->xmin = 0;
	plot->ymin = 0;
	plot->xmax = 32;
	plot->ymax = 1;
	plot->xval = 0.0;
	plot->yval = 0.0;
	plot->syval = -1;
	plot->lborder = 20;
	plot->rborder = 0;
	plot->tborder = 0;
	plot->bborder = 20;
	plot->title = "";
	plot->xtextmax = "";
	plot->xtextmid = "";
	plot->xtextmin = "";
	plot->ytextmax = "";
	plot->ytextmid = "";
	plot->ytextmin = "";
	plot->pts = (char *)calloc(plot->npts, sizeof(XPoint));
	plot->xaxis_flag = 1;
	plot->yaxis_flag = 1;
	plot->nnewpts = 1;
	plot->ringpts = 0;
	plot->color = XBlackPixel((Display *)XgDisplay(),0);
	plot->colorptr = &plot->color;
	plot->enabled = 1;

	plot->shortptr_flag = 0;

    /* arguments are: object_type name [field value] ... */

	if ((parentW = xoFindParentForm(plot)) == NULL) return(0);
	fW = (Widget)XtVaCreateManagedWidget(
		xoFullName(plot), framedWidgetClass, parentW,
						XtVaTypedArg,XtNhGeometry,XtRString,"80%",4,
						XtNchildClass, fastplotWidgetClass,
					    XtNmappedWhenManaged, False,
					    NULL);
    plot->widget = (char *)XoGetFramedChild(fW);
	XtAddCallback((Widget)plot->widget,XtNcallback,xoCallbackFn,
		(XtPointer)plot);
    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(plot,ac,av);
	XtMapWidget(fW);
	xoGetGeom(plot);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
  	if (strcmp(av[0],"npts") == 0) {
		int npts = atoi(av[1]);
		change_npts(npts, plot);
		return 0;
	}
  	if (strcmp(av[0],"xval") == 0) {
		plot->xval = Atof(av[1]);
		do_range(plot);
		return 0;
	}
  	if (strcmp(av[0],"yval") == 0) {
		plot->yval = Atof(av[1]);
		do_range(plot);
		return 0;
	}
  	if (strcmp(av[0],"xmax") == 0) {
		plot->xmax = Atof(av[1]);
		if (plot->xmax <= plot->xmin)
			plot->xmax = plot->xmin + 1;
		do_range(plot);
		return 0;
	}
  	if (strcmp(av[0],"ymax") == 0) {
		plot->ymax = Atof(av[1]);
		if (plot->ymax <= plot->ymin)
			plot->ymax = plot->ymin + 1;
		do_range(plot);
		return 0;
	}
  	if (strcmp(av[0],"xmin") == 0) {
		plot->xmin = Atof(av[1]);
		if (plot->xmin >= plot->xmax)
			plot->xmin = plot->xmax - 1;
		do_range(plot);
		return 0;
	}
  	if (strcmp(av[0],"ymin") == 0) {
		plot->ymin = Atof(av[1]);
		if (plot->ymin >= plot->ymax)
			plot->ymin = plot->ymax - 1;
		do_range(plot);
		return 0;
	}

	if (strcmp(av[0],"title") == 0 || strncmp(av[0] + 1,"text",4) == 0){
		char *tv[2];
		tv[0] = av[0];
		tv[1] = (char *)CopyString(av[1]);
		gx_convert(plot,GXconvert,XtNumber(GXconvert),2,tv);
		return(0);
	}
    if (ac && !xupdating) {			/* need to set fields */
		gx_convert(plot,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
  	if (plot->npts > 0)
		free(plot->pts);
	if (!(plot->widget)) return(0);
	XtDestroyWidget(XtParent((Widget)plot->widget));
	break;
  case XUPDATE : /* update fastplot fields due to changes in widget */
		xupdating = 1;
		xg_convert(plot,GXconvert,XtNumber(GXconvert),ac,av);
		xupdating = 0;
		if (plot->reset_flag) {
			plot->xval = plot->sxval/plot->xscale + plot->xoffset;
			plot->yval = plot->syval/plot->yscale + plot->yoffset;
		}
	break;
  default:
    xoExecuteFunction(plot,action,plot->script,"");
	break;
  }
  return 0;
}

#undef MAX_NTARGS /* 20 */
