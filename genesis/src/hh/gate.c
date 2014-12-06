static char rcsid[] = "$Id: gate.c,v 1.3 2005/07/01 10:03:03 svitak Exp $";

/*
** $Log: gate.c,v $
** Revision 1.3  2005/07/01 10:03:03  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:38  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1995/03/23 01:36:28  dhb
** Bug in previous changes.
**
** Revision 1.2  1995/03/23  01:01:38  dhb
** Added number of values to SAVE2 file format
**
** Revision 1.1  1992/12/11  19:02:58  dhb
** Initial revision
**
*/

#include "hh_ext.h"

static double       savedata;


/* Modified by E. De Schutter, Caltech 1/91, added SAVE2/RESTORE2 */
/*
** calculates the voltage dependent gate state 
** using activation as the default voltage
*/
int VDepGate(gate,action)
register struct vdep_gate_type *gate;
Action		*action;
{
MsgIn	*msg;
double	GateState();
double	GateSteadyState();
int	n;

    if(debug > 1){
	ActionHeader("VDepGate",gate,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
	/*
	** check all of the messages to the gate
	*/
	MSGLOOP(gate,msg) {
	    case 0:				/* compartment */
		/*
		** 0 = membrane potential
		*/
		gate->activation = MSGVALUE(msg,0);
		break;
	}

	/* 
	** calculate the voltage dependent state variable m
	*/
	if(gate->instantaneous){
	    /*
	    ** calculate the steady state value of the state variable
	    */
	    gate->m = GateSteadyState(gate,
		gate->activation,
		gate->alpha_A,
		gate->alpha_B,
		gate->alpha_C,
		gate->alpha_D,
		gate->alpha_F,
		gate->beta_A,
		gate->beta_B,
		gate->beta_C,
		gate->beta_D,
		gate->beta_F);
	} else {
	    gate->m = GateState(gate, gate->m, gate->activation,
		gate->alpha_A,
		gate->alpha_B,
		gate->alpha_C,
		gate->alpha_D,
		gate->alpha_F,
		gate->beta_A,
		gate->beta_B,
		gate->beta_C,
		gate->beta_D,
		gate->beta_F,
		(int)(gate->object->method), "m");
	}
	break;

    case RESET:
	/*
	** check all of the messages to the gate
	*/
	MSGLOOP(gate,msg) {
	    case 2:				/* Erest */
		gate->activation = MSGVALUE(msg,0);
		break;
	}

	/*
	** calculate the steady state value of the state variable
	*/
	gate->m = GateSteadyState(gate,
	    gate->activation,
	    gate->alpha_A,
	    gate->alpha_B,
	    gate->alpha_C,
	    gate->alpha_D,
	    gate->alpha_F,
	    gate->beta_A,
	    gate->beta_B,
	    gate->beta_C,
	    gate->beta_D,
	    gate->beta_F);
	break;
    case SAVE2:
    savedata = gate->m;
    /* action->data contains the file pointer */
    n=1;
    fwrite(&n,sizeof(int),1,(FILE*)action->data);
    fwrite(&savedata,sizeof(double),1,(FILE*)action->data);
    break;

    case RESTORE2:
    /* action->data contains the file pointer */
    fread(&n,sizeof(int),1,(FILE*)action->data);
    if (n != 1) {
	ErrorMessage("VDepGate", "Invalid savedata length", gate);
	return n;
    }
    fread(&savedata,sizeof(double),1,(FILE*)action->data);
    gate->m = savedata;
    break;


    }

    return(0);
}

void VDepGate_CALC_MINF(gate,action)
register struct vdep_gate_type *gate;
Action		*action;
{
double	GateSteadyState();

    if(action->argc > 0){
	gate->activation = Atof(action->argv[0]);
    } else {
	Error();
	printf("CALC_MINF action requires voltage argument\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
    gate->m = GateSteadyState(gate,
	gate->activation,
	gate->alpha_A,
	gate->alpha_B,
	gate->alpha_C,
	gate->alpha_D,
	gate->alpha_F,
	gate->beta_A,
	gate->beta_B,
	gate->beta_C,
	gate->beta_D,
	gate->beta_F);
    action->passback = ftoa(gate->m);
}

void VDepGate_CALC_ALPHA(gate,action)
register struct vdep_gate_type *gate;
Action		*action;
{
double	RateState();
double	Vm = 0.0;
double	alpha;

    if(action->argc > 0){
	Vm = Atof(action->argv[0]);
    } else {
	Error();
	printf("CALC_ALPHA action requires voltage argument\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
    alpha = RateState(gate,
	Vm,
	gate->alpha_A,
	gate->alpha_B,
	gate->alpha_C,
	gate->alpha_D,
	gate->alpha_F);
    action->passback = ftoa(alpha);
}

void VDepGate_CALC_BETA(gate,action)
register struct vdep_gate_type *gate;
Action		*action;
{
double	RateState();
double	Vm = 0.0;
double	beta;

    if(action->argc > 0){
	Vm = Atof(action->argv[0]);
    } else {
	Error();
	printf("CALC_BETA action requires voltage argument\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
    beta = RateState(gate,
	Vm,
	gate->beta_A,
	gate->beta_B,
	gate->beta_C,
	gate->beta_D,
	gate->beta_F);
    action->passback = ftoa(beta);
}
