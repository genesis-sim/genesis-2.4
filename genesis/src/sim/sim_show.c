static char rcsid[] = "$Id: sim_show.c,v 1.2 2005/06/27 19:01:09 svitak Exp $";

/*
** $Log: sim_show.c,v $
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
** Revision 1.14  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.13  1997/05/28 23:56:21  dhb
** Changes from Antwerp GENESIS 21e
**
 *
 * EDS21e revison: EDS BBF-UIA 97/04/22
 *  added IsGElement and updated  (so that x0,y0,z0 are not shown)
 *
 * Revision 1.12  1995/01/10  19:19:30  dhb
 * On some systems, printf() fails to handle a NULL pointer on a %s
 * conversion.  Changed ShowObject() to check func_name values and
 * the object->author field for NULL values.
 *
 * Revision 1.11  1994/10/21  01:00:16  dhb
 * do_show() was not freeing its element list when an error was encountered.
 *
 * Revision 1.10  1994/09/16  20:44:34  dhb
 * Removed connection related code.  This functionality will be provided
 * in connection specific commands in the connection compatability library.
 *
 * Revision 1.9  1994/06/04  00:11:11  dhb
 * Compilation problems in 1.8.
 *
 * Revision 1.8  1994/06/04  00:08:34  dhb
 * Changed options to showfield command to
 *      -all            Show all element specific fields
 *      -basic          Show only basic element field info
 *      -describe       Show element's object
 *
 * Revision 1.7  1993/10/15  18:34:26  dhb
 * Fixed bad pointer comparision.
 *
 * Revision 1.6  1993/03/18  17:15:55  dhb
 * Element field protection.
 *
 * Revision 1.5  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.4  1993/03/08  18:26:29  dhb
 * Added default element field values.
 *
 * Revision 1.3  1993/02/16  15:18:49  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_show (show) changed to use GENESIS getopt routines.  Added -all
 * 	-basic and -specific options to replace **, *** and * respectively.
 * 	The **, *** and * forms are still handled however.
 *
 * Revision 1.2  1992/12/18  21:12:23  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  20:27:55  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define field_is(S) (all || strcmp(field,"S") == 0 )

void ShowObject(object)
GenesisObject	*object;
{
int	i;
ActionList	*alist;
MsgList		*mlist;
ClassList	*clist;
char		*func_name;
int		special;

    if(object == NULL) return;
    func_name = GetFuncName(object->function);
    if (func_name == NULL)
	func_name = "(null)";
    printf("\n");
    printf("%-20s = %s\n","object",object->name);
    printf("%-20s = %s\n","datatype",object->type);
    printf("%-20s = %s()\n","function",func_name);
    printf("%-20s = ","class");
    for(clist=object->class;clist;clist=clist->next){
	printf("[ %s ] ", GetClassName(clist->id));
    }
    printf("\n");
    printf("%-20s = %d bytes\n","size",object->size);
    printf("%-20s = %s\n","author",object->author == NULL ? "(null)" :
							    object->author);
    special = 0;
    if(object->valid_actions){
	printf("\nVALID ACTIONS\n\t");
	for(alist=object->valid_actions;alist;alist=alist->next){
	    if(alist->function != object->function) special = 1;
	    printf("%s  ",alist->name);
	}
	printf("\n");
    }
    if(special){
	printf("\nSPECIAL FUNCTIONS\n");
	for(alist=object->valid_actions;alist;alist=alist->next){
	    if(alist->function != object->function){
		func_name = GetFuncName(alist->function);
		if (func_name == NULL)
		    func_name = "(null)";
		printf("\t%-20s %s()\n", alist->name, func_name);
	    } 
	}
    }
    if(object->valid_msgs){
	printf("\nVALID MESSAGES\n");
	for(mlist=object->valid_msgs;mlist;mlist=mlist->next){
	    printf("\t[%d] %-20s : ",mlist->type,mlist->name);
	    for(i=0;i<mlist->slots;i++){
		printf("%s ",mlist->slotname[i]);
	    }
	    printf("\n");
	}
    }
    DisplayFieldList(object);
    if(object->description != NULL){
	printf("\nDESCRIPTION\n");
	printf("\t%s",object->description);
    }
    printf("\n");
}

/* EDS modification: include GELement case */

void ShowSpecific(element)
Element		*element;
{
    if(element == NULL) return;
    /*
    ** display all of the fields after the last field of the basic
    ** element
    */
    if(element->object){
	if (IsGElement(element)) {
	    DisplayFields(element,element->object,
		element,element->object->type,"z0",1);
	    DisplayExtFields(element);
	} else {
	    DisplayFields(element,element->object,
		element,element->object->type,"next",1);
	    DisplayExtFields(element);
	}
    }
}

/* EDS modification: include GELement case */

void ShowBasic(element)
Element 	*element;
{
GElement 	*gelm;

    if(element == NULL) return;

    if (IsGElement(element)) {
	gelm=(GElement *)element;
	printf("%-10s = ( %e , %e , %e )\n",
	"x0y0z0", gelm->x0, gelm->y0, gelm->z0);
    }
    printf("%-10s = ( %e , %e , %e )\n",
    "xyz", element->x, element->y, element->z);
    printf("%-10s = %x\n","flags",element->flags);
    if(IsEnabled(element)){
	if(IsActive(element)){
	    printf("FUNCTIONAL\n");
	} else {
	    printf("NOT FUNCTIONAL\n");
	}
    } else {
	printf("BLOCKED\n");
    }
    printf("Clock [ %d ] = %e\n",Clock(element),Clockrate(element));

    printf("%d incoming messages\n", element->nmsgin);
    printf("%d outgoing messages\n", element->nmsgout);
    printf("-----------------------------------------------------\n");

}

void ShowAll(element)
Element 	*element;
{
    if(element == NULL) return;
    ShowObject(element->object);
    ShowBasic(element);
    ShowSpecific(element);
}

void do_show(argc,argv)
int	argc;
char	**argv;
{
Element 	*element = NULL;
int 		nxtarg;
char 		*field;
int		startarg;
ElementList	*list;
int		multiple;
int		ecount;
char		*pathname;
int		status;
int		basic;
int		all;
int		describe;

    all = 0;
    basic = 0;
    describe = 0;
    initopt(argc, argv, "[path[:connections]] [field] ... -all -basic -describe");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-all") == 0)
	    all = 1;
	else if (strcmp(G_optopt, "-basic") == 0)
	    basic = 1;
	else if (strcmp(G_optopt, "-describe") == 0)
	    describe = 1;
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if (optargc < 2)
	pathname = Pathname(WorkingElement());
    else
	pathname = optargv[1];

    nxtarg = 1;
    list = WildcardGetElement(pathname,0);

    /*
    ** if there are more than 2 arguments then assume that there is
    ** a path specification. Try opening it as an element.
    */
    if((ecount = list->nelements) > 0){
	multiple = 1;
	nxtarg++;
    } else {
	element = WorkingElement();
	if (optargc >= 2 && strcmp(optargv[nxtarg], "*") != 0 &&
		strcmp(optargv[nxtarg], "**") != 0 &&
		strcmp(optargv[nxtarg], "***") != 0 &&
		ElmFieldValue(element, optargv[nxtarg]) == NULL) {
	    printf("'%s' is not an element or the field of the working element\n", optargv[nxtarg]);
	    FreeElementList(list);
	    return;
	}

	ecount = 1;
	multiple = 0;
    }
    startarg = nxtarg;

    while(ecount>0){
	if(multiple){
	    element = list->element[list->nelements - ecount];
	}
	ecount--;
	nxtarg = startarg;
	    printf("\n[ %s ]\n",Pathname(element));

	/*
	** I assume ShowAll shows all fields so any setting of basic or
	** describe is redundant.  So, just show the fields once.
	*/

	if (all)
	  {
	    ShowBasic(element);
	    ShowSpecific(element);
	  }
	else
	  {
	    if (describe)
		ShowObject(element->object);
	    if (basic)
		ShowBasic(element);
	  }

	while(nxtarg < optargc){
	    field = optargv[nxtarg++];
		ShowField(element,field);
	}
    }

    FreeElementList(list);
}

void ShowField(element,field)
Element	*element;
char	*field;
{
    if(strcmp(field,"**") == 0){
	ShowAll(element);
    } else
    if(strcmp(field,"***") == 0){
	ShowObject(element->object);
	ShowBasic(element);
    } else 
    if(strcmp(field,"*") == 0){
	ShowSpecific(element);
    } else {
	char*	value;

	if (GetFieldListProt(element->object, field) != FIELD_HIDDEN)
	  {
	    value = GetExtField(element, field);
	    if (value != NULL)
		FieldFormat(field, value);
	    else
		DisplayField(element,element->object,field);
	  }
    }
}

/* 
 * IsGElement
 *
 * FUNCTION
 *	Returns true if element is a GELEMENT (has x0,y0,z0 fields), false
 *      for all other elements.
 *
 * ARGUMENTS
 *     element -- address of element
 *
 * RETURN VALUE
 *     int
 *
 * AUTHORS
 *	Erik De Schutter
 */

int IsGElement(elm)
	Element *elm;
{
	char*   oname;

	oname = BaseObject(elm)->name;
	if ((strcmp(oname,"compartment")==0) ||
	    (strcmp(oname,"symcompartment")==0)) return(1);
	else return(0);
}

