static char rcsid[] = "$Id: sim_set.c,v 1.2 2005/06/27 19:01:09 svitak Exp $";

/*
** $Log: sim_set.c,v $
** Revision 1.2  2005/06/27 19:01:09  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.15  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.14  1997/07/18 19:59:10  dhb
** Merged in 1.12.1.1 changes
**
** Revision 1.13  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.12.1.1  1997/07/18 16:05:59  dhb
** Changes from PSC: -empty_ok option for setfield command.
**
** Revision 1.12  1995/07/08 02:58:32  dhb
** SetElement() function now uses ActiveElement and ActiveObject
** context and CallSetActions().
**
 * Revision 1.11  1994/10/21  00:26:43  dhb
 * do_set() was neglecting to free some things when errors were detected.
 *
 * Revision 1.10  1994/09/16  23:39:25  dhb
 * Fix on previous changes.
 *
 * Revision 1.9  1994/09/16  20:44:34  dhb
 * Removed connection related code.  This functionality will be provided
 * in connection specific commands in the connection compatability library.
 *
 * Revision 1.8  1994/08/31  02:52:17  dhb
 * Added FieldNameProper() which returns the fieldname part of a complex
 * addressing in setfield (e.g. FieldNameProper("X->table[0]") returns "X").
 * The return value is in static memory!
 *
 * Changed do_set() and SetElement() to use FieldNameProper() on field name
 * to determine proper field protection.
 *
 * Revision 1.7  1994/06/03  21:51:12  dhb
 * Change from Upi Bhalla to delay field protection checking in
 * SetElement() until after the SET action is called.
 *
 * Revision 1.6  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.5  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.4  1993/03/18  17:15:55  dhb
 * Element field protection.
 *
 * Revision 1.3  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.2  1993/02/15  23:36:23  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_set (set) changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:27:28  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <ctype.h>
#include "shell_func_ext.h"
#include "sim_ext.h"


/*
** CallSetActions
**
** FUNCTION
**	Special version of CallActionsSubToBase for the set command which
**	stops calling super object actions once in the action says that the
**	field is already set.
**
** ARGUMENTS
**	ActionList* actionList	- ActionList to execute
**	Element* elm		- Element to call action on
**	Action* action		- Action data
**
** RETURN VALUE
**	int			- 1 if field gets set by an action, 0 if not
**
** NOTES
**	Calling sequence differs from CallActionsSubToBase.
**
** AUTHOR
**	David Bilitch
*/

int CallSetActions(actionList, elm, action)

ActionList*	actionList;
Element*	elm;
Action*		action;

{	/* CallSetActions --- Call SET actions from sub to base objects */

	do
	  {
	    GenesisObject*	parent;

	    if (CallActionByActionList(actionList, elm, action))
		return 1;

	    parent = SuperObject(actionList->object);
	    actionList = parent == NULL ?
			    NULL : GetActionListByType(parent, action->type);
	  }
	while (actionList != NULL);

	return 0;

}	/* CallSetActions */


char* FieldNameProper(fieldSpec)

char*	fieldSpec;

{	/* FieldNameProper --- Return field name portion of setfield field spec */

	static char	fieldName[100];
	char*		specp;
	char*		namep;

	for (specp = fieldSpec; *specp != '\0'; specp++)
	    if (isalpha(*specp) || *specp == '_')
		break;


	namep = fieldName;
	while (isalnum(*specp) || *specp == '_')
	    *namep++ = *specp++;
	*namep = '\0';

	return fieldName;

}	/* FieldNameProper */


int do_set(argc,argv)
int 	argc;
char 	**argv;
{
Element	*element;
char 		*pathname;
int 		nxtarg;
int 		field_offset=0;
Info 		info;
char 		*field;
char 		*value;
ElementList	*list;
GenesisObject		*previous_object;
int		i;
int		j;
Action		action;
GenesisObject		*object;
int		already_set;
char 		*adr;
void*		savedopt;
void*		savopt();
int		status;
int		empty_ok = 0;

    initopt(argc, argv, "[path[:connection]] field value ... -empty_ok");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if(strcmp(G_optopt,"-empty_ok") == 0)
	  empty_ok = 1;
      }
    if (status < 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    nxtarg = 1;
    action.argv = (char **)malloc((optargc+3)*sizeof(char *));
    action.type = SET;
    action.name = "SET";
    action.argc = 2;
    /*
    ** check for a pathname
    */
    if(((optargc - nxtarg)%2) != 0){
	pathname = optargv[nxtarg];
	nxtarg++;
    } else {
	pathname = ".";
    }

    if((list = WildcardGetElement(pathname,0)) == NULL){
	InvalidPath(optargv[0],pathname);
	free(action.argv);
	return(0);
    }
    if (list->nelements < 1) {
        if (empty_ok)
	  {
	    FreeElementList(list);
	    free(action.argv);
	    OK();
	    return(1);
	  }
	else
	  {
	    InvalidPath(optargv[0],pathname);
	    FreeElementList(list);
	    free(action.argv);
	    return(0);
	  }
    }

    /*
    ** loop over all the fields to be set
    */
    while(nxtarg < optargc){
	/*
	** get the field to be set
	*/
	field = optargv[nxtarg++];
	if(nxtarg == optargc){
	    TraceScript();
	    printf("missing a value for field '%s'\n",field);
	    FreeElementList(list);
	    free(action.argv);
	    return(0);
	}
	/*
	** and its value
	*/
	value = optargv[nxtarg++];
	previous_object = NULL;
	/*
	** loop over all the elements in the list
	*/
	for(i=0;i<list->nelements;i++){
		ActionList*	actionList;
		int		prot;
	    /*
	    ** get the element
	    */
	    if((element = list->element[i]) == NULL) continue;

		if (element == ActiveElement && ActiveObject != NULL)
		  {
		    object = ActiveObject->super;
		    if (object == NULL || (prot = GetFieldListProt(object, FieldNameProper(field))) == -1)
		      {
			object = ActiveObject;
			prot = FIELD_READWRITE; /* all fields defined by this
						   object are READWRITE while
						   executing this objects
						   actions */
		      }
		  }
		else
		  {
		    object = element->object;
		    prot = GetFieldListProt(object, field);
		  }

		if (prot == FIELD_HIDDEN)
		  {
		    InvalidField(optargv[0],element,field);
		    continue;
		  }

		if (prot == FIELD_READONLY)
		  {
		    printf("%s: cannot set field '%s' of element '%s'\n",
				optargv[0], field, Pathname(element));
		    continue;
		  }

		/*
		** check to see if the  object has a SET action
		*/
		already_set = 0;
		actionList = GetActionListByType(object, SET);
		if(!(element == ActiveElement && object == ActiveObject &&
		     ActiveAction == SET) &&
		     actionList != NULL){
		    /*
		    ** does this object want all of the arguments
		    ** at once
		    */
		    if(GetExtField(object->defaults,"SPECIAL_SET")){
			/*
			** call the element with the rest of the argument
			** line. Then null the list element so that it is
			** not called again for subsequent arguments in the 
			** arglist
			*/
			action.argc = optargc - nxtarg + 2;
			for(j=0;j<action.argc;j++){
			    action.argv[j] = optargv[j+nxtarg-2];
			}
			list->element[i] = NULL;
			already_set = 1;
			/*
			** call the function with the SET action
			*/
			savedopt = savopt();
			CallActionsSubToBase(element, object, &action);
			restoropt(savedopt);
		    } else {
			action.argv[0] = field;
			action.argv[1] = value;
			action.argc = 2;
			/*
			** call the function with the SET action
			** and check its return status to determine
			** whether it actually set anything or not
			*/
			savedopt = savopt();
			already_set =
			    CallSetActions(actionList, element, &action);
			restoropt(savedopt);
		    }
		} 
		/*
		** do the default set
		*/
		if(!already_set){
		    /*
		    ** Try setting an extended field
		    */
		    if (SetExtField(element, field, value))
			continue;

		    /*
		    ** calculate the field offset if new object
		    ** is being used
		    */
		    if(element->object != previous_object){
			if((field_offset = 
			CalculateOffset(Type(element),field,&info)) == -1){
			    InvalidField(optargv[0],element,field);
			    continue;
			}
			previous_object = element->object;
		    }
		    if(field_offset == -2){ 
			/* 
			** have to compute the address
			** from scratch
			*/
			if((adr=
			CalculateAddress(element,Type(element),field,&info)) ==
			NULL){
			    InvalidField(optargv[0],element,field);
			    continue;
			}
		    } else {
			adr = (char *)((char *)(element)+field_offset);
		    }
		    /*
		    ** set the field value
		    */
		    PutValue(adr,&info,value);
		}
	}
    }
    /*
    ** done with the list
    */
    FreeElementList(list);
    free(action.argv);
    OK();
    return(1);
}

int SetElement(element,field,value)
Element		*element;
char		*field;
char		*value;
{
GenesisObject	*object;
Action		action;
ActionList*	actionList;
int 		field_offset;
Info 		info;
char*		adr;
int		already_set;
int		prot;

    if(element == NULL || field == NULL || value == NULL){
	return(0);
    } 

    if (element == ActiveElement && ActiveObject != NULL)
      {
	object = ActiveObject->super;
	if (object == NULL || (prot = GetFieldListProt(object, FieldNameProper(field))) == -1)
	  {
	    object = ActiveObject;
	    prot = FIELD_READWRITE; /* all fields defined by this
				       object are READWRITE while
				       executing this objects
				       actions */
	  }
      }
    else
      {
	object = element->object;
	prot = GetFieldListProt(object, field);
      }

    already_set = 0;

    /*
    ** Check to see if the object has a SET action
    */
    actionList = GetActionListByType(object, SET);
    if (actionList != NULL) {
	/*
	** if it does then call the function with the action
	*/

	action.name = "SET";
	action.type = SET;
	action.argv = (char **)malloc(5*sizeof(char *));
	action.argv[0] = field;
	action.argv[1] = value;
	action.argc = 2;

	already_set = CallSetActions(actionList, element, &action);

	free(action.argv);
    }

    if (!already_set){
	if (prot != FIELD_READWRITE)
	    return 0;

	/*
	** Try setting an extended field
	*/
	if (SetExtField(element, field, value))
	    return 1;

	if((field_offset = 
	CalculateOffset(Type(element),field,&info)) == -1){
	    InvalidField("SetElement",element,field);
	    return 0;
	}
	if(field_offset == -2){ 
	    /* 
	    ** have to compute the address
	    ** from scratch
	    */
	    if((adr=
	    CalculateAddress(element,Type(element),field,&info)) ==
	    NULL){
		InvalidField("SetElement",element,field);
		return 0;
	    }
	} else {
	    adr = (char *)((char *)(element)+field_offset);
	}
	/*
	** set the field value
	*/
	PutValue(adr,&info,value);
    }

    return(1);
}


/* 
** Upi addition : DirectSetElement sets the field values without
** referring to the SET action in the element 
*/
int DirectSetElement(element,field,value)
Element		*element;
char		*field;
char		*value;
{
int 		field_offset;
Info 		info;
char 		*adr;

    if(element == NULL || field == NULL || value == NULL){
	return(0);
    } 
    if((field_offset = CalculateOffset(Type(element),field,&info)) == -1){
    	InvalidField("DirectSetElement",element,field);
    	return(0);
    }
    if(field_offset == -2){
	/* 
	** have to compute the address
	** from scratch
	*/
	if((adr=
	CalculateAddress(element,Type(element),field,&info)) ==
	NULL){
	    InvalidField("DirectSetElement",element,field);
	    return(0);
	}
    } else {
	adr = (char *)((char *)(element)+field_offset);
    }
    /*
    ** set the field value
    */
    PutValue(adr,&info,value);

    return(1);
}


int SetField(address,type,field,value)
char *address;
char *type;
char *field;
char *value;
{
Info info;
int offset;

    /*
    ** is the address valid?
    */
    if(address == NULL){
	Error();
	printf("SetField : NULL address for field '%s'. Unable to set\n",
	field);
	return(0);
    }
    /*
    ** is the field valid ?
    */
    if(field){
	/*
	** get the offset of the field from the beginning of the structure
	** type
	*/
	offset = CalculateOffset(type,field,&info);
    } else {
	Error();
	printf("SetField : must give a field to set\n");
	return(0);
    }
    /*
    ** did the offset calculation succeed?
    */
    if(offset >= 0){
	/*
	** if it did then set the value and return the status
	** of the assignment operation
	*/
	return(PutValue(address+offset, &info,value));
    } else {
	Error();
	printf("SetField : could not make assignment to '%s'\n",field);
	return(0);
    }
}

/* LATER get the event setting function right */
void do_set_event(argc,argv)
int argc;
char **argv;
{
Element 	*element;
char 		*pathname;
char 		*type;
int		size = 0;

    if(argc < 3){
	printf("usage: %s buffer_name event_object [-size #]\n",argv[0]);
	return;
    }
    /*
    ** get arguments
    */
    pathname = argv[1];
    size = atoi(argv[2]);
    type = argv[3];

    element = GetElement(pathname);

    /*
    ** check arguments
    */
    if(element == NULL){
	TraceScript();
	printf("%s: buffer is undefined\n",argv[0]);
	return;
    }
    if(!CheckClass(element,BUFFER_ELEMENT)){
	TraceScript();
	printf("%s: '%s' is not a buffer element\n",argv[0],pathname);
	return;
    }
    if(size <= 0){
	TraceScript();
	printf("%s: invalid buffer size\n",argv[0]);
	return;
    }
    /*
    ** add the event buffer
    */
    AddEventBuffer(element,size,type);
}
