static char rcsid[] = "$Id: sim_stack.c,v 1.2 2005/06/27 19:01:10 svitak Exp $";

/*
** $Log: sim_stack.c,v $
** Revision 1.2  2005/06/27 19:01:10  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  1996/10/29 01:03:54  venkat
** Bumped the static limit for the number of simulataneous element stacks to 100
** from the rather low value of 5.
**
 * Revision 1.3  1994/05/27  23:26:44  dhb
 * Removed the requirement that an element be ACTIVE() to be included
 * when blockmode&0x1 is set (e.g. This is a wildcard for elements in
 * a schedule).  This allows extended objects built on neutral elements
 * to be called from the simulation schedule.
 *
 * Revision 1.2  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.1  1992/10/27  20:28:46  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define STK_SIZE	500
#define MAX_STKS 	100	

static ElementStack	element_stack[MAX_STKS];

static Element	*PopElementStack(stk)
ElementStack *stk;
{
    if(--(stk->sp) >= stk->element){
	return(*(stk->sp));
    } else {
	stk->sp = stk->element;
	return(NULL);
    }
}

static int PushElementStack(element,stk)
Element	*element;
ElementStack	*stk;
{
    if(element == NULL){
	return(0);
    }
    *(stk->sp) = element;
    if(++(stk->sp) > (stk->element + STK_SIZE)){
	Error();
	printf("tree stack overflow '%s'\n",
	Pathname(element));
	stk->sp = stk->element;
	return(0);
    }
    return(1);
}

/*
** puts the child of the element on the stack
*/
ElementStack *NewPutElementStack(element)
Element	*element;
{
short i;
ElementStack *stk;

    /*
    ** find the next free stack
    */
    stk = NULL;
    for(i=0;i<MAX_STKS;i++){
	if(element_stack[i].sp_used == 0){
	    stk = element_stack+i;
	    break;
	}
    }
    if(stk == NULL){
	Error();
	printf("no more free tree stacks\n");
    } else {
	stk->element = (Element **)malloc(STK_SIZE*sizeof(Element *));
	stk->sp = stk->element;
	stk->sp_used = 1;
	*(stk->sp) = element->child;
	(stk->sp)++;
    }
    return(stk);
}

void NewFreeElementStack(stk)
ElementStack *stk;
{
    if(stk->element != NULL){
	free(stk->element);
	stk->element = NULL;
    }
    stk->sp_used = 0;
}

Element *NewTopOfStack(stk)
ElementStack *stk;
{
    if((stk->sp - 1) >= stk->element ){
	return(*(stk->sp - 1));
    } else {
	return(NULL);
    }
}

/*
** the flag indicates whether to check enabled status or not
** during traversal.
** the class indicates the class required for the element
*/
Element *NewNextElement(flag,class,stk)
int 		flag;
int 		class;
register ElementStack 	*stk;
{
Element 	*next;
Element 	*element;
int		valid_element;

    do{
	/*
	** get an element from the stack
	*/
	if((element = PopElementStack(stk)) == NULL){
	    return(NULL);
	}
	/*
	** first push the neighbors then the children
	** this gives a depth first search of the tree
	** These can be switched around allowing the 
	** but max stack size will then have to reflect the
	** maximum breadth which is normally much larger then
	** the max depth
	*/

	/*
	** before taking off the top element, push its neighbors 
	*/
	if((next=element->next) != NULL){
	    PushElementStack(next,stk);
	}

	/*
	** get the children of the element.
	** If the element is blocked or is a component of its parent
	** then dont put any of its children on the stack
	** unless the flag is set to ignore the blocking status
	*/
	if((next=element->child) != NULL &&
		(!(flag&0x1) || ENABLED(element)) &&
		((flag&0x2) || VISIBLE(element))){
	    PushElementStack(next,stk);
	}
	/*
	** is this a valid element?
	*/
	if((!class || CheckClass(element,class)) && 
	(!(flag&0x1) || (ENABLED(element) && ACTIVE(element))) &&
	((flag&0x2) || VISIBLE(element))){
	    valid_element = TRUE;
	} else
	    valid_element = FALSE;

	/*
	** keep searching until a valid element is found
	*/
    } while (!valid_element);
    return(element);
}

/*
** streamlined version of NextElement
*/
Element *NewFastNextElement(blockmode,stk)
int 		blockmode;
register ElementStack 	*stk;
{
Element 	*next;
Element 	*element;

    do{
	/*
	** pop an element from the stack
	*/
	if(--(stk->sp) >= stk->element){
	    element = *(stk->sp);
	} else {
	    stk->sp = stk->element;
	    return(NULL);
	}
	if(element == NULL){
	    return(NULL);
	}
	/*
	** before taking off the top element, push its neighbors 
	*/
	if((next=element->next) != NULL){
	    *(stk->sp) = next;
	    if(++(stk->sp) > (stk->element + STK_SIZE)){
		Error();
		printf("tree stack overflow '%s'\n",
		Pathname(element));
		stk->sp = stk->element;
	    }
	}

	/*
	** get the children of the element.
	** If the element is blocked or is a component of it parent
	** then dont put any of its children on the stack
	*/
	if(((next=element->child) != NULL) &&
		(!(blockmode&0x1) || ENABLED(element)) &&
		((blockmode&0x2) || VISIBLE(element))){
	    *(stk->sp) = next;
	    if(++(stk->sp) > (stk->element + STK_SIZE)){
		Error();
		printf("tree stack overflow '%s'\n",
		Pathname(element));
		stk->sp = stk->element;
	    }
	}
	/*
	** keep searching until a valid element is found
	*/
    } while(((blockmode&0x1) && !ENABLED(element)) ||
	    !((blockmode&0x2) || VISIBLE(element)));
    return(element);
}
