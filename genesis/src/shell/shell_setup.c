static char rcsid[] = "$Id: shell_setup.c,v 1.3 2005/06/26 08:24:36 svitak Exp $";

/*
** $Log: shell_setup.c,v $
** Revision 1.3  2005/06/26 08:24:36  svitak
** Interpreter was being called with -1 as a parameter but no parameters
** are declared... -1 removed.
**
** Revision 1.2  2005/06/17 19:27:17  svitak
** Added code for searching for executables with .exe extension when
** running under Cygwin.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/06/29 21:26:03  mhucka
** New mkdir function from Hugo C.
**
** Revision 1.11  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.9  2000/04/24 07:59:06  mhucka
** 1) Removed use of getwd, which under some systems (e.g., Linux) is
**    undesirable for security reasons.
** 2) Did some cleanup of the code.
**
** Revision 1.8  1995/07/28 01:24:44  dhb
** Removed the execute command from being addfunced at shell initialization.
**
** Revision 1.7  1995/06/16  05:59:42  dhb
** FreeBSD compatibility.
**
** Revision 1.6  1994/06/15  23:35:58  dhb
** Changed command result types for the debug, debugfunc and silent
** commands to int.
**
** Revision 1.5  1994/04/14  17:34:22  dhb
** Added extern char* getcwd() for SYSV systems
**
** Revision 1.4  1994/03/20  21:12:24  dhb
** Changes for C code startups
**
** Revision 1.3  1994/03/20  20:52:33  dhb
** 2.0 command names
** 	showcommands ==> listcommands
**
** Also removed a number of obsolete commands.
**
** Revision 1.2  1993/02/26  22:08:44  dhb
** 1.4 to 2.0 command name changes.
**
** Revision 1.1  1992/12/11  19:04:43  dhb
** Initial revision
**
*/

#include "shell.h"
#ifndef NEW
    ParseInit(){}
#endif

char *progname;

#ifdef NEW
void SetUpFuncTable()
{
	AddFunc("debug",			do_debug	,"int");
	AddFunc("debugfunc",			do_debug_func	,"int");
	AddFunc("h",				ShowHistory	,"void");
	AddFunc("cd",				do_cd		,"int");
	AddFunc("mkdir",			do_mkdir	,"int");
	AddFunc("echo",				do_echo		,"void");
	AddFunc("setprompt",			do_set_prompt	,"void");
	AddFunc("where",			do_where	,"void");
	AddFunc("exit",				do_exit		,"void");
	AddFunc("quit",				do_exit		,"void");
	AddFunc("tset",				tset		,"void");
	AddFunc("listcommands",			ShowFuncNames	,"void");
	AddFunc("printargs",			do_printargs	,"void");
	AddFunc("silent",			do_silent	,"int");
	AddFunc("setenv",			do_setenv	,"void");
	AddFunc("getenv",			do_getenv	,"char*");
	AddFunc("printenv",			do_printenv	,"void");
}

char *StripCR(s)
char *s;
{
    char *ptr;

    ptr = s;
    for (ptr = s; *ptr != '\0' && *ptr != '\n'; ptr++)
	;
    *ptr = '\0';
    return(s);
}

/*
** The following function sets the global variable "progname" as a side-effect,
** and returns an integer 0 or 1 depending on success.
*/
int SetupInterp(file)
char *file;
{
    extern int STARTUP_shell();
    FILE *fp;
    char tmp[MAXPATHLEN];
    char tmpwd[MAXPATHLEN];

    /*
    ** Check to see whether the executable can be opened.
    ** Note: SearchForExecutable mallocs the string returned in &progname.
    */
    if ((fp = SearchForExecutable(file, "r", &progname)) == NULL) {
#ifdef Cygwin
        sprintf(tmp, "%s.exe", file);
        if ((fp = SearchForExecutable(tmp, "r", &progname)) == NULL)
#endif
           return(0);
    }
    fclose(fp);

    /*
    ** Construct the absolute pathname for the program if it has 
    ** has a relative specification.
    */
    if (progname != NULL && *progname != '/') {
	if ((char *) getcwd(tmpwd, (size_t)MAXPATHLEN) == NULL) {
	    printf("Unable to get the working directory.\n");
	}
	else {
	    sprintf(tmp, "%s/%s", StripCR(tmpwd), progname);
	    FreeString(progname);
	    progname = CopyString(tmp);
	}
    }
    if (IsSilent() < 2) {
	printf("%-20s%s\n", "Executable:", progname);
    }
    RecordStartTime();
    HashInit();
    if (Dynamic()) {
	printf("Initializing dynamic load module...\n");
	LoadFuncInit(file);
    }
    STARTUP_shell();
    SetUpFuncTable();
    DefaultPrompt();
    return(1);
}

void Interpreter()
{
    extern int yyparse();

    yyparse();
}
#endif
