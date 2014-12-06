static char rcsid[] = "$Id: sim_tstack.c,v 1.2 2005/06/27 19:01:12 svitak Exp $";

/*
** $Log: sim_tstack.c,v $
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1993/12/23 02:29:47  dhb
** Extended Objects (mostly complete)
**
 * Revision 1.2  1993/07/21  21:32:47  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:36:48  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define STK_SIZE	100
#define MAX_STKS 	5


static Element	*tree_stk[MAX_STKS][STK_SIZE];
static short		sp[MAX_STKS];
static short		sp_used[MAX_STKS];


Element	*PopElementStack(stk)
short stk;
{
    if(--sp[stk] >= 0){
	return(tree_stk[stk][sp[stk]]);
    } else {
	sp[stk] = 0;
	return(NULL);
    }
}

int PutElementStack(element)
Element	*element;
{
short stk;
    /*
    ** find the next free stack
    */
    for(stk=0;stk<MAX_STKS;stk++){
	if(sp_used[stk] == 0) break;
    }
    if(stk==MAX_STKS){
	Error();
	printf("no more free tree stacks\n");
	stk = 0;
    }
    sp[stk] = 0;
    sp_used[stk] = 1;
    tree_stk[stk][sp[stk]++] = element->child;
    return (int)(stk);
}

void FreeElementStack(stk)
short stk;
{
    sp_used[stk] = 0;
}

int PushElementStack(element,stk)
Element	*element;
short 		stk;
{
    if(element == NULL){
	return(0);
    }
    tree_stk[stk][sp[stk]] = element;
    if(++sp[stk] > STK_SIZE){
	Error();
	printf("tree stack overflow '%s'\n",
	Pathname(element));
	sp[stk] = STK_SIZE;
	return(0);
    }
    return(1);
}

Element *TopOfStack(stk)
short stk;
{
    if(sp[stk] -1 >=0 ){
	return(tree_stk[stk][sp[stk]-1]);
    } else {
	return(NULL);
    }
}

/*
** the blockmode indicates whether to check enabled status or not
** during traversal.
** the class indicates the class required for the element
*/
Element *NextElement(blockmode,class,stk)
int 	blockmode;
int 	class;
short 	stk;
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
	** If the element is blocked
	** then dont put any of its children on the stack
	** unless the blockmode is set to ignore the blocking status
	*/
	if((next=element->child) != NULL &&
		(!(blockmode&0x1) || ENABLED(element)) &&
		((blockmode&0x2) || VISIBLE(element))){
	    PushElementStack(next,stk);
	}
	/*
	** is this a valid element?
	*/
	if((!class || CheckClass(element,class)) && 
		(!(blockmode&0x1) || ENABLED(element)) &&
		((blockmode&0x2) || VISIBLE(element))){
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
Element *FastNextElement(blockmode,stk)
int 	blockmode;
short 	stk;
{
Element 	*next;
Element 	*element;

    do{
	/*
	** pop an element from the stack
	*/
	if(--sp[stk] >= 0){
	    element = tree_stk[stk][sp[stk]];
	} else {
	    sp[stk] = 0;
	    return(NULL);
	}
	/*
	** before taking off the top element, push its neighbors 
	*/
	if((next=element->next) != NULL){
	    tree_stk[stk][sp[stk]] = next;
	    if(++sp[stk] > STK_SIZE){
		Error();
		printf("tree stack overflow '%s'\n",
		Pathname(element));
		sp[stk] = STK_SIZE;
	    }
	}

	/*
	** get the children of the element.
	** If the element is blocked
	** then dont put any of its children on the stack
	*/
	if(((next=element->child) != NULL) &&
		(!(blockmode&0x1) || ENABLED(element)) &&
		((blockmode&0x2) || VISIBLE(element))){
	    tree_stk[stk][sp[stk]] = next;
	    if(++sp[stk] > STK_SIZE){
		Error();
		printf("tree stack overflow '%s'\n",
		Pathname(element));
		sp[stk] = STK_SIZE;
	    }
	}
	/*
	** keep searching until a valid element is found
	*/
    } while(!blockmode || !((!(blockmode&0x1) || ENABLED(element)) &&
			    ((blockmode&0x2) || VISIBLE(element))));
    return(element);
}
