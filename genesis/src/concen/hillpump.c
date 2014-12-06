/* Version EDS227 99/03/24, Erik De Schutter, BBF-UIA 11/97-3/99 */

/* EDS22h revison: EDS BBF-UIA 99/03/10-99/03/24
 * updated RESET for negative vmax
*/

/* Implementation of a simple pump with Michaelis Menten kinetics. 
** This is a simple model for the store membrane pump (SERCA).
** Should be coupled to a concpool, where the changes in concentration 
**  is computed.  Note that this object does not compute anything by
**  itself.  By making it a  separate object, multiple pumps are possible */

#include <math.h>
#include "conc_ext.h"

void HillPump(pump,action)
register Hpump *pump;
Action		*action;
{
MsgIn 	*msg;

    if(debug > 1){
		ActionHeader("hillpump",pump,action);
    }

    SELECT_ACTION(action){
    case INIT:
		break;
	
    case PROCESS: 
    case RESET:
	/* Read the msgs  */
	MSGLOOP(pump,msg){
		case MMKD:	/* Kd */
			pump->Kd = MSGVALUE(msg,0);
			break;
	}
	pump->vmax=fabs(pump->vmax);	/* to remove negative values from readcell call */

	break;

    case CHECK:
	if(pump->vmax <= 0.0)
		ErrorMessage("hillpump", "Invalid vmax.", pump);
	if(pump->Kd <= 0.0)
		ErrorMessage("hillpump", "Invalid Kd.", pump);
	if(pump->Hill <= 0)
		ErrorMessage("hillpump", "Invalid Hill.", pump);
	break;
    }
}

