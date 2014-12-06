static char rcsid[] = "$Id: sim_simulate.c,v 1.2 2005/06/27 19:01:10 svitak Exp $";

/*
** $Log: sim_simulate.c,v $
** Revision 1.2  2005/06/27 19:01:10  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  1995/07/23 18:57:47  dhb
** 1.5 changes result in undefined value for the actionlist in
** task->argv[2] which would later be freed.  The actionlist is
** now set to NULL when there are no elements to simulate in the
** task and is only freed if non-NULL.
**
 * Revision 1.5  1995/07/18  17:48:48  dhb
 * BUILD_Simulate() modified to free the actionlist, action and
 * task->argv on second and subsequent calls.
 *
 * Revision 1.4  1995/03/15  01:20:33  dhb
 * Fix to BUILD_Simulate() to set action->argc to zero.  Previously
 * the old value of action->argc was used which is often changed
 * whenever the given action is called resulting in trying to
 * access additional args in argv.
 *
 * Revision 1.3  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.2  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.1  1992/10/27  20:28:25  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

extern Schedule *GetWorkingTask();

void Simulate(task)
Schedule	*task;
{
register Element 	**element;
ElementList	*list;
Action		*action;
ActionList	**actionlist;
int		i;
extern	short	clock_active[NCLOCKS];

    if(debug > 0){
	for(i=0;i<task->spec_argc;i++){
	    printf("%s\t",task->spec_argv[i]);
	}
	printf("\n");
    }

    /*
    ** get the element list, action and actionlist from the argument list
    */
    list = (ElementList *)(task->argv[0]);
    action = (Action *)(task->argv[1]);
    actionlist = (ActionList **) (task->argv[2]);

    /*
    ** go through all of the elements in the element list
    */
    if(list->nelements > 0){
	element = list->element;
	for(i=0;i<list->nelements;i++,element++){
	    /*
	    ** check the interval to see whether it is
	    ** it is time to update the element
	    */
	    if(clock_active[Clock(*element)]){ 
		/*
		** call the element function 
		*/
		if(*actionlist){
		    CallActionByActionList(*actionlist, *element, action);
		}
	    }
	    actionlist++;
	}
    }
}

/* LATER get the build simulate command up to snuff */
int BUILD_Simulate(argc,argv,task)
int		argc;
char		**argv;
Schedule 	*task;
{
char		*path;
ElementList	*list;
ElementList	*finallist;
ElementList	*CreateElementList();
ActionList	**actionlist;
int		nxtarg;
Action		*action;
int		i;

    path = argv[1];
    nxtarg = 1;
    action = NULL;

    while(++nxtarg < argc){
	if(strcmp(argv[nxtarg],"-action") ==0) {
	    nxtarg--;
	    break;
	}
    }
    /*
    ** check to see if this task was previously used by the
    ** same function
    */
    if((task->function == (PFI)Simulate) && (task->argc > 1)){
	/*
	** if so then free the old list
	*/
	FreeElementList(task->argv[0]);
	free(task->argv[1]);
	if (task->argv[2] != NULL)
	    free(task->argv[2]);
	free(task->argv);
	task->argc = 0;
    }
    while(++nxtarg < argc){
	if(strcmp(argv[nxtarg],"-action") ==0){
	    /*
	    ** get the action
	    */
	    action = GetAction(argv[++nxtarg]);
	} 
    }
    if(!action){
	Error();
	printf("no action specified\n");
    }

    /*
    ** construct the element list based on the path
    */
    list = WildcardGetElement(path,3);

    /*
    ** set up the final element list based on valid actions
    */
    finallist = CreateElementList(10);
    /*
    ** go through the list and check for valid action functions
    */
    for(i=0;i<list->nelements;i++){
	if(GetActionFunc(list->element[i]->object,action->type,NULL,NULL)){
	    /*
	    ** if the element can perform the action then add it to
	    ** the final list
	    */
	    if (!IsHsolved(list->element[i])) {
		/*
		** If the element is not being hsolved, add it to list
		*/
	    	AddElementToList(list->element[i],finallist);
	    }
	}
    }
    /*
    ** free the old list
    */
    FreeElementList(list);

    /*
    ** allocate the actionlist array
    */
    actionlist = NULL;  /* actionlist == NULL if there are no elements */
    if (finallist->nelements > 0)
      {
	actionlist = (ActionList**)
			    malloc(finallist->nelements*sizeof(ActionList*));

	/*
	** go through the finallist and assign the action functions
	*/
	for(i=0;i<finallist->nelements;i++){
	    actionlist[i] = GetActionListByType(finallist->element[i]->object, action->type);
	}
      }

    /*
    ** construct the task argument list
    */
    task->argc = 3;
    task->argv = (char **)calloc(4,sizeof(char *));
    task->argv[0] = (char *)finallist;
    task->argv[1] = (char *)malloc(sizeof(Action));
    action->argc = 0;
    BCOPY(action,task->argv[1],sizeof(Action));
    task->argv[2] = (char*)actionlist;
    /*
    ** check the new element list
    */
    if(finallist->nelements <= 0){
	return(0);
    } else {
	return(1);
    }
}

void ClearCheckMarkers()
{
int 		i,j;
Schedule 	*task;
ElementList	*list;

    for(i=0;i<WorkingTasks();i++){
	task = GetWorkingTask(i);
	/*
	** look for the simulation entries
	*/
	if(task->function == (PFI)Simulate){
	    /*
	    ** go through all of the elements in the list
	    ** and clear their markers
	    */
	    list = (ElementList *)task->argv[0];
	    for(j=0;j<list->nelements;j++){
		list->element[j]->flags &= ~MARKERMASK;
	    }
	}
    }
}

void CHECK_Simulate()
{
int		i;
int		j;
int		k;
ElementList	*list;
ElementList	*fulllist;
Schedule	*task;
Action		*action_table[100];
int		acount = 0;
int		can_do_action;
ActionList	*alist;
char		*action_name;

    /*
    ** go through the scheduling table 
    */
    for(i=0;i<WorkingTasks();i++){
	task = GetWorkingTask(i);
	/*
	** look for the simulation entries
	*/
	if(task->function == (PFI)Simulate){
	    /*
	    ** keep track of the different actions
	    */
	    for(j=0;j<acount;j++){
		if(strcmp(action_table[j]->name,
		((Action *)(task->argv[1]))->name) == 0) break;
	    }
	    if(j>=acount){
		/*
		** it was not found so add it to the list
		*/
		action_table[acount++] = (Action *)(task->argv[1]);
	    }
	}
    }
    /*
    ** go through all of the actions and look for duplication
    ** of elements in the simulate tasks
    */
    for(j=0;j<acount;j++){
	action_name = action_table[j]->name;
	/*
	** clear the markers for all the elements in the schedule
	*/
	ClearCheckMarkers();
	for(i=0;i<WorkingTasks();i++){
	    task = GetWorkingTask(i);
	    if(task->function == (PFI)Simulate){
		/*
		** does the task action match the current one 
		** being checked. If not then move to the next task
		*/
		if(strcmp(action_name,
		((Action *)(task->argv[1]))->name) != 0) continue;
		/*
		** go through all of the elements in the list
		** and set their markers checking for repeats
		*/
		list = (ElementList *)task->argv[0];
		for(k=0;k<list->nelements;k++){
		    if(debug > 0){
			printf("%-50s %20s\n",Pathname(list->element[k]),action_name);
		    }
		    /*
		    ** check to see whether this element has already
		    ** been referenced with this action
		    */
		    if(list->element[k]->flags & MARKERMASK){
			/*
			** already called with this action. 
			*/
			Error();
			printf("'%s' multiply invoked with action '%s'.",
			Pathname(list->element[k]),
			Actionname(action_table[j]));
			printf(" Check task [%d]\n",i+1);
		    }
		    list->element[k]->flags |= MARKERMASK;
		}
	    }
	}
    }
    /*
    ** get the full element list
    */
    fulllist = WildcardGetElement("/##[]",3);
    /*
    ** clear the markers
    */
    for(k=0;k<fulllist->nelements;k++){
	fulllist->element[k]->flags &= ~MARKERMASK;
    }
    /*
    ** go through all of the elements in 
    ** each scheduled Simulate task and
    ** check for unscheduled elements
    */
    for(i=0;i<WorkingTasks();i++){
	task = GetWorkingTask(i);
	if(task->function == (PFI)Simulate){
	    list = (ElementList *)task->argv[0];
	    for(k=0;k<list->nelements;k++){
		/*
		** mark the element as referenced
		*/
		list->element[k]->flags |= MARKERMASK;
	    }
	}
    }
    /*
    ** go back and check for unreferenced elements
    */
    for(k=0;k<fulllist->nelements;k++){
	if(!(fulllist->element[k]->flags & MARKERMASK)){
	    /*
	    ** check to see whether the element is capable of
	    ** performing any of the scheduled actions
	    */
	    can_do_action = 0;
	    for(j=0;j<acount;j++){
		for(alist=fulllist->element[k]->object->valid_actions;
		alist;alist=alist->next){
		    if(strcmp(action_table[j]->name,
		    alist->name) == 0){
			can_do_action = 1;
			break;
		    }
		}
	    }
	    if(can_do_action){
		Warning();
		printf("'%s' is not scheduled for simulation.\n",
		Pathname(fulllist->element[k]));
	    }
	}
    }
    FreeElementList(fulllist);
}
