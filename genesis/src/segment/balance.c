static char rcsid[] = "$Id: balance.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: balance.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:04:42  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/25 18:45:51  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:58  dhb
** Initial revision
**
*/

#include "seg_ext.h"

/* M.Wilson Caltech 4/89 */
void do_balance_Em(argc,argv)
int argc;
char **argv;
{
ElementList *list;
struct compartment_type *compartment;
float	A;
float	B;
MsgIn	*msg;
float	voltage;
int 	i;

    initopt(argc, argv, "path voltage");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    list = WildcardGetElement(optargv[1],1);
    voltage = Atof(optargv[2]);
    for(i=0;i<list->nelements;i++){
	compartment = (struct compartment_type *)list->element[i];
	if(strcmp(compartment->object->type,"compartment_type") != 0){
	    /*
	    ** its not a compartment so skip it
	    */
	    continue;
	}
	/*
	** Adjust Em to give no net membrane current at given voltage
	**
	** A = SUM(Ek*Gk)
	** B = SUM(Gk)
	*/
	A = B = 0;
	MSGLOOP(compartment,msg){
	case 0:			/* channel */
	    /*
	    ** 0 = Gk ; 1 = Ek
	    */
	    A += MSGVALUE(msg,1)*MSGVALUE(msg,0);
	    B += MSGVALUE(msg,0);
	    break;
	}
	compartment->Em = (1+B*compartment->Rm)*voltage - A*compartment->Rm;
    }
    FreeElementList(list);
}

