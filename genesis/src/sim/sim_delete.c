static char rcsid[] = "$Id: sim_delete.c,v 1.2 2005/06/27 19:00:59 svitak Exp $";

/*
** $Log: sim_delete.c,v $
** Revision 1.2  2005/06/27 19:00:59  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.19  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.18  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.17  1996/05/16 22:19:02  dhb
** Support for element path hash table.
**
 * Revision 1.16  1995/11/02  02:38:05  venkat
 * Removee option -all from the deleteall command
 *
 * Revision 1.15  1995/08/01  00:10:31  dhb
 * Moved check fro delete on cwe from reaper/FreeElement time to
 * delete time.  This will result in user getting errors if he
 * tries to access the element as cwe.
 *
 * Revision 1.14  1995/06/12  17:15:56  venkat
 * Temporary fix to avoid processing elements in the hierarchy
 * which might have been detached by DeleteElement() and to be freed
 * hereSome other modules such as hsolve etc would also so a similar
 * and a more structured set of hook-set-ups needs to be done in the near future.
 *
 * Revision 1.13  1995/03/02  18:27:03  dhb
 * Moved removal of messages to delete time from free time.  Also, messages are
 * now deleted prior to calling the DELETE action.
 *
 * Revision 1.12  1995/02/17  22:08:19  dhb
 * DeleteElement() changed to delay freeing of elements.  Now detaches
 * the element and calls the DELETE action.  The element is placed on
 * the delete_list ElementList.  The ElementReaper() function is called
 * by the shell cleanup callback code later to free the elements on the
 * delete_list.
 *
 * Revision 1.11  1995/02/11  01:21:43  dhb
 * Changed call of DELETE action in FreeTree() to be called after
 * freeing child elements.  I don't think this'll hurt anything
 * and may be better for widgets which will delete entire widget
 * hierarchies if done top down.
 *
 * Revision 1.10  1994/12/20  00:25:51  dhb
 * Replaced message delete code in FreeElement() with call to
 * DeleteAllMsgs().
 *
 * Revision 1.9  1994/10/18  15:47:26  dhb
 * FreeElement() frees the element name.
 *
 * Revision 1.8  1994/09/20  17:05:00  dhb
 * Moved connection related code to connection compatability library.
 *
 * Revision 1.7  1994/08/31  02:26:16  dhb
 * Fixed problem in deleting messages which resulted in not being able
 * to find the MsgOut for a MsgIn.
 *
 * Changed when DELETE actions are called.  Now DELETE get called before
 * an element's children are deleted instead of after.
 *
 * Revision 1.6  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.5  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.4  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.3  1993/02/16  22:13:04  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_delete_element (delete), do_delete_connection (delete_connection)
 * 	and do_clean (clean) changed to use GENESIS getopt routines.  do_clean
 * 	-f option changed to -force.
 *
 * Revision 1.2  1992/12/18  21:12:23  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  19:54:06  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"


/*
** call delete action for all elements in element tree
*/

static void DeleteStage1(element)

Element*	element;

{
    static Action delete_action = { "DELETE", DELETE, NULL, 0, NULL, NULL };

    /*
    ** if the current element is about to be deleted then
    ** change the current element to someplace safe
    */
    if(element == WorkingElement()){
	SetWorkingElement(RootElement());
    }

    /*
    ** should remove all possible dependencies
    ** on the element including map references, connections to
    ** segments on the element
    */

    DeleteAllMsgs(element);

    if(element->child){
	/*
	** do it for the children
	*/
	DeleteStage1(element->child);
    } 

    /*
    ** try to call any special delete function 
    */

    CallActionsSubToBase(element, element->object, &delete_action);

    if(element->next){
	/*
	** and the siblings
	*/
	DeleteStage1(element->next);
    }
}
/*
** recursively delete the element tree in a depth first fashion
*/
void FreeTree(element)
Element *element;
{
    if(element->child){
	/*
	** free the children
	*/
	FreeTree(element->child);
    } 

    if(element->next){
	/*
	** and the siblings
	*/
	FreeTree(element->next);
    }
    /*
    ** then free the element
    */
    FreeElement(element);
}


static ElementList delete_list;

/*
** ElementReaper
**
**	Called from the shell cleanup function code to free elements
**	which have been deleted.  This is necessary, since an element
**	which is in active use might delete itself or be deleted by
**	another element in a callback script.
*/

int ElementReaper()

{	/* ElementReaper --- Free previously deleted elements */
	int	i;

 	/** Temporary fix to avoid processing elements in the hierarchy 
	which might have been detached by DeleteElement() and to be freed
	here **/

	if(delete_list.nelements > 0) Reschedule();

	for (i = 0; i < delete_list.nelements; i++)
	  {
	    FreeTree(delete_list.element[i]);
	    delete_list.element[i] = NULL;
	  }

	delete_list.nelements = 0;

	return 0;

}	/* ElementReaper */


int DeleteElement(element)
Element *element;
{
Element *child;
Element *parent;

    if(element == NULL || (parent = element->parent) == NULL){
	    return(0);
    }
    /*
    ** try to remove the reference to the element in its parent
    */
    /*
    ** find the location of the element in the child list
    */
    for(child = parent->child;
	(child && child->next && !(child->next == element) && 
	!(child == element));
	child=child->next);
    /*
    ** the element was not found
    */
    if(child == NULL){
	    return(0);
    } else
    /*
    ** it reached the end of the list but still no match
    */
    if(child->next == NULL && child != element){
	    return(0);
    } else
    /*
    ** a match was found as the first child
    ** this can only happen if it is the first child
    */

    /* Remove the element and its children from the element hash table */
    ElementHashRemoveTree(element);

    if(child == element){
	    parent->child = element->next;
    } else 
    /*
    ** a match was found
    */
    if(child->next == element){
	child->next = element->next;
    } else {
	return(0);
    }
    /*
    ** traverse the subtree and free the children.
    ** this is now delayed so the element pointers
    ** remain valid until we can be sure they are
    ** no longer needed.
    */
    element->next = NULL;
    AddElementToList(element, &delete_list);

    /*
    ** Do delete time processing: DELETE action, removing messages, etc.
    */

    DeleteStage1(element);

    return(1);
}

void do_clean(argc,argv)
int argc;
char **argv;
{

    int	force;
    int	all;
    int	status;

    force = 0;
    all = 0;

    initopt(argc, argv, "-force");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-force") == 0)
	    force = 1;
	else if (strcmp(G_optopt, "-all") == 0)
	    all = 1;
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if(!force && !all){
	printf("\n*** WARNING ***\n");
	printf("This function removes all elements from the simulation.\n");
	printf("If you really want to do this use 'clean -force'.\n");
	printf("\n");
	printoptusage(argc, argv);
	return;
    }

    ElementHashRemoveTree(RootElement());
    if(force){
	DeleteStage1(RootElement());
	FreeTree(RootElement());
	sfreeall();
	StartupElements();
    } else if(all){
	DeleteStage1(RootElement());
	FreeTree(RootElement());
	sfreeall();
	SimStartup();
    }

    ClearWorkingSchedule();
    CloseAllAsciiFiles();
    ResetErrors();
    DefaultPrompt();
    printf("Simulator cleaned out.\n");
    OK();
}

int FreeElement(element)
Element *element;
{
    if(element == NULL){
	return(0);
    }

    /*
    ** free any extended fields
    */

    DelAllExtFields(element);

    /*
    ** free the element
    */
    if (element->name != NULL)
	free(element->name);
    free(element);
    return(1);
}

void do_delete_element(argc,argv)
int argc;
char **argv;
{
Element *element;

    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    element = GetElement(optargv[1]);
    if(element){
	if(!DeleteElement(element)){
	    printf("unable to delete element %s\n", optargv[1]);
	return;
	}
    } else {
	printf("unable to find element %s\n", optargv[1]);
	return;
    }
    OK();
}
