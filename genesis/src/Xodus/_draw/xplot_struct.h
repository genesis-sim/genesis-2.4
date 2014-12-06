/* $Id: xplot_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xplot_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  1997/06/03 19:38:49  venkat
 * Added the processed field to the structure.
 *
 * Revision 1.4  1995/05/12  23:46:03  dhb
 * Added ADDPTS action.
 *
 * Revision 1.3  1994/06/13  23:03:17  bhalla
 * added input and do_slope fields for handling slopes
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * eliminated soft actions.
 * Added Version2 stuff.
 * */
#ifndef _xplot_struct_h
#define _xplot_struct_h
#include "../_widg/widg_defs.h"

#define	ADDPTS 219

struct xplot_type {
  XGADGET_TYPE
  /* fg is automatic */
  struct interpol_struct	*xpts,*ypts;
  int		npts;
  int		linewidth;
  char		*linestyle;
  float		xmin,ymin,xmax,ymax; /* range of plotted values. These
			handle offsets and scaling */
  float		wx,wy; /* scale factors for plot */
  float		input; /* keeps the latest value of the input msg for y */
  int		plot_placement; /* overlay fractional fixed */
  char		*memory_mode; /* limit roll oscope squish halve expand file */
  int		auto_mode; /* 0: none, 1: autoscale y. 2: auto center y.
						** 3: autoscale and auto center y */
  int		sensitivity; /* select local-value move scale edit draw */
  int		do_slope;	/* flag for doing slope rather than absolute */
  float     xsquish,ysquish; /* squish factor used in data compression*/
  /* tx,ty,tz specify the location of the 0,0 corner of the window in
  ** space. Later we could add x and y vectors in 3-space
  ** for the axes.
  */
  /* private values */
  int		allocated_pts;
  int		compressed_pts;
  char		*labelpix;
  int		notxpts;
  short 	processed;
};
#endif
