static char rcsid[] = "$Id: unit.c,v 1.2 2005/07/01 10:03:08 svitak Exp $";

/*
** $Log: unit.c,v $
** Revision 1.2  2005/07/01 10:03:08  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1992/10/29 18:41:07  dhb
** Replaced explicit msgin looping with MSGLOOP macro.
**
 * Revision 1.1  1992/10/29  18:39:46  dhb
 * Initial revision
 *
*/

#include "seg_ext.h"

int	DEBUG_Unit = 0;

/* 7/88 Matt Wilson */
void Unit(unit,action)
register struct unit_type 	*unit;
Action				*action;
{
double				dv_dt;
double				dt;
MsgIn				*msg;

    if(Debug(DEBUG_Unit) > 1){
	printf("Unit '%s' action '%s'.\n",
	Pathname(unit),
	Actionname(action)
	);
    }
    switch(action->type){
    case PROCESS:
	dt = Clockrate(unit);
	/*
	** leakage component
	*/
	dv_dt = (unit->Em - unit->state)/unit->Rm  
		+ unit->inject + unit->activation;
	MSGLOOP(unit, msg) {
	    /*
	    ** type 0 : current injection
	    **     0 = Ik
	    */
	    case 0:
		dv_dt += MsgValue(msg,double,0);
		break;
	}
	dv_dt /= unit->Cm;
	/*
	** INTEGRATION
	*/
	unit->state = 
	DirectIntegrate(unit->object->method,unit,unit->state,dv_dt,dt,"Ustate");
	break;
    case INIT:
	unit->activation = 0;
	break;
    case RESET:
	unit->activation = 0;
	unit->state = unit->Em;
	break;
    case CHECK:
	if(unit->Cm <= 0.0){
	    ErrorMessage("Unit","Cm <= 0.",unit);
	}
	if(unit->Rm == 0.0){
	    ErrorMessage("Unit","Rm not set.",unit);
	}
	break;
    }
}
