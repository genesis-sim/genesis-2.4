/* $Id: xbutton_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xbutton_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1994/02/02 20:30:34  bhalla
 * cleaned up a bit
 * */
#ifndef _xbutton_struct_h
#define _xbutton_struct_h
#include "widg_defs.h"

struct xbutton_type {
  XWIDG_TYPE		/* Defined   in widg_defs.h */
  char *onlabel;
  char *offlabel;
  char *onfg;
  char *offfg;
  char *onbg;
  char *offbg;
  char *onfont;
  char *offfont;
  char *script;
};
/* Do not add anything after this point */
#endif
