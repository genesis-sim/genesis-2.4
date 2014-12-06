static char rcsid[] = "$Id: spike.c,v 1.2 2005/06/27 18:59:57 svitak Exp $";

/*
** $Log: spike.c,v $
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1993/01/21 20:31:35  dhb
** Syntax error
**
 * Revision 1.2  1992/10/28  22:35:08  dhb
 * Updated to use MSGLOOP
 *
 * Revision 1.1  1992/10/28  22:29:40  dhb
 * Initial revision
 *
*/

#include "buf_ext.h"

int	DEBUG_SpikeEvent = 0;

#define Field(F)	(buffer->F)

/* 6/88 Matt Wilson */
void SpikeEvent (buffer,action)
register struct spike_type *buffer;
Action		*action;
{
double 	state = 0.0;
double 	thresh;
MsgIn	*msg;
int	stat;

    if(Debug(DEBUG_SpikeEvent) > 1){
	ActionHeader("SpikeEvent",buffer,action);
    }
    SELECT_ACTION(action){
    case RESET:
	ClearBuffer(buffer);
	break;
    case PROCESS:
	/*
	** set the default values
	*/
	thresh = buffer->thresh;
	MSGLOOP(buffer,msg){
	case 0:		/* potential */
	    state = MSGVALUE(msg,0);
	    break;
	case 1:		/* threshold */
	    thresh = MSGVALUE(msg,0);
	    break;
	}
	/*
	** evaluate the criteria for spiking.
	** has it exceeded threshold?
	** is it past its refractory state?
	*/
#ifdef FLOATTIME
	if(( state >= thresh) &&
	( !ValidEvent(buffer) ||
	(CurrentEventTime(buffer) <= 
	(SimulationTime() - buffer->abs_refract)
	+ClockValue(0)/2.0))) {
#else
	if(( state >= thresh) &&
	( !ValidEvent(buffer) ||
	(CurrentEventTime(buffer)*global_clock[buffer->clock] <= 
	(SimulationTime() - Field(abs_refract))))) {
#endif
	    /*
	    ** add a spike to the spike event buffer
	    */
	    PutEvent(buffer,(double)Field(output_amp),NULL,WRAP);
	    buffer->state = Field(output_amp);
	} else {
	    buffer->state = 0;
	}
	break;
    case CHECK:
	stat = 0;
	MSGLOOP(buffer, msg) {
	    case 0:
		stat = 1;
		break;
	}
	if(!stat){
	    ErrorMessage("Spike","State input is required.",buffer);
	}
	break;
    default:
	InvalidAction("Spike",buffer,action);
	break;
    }
}
#undef Field

