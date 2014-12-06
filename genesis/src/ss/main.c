static char rcsid[] = "$Id: main.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $";

/*
** $Log: main.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:03  dhb
** Initial revision
**
*/

#include <stdio.h>
#include "y.tab.h"
#include "parse.h"

Result ExecuteCommand(argc, argv)

int	argc;
char	*argv[];

{	/* ExecuteCommand --- Stub which prints command */

	Result	r;
	int	i;

	for (i = 0; i < argc; i++)
	    fprintf(stderr, "%s ", argv[i]);
	fprintf(stderr, "\n");

	r.r_type = INT;
	r.r.r_int = 0;
	return(r);

}	/* ExecuteCommand */


yyerror(s)

char	*s;

{

	fprintf(stderr, "%s\n", s);
	exit(1);

}

main()

{

	yyparse();

}
