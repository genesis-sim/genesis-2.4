/*
** $Id: sim_ext.h,v 1.3 2005/12/16 06:59:26 svitak Exp $
** $Log: sim_ext.h,v $
** Revision 1.3  2005/12/16 06:59:26  svitak
** Included math.h again since there may be dependence on it by
** user-compiled libraries and executables.
**
** Revision 1.2  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1993/06/29 18:56:07  dhb
** Extended objects (incomplete)
**
 * Revision 1.1  1992/10/27  21:19:36  dhb
 * Initial revision
 *
*/

#include <math.h>	/* for historical reasons, leave this in */
#include <stdio.h>
#include "copyright.h"
#include "header.h"
#include "sim_defs.h"
#include "sim_struct.h"
#include "sim_func_ext.h"
#include "shell_defs.h"
/* mds3 changes */
#include "system_deps.h"

extern double				simulation_time;
extern double				clock_value[NCLOCKS];
extern int				debug;
extern Element*				ActiveElement;
extern GenesisObject*			ActiveObject;
extern int				ActiveAction;
