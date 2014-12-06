static char rcsid[] = "$Id: sim_class.c,v 1.2 2005/06/27 19:00:58 svitak Exp $";

/*
** $Log: sim_class.c,v $
** Revision 1.2  2005/06/27 19:00:58  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2001/04/01 19:31:21  mhucka
** Small cleanup fixes involving variable inits.
**
** Revision 1.7  1997/08/12 21:30:28  dhb
** Removed explicit declaration of malloc().  Was conflicting
** with the declration in stdlib.h.
**
** Revision 1.6  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5  1995/03/17 23:11:00  dhb
** changed do_list_classes() to use GENESIS getopt().
**
 * Revision 1.4  1993/12/23  02:25:36  dhb
 * Extended Objects (mostly complete)
 *
 * Removed extraneous backslash in DEFINED CLASS printf.
 *
 * Revision 1.3  1993/02/22  19:18:49  dhb
 * Fixed delcaration error for rcsid variable.
 *
 * Revision 1.2  1993/02/17  21:18:56  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_add_class (newclass) changed to use GENESIS getopt routines.
 *
 * 	do_list_classes (listclasses) takes no command arguments or options
 * 	so it was not changed.
 *
 * Revision 1.1  1992/10/27  19:42:28  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "hash.h"

HASH *class_hash_table;

int CheckClass(element,id)
Element *element;
int 	id;
{
ClassList	*class;

    /*
    ** search the class list of the element
    */
    for(class=element->object->class;class;class=class->next){
	if(class->id == id){
	    return(1);		/* found */
	}
    }
    return(0);			/* not found */
}


/*
** ClassListEqual
**
** FUNCTION
**	Compare two class lists to determine if they are the same list.
**
** ARGUMENTS
**	ClassList* list1	- Class lists to compare.
**	ClassList* list2
**
** RETURN VALUE
**	int		- 1 if the lists are the same, 0 if not.
**
** NOTES
**	This routine makes the assumption that the class list does not
**	allow duplicate ids in a list.  If the lists are the same length
**	then all ids in list1 are checked to see that they appear in list2.
**
** AUTHOR
**	David Bilitch
*/

int ClassListEqual(list1, list2)

ClassList*	list1;
ClassList*	list2;

{	/* ClassListEqual --- Return TRUE if the ClassLists are the same */

    ClassList*	c1;
    ClassList*	c2;

    c1 = list1;
    c2 = list2;
    while (c1 != NULL && c2 != NULL)
      {
	c1 = c1->next;
	c2 = c2->next;
      }

    if (c1 != c2)
	return 0;

    for (c1 = list1; c1 != NULL; c1 = c1->next)
      {
	for (c2 = list2; c2 != NULL; c2 = c2->next)
	  {
	    if(c1->id == c2->id)
		break;
	  }

	if (c2 == NULL)
	    return 0;
      }

    return 1;

}	/* ClassListEqual */


/*
** Hash table utilities
*/
void ClassHashInit()
{
HASH *hash_create();

    /*
    ** create the table
    */
    class_hash_table = hash_create(100);
}

int ClassHashPut(key,class)
char	*key;
int 	class;
{
ENTRY	item,*hash_enter();

    item.data = (char *)malloc(sizeof(int));
    *((int *)(item.data)) = class;
    item.key = CopyString(key);
    /*
    ** put the class into the table
    */
    if(hash_enter(&item,class_hash_table) == NULL){
	Error();
	printf("class hash table full\n");
	return(0);
    };
    return(1);
}

int ClassHashFind(key)
char	*key;
{
ENTRY	*found_item,*hash_find();

    /*
    ** get the class from the table
    */
    if((found_item = hash_find(key,class_hash_table)) != NULL){
	return(*((int *)(found_item->data)));
    } else
	return(INVALID_CLASS);
}

int ClassID(name)
char	*name;
{
int	id;

    /*
    ** look up the class name in the class id table
    */
    id = ClassHashFind(name);
    return(id);
}

char *GetClassName(id)
int	id;
{
extern HASH *class_hash_table;
char 	*name;
int 	i;

    for(i=0;i<class_hash_table->size;i++){
	if ((name = class_hash_table->entry[i].key)) {
	    if(id == *((int *)(class_hash_table->entry[i].data))){
		return(name);
	    }
	}
    }
    return("???");
}

void AddClass(name,class)
char 	*name;
int	class;
{
    ClassHashPut(name,class);
}


/*
** ClassIsPermanent
**
** FUNCTION
**	Return true if CLASS_PERMANENT is set in the class list flags.
**
** ARGUMENTS
**	GenesisObject* object	- object with class list to check
**	char* className		- name of class to check
**
** RETURN VALUE
**	int			- 1 if class is permanent, 0 if not
**
** AUTHOR
**	David Bilitch
*/

int ClassIsPermanent(object, className)

GenesisObject*	object;
char*		className;

{	/* ClassIsPermanent --- Return TRUE if class is permanent */

	ClassList*	cur;
	int		classId;

	classId = ClassID(className);
	if (classId == INVALID_CLASS)
	    return 0;

	for (cur = object->class; cur != NULL; cur = cur->next)
	  {
	    if (cur->id == classId)
		return (cur->flags&CLASS_PERMANENT) == CLASS_PERMANENT;
	  }

	return 0;

}	/* ClassIsPermanent */


/*
** ClassListMakePermanent
**
** FUNCTION
**	Sets the CLASS_PERMANENT flag for all the new classes in the
**	class list.
**
** ARGUMENTS
**	GenesisObject* object	- object with class list to make permanent
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch
*/

void ClassListMakePermanent(object)

GenesisObject*	object;

{	/* ClassListMakePermanent --- Make object's classes permanent */

	ClassList*	cur;

	cur = object->class;
	while (cur != NULL && (cur->flags&CLASS_PERMANENT) != CLASS_PERMANENT)
	  {
	    cur->flags |= CLASS_PERMANENT;
	    cur = cur->next;
	  }

}	/* ClassListMakePermanent */


int do_add_class(argc,argv)
int	argc;
char 	**argv;
{
Element*	elm;
char	*name;
char	*newclass;
int	id;
int	i;

    initopt(argc, argv, "[element] class-name");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return 0;
      }

    elm = NULL;
    if (optargc == 2)
	newclass = optargv[1];
    else
      {
	elm = GetElement(optargv[1]);
	if (elm == NULL)
	  {
	    fprintf(stderr, "%s: cannot find element '%s'\n",
		optargv[0], optargv[1]);
	    return 0;
	  }

	newclass = optargv[2];
      }

    if (optargc == 2 || ClassID(newclass) == INVALID_CLASS)
      {
	/*
	** find a new class by getting the max class id and adding one
	*/
	id = 0;
	for(i=0;i<class_hash_table->size;i++){
	    /*
	    ** is this a non-NULL class name
	    */
	    if ((name = class_hash_table->entry[i].key)) {
		/*
		** check the new class to see if it matches an existing
		** class
		*/
		if(strcmp(newclass,name) == 0){
		    /*
		    ** if it does then dont bother adding it
		    */
		    return 0;
		}
		/*
		** check its id against the largest found so far
		*/
		id = MAX(id,*((int *)(class_hash_table->entry[i].data)));
	    }
	}
	AddClass(newclass,id+1);
      }

    if (elm != NULL)
      {
	GenesisObject*	object;

	object = ObjectModify(elm->object);
	if (object == NULL)
	  {
	    fprintf(stderr, "%s: Cannot modify object; cannot add class '%s' to element '%s'\n",
		optargv[0], newclass, Pathname(elm));
	    return 0;
	  }

	ObjectAddClass(object, ClassID(newclass), 0);

	elm->object = ObjectCacheCheck(object);
      }

    OK();
    return 1;
}


/*
** do_delete_class
**
** FUNCTION
**	Delete a class from an element.
**
** ARGUMENTS
**	int argc	- Number of command arguments
**	char** argv	- Command arguments
**
** RETURN VALUE
**	int		- 1 on success, 0 if element or class doesn't exist
**			  or the class is permanent
**
** NOTES
**	Since the class list is shared with other objects, all the classes
**	in the list must be copied up to the one to be removed.  The removed
**	ClassList structure is not freed as it still apprears in other object
**	class lists.
**
** AUTHOR
**	David Bilitch
*/

int do_delete_class(argc, argv)

int	argc;
char**	argv;

{	/* do_delete_class --- Delete class from an element */

	GenesisObject*	object;
	Element*	elm;
	ClassList*	cur;
	ClassList*	copy;
	ClassList*	curcopy = NULL;
	char*		className;
	int		classId;

	initopt(argc, argv, "element class-name");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	elm = GetElement(optargv[1]);
	if (elm == NULL)
	  {
	    fprintf(stderr, "%s: cannot find element '%s'\n",
		optargv[0], optargv[1]);
	    return 0;
	  }
	className = optargv[2];

	classId =  ClassID(className);
	if (classId == INVALID_CLASS)
	  {
	    fprintf(stderr, "%s: invalid class\n", optargv[0]);
	    return 0;
	  }

	if (ClassIsPermanent(elm->object, className))
	  {
	    fprintf(stderr, "%s: cannot delete permanent class '%s' from element '%s'\n",
			optargv[0], className, Pathname(elm));
	    return 0;
	  }

	copy = NULL;
	for (cur = elm->object->class; cur != NULL; cur = cur->next)
	  {
	    ClassList*	new;

	    if (cur->id == classId)
		break;

	    new = (ClassList*) malloc(sizeof(ClassList));
	    if (new == NULL)
	      {
		perror("do_delete_class");
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

	    fprintf(stderr, "%s: class '%s' not found on element '%s'\n",
		    optargv[0], className, Pathname(elm));
	    return 0;
	  }

	if (copy == NULL)
	    copy = cur->next;
	else
	    curcopy->next = cur->next;

	object = ObjectModify(elm->object);
	object->class = copy;
	elm->object = ObjectCacheCheck(object);

	OK();
	return 1;

}	/* do_delete_class */



void BasicClasses()
{
    AddClass(	"element",	ELEMENT_ELEMENT		);
    AddClass(	"segment",	SEGMENT_ELEMENT		);
    AddClass(	"buffer",	BUFFER_ELEMENT		);
    AddClass(	"projection",	PROJECTION_ELEMENT	);
    AddClass(	"connection",	CONNECTION_ELEMENT	);
}

void do_list_classes(argc, argv)

int	argc;
char*	argv[];

{
extern HASH *class_hash_table;
char 	*name;
int	cnt=0;
int 	i;

    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    printf("DEFINED CLASSES:\n");
    for(i=0;i<class_hash_table->size;i++){
	if ((name = class_hash_table->entry[i].key)) {
	    printf("%-12s", name);
	    if(((++cnt)%4) == 0){
		printf("\n");
	    }
	}
    }
    printf("\n\n");
}
