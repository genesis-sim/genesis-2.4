static char rcsid[] = "$Id: channelb.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: channelb.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:00  dhb
** Initial revision
**
*/

#include "seg_ext.h"

int	DEBUG_ChannelB = 0;

#define	Field(F) 	(channel->F)

/* 8/88 Matt Wilson */
/*
** another second order synaptic transformation function.
** It is basically the same as ConductanceA
*/
void ChannelB(channel,action)
struct channelB_type 	*channel;
Action			*action;
{
double 	gk;
double 	ak;
double 	dg;
double 	da;
double 	dt;
int	has_state;
double	V = 0.0;
MsgIn	*msg;


    if(Debug(DEBUG_ChannelB) > 1){
	ActionHeader("ChannelB",channel,action);
    }
    SELECT_ACTION(action){
    case INIT:
	channel->activation = 0;
	break;
    case PROCESS:
	has_state = 0;
	MSGLOOP(channel,msg){
	    case 0:			/* membrane potential */
		has_state = 1;
		V = MSGVALUE(msg,0);
		break;
	    case 1:			/* channel activation */
		channel->activation += MsgValue(msg,double,0);
		break;
	}
	dt = Clockrate(channel);
	/*
	** calculate the activation level of the channel
	*/
	ak = Field(Ak);
	da = -ak/Field(tau_ak);
	ak = Euler(ak,da,dt) + channel->activation;
	/*
	** calculate the conductance based on activation input
	*/
	gk = Field(Yk);
	dg = -gk/Field(tau_gk) + ak*Field(gain_ak);
	gk = Euler(gk,dg,dt);
	/*
	** set the current state of the node to the newly calculated
	** states
	*/
	Field(Yk) = gk;
	Field(Gk) = gk*Field(gain_gk);
	Field(Ak) = ak;
	if(has_state){
	    /*
	    ** calculate the channel current based on the channel conductance
	    ** the ion equilibrium potential and the current membrane potential
	    */
	    Field(Ik) = (Field(Ek) - V) * Field(Gk);
	}
	break;
    case RESET:
	channel->activation = 0;
	Field(Gk) = 0;
	Field(Ak) = 0;
	Field(Yk) = 0;
	break;
    case CHECK:
	if(channel->tau_ak == 0 || channel->tau_gk == 0){
	    ErrorMessage("ChannelB","Invalid tau parameters.",channel);
	}
	break;
    }
}

#undef Field

