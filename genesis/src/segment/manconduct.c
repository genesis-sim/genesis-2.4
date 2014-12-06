static char rcsid[] = "$Id: manconduct.c,v 1.3 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: manconduct.c,v $
** Revision 1.3  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:04  dhb
** Initial revision
**
*/

#include <math.h>
#include "seg_ext.h"

/*
** Implementation of MANUEL postsynaptic conductance transformation
** MANUEL simulation software by D.H Perkel
**
*/
/* 1/89 Matt Wilson */
/* 12/90 modified Ik, Erik De Schutter */
void ManuelConduct(channel,action)
register struct manuelconduct_type *channel;
Action		*action;
{
/* Variables h and z are relative to the total number of receptors
**	and gmax should be equal to g-bar * total number of receptors */
double 	h;	/* number of open (active) channel-receptor complexes */
double 	z;	/* total amount of transmitter (free+bound&closed) */
double 	c;  /* number of closed (inactive) channel-receptor complexes */
double 	W;  /* computation variable */
double 	dt;
double 	V = 0.0;
MsgIn 	*msg;

    if(debug > 1){
	ActionHeader("ManuelConduct",channel,action);
    }
    SELECT_ACTION(action){
    case INIT:
	channel->activation = 0;
	break;

    case PROCESS:
	dt = Clockrate(channel);
	MSGLOOP(channel,msg){
	case 0:			/* membrane potential */
	    V = MSGVALUE(msg,0);
	    break;
	case 1:		/* channel activation */
	    channel->activation += MSGVALUE(msg,0);
	    break;
	case 2:			/* random channel activation */
	    /*
	    ** 0 = probability (1/time)   1 = amplitude
	    */
	    if(MSGVALUE(msg,0)*Clockrate(channel) > urandom()){
		channel->activation += MSGVALUE(msg,1);
	    }
	    break;
	}
	h = channel->H;
	z = channel->Z;

	W = 1 - h + z + channel->K_r;
	c = (2*z*(1-h))/(W + sqrt(W*W - 4*z*(1-h)));
	/*
	** dz/dt = activation - (z - c)/tau_d - c/tau_g
	** dz/dt = (activation + c/tau_d - c/tau_g) - z*(1/tau_d)
	*/
	z = IntegrateMethod(0,channel,z,
		channel->activation + c/channel->tau_d - c/channel->tau_g,
		1/channel->tau_d,dt,"z");

	/*
	** dh/dt = c/tau_g - h/tau_h
	*/
	h = IntegrateMethod(0,channel,h,
		c/channel->tau_g,
		1/channel->tau_h,dt,"h");

    if(debug > 2){
	fprintf(stderr,"%f  %f  %f  %f  %f  %f\n",channel->Z,channel->H,
			W,c,z,h);
    }
	/*
	** set the new values of the state variables
	*/
	channel->H = h;
	channel->Z = z;
	channel->Gk = channel->gmax*h;
	channel->Ik = (channel->Ek - V)*channel->Gk;
	break;

    case RESET:
	channel->activation = 0;
	channel->H = 0;
	channel->Z = 0;
	channel->Gk = 0;
	channel->Ik = 0;
	break;

    case CHECK:
	if(channel->tau_g <= 0 || channel->tau_h <= 0 || channel->tau_d <= 0){
	    ErrorMessage("ManuelConduct", "Invalid tau parameters.", channel);
	}
	break;
    }
}
