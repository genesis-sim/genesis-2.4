/* $Id: xgraph_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xgraph_struct.h,v $
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
 * Revision 1.5  1997/06/03 19:36:34  venkat
 * Changed the processed and overlay fields to shorts.
 *
 * Revision 1.4  1995/03/21  21:51:58  venkat
 * Added processed flag to prevent appearance of
 * overlay labels with multiple RESET actions.
 *
 * Revision 1.3  1994/11/07  22:16:55  bhalla
 * Added in new fields cdxmin etc to represent the limits of the
 * draw widget base class for the graph widget.
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 * Added stuff for VERSION2 compatibility
 * */
#ifndef _xgraph_struct_h
#define _xgraph_struct_h
#include "../_widg/widg_defs.h"

struct xgraph_type {
  XWIDG_TYPE
  float	xmin,xmax, ymin,ymax;
  float	cdxmin,cdxmax, cdymin,cdymax;
  float	xoffset,yoffset;
  int	drawflags;
  char	*script;
  short	overlay;
  short processed;
  char	*title;
  char	*xlabel;
  char	*XUnits;
  char	*ylabel;
  char	*YUnits;
  /* Private fields : */
  char *xaxis_w;
  char *yaxis_w;
  int	overlay_no;
};
#endif
