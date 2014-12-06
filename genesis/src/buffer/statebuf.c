static char rcsid[] = "$Id: statebuf.c,v 1.2 2005/06/27 18:59:57 svitak Exp $";

/*
** $Log: statebuf.c,v $
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/06/12 04:29:18  mhucka
** Added NOTREACHED comments where appropriate.
**
** Revision 1.2  1993/01/21 20:31:35  dhb
** Changes to support reduced memory messages
**
 * Revision 1.1  1992/10/28  22:29:41  dhb
 * Initial revision
 *
*/

#include "buf_ext.h"

int	DEBUG_StateEvent = 0;

/* 
** StateEvent
** 
** place the state into the buffer
*/

/* 6/88 Matt Wilson */
int StateEvent(buffer,action)
register struct state_type 	*buffer;
Action			*action;
{

    if(Debug(DEBUG_StateEvent) > 1){
	ActionHeader("StateEvent",buffer,action);
    }
    SELECT_ACTION(action){
    case RESET:
	ClearBuffer(buffer);
	break;
    case PROCESS:
	/*
	** just use the buffer state as its output
	*/
	if(buffer->msgin){
	    buffer->state = MsgValue(buffer->msgin,double,0);
	}
	PutEvent(buffer, buffer->state, NULL, WRAP);
	break;
    case CHECK:
	if(!buffer->msgin){
	    ErrorMessage("StateEvent","Input is required.",buffer);
	}
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
	InvalidAction("StateEvent",buffer,action);
	break;
    }
    return 0;
}

