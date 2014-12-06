static char rcsid[] = "$Id: sim_valid.c,v 1.2 2005/06/27 19:01:12 svitak Exp $";

/*
** $Log: sim_valid.c,v $
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:37:19  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int ValidElement(element)
Element	*element;
{
    if(element == NULL) return(0);
    if(element->object == NULL) return(0);
    return(1);
}
