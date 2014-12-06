static char rcsid[] = "$Id: linear.c,v 1.2 2005/06/27 18:59:47 svitak Exp $";

/*
** $Log: linear.c,v $
** Revision 1.2  2005/06/27 18:59:47  svitak
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
 * Revision 1.1  1992/10/28  22:29:35  dhb
 * Initial revision
 *
*/

#include "buf_ext.h"

int	DEBUG_LinearEvent = 0;

/* 
** LinearEvent intended for unit types
*/
/* 6/88 Matt Wilson */
void LinearEvent(buffer,action)
register struct linear_type	*buffer;
Action 	*action;
{
MsgIn	*msg;
double 	fval;
double 	thresh;
int	stat;

    if(Debug(DEBUG_LinearEvent) > 1){
	ActionHeader("LinearEvent",buffer,action);
    }
    switch(action->type){
    case PROCESS:
	thresh = buffer->thresh;
	/*
	** read the msgs
	*/
	MSGLOOP(buffer, msg) {
	    /*
	    ** type 0 : x value used to calculate y=G*(x - thresh)
	    */
	    case 0:
		buffer->state = MsgValue(msg,double,0);
		break;
	    /*
	    ** type 1 : x-intercept threshold
	    */
	    case 1:
		thresh = MsgValue(msg,double,0);
		break;
	}
	/*
	** add an event to the potential buffer
	*/
	fval = buffer->gain* (buffer->state - thresh);

	PutEvent(buffer, fval, NULL, WRAP);
	break;
    case RESET:
	ClearBuffer(buffer);
	break;
    case CHECK:
	stat = 0;
	MSGLOOP(buffer, msg) {
	    case 0:
		stat = 1;
		break;
	}
	if(!stat){
	    ErrorMessage("LinearEvent","No input state",buffer);
	}
	break;
    }
}

