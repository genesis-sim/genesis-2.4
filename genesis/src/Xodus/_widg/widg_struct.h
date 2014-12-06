/* $Id: widg_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: widg_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1998/07/15 06:27:19  dhb
 * Upi update
 *
 * Revision 1.2  1994/02/02 20:30:34  bhalla
 * *** empty log message ***
 * */
#ifndef _widg_struct_h
#define _widg_struct_h
#include "widg_defs.h"

struct xwidg_type {
	XWIDG_TYPE
};

#include  "xform_struct.h"
#include  "xlabel_struct.h"
#include  "xbutton_struct.h"
#include  "xtoggle_struct.h"
#include  "xdialog_struct.h"
#include "xtext_struct.h"
#include "ximage_struct.h"
#include "xfastplot_struct.h"

/* Do not add anything after this point */
#endif
