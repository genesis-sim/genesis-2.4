static char rcsid[] = "$Id: shell_debug.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: shell_debug.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2003/05/29 22:33:01  gen-dbeeman
** Replaced C++ style comments for K&R compatiblility
**
** Revision 1.5  2003/03/28 21:10:46  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
**
** Revision 1.4  2000/04/24 07:54:36  mhucka
** Moved the definition of variable "debug" out of shell.h and into this
** file.  .h files should not define variables.
**
** Revision 1.3  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/26 22:08:59  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:24  dhb
** Initial revision
**
*/

#include "shell_ext.h"
#include "ss_func_ext.h"

int debug = 0;


int do_debug(argc,argv)
int	argc;
char	**argv;
{
    initopt(argc, argv, "[level]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return 0;
      }

    if (optargc == 2)
	debug = atoi(argv[1]);

    if(IsSilent()<1)
	printf("debug level %d\n",debug);
    return(debug);
}

int do_debug_func(argc,argv)
int	argc;
char	**argv;
{
char	debug_name[100];
int	*debug_ptr;

    initopt(argc, argv, "function [level]");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return 0;
      }

    sprintf(debug_name,"DEBUG_%s",optargv[1]);
    /* LATER look up global vars more safely */
    if((debug_ptr = (int *)GetFuncAddress(debug_name)) == NULL){
	printf("unable to selectively debug function '%s'\n",optargv[1]);
	return(0);
    }

    if(optargc == 3)
	*debug_ptr = atoi(optargv[2]);

    /*
    ** locate the debug variable for the function
    */
    if(IsSilent()<1){
	printf("debug level %d\n",debug + *debug_ptr);
    }
    return(debug + *debug_ptr);
}

int do_gftrace(argc,argv)
int	argc;
char	**argv;
{
    int iLevel = 0;

    /* get previous trace level */

    int iResult = GetFunctionTraceLevel();

    /* parse options */

    initopt(argc, argv, "level");
    if (G_getopt(argc, argv))
    {
	printoptusage(argc, argv);
	return(iResult);
    }

    /* get trace level */

    iLevel = atoi(argv[1]);

    /* set trace level in parse tree evaluation routines */

    SetFunctionTraceLevel(iLevel);

    /* return previous level */

    return(iResult);
}

int do_gctrace(argc,argv)
int	argc;
char	**argv;
{
    int iLevel = 0;

    /*  get previous trace level */

    int iResult = GetCommandTraceLevel();

    /* parse options */

    initopt(argc, argv, "level");
    if (G_getopt(argc, argv))
    {
	printoptusage(argc, argv);
	return(iResult);
    }

    /* get trace level */

    iLevel = atoi(argv[1]);

    /* set trace level in parse tree evaluation routines */

    SetCommandTraceLevel(iLevel);

    /* return previous level */

    return(iResult);
}
