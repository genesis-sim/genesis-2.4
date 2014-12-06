/* $Id: xcell_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xcell_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.7  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.6  1996/07/02 17:47:38  venkat
 * Introduced allocated_interps field which tracks the number of allocated
 * interpol divisions for the xcell.
 *
 * Revision 1.5  1995/07/08  03:27:17  dhb
 * Backed out preceeding changes re DIRTY flags.
 *
 * Revision 1.4  1995/06/23  22:34:21  dhb
 * Added dirty flag to xcell structure.  Supports flagging of changes to
 * path, fieldpath and field fields.
 *
 * Revision 1.3  1995/03/17  23:12:38  venkat
 * Added cellmode resource member
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions, put in hooks for version 2 but not yet
 * implemented them
 * */
#ifndef _xcell_struct_h
#define _xcell_struct_h
#include "../_widg/widg_defs.h"

struct xcell_type {
  XGADGET_TYPE
  /* fg is automatic */
  struct interpol_struct	*xpts,*ypts,*zpts,*dia,*color;
  float	colmin,colmax,diamin,diamax,diarange;
  int	soma;
  char	**names;
  char	*path;
  char	*field;
  char	*fieldpath;
  int	autocol,autodia;
  char	*labelmode; /* specifies what to do with labels */
  char	*cellmode; /* specifies how to draw the cell */

  /* tx,ty,tz specify the location of the 0,0 corner of the window in
  ** space. Later we could add x and y vectors in 3-space
  ** for the axes.
  */
  /* private values */
  int 		allocated_interps; /* Keeps track of number of allocated interpol divs */
  int		allocated_pts; /* Keeps track of the number of shape list points allocated  for the cell */
  int		npts;
/*  int		*parenthood; */
  int		*fromlist, *tolist,*shapelist;
  int		nlist;
  int		nfield;
  int		*fieldref;
};
#endif
