/* $Id: draw_ext.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: draw_ext.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2001/04/25 17:16:57  mhucka
 * Misc. small changes to improve portability and address compiler warnings.
 *
 * Revision 1.3  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1994/02/02 20:04:53  bhalla
 * *** empty log message ***
 * */
#ifndef _widg_ext_h
#define _widg_ext_h

#include "sim_ext.h"
#include "shell_func_ext.h"

extern void xoCallbackFn();

#include "_xo/xo_defs.h"
#include "../_widg/widg_defs.h"
#include "draw_defs.h"
#include "draw_struct.h"

#endif



