static char rcsid[] = "$Id: sim_copy.c,v 1.2 2005/06/27 19:00:58 svitak Exp $";

/*
** $Log: sim_copy.c,v $
** Revision 1.2  2005/06/27 19:00:58  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.26  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.24  2001/04/01 19:31:21  mhucka
** Small cleanup fixes involving variable inits.
**
** Revision 1.23  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.22  1996/09/13 01:03:00  dhb
** Added valid_index handling to GetTreeCount().
**
 * Revision 1.21  1996/05/16  21:59:24  dhb
 * Support for element path hash table.
 *
 * Revision 1.20  1995/03/29  21:19:28  dhb
 * CopyAction() now frees its element stacks when a COPY action
 * returns a failed status.
 *
 * Revision 1.19  1995/01/07  02:32:25  dhb
 * CopyAction() was not returning a value on successful calling of the
 * copy action leading to spurious failures of the copy command.
 *
 * Revision 1.18  1994/12/09  02:22:23  venkat
 * changed do_copy to check for status and delete new_element
 * if necessary.
 * changed CopyAction to check for status and return it to do_copy
 *
 * Revision 1.17  1994/12/07  23:43:47  venkat
 * Initializing src and dest elements in do_copy
 *
 * Revision 1.16  1994/11/10  21:31:23  dhb
 * Moved CopyMsgs() into sim_msg.c where it belongs.
 *
 * Revision 1.15  1994/10/21  20:22:17  dhb
 * do_copy() didn't handle -repeat option when copying the source element
 * under and existing element.  Changed to number new indexes in this case
 * relative to the source element index.  (e.g. "copy src dst -repeat 3"
 * creates dst/src[0-2], while "copy src[5] dst
 * creates dst/src[5-7].
 *
 * Revision 1.14  1994/10/20  21:53:56  dhb
 * Removed call to CopyConnections() in do_copy().  This function will now
 * be performed by COPY action code in the connection compatability library.
 *
 * The CopyConnections() function has been moved to the connection
 * compatability library.
 *
 * Revision 1.13  1994/10/20  17:55:25  dhb
 * do_copy()'s do_autoindex flag was uninitialized.
 *
 * Revision 1.12  1994/10/18  19:46:45  dhb
 * CopyElement() now allocates a copy of the element name.
 *
 * Revision 1.11  1994/06/03  21:33:13  dhb
 * Merged in 1.9.1.1 changes.
 *
<<<<<<< 1.10
 * Revision 1.10  1994/05/27  23:06:27  dhb
 * Changed call to GetExtFieldAdr() in CopyMsgs() to supply new access
 * function argument.
 *
=======
 * Revision 1.9.1.1  1994/06/03  21:25:39  dhb
 * *** empty log message ***
 *
 * Revision 1.9.1.1  1994/04/13  19:08:05  bhalla
 * Added autoindex to the copy command
 *
>>>>>>> 1.9.1.1
 * Revision 1.9  1993/12/23  02:27:15  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.8  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.7  1993/03/11  18:11:41  dhb
 * Made messages work for extended fields.
 *
 * Revision 1.6  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.5  1993/02/16  16:47:54  dhb
 * Added (MsgIn*) cast to call to GetMsgInByMsgOut to silence compiler
 * warning.
 *
 * Revision 1.4  1993/02/16  16:45:11  dhb
 * Added OK(); to end of do_copy.
 *
 * Revision 1.3  1993/02/16  16:01:23  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_copy (copy) changed to use GENESIS getopt routines.  Removed buzz
 * 	words (e.g. copy /output to /lkj ==> copy /output /lkj).
 * ,
 *
 * Revision 1.2  1992/12/18  21:12:23  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  19:45:30  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"
#include "shell_func_ext.h"

/*
** routines for copying individual components
*/
Event *CopyEvents(buffer)
Buffer *buffer;
{
Event *event;
Event *new_event;
int 	buffer_size;
int 	event_size;

    if(buffer == NULL) return(NULL);
    event = buffer->event;
    buffer_size = buffer->size;
    event_size = buffer->event_size;
    if(event == NULL) return(NULL);
    /*
    ** allocate the new event
    */
    new_event = (Event *) malloc(event_size*buffer_size);
    /*
    ** copy the contents of the old event into the new event
    */
    BCOPY(event,new_event,event_size*buffer_size);
    return(new_event);
}

/*
** copy a single element
*/
Element *CopyElement(element)
Element	*element;
{
Element	*new_element;
int		size;

    if(element == NULL){
	return(NULL);
    }
    size = element->object->size;
    /*
    ** allocate the new element
    */
    if((new_element=(Element *)malloc(size)) == NULL){
	return(NULL);
    }
    /*
    ** copy the contents of the old element into the new element
    */
    BCOPY(element,new_element,size);
    /*
    ** erase the message structures
    */
    new_element->nmsgin = 0;
    new_element->msgin = NULL;
    new_element->nmsgout = 0;
    new_element->msgout = NULL;
    /*
    ** Copy the extended fields
    */
    CopyExtFields(element, new_element);
    /*
    ** null the pointers
    */
    new_element->next = NULL;
    new_element->child = NULL;
    new_element->parent = NULL;
    new_element->componentof = NULL;
    /*
    ** Copy the name
    */
    new_element->name = CopyString(element->name);
    return(new_element);
}

/*
** copy an element and all its sub-elements
*/

Element *CopyElementTree(src_element)
Element	*src_element;
{
Element	*last_element;
Element	*new_element;
Element	*new_child;
Element	*child;

    new_element = CopyElement(src_element);
    if(new_element == NULL){
	return(NULL);
    }
    last_element = new_element->child;
    for(child=src_element->child;child;child=child->next){
	new_child = CopyElementTree(child);
	AttachToEnd(new_element,last_element,new_child);
	if (child->componentof == src_element)
	    new_child->componentof = new_element;
	last_element = new_child;
    }
    return(new_element);
}

int CopyAction(orig,copy)
Element	*orig;
Element	*copy;
{
Action	action;
ElementStack	*stk0,*stk1;
Element	*orig_ptr,*copy_ptr;

    /*
    ** the trees should be identical or strange things could happen
    */
    stk0 = NewPutElementStack(orig);
    stk1 = NewPutElementStack(copy);
    /*
    ** check the first element 
    */
    action.name = "COPY";
    action.type = COPY;
    action.argc = 0;
    action.argv = NULL;
    action.data = (char *)copy;
    if (CallActionsBaseToSub(orig, orig->object, &action, 0)==0){
	NewFreeElementStack(stk0);
	NewFreeElementStack(stk1);
  	return 0;
    }
  	
    /*
    ** construct the element equivalence table
    ** by adding all elements in the tree
    */
    while((orig_ptr = NewFastNextElement(2,stk0)) != NULL){
	copy_ptr = NewFastNextElement(2,stk1);

	action.data = (char *)copy_ptr;
        if (CallActionsBaseToSub(orig_ptr, orig_ptr->object, &action, 0)==0){
	    NewFreeElementStack(stk0);
	    NewFreeElementStack(stk1);
	    return 0;
        }
    }
    NewFreeElementStack(stk0);
    NewFreeElementStack(stk1);

    return 1;
}

void do_copy(argc,argv)
int		argc;
char		**argv;
{
char 		*src;
char 		*dst;
int 		repeat;
Element	*src_element=NULL;
Element	*dst_element=NULL;
Element	*new_element=NULL;
char		*new_name;
char		*dst_path;
char		*ptr;
int		new_index = 0;
int		valid_index;
int		i;
int		status;
int		do_autoindex;

    repeat = 1;
    do_autoindex = 0;
    initopt(argc, argv, "source-element dest-element -repeat # -autoindex");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-repeat") == 0)
	    repeat = atoi(optargv[1]);
	if (strcmp(G_optopt, "-autoindex") == 0)
	    do_autoindex = 1;
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }
	if (repeat != 1 && do_autoindex) {
		printoptusage(argc,argv);
		printf("Cannot combine autoindex with repeat\n");
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
	    printf("missing or bad element index in copy destination path '%s'\n", dst);
	    return;
	}
	if ((ptr = strchr(new_name,'['))) {
	    *ptr = '\0';
	}
    }
    if(!ValidHierarchy(src_element,dst_element)){
	Error();
	printf("can't copy an element into itself\n");
	return;
    }
    /*
    ** copy it
    */
    for(i=0;i<repeat;i++){
	if((new_element = CopyElementTree(src_element)) == NULL){
	    printf("could not copy %s to %s\n",src,dst);
	    break;
	}
	if(new_name){
	    Name(new_element,new_name);
	    new_element->index = new_index;
	}
	if (do_autoindex) {
		new_element->index =
			FindFreeIndex(dst_element,new_element->name);
	} else
	    new_element->index += i;
	/*
	** attach the new element to the destination
	*/
	Attach(dst_element,new_element);
	/*
	** add the new element to the element hash table
	*/
	ElementHashPutTree(new_element);
	/*
	** copy the msgs between elements
	*/
	CopyMsgs(src_element,new_element);
	/*
	** call special copy routines if any
	*/
       if (CopyAction(src_element,new_element)==0){
	  DeleteElement(new_element);
	  SetRecentElement(NULL);
	  FAILED();
	  return;
	}
    }
    SetRecentElement(new_element);
    OK();
}

