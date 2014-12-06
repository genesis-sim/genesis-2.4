static char rcsid[] = "$Id: rcunit.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: rcunit.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1993/01/21 20:36:49  dhb
** Changed to use MSGLOOP macro
**
 * Revision 1.1  1992/10/29  18:26:26  dhb
 * Initial revision
 *
*/

#include "seg_ext.h"

int	DEBUG_RC_Unit = 0;

/* 10/88 Matt Wilson */
void RC_Unit(unit,action)
struct RCunit_type 	*unit;
Action			*action;
{
MsgIn				*msg;
double				A;

    if(Debug(DEBUG_RC_Unit) > 1){
	ActionHeader("RC_Unit",unit,action);
    }
    switch(action->type){
    case PROCESS:
	A = unit->inject + unit->V0/unit->R;
	MSGLOOP(unit, msg) {
	  default: /* Shouldn't this be case 0: ? */
	    /*
	    ** type 0 : current injection
	    **    0 = Ik
	    */
	    A += MsgValue(msg,double,0);
	}
	/*
	** INTEGRATION
	*/
	unit->state = 
	IntegrateMethod(unit->object->method, unit,unit->state,
	A/unit->C,1/(unit->R*unit->C),
	Clockrate(unit),"Ustate");
	break;
    case RESET:
	unit->state = unit->V0;
	break;
    case CHECK:
	/*
	** check RC to insure it is non-zero
	*/
	if(unit->R <= 0.0 || unit->C <= 0.0){
	    ErrorMessage("RC_Unit","Invalid RC time constant",unit);
	}
	break;
    }
}
