static char rcsid[] = "$Id: sim_basic.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $";

/*
** $Log: sim_basic.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:31:57  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  19:35:07  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"

Basic *GetComponent(list,name)
Basic *list;
char *name;
{
Basic *component;

    if(list == NULL){
	return(NULL);
    }
    for(component=list;component;component=component->next){
	if(component->name == NULL){
	    return(NULL);
	}
	if(name == NULL ||
	strcmp(component->name,name) == 0)
	    return(component);
    }
    return(NULL);
}

