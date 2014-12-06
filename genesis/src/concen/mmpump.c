static char rcsid[] = "$Id: mmpump.c,v 1.3 2005/07/01 10:03:00 svitak Exp $";

/* Version EDS22b 97/11/14, Erik De Schutter, BBF-UIA 7/94-11/97 */

/*
** $Log: mmpump.c,v $
** Revision 1.3  2005/07/01 10:03:00  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
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
 * EDS22b revison: EDS BBF-UIA 97/11/14
 * Added to comments
 *
** Revision 1.2  1997/05/28 21:23:50  dhb
** Added RCS id and log headers
**
*/

/* Implementation of a simple pump with Michaelis Menten kinetics. 
** This is a simple model for the plasma membrane pump (PMCA).
** Should be coupled to a difshell, where the changes in concentration 
**  is computed.  Note that this object does not compute anything by
**  itself, except if the electrogenic action is simulated (val!=0).  
**  By making it a  separate object, multiple pumps are possible */

#include "conc_ext.h"

void MMPump(pump,action)
register Mpump *pump;
Action		*action;
{
double	concen = 0.0;
int	n;
MsgIn 	*msg;

    if(debug > 1){
		ActionHeader("mmpump",pump,action);
    }

    SELECT_ACTION(action){
    case INIT:
		break;
    case PROCESS: 
	
	/* Read the msgs  */
	MSGLOOP(pump,msg){
		case CONCEN:	/* concentration, compute electrogenic effect */
			concen = MSGVALUE(msg,0);
			break;
		case MMKD:	/* Kd */
			pump->Kd = MSGVALUE(msg,0);
			break;
	}

	if (pump->val) {
	    pump->Ik=pump->mmconst * concen / (pump->Kd + concen);
	}
	break;

    case RESET:
	if (pump->val) {
	    n=0;
	    /* Read the msgs  */
	    MSGLOOP(pump,msg){
		case CONCEN:	/* concentration, compute electrogenic effect */
			concen = MSGVALUE(msg,0);
			n++;
			break;
		case MMKD:	/* Kd */
			pump->Kd = MSGVALUE(msg,0);
			break;
	    }
	    pump->mmconst=pump->vmax * pump->val * FARADAY;
	    pump->Ik=pump->mmconst * concen / (pump->Kd + concen);
	}
	break;

    case CHECK:
	if(pump->vmax <= 0.0)
		ErrorMessage("mmpump", "Invalid vmax.", pump);
	if(pump->Kd <= 0.0)
		ErrorMessage("mmpump", "Invalid Kd.", pump);
	if (pump->val) {
	    n=0;
	    /* Read the msgs  */
	    MSGLOOP(pump,msg){
		case CONCEN:	/* concentration, compute electrogenic effect */
			n++;
			break;
	    }
	    if(!n)
		ErrorMessage("mmpump", "CONCEN msg needed to compute electrogenic effect.", pump);
	}
	break;
    }
}

