static char rcsid[] = "$Id: delay.c,v 1.2 2005/07/01 10:03:05 svitak Exp $";

/*
** $Log: delay.c,v $
** Revision 1.2  2005/07/01 10:03:05  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1995/03/29 21:15:32  dhb
** CREATE and COPY actions now return valid status results.
**
** Revision 1.4  1994/10/20  22:25:03  dhb
** Added CREATE and COPY actions to copy connections when necessary.
**
** Revision 1.3  1994/09/16  20:24:58  dhb
** Moved connection RESET code from sim_reset.c into RESET action.
**
** Revision 1.2  1994/06/15  20:56:20  dhb
** Moved check of projections from sim_check.c into projections.
**
** Revision 1.1  1992/12/11  19:02:25  dhb
** Initial revision
**
*/

#include "axon_ext.h"

int	DEBUG_DelayConnection = 0;

int DelayConnection(projection,action)
register struct delay_projection_type	*projection;
Action		*action;
{
#ifdef FLOATTIME
float		duration;
float		leading_edge;
float		offset_time;
float		latency;
float		tmax;
float		tmin;
float		t1;
float		simtime_minus_latency;
float		error_correction;
#else
int		duration;
int		leading_edge;
int		offset_time;
int		latency;
int		tmax;
int		tmin;
int		t1;
int		simtime_minus_latency;
#endif

float		clock;
int		buffer_size;

Connection	*connection;
Event		*baseptr;
Event 		*startptr;

register	Connection	*connection_ptr;
register	Event		*buffer_ptr;

Buffer 		*buffer = NULL;

PFI		connection_func;
MsgIn		*msg;
Element*	orig_elm;

    if(Debug(DEBUG_DelayConnection) > 1){
	ActionHeader("DelayConnection",projection,action);
    }

    SELECT_ACTION(action){
    case RESET:
	ResetProjections(projection);
	/* Fall through to RECALC code */

    case RECALC:
	/*
	** sets the values of tmax for projections and
	** tmin for buffers which are necessary for the
	** propagation of events.
	** This must be done whenever a parameter affecting the
	** propagation delay is modified, such as delay, duration, or latency.
	*/
	projection->tmax = 0;
	/*
	** for each each connection of the projection
	*/
	for(connection=projection->connection;connection;
	connection=connection->next){
	    /*
	    ** determine the maximum delay along this projection
	    */
	    if(connection->delay > projection->tmax){
		    projection->tmax = connection->delay;
	    }
	}
	/*
	** and use this to 
	** set the tmin of buffers which are attached to
	** the projection
	*/
	/*
	** get the projection buffer from the msg
	*/
	buffer = NULL;
	MSGLOOP(projection,msg){
	    case 0:
		buffer = (Buffer *)MSGPTR(msg,0);
		break;
	}
#ifdef FLOATTIME
	if(buffer){
	    tmin = projection->tmax +
	    projection->latency + projection->duration;
	    if(projection->duration == 0.0){
		tmin += Clockrate(projection);
	    }
	    /*
	    ** if the duration is 0 then use dt instead
	    */
	    if(buffer->tmin < tmin){
		buffer->tmin = tmin;
	    }
	}
#else
	if(buffer){
	    tmin = projection->tmax +
	    round(projection->latency/Clockrate(projection)) +
	    round(projection->duration/Clockrate(projection));
	    if(buffer->tmin < tmin){
		buffer->tmin = tmin;
	    }
	}
#endif
	break;
    case PROCESS:
	if(projection->connection == NULL) return 0;
	/*
	** get the buffer to be used from the msg
	*/
	MSGLOOP(projection,msg){
	    case 0:			/* buffer */
		buffer = (Buffer *)MSGPTR(msg,0);
		break;
	}
	/*
	** check to see if there is anything in the buffer
	*/
	if(!buffer || (buffer->current == buffer->start)){
	    /*
	    ** there isnt so return
	    */
	    return 0;
	}

	/* 
	** connection contains the connection information about individual 
	** connections
	*/
	connection = projection->connection;

	/*
	** get the amount of time between updates along this projection
	*/
	clock = Clockrate(projection);
#ifdef FLOATTIME
	error_correction = CORRECTION;
	latency = projection->latency;
	duration = projection->duration;
	if(duration < clock/2) duration = clock;
	simtime_minus_latency = simulation_time - latency;
	t1 = simtime_minus_latency - duration;
	duration -= error_correction;
#else
	/*
	** convert time quantities into projection interval units
	*/
	latency = round(projection->latency/clock);
	duration = round(projection->duration/clock);
	if(duration == 0) duration = 1;
	simtime_minus_latency = round(simulation_time/clock - latency);
	t1 = simtime_minus_latency - duration;
#endif

	/*
	** get the function which maps events onto segments
	*/
	if((connection_func = projection->connection_object->function) == NULL){
	    ErrorMessage("DelayConnection","No connection function.",projection);
	    return 0;
	}

	/*
	** tmax gives the maximum propagation time of a signal originating 
	** from a particular src cell. This is used to determine how 
	** far back in the event buffer to search.
	*/
	tmax = projection->tmax + duration;
	buffer_size = buffer->size;
	/*
	** point to the beginning of the event buffer for use in implementing
	** circularity in the buffer
	*/
	baseptr = buffer->event;
	/*
	** buffer_ptr points to the current buffer event
	*/
	buffer_ptr = baseptr + buffer->current;
	/*
	** startptr points to one location before the first valid
	** buffer event
	*/
	startptr = baseptr + buffer->start;
	/* 
	** implement latency 
	*/
	/* 
	** backs you up to the most recent event
	** that could have any effect on other elements
	*/
	/*
	** simtime_minus_latency is the current simulation time - the latency
	**
	** if the event time is greater than this then it 
	** has not yet reached the minimal delay requirements
	** for activation of the destination
	*/
#ifdef FLOATTIME
	while (buffer_ptr->time > simtime_minus_latency + error_correction) {
#else
	while (buffer_ptr->time > simtime_minus_latency) {
#endif
	    /* 
	    ** decrement the event pointer checking for circularity 
	    */
	    if (--buffer_ptr < baseptr){
		buffer_ptr = baseptr + buffer_size - 1;
	    }
	    /*
	    ** dont go beyond the beginning of the buffer
	    ** remember that startptr points one location before the
	    ** actual beginning of the buffer
	    */
	    if (buffer_ptr == startptr){
		/*
		** therefore you must increment by one to point to the
		** first location
		*/
		buffer_ptr = baseptr + (buffer->start +1) % buffer_size;
		break;
	    }
	}

	/*
	** distribute the events falling within the duration time
	** window
	*/
	while((leading_edge =simtime_minus_latency - buffer_ptr->time) <= tmax){
	    /* 
	    ** scan the connections 
	    */
	    connection_ptr = connection;
	    while(connection_ptr){
		/*
		** the use of break instead of continue
		** this assumes that the connections are ordered according
		** to increasing delay. Therefore if one connection
		** has not yet reached its target then neither will subsequent
		** connections 
		*/
#ifdef FLOATTIME
		if((offset_time = leading_edge - connection_ptr->delay ) < 
		-error_correction) {
#else
		if((offset_time = leading_edge - connection_ptr->delay ) < 0) {
#endif
		    connection_ptr = connection_ptr->next;
		    continue;
		    /*
		    break;
		    */
		}
		/*
		** map the rest
		*/
		if(offset_time < duration ){
		    /*
		    ** call the connection function
		    */
		    connection_func( 
			connection_ptr, 
			action,
			projection, 
			buffer_ptr,
			offset_time);
		}
		connection_ptr = connection_ptr->next;
	    }
	    /* 
	    ** decrement the buffer pointer checking for circularity 
	    */
	    if (--buffer_ptr < baseptr){
		buffer_ptr = baseptr + buffer_size - 1;
	    }
	    /*
	    ** dont go beyond the beginning of the buffer
	    */
	    if (buffer_ptr == startptr){
		break;
	    }
	}
	break;
    case CHECK:
	/*
	** get the buffer to be used from the msg
	*/
	buffer = NULL;
	MSGLOOP(projection,msg){
	    case 0:			/* buffer */
		buffer = (Buffer *)MSGPTR(msg,0);
		break;
	}
	if(!buffer){
	    ErrorMessage("Axon","No buffer attached to axon.",projection);
	} else {
	    if(!CheckClass(buffer,BUFFER_ELEMENT)){
		ErrorMessage("Axon","Invalid buffer sent to axon.",projection);
	    }
	}

	CheckProjections(projection);
	break;
    case CREATE:
	orig_elm = (Element*) action->passback;
	if (orig_elm != NULL)
	    CopyConnections(orig_elm, projection);
	return 1;
    case COPY:
	orig_elm = (Element*) action->data;
	if (orig_elm != NULL)
	    CopyConnections(orig_elm, projection);
	return 1;
    default:
	InvalidAction("DelayConnection",projection,action);
	break;
    }

    return 0;
}

