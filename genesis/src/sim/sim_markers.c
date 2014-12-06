static char rcsid[] = "$Id: sim_markers.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_markers.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.1  1994/09/20 18:54:31  dhb
** Initial revision
**
**
** This code was originally in with the connection code.  As it has
** usefulness outside that context it is now separated out.
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int CountMarkers(element)
Element *element;
{
short stk;
int count;

    stk = PutElementStack(element);
    count = ((element->flags & MARKERMASK) != 0);
    while ((element = NextElement(0,0,stk))) {
	count += ((element->flags & MARKERMASK) != 0);
    }
    FreeElementStack(stk);
    return(count);
}

void ClearMarkers(element)
Element *element;
{
short stk;

    stk = PutElementStack(element);
    element->flags &= ~MARKERMASK;
    while ((element = NextElement(0,0,stk))) {
	element->flags &= ~MARKERMASK;
    }
    FreeElementStack(stk);
}
