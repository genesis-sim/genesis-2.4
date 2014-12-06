static char rcsid[] = "$Id: sim_call.c,v 1.3 2005/07/01 10:03:09 svitak Exp $";

/*
** $Log: sim_call.c,v $
** Revision 1.3  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:58  svitak
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
** Revision 1.7  1997/07/24 00:21:31  dhb
** Fix from PSC:
** CallElement ==> CallElementMode in do_call_element().
**
** Revision 1.6  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5  1996/10/07 21:18:04  dhb
** Call mode param to CallElement() removed which returns CallElement()
** to its pre-2.0 state.  CallElementWithMode() adds the call mode
** param.  In addition, CallElementParentObject() and
** CallElementVirtualObject() have been added as external interface
** functions to allow these types of calls without exporting call
** modes.
**
 * Revision 1.4  1993/12/23  02:21:35  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/02/17  23:17:40  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_call_element (call) not changed, since it take arguments based on
 * 	what element is called.
 *
 * Fixed rcsid variable typing.
 *
 * Revision 1.1  1992/10/27  19:39:11  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define	CALLMODE_NORMAL		0
#define	CALLMODE_PARENT		1
#define	CALLMODE_VIRTUAL	2

int CallElement(element,action)
Element *element;
Action *action;
{
    return CallElementWithMode(element, action, CALLMODE_NORMAL);
}

int CallElementParentObject(element,action)
Element *element;
Action *action;
{
    return CallElementWithMode(element, action, CALLMODE_PARENT);
}

int CallElementVirtualObject(element,action)
Element *element;
Action *action;
{
    return CallElementWithMode(element, action, CALLMODE_VIRTUAL);
}

int CallElementWithMode(element,action,callmode)
Element *element;
Action *action;
int callmode;
{
GenesisObject*	actobject;
ActionList*	actionList;

    if(element == NULL || action == NULL){
	return(0);
    }

    actobject = element->object;
    if (ActiveObject != NULL && ActiveElement == element) {
	if (callmode == CALLMODE_NORMAL)
	    actobject = ActiveObject;
	else if (callmode == CALLMODE_PARENT)
	    actobject = SuperObject(ActiveObject) == NULL ? ActiveObject :
						    SuperObject(ActiveObject);
    }

    /*
    ** call the element function with the desired action
    */

    actionList = GetActionListByType(actobject, action->type);
    if (actionList == NULL)
	return 0;

    CallActionByActionList(actionList, element, action);

    return(1);
}

char *do_call_element(argc,argv)
int	argc;
char	**argv;
{
Element	*element;
ElementList *list;
Action	*action;
int	callMode;
int	status;
int	i;

    callMode = CALLMODE_NORMAL;
    initopt(argc, argv, "path action [args...] -parent -virtual");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-parent") == 0)
	    callMode = CALLMODE_PARENT;
	else if (strcmp(G_optopt, "-virtual") == 0)
	    callMode = CALLMODE_VIRTUAL;
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return NULL;
      }

    if((action = GetAction(optargv[2])) == NULL){
	Error();
	printf("%s : could not find action '%s'.\n",argv[0],argv[2]);
	return(NULL);
    }
    action->argc = optargc-3;
    action->argv = optargv+3;
    action->passback = NULL;
    list = WildcardGetElement(optargv[1],1);
    if(list->nelements <= 0){
	Error();
	printf("%s : invalid path '%s'.\n",optargv[0],optargv[1]);
	return(NULL);
    }
    for(i=0;i<list->nelements;i++){
	element = list->element[i];
	if(!CallElementWithMode(element,action,callMode)){
	    Error();
	    printf("could not call '%s' with action '%s'\n",
	    Pathname(element),
	    optargv[2]);
	    return(NULL);
	}
    }
    OK();
    return(CopyString(action->passback));
}
