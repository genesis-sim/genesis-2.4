static char rcsid[] = "$Id: efield.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: efield.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:35  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1992/10/29 16:00:12  dhb
** Changed direct reference to MsgIn slot to use MSGSLOT macro.
** Note: Code for the RECALC action pokes data into elements sending
**       data to the statebuf.  BOGUS!!!
**
*/

#include <math.h>
#include "dev_ext.h"

/*
** an extracellular field potential recording electrode that uses
** current sources and their distance from the electrode site
** to calculate the field.
*/
/* M.Wilson Caltech 2/89 */
void ExtracellularRecordingElectrode(amp,action)
register struct efield_type *amp;
Action		*action;
{
MsgIn	*msg;
double	distance;

    if(debug > 1){
	ActionHeader("ExtracellularRecordingElectrode",amp,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
	amp->field = 0;
	MSGLOOP(amp,msg) {
	    case 0:				/* current source */
		/*
		** 0 = current
		** 1 = distance from electrode
		*/
		amp->field += MSGVALUE(msg,0)/MSGVALUE(msg,1);
		break;
	}
	amp->field *= amp->scale; 
	break;
    case RESET:
	amp->field = 0;
	/*
	** compute current source distance
	*/
    case RECALC:
	MSGLOOP(amp,msg) {
	    case 0:				/* current source */
		/*
		** 0 = current
		** 1 = distance from electrode
		*/
		if((distance = sqrt(
		pow(msg->src->x - amp->x,2.0) +
		pow(msg->src->y - amp->y,2.0) +
		pow(msg->src->z - amp->z,2.0)
		)) <= 0){
		    distance = 1e-30;
		};
		*((double *)(MSGSLOT(msg)[1].data)) = distance;
		break;
	}
	break;
    }
}
