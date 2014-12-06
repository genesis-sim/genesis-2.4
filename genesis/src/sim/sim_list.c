static char rcsid[] = "$Id: sim_list.c,v 1.3 2005/10/24 06:33:05 svitak Exp $";

/*
** $Log: sim_list.c,v $
** Revision 1.3  2005/10/24 06:33:05  svitak
** Fixed decls missing types for argc and argv.
**
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.9  2000/09/11 22:46:33  mhucka
** Fixed missing argument in a printf call.
**
** Revision 1.8  2000/09/07 02:44:25  mhucka
** Added function do_count_list() from Hugo Cornelis.
**
** Revision 1.7  2000/06/12 04:53:44  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.6  2000/05/19 05:26:55  mhucka
** Removed extraneous statement that was never reached.
**
** Revision 1.5  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1994/10/14 16:59:00  dhb
** Changed to do_construct_list() to allow getting an element list
** from an element's field caused element lists obtained by wildcard
** pathnames never to be freed.  Fixed it.
**
 * Revision 1.3  1994/04/08  23:34:18  dhb
 * Update from Upi
 *
 * Revision 1.2  1994/04/07  13:36:42  bhalla
 * Added option to do_construct_list which allows it to build
 * a list from an ElementList struct in an element.
 *
 * Revision 1.1  1994/03/23  16:14:13  bhalla
 * Initial revision
 *
 * Revision 1.2  1993/02/23  19:10:32  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_construct_list changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:14:41  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

char **do_construct_list(argc,argv)
int	argc;
char	**argv;
{
ElementList	*list;
char		**arglist;
int		i;
int		status;
int		do_use_listname = 0;
char		*listname = NULL;
short		datatype;

    /* The listname argument tells it to look for the field
    ** <listname> on the element in <path> and treat that
    ** as an element list */
    initopt(argc, argv, "path -listname listname");
    while ((status = G_getopt(argc,argv)) == 1) {
	if (strcmp(G_optopt,"-listname") == 0)
	    do_use_listname = 1;
	    listname = optargv[1];
    }
    if (status < 0)
      {
	printoptusage(argc, argv);
	return(NULLArgv());
      }
    
    if (do_use_listname) {
	Element *elm;
	if ((elm = GetElement(optargv[1])) == NULL) {
		Error();
		printf("could not find element %s\n", optargv[1]);
		return(NULLArgv());
	}
	list = *(ElementList **) GetFieldAdr(elm,listname,&datatype);
	if (!list || datatype != INVALID) {
	    Error();
	    printf("no elist '%s' on %s\n",listname,optargv[1]);
	    return(NULLArgv());
	}
    } else {
    	list = WildcardGetElement(optargv[1],0);
	if (!list) {
	    Error();
	    printf("Cannot make elist with  %s\n",optargv[1]);
	    return(NULLArgv());
	}
    }

    if (list->nelements > 0) {
	/* I wonder how these are meant to be freed */
    	arglist = (char **)calloc(list->nelements +1,sizeof(char *));
    	for(i=0;i<list->nelements;i++){
		arglist[i] = CopyString(Pathname(list->element[i]));
    	}
	if (!do_use_listname) FreeElementList(list);
	return(arglist);
    } else {
	if (!do_use_listname) FreeElementList(list);
	return(NULLArgv());
    }
}

int do_count_list(argc,argv)
int argc;
char **argv;
{
ElementList	*list;
int		i;
int		status;
int		do_use_listname = 0;
char		*listname = NULL;
short		datatype;

    /* The listname argument tells it to look for the field
    ** <listname> on the element in <path> and treat that
    ** as an element list */
    initopt(argc, argv, "path -listname listname");
    while ((status = G_getopt(argc,argv)) == 1) {
	if (strcmp(G_optopt,"-listname") == 0)
	    do_use_listname = 1;
	    listname = optargv[1];
    }
    if (status < 0)
      {
	printoptusage(argc, argv);
	return(-1);
      }
    
    if (do_use_listname) {
	Element *elm;
	if ((elm = GetElement(optargv[1])) == NULL) {
		Error();
		printf("could not find element %s\n", optargv[1]);
		return(-1);
	}
	list = *(ElementList **) GetFieldAdr(elm,listname,&datatype);
	if (!list || datatype != INVALID) {
	    Error();
	    printf("no elist '%s' on %s\n",listname,optargv[1]);
	    return(-1);
	}
    } else {
    	list = WildcardGetElement(optargv[1],0);
	if (!list) {
	    Error();
	    printf("Cannot make elist with  %s\n",optargv[1]);
	    return(-1);
	}
    }

    /*- set result */

    i = list->nelements;

    if (!do_use_listname) FreeElementList(list);

    return(i);
}


