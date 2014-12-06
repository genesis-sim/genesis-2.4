static char rcsid[] = "$Id: sim_elemlist.c,v 1.2 2005/06/27 19:00:59 svitak Exp $";

/*
** $Log: sim_elemlist.c,v $
** Revision 1.2  2005/06/27 19:00:59  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  1996/07/23 00:36:50  dhb
** Moved IsElementInList(elm, elmlist) from newconn: returns non-zero
** if elm is in the elmlist.
**
 * Revision 1.4  1995/06/16  06:00:30  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.3  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:00:48  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define LIST_RESIZE	100
static void IncrementElementList();
static void ExpandElementList();

/*
** public functions are:
**
** 	CreateElementList(size)
** 	AddElementToList(element,list)
** 	FreeElementList(list)
*/

ElementList *CreateElementList(size)
int	size;
{
ElementList *list;

    list = (ElementList *) calloc(1,sizeof(ElementList));
    ExpandElementList(size,list);
    return(list);
}

void AddElementToList(element,list)
Element 	*element;
ElementList 	*list;
{
    if(list != NULL){
	/*
	** advance the nelements count
	*/
	IncrementElementList(list);
	/*
	** attach the element
	*/
	list->element[list->nelements-1] = element;
    }
}

void FreeElementList(list)
ElementList	*list;
{
    if(list){
	if(list->element){
	    free(list->element);
	}
	free(list);
    }
}

/*
 * IsElementInList
 *
 * FUNCTION
 *     This function determines whether a given element is
 *     contained within an element list.
 *
 * ARGUMENTS
 *     element -- pointer to element
 *     list    -- element list
 *
 * RETURN VALUE
 *     int -- 0 if element is not in list; 1 if it is
 *
 * AUTHORS
 *    Mike Vanier
 */

int IsElementInList(element, list)
     Element     *element;
     ElementList *list;
{
  int i;

  for (i = 0; i < list->nelements; i++)
    {
      if (element == list->element[i])
	return 1; /* found the element */
    }

  return 0; /* didn't find it */
}

static void IncrementElementList(list)
ElementList *list;
{
    if(list != NULL){
	if(list->nelements >= list->size){
	    ExpandElementList(LIST_RESIZE,list);
	}
	list->nelements++;
    }
}

static void ExpandElementList(expansion_size,list)
ElementList 	*list;
int 		expansion_size;
{

    if(list != NULL){
	/*
	** allocate the new element pointer array
	*/
	if(list->element){
	    list->element  = (Element **)
	    realloc(list->element,
	    sizeof(Element *)*(list->size += expansion_size));
	} else {
	    list->element  = (Element **)
	    calloc(1, sizeof(Element *)*(list->size += expansion_size));
	}
    }
}
