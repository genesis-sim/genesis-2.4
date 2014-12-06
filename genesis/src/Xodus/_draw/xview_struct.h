/* $Id: xview_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xview_struct.h,v $
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
 * Revision 1.5  1995/05/12 23:20:12  dhb
 * Changes to xview to support 1.4 xfileview type functionality
 * using disk_in and xview elements.
 *
 * Revision 1.4  1994/04/25  17:59:11  bhalla
 * added 'valuemode' field
 *
 * Revision 1.3  1994/02/02  20:04:53  bhalla
 * *** empty log message ***
 * */
#ifndef _xview_struct_h
#define _xview_struct_h
#include "../_widg/widg_defs.h"
#include "../Draw/VarShape.h"

struct xview_type {
  XGADGET_TYPE
  varShape	*shapes;
  int		nshapes;
  char		*mode;
  char		*viewmode;
  double	value_min[XoVarNValues];
  double	value_max[XoVarNValues];
  float		*values[XoVarNValues]; 
  int		color_val;
  int		morph_val;
  int		xoffset_val;
  int		yoffset_val;
  int		zoffset_val;
  int		text_val;
  int		textcolor_val;
  int		linethickness_val;
  int		autolimits; /* on or off - 0 or 1 */
  double	sizescale;

  /* new stuff for view */
  struct interpol_struct 	*xpts;
  struct interpol_struct 	*ypts;
  struct interpol_struct 	*zpts;
  int		ncoords;
  int		max_ncoords;
  int		autoscale; /* 0 - off. 1 - max-history. 2 - max current */
  int		undraw_by_blanking; /* 0 for off, 1 for on */
  char		*path;		/* Path for looking up coords and values */
  char		*relpath;	/* path for each field relative to 'path' */
  char		*field;		/* field to display. Can also be msgin/msgout */
  char		*field2;	/* another field to display, on value2. */
  char		*msgpath;	/* Path for looking up msgins and outs */
  char		*msgtype;	/* Type of msg to get field from. */
  int		msgslotno;	/* which of the msg slots to display */
  char		*valuemode;	/* one of : path, index, lookup. */

  /* private values */
  int		allocated_pts;
  int		viewflags;	/* bit 0: data from msgs or path	**
						** bit 1: values from field 		**
						** bit 2: values from field2 		**
						** bit 3: values from msgins		**
						** bit 4: values from msgouts		**
						** bit 5: scan msgsrc path for msgs	**
						** bit 6: scan msgdest path for msgs**
						*/
  ElementList *coord_elist;
  ElementList *value_elist;
  ElementList *msg_elist;
};
#endif
