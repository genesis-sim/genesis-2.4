static char rcsid[] = "$Id: sim_attach.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_attach.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  1996/05/16 21:56:51  dhb
** Changes for element path hash table.
**
 * Revision 1.5  1994/10/21  21:31:14  dhb
 * Changes to Attach() to free overwritten elements were freeing following
 * children of the overwritten elements parents.  FreeTree() frees an
 * elements following siblings, so we need to NULL the next pointer.
 *
 * Revision 1.4  1994/10/18  15:58:25  dhb
 * Attach() now frees an element tree which is being overwritten.
 * ,
 *
 * Revision 1.3  1993/10/13  16:59:45  dhb
 * Fixed uninitialized variable 'status' in Attach().
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  19:30:27  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"
#include "shell_func_ext.h"

/*
** add the element pointer to the parents list of children
*/
int Attach(parent,child)
Element *parent;
Element *child;
{
Element *ptr;
Element *prev;
Element *current;
int status;
	

    if(parent == NULL || child == NULL) return(0);
    /*
    ** find the append/insert point in the child list
    */
    status = 0;
    if(parent->child == NULL){
	/*
	** the child list is empty so just append to the head 
	*/
	parent->child =  child;
    } else{
	prev = NULL;
	for(ptr=parent->child;ptr;ptr=ptr->next){
	    current = ptr;
	    if(ptr->index == child->index &&
	    (strcmp(ptr->name,child->name) == 0)){
		/*
		** found an identical child so replace it
		*/
		break;
	    }
	    prev = ptr;
	}
	if(ptr == NULL){
	    /*
	    ** append the child to the parent list
	    */
	    prev->next = child;
	    status = 1;
	} else {

	/*
	** this isn't the best place for this, but if an element
	** is overwritten it needs to be removed the the hash table.
	*/
	ElementHashRemoveTree(current);
	if(prev == NULL){
	    /*
	    ** replace current with new
	    */
	    child->next = current->next;
	    parent->child = child;
	    status = 2;
	    Warning();
	    printf("overwriting an existing element '%s'\n",
	    Pathname(current));
	    current->next = NULL;
	    FreeTree(current);
	} else {
	    /*
	    ** replace current with new
	    */
	    child->next = current->next;
	    prev->next = child;
	    status = 2;
	    Warning();
	    printf("overwriting an existing element '%s'\n",
	    Pathname(current));
	    current->next = NULL;
	    FreeTree(current);
	}
	}
    }
    /*
    ** inform the child about who just adopted it
    */
    child->parent = parent;
    return(status);
}

/*
** add the element pointer to the parents list of children
*/
int AttachToEnd(parent,last_child,child)
Element *parent;
Element *last_child;
Element *child;
{

    if(parent == NULL || child == NULL) return(0);
    /*
    ** insert the child at the end last_child list
    */
    if(last_child == NULL)
	parent->child = child;
    else
	last_child->next = child;
    /*
    ** inform the child about who just adopted it
    */
    child->parent = parent;
    return(1);
}
