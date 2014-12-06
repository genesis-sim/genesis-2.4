static char rcsid[] = "$Id: sigmoid.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: sigmoid.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 18:59:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/10/28 22:29:39  dhb
** Initial revision
**
*/

#include <math.h>
#include "buf_ext.h"

int	DEBUG_SigmoidEvent = 0;

/* 
** SigmoidEvent intended for unit types
*/
/* M.Wilson Caltech 6/88 */
void SigmoidEvent(sigmoid,action)
register struct sigmoid_type 	*sigmoid;
Action				*action;
{
MsgIn	*msg;

    if(Debug(DEBUG_SigmoidEvent) > 1){
	ActionHeader("SigmoidEvent",sigmoid,action);
    }
    SELECT_ACTION(action){
    case PROCESS:
	MSGLOOP(sigmoid,msg){
	case 0:				/* input */
	    sigmoid->input = MSGVALUE(msg,0);
	    break;
	case 1:				/* threshold */
	    sigmoid->thresh = MSGVALUE(msg,0);
	    break;
	case 2:				/* gain */
	    sigmoid->gain = MSGVALUE(msg,0);
	    break;
	case 3:				/* peak amplitude */
	    sigmoid->amplitude = MSGVALUE(msg,0);
	    break;
	}
	/*
	** the buffer state is the value of the sigmoid
	*/
	sigmoid->state = sigmoid->amplitude* 
	(tanh(sigmoid->gain*(sigmoid->input - sigmoid->thresh)) + 1)/2.0;
	PutEvent(sigmoid, sigmoid->state, NULL, WRAP);
	break;
    case RESET:
	ClearBuffer(sigmoid);
	break;
    default:
	InvalidAction("SigmoidEvent",sigmoid,action);
	break;
    }
}
