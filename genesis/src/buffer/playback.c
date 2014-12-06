static char rcsid[] = "$Id: playback.c,v 1.2 2005/06/27 18:59:57 svitak Exp $";

/*
** $Log: playback.c,v $
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2000/06/12 04:28:52  mhucka
** Added NOTREACHED comments where appropriate.
**
** Revision 1.1  1992/10/28 22:29:37  dhb
** Initial revision
**
*/

#include "buf_ext.h"

int	DEBUG_PlaybackBuffer = 0;

/* 
** UpdateBufferEvent 
**
** update a pre-filled buffer 
*/

/* 6/88 Matt Wilson */
int PlaybackBuffer (buffer,action)
register struct playback_type *buffer;
Action		*action;
{
float event_time;

    if(Debug(DEBUG_PlaybackBuffer) > 1){
	ActionHeader("PlaybackBuffer",buffer,action);
    }
    switch(action->type){
    case PROCESS:
	/*
	** update the buffer pointer to bring it up to date with the
	** current simulation time
	*/
	if(buffer->current != buffer->end && buffer->current == buffer->start){
	    buffer->current = (buffer->current +1)%buffer->size;
	}
	/*
	** this can only occur if the buffer is empty
	*/
	if(buffer->current == buffer->start) return 0;
	event_time = CurrentEvent(buffer).time;
	/*
	** increment the pointers until reaching a time which
	** has not yet been reached
	*/
	while(SimulationTime() >= event_time - CORRECTION){ 
	    if((event_time >= SimulationTime() - Clockrate(buffer)/2) &&
	    (event_time <= SimulationTime() + Clockrate(buffer)/2)){
		buffer->state = CurrentEvent(buffer).magnitude;
	    } else {
		buffer->state = 0;
	    }
	    if(buffer->current != buffer->end){ 
		buffer->current = (buffer->current + 1) % buffer->size;
	    } else {
		break;
	    }
	    event_time = CurrentEvent(buffer).time;
	}
	break;
    case RESET:
	ResetBuffer(buffer);
	buffer->state = 0;
	break;
    case CREATE:
	buffer->event_size = sizeof(struct event_type);
	return(1);
	/* NOTREACHED */
	break;
    case COPY:
	BufferCopy(buffer,action);
	break;
    default:
	InvalidAction("PlaybackBuffer",buffer,action);
	break;
    }
    return(1);
}
