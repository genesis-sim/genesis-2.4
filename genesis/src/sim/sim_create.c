static char rcsid[] = "$Id: sim_create.c,v 1.2 2005/06/27 19:00:58 svitak Exp $";

/*
** $Log: sim_create.c,v $
** Revision 1.2  2005/06/27 19:00:58  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.16  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.15  2001/04/01 19:31:21  mhucka
** Small cleanup fixes involving variable inits.
**
** Revision 1.14  1996/09/13 01:03:00  dhb
** Added valid_index handling to GetTreeCount().
**
 * Revision 1.13  1996/05/16  21:59:55  dhb
 * Support for element path hash table.
 *
 * Revision 1.12  1995/02/22  18:59:03  dhb
 * Bug in -autoindex processing in do_create() fixed.
 *
 * Revision 1.11  1994/10/21  20:26:08  dhb
 * Fixed some bugs in the CreateAction() changes.
 *
 * Revision 1.10  1994/10/21  00:10:56  dhb
 * Illegal pointer compination warnings.
 *
 * Revision 1.9  1994/10/20  23:57:05  dhb
 * Took CREATE action code out of Create() function and made a new
 * CreateAction() which is also used from CreateMap().
 *
 * Revision 1.8  1994/10/20  21:48:06  dhb
 * Removed call to CopyConnections() from Create().  Copying connections
 * will now be done in the COPY and CREATE actions of projections (now in
 * the connection compatability library).  Now we pass a pointer to the
 * object defaults value in the passback field of the CREATE action, as
 * the projection will need access to the element from which connections
 * are copied (in this case, the object defaults).
 *
 * Revision 1.7  1994/08/08  22:00:54  dhb
 * Changes from Upi.
 *
 * Revision 1.7  1994/06/13  22:32:11  bhalla
 * minor bugfix for empty arg case
 *
 * Revision 1.6  1994/06/03  21:36:31  dhb
 * Autoindex stuff from Upi Bhalla.
 *
 * Revision 1.5.1.1  1994/04/13  19:06:56  bhalla
 * Added autoindex facility to the create command
 *
 * Revision 1.5  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.4  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.3  1993/03/08  18:26:29  dhb
 * Added default element field values.
 *
 * Revision 1.2  1993/02/16  15:42:36  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_create (create) changed to remove buzz words (e.g. create a neutral
 * 	called /lkj ==> create neutral /lkj).  do_create doesn't take any
 * 	options.  Options processing is handled in CREATE action for each
 * 	object.
 *
 * Revision 1.1  1992/10/27  19:47:38  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"


int CreateAction(newelm, defelm, action)

Element*	newelm;
Element*	defelm;
Action*		action;

{	/* CreateAction --- Call CREATE action for the new element,
	** including any elements under
	** the new element in the case of an extended object.
	*/

	ElementStack*	stk;
	ElementStack*	defstk;
	Element 	*createElm;
	Element 	*defElm;
	Action		defaction;
	char		*defargv[2];
	int		status;
	int		createFailed;

	defaction.type = CREATE;
	defaction.name = "CREATE";
	defaction.argv = defargv;
	defaction.argc = 2;

	createFailed = 0;
	stk = NewPutElementStack(newelm);
	if (defelm == NULL)
	    defstk = NULL;
	else
	    defstk = NewPutElementStack(defelm);

	createElm = newelm;
	defElm = defelm;
	if (action != NULL)
	  {
	    action->passback = (char*) defElm;
	    status = CallActionsBaseToSub(createElm, createElm->object, action);

	    if (!status)
		createFailed = 1;
	    createElm = NewFastNextElement(2, stk);
	    if (defstk != NULL)
		defElm = NewFastNextElement(2, defstk);
	  }

	while (createElm != NULL)
	  {
	    defargv[0] = createElm->object->name;
	    defargv[1] = CopyString(Pathname(createElm));
	    defaction.data = (char*) createElm->parent;
	    defaction.passback = (char*) defElm;
	    status = CallActionsBaseToSub(createElm, createElm->object, &defaction);
	    free(defargv[1]);

	    if (!status)
		createFailed = 1;

	    createElm = NewFastNextElement(2, stk);
	    if (defstk != NULL)
		defElm = NewFastNextElement(2, defstk);
	  }
	NewFreeElementStack(stk);
	if (defstk != NULL)
	    NewFreeElementStack(defstk);

	return createFailed;

}	/* CreateAction */


Element *Create(object_name,name,parent,action,index)
char	*object_name;
char 	*name;
Element	*parent;
Action	*action;
int	index;
{
Element 	*element;
GenesisObject 	*object;
int		size;

    /*
    ** find the object type in the object table
    */
    if((object = GetObject(object_name)) != NULL){
	/*
	** create an element using the object information
	*/
	if (object->defaults != NULL)
	  {
	    Element 	*child;

	    if((element = CopyElementTree(object->defaults)) == NULL){
		printf("could not make a copy of defaults\n");
		return NULL;
	    }

	    /*
	    ** go through each child and make it a component of the
	    ** root element of the object.
	    */
	    child = element->child;
	    while (child != NULL)
	      {
		child->componentof = element;
		child = child->next;
	      }

	    /*
	    ** copy the msgs between elements
	    */
	    CopyMsgs(object->defaults,element);
	  }
	else
	    if((size = object->size) > 0){
		element = (Element *)calloc(1,size);
	    } else {
		Error();
		printf("zero sized object '%s'\n", object_name);
		return(NULL);
	    }
    } else {
	Error();
	printf("could not find object '%s'\n", object_name);
	return(NULL);
    }
    element->object  = object;
    /*
    ** name the element
    */
    if(!Name(element,name)){
	Error();
	printf("could not assign element name.\n");
	return(NULL);
    }
    element->index = index;
    /*
    ** and attach it to the parent
    */
    Attach(parent,element);

    if (CreateAction(element, object->defaults, action))
      {
	/* 
	** Reject the element
	*/
	DeleteElement(element);
	return(NULL);
      }

    return(element);
}

char *CreateObject(object,action)
GenesisObject 	*object;
Action	*action;
{
char 		*instance;
int		size;
Action		defaction;
PFI		function;

    if(object == NULL) return(NULL);
    /*
    ** create an element using the object information
    */
    if((size = object->size) > 0){
	instance = (char *)calloc(1,size);
    } else {
	Error();
	printf("zero sized object'%s'\n", object->name);
	return(NULL);
    }
    /*
    ** check to see if the  object has a create action
    */
    if ((function = GetActionFunc(object,CREATE,NULL,NULL))) {
	/*
	** if it does then call the function with the action
	*/
	if(action){
	    function(instance,action);
	} else {
	    defaction.type = CREATE;
	    defaction.name = "CREATE";
	    function(instance,&defaction);
	}
    }
    return(instance);
}

void do_create(argc,argv)
int	argc;
char	**argv;
{
char 		*name;
char 		*type;
char 		*parent_name;
char 		*target;
Element		*new_element;
Element		*parent_element;
Action		action;
int		index;
int		valid_index;
char		*ptr;
char          **targv = NULL;
int                   autoindex = 0;
int                   i,j = 0;

    if(argc < 3){
	fprintf(stderr,"%s: too few command arguments\n", argv[0]);
	fprintf(stderr, "usage: %s object name -autoindex [object-specific-options]\n", argv[0]);
	return;
    }

    /*
    ** parse the command line arguments
    */

    type = argv[1];
    target = argv[2];

    if(strlen(parent_name = GetParentComponent(target)) == 0){
	parent_name = ".";
	name = target;
    } else {
	name = GetBaseComponent(target);
    }
    index = GetTreeCount(name, &valid_index);
    if (!valid_index){
	Error();
	printf("missing or bad element index in create path '%s'\n", target);
	return;
    }
    if ((ptr = strchr(name,'['))) {
	*ptr = '\0';
    }
    if((parent_element = (Element *)GetElement(parent_name)) == NULL){
	Error();
	printf("cannot find '%s'\n",parent_name);
	printf("unable to create '%s'\n",name);
	return;
    }
    /* here we check for autoindex */
    if (argc > 3) {
	for(j = 3; j < argc; j++) {
	    if (strcmp(argv[j],"-autoindex") == 0) {
		autoindex = 1;
		break;
	    }
	}
    }
    if (autoindex) {
	targv = (char **)calloc(argc-1,sizeof(char *));
	index = FindFreeIndex(parent_element,name);
	argc--;
	for(i = 0; i < argc; i++) {
	    if (i < j) {
		    targv[i] = argv[i];
	    } else {
		    targv[i] = argv[i+1];
	    }
	}
	action.argv = targv + 1;
    } else {
	action.argv = argv + 1;
    }
    /*
    ** create the element
    */
    action.argc = argc - 1;
    action.type = CREATE;
    action.name = "CREATE";
	action.data = (char *)parent_element;
    if ((new_element = Create(type,name,parent_element,&action,index))) {
	/* add the new element to the element hash table */
	ElementHashPutTree(new_element);

	SetRecentElement(new_element);
    } else {
	printf("unable to create '%s'\n",name);
	return;
    }
    if (autoindex)
      free(targv);

    OK();
}

