static char rcsid[] = "$Id: graded.c,v 1.2 2005/06/27 18:59:47 svitak Exp $";

/*
** $Log: graded.c,v $
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
 * Revision 1.1  1992/10/28  22:29:33  dhb
 * Initial revision
 *
*/

#include "buf_ext.h"

int	DEBUG_GradedEvent = 0;

#define Field(F)	(buffer->F)

/* M.Wilson Caltech 6/88 */
void GradedEvent (buffer,action)
register struct graded_type *buffer;
Action 	*action;
{
MsgIn	*msg;
double	scale;
double 	baseline;	
double 	grade;	
double 	rectify;	
int	status;
int	has_state;

    if(Debug(DEBUG_GradedEvent) > 1){
	ActionHeader("GradedEvent",buffer,action);
    }
    switch(action->type){
    case PROCESS:
	/*
	** set default values
	*/
	baseline = Field(baseline);
	rectify = Field(rectify);
	scale = Field(scale);
	if(scale == 0) scale = 1;
	/*
	** read messages
	*/
	MSGLOOP(buffer, msg) {
	    /*
	    ** type 0 : buffer state used to compute graded output
	    */
	    case 0:
		buffer->state = MsgValue(msg,double,0);
		break;
	    /*
	    ** type 1 : rectification baseline
	    */
	    case 1:
		baseline = MsgValue(msg,double,0);
		break;
	}
	/*
	** calculate graded output
	*/
	grade = (buffer->state - baseline)*scale;
	if(!rectify || grade >= 0){
	    /*
	    ** add an event to the potential buffer
	    */
	    PutEvent(buffer,grade, NULL, WRAP);
	}
	break;
    case RESET:
	ClearBuffer(buffer);
	break;
    case CHECK:
	status = 1;
	has_state = 0;
	MSGLOOP(buffer, msg) {
	    case 0:
		has_state = 1;
		break;
	}
	if(!has_state){
	    ErrorMessage("GradedEvent","No input.",buffer);
	}
	break;
    default:
	InvalidAction("GradedEvent",buffer,action);
	break;
    }
}
#undef Field

