static char rcsid[] = "$Id: thresh.c,v 1.4 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: thresh.c,v $
** Revision 1.4  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:37  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:52  dhb
** Initial revision
**
*/

#include <math.h>
#include "dev_ext.h"

void ExpThresh(thresh,action)
struct expthresh_type *thresh;
Action	*action;
{
MsgIn	*msg;
Buffer 		*buffer = NULL;

    SELECT_ACTION(action){
    case PROCESS:
	MSGLOOP(thresh,msg) {
	case 0:				/* buffer */
	    buffer = (Buffer *)MSGPTR(msg,0);
	    break;
	}

	if(ValidEvent(buffer)){
	    thresh->state =
	    thresh->theta_s + (thresh->theta_0 -thresh->theta_s)*
	    exp((CurrentEventTime(buffer) - SimulationTime())/thresh->tau_theta);
	}
	break;
    case RESET:
	thresh->state = thresh->theta_s;
	break;
    case CHECK:
	buffer = NULL;
	MSGLOOP(thresh,msg) {
	case 0:				/* buffer */
	    buffer = (Buffer *)MSGPTR(msg,0);
	    break;
	}
	if(!buffer){
	    ErrorMessage("ExpThresh","No buffer attached.",thresh);
	} else {
	    if(!CheckClass(buffer,BUFFER_ELEMENT)){
		ErrorMessage("ExpThresh","Invalid buffer sent to thresh.",thresh);
	    }
	}
	break;
    }
}
 
