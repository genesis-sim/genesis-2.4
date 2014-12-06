static char rcsid[] = "$Id: sim_bindings.c,v 1.2 2005/06/27 19:00:57 svitak Exp $";

/*
** $Log: sim_bindings.c,v $
** Revision 1.2  2005/06/27 19:00:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1995/06/28 00:18:26  dhb
** Changed showalias to showcommand which shows the C function
** implementing a given command.
**
 * Revision 1.3  1994/03/20  20:28:24  dhb
 * Fixed bug in call to AddFunc() from do_alias().
 *
 * Changed options processing to use initopt/getopt.
 *
 * Added return values: 0 = failed, 1 = success; for alias and showalias
 * commands.
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  19:35:36  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

char *GetBinding(name)
char *name;
{
    /*
    ** try to find the actual function name
    */
    return(GetFuncName(GetCommand(name).adr));
}

int do_show_binding(argc,argv)
int argc;
char **argv;
{
char *funcname;

    initopt(argc, argv, "command-name");
    if (G_getopt(argc, argv) != 0) {
	printoptusage(argc, argv);
	return 0;
    }

    if((funcname = GetBinding(argv[1])) == NULL){
	Error(); printf("%s: could not find command %s\n", argv[0], argv[1]);
	return 0;
    } else {
	printf("'%s' is implemented by the C function '%s()'\n",argv[1],funcname);
	return 1;
    }
}

int do_alias(argc,argv)
int argc;
char **argv;
{
FuncTable	command;

    initopt(argc, argv, "alias-name command-name");
    if (G_getopt(argc, argv) != 0) {
	printoptusage(argc, argv);
	return 0;
    }

    command = GetCommand(argv[2]);
    if(command.adr == NULL){
	Error(); printf("%s: could not find command %s\n", argv[0], argv[2]);
	return 0;
    } else {
	AddFunc(argv[1], command.adr, command.type);
	OK();
	return 1;
    }
}
