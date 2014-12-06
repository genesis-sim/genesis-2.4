static char rcsid[] = "$Id: randombuf.c,v 1.2 2005/06/27 18:59:57 svitak Exp $";

/*
** $Log: randombuf.c,v $
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/10/28 22:29:38  dhb
** Initial revision
**
*/

#include "buf_ext.h"

int	DEBUG_RandomEvent = 0;

#define TIME_RV		0x01
#define AMP_RV		0x02
#define AMP_RESET	0x04

/* 
** RandomEvent
** 
** place a random event into the buffer
*/

#define Field(F)	(buffer->F)

/* 9/88 Matt Wilson */
void RandomEvent(buffer,action)
register struct random_type 	*buffer;
Action 			*action;
{
MsgIn	*msg;
float	p;

    if(Debug(DEBUG_RandomEvent) > 1){
	ActionHeader("RandomEvent",buffer,action);
    }
    switch(action->type){
    case INIT:
	if(buffer->reset){
	    buffer->state = buffer->reset_value;
	}
	break;
    case RESET:
	ClearBuffer(buffer);
	buffer->state = buffer->reset_value;
	break;
    case PROCESS:
	MSGLOOP(buffer,msg){
	    case 0:	/* RATE */
		buffer->rate = MSGVALUE(msg,0);
		break;
	    case 1:	/* MIN/MAX*/
		buffer->min_amp = MSGVALUE(msg,0);
		buffer->max_amp = MSGVALUE(msg,1);
		break;
	}
	/*
	** add an event to the potential buffer
	*/
	p = buffer->rate*Clockrate(buffer);
	if( (p >= 1) || (p >= urandom())){
	    if(buffer->min_amp != buffer->max_amp){
		buffer->state = frandom(Field(min_amp),Field(max_amp));
	    } else {
		buffer->state = buffer->min_amp;
	    }
	    PutEvent(buffer, buffer->state, NULL, WRAP);
	}
	break;
    default:
	InvalidAction("RandomEvent",buffer,action);
	break;
    }
}
#undef Field
