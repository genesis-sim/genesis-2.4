static char rcsid[] = "$Id: PID.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: PID.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:22  dhb
** Initial revision
**
*/

#include "user_ext.h"

/*
**===============================================================
** PID (Proportional, Integral, Derivative) controller
**
** Used for feedback control of dynamical systems
**
** INPUTS: cmd - the command (desired) value
**         sns - the sensed (measured) value
**
** OUTPUT: out = gain*(e + tau_d*de/dt + 1/tau_i * INTEGRAL e dt)
**         where e is the error, e = cmd-sns
**
** M. Nelson Caltech 1/89 
**===============================================================
*/
void PIDcontroller(pid,action)
register struct PID_type *pid;
Action		*action;
{
MsgIn	*msg;
double	cmd; 
double	sns; 

    if(debug > 1){
	ActionHeader("PIDcontroller",pid,action);
    }

    SELECT_ACTION(action){
    case INIT:
	pid->e_previous = pid->e;
	break;
    case PROCESS:
	cmd = pid->cmd;
	sns = pid->sns;
	MSGLOOP(pid,msg) {
	    case 0:				/* command */
		cmd = MSGVALUE(msg,0);
		break;
	    case 1:				/* sense */
		sns = MSGVALUE(msg,0);
		break;
	    case 2:				/* gain */
		pid->gain = MSGVALUE(msg,0);
		break;
	}

	pid->e = cmd - sns;
	pid->e_deriv = (pid->e - pid->e_previous)/Clockrate(pid);
	pid->e_integral += 0.5*(pid->e + pid->e_previous)*Clockrate(pid);

	pid->output = pid->gain*
	(pid->e + pid->tau_d * pid->e_deriv + 1/pid->tau_i * pid->e_integral);

	/* Unwind integrator if output is saturated */
	if(pid->output > pid->saturation){
	    pid->output = pid->saturation;
	    pid->e_integral -= 0.5*(pid->e + pid->e_previous)*Clockrate(pid);
	    }
	else if(pid->output < -pid->saturation) {
	    pid->output = -pid->saturation;
	    pid->e_integral -= 0.5*(pid->e + pid->e_previous)*Clockrate(pid);
	    }

	break;
    case RESET:
	pid->output = 0;
	pid->e_previous = 0;
	pid->e_integral = 0;
	break;
    }
}
