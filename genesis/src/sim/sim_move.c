static char rcsid[] = "$Id: sim_move.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_move.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  1997/08/08 20:10:41  dhb
** Fixed test of DetachElement() which returns int (bool) value
** rather than a pointer.
**
** Revision 1.5  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1996/09/13 01:03:00  dhb
** Added valid_index handling to GetTreeCount().
**
 * Revision 1.3  1996/05/16  22:13:54  dhb
 * Support for element path hash table.
 *
 * Revision 1.2  1993/02/16  17:04:01  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_move (move) changed to use GENESIS getopt routines.  Removed buzz
 * 	words (e.g. move /output to /lkj ==> move /output /lkj).
 *
 * Revision 1.1  1992/10/27  20:19:17  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

/*
** check to see whether dst is a child of src
*/
int ValidHierarchy(src,dst)
Element	*src;
Element	*dst;
{
Element	*ptr;

    for(ptr=dst;ptr;ptr=ptr->parent){
	if(ptr == src){
	    return(0);
	}
    }
    return(1);
}

void do_move(argc,argv)
int		argc;
char		**argv;
{
char 		*src;
char 		*dst;
Element		*src_element;
Element		*dst_element;
char		*new_name;
char		*dst_path;
int		new_index = 0;
int		valid_index;
char		*ptr;

    initopt(argc, argv, "source-element dest-element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    src = optargv[1];
    dst = optargv[2];
    new_name = NULL;

    if((src_element = GetElement(src)) == NULL){
	Error();
	printf("could not find src element '%s'\n",src);
	return;
    }
    if((dst_element = GetElement(dst)) == NULL){
	/*
	** try and find the parent and use the base as the
	** name of the copy
	*/
	if(strlen(dst_path = GetParentComponent(dst)) == 0){
	    dst_path = ".";
	}
	if((dst_element = GetElement(dst_path)) == NULL){
	    Error();
	    printf("invalid copy destination '%s'\n",dst);
	    return;
	}
	/*
	** if the parent was found then use the base as the
	** new name of the copy
	*/
	new_name = GetBaseComponent(dst);
	new_index = GetTreeCount(new_name, &valid_index);
	if (!valid_index){
	    Error();
	    printf("missing or bad element index in move destination path '%s'\n", dst);
	    return;
	}
	if ((ptr = strchr(new_name,'['))) {
	    *ptr = '\0';
	}
    }
    if(!ValidHierarchy(src_element,dst_element)){
	Error();
	printf("can't move an element into itself\n");
	return;
    }
    /*
    ** detach the element from its current parent
    */
    ElementHashRemoveTree(src_element);
    if(!DetachElement(src_element)){
	Error();
	printf("could not move %s to %s\n",src,dst);
	return;
    }
    /*
    ** assign the new name
    */
    if(new_name){
	Name(src_element,new_name);
	src_element->index = new_index;
    }
    /*
    ** attach the new element to the destination
    */
    Attach(dst_element,src_element);
    ElementHashPutTree(src_element);
    OK();
}
