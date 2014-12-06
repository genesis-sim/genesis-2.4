/* $Id: xshape_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xshape_struct.h,v $
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
 * Revision 1.5  1996/07/02 18:12:02  venkat
 * Added the allocated_pts field to keep track of the allocated
 * interpol divs.
 *
 * Revision 1.4  1994/04/25  17:58:50  bhalla
 * Added the action definitions for XSHAPE_ADDPT and XSHAPE_MOVEPT
 *
 * Revision 1.3  1994/03/22  15:43:46  bhalla
 * removed transfmode field
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 * */
#ifndef _xshape_struct_h
#define _xshape_struct_h
#include "../_widg/widg_defs.h"

#define XSHAPE_ADDPT 217
#define XSHAPE_MOVEPT 218

struct xshape_type {
  XGADGET_TYPE
  struct interpol_struct	*xpts,*ypts,*zpts; /* later to be cast into Interpols */
  int		allocated_pts; /* Keeps track of number of interpol elements 
				 allocated and initialized */
  int		npts;
  char		*pts;
  char		*drawmode;
  int		linewidth;
  char		*linestyle;
  char		*capstyle;
  char		*joinstyle;
  char		*text;
  char		*textcolor;
  char		*textmode;
  char		*textfont;
};
#endif
