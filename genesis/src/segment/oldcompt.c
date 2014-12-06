static char rcsid[] = "$Id: oldcompt.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: oldcompt.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1995/03/23 15:17:00  dhb
** Added number of values to SAVE2 file format
**
** Revision 1.1  1992/12/11  19:04:06  dhb
** Initial revision
**
*/

#include "seg_ext.h"

int	DEBUG_Compartment = 0;
static double		savedata[2];


#define	Field(F) (compartment->F)

/* 7/88 Matt Wilson */
/* 11/90 Erik De Schutter: CHECK now checks messages more extensively */
/* 2/91 Upinder S. Bhalla : CHECK bug fixed */

int Compartment(compartment,action)
register struct compartment_type *compartment;
Action		*action;
{
double		A;
double		B;
double		dt;
double		Vm1,Ra1;
MsgIn		*msg;
MsgOut      *msgout;
short       failed;
int		i;
int		n;

    if(Debug(DEBUG_Compartment) > 1){
	ActionHeader("Compartment",compartment,action);
    }
    SELECT_ACTION(action){
    case INIT:
	compartment->previous_state = compartment->Vm;
	break;

    case PROCESS:
	dt = Clockrate(compartment);
	/* calculate the integration parameters */
	A = Field(Em)/Field(Rm) + Field(inject);
	B = 1.0/Field(Rm);
	compartment->Im = 0;
	/* Read the msgs. These are factors used in the integration
	** across the membrane capacitance
	**
	** A = SUM(Ek*Gk) + I
	** B = SUM(Gk) */
	MSGLOOP(compartment,msg){
	    case 0:			/* channel */
		/* 0 = Gk 	1 = Ek */
		A += MSGVALUE(msg,1)*MSGVALUE(msg,0);
		B += MSGVALUE(msg,0);
		break;

	    case 1:			/* resistive axial */
		/* 0 = Ra(n-1) 	1 = Vm(n-1) */
		Vm1 = MSGVALUE(msg,1);
		Ra1 = MSGVALUE(msg,0);
		A += Vm1/Ra1;
		B += 1.0/Ra1;
		/* inward positive current convention */
		compartment->Im += (Vm1 - compartment->Vm)/Ra1;
		break;

	    case 2:			/* non-resistive axial */
		/* 0 = Vm(n+1) */
		Vm1 = MSGVALUE(msg,0);
		A += Vm1/ Field(Ra);
		B += 1.0/Field(Ra);
		/* inward positive current convention */
		compartment->Im += (Vm1 - compartment->Vm)/Field(Ra);
		break;

	    case 3:			/* current injection */
		/* 0 = inject */
		compartment->Im += MSGVALUE(msg,0);
		A += MSGVALUE(msg,0);
		break;
	}
	/* Integrate the component membrane currents
	** to get the net membrane potential */
	compartment->Vm =
	IntegrateMethod(Field(method),compartment,
	compartment->Vm,A/Field(Cm),B/Field(Cm),dt,"Vm");
	break;

    case RESET:
	compartment->Vm = compartment->Em;
	break;

    case CHECK:
	/* membrane leakage resistance */
	if(Field(Rm) <= 0.0)
	    ErrorMessage("Compartment", "Invalid Rm.", compartment);

	/* membrane capacitance */
	if(Field(Cm) <= 0.0)
	    ErrorMessage("Compartment", "Invalid Cm.", compartment);

	/* check messages:
	**  all incoming messages are checked for legal Ra values
	**	1 rule is  enforced at the tail of the compartment:
	**		for every incoming message, there should be an outgoing */
	MSGLOOP(compartment,msg){
	    case 1:			/* incoming axial resistance */
		if(MSGVALUE(msg,0) <= 0.0)
		    ErrorMessage("Compartment","Incoming Ra invalid.", compartment);
		break;

		case 2:			/* check whether valid local Ra */
		if(Field(Ra) <= 0.0)
	   		ErrorMessage("Compartment", "Invalid Ra.", compartment);
		break;
	}

	for (msgout = compartment->msg_out; msgout; msgout = msgout->next) {
		if(strcmp(msgout->dst->object->type,"compartment_type") != 0)
			continue;
		switch (msgout->msg_in->type) {
	    	case 2:			/* outgoing axial resistance */
				/* find corresponding incoming message */
				failed = 1;
				for (msg=compartment->msg_in; msg; msg=msg->next) {	
					if ((msg->type == 1) && (msg->src == msgout->dst)) {
						failed = 0;
						break;
					}
				}
				if (failed)
	    			ErrorMessage("Compartment", 
						"Missing RAXIAL IN msg.", compartment);
				break;
		}
	}
	break;

    case SAVE2:
	savedata[0] = compartment->Vm;
	savedata[1] = compartment->previous_state;
	/* action->data contains the file pointer */
	n=2;
	fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),2,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 2) {
	    ErrorMessage("Compartment","Invalid savedata length",compartment);
	    return(n);
	}
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	compartment->Vm = savedata[0];
	compartment->previous_state = savedata[1];
	break;

    case SETUP:
#ifdef LATER
	/* compute how many state variables are coupled */
	n = 0;
	MSGLOOP(compartment,msg){
	    case 1:			/* axial coupling */
	    case 2:
		n++;
		break;
	}
	matrixmsg = SetupMatrixMsg(compartment,matrixsolver,count);
	matrixmsg->svid = &Vm;
	matrixmsg->variable_coeff = 1;
	MSGLOOP(compartment,msg){
	    case 1:			/* axial coupling */
		AddSVToMsg(matrixmsg,MSGPTR(msg,1));
	    case 2:
		AddSVToMsg(matrixmsg,MSGPTR(msg,0));
		break;
	}
#endif
	break;

    case RESULTS:
	break;
    }
    return(0);
}

#undef Field

