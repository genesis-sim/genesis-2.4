#ifndef _xfastplot_struct_h
#define _xfastplot_struct_h
#include "../_widg/widg_defs.h"

struct xfastplot_type {
  XWIDG_TYPE
  double	xmin;		/* range of pts in view. Used to scale data */
  double	xmax;		/* range of pts in view. Used to scale data */
  double	ymin; 		/* range of pts in view. Used to scale data */
  double	ymax;		/* range of pts in view. Used to scale data */
  int		npts;		/* Number of points in plot. */
  int	overlay_flag;	/* Overlay plots or clear them */
  int	line_flag;		/* Draw lines or points */
  int	roll_flag;		/* Draw plot in roll mode.
  						** Expects a message with the latest points */
  int	oscope_flag;	/* Draw plot in oscilloscope mode */
  int	trig_sign;		/* 1 for +ve, 0 for none, -1 for negative. */
  int	ac_trig_flag;	/* 1 for AC mode trig, otherwise DC trig */
  int 	enabled;		/* Obvious */
  double	trig_thresh;	/* Threshold to apply for display trigger
  						** in oscilloscope mode */
  double	update_dt;	/* average dt to maintain between X updates */
  double	request_dt;	/* Actual average dt between plot events. Used
  						** to work out throttling */
  double	yval;		/* value increment/decrement on y arrow keys in
  						** window. Can be
  						** used to specify thresh for data acquisition*/
  double	xval;		/* value increment/decrement on x arrow keys in
  						** window. Can be used to specify x offset */
  int	xaxis_flag;		/* Do we draw x axis ? */
  int	yaxis_flag;		/* Do we draw y axis ? */
  int 	color;			/* Color to use for next point(s) */
  int 	nnewpts;		/* Number of points added to plot. */
  int	ptindex;		/* Index offset for the pts array(s) */
  int lborder;			/* Border width */
  int rborder;
  int tborder;			/* Border height */
  int bborder;
  char	*title;			/* Title text to draw in plot window */
  char	*xtextmax;		/* X axis top text */
  char	*xtextmid;		/* X axis middle text */
  char	*xtextmin;		/* X axis lower text */
  char	*ytextmax;		/* X axis top text */
  char	*ytextmid;		/* X axis middle text */
  char	*ytextmin;		/* X axis lower text */
  char	*script;		/* Function to execute on event */
  int	shortptr_flag;	/* Flag to indicate that message is a pointer
  						** to a short. Needed to work around GENESIS
						** limitations on messaging */
  /* Private fields */
  char *pts;			/* X and Y data values, cast to an XPoint */
  short* xptr;			/* Pointer to variable or array with data */
  short* yptr;			/* from source element */
  int alloced_npts;		/* Number of points actually allocated */
  int plotmode;			/* options to decide how to process data */
  int wid;				/* Screen ranges for plot axes */
  int ht;
  double yoffset;		/* Convert plot values to screen coords */
  double xoffset;
  double yscale;
  double xscale;
  int	*nptr;			/* Pointer to variable with number of points */
  int	ringpts;		/* Location of current insertion pt in
  							** ring or oscope mode */
  int	trig_flag;		/* Has oscope been triggered ? */
  double	ac_trig_buf;	/* previous point for ac_trig mode */
  int*	colorptr;		/* Pointer to color value from source elm */
  int	reset_flag;		/* Flag to indicate that the plot has been **
  						** reset at least once and is valid */
  int   pm;				/* plot mode information filled on RESET */
  int	double_flag;	/* The plot msgs are doubles */
  Slot	color_slot;		/* Color msg handling for plot */
  Slot	nnewpts_slot;	/* nnewpts in NPLOT msg handling for plot */
  Slot	index_slot;		/* index in AVEPLOT msg handling for plot */
  int	sxval;			/* Used for passing from graphics to element */
  int	syval;			/* Used for passing from graphics to element */
};

/*
Messaging interface for xfastplot:
PLOT ptr: pointer to first entry in y array with the plot data.
	Assumes npts added each time.
NPLOT ptr npts: ptr to first double, npts to add
XPLOT xptr: Pointer to first entry in x array with the plot data.
COLOR color: color int to use for latest points.
UPDATE_DT: Sets update_dt to throttle X events 


For now, the things to concentrate on are basic stuff needed to
plot a fixed number of points every clock tick if NPTS message is
nonzero. We can get creative later.

*/
#endif
