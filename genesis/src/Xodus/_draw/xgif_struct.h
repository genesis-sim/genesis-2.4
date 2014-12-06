/* $Id: xgif_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xgif_struct.h,v $
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
 * Revision 1.2  1994/03/22 15:43:28  bhalla
 * Added format field for future use
 *
 * Revision 1.1  1994/02/02  20:04:53  bhalla
 * Initial revision
 * */
#ifndef _xgif_struct_h
#define _xgif_struct_h
#include "../_widg/widg_defs.h"

struct xgif_type {
XGADGET_TYPE
char *filename;
char	*format; /* for future use */
int srcx, srcy, dstx, dsty;
};
#endif



