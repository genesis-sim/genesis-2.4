static char rcsid[] = "$Id: sim_find.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_find.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:31:57  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:05:13  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

Element *FindElement(element,name)
Element *element;
char *name;
{
int 		index;

    if(element == NULL) return(NULL);
    name = GetNameAndIndex(name,&index);
    for(;element;element=element->next){
	if(name == NULL || strcmp(element->name,name) ==0){
	    if(--index < 0)
		return(element);
	}
    }
    return(NULL);
}

