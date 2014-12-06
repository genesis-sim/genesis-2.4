static char rcsid[] = "$Id: taupump.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: taupump.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:32  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1999/10/17 22:13:23  mhucka
** New version from Erik De Schutter, dated circa March 1999.
**
 * EDS22c revison: EDS BBF-UIA 98/03/13
 * Corrected bug in PROCESS
 *
** Revision 1.2  1997/05/28 21:23:50  dhb
** Update from Antwerp GENESIS
**
** Revision 1.1  1992/12/11 19:02:42  dhb
** Initial revision
**
*/

/* Version EDS20i 95/06/02, Erik De Schutter, Caltech & BBF-UIA 1/91-6/95 */
#include <math.h>
#include "conc_ext.h"

/* Implementation of a simple pump with a variable time constant 
**  of removal.  Should be coupled to a difshell, where the changes 
**  in concentration is computed. */

/* 1/91 Erik De Schutter */
/* 5/92 EDS, added T_C field: a constant offset for tau */
/* 9/94 EDS, some esthetic improvements */
/* 6/95 EDS, removed dt */

void TauPump(pump,action)
register Tpump *pump;
Action		*action;
{
double	tau;
MsgIn 	*msg;
int		nv = 0;

    if(debug > 1){
	ActionHeader("taupump",pump,action);
    }

    SELECT_ACTION(action){
    case INIT:
	break;

    case PROCESS:
	
	/* Read the msgs to get the membrane voltage */
	/* Tau = T_A * exp ((v-T_V)/T_B) + T_C 
	** kP = 1/Tau */
	if (pump->T_C < 1e-20) {
		tau = pump->T_A;
	} else {
		tau = pump->T_C;
	}
	MSGLOOP(pump,msg){
		case 0:		/* voltage, or any other parameter to compute tau*/
					/* 0 = Vm */
		tau = pump->T_A*exp((MSGVALUE(msg,0)-pump->T_V)/pump->T_B)+pump->T_C;
		break;
	}
	pump->kP = 1.0/tau;
	break;

    case RESET:
	/* backward compatibility: if T_C is not set, use T_A as default value
	** for tau */
	if (pump->T_C < 1e-20) {
		tau = pump->T_A;
	} else {
		tau = pump->T_C;
	}
	/* check all the messages to the pump and calculate tau */
	MSGLOOP(pump,msg){
		case 1:		/* voltage, or any other parameter to compute tau*/
					/* 0 = Vm */
		tau = pump->T_A*exp((MSGVALUE(msg,0)-pump->T_V)/pump->T_B)+pump->T_C;
		break;
	}
	pump->kP = 1.0/tau;
	break;
    
	case CHECK:
	if(pump->Ceq < 0.0)
	    ErrorMessage("taupump", "Invalid Ceq.", pump);
	if(pump->T_A <= 0.0)
	    ErrorMessage("taupump", "Invalid T_A.", pump);
	MSGLOOP(pump,msg){
		case 1:		/* only one voltage signal allowed! */
		nv += 1;
		break;
	}
	if(nv > 1)
	    ErrorMessage("taupump", "Too many VOLTAGE msg.", pump);
	if((nv==1)&&(pump->T_B == 0.0))
	    ErrorMessage("taupump", "Invalid T_B.", pump);
	break;
    }
}

