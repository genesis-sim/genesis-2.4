static char rcsid[] = "$Id: leakage.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: leakage.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:03  dhb
** Initial revision
**
*/

#include "seg_ext.h"

int	DEBUG_Leakage = 0;

/*
** This function implements a resistance in series with
** a battery connected to a zero reference voltage.
** It is intended to be used as a flexible leakage term in conjunction
** with the membrane model.
** channel->activation is used as the leakage conductance.
*/
/* 7/88 Matt Wilson */
void Leakage(channel,action)
register struct leakage_type *channel;
Action 	*action;
{
double	V = 0.0;
double	Gk;
double	Ek;
int	has_state;
MsgIn	*msg;

    if(Debug(DEBUG_Leakage) > 1){
	ActionHeader("Leakage",channel,action);
    }
    SELECT_ACTION(action){
    case PROCESS :
	Gk = channel->activation;
	Ek = channel->Ek;
	MSGLOOP(channel,msg){
	    case 0:		/* membrane potential */
		V = MSGVALUE(msg,0);
		break;
	    case 1:		/* Gk */
		Gk = MSGVALUE(msg,0);
		break;
	    case 2:		/* Ek */
		Ek = MSGVALUE(msg,0);
		break;
	}
	/*
	* leakage component
	*/
	channel->Ik = (Ek - V)*Gk;
	break;
    case RESET :
	channel->Ik = 0;
	break;
    case CHECK:
	has_state = 0;
	MSGLOOP(channel,msg){
	    case 0:		/* membrane potential */
		has_state = 1;
	}
	if(!has_state){
	    ErrorMessage("Leakage","No membrane potential.",channel);
	}
	break;
    }
}
