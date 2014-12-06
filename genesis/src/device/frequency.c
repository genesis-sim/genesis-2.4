static char rcsid[] = "$Id: frequency.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: frequency.c,v $
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
** Revision 1.1  1992/12/11 19:02:48  dhb
** Initial revision
**
*/

#include <math.h>
#include "dev_ext.h"

/*
** frequency measuring device which computes the running event frequency
** over a time interval using exponential weighting.
** monitor->tmin specifies the interval. 
*/
/* M.Wilson Caltech 2/89 */
void FrequencyMonitor(monitor,action)
register struct freq_mon_type *monitor;
Action		*action;
{
MsgIn	*msg;
double	state;
Event		*baseptr;
Event 		*startptr;
register	Event		*buffer_ptr;
int	buffer_size;
float	time;
int		trigger;


    if(debug > 1){
	ActionHeader("FrequencyMonitor",monitor,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
	state = 1;
	trigger = 0;
	MSGLOOP(monitor,msg) {
	    case 0:				/* non-zero */
		trigger = (MSGVALUE(msg,0) != 0);
		break;
	    case 1:				/* zero */
		trigger = (MSGVALUE(msg,0) == 0);
		break;
	    case 2:				/* positive non-zero */
		trigger = (MSGVALUE(msg,0) > 0);
		break;
	    case 3:				/* negative non-zero */
		trigger = (MSGVALUE(msg,0) < 0);
		break;
	    case 4:				/* window size */
		monitor->tmin = (MSGVALUE(msg,0) < 0);
		break;
	    case 5:				/* continuous input */
		state = MSGVALUE(msg,0);
		trigger = 1;
		break;
	}
	/*
	** if an event has occurred then add it to the buffer
	*/
	if(trigger){
	    PutEvent(monitor,(double)state,NULL,WRAP);
	}
	/*
	** scan the buffer
	** see sim_abs.c for comments
	*/
	monitor->frequency = 0;
	buffer_size = monitor->size;
	baseptr = monitor->event;
	buffer_ptr = baseptr + monitor->current;
	startptr = baseptr + monitor->start;
	if(buffer_ptr == NULL) return;
	while ((time = simulation_time - buffer_ptr->time) <= monitor->tmin) {
	    /*
	    ** compute the frequency
	    */
	    if(monitor->exponent == 0){
		monitor->frequency += buffer_ptr->magnitude;
	    } else {
		/* exponentially weighted interval average */
		monitor->frequency += exp(monitor->exponent*time)*
		buffer_ptr->magnitude;
	    }
	    /*
	    ** decrement the buffer pointers
	    */
	    if (--buffer_ptr < baseptr){
		buffer_ptr = baseptr + buffer_size - 1;
	    }
	    if (buffer_ptr == startptr){
		buffer_ptr = baseptr + (monitor->start +1) % buffer_size;
		break;
	    }
	}
	/*
	** divide the total sum by the time window
	*/
	monitor->frequency /= monitor->tmin;
	break;
    case RESET:
	ClearBuffer(monitor);
	monitor->frequency = 0;
	break;
    }
}
