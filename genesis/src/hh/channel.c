static char rcsid[] = "$Id: channel.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: channel.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:37  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1998/07/15 06:32:54  dhb
** Upi update
**
** Revision 1.3  1995/03/23 01:01:38  dhb
** Added number of values to SAVE2 file format
**
 * Revision 1.2  1992/10/29  16:10:37  dhb
 * Changed reference to msg_in field to use nmsgin for existence of msg
 *
 * Revision 1.1  1992/10/29  16:05:31  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "hh_ext.h"

static double       savedata[2];
#define	Field(F) (channel->F)
#ifndef EXPONENTIAL
#define	EXPONENTIAL	1
#endif
#ifndef SIGMOID
#define	SIGMOID	2
#endif
#ifndef LINOID
#define	LINOID	3
#endif

/* M.Nelson Caltech 8/88 */
/* E. De Schutter Caltech 1/91, added SAVE2/RESTORE2 */
/* U.S. Bhalla Caltech 2/91, added CALC_ALPHA, CALC_BETA, CALC_MINF */
/* U.S. Bhalla, Caltech Jul 91 : cleaned out integ method */

int hh_channel(channel,action)
register struct hh_channel_type *channel;
Action		*action;
{
double 	v = 0;
MsgIn	*msg;
int	n;

    if(debug > 1){
	ActionHeader("hh_channel",channel,action);
    }

    SELECT_ACTION(action){
    case INIT:
	channel->activation = 0;
	break;
    case PROCESS:
	MSGLOOP(channel,msg) {
	case 0:		/* compartment */
	    /*
	    ** 0 = membrane potential
	    */
	    v = MSGVALUE(msg,0);
	    break;
	}

	/* 
	** calculate the active state variables x and y 
	*/
	if(Field(Xpower) != 0){
	    hh_update(channel,&Field(X),"X",
		(int)(channel->object->method),v,
		Field(X_alpha_FORM),
		Field(X_alpha_A),
		Field(X_alpha_B),
		Field(X_alpha_V0),
		Field(X_beta_FORM),
		Field(X_beta_A),
		Field(X_beta_B),
		Field(X_beta_V0));
	}
	if(Field(Ypower) != 0){
	    hh_update(channel,&Field(Y),"Y",
		(int)(channel->object->method),v,
		Field(Y_alpha_FORM),
		Field(Y_alpha_A),
		Field(Y_alpha_B),
		Field(Y_alpha_V0),
		Field(Y_beta_FORM),
		Field(Y_beta_A),
		Field(Y_beta_B),
		Field(Y_beta_V0));
	}

	/* 
	** calculate the conductance 
	*/
	if(Field(Xpower) != 0 && Field(Ypower) != 0){
	    Field(Gk) = Field(Gbar) * pow(Field(X),Field(Xpower)) * 
	    		pow(Field(Y),Field(Ypower));
	} else 
	if(Field(Ypower) == 0) {
	    Field(Gk) = Field(Gbar) * pow(Field(X),Field(Xpower));
	} else {
	    Field(Gk) = Field(Gbar);
	}

	/* 
	** calculate the transmembrane current 
	*/
	Field(Ik) = (Field(Ek) - v) * Field(Gk);

	break;
    case RESET:
	channel->activation = 0;
	v = 0;
	/* 
	** calculate the conductance at Erest 
	*/
	MSGLOOP(channel,msg) {
	    case 0:		/* Vm */
		v = MSGVALUE(msg,0);
		break;
	}

	/*
	** calculate the active state variables x and y at steady state
	*/
	if(Field(Xpower) != 0){

	    hh_evaluate(&Field(X),v,
		Field(X_alpha_FORM),
		Field(X_alpha_A),
		Field(X_alpha_B),
		Field(X_alpha_V0),
		Field(X_beta_FORM),
		Field(X_beta_A),
		Field(X_beta_B),
		Field(X_beta_V0));
	}

	if(Field(Ypower) != 0){
	    hh_evaluate(&Field(Y),v,
		Field(Y_alpha_FORM),
		Field(Y_alpha_A),
		Field(Y_alpha_B),
		Field(Y_alpha_V0),
		Field(Y_beta_FORM),
		Field(Y_beta_A),
		Field(Y_beta_B),
		Field(Y_beta_V0));
	}
	/*
	** calculate the conductance
	*/
	if(Field(Xpower) != 0 && Field(Ypower) != 0){
	    Field(Gk) = Field(Gbar)*pow(Field(X),Field(Xpower))* 
	    pow(Field(Y),Field(Ypower));
	} else 
	if(Field(Ypower) == 0) {
	    Field(Gk) = Field(Gbar)* pow(Field(X),Field(Xpower));
	} else{
	    Field(Gk) = Field(Gbar);
	}
	break;
    case CHECK:
	if(channel->nmsgin == 0){
	    ErrorMessage("hh_channel","Missing v msg.",channel);
	}
	break;
    case SAVE2:
        savedata[0] = channel->X;
        savedata[1] = channel->Y;
        /* action->data contains the file pointer */
        n=2;
        fwrite(&n,sizeof(int),1,(FILE*)action->data);
        fwrite(savedata,sizeof(double),2,(FILE*)action->data);
        break;
    case RESTORE2:
        /* action->data contains the file pointer */
        fread(&n,sizeof(int),1,(FILE*)action->data);
        if (n != 2) {
            ErrorMessage("hh_channel", "Invalid savedata length", channel);
            return n;
        }
        fread(savedata,sizeof(double),2,(FILE*)action->data);
        channel->X = savedata[0];
        channel->Y = savedata[1];
        break;
    }

    return(0);
}

void hh_channel_CALC_MINF(channel,action)
register struct hh_channel_type *channel;
Action		*action;
{
float	GateVal();
char	*gate = NULL;
float	alpha = 0.0,beta = 0.0,Vm = 0.0,m;

    if(action->argc == 2){
		gate = action->argv[0];
		Vm = Atof(action->argv[1]);
    } else {
	Error();
	printf("usage : CALC_MINF gate voltage\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	if (strcmp(gate,"X") == 0) {
    	alpha = GateVal(Vm,
		channel->X_alpha_FORM,
		channel->X_alpha_A,
		channel->X_alpha_B,
		channel->X_alpha_V0);
    	beta = GateVal(Vm,
		channel->X_beta_FORM,
		channel->X_beta_A,
		channel->X_beta_B,
		channel->X_beta_V0);
	} else if (strcmp(gate,"Y") == 0) {
    	alpha = GateVal(Vm,
		channel->Y_alpha_FORM,
		channel->Y_alpha_A,
		channel->Y_alpha_B,
		channel->Y_alpha_V0);
    	beta = GateVal(Vm,
		channel->Y_beta_FORM,
		channel->Y_beta_A,
		channel->Y_beta_B,
		channel->Y_beta_V0);
	}
   	m = alpha/(alpha + beta);
    action->passback = ftoa(m);
}

void hh_channel_CALC_ALPHA(channel,action)
register struct hh_channel_type *channel;
Action		*action;
{
float	GateVal();
float	Vm = 0.0;
float	alpha = 0.0;
char	*gate = NULL;

    if(action->argc == 2){
		gate = action->argv[0];
		Vm = Atof(action->argv[1]);
    } else {
	Error();
	printf("usage : CALC_ALPHA gate voltage\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	if (strcmp(gate,"X") == 0)
    	alpha = GateVal(Vm,
		channel->X_alpha_FORM,
		channel->X_alpha_A,
		channel->X_alpha_B,
		channel->X_alpha_V0);
	else if (strcmp(gate,"Y") == 0)
    	alpha = GateVal(Vm,
		channel->Y_alpha_FORM,
		channel->Y_alpha_A,
		channel->Y_alpha_B,
		channel->Y_alpha_V0);
    action->passback = ftoa(alpha);
}

void hh_channel_CALC_BETA(channel,action)
register struct hh_channel_type *channel;
Action		*action;
{
float	GateVal();
float	Vm = 0.0;
float	beta = 0.0;
char	*gate = NULL;

    if(action->argc == 2){
		gate = action->argv[0];
		Vm = Atof(action->argv[1]);
    } else {
	Error();
	printf("usage : CALC_BETA gate voltage\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	if (strcmp(gate,"X") == 0)
    	beta = GateVal(Vm,
		channel->X_beta_FORM,
		channel->X_beta_A,
		channel->X_beta_B,
		channel->X_beta_V0);
	else if (strcmp(gate,"Y") == 0)
    	beta = GateVal(Vm,
		channel->Y_beta_FORM,
		channel->Y_beta_A,
		channel->Y_beta_B,
		channel->Y_beta_V0);
    action->passback = ftoa(beta);
}

float GateVal(v,form,A,B,V0)
	float v;
	int	form;
	float V0,A,B;
{
	 float val = 0.0;
     switch(form){
     case EXPONENTIAL:
     val = A*exp((v-V0)/B);
     break;
     case SIGMOID:
     val = A/(exp((v-V0)/B)+1);
     break;
     case LINOID:
     if(v == V0)
         val = A*B;
     else
         val = A*(v-V0)/(exp((v-V0)/B)-1);
     break;
	}
	return(val);
}

#undef Field
#undef EXPONENTIAL
#undef SIGMOID
#undef LINOID
