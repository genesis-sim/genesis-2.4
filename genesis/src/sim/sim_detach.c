static char rcsid[] = "$Id: sim_detach.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $";

/*
** $Log: sim_detach.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:31:57  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  19:57:55  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int DetachElement(element)
Element		*element;
{
Element		*parent;
Element		*ptr;
    
    if((parent = element->parent) == NULL){
	return(0);
    }
    /*
    ** this can only happen at the beginning of the list
    */
    if(parent->child == element){
	parent->child = element->next;
	element->next = NULL;
	element->parent = NULL;
	return(1);
    }
    for(ptr=parent->child;ptr;ptr=ptr->next){
	if(ptr->next == element){
	    ptr->next = element->next;
	    element->next = NULL;
	    element->parent = NULL;
	    return(1);
	}
    }
    return(0);
}
