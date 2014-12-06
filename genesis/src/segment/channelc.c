static char rcsid[] = "$Id: channelc.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: channelc.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2000/10/09 23:00:57  mhucka
** Removed definition of M_E and #included math.h
**
** Revision 1.3  2000/09/21 19:35:42  mhucka
** Wrapped M_E definition inside #ifndef/endif.
**
** Revision 1.2  1995/03/23 15:17:00  dhb
** Added number of values to SAVE2 file format
**
** Revision 1.1  1992/12/11  19:04:00  dhb
** Initial revision
**
*/

#include <math.h>
#include "seg_ext.h"

int	DEBUG_ChannelC = 0;
static double		savedata[10];

#define	Field(F) (channel->F)

/*
** Generalized second order synaptic conductance transformation.
** Implements alpha function/dual exponent conductance
** waveforms for impulse spike input.
** Does not calculate channel current.
*/
/* 8/88 Matt Wilson */
int ChannelC(channel,action)
register struct channelC_type *channel;
Action		*action;
{
double 	x;
double 	dt;
float	tpeak;
MsgIn 	*msg;
int	n;

    if(Debug(DEBUG_ChannelC) > 1){
	ActionHeader("ChannelC",channel,action);
    }
    SELECT_ACTION(action){
    case INIT:
	channel->activation = 0;
	break;
    case PROCESS:
	MSGLOOP(channel,msg){
	case 0:			/* channel activation */
	    channel->activation += MSGVALUE(msg,0);
	    break;
	case 1:			/* random channel activation */
	    /*
	    ** 0 = probability (1/time)   1 = amplitude
	    */
	    if(MSGVALUE(msg,0)*Clockrate(channel) >= urandom()){
		channel->activation += MSGVALUE(msg,1);
	    }
	    break;
	}
	/*
	** calculate the activation level of the channel
	** da/dt = (activation*tau1 - a)/tau1
	** x = activation*tau1*(1-exp(-dt/tau1)) + x*exp(-dt/tau1))
	** or
	** x = activation*xconst1 +x*xconst2
	*/
	x = channel->activation*Field(xconst1) +
		    Field(X)*Field(xconst2);
	/*
	** calculate the conductance of the channel
	** dy/dt = (x*tau2 - y)/tau2
	** y = x*tau2*(1-exp(-dt/tau2)) + y*exp(-dt/tau2))
	** or
	** y = x*yconst1 +y*yconst2
	*/
	Field(Y) = Field(X)*Field(yconst1) +
		    Field(Y)*Field(yconst2);
	/*
	** set the new values of the state variables
	*/
	Field(X) = x;
	Field(Gk) = Field(Y)*Field(norm);
	/*
	** calculate the channel current based on the channel conductance
	** the ion equilibrium potential and the membrane potential
	*/
	break;
    case RESET:
	channel->activation = 0;
	Field(Gk) = 0;
	Field(X) = 0;
	Field(Y) = 0;
    case RECALC:
	dt = Clockrate(channel);
	Field(xconst1) = Field(tau1)*(1-exp(-dt/Field(tau1)));
	Field(xconst2) = exp(-dt/Field(tau1));
	Field(yconst1) = Field(tau2)*(1-exp(-dt/Field(tau2)));
	Field(yconst2) = exp(-dt/Field(tau2));
	if(Field(tau1) == Field(tau2)){
	    Field(norm) = Field(gmax)*M_E/Field(tau1);
	} else {
	    tpeak = Field(tau1)*Field(tau2) *
		log(Field(tau1)/Field(tau2))/
		(Field(tau1) - Field(tau2));
	    Field(norm) = Field(gmax)*(Field(tau1) - Field(tau2))/
		(Field(tau1)*Field(tau2)*
		(exp(-tpeak/Field(tau1)) - exp(-tpeak/Field(tau2))));
	}
	break;
    case CHECK:
	if(Field(tau1) <= 0 || Field(tau2) <= 0){
	    ErrorMessage("ChannelC", "Invalid tau parameters.", channel);
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
	    ErrorMessage("ChannelC","Invalid savedata length",channel);
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

#undef Field
