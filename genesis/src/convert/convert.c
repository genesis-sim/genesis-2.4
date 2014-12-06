static char rcsid[] = "$Id: convert.c,v 1.3 2005/07/01 10:03:01 svitak Exp $";

/*
** $Log: convert.c,v $
** Revision 1.3  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:33  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/18 22:39:36  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.4  1999/12/29 10:24:59  mhucka
** Fixed compilation warning involving signal return datatypes.
**
** Revision 1.3  1997/07/18 03:13:37  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1995/02/01 23:01:03  dhb
** Added support for toggling use of X1compat library using
** -noX1compat switch.
**
 * Revision 1.1  1995/01/13  01:09:48  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <string.h>
#include "system_deps.h"
#include "header.h"

extern int yydebug;
extern int InputFs();
extern int yyparse();

static char* CurrentFile = "<stdin>";

void yyerror(str)
char*	str;
{
	fprintf(stderr, "%s: error: %s\n", CurrentFile, str);
	fprintf(stderr, "conversion failed\n");
	exit(1);
}

void yyaccept()
{
	exit(0);
}

SIGTYPE
sig_msg_restore_context()
{
	fprintf(stderr, "%s: conversion failed\n", CurrentFile);
	exit(1);
}

int main(argc, argv)
int	argc;
char*	argv[];
{	/* convert --- Convert a script from GENESIS 1.4.1 to 2.0 */

	FILE*	fs;
	int	status;
	int	arg;

	SetX1compat(1);

	/*yydebug = argc - 1;*/
	DisableOutput();

	initopt(argc, argv, "[old-script-file [new-script-file]] -noX1compat -constants script-files ...");
	while ((status = G_getopt(argc, argv)) == 1)
	    if (strcmp(G_optopt, "-constants") == 0)
		for (arg = 1; arg < optargc; arg++)
		  {
		    FILE*	fs;

		    CurrentFile = optargv[arg];
		    fs = fopen(CurrentFile, "r");
		    if (fs == NULL)
		      {
			perror(CurrentFile);
			return(-1);
		      }

		    InputFs(fs);
		    yyparse();
		    fclose(fs);
		  }
	    else if (strcmp(G_optopt, "-noX1compat") == 0)
		SetX1compat(0);

	if (status != 0 || optargc > 3)
	  {
	    printoptusage(argc, argv);
	    return(1);
	  }

	if (optargc > 1)
	  {
	    CurrentFile = optargv[1];
	    fs = fopen(CurrentFile, "r");
	    if (fs == NULL)
	      {
		perror(CurrentFile);
		return(1);
	      }
	  }
	else
	  {
	    CurrentFile = "<stdin>";
	    fs = stdin;
	  }

	if (optargc == 2)
	  {
	    /*
	    ** Use basename of input file as output file name
	    */

	    char*	cp;

	    for (cp = optargv[1]+strlen(optargv[1]) - 1; cp > optargv[1]; cp--)
		if (*cp == '/')
		  {
		    cp++;
		    break;
		  }

	    /*
	    ** if the input file name has no directory path then output to
	    ** stdout.
	    */

	    if (cp != optargv[1])
		freopen(cp, "w", stdout);
	  }
	else if (optargc == 3)
	    freopen(optargv[2], "w", stdout);

	EnableOutput();
	InputFs(fs);
	nextchar(1);
	yyparse();

	return 0;
}	/* convert */
