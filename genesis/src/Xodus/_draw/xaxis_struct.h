/* $Id: xaxis_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xaxis_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/06/13 23:03:06  bhalla
 * added update_parent field
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated softactions
 * */
#ifndef _xaxis_struct_h
#define _xaxis_struct_h
#include "../_widg/widg_defs.h"

struct xaxis_type {
  XGADGET_TYPE
  int		linewidth;
  char		*linestyle;
  char		*textcolor;
  char		*textmode;
  char		*textfont;
  char		*units;
  char		*title;
  float		unitoffset;
  float		titleoffset;
  float		ticklength;
  int		tickmode;
  float		labeloffset;
  int		rotatelabels;
  float		axx;
  float		axy;
  float		axz;
  float		axmin;
  float		axmax;
  float		axlength;
  float		tickx;
  float		ticky;
  float		tickz;
  float		labmin;
  float		labmax;
  int		update_parent;
};
#endif
