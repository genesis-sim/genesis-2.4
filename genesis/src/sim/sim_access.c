static char rcsid[] = "$Id: sim_access.c,v 1.2 2005/06/27 19:00:57 svitak Exp $";

/*
** $Log: sim_access.c,v $
** Revision 1.2  2005/06/27 19:00:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/16 22:21:32  dhb
** 1.4 to 2.0 command argument changes.
**
** 	do_enable (enable) and do_disable (disable) changed to use GENESIS
** 	getopt routines.
**
 * Revision 1.1  1992/10/27  19:25:34  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
/* Modified '91, by Upi Bhalla to put in Hsolve related funcs */


void Enable(element)
Element *element;
{
    if(element != NULL)
    element->flags &= ~BLOCKMASK;
}

void Block(element)
Element *element;
{
    if(element != NULL)
    element->flags |= BLOCKMASK;
}

int IsEnabled(element)
Element *element;
{
    if(element != NULL)
	return((element->flags & BLOCKMASK) == 0);
    else
    return(FALSE);
}

void HsolveEnable(element)
Element *element;
{
    if(element != NULL)
    element->flags &= ~HSOLVEMASK;
}

void HsolveBlock(element)
Element *element;
{
    if(element != NULL)
    element->flags |= HSOLVEMASK;
}

int IsHsolved(element)
Element *element;
{
    if(element != NULL)
	return((element->flags & HSOLVEMASK) != 0);
    else
    return(FALSE);
}

int IsActive(element)
Element *element;
{
    if(element != NULL && element->object != NULL){
	return(element->object->function != NULL);
    } else {
	return(0);
    }
}

int do_enable(argc,argv)
int	argc;
char	**argv;
{
Element		*element;

    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    if((element = GetElement(optargv[1])) == NULL){
	Error();
	printf("cannot find element '%s'\n", optargv[1]);
	return(0);
    }
    Enable(element);
    OK();
    return(1);
}

int do_disable(argc,argv)
int	argc;
char	**argv;
{
Element		*element;

    initopt(argc, argv, "element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    if((element = GetElement(optargv[1])) == NULL){
	Error();
	printf("cannot find element '%s'\n", optargv[1]);
	return(0);
    }
    Block(element);
    OK();
    return(1);
}
