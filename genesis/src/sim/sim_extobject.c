static char rcsid[] = "$Id: sim_extobject.c,v 1.3 2005/07/01 10:03:09 svitak Exp $";

/*
** $Log: sim_extobject.c,v $
** Revision 1.3  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:01:00  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.10  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.9  1996/05/16 22:03:31  dhb
** Support for element path hash table.
**
 * Revision 1.8  1995/02/16  17:57:40  dhb
 * The addobject command wasn't setting the object->defaults->object.
 *
 * Revision 1.7  1995/01/24  20:33:33  dhb
 * Now allocates object cache using malloc() rather than smalloc().
 *
 * Revision 1.6  1994/09/02  21:31:47  dhb
 * Fixed initopt() string in do_add_extobject() to allow multiple arguments
 * to the -description option.
 *
 * Changed order of isa command arguments from "isa element object" to
 * "isa object element".
 *
 * Revision 1.5  1994/09/02  04:02:33  dhb
 * Changed do_add_object() to create a modified object if the object
 * the extended object is created from has not been modified already.
 * This allows one to make an extended object that is an exact duplicate
 * of the original, except that it has an ancestor object.
 *
 * Revision 1.4  1994/06/07  21:40:44  dhb
 * Merged in 1.2.1.1 changes.
 *
<<<<<<< 1.3
 * Revision 1.3  1994/05/27  22:39:22  dhb
 * Added do_isa() for the isa command.
 *
=======
 * Revision 1.2.1.1  1994/06/07  21:35:39  dhb
 * These are the 1.3 changes plus changes to do_addextobject() to make a
 * copy of the element's object in creating the new object rather than just
 * using it directly.  A related change in the object cache comparison code
 * requires that the defaults fields match (e.g. the two objects have the
 * same defaults element).
 *
>>>>>>> 1.2.1.1
 * Revision 1.2  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.1  1993/06/29  18:53:23  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

typedef struct _cacheEntry
  {
    struct _cacheEntry*	next;
    GenesisObject*	object;
  } CacheEntry;

static CacheEntry*	CacheList = NULL;


/*
** BaseObject
**
** FUNCTION
**	Return the base object type of the object which is the last object
**	in the object->super linked list.
**
** ARGUMENTS
**	Element* elm	- Element for which to find base object.
**
** RETURN VALUE
**	GenesisObject*	- Builtin object which is the base object of the
**			  element.
**
** AUTHOR
**	David Bilitch
*/

GenesisObject* BaseObject(elm)

Element*	elm;

{	/* BaseObject --- Return base object type of the element */

	GenesisObject*	object;

	object = elm->object;
	if (object != NULL)
	    while (object->super != NULL)
		object = object->super;

	return object;

}	/* BaseObject */


/*
** ElementIsA
**
** FUNCTION
**	Returns TRUE if the element is created from the given object or
**	a decendent thereof.
**
** ARGUMENTS
**	Element* elm	- An element to test IsA on.
**	char* objname	- The object name to check for.
**
** RETURN VALUE
**	int		- 1 if the element is of the give type, 0 otherwise.
**
** AUTHOR
**	David Bilitch
*/

int ElementIsA(elm, objname)

Element*	elm;
char*		objname;

{	/* ElementIsA --- */

	GenesisObject*	targetObject;
	GenesisObject*	object;
	GenesisObject*  ObjectHashFind();

	targetObject = (GenesisObject*) ObjectHashFind(objname);

	object = elm->object;
	while (object != NULL)
	  {
	    if (object == targetObject)
		return 1;

	    object = object->super;
	  }

	return 0;

}	/* ElementIsA */


/*
** ObjectFree
**
** FUNCTION
**	Free the memory associated with an object.
**
** ARGUMENTS
**	GenesisObject* object	- The object to free.
**
** RETURN VALUE
**	None.
**
** NOTES
**	The only memory which we can safely free is the fieldlist and
**	the object itself.  We may leak memory is class, action and
**	msg lists.
**
** AUTHOR
**	David Bilitch
*/

void ObjectFree(object)

GenesisObject*	object;

{	/* ObjectFree --- */

	FieldListFree(object);
	free(object);

}	/* ObjectFree */


/*
** ObjectCacheEnter
**
** FUNCTION
**	Enter an object into the object cache.  The object cache is the set of
**	all extended objects with differing actions, classes, messages, fields
**	super object or documenation.  The cache is a means of sharing objects
**	with identical values which are created in the process of making an
**	extended object.
**
** ARGUMENTS
**	GenesisObject* object	- The object to enter.
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch
*/

void ObjectCacheEnter(object)

GenesisObject*	object;

{	/* ObjectCacheEnter --- Enter the object into the object cache */

	CacheEntry	*ce;

	ce = (CacheEntry*) malloc(sizeof(CacheEntry));
	if (ce == NULL)
	  {
	    fprintf(stderr, "ObjectCacheEnter: out of memory\n");
	    return;
	  }

	ce->object = object;
	ce->next = CacheList;
	CacheList = ce;

}	/* ObjectCacheEnter */


/*
** ObjectCacheFind
**
** FUNCTION
**	Find an object in the object cache which matches the given object.
**	The comparison includes: 
**	valid_actions, fieldlist, class, valid_msgs, description, author,
**	defaults and super.
**
** ARGUMENTS
**	GenesisObject* object	- The object to find a match for.
**
** RETURN VALUE
**	GenesisObject*		- The object in the cache which matches or
**				  NULL if none match.
**
** AUTHOR
**	David Bilitch
*/

#define StrsEqual(s1, s2) \
	((s1) == (s2) || ((s1) != NULL && (s2) != NULL && strcmp(s1, s2) == 0))

GenesisObject* ObjectCacheFind(object)

GenesisObject*	object;

{	/* ObjectCacheFind --- Search the object cache for a match */

	CacheEntry	*ce;

	ce = CacheList;
	while (ce != NULL)
	  {
	    if (ce->object->super == object->super &&
		ce->object->defaults == object->defaults &&
		StrsEqual(ce->object->description, object->description) &&
		StrsEqual(ce->object->author, object->author) &&
		ClassListEqual(ce->object->class, object->class) &&
		ActionListEqual(ce->object->valid_actions, object->valid_actions) &&
		FieldListEqual(ce->object->fieldlist, object->fieldlist) &&
		MsgListEqual(ce->object->valid_msgs, object->valid_msgs))
	      {
		return ce->object;
	      }

	    ce = ce->next;
	  }

	return NULL;

}	/* ObjectCacheFind */

#undef StrsEqual


/*
** ObjectCacheCheck
**
** FUNCTION
**	Checks the cache of objects for an identical object (by contents).
**	The comparison includes: 
**	valid_actions, fieldlist, class, valid_msgs, description, author
**	and super.  If a match is found in the cache then the object found
**	is returned and the object given is freed.  Otherwise the object
**	given is entered into the cache and returned back to the caller.
**
**	The comparison for object equality now includes the object defaults
**	element.
**
** ARGUMENTS
**	GenesisObject*	object	- object to match in the cache
**
** RETURNS
**	GenesisObject* 		- Returns a previously cached object which
**				  matched the argument or the argument if
**				  a match is not found.
**
** SIDE EFFECTS
**	Object is entered into the object cache if a match is not found.
**	Object is freed if a match was found.
**
** NOTES
**	The object cache is distinct from the object hash table which
**	contains only namable objects.  The object cache contains all
**	distinct extended objects.
**
** AUTHOR
**	Dave Bilitch
*/

GenesisObject* ObjectCacheCheck(object)

GenesisObject*	object;

{	/* ObjectCacheCheck --- Check cache for match to object arg */

	GenesisObject*	cacheobject;

	cacheobject = ObjectCacheFind(object);
	if (cacheobject != NULL)
	  {
	    if (object != cacheobject)
		ObjectFree(object);

	    return cacheobject;
	  }

	/*
	** object does not match any object in the cache.  Enter object
	** into the cache and return the object.
	*/

	ObjectCacheEnter(object);
	return object;

}	/* ObjectCacheCheck */


/*
** ObjectModify
**
** FUNCTION
**	Makes a modified copy of the given object.  This entails allocation of
**	a new object, copying the object data and making a separate copy of
**	the object FieldList.  The modified object flag is set in the new
**	object flags field.
**
** ARGUMENTS
**	GenesisObject*	object	- object to duplicate
**
** RETURN Value
**	GenesisObject* 		- Returns the duplicate of the original
**				  object or NULL if out of memory.
**
** SIDE EFFECTS
**	The first modification of an object results in a modified object
**	which is derived from the original object.  Subsequent modifications
**	of the object continue to have the original object as its super
**	object.
**
** NOTES
**	The new object is not entered as a separate object in the object hash
**	table or entered into the object cache.
**
** AUTHOR
**	Dave Bilitch
*/

GenesisObject* ObjectModify(object)

GenesisObject*	object;

{	/* ObjectModify --- Make a modification of an existing object */

	GenesisObject*	new_object;

	new_object = (GenesisObject*) malloc(sizeof(GenesisObject));
	if (new_object == NULL)
	  {
	    Error(); printf("out of memory\n");
	    return NULL;
	  }

	BCOPY(object, new_object, sizeof(GenesisObject));
	new_object->fieldlist = NULL;
	CopyFieldList(object, new_object);
	ObjectSetModified(new_object);

	/*
	** First modification of an object definition creates a derived
	** object.
	*/

	if (!ObjectIsModified(object))
	    new_object->super = object;

	return new_object;

}	/* ObjectModify */


int do_isa(argc, argv)

int	argc;
char**	argv;

{	/* do_isa --- Check if an element's object or ancestor object isa */

	Element*	elm;
	char*		elmPath;
	char*		objName;

	initopt(argc, argv, "object element");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	objName = argv[1];
	elmPath = argv[2];

	elm = GetElement(elmPath);
	if (elm == NULL) {
	    InvalidPath(argv[0], elmPath);
	    return 0;
	}

	return ElementIsA(elm, objName);

}	/* do_isa */



int do_add_extobject(argc,argv)
int	argc;
char 	**argv;
{
GenesisObject*	object;
GenesisObject	objectCopy;
Element*	rootElm;
char	description[1000];
char	author[100];
char*	objectName;
int	status;
int	i;

    initopt(argc, argv, "object-name element-name -author author-text -description description-text ...");

    objectName = optargv[1];
    object = GetObject(objectName);
    if (object != NULL)
      {
	Error();
	printf("%s: object '%s' already exists\n", optargv[0],
		    objectName);
	return 0;
      }

    if (optargc != 3)
      {
	printoptusage(argc, argv);
	return 0;
      }

    rootElm = GetElement(optargv[2]);
    if (rootElm == NULL)
      {
	Error();
	printf("%s: element %s not found\n", optargv[0], optargv[2]);
	return 0;
      }

    if (!ObjectIsModified(rootElm->object))
	rootElm->object = ObjectModify(rootElm->object);
    objectCopy = *(rootElm->object);

    description[0] = '\0';
    author[0] = '\0';
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-description") == 0)
	  {
	    /*
	    ** read arguments into a description string
	    */
	    for (i = 1; i < optargc; i++)
	      {
		strcat(description,optargv[i]);
		strcat(description,"\n");
	      }
	  }
	else if (strcmp(G_optopt, "-author") == 0)
	  {
	    /*
	    ** read arguments into an author string
	    */
	    for (i = 1; i < optargc; i++)
		strcat(author,optargv[i]);
	  }
      }

    if(status < 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    objectCopy.name = CopyString(objectName);
    if (!ObjectHashPut(&objectCopy))
	return 0;

    object = GetObject(objectName);
    object->defaults = rootElm;
    object->defaults->object = object;

    if(strlen(description) > 0){
	object->description = CopyString(description);
    }
    if(strlen(author) > 0){
	object->author = CopyString(author);
    }
    else
	object->author = "";

    ObjectResetModified(object);
    FieldListMakePermanent(object);
    ClassListMakePermanent(object);
    MsgListMakePermanent(object);
    ActionListMakePermanent(object);

    /*
    ** Set the object for new actions to this object

       This is done in the addaction command now.

    for (act = object.valid_actions; act != NULL && act->object == NULL;
							     act = act->next)
	act->object = defaults->object;
    */

    /* remove the element from the element hash table */
    ElementHashRemoveTree(rootElm);
    DetachElement(rootElm);

    OK();
    return 1;
}
