static char rcsid[] = "$Id: channela.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: channela.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:59  dhb
** Initial revision
**
*/

#include "seg_ext.h"

int	DEBUG_ChannelA;

/* 7/88 Matt Wilson */
/*
** Non-filtered input to conductance transformation
*/
void ChannelA(channel,action)
struct channelA_type *channel;
Action		*action;
{
float	V = 0.0;
int	has_state;
MsgIn	*msg;

    if(Debug(DEBUG_ChannelA) > 1){
	ActionHeader("ChannelA",channel,action);
    }
    SELECT_ACTION(action){
    case INIT:
	channel->activation = 0;
	break;
    case PROCESS:
	MSGLOOP(channel,msg){
	    case 0:			/* membrane potential */
		V = MSGVALUE(msg,0);
		break;
	    case 1:			/* channel activation */
		channel->activation += MSGVALUE(msg,0);
		break;
	}
	/*
	** set the conductance equal to the activation level
	*/
	channel->Gk = channel->activation;
	/*
	** calculate the channel current based on the channel conductance
	** the ion equilibrium potential and the current membrane potential
	*/
	channel->Ik = (channel->Ek - V) * channel->Gk;
	break;
    case RESET:
	channel->activation = 0;
	channel->Gk = 0;
	break;
    case CHECK:
	/*
	** read the msg
	*/
	has_state = 0;
	MSGLOOP(channel,msg){
	    case 0:	/* membrane potential */
		has_state = 1;
		break;
	}
	if(!has_state == 0){
	    ErrorMessage("ChannelA","No membrane potential.",channel);
	} 
	break;
    }
}

