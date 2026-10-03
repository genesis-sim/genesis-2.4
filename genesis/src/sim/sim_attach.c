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
Element *prev;
Element *ptr;
Element *current;
char	childpath[300];
int status;


    if(parent == NULL || child == NULL) return(0);
    /*
    ** name/index must be final before we can compute child's pathname
    ** (Pathname() walks up via ->parent, which Create() has not set yet)
    */
    child->parent = parent;
    status = 0;
    if(parent->child == NULL){
	/*
	** the child list is empty so just append to the head
	*/
	parent->child = child;
	parent->childtail = child;
    } else {
	/*
	** O(1) duplicate check via the global element hash table instead
	** of an O(n) linear scan of the sibling list -- do_create() always
	** calls ElementHashPutTree() right after Attach() returns, so the
	** hash table already holds every previously-attached sibling by
	** the time we get here.  Pathname(child) is globally unique, so a
	** hit can only be the existing child at this exact name+index.
	**
	** Pathname() returns a pointer into a static buffer that gets
	** overwritten by ElementHashFind()'s own internal Pathname() calls
	** (used for its comparisons), so the key must be copied out first
	** -- exactly the precaution ElementHashRemove() already takes.
	*/
	strcpy(childpath, Pathname(child));
	current = ElementHashFind(childpath);
	if(current == NULL){
	    /*
	    ** genuinely new: O(1) append via the tracked tail
	    ** (mirrors AttachToEnd, which does the same thing when the
	    ** caller tracks the tail itself, e.g. CopyElementTree/createmap)
	    */
	    parent->childtail->next = child;
	    parent->childtail = child;
	    status = 1;
	} else {
	    /*
	    ** rare path: an identical child already exists and must be
	    ** replaced.  Finding its predecessor still needs a scan, but
	    ** this is the cold "overwriting" branch, not every create.
	    */
	    prev = NULL;
	    for(ptr=parent->child;ptr && ptr != current;ptr=ptr->next){
		prev = ptr;
	    }
	    /*
	    ** this isn't the best place for this, but if an element
	    ** is overwritten it needs to be removed the the hash table.
	    */
	    ElementHashRemoveTree(current);
	    child->next = current->next;
	    if(prev == NULL){
		parent->child = child;
	    } else {
		prev->next = child;
	    }
	    if(parent->childtail == current){
		parent->childtail = child;
	    }
	    status = 2;
	    Warning();
	    printf("overwriting an existing element '%s'\n",
	    Pathname(current));
	    current->next = NULL;
	    FreeTree(current);
	}
    }
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
    parent->childtail = child;
    /*
    ** inform the child about who just adopted it
    */
    child->parent = parent;
    return(1);
}
