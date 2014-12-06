/*
** $Id: olf_ext.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
**
** $Log: olf_ext.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:16:59  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1997/05/29 08:30:59  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS20i revison: EDS BBF-UIA 95/06/01
 * Added Tab2DInterp
 *
 * Revision 1.1  1992/11/14  00:37:20  dhb
 * Initial revision
 *
*/

#include "sim_ext.h"
#include "shell_func_ext.h"
#include "olf_struct.h"

extern double TabInterp();
extern double Tab2DInterp();
