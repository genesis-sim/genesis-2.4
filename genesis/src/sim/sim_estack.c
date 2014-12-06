static char rcsid[] = "$Id: sim_estack.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_estack.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/15 23:13:03  dhb
** 1.4 to 2.0 command argument changes.
**
** 	do_pushe (push) changed to use GENESIS getopt routines.  do_pope (pop)
** 	and do_show_estack (stack) unchanged.
**
 * Revision 1.1  1992/10/27  20:01:06  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define STK_SIZE	100

static Element	*element_stk[STK_SIZE];
static short	sp;


Element *pop_element()
{
    if(--sp >= 0){
	return(element_stk[sp]);
    } else 
    {
	Error();
	printf("empty element stack\n");
	sp = 0;
	return(NULL);
    }
}

int push_element(element)
Element	*element;
{
    if(element == NULL){
	return(0);
    }
    element_stk[sp] = element;
    if(++sp > STK_SIZE){
	Error();
	printf("element stack overflow '%s'\n",element->name);
	sp = STK_SIZE;
	return(0);
    }
    return(1);
}

void do_show_estack(argc,argv)
int argc;
char **argv;
{
int i;

    for(i=0;i<sp;i++){
	printf("%s\n",Pathname(element_stk[i]));
    }
}

void do_pushe(argc,argv)
int 	argc;
char 	**argv;
{
Element *element;

    initopt(argc, argv, "[path]");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc > 1){
	element = GetElement(optargv[1]);
	if(element == NULL){
	    printf("cant find element '%s'\n",optargv[1]);
	    return;
	}
    } else {
	element = WorkingElement();
    }
    push_element(WorkingElement());
    SetWorkingElement(element);
    if(IsSilent()<1)
	printf("%s\n",Pathname(element));
}

void do_pope(argc,argv)
int 	argc;
char 	**argv;
{
Element *element;

    element = pop_element();
    if(element != NULL){
	SetWorkingElement(element);
	if(IsSilent()<1)
	    printf("%s\n",Pathname(element));
    }
}
