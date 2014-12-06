static char rcsid[] = "$Id: periodic.c,v 1.2 2005/06/27 18:59:57 svitak Exp $";

/*
** $Log: periodic.c,v $
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/10/28 22:29:36  dhb
** Initial revision
**
*/

#include "buf_ext.h"

int	DEBUG_PeriodicEvent = 0;

/*
** update a pre-filled buffer with periodic wraparound
*/
/* 9/88 Matt Wilson */
void PeriodicEvent (periodic,action)
register struct periodic_type 	*periodic;
Action		*action;
{
float event_time;

    if(Debug(DEBUG_PeriodicEvent) > 1){
	ActionHeader("PeriodicEvent",periodic,action);
    }
    switch(action->type){
    case PROCESS:
	/*
	** update the buffer pointer to bring it up to date with the
	** current simulation time
	*/
	if(periodic->current != periodic->end && 
	periodic->current == periodic->start){
	    periodic->current = (periodic->current +1)%periodic->size;
	}
	/*
	** this can only occur if the buffer is empty
	*/
	if(periodic->current == periodic->start) return;
	/*
	** get the relative time of the current event, 
	** apply the expansion factor
	** and add the time of the beginning of the cycle
	*/
	event_time = periodic->scale*CurrentEvent(periodic).time 
	+ periodic->stime;

	/*
	** increment the pointers until reaching a time which
	** has not yet been reached
	*/
	while(SimulationTime() >= event_time - CORRECTION){ 
	    if((event_time >= SimulationTime() - Clockrate(periodic)/2) &&
	    (event_time <= SimulationTime() + Clockrate(periodic)/2)){
		periodic->state = CurrentEvent(periodic).magnitude;
	    } else {
		if(periodic->mode == 1){
		    /*
		    ** impulse mode
		    */
		    periodic->state = 0;
		}
	    }
	    if (periodic->current != periodic->end){ 
		periodic->current = (periodic->current + 1) % periodic->size;
	    } else {
		/*
		** if this is the end of the cycle then reset the new 
		** cycle indicators
		*/
		periodic->current = (periodic->start + 1) % periodic->size;
		periodic->stime = SimulationTime();
	    }
	    event_time = periodic->scale*CurrentEvent(periodic).time 
	    + periodic->stime;
	}
	break;
    case RESET:
	ResetBuffer(periodic);
	periodic->state = 0;
	periodic->stime = 0;
	break;
    default:
	InvalidAction("PeriodicEvent",periodic,action);
	break;
    }
}
