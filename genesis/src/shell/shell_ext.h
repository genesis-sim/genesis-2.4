/*
** $Id: shell_ext.h,v 1.3 2005/12/16 06:59:26 svitak Exp $
** $Log: shell_ext.h,v $
** Revision 1.3  2005/12/16 06:59:26  svitak
** Included math.h again since there may be dependence on it by
** user-compiled libraries and executables.
**
** Revision 1.2  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  2000/10/09 23:01:21  mhucka
** Portability firxes for DEC Alpha.
**
** Revision 1.2  2000/04/24 07:57:39  mhucka
** Moved the extern declarations into the more appropriate shell_defs.h
**
** Revision 1.1  1992/12/11 19:04:52  dhb
** Initial revision
**
*/

#ifndef SHELL_EXT_H
#define SHELL_EXT_H

#include <math.h>	/* for historical reasons, leave this in */
#include "header.h"
#include "hash.h"
#include "shell_defs.h"
#include "shell_struct.h"
#include "shell_func_ext.h"
/* mds3 changes */
#include "system_deps.h"


#endif /* SHELL_EXT_H */
