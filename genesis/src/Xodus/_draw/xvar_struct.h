/* $Id: xvar_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xvar_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1994/02/02 20:04:53  bhalla
 * Eliminated soft actions
 * */
#ifndef _xvar_struct_h
#define _xvar_struct_h
#include "../_widg/widg_defs.h"
#include "../Draw/VarShape.h"

struct xvar_type {
  XGADGET_TYPE
  varShape	*shapes;
  int		nshapes;
  char		*mode;
  char		*varmode;
  double	value_min[XoVarNValues];
  double	value_max[XoVarNValues];
  double	values[XoVarNValues];
  int		color_val;
  int		morph_val;
  int		xoffset_val;
  int		yoffset_val;
  int		zoffset_val;
  int		text_val;
  int		textcolor_val;
  int		linethickness_val;
  int		autolimits;
  double	sizescale;
};
#endif
