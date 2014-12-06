static char rcsid[] = "$Id: sim_index.c,v 1.2 2005/06/27 19:01:07 svitak Exp $";

/*
** $Log: sim_index.c,v $
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.7  2000/07/03 18:33:50  mhucka
** I think qsort() needs a function argument that takes const void *
** not only under Solaris but in ISO C in general.
**
** Revision 1.6  2000/05/19 18:42:45  mhucka
** Added a prototype declaration to appease the Solaris Workshop 5.0 cc
** compiler.
**
** Revision 1.5  1994/10/21 00:11:48  dhb
** Typo
**
 * Revision 1.4  1994/10/20  17:46:09  dhb
 * FindFreeIndex() was not freeing its allocated temporary array.  Also
 * was not handling out of mem condition.
 *
 * Now maintains allocated array between calls.  If mem allocation fails
 * will return the highest existing index plus one.
 *
 * Revision 1.3  1994/06/03  21:48:20  dhb
 * Autoindex stuff from Upi Bhalla.
 *
 * Revision 1.2.1.1  1994/04/13  17:08:44  bhalla
 * Added the function FindFreeIndex for use in autoindexing
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:09:10  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

static int icompare();

int GetElementIndex(element,element_id)
Element 	*element;
int		element_id;
{
Element 	*tmp;
int 		index = 0;

    if(element == NULL) return(-1);
    for(tmp=element->parent->child;tmp;tmp=tmp->next){
	if(tmp == element)
	    return(index);
	if(tmp->name && strcmp(element->name,tmp->name) == 0)
	    index++;
    }
    return(-1);
}

/* used below for the quicksort */
#if defined( Solaris ) || defined( __STDC__ )
static int icompare(i,j)
	const void *i,*j;
#else
static int icompare(i,j)
	int	*i,*j;
#endif
{
	return(*(int *)i - *(int *)j);
}

/* This function finds the first unused index for children of 'parent'
** with the same name as 'child'. It returns the end of the child list
** in prev.
*/
int FindFreeIndex(parent,childname)
    Element	*parent;
    char	*childname;
{
    Element	*elm;
    int		nkids = 0;
    int		tarraysize = 100;
    static int	*tarray = NULL;
    int		maxindex = 0;
    int		i;

    if (tarray == NULL) {
	tarray = (int *)calloc(tarraysize,sizeof(int));
	if (tarray == NULL)
	    perror("FindFreeIndex");
    }

    for(elm = parent->child; elm; elm = elm->next) {
	if (strcmp(elm->name,childname) == 0) {
	    if (tarray != NULL)
		tarray[nkids] = elm->index;
	    if (maxindex < elm->index)
		maxindex = elm->index;
	    nkids++;
	    if (nkids >= tarraysize && tarray != NULL) {
		tarraysize *= 2;
		tarray = (int *)realloc(tarray,tarraysize * sizeof(int));
		if (tarray == NULL)
		    perror("FindFreeIndex");
	    }
	}
    }
    /* if mem alloc fails, return highest existing index plus one */
    if (tarray == NULL)
	return(maxindex+1);
    /* check if maxindex == nkids -1, in which case nextindex is == nkids */
    if (nkids == 0 && maxindex == 0) {
	return(0);
    } else if (maxindex == nkids -1) {
	return(nkids);
    } else {
	/* qsort time */
	qsort(tarray,nkids,sizeof(int),icompare);
	/* scan tarray to find an unused index */
	for (i = 0; i < nkids; i++) {
	    if (i != tarray[i])
		return(i);
	}
	/* this should never happen */
	printf("Unable to find free attach point for child \n");
	return(nkids);
    }
}
