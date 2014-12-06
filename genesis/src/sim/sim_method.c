static char rcsid[] = "$Id: sim_method.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_method.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3  1994/08/31 02:42:40  dhb
** Changed integration method numbering and disabled broken methods.
**
** Also fixed a bug where silent >= 1 would disable setting of method.
**
 * Revision 1.2  1993/02/23  00:01:58  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_set_method changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:18:48  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

/*
** Setting integration methods. The method is stored in the method
** field of the object associated with each element. 
*/

void do_set_method(argc,argv)
int	argc;
char	**argv;
{
ElementList	*list;
Element		**elms;
int		i,j;
short	method;
GenesisObject	*new_obj,*old_obj;

    initopt(argc, argv, "[path] method");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
		printoptusage(argc, argv);
		printf("If no path is specified then the method is globally changed\n");
		return;
      }

	if (optargc == 2) { /* Global change of method */
		method = atoi(optargv[1]);
		if (!check_method(method)) return;
		/*
		** Scan through all elements recursively to set their
		** object->method
		** fields. This will have to be changed for parallelising.
		*/
		SetIntMethods(RootElement(),method);
	} else { /* changing method on path */
		method = atoi(optargv[2]);
		if (!check_method(method)) return;
		list = WildcardGetElement(optargv[1],0);
		elms = list->element;
		/*
		** Algorithm : identify the object for each entry in the list
		** and duplicate it with the new method. Scan the remainder of
		** the list for identical objects and replace them with the
		** duplicate. Null out elements on the list which have had
		** their method changed in this way.
		** 
		** First, scan element list for non-null entries
		*/
		for(i=0;i<list->nelements;i++) {
			if (elms[i]) {
				old_obj=elms[i]->object;
				/* 
				** A little bit of space stinginess : don't bother
				** with altering the neutrals in the path
				*/
				if (strcmp(old_obj->name,"neutral")==0)
					continue;
				/*
				** A bit more stinginess : if the old method is the
				** same as the new one, dont bother
				*/
				if (old_obj->method == method)
					continue;

				/*
				** If the method is an implicit one, only apply it to
				** hsolve elements.
				*/
				if (method >= BEULER_INT) {
					if (strcmp(old_obj->name,"hsolve") != 0)
						continue;
				}
				/*
				** If the method must be changed, make a copy of the
				** object, with only the method altered.
				*/
				new_obj=(GenesisObject *)malloc(sizeof(GenesisObject));
				BCOPY(old_obj,new_obj,sizeof(GenesisObject));
				new_obj->method = method;
				/*
				** Scan the rest of the list for identical old objects,
				** swapping for the new ones and nulling out the entries
				** on the elmlist.
				*/
				for(j=i;j<list->nelements;j++) {
					if (elms[j] && elms[j]->object == old_obj) {
						elms[j]->object = new_obj;
						elms[j] = NULL;
					}
				}
			}
		}
		FreeElementList(list);
	}
}

void SetIntMethods(elm,method)
	Element	*elm;
	short	method;
{
	Element *child;

	if ((method != BEULER_INT && method != CRANK_INT) || strcmp(elm->object->name,"hsolve") == 0)
		elm->object->method = method;
	for(child=elm->child;child;child=child->next)
		SetIntMethods(child,method);
}

#define	SetMethodStr(str)	methodstr = str
int check_method(method)
	short method;
{
	int	valid;
	char*	methodstr;


	valid = 1;
		switch(method) {
			case FEULER_INT : SetMethodStr("Forward Euler\n");
				break;
			case EEULER_INT : SetMethodStr("Exponential Euler\n");
				break;
			/*
			case GEAR_INT : SetMethodStr("Gear\n");
				break;
			*/
			case AB2_INT : SetMethodStr("Adams-Bashforth 2-step\n");
				break;
			case AB3_INT : SetMethodStr("Adams-Bashforth 3-step\n");
				break;
			/*
			case TRAPEZOIDAL_INT : SetMethodStr("Trapezoidal\n");
				break;
			*/
			case AB4_INT : SetMethodStr("Adams-Bashforth 4-step\n");
				break;
			case AB5_INT : SetMethodStr("Adams-Bashforth 5-step\n");
				break;
			/*
			case RK_INT : SetMethodStr("Runge-Kutta\n");
				break;
			*/
			case BEULER_INT : SetMethodStr("Backward Euler. Note : applies only to hsolve elements\n");
				break;
			case CRANK_INT : SetMethodStr("Crank-Nicolson. Note : applies only to hsolve elements\n");
				break;
			/*
			case EPC_INT : SetMethodStr("Euler predictor-corrector : not yet implemented\n");
			*/
			default : 
				Error(); printf("Invalid method: %d\n", method);
				printf("  Valid methods :\n");
				/*
				printf("  4	: Trapezoidal\n");
				printf("  1	: Gear\n");
				printf("  7	: Runge-Kutta\n");
				*/
				printf("  -1	: Forward Euler\n");
				printf("  0	: Exponential Euler\n");
				printf("  2	: Adams-Bashforth 2-step\n");
				printf("  3	: Adams-Bashforth 3-step\n");
				printf("  4	: Adams-Bashforth 4-step\n");
				printf("  5	: Adams-Bashforth 5-step\n");
				printf(" 10	: Backward Euler - only for hsolve elements\n");
				printf(" 11	: Crank-Nicolson - only for hsolve elements\n");
				valid = 0;
				break;
		}

	if (valid && IsSilent() < 1) {
	    printf(methodstr);
	}

	return(valid);
}
#undef SetMethodStr
