/* $Id: xform_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xform_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1997/06/28 00:06:25  venkat
 * Added the nested flag for embedded forms/
 *
 * Revision 1.3  1994/03/22  15:42:27  bhalla
 * Added auto_resize and label_w fields
 *
 * Revision 1.2  1994/02/02  20:30:34  bhalla
 * Added auto_resize field, not used yet
 * */
#ifndef _xform_struct_h
#define _xform_struct_h
#include "widg_defs.h"

struct xform_type {
  XWIDG_TYPE
  char *title;
  int	auto_resize;
  /* private fields */
  char *shell;
  char *outerframe;
  char *middleframe;
  char *innerframe;
  char *label_w;
	
  short nested;
};
#endif
