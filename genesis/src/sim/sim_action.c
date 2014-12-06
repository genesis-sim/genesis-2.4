static char rcsid[] = "$Id: sim_action.c,v 1.2 2005/06/27 19:00:57 svitak Exp $";

/*
** $Log: sim_action.c,v $
** Revision 1.2  2005/06/27 19:00:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.21  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.19  2001/04/01 19:31:21  mhucka
** Small cleanup fixes involving variable inits.
**
** Revision 1.18  1997/07/22 17:29:29  venkat
** QUIT action included as one of the BASIC actions
**
** Revision 1.17  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.16  1996/10/05 17:21:36  dhb
** Added USECLOCK action
**
 * Revision 1.15  1996/05/16  21:53:45  dhb
 * Added CallEventActionWithTime() which passes event time explicitly
 * rather than using SimulationTime() as in CallEventAction().
 *
 * CallEventAction() and CallEventActionWithTime() now check to ensure
 * that only active messages are delivered.
 *
 * Revision 1.14  1995/03/17  23:07:14  dhb
 * changed do_list_actions() to use GENESIS getopt().
 *
 * Revision 1.13  1995/02/18  00:53:15  dhb
 * Increased action hash table size from 100 to 1000.
 *
 * Revision 1.12  1994/12/15  17:47:49  dhb
 * Added MSGINDELETED and MSGOUTDELETED actions which are called AFTER
 * a MsgIn or MsgOut is deleted.  This is in addition to DELETEMSGIN and
 * DELETEMSGOUT which are called prior to deleting MsgIn or MsgOut.
 *
 * Revision 1.11  1994/10/21  00:45:49  dhb
 * CallScriptActionFunc() was calling CopyString() on returns values from
 * itoa().  But itoa() returns allocated memory!.  Removed the
 * CopyString().
 *
 * Revision 1.10  1994/08/31  02:16:17  dhb
 * Code change in CallActionFunc() to handle callers not filling in
 * the action->name.  Now sets this from actionlist->name which overrides
 * any caller setting of this field.
 *
 * Added a SimExecuteFunction() wrapper to ExecuteFunction() to reset the
 * context of a script function call to global scope.  Script callbacks
 * in Xodus widgets need to do this since the scripts are called from an
 * action on the Xodus widget.
 *
 * Revision 1.9  1994/06/03  21:14:37  dhb
 * Changed CallEventAction() to use backpointer in the MsgOut to get the
 * corresponding MsgIn rather than call GetMsgInByMsgOut().
 *
 * Revision 1.8  1994/05/27  22:37:12  dhb
 * Added code to CallScriptActionFunc() to call the MSG actions with the
 * message number as argument.
 *
 * Revision 1.7  1994/03/22  20:30:35  dhb
 * Added DUMP and UNDUMP to basic actions added by default.
 *
 * Revision 1.6  1994/03/18  22:24:12  dhb
 * Changed AddActionToObject() to set the action's object to the calling
 * object when the caller indicates the action function is not a script
 * function.
 *
 * Revision 1.5  1993/12/23  02:20:21  dhb
 * Extended objects (mostly complete)
 *
 * Revision 1.4  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.3  1993/02/17  23:13:46  dhb
 * Fixed type of rcsid variable.
 *
 * Revision 1.2  1993/02/17  23:09:56  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_add_action (addaction) changed to use GENESIS getopt routines.
 *
 * do_list_actions (not bound, yet) changed header which used to read:
 * DEFINED CLASSES to read: DEFINED ACTIONS.
 *
 * Revision 1.1  1992/10/27  19:27:49  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"
#include "hash.h"
#include "shell_func_ext.h"
#include "ss_func_ext.h"

HASH *action_hash_table;

Element*	ActiveElement = NULL;
GenesisObject*	ActiveObject = NULL;
int		ActiveAction = -1;

extern int yyerror();

/*
** Hash table utilities
*/
void ActionHashInit()
{
HASH *hash_create();

    /*
    ** create the table
    */
    action_hash_table = hash_create(1000);
}

int ActionHashPut(key,action)
char	*key;
Action	*action;
{
ENTRY	item,*hash_enter();

    item.data = (char *)malloc(sizeof(Action));
    BCOPY(action,item.data,sizeof(Action));
    item.key = CopyString(key);
    /*
    ** put the action into the table
    */
    if(hash_enter(&item,action_hash_table) == NULL){
	Error();
	printf("action hash table full\n");
	return(0);
    };
    return(1);
}


Action *GetAction(name)
char	*name;
{
ENTRY	*found_item,*hash_find();

    /*
    ** get the action from the table
    */
    if((found_item = hash_find(name,action_hash_table)) != NULL){
	return((Action *)(found_item->data));
    } else
	return(NULL);
}

char *GetActionName(type)
int	type;
{
extern HASH *action_hash_table;
char 	*name;
int 	i;
static char	actionname[200];
int	multiple = 0;

    actionname[0] = '\0';
    for(i=0;i<action_hash_table->size;i++){
	if ((name = action_hash_table->entry[i].key)) {
	    if(type == ((Action *)(action_hash_table->entry[i].data))->type){
		if(multiple){
		    strcat(actionname," , ");
		}
		strcat(actionname,name);
		multiple = 1;
	    }
	}
    }
    if(strlen(actionname) == 0){
	return("???");
    } else
	return(actionname);
}

void AddAction(name,type)
char	*name;
int	type;
{
Action	action;

    BZERO(&action,sizeof(Action));
    action.name = CopyString(name);
    action.type = type;
    ActionHashPut(action.name,&action);
}


/*
** do_add_action
**
** FUNCTION
**	The addaction command adds an action to an element.  The named
**	action is associated with the script function given.
**
** ARGUMENTS
**	int argc	- Number of command arguments
**	char** argv	- The command arguments
**
** RETURN VALUE
**	None.
**
** SIDE EFFECTS
**	If the action name is unknown the name is added to the table of
**	action names.  An action number is automatically generated.
**
** NOTES
**	This version of do_add_action allows the GENESIS version 1 usage
**	of the addaction command for backward compatability.
**
** AUTHOR
**	David Bilitch
*/


int do_add_action(argc,argv)

int	argc;
char 	**argv;

{	/* do_add_action --- addaction command; add action to element */

	static int	nextActionNumber = 10000;

	initopt(argc, argv, "[element] action-name value-or-function");
	if (G_getopt(argc, argv) != 0 || optargc > 4)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	if (optargc == 3)
	    AddAction(optargv[1],atoi(optargv[2]));
	else
	  {
	    GenesisObject*	object;
	    GenesisObject*	newobject;
	    Element*		elm;

	    elm = GetElement(optargv[1]);
	    if (elm == NULL)
	      {
		InvalidPath(optargv[0], optargv[1]);
		return 0;
	      }

	    if (GetAction(optargv[2]) == NULL)
		AddAction(optargv[2], nextActionNumber++);

	    object = ObjectModify(elm->object);
	    if (object == NULL)
	      {
		fprintf(stderr, "%s: Cannot modify object; cannot add action '%s' to element '%s'\n",
		    optargv[0], optargv[2], Pathname(elm));
		return 0;
	      }

	    AddActionToObject(object, optargv[2], CopyString(optargv[3]), 1);
	    newobject = ObjectCacheCheck(object);
	    if (newobject == object)
		if (!UpdateActionObjects(object))
		    return 0;

	    elm->object = newobject;
	  }

	OK();
	return 1;

}	/* do_add_action */


/*
** do_delete_action
**
** FUNCTION
**	Delete an action from an element.
**
** ARGUMENTS
**	int argc	- Number of command arguments
**	char** argv	- Command arguments
**
** RETURN VALUE
**	int		- 1 on success, 0 if the element is not found or
**			  wrong command arguments or the action is permanent
**
** SIDE EFFECTS
**	Deleting an action changes the object of an element.
**
** AUTHOR
**	David Bilitch
*/

int do_delete_action(argc, argv)

int	argc;
char**	argv;

{	/* do_delete_action --- Delete the given action from the element */

	GenesisObject*	object;
	Element*	elm;
	char*		actionName;

	initopt(argc, argv, "element action-name\n");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	elm = GetElement(optargv[1]);
	if (elm == NULL)
	  {
	    InvalidPath(optargv[0], optargv[1]);
	    return 0;
	  }
	actionName = optargv[2];

	if (ActionIsPermanent(elm->object, actionName))
	  {
	    fprintf(stderr, "%s: Cannot delete permanent action '%s' on element '%s'\n",
		    optargv[0], actionName, Pathname(elm));

	    return 0;
	  }

	object = ObjectModify(elm->object);
	if (!DeleteActionFromObject(object, actionName))
	  {
	    ObjectFree(object);
	    return 0;
	  }

	elm->object = ObjectCacheCheck(object);

	OK();
	return 1;

}	/* do_delete_action */


void BasicActions()
{
    AddAction(	"INIT",		INIT		);
    AddAction(	"PROCESS",	PROCESS		);
    AddAction(	"RESET",	RESET		);
    AddAction(	"CHECK",	CHECK		);
    AddAction(	"CREATE",	CREATE		);
    AddAction(	"COPY",		COPY		);
    AddAction(	"SET",		SET		);
    AddAction(	"SHOW",		SHOW		);
    AddAction(	"RECALC",	RECALC		);
    AddAction(	"DELETE",	DELETE		);
    AddAction(	"SAVE",		SAVE		);
    AddAction(	"RESTORE",	RESTORE		);
    AddAction(	"SAVE2",	SAVE2		);
    AddAction(	"RESTORE2",	RESTORE2	);
    AddAction(	"SETUP",	SETUP		);
    AddAction(	"RESULTS",	RESULTS		);
    AddAction(	"EVENT",	EVENT		);
    AddAction(	"ADDMSGIN",	ADDMSGIN	);
    AddAction(	"ADDMSGOUT",	ADDMSGOUT	);
    AddAction(	"DELETEMSGIN",	DELETEMSGIN	);
    AddAction(	"DELETEMSGOUT",	DELETEMSGOUT	);
    AddAction(	"DUMP",		DUMP	);
    AddAction(	"UNDUMP",	UNDUMP	);
    AddAction(	"MSGINDELETED",	MSGINDELETED	);
    AddAction(	"MSGOUTDELETED",MSGOUTDELETED	);
    AddAction(	"USECLOCK",     USECLOCK	);
    AddAction(	"QUIT",     	QUIT	);
}

void do_list_actions(argc, argv)

int	argc;
char*	argv[];

{
extern HASH *action_hash_table;
char 	*name;
int	cnt=0;
int 	i;

    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    printf("DEFINED ACTIONS:\n");
    for(i=0;i<action_hash_table->size;i++){
	if ((name = action_hash_table->entry[i].key)) {
	    printf("[ %3d ] %-12s",
	    *((int *)(action_hash_table->entry[i].data)),name);
	    if(((++cnt)%4) == 0){
		printf("\n");
	    }
	}
    }
    printf("\n\n");
}

/*
** WARNING: Code for extended objects assumes that this function adds actions
** at the beginning of the list.
*/

int AddActionToObject(object, name, function, scriptfunc)
GenesisObject	*object;
char	*name;
PFI	function;
int	scriptfunc;
{
ActionList	*new;
Action		*action;

    if(!(action = GetAction(name))){
	return(0);
    }
    /*
    ** add the new valid action to the object
    */
    new = (ActionList *)calloc(1,sizeof(ActionList));
    new->name = action->name;
    new->type = action->type;
    new->scriptfunc = scriptfunc;
    new->function = function;
    new->object = NULL;
    new->next = object->valid_actions;
    object->valid_actions = new;

    if (!scriptfunc)
      {
	new->flags = ACTION_PERMANENT;
	new->object = object;
      }

    return(1);
}


/*
** UpdateActionObjects
**
** FUNCTION
**	Update the objects for each non-permanent action to refer to the
**	given object.
**
** ARGUMENTS
**	GenesisObject* object	- Object with action list to update
**
** RETURN VALUE
**	int			- 1 on success, 0 if out of memory
**
** NOTES
**	All non-permanent actions have to be copied, since the object for
**	each action must be the object in which the action is to execute.
**	This will differ for each temporary object created.
**
** AUTHOR
**	David Bilitch
*/

int UpdateActionObjects(object)

GenesisObject*	object;

{	/* UpdateActionObjects --- Update action objects to refer to object */

	ActionList*	old = NULL;
	ActionList*	copy = NULL;
	ActionList*	prev = NULL;

	old = object->valid_actions;
	while (old != NULL && !(old->flags&ACTION_PERMANENT))
	  {
	    ActionList*	new;

	    new = (ActionList *) malloc(sizeof(ActionList));
	    if (new == NULL)
	      {
		perror("UpdateActionObjects");
		while (copy != NULL)
		  {
		    prev = copy;
		    copy = copy->next;
		    free(prev);
		  }

		return 0;
	      }

	    *new = *old;
	    new->object = object;
	    new->next = NULL;

	    if (copy == NULL)
		copy = new;
	    else
		prev->next = new;

	    prev = new;
	    old = old->next;
	  }

	if (copy != NULL)
	  {
	    prev->next = old;
	    object->valid_actions = copy;
	  }

	return 1;

}	/* UpdateActionObjects */


/*
** DeleteActionFromObject
**
** FUNCTION
**	Delete an action from an object. 
**
** ARGUMENTS
**	GenesisObject*	object	- Object from which to delete action
**	char* actionName	- Name of the action to delete
**
** RETURN VALUE
**	int			- 1 on success, 0 if the named action is not
**				  in the list
**
** NOTES
**	Because the object action list is shared between objects we have
**	to make a copy of all actions up to the one we want to remove.  The
**	removed action cannot be freed as it still appears in other action
**	lists.
**
** AUTHOR
**	David Bilitch
*/

int DeleteActionFromObject(object, actionName)

GenesisObject*	object;
char*		actionName;

{	/* DeleteActionFromObject --- Delete an action from an object */

	ActionList*	cur;
	ActionList*	copy;
	ActionList*	curcopy = NULL;
	Action*		action;

	action = GetAction(actionName);
	if(action == NULL)
	    return(0);

	copy = NULL;
	for (cur = object->valid_actions; cur != NULL; cur = cur->next)
	  {
	    ActionList*	new;

	    if (cur->type == action->type)
		break;

	    new = (ActionList*) malloc(sizeof(ActionList));
	    if (new == NULL)
	      {
		perror("DeleteActionFromObject");
		return 0;
	      }

	    *new = *cur;
	    new->next = NULL;

	    if (copy == NULL)
		copy = new;
	    else
		curcopy->next = new;

	    curcopy = new;
	  }

	if (cur == NULL)
	  {
	    while (copy != NULL)
	      {
		cur = copy;
		copy = cur->next;
		free(cur);
	      }

	    return 0;
	  }

	if (copy == NULL)
	    copy = cur->next;
	else
	    curcopy->next = cur->next;

	object->valid_actions = copy;

	return 1;

}	/* DeleteActionFromObject */


/*
** ActionIsPermanent
**
** FUNCTION
**	Returns TRUE if a given action is permanent.
**
** ARGUMENTS
**	GenesisObject* object	- object with action list to check
**	char* actionName	- action name to check for
**
** RETURN VALUE
**	int			- 1 if permament, 0 if not
**
** AUTHOR
**	David Bilitch
*/

int ActionIsPermanent(object, actionName)

GenesisObject*	object;
char*		actionName;

{	/* ActionIsPermanent --- Return permanent status of action */

	ActionList*	cur;
	Action*		action;

	action = GetAction(actionName);
	if (action == NULL)
	    return 0;

	for (cur = object->valid_actions; cur != NULL; cur = cur->next)
	  {
	    if (cur->type == action->type)
		return (cur->flags&ACTION_PERMANENT) == ACTION_PERMANENT;
	  }

	return 0;

}	/* ActionIsPermanent */


/*
** ActionListMakePermanent
**
** FUNCTION
**	Sets the ACTION_PERMANENT flag for all the new actions in the
**	valid_actions list.
**
** ARGUMENTS
**	GenesisObject* object	- object with action list to make permanent
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch
*/

void ActionListMakePermanent(object)

GenesisObject*	object;

{	/* ActionListMakePermanent --- Make object's actions permanent */

	ActionList*	cur;

	cur = object->valid_actions;
	while (cur != NULL && (cur->flags&ACTION_PERMANENT) != ACTION_PERMANENT)
	  {
	    cur->flags |= ACTION_PERMANENT;
	    cur = cur->next;
	  }

}	/* ActionListMakePermanent */


/*
** ActionListEqual
**
** FUNCTION
**	Check two action lists to see if they are the same.
**
** ARGUMENTS
**	ActionList* list1	- Lists to compare.
**	ActionList* list2
**
** RETURN VALUE
**	int			- 1 if the lists are the same, 0 if not.
**
** NOTES
**
**	Assumes no duplicate actions are in the list.  If the list lengths
**	are the same then each action in list1 is checked to see that it
**	appears in list2.
**
** AUTHOR
**	David Bilitch
*/

int ActionListEqual(list1, list2)

ActionList*	list1;
ActionList*	list2;

{	/* ActionListEqual --- Check that ActionLists are the same */

	ActionList*	a1;
	ActionList*	a2;

	a1 = list1;
	a2 = list2;
	while (a1 != NULL && a2 != NULL)
	  {
	    a1 = a1->next;
	    a2 = a2->next;
	  }

	if (a1 != a2)
	    return 0;

	for (a1 = list1; a1 != NULL; a1 = a1->next)
	  {
	    for (a2 = list2; a2 != NULL; a2 = a2->next)
	      {
		if (a1->type != a2->type)
		    continue;
		if (a1->scriptfunc != a2->scriptfunc)
		    continue;
		if (a1->function == a2->function)
		    break;
		if (a1->scriptfunc &&
		      strcmp((char*)a1->function,(char*)a2->function) == 0)
		    break;
	      }

	    if (a2 == NULL)
		return 0;
	  }

	return 1;

}	/* ActionListEqual */


char *Actionname(action)
Action		*action;
{
    return(action->name);
}

char *CheckActionType(object,type)
GenesisObject	*object;
int	type;
{
ActionList	*list;

    for(list=object->valid_actions;list;list=list->next){
	if(list->type == type){
	    return(list->name);
	}
    }
    return(NULL);
}

int CheckActionName(object,name)
GenesisObject	*object;
char	*name;
{
ActionList	*list;

    for(list=object->valid_actions;list;list=list->next){
	if(strcmp(list->name,name) == 0){
	    return(list->type);
	}
    }
    return(-1);
}


ActionList* GetActionListByType(object, type)

GenesisObject*	object;
int		type;

{	/* GetActionListByType --- Return the actionlist in object for the
				   given type */

	ActionList	*list;

	for(list=object->valid_actions;list;list=list->next){
	    if(list->type == type){
		return(list);
	    }
	}
	return(NULL);

}	/* GetActionListByType */


PFI GetActionFunc(object,type,scriptfunc,actobject)
GenesisObject	*object;
int		type;
int		*scriptfunc;
GenesisObject	**actobject;
{
ActionList	*alist;

    for(alist=object->valid_actions;alist;alist=alist->next){
	if(alist->type == type){
	    if (scriptfunc != NULL)
		*scriptfunc = alist->scriptfunc;
	    if (actobject != NULL)
		*actobject = alist->object;

	    return(alist->function);
	}
    }
    return(NULL);
}


int CallScriptActionFunc(func, element, action)

char*		func;
Element*	element;
Action*		action;

{	/* CallScriptActionFunc --- Call a script function for an action */

	Element* pop_element();
	int	argc;
	char**	argv;
	int	freearg;
	int	nxtarg;
	int	status;

	/*
	** Allocate a new argv which is large enough for the script
	** function anem, action name, original argv values plus
	** one additional argv value which may be added depending
	** on the action called.  If any action requires more than
	** one additional arg, we'll need to allocate a larger argv.
	*/

	argc = action->argc + 2;
	argv = (char**) malloc((argc+1)*sizeof(char*));

	argv[0] = (char*) func;
	argv[1] = action->name;

	nxtarg = 2;
	freearg = 0;
	switch (action->type)
	  {

	  case CREATE:
	  case COPY:
	    freearg = nxtarg;
	    argv[nxtarg++] = CopyString(Pathname((Element*)action->data));
	    argc++;
	    break;

	  case SHOW:
	    argv[nxtarg++] = action->data;
	    argc++;
	    break;

	  case EVENT:
	    freearg = nxtarg;
	    argv[nxtarg++] = CopyString(Pathname(((MsgIn*)action->data)->src));
	    argc++;
	    break;

	  case ADDMSGIN:
	  case DELETEMSGIN:
	    freearg = nxtarg;
	    argv[nxtarg++] = itoa(GetMsgInNumber(element, action->data));
	    argc++;
	    break;

	  case ADDMSGOUT:
	  case DELETEMSGOUT:
	    freearg = nxtarg;
	    argv[nxtarg++] = itoa(GetMsgOutNumber(element, action->data));
	    argc++;
	    break;

	  case USECLOCK:
	    freearg = nxtarg;
	    argv[nxtarg++] = itoa(* (int*) action->data);
	    argc++;
	    break;
	  }

	if (action->argc > 0)
	    BCOPY(action->argv, &argv[nxtarg], action->argc*sizeof(char*));

#ifdef COMMENT
printf("CallScriptActionFunc:\n");
{
    int	i;

    for (i = 0; i < argc; i++)
	printf("%s ", argv[i]);
    printf("\n");
}
#endif
	if (!push_element(WorkingElement()))
	  {
	    if (freearg != 0)
		free(argv[freearg]);

	    yyerror("Can't call script action: Can't push onto element stack");
	    /* No return */
	  }
	SetWorkingElement(element);

	status = ExecuteFunction(argc, argv);

	element = pop_element();
	if (element == NULL)
	  {
	    if (freearg != 0)
		free(argv[freearg]);
	    yyerror("Element stack empty after script action");
	    /* No return */
	  }
	SetWorkingElement(element);

	if (freearg != 0)
	    free(argv[freearg]);
	free(argv);

	return status;

}	/* CallScriptActionFunc */



/*
** CallActionsBaseToSub
**
** FUNCTION
**	Recursive call to element create actions for each parent object.
**	Calls action functions from the base object out.
**
** ARGUMENTS
**	Element* elm		- element to call
**	GenesisObject* object	- object context in which to call
**	Action* action		- action call
**
** RETURN VALUE
**	int			- 1 on success, 0 if action fails
**
** AUTHOR
**	David Bilitch
*/

int CallActionsBaseToSub(elm, object, action)

Element*	elm;
GenesisObject*	object;
Action*		action;

{	/* CallActionsBaseToSub --- Call action for all derived objects */

	ActionList*	actionList;

	if (object == NULL)
	    return 1;

	/*
	** check to see if the  object has a create action
	*/
	actionList = GetActionListByType(object, action->type);
	if (actionList == NULL)
	    return 1;

	if (CallActionsBaseToSub(elm, SuperObject(actionList->object), action))
	    return CallActionByActionList(actionList, elm, action);

	return 0;

}	/* CallActionsBaseToSub */


/*
** CallActionsSubToBase
**
** FUNCTION
**	Recursive call to element create actions for each parent object.
**	Calls action functions from the sub object in to the base.
**
** ARGUMENTS
**	Element* elm		- element to call
**	GenesisObject* object	- object context in which to call
**	Action* action		- action call
**
** RETURN VALUE
**	int			- 1 on success, 0 if action fails
**
** AUTHOR
**	David Bilitch
*/

int CallActionsSubToBase(elm, object, action)

Element*	elm;
GenesisObject*	object;
Action*		action;

{	/* CallActionsSubToBase --- Call action for all derived objects */

	ActionList*	actionList;

	if (object == NULL)
	    return 1;

	/*
	** check to see if the  object has a create action
	*/
	actionList = GetActionListByType(object, action->type);
	if (actionList == NULL)
	    return 1;

	if (CallActionByActionList(actionList, elm, action))
	    return CallActionsSubToBase(elm,
				    SuperObject(actionList->object), action);

	return 0;

}	/* CallActionsSubToBase */


int CallActionByActionList(actionlist, element, action)

ActionList*	actionlist;
Element*	element;
Action*		action;

{	/* CallActionByActionList --- Call action function and return status */

	Element*	saveElement;
	GenesisObject*	saveObject;
	int		saveAction;
	int		status;

	saveElement = ActiveElement;
	saveObject = ActiveObject;
	saveAction = ActiveAction;

	ActiveElement = element;
	ActiveObject = actionlist->object;
	ActiveAction = action->type;

	if (actionlist->scriptfunc)
	    status = CallScriptActionFunc((char*) actionlist->function, ActiveElement, action);
	else
	    status = actionlist->function(element, action);

	ActiveElement = saveElement;
	ActiveObject = saveObject;
	ActiveAction = saveAction;

	return status;

}	/* CallActionByActionList */



int CallActionFunc(element, action)

Element*	element;
Action*		action;

{	/* CallActionFunc --- Call the action function and return status */

	ActionList*	actionlist;
	GenesisObject*	object;

	if (ActiveElement == element && ActiveObject != NULL)
	    object = ActiveObject;
	else
	    object = element->object;

	actionlist = GetActionListByType(object, action->type);
	if (actionlist == NULL)
	    return -1;

	action->name = actionlist->name;
	return CallActionByActionList(actionlist, element, action);

}	/* CallActionFunc */


/*
** CallEventAction
** CallEventActionWithTime
**
** FUNCTION
**
**	Sends an event to an element by calling the element EVENT action.
**	The action data is the msg structure associated with the action
**	and the action argv is a pointer to a double with the simulation
**	time for the event.  CallEventAction uses the current simulation
**	time as the event time, while CallEventActionWithTime uses the
**	evtime parameter as the event time.
**
** ARGUMENTS
**
**	Element* src	- The source element sending the event.
**	Msg*	 msg	- The Msg on the source referencing the target
**			  element.
**	double	 evtime	- (CallEventActionWithTime only) time at which
**			  the event occured.
**
** RETURN VALUE
**	int		- The return value from the action function.
**			  or -1 for non-active messages
**
** AUTHOR
**	David Bilitch
*/

int CallEventActionWithTime(src, msg, evtime)

Element*	src;
Msg*		msg;
double		evtime;

{	/* CallEventActionWithTime --- Send event from src to msg->dst */

	Action	action;

	/* By convention, negative message types are active */
	if (msg->type >= 0)
	    return -1;

	action.name = "EVENT";
	action.type = EVENT;
	action.data = (char*) msg;
	action.argc = 0;
	action.argv = (char**) &evtime;

	return CallActionFunc(msg->dst, &action);

}	/* CallEventActionWithTime */


int CallEventAction(src, msg)

Element*	src;
Msg*		msg;

{	/* CallEventAction --- Send event from src to msg->dst */

	double	evtime;

	evtime = SimulationTime();
	return CallEventActionWithTime(src, msg, evtime);

}	/* CallEventAction */


void SimExecuteFunction(argc, argv)

int	argc;
char*	argv[];

{	/* SimExecuteFunction --- Save action state and call function from
				  outside of action scope */

	Element*	saveElm;
	GenesisObject*	saveObj;
	int		saveAct;

	saveElm = ActiveElement;
	saveObj = ActiveObject;
	saveAct = ActiveAction;

	ActiveElement = NULL;
	ActiveObject = NULL;
	ActiveAction = -1;
	ExecuteFunction(argc, argv);

	ActiveElement = saveElm;
	ActiveObject = saveObj;
	ActiveAction = saveAct;

}	/* SimExecuteFunction */
