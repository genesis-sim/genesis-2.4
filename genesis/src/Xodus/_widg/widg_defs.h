/* $Id: widg_defs.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: widg_defs.h,v $
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
 * Added value and pixflags to gadget core fields
 * Added VERSION2 compatibility stuff
 * */
#ifndef _widg_defs_h
#define _widg_defs_h
#include "struct_defs.h"
#include "_xo/xo_defs.h"
/*
*/

/* Note the hack : widgets are defined as void* since
** we do not want the baggage of the X headers in the
** xodus code.
** replacement hack : the symbol converter doesnt know
** voids, so I'll use chars
*/

#define XCORE_TYPE \
	ELEMENT_TYPE \
  	char        *widget; \
  	char		  *fg;

#define XGADGET_TYPE \
  XCORE_TYPE \
  char		*script; \
  float		tx,ty,tz; \
  char		*value; \
  int		pixflags;

#define XWIDG_TYPE \
  XCORE_TYPE \
  char		  *bg; \
  char        *xgeom, *ygeom, *wgeom, *hgeom; \


/* Temporarily here till the main list in sim_defs.h is
** updated
*/
#ifndef VERSION2

#define ADDMSGIN 299
#define DELETEMSGIN 298
#define MSGACTION 297

#endif

typedef struct {
	char*	g;	/* Genesis field name */
	char*	x;	/* Xodus  field name */
} Gen2Xo;

#endif
