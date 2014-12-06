static char rcsid[] = "$Id: channelc3.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: channelc3.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2000/10/09 23:00:57  mhucka
** Removed definition of M_E and #included math.h
**
** Revision 1.4  2000/09/21 19:35:42  mhucka
** Wrapped M_E definition inside #ifndef/endif.
**
** Revision 1.3  1995/03/23 15:17:00  dhb
** Added number of values to SAVE2 file format
**
** Revision 1.2  1993/08/12  17:18:07  dhb
** Mods by Erik De Schutter handling channel activation messages
**
** Revision 1.1  1992/12/11  19:04:02  dhb
** Initial revision
**
*/

#include <math.h>
#include "seg_ext.h"

int	DEBUG_ChannelC3 = 0;
static double		savedata[3];


#define Bfield(F)	(channel->F)

/*
** Generalized second order synaptic conductance transformation.
** Implements alpha function/dual exponent conductance
** waveforms for impulse spike input.
** Calculates channel current therefore needs membrane state.
*/
/* E De Schutter, Caltech 11/91. Slightly modified from Matt Wilsons channelc2*/
int ChannelC3(channel,action)
register struct channelC3_type *channel;
Action 	*action;
{
double 	x;
double 	dt;
int	has_state;
float	tpeak;
MsgIn 	*msg;
double	V = 0.0;
int	n;

    if(Debug(DEBUG_ChannelC3) > 1){
	ActionHeader("ChannelC3",channel,action);
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
	case 1:                 /* channel activation */
	    channel->activation += MSGVALUE(msg,0);
	    break;
	}
	/* 'endogenous' firing frequency: */
	if(urandom() <= channel->frequency*Clockrate(channel)){
		channel->activation++;
	}

	/*
	** calculate the activation level of the channel
	** dx/dt = (activation*tau1 - x)/tau1
	** x = activation*tau1*(1-exp(-dt/tau1)) + x*exp(-dt/tau1))
	** or
	** x = activation*xconst1 +x*xconst2
	*/
	x = channel->activation*Bfield(xconst1) +
		    Bfield(X)*Bfield(xconst2);
	/*
	** calculate the conductance of the channel
	** dy/dt = (x*tau2 - y)/tau2
	** y = x*tau2*(1-exp(-dt/tau2)) + y*exp(-dt/tau2))
	** or
	** y = x*yconst1 +y*yconst2
	*/
	Bfield(Y) = Bfield(X)*Bfield(yconst1) +
		    Bfield(Y)*Bfield(yconst2);
	/*
	** set the new values of the state variables
	*/
	Bfield(X) = x;
	Bfield(Gk) = Bfield(Y)*Bfield(norm);
	/*
	** calculate the channel current based on the channel conductance
	** the ion equilibrium potential and the membrane potential
	*/
	Bfield(Ik) = (Bfield(Ek) - V) * Bfield(Gk);
	break;
    case RESET:
	channel->activation = 0;
	Bfield(Gk) = 0;
	Bfield(X) = 0;
	Bfield(Y) = 0;
    case RECALC:
	dt = Clockrate(channel);
	Bfield(xconst1) = Bfield(tau1)*(1-exp(-dt/Bfield(tau1)));
	Bfield(xconst2) = exp(-dt/Bfield(tau1));
	Bfield(yconst1) = Bfield(tau2)*(1-exp(-dt/Bfield(tau2)));
	Bfield(yconst2) = exp(-dt/Bfield(tau2));
	if(Bfield(tau1) == Bfield(tau2)){
	    Bfield(norm) = Bfield(gmax)*M_E/Bfield(tau1);
	} else {
	    tpeak = Bfield(tau1)*Bfield(tau2) *
		log(Bfield(tau1)/Bfield(tau2))/
		(Bfield(tau1) - Bfield(tau2));
	    Bfield(norm) = Bfield(gmax)*(Bfield(tau1) - Bfield(tau2))/
		(Bfield(tau1)*Bfield(tau2)*
		(exp(-tpeak/Bfield(tau1)) - exp(-tpeak/Bfield(tau2))));
	}
	break;
    case CHECK:
	has_state = 0;
	MSGLOOP(channel,msg){
	    case 0:
		has_state = 1;
		break;
	}
	if(!has_state){
	    ErrorMessage("ChannelC3","No membrane potential.",channel);
	}
	if(Bfield(tau1) <= 0 || Bfield(tau2) <= 0){
	    ErrorMessage("ChannelC3","Invalid tau parameters.",channel);
	}
	break;
    case SAVE2:
	savedata[0] = channel->Gk;
	savedata[1] = channel->X;
	savedata[2] = channel->Y;
	n=3;
	fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),3,(FILE*)action->data);
	break;
    case RESTORE2:
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 3) {
	    ErrorMessage("ChannelC3","Invalid savedata length",channel);
	    return(n);
	}
	fread(savedata,sizeof(double),3,(FILE*)action->data);
	channel->Gk = savedata[0];
	channel->X = savedata[1];
	channel->Y = savedata[2];
	break;
    }
    return(0);
}

