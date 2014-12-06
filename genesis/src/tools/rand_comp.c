static char rcsid[] = "$Id: rand_comp.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: rand_comp.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  1997/08/08 19:28:03  dhb
** Added missing CopyString() for return value from do_rand_comp().
**
** Revision 1.3  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 18:11:12  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:06:05  dhb
** Initial revision
**
*/

/* Erik De Schutter, 8/91 */
/* Returns name of a randomly selected compartment from {root} {path} */

#include <stdio.h>
#include "sim_ext.h"
#include "shell_func_ext.h"
#include "tools.h"
#include "seg_struct.h"

char *do_rand_comp(argc,argv)
/* As there is no easy way to know how many compartments there are, we
**  randomly determine in each one whether it should be selected.  An
**  individual compartment has a chance of 1.0% to be selected, unless the
**  user specifies another probability */  
int argc;
char **argv;

{
Element		*elm,*path,*root;
char		name[100];
float		prob = 0.010;
int		status;

	initopt(argc, argv, "root-element [path] -probability p");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-probability") == 0)
		prob = Atof(optargv[1]);
	  }

	if (status < 0 || optargc > 3) {
		printoptusage(argc, argv);
		printf("\n");
		printf("returns pseudo-randomly selected compartment from tree root\n");
		printf("on repeated calls path should be the result from the preceeding call\n");
		return NULL;
	}

	root = GetElement(optargv[1]);
	if (!root) {
		fprintf(stderr,"could not find (sym)compartment '%s'\n",optargv[1]);
		return(NULL);
	}
	if (optargc == 3) {
		path = GetElement(optargv[2]);
		if (!path) {
			fprintf(stderr,"could not find (sym)compartment '%s'\n",optargv[2]);
			return(NULL);
		}
		path = path->next;
	} else {
		path = root;
	}

	for (;;) {
		for (elm=path;elm;elm=elm->next) {
			if ((strcmp(elm->object->name,"compartment") == 0) ||
				(strcmp(elm->object->name,"symcompartment") == 0)) {
				if (urandom() < prob) {
					sprintf (name,"%s[%d]",elm->name,elm->index);
					return(CopyString(name));
				}
			}
		}
		path = root;
	}
}
