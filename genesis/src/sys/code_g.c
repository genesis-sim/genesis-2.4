static char* rcsid = "$Id: code_g.c,v 1.6 2005/06/29 18:24:17 svitak Exp $";

/*
** $Log: code_g.c,v $
** Revision 1.6  2005/06/29 18:24:17  svitak
** Removed debugging printf.
**
** Revision 1.5  2005/06/29 16:35:51  svitak
** Removed ifdef SYSV lines since these are already in system_deps.h. If
** they don't work there, they won't work here.
**
** Revision 1.4  2005/06/27 19:01:14  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.3  2005/06/26 08:24:36  svitak
** Interpreter was being called with -1 as a parameter but no parameters
** are declared... -1 removed.
**
** Revision 1.2  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.15  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.13  2000/06/19 06:10:09  mhucka
** The Script structure and the handling of scripts had this bogosity in which
** FILE pointers were being cast to char * and back again.  Why on earth
** didn't the authors just keep a separate pointer in the Script structure for
** FILE pointers, instead of shoehorning the pointers into the .ptr char *
** field?  Anyway, for better robustness and portability, I reimplemented this
** part using two separate pointers.
**
** Revision 1.12  2000/03/27 10:38:22  mhucka
** Commented out statusline code.
**
** Revision 1.11  1999/08/22 04:42:15  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.10  1998/06/30 21:34:17  dhb
** General random number generator support
**
** Revision 1.9  1997/07/18 03:14:53  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.8  1995/08/01 19:42:00  dhb
** The hashfunc command wasn't setting the return type for the function
** when is was present.  Command function was requiring four args rather
** than three.
**
 * Revision 1.7  1995/03/02  18:28:40  dhb
 * Added explicit calls in startup code to make field and message lists
 * permanent.
 *
 * Revision 1.6  1994/10/26  00:01:01  dhb
 * Added "hashfunc func [ret-type]" command to add library functions
 * to the function hash table.
 *
 * Revision 1.5  1994/08/09  18:52:06  dhb
 * Fixed bug in do_object().  Object init code was not initializing
 * object->defaults->name.  Now sets it to (a copy of) object->name.
 *
 * Revision 1.4  1994/04/13  22:10:33  dhb
 * The 1.3 changes still don't work for SYSV systems, because most of the
 * libraries don't define the preprocessor symbol for the OS.  These changes
 * make code_g use memset() for SYSV and BZERO() for non-SYSV.
 *
 * Revision 1.3  1994/04/13  17:41:02  dhb
 * Added an include of sys/system_deps.h to the STARTUP_... C code
 * to handle SYSV systems where BZERO is not defined.
 *
 * Revision 1.2  1994/04/01  22:13:26  dhb
 * Removed trailing semi on memusage() function def and changed return type
 * for ran1() to match declaration in header file.
 *
 * Revision 1.1  1994/03/23  00:41:50  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <setjmp.h>
#include "system_deps.h"
#include "header.h"
#include "shell_defs.h"
#include "shell_func_ext.h"
#include "ss_func_ext.h"

/* 1999-07-16 <mhucka@bbb.caltech.edu>
 * Glibc2 under (e.g.) Red Hat Linux 6.0 apparently does not define the
 * different stdout/stderr streams as constants, which causes code like
 * the following to fail to compile.  It is therefore necessary to set
 * these to NULL instead.  The variables are initialized in main().
 * This fix is courtesy of Mike Vanier <mvanier@bbb.caltech.edu>.
 *
 * FILE*	LibFs = stdout;
 * FILE*	HeaderFs = stdout;
 */
FILE*	LibFs = NULL;
FILE*	HeaderFs = NULL;

int memusage () { return 0; }

void sim_set_float_format () {}

jmp_buf main_context;

float G_RNG () { return 0.0; }


static void do_newclass(argc, argv)

int	argc;
char*	argv[];

{	/* do_newclass --- Add new class at startup */

	char*	className;

	initopt(argc, argv, "class-name");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    exit(1);
	  }

	className = optargv[1];

	fprintf(LibFs, "\n");
	fprintf(LibFs, "  argv[0] = \"newclass\";\n");
	fprintf(LibFs, "  argv[1] = \"%s\";\n", className);
	fprintf(LibFs, "  do_add_class(2, argv);\n");

}	/* do_newclass */


static void do_addaction(argc, argv)

int	argc;
char*	argv[];

{	/* do_addaction --- Add action at startup */

	static int	nextActionNo = 10000;

	char*		actionName;

	initopt(argc, argv, "action-name [action-number]");
	if (G_getopt(argc, argv) != 0 || optargc > 3)
	  {
	    printoptusage(argc, argv);
	    exit(1);
	  }

	actionName = optargv[1];
	if (optargc == 3)
	  {
	    char*	actionNo;

	    actionNo = optargv[2];

	    fprintf(HeaderFs, "#define %s %s\n", actionName, actionNo);
	    fprintf(LibFs, "  AddAction(\"%s\", %s);\n", actionName, actionNo);
	  }
	else
	  {
	    fprintf(HeaderFs, "#define %s %d\n", actionName, nextActionNo);
	    fprintf(LibFs, "AddAction(\"%s\", %d);\n", actionName,
							nextActionNo++);
	  }

}	/* do_addaction */


static void do_hashfunc(argc, argv)

int	argc;
char*	argv[];

{	/* do_hashfunc --- Add function to function hash table */

	char*	function;
	char*	returnType;

	initopt(argc, argv, "function-name [return-type]\n");
	if (G_getopt(argc, argv) != 0 || argc > 3)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	function = optargv[1];

	if (argc == 3)
	    returnType = optargv[2];
	else
	    returnType = "void";

	fprintf(LibFs, "  { extern %s %s(); ", returnType, function);
	fprintf(LibFs, "  HashFunc(\"%s\", %s, \"%s\"); }\n",
					    function, function, returnType);

}	/* do_hashfunc */


static void do_addfunc(argc, argv)

int	argc;
char*	argv[];

{	/* do_addfunc --- Add function as a command */

	char*	command;
	char*	function;
	char*	returnType;

	initopt(argc, argv, "command-name function-name [return-type]\n");
	if (G_getopt(argc, argv) != 0 || argc > 4)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	command = optargv[1];
	function = optargv[2];

	if (argc == 4)
	    returnType = optargv[3];
	else
	    returnType = "void";

	fprintf(LibFs, "  { extern %s %s(); AddFunc(\"%s\", %s, \"%s\"); ",
			returnType, function, command, function, returnType);
	fprintf(LibFs, "  HashFunc(\"%s\", %s, \"%s\"); }\n",
					    function, function, returnType);

}	/* do_addfunc */

static void FieldSetup(fieldName, fieldDesc, defaultValue, fieldProt)

char*		fieldName;
char*		fieldDesc;
char*		defaultValue;
char*		fieldProt;

{	/* FieldSetup --- Set up field protection, description and def val */

	    fprintf(LibFs, "  SetFieldListProt(object, \"%s\", %s);\n",
		fieldName, fieldProt);
	    fprintf(LibFs, "  SetFieldListDesc(object, \"%s\", \"%s\");\n",
		fieldName, fieldDesc);
	    if (defaultValue != NULL)
		fprintf(LibFs,
		    "DirectSetElement(object->defaults, \"%s\", \"%s\");\n",
		    fieldName, defaultValue);

}	/* FieldSetup */


static void do_object(argc, argv)
int     argc;
char*   argv[];
 
{	/* do_object --- Generate commands to define new objects at startup */

    char*	objectName;
    char*	objectType;
    char*	objectFunction;
    int		nextMsgId = 10000;
    int		nextActMsgId = -10000;
    int		status;
    int		arg;

    initopt(argc, argv, "object-name c-structure c-function classes ...  -author author-info -action name [c-function] -message name [slot-names] -activemessage name [slot-names] -readwrite field-name field-description [default-value] -readonly field-name field-description [default-value] -hidden field-name field-description [default-value] -description line1 [additional-lines...] -actions action-defs ...  -messages name type nslots [slot-names...] [additional-messages...] -fields ...");

    objectName = optargv[1];
    objectType = optargv[2];
    objectFunction = optargv[3];

    fprintf(LibFs, "\n");
    fprintf(LibFs, "  /* Definition of object %s */\n", objectName);
    fprintf(LibFs, "  BZERO(&tobject,sizeof(GenesisObject));\n");
    fprintf(LibFs, "  tobject.name = \"%s\";\n", objectName);
    fprintf(LibFs, "  tobject.type = \"%s\";\n", objectType);
    fprintf(LibFs, "  tobject.size = sizeof(struct %s);\n", objectType);
    fprintf(LibFs, "  { extern int %s(); tobject.function = %s; ", 
		    objectFunction, objectFunction);
    fprintf(LibFs, "HashFunc(\"%s\", %s, \"int\"); }\n",
		    objectFunction, objectFunction);

    for (arg = 4; arg < optargc; arg++)
      {
	fprintf(LibFs, "  ObjectAddClass(&tobject,ClassID(\"%s\"),CLASS_PERMANENT);\n", optargv[arg]);
      }
    fprintf(LibFs, "  AddDefaultFieldList(&tobject);\n");

    fprintf(LibFs, "  tobject.defaults = (Element*) calloc(1, tobject.size);\n");
    fprintf(LibFs, "  AddObject(&tobject);\n");
    fprintf(LibFs, "  object = GetObject(\"%s\");\n", objectName);
    fprintf(LibFs, "  object->defaults->object = object;\n");
    fprintf(LibFs, "  object->defaults->name = CopyString(\"%s\");\n",
								objectName);

    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp("-action", G_optopt) == 0)
	  {
	    char*	actionName;
	    char*	actionFunction;

	    actionName = optargv[1];
	    if (optargc == 2)
		actionFunction = objectFunction;
	    else
		actionFunction = optargv[2];

	    fprintf(LibFs, "  { extern int %s(); ", actionFunction);
	    fprintf(LibFs, "AddActionToObject(object, \"%s\", %s, 0) ",
		actionName, actionFunction);
	    fprintf(LibFs, "? 0 : (Error(), printf(\"adding action '%s' to object '%s'\n\")); ", actionName, objectName);
	    fprintf(LibFs, "HashFunc(\"%s\", %s, \"int\"); }\n",
					    actionFunction, actionFunction);
	  }
	else if (strcmp("-message", G_optopt) == 0)
	  {
	    char*	msgName;
	    int		nslots;
	    int		slot;

	    msgName = optargv[1];
	    nslots = optargc - 2;

	    for (slot = 0; slot < nslots; slot++)
		fprintf(LibFs, "  slotnames[%d] = \"%s\";\n", slot,
							    optargv[slot+2]);
	    fprintf(LibFs, "  MsgListAdd(object, \"%s\", MSG_%s, slotnames, %d);\n", msgName, msgName, nslots);

	    fprintf(HeaderFs, "#define MSG_%s %d\n", msgName, nextMsgId++);
	  }
	else if (strcmp("-readwrite", G_optopt) == 0)
	  {
	    char*	defaultValue;

	    if (optargc > 3)
		defaultValue = optargv[3];
	    else
		defaultValue = NULL;

	    FieldSetup(optargv[1], optargv[2], defaultValue, "FIELD_READWRITE");
	  }
	else if (strcmp("-readonly", G_optopt) == 0)
	  {
	    char*	defaultValue;

	    if (optargc > 3)
		defaultValue = optargv[3];
	    else
		defaultValue = NULL;

	    FieldSetup(optargv[1], optargv[2], defaultValue, "FIELD_READONLY");
	  }
	else if (strcmp("-hidden", G_optopt) == 0)
	  {
	    char*	defaultValue;

	    if (optargc > 3)
		defaultValue = optargv[3];
	    else
		defaultValue = NULL;

	    FieldSetup(optargv[1], optargv[2], defaultValue, "FIELD_HIDDEN");
	  }
	else if (strcmp("-activemessage", G_optopt) == 0)
	  {
	    char*	msgName;
	    int		nslots;
	    int		slot;

	    msgName = optargv[1];
	    nslots = optargc - 2;

	    fprintf(HeaderFs, "#define %s_%s %d\n", objectName, msgName,
							    nextActMsgId--);

	    for (slot = 0; slot < nslots; slot++)
		fprintf(LibFs, "  slotnames[%d] = \"%s\";\n", slot,
							    optargv[slot+2]);
	    fprintf(LibFs, "  MsgListAdd(object, \"%s\", %s_%s, slotnames, %d);\n", msgName, objectName, msgName, nslots);
	  }
	else if (strcmp("-author", G_optopt) == 0)
	  {
	    fprintf(LibFs, "  object->author = \"%s\";\n", optargv[1]);
	  }
	else if (strcmp("-description", G_optopt) == 0)
	  {
	    int	arg;

	    fprintf(LibFs, "  object->description = \"%s", optargv[1]);
	    for (arg = 2; arg < optargc; arg++)
		fprintf(LibFs, "\\n%s", optargv[arg]);
	    fprintf(LibFs, "\\n\";\n");
	  }
	else if (strcmp("-actions", G_optopt) == 0)
	  {
	    int	arg;

	    arg = 1;
	    while (arg < optargc)
	      {
		char*	actionName;
		char*	actionFunction;

		actionName = optargv[arg++];
		if (*actionName == '*')
		  {
		    actionName++;
		    if (arg < optargc)
			actionFunction = optargv[arg++];
		    else
		      {
			fprintf(stderr, "insufficient args to object command -actions option\n");
			exit(1);
		      }
		  }
		else
		    actionFunction = objectFunction;

		fprintf(LibFs, "  { extern int %s(); ", actionFunction);
		fprintf(LibFs, "AddActionToObject(object, \"%s\", %s, 0) ",
		    actionName, actionFunction);
		fprintf(LibFs, "? 0 : (Error(), printf(\"adding action '%s' to object '%s'\\n\")); ", actionName, objectName);
		fprintf(LibFs, "HashFunc(\"%s\", %s, \"int\"); }\n",
					    actionFunction, actionFunction);
	      }
	  }
	else if (strcmp("-messages", G_optopt) == 0)
	  {
	    int	arg;

	    arg = 1;
	    while (arg < optargc)
	      {
		char*	msgName;
		char*	msgType;
		int	nslots;
		int	slot;

		if (optargc - arg < 3)
		  {
		    fprintf(stderr, "insufficient args to '%s' object command -messages option\n", objectName);
		    exit(1);
		  }
		msgName = optargv[arg++];
		msgType = optargv[arg++];
		nslots = atoi(optargv[arg++]);
		if (optargc - arg < nslots)
		  {
		    fprintf(stderr, "insufficient args to '%s' object command -messages option, message '%s'\n", objectName, msgName);
		    exit(1);
		  }

		fprintf(HeaderFs, "#define %s_%s %s\n", objectName, msgName,
								    msgType);

		for (slot = 0; slot < nslots; slot++)
		    fprintf(LibFs, "  slotnames[%d] = \"%s\";\n", slot,
								optargv[arg++]);
		fprintf(LibFs, "  MsgListAdd(object, \"%s\", %s_%s, slotnames, %d);\n", msgName, objectName, msgName, nslots);
	      }
	  }
      }
    fprintf(LibFs, "  FieldListMakePermanent(object);\n");
    fprintf(LibFs, "  MsgListMakePermanent(object);\n");

    if (status < 0)
      {
	printoptusage(argc, argv);
	exit(1);
      }

}	/* do_object */


static void do_quit()

{
    fprintf(stderr, "You shouldn't be quiting out of a startup file\n");
    fprintf(stderr, "Quit command ignored\n");
    fflush(stderr);
}


int main(argc, argv)
int     argc;
char   *argv[];
{
FILE	*pfile;
char	*pname;
char	*libname;
char	*libstruct;
char	outname[100];
char	headername[100];
char* largv[2];
char*	cstartup = NULL;
int	variables = 1;
int	objects = 1;
int	status;

        /* 1999-07-16 <mhucka@bbb.caltech.edu>  See comments for LibFS above. */
        LibFs    = stdout;
        HeaderFs = stdout;

	initopt(argc, argv, "startup-file struct-file library-name -novariables -noobjects -cstartup c-function-name");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-novariables") == 0)
		variables = 0;
	    else if (strcmp(G_optopt, "-noobjects") == 0)
		objects = 0;
	    else if (strcmp(G_optopt, "-cstartup") == 0)
		cstartup = optargv[1];
	  }

	if (status != 0)
	  {
	    printoptusage(argc, argv);
	    return(1);
	  }

	pname = argv[1];
	libstruct = argv[2];
	libname = argv[3];

	pfile = fopen(pname, "r");
	if (pfile == NULL) {
	    perror(pname);
	    return(1);
	}

	sprintf(outname, "%s_g@.c", libname);
	LibFs = fopen(outname, "w");
	if (LibFs == NULL) {
	    perror(outname);
	    return(1);
	}

	sprintf(headername, "%s_g@.h", libname);
	HeaderFs = fopen(headername, "w");
	if (HeaderFs == NULL) {
	    perror(headername);
	    return(1);
	}

	SetScript(NULL, pfile, 0, NULL, FILE_TYPE);

    /*
    ** initialize the command interpreter
    */
    SetDefaultPrompt("genesis #!");

    if(!SetupInterp(argv[0])){
	    printf("Unable to find the simulator. ");
	    printf("Check your PATH environment variable\n");
	return(1);
    };

    SetBatchMode(1);

#ifdef COMMENT
    /*
    ** if a script has been specified on the command line
    ** then read it in
    ** using the remaining command line args as args to the file
    */
    if(pname != NULL) {
	if((pfile=SearchForScript(pname,"r")) == NULL){
	    Error();
    	    if (IsSilent() < 2) 
	   	 printf("can't open parameter file %s\n",pname);
	} else {
	    if(IsSilent()<1) printf("%-20s%s\n","Simulation Script:",pname);
	    /*
	    ** try reading from the parameter file
	    */
	    AddScript(NULL, pfile, argc - file_arg, argv + file_arg, FILE_TYPE);
	    /*
	    SetScript(pfile,argc - file_arg,argv + file_arg,0);
	    Interpreter();
	    */
	}
    }

    /*
    ** try reading from the startup file
    */
    if(use_simrc){
	/*
	** first try and find it locally
	*/
	strcpy(string,simrc_name);
	if((pfile=fopen(string,"r")) == NULL){
	    /*
	    ** else try and find it in the home directory
	    */
	    if(home = getenv("HOME")){
		sprintf(string,"%s/%s",home,simrc_name);
		pfile=fopen(string,"r");
	    }
	}
	if(pfile){
	    if(IsSilent()<1) printf("%-20s%s\n","Startup script:",string);
	    /*
	    * get input from .simrc
	    */
	    AddScript(NULL, pfile, 0, NULL, FILE_TYPE);
	}
    }
    /*
    ** if it is batch mode then dont bother going into
    ** interactive mode
    */
    if(!IsBatchMode()){
#ifdef STATUSLINE
	if(status_line){
	    EnableStatusLine();
	}
#endif /* STATUSLINE */
	/*
	** put the terminal into non-blocking custom io mode
	*/
	terminal_setup();
	/*
	** set the return point for context jumps
	*/
	if(setjmp(main_context) != 0){
	    /*
	    ** coming back from a longjump
	    */
	    ClearScriptStack();
	    RemoveSimulationJob();
	    tset();
	}

	ParseInit();

	if(catch){
	    SetSignals();
	}
	/*
	** go to the interactive interpreter level
	*/
    }
    EnableHistory(1);
#endif
    AlternatePrompt(NULL);

    AddFunc("addaction", do_addaction, "void");
    AddFunc("addfunc", do_addfunc, "void");
    AddFunc("exit", do_quit, "void");
    AddFunc("hashfunc", do_hashfunc, "void");
    AddFunc("newclass", do_newclass, "void");
    AddFunc("object", do_object, "void");
    AddFunc("quit", do_quit, "void");

    largv[0] = "autoshell";
    largv[1] = "0";
    do_autoshell(2, largv);

    fprintf(LibFs, "#include <stdio.h>\n");
    fprintf(LibFs, "#include \"%s\"\n", libstruct);
    fprintf(LibFs, "#include \"%s_g@.h\"\n", libname);
    fprintf(LibFs, "\n");

    fprintf(LibFs, "void STARTUP_%s() {\n", libname);
    if (variables)
      {
	if (objects)
	  {
	    fprintf(LibFs, "  GenesisObject* object;\n");
	    fprintf(LibFs, "  GenesisObject  tobject;\n");
	  }
	fprintf(LibFs, "  char*          slotnames[50];\n");
	fprintf(LibFs, "  char*          argv[50];\n");
      }

    if (cstartup != NULL)
	fprintf(LibFs, "  { extern void %s(); %s(); }\n", cstartup, cstartup);

    if (setjmp(main_context) == 0)
	Interpreter();

    fprintf(LibFs, "\n");
    fprintf(LibFs, "/* Script variables */\n");
    CompileScriptVars(LibFs, "  ");

    fprintf(LibFs, "\n");
    fprintf(LibFs, "} /* STARTUP_%s */\n", libname);

    fclose(LibFs);
    fclose(HeaderFs);

    return(0);
}
