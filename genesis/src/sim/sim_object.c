static char rcsid[] = "$Id: sim_object.c,v 1.3 2005/06/29 17:15:35 svitak Exp $";

/*
** $Log: sim_object.c,v $
** Revision 1.3  2005/06/29 17:15:35  svitak
** Removed local proto of strtok (included in string.h). Init for fieldname to
** quiet compiler. Added 'do_add_object' to error strings.
**
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.14  2001/06/29 21:28:55  mhucka
** Fixes from Hugo C.
**
** Revision 1.13  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.11  1997/08/08 20:14:04  dhb
** Fixed return value in error case and return success value in
** normal case for so_setobjenv().
**
** Revision 1.10  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.9  1994/06/03 23:28:51  dhb
** Changes to BasicObjects() to use the AddDefaultFieldList() function
** to set the right fieldlist defaults.
**
 * Revision 1.8  1994/06/02  16:28:03  dhb
 * Fixed return values from do_getarg() which must return allocated values.
 *
 * Revision 1.7  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.6  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.5  1993/03/18  17:15:55  dhb
 * Element field protection.
 *
 * Revision 1.4  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.3  1993/03/08  18:26:29  dhb
 * Added default element field values.
 *
 * Revision 1.2  1993/02/17  20:03:52  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_show_object (showobject), do_setobjenv (setobjenv) and do_getobjenv
 * 	(getobjenv) changed to use GENESIS getopt routines.
 *
 * 	do_add_object (object) not changed as this command will disappear in
 * 	other 2.0 related changes.  do_list_objects not changed since it takes
 * 	no command arguments or options.
 *
 * Revision 1.1  1992/10/27  20:20:35  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <string.h>	/* strtok */
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "hash.h"

HASH *object_hash_table;

/*
** Hash table utilities
*/
void ObjectHashInit()
{
HASH *hash_create();

    /*
    ** create the table
    */
    object_hash_table = hash_create(2000);
}



/*
** ObjectHashEnter
**
** FUNCTION
** 	Enter an object in the object hash table.
**
** ARGUMENTS
**	GenesisObject*	object	- object to place in the hash table
**
** RETURNS
**	0	- Hash table full
**	1	- Success
*/

int ObjectHashEnter(object)
GenesisObject *object;
{
ENTRY	item,*hash_enter();

    item.data = (char *)object;
    item.key = (char *)(((GenesisObject *)(item.data))->name);
    /*
    ** put the object into the table
    */
    if(hash_enter(&item,object_hash_table) == NULL){
	Error();
	printf("object hash table full\n");
	return(0);
    };
    return(1);
}


/*
** ObjectHashPut
**
** FUNCTION
**	Allocates memory for and copies a GenesisObject then enters the
**	object copy in the object hash table.
**
** ARGUMENTS
**	GenesisObject*	object	- Object to be put in the hash table
**
** RETURNS
**	0	- Hash table full or out of memory
**	1	- Success
*/

int ObjectHashPut(object)
GenesisObject *object;
{
GenesisObject*	cp_object;

    cp_object = (GenesisObject *)calloc(1,sizeof(GenesisObject));
    if (cp_object == NULL)
      {
	Error(); printf("out of memory\n");
	return 0;
      }
    BCOPY(object,cp_object,sizeof(GenesisObject));

    return ObjectHashEnter(cp_object);
}

GenesisObject *ObjectHashFind(key)
char	*key;
{
ENTRY	*found_item,*hash_find();

    /*
    ** get the object from the table
    */
    if((found_item = hash_find(key,object_hash_table)) != NULL){
	return((GenesisObject *)found_item->data);
    } else
	return(NULL);
}



/*
** get the object from the object table
*/
GenesisObject *GetObject(name)
char *name;
{
extern GenesisObject	*ObjectHashFind();
#ifdef LATER
static	GenesisObject	*saved_object = NULL;
static	char	*saved_name = NULL;
#endif
    if(name == NULL) return(NULL);
#ifdef LATER
    if(saved_name == name) return(saved_object);
    saved_name = name;
    saved_object = ObjectHashFind(name);
    return(saved_object);
#else
    return(ObjectHashFind(name));
#endif
}


/*
** get the value of var from the object environment list
*/
char *getobjenv(object,var)
GenesisObject 	*object;
char	*var;
{

    if(object == NULL || var == NULL) return(NULL);

    return GetExtField(object->defaults, var);
}


/*
** add an object to the object table
*/
void AddObject(object)
GenesisObject *object;
{
    ObjectHashPut(object);
}


/*
** WARNING: code for extended objects assumes that this function adds classes
** to the beginning of the list.
*/
void ObjectAddClass(object,id,flags)
GenesisObject 	*object;
int	id;
int	flags;
{
ClassList	*class;

    class = (ClassList *)malloc(sizeof(ClassList));
    class->id = id;
    class->flags = flags;
    class->next = object->class;
    object->class = class;
}

int do_add_object(argc,argv)
int	argc;
char 	**argv;
{
GenesisObject	object;
Element*	defaults;
int	nxtarg;
int	mode = 0;
char	*name;
int	type;
int	slots;
int	i;
MsgList		*new_msg;
ActionList	*act;
char	description[1000];
char	author[100];
char	*function_name;
PFI	function;
int	envarg;
char	*ptr;
char	*FieldHashFind();
char	*FieldHashFindAndCopy();

    if(argc < 5){
	printf("usage: %s name data function class\n", argv[0]);
	printf("\t[-author text]\n");
	printf("\t[-messages name type nslots slotnames]\n");
	printf("\t[-actions names]\n");
	printf("\t[-fields names]\n");
	printf("\t[-default args]\n");
	printf("\t[-environment var value var2 value2 ..]\n");
	printf("\t[-description text]\n");
	return 0;
    }
    function_name = argv[3];
    BZERO(&object,sizeof(GenesisObject));
    /*
    ** make an empty object
    */
    object.name 		= CopyString(argv[1]);
	object.method		= 0;
    /*
    ** check on the type
    */
    if(!FieldHashFind(argv[2])){
	Error();
	printf("do_add_object: invalid data type\n");
	return 0;
    }
    object.type 		= CopyString(argv[2]);
    object.size			= StructSize(object.type);
    if((object.function 	= GetFuncAddress(argv[3])) == NULL){
	Error();
	printf("do_add_object: could not find function '%s'\n",argv[3]);
	return 0;
    }

    /*
    ** Add all fields to fieldlist setting default protection
    */

    ptr = FieldHashFindAndCopy(object.type);
    if (ptr != NULL)
      {
	/* char	string[1000]; */
	char*	fieldname = NULL;

	/* strcpy(string,ptr); */
	fieldname = strtok(/* string */ptr, " \t\n");
	while (fieldname != NULL)
	  {
	    if (!AddFieldList(&object, fieldname))
	      {
		printf("do_add_object: error adding to field list\n");

		free(ptr);

		return 0;
	      }

	    fieldname = strtok(NULL, " \t\n");
	  }

	free(ptr);
      }

    nxtarg = 3;
    envarg = -1;
    /*
    ** get the class specification up to the first option
    */
    while(++nxtarg < argc){
	if((strcmp(argv[nxtarg],"-default") ==0) ||
	(strcmp(argv[nxtarg],"-environment") ==0) ||
	(strcmp(argv[nxtarg],"-actions") ==0) ||
	(strcmp(argv[nxtarg],"-fields") ==0) ||
	(strcmp(argv[nxtarg],"-description") ==0) ||
	(strcmp(argv[nxtarg],"-author") ==0) ||
	(strcmp(argv[nxtarg],"-messages") ==0) 
	) {
	    nxtarg--;
	    break;
	}
	ObjectAddClass(&object,ClassID(argv[nxtarg]),CLASS_PERMANENT);
    }
    description[0] = '\0';
    author[0] = '\0';
    /*
    ** get the options
    */
    while(++nxtarg < argc){
	if(strcmp(argv[nxtarg],"-default") ==0){
	    mode = 1;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-environment") ==0) {
	    mode = 2;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-actions") ==0) {
	    mode = 3;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-fields") ==0) {
	    mode = 4;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-hfields") ==0) {
	    mode = -4;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-messages") ==0) {
	    mode = 5;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-description") ==0) {
	    mode = 6;
	    continue;
	}
	if(strcmp(argv[nxtarg],"-author") ==0) {
	    mode = 7;
	    continue;
	}
	switch(mode){
	case 0:
	    Error();
	    printf("do_add_object: syntax error\n");
	    return 0;
	case 1:			/* default */
	    /*
	    ** get the default values or execute a function on the
	    ** default object
	    */
	    break;
	case 2:			/* environment */
	    if (envarg == -1)
		envarg = nxtarg;
	    nxtarg++;
	    break;
	case 3:			/* actions */
	    name = argv[nxtarg];
	    function = object.function;
	    /*
	    ** try to find a special action function
	    */
	    if(name[0] == '*'){
		name++;
		/*
		** get the action function name 
		*/
		if((function = GetFuncAddress(argv[++nxtarg])) == NULL){
		    Error();
		    printf("do_add_object: cant find function '%s'\n",argv[nxtarg]);
		    return 0;
		}
	    }
	    if(!AddActionToObject(&object,name,function,0)){
		Error();
		printf("do_add_object: unable to add action '%s' to '%s'\n",name,object.name);
	    }
	    break;
	case -4:			/* hidden fields */
	    if (SetFieldListProt(&object, argv[nxtarg], FIELD_HIDDEN) != -1)
		SetFieldListDesc(&object, argv[nxtarg], argv[nxtarg+1]);
else
    printf("do_add_object: object %s field %s desc %s\n", object.name, argv[nxtarg],
							argv[nxtarg+1]);
	    nxtarg++;
	    break;
	case 4:			/* fields */
	    if (SetFieldListProt(&object, argv[nxtarg], FIELD_READWRITE) != -1)
		SetFieldListDesc(&object, argv[nxtarg], argv[nxtarg+1]);
else
    printf("do_add_object: object %s field %s desc %s\n", object.name, argv[nxtarg],
							argv[nxtarg+1]);
	    nxtarg++;
	    break;
	case 5:			/* messages */
	    name = argv[nxtarg];
	    if(++nxtarg >= argc){
		printf("do_add_object: insufficient arguments to %s\n",argv[0]);
		return 0;
	    }
	    type = atoi(argv[nxtarg]);
	    if(++nxtarg >= argc){
		printf("do_add_object: insufficient arguments to %s\n",argv[0]);
		return 0;
	    }
	    slots = atoi(argv[nxtarg]);
	    new_msg = (MsgList *)malloc(sizeof(MsgList));
	    new_msg->type = type;
	    new_msg->name = CopyString(name);
	    new_msg->flags = MSGLIST_PERMANENT;
	    new_msg->slots = slots;
	    new_msg->slotname = (char **)malloc(slots*sizeof(char *));
	    for(i=0;i<slots;i++){
		if(++nxtarg >= argc){
		    printf("do_add_object: insufficient arguments to %s\n",argv[0]);
		    return 0;
		}
		new_msg->slotname[i] = CopyString(argv[nxtarg]);
	    }
	    new_msg->next = object.valid_msgs;
	    object.valid_msgs = new_msg;
	    break;
	case 6:
	    /*
	    ** read arguments into a description string
	    */
	    strcat(description,argv[nxtarg]);
	    strcat(description,"\n");
	    break;
	case 7:
	    /*
	    ** read arguments into a description string
	    */
	    strcat(author,argv[nxtarg]);
	    break;
	}
    }
    if(strlen(description) > 0){
	object.description = CopyString(description);
    }
    if(strlen(author) > 0){
	object.author = CopyString(author);
    }

    defaults = (Element*) calloc(1, object.size);
    object.defaults = defaults;
    AddObject(&object);

#ifdef COMMENT
    if (object.name[0] != 'x')
	defaults = (Element *) Create(object.name,"proto",NULL,NULL,0);
    else
	defaults = NULL;
    defaults = (Element *) Create(object.name,"proto",NULL,NULL,0);
#endif
    if (defaults != NULL)
      {
	defaults->object = GetObject(object.name);

	if (envarg != -1)
	    while (envarg < argc && argv[envarg][0] != '-')
	      {
		AddExtField(defaults, argv[envarg]);
		SetExtField(defaults, argv[envarg], argv[envarg+1]);
		envarg += 2;
	      }

	/*
	** Add object pointer to actions defined on the object
	*/

	for (act = object.valid_actions; act != NULL; act = act->next)
	    act->object = defaults->object;

	ActionListMakePermanent(defaults->object);
	ClassListMakePermanent(defaults->object);
	MsgListMakePermanent(defaults->object);
	FieldListMakePermanent(defaults->object);
      }

    OK();
    return 1;
}

/*
** preload the object table with some basic objects
*/
void BasicObjects()
{
GenesisObject	object;
Element*	defaults;
/* char*		ptr; */

    BZERO(&object,sizeof(GenesisObject));
    /*
    ** make an empty object
    */
    object.name 		= "neutral";
    ObjectAddClass(&object,ELEMENT_ELEMENT,CLASS_PERMANENT);
    object.function 		= NULL;
    object.type 		= "element_type";
    object.size			= sizeof(struct element_type);
    object.method		= 0;

    /*
    ** Add all fields to fieldlist setting default protection
    */

    AddDefaultFieldList(&object);
    AddObject(&object);

    defaults = (Element *) Create(object.name,"proto",NULL,NULL,0);
    defaults->object->defaults = defaults;
}

void do_list_objects()
{
extern HASH *object_hash_table;
char 	*name;
int	cnt=0;
int 	i;
char	**namelist;
extern int Strcmp();

    /*
    ** count the objects
    */
    for(i=0;i<object_hash_table->size;i++){
	if ((name = object_hash_table->entry[i].key)) {
	    cnt++;
	}
    }
    if(cnt <1) return;
    /*
    ** make the list
    */
    namelist = (char **)malloc(sizeof(char *)*cnt);
    cnt = 0;
    for(i=0;i<object_hash_table->size;i++){
	if ((name = object_hash_table->entry[i].key)) {
	    namelist[cnt++] = name;
	}
    }
    /*
    ** sort the list
    */
    qsort(namelist,cnt,sizeof(char *),Strcmp);
    printf("\nAVAILABLE OBJECTS:\n");
    for(i=0;i<cnt;i++){
	printf("%-20s",namelist[i]);
	if(((i+1)%4) == 0){
	    printf("\n");
	}
    }
    printf("\n\n");
    free(namelist);
}

void do_show_object(argc,argv)
int	argc;
char 	**argv;
{
GenesisObject	*object;

    initopt(argc, argv, "object-name");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }
    if ((object = GetObject(optargv[1]))) {
	ShowObject(object);
    } else {
	Error();
	printf("could not find object '%s'\n",optargv[1]);
    }
}

char *do_getelementenv(argc,argv)
int argc;
char **argv;
{
Element	*element;

    if(argc < 3){
	printf("usage: %s element variable\n",argv[0]);
	return(NULL);
    }
    if((element = GetElement(argv[1])) == NULL){
	InvalidPath(argv[0],argv[1]);
	return(NULL);
    }
    return(CopyString(GetExtField(element->object,argv[2])));
}

int SetElementEnv(element,var,value)
Element *element;
char *var;
char *value;
{
    if(element == NULL || var == NULL || value == NULL){
	return(0);
    }

    return SetExtField(element, var, value);
}


int do_setelementenv(argc,argv)
int argc;
char **argv;
{
Element	*element;
int	nxtarg;

    if(argc < 4){
	printf("usage: %s element variable value [variable value]\n",argv[0]);
	return(0);
    }
    if((element = GetElement(argv[1])) == NULL){
	InvalidPath(argv[0],argv[1]);
	return(0);
    }

    nxtarg = 2;
    while(argc > nxtarg + 1){
	/*
	** set the object environment variable
	*/
	SetExtField(element, argv[nxtarg], argv[nxtarg+1]);
	nxtarg += 2;
    }

    return(1);
}

int do_setobjenv(argc,argv)
int argc;
char **argv;
{
GenesisObject	*object;
int	nxtarg;

    initopt(argc, argv, "object-name variable value ...");
    if (G_getopt(argc, argv) != 0 || optargc%2 == 1)
      {
	printoptusage(argc, argv);
	return(0);
      }
    if((object = GetObject(optargv[1])) == NULL){
	Error();
	printf("could not find object '%s'\n",optargv[1]);
	return(0);
    }
    nxtarg = 2;
    while(nxtarg < optargc){
	/*
	** set the object environment variable
	*/
	SetExtField(object->defaults,optargv[nxtarg],optargv[nxtarg+1]);
	nxtarg += 2;
    }

    return(1);
}

char *do_getobjenv(argc,argv)
int argc;
char **argv;
{
GenesisObject	*object;

    initopt(argc, argv, "object-name variable");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return NULL;
      }
    if((object = GetObject(optargv[1])) == NULL){
	Error();
	printf("could not find object '%s'\n",optargv[1]);
	return(NULL);
    }
    return(CopyString(GetExtField(object->defaults,optargv[2])));
}

void do_setdefault(argc, argv)

int	argc;
char**	argv;

{	/* do_setdefault --- Set default values for elements of this object */

	GenesisObject*	object;
	Element*	elm;
	int		nxtarg;

	initopt(argc, argv, "object-name field-name value ...");
	if (G_getopt(argc, argv) != 0 || optargc%2 != 0)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	object = GetObject(optargv[1]);
	if (object == NULL)
	  {
	    printf("%s: Unknown object '%s'\n", optargv[0], optargv[1]);
	    return;
	  }

	elm = object->defaults;
	if (elm == NULL)
	  {
	    elm = (Element*) calloc(object->size, 1);
	    if (elm == NULL)
	      {
		printf("%s: Could not allocate default values storage\n",
							optargv[0]);
		return;
	      }

	    elm->object = object;
	    object->defaults = elm;
	  }

	for (nxtarg = 2; nxtarg < optargc; nxtarg += 2)
	    if (!SetElement(elm, optargv[nxtarg], optargv[nxtarg+1]))
		printf("%s: cannot set default value for field '%s' of object '%s'\n", optargv[0], optargv[nxtarg], optargv[1]);

}	/* do_setdefault */

char* do_getdefault(argc, argv)

int	argc;
char**	argv;

{	/* do_getdefault --- Return default value for a field of this object */

	GenesisObject*	object;
	Element*	elm;

	initopt(argc, argv, "object-name field-name");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return NULL;
	  }

	object = GetObject(optargv[1]);
	if (object == NULL)
	  {
	    printf("%s: Unknown object '%s'\n", optargv[0], optargv[1]);
	    return NULL;
	  }

	elm = object->defaults;
	if (elm == NULL)
	  {
	    elm = (Element*) calloc(object->size, 1);
	    if (elm == NULL)
	      {
		printf("%s: Could not allocate default values storage\n",
							optargv[0]);
		return CopyString("0");
	      }

	    elm->object = object;
	    object->defaults = elm;
	  }

	return CopyString(ElmFieldValue(elm, optargv[2]));

}	/* do_getdefault */
