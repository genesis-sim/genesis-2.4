/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/* $Id: kin_ext.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $ */
 
/*
 * $Log: kin_ext.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2001/04/25 17:16:59  mhucka
 * Misc. small changes to improve portability and address compiler warnings.
 *
 * Revision 1.4  2000/06/12 05:07:03  mhucka
 * Removed nested comments; added NOTREACHED comments where appropriate.
 *
 * Revision 1.3  1998/07/21 19:49:11  dhb
 * Fixed ANSI C isms.
 *
 * Revision 1.2  1998/07/15 06:45:33  dhb
 * Upi update
 *
 * Revision 1.1  1997/07/24 17:49:40  dhb
 * Initial revision
 *
 * Revision 1.1  1994/06/13  22:55:39  bhalla
 * Initial revision
 * */

#include "sim_ext.h"
#include "shell_func_ext.h"
#include "kin_struct.h"

extern double Tab2DInterp();
extern struct interpol2d_struct *create_interpol2d();

#ifdef __STDC__
extern double** alloc_msgdata(int n, double** data);
#else
extern double** alloc_msgdata();
#endif

extern double DoubleMessageData();
