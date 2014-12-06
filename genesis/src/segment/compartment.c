static char rcsid[] = "$Id: compartment.c,v 1.3 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: compartment.c,v $
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
** Revision 1.7  1995/03/23 15:17:00  dhb
** Added number of values to SAVE2 file format
**
 * Revision 1.6  1994/06/02  20:39:20  dhb
 * Changed RESET code to initialize Vm from initVm rather than Em.
 *
 * Revision 1.5  1994/06/02  17:27:36  dhb
 * Added SET action to have initVm value follow Em unless initVm is set
 * directly.
 *
 * Revision 1.4  1994/05/27  21:29:56  dhb
 * Changes by Dave Beeman (modified by Dave Bilitch) to have the INJECT
 * message set the inject field in the same way the EREST message set Em.
 * The INJECT message values from multiple INJECT messages will sum.
 *
 * Revision 1.3  1992/10/29  18:24:23  dhb
 * The Id tag was missing the trailing $.
 *
 * Revision 1.2  1992/10/29  18:21:02  dhb
 * Replaced explicit msgin and msgout looping with MSGLOOP and MASOUTLOOP
 * macros.
 *
 * Note: CHECK action code looks wrong to me!!!
 *
 * Revision 1.1  1992/10/29  18:04:40  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "seg_ext.h"

int	DEBUG_Compartment = 0;
static double		savedata[2];

#define CHANNEL		0
#define RAXIAL		1
#define AXIAL		2
#define INJECT		3
#define MATRIXRESULT	4
#define RANDINJECT	5
#define EREST	6

#define CRANK		11
#define BEULER		10

#define	Field(F) (compartment->F)

/* 7/88 Matt Wilson */
/* 11/90 Erik De Schutter: CHECK now checks messages more extensively */
/* 2/91 Upinder S. Bhalla : CHECK bug fixed */
/* Added EREST message, E De Schutter 5/91 */

int Compartment(compartment,action)
register struct compartment_type *compartment;
Action		*action;
{
double		A;
double		inject;
double		B;
double		C;
double		dt;
double		Vm1,Ra1;
MsgIn		*msg;
MsgOut		*msgout;
int			n;
int			failed;
int			useinject;


    if(Debug(DEBUG_Compartment) > 1){
	ActionHeader("Compartment",compartment,action);
    }
    SELECT_ACTION(action){
    case INIT:
	compartment->previous_state = compartment->Vm;
	break;
    case PROCESS:
	dt = Clockrate(compartment);
	/*
	** calculate the integration parameters
	*/
	A = 0;
	B = 1.0/Field(Rm);
	C = 0;
	compartment->Im = 0;
	inject = 0;
	useinject = 0;
	/*
	** Read the msgs. These are factors used in the integration
	** across the membrane capacitance
	**
	** A = SUM(Ek*Gk) + I
	** B = SUM(Gk)
	** C = SUM(V/Ra) 
	**
	** dv/dt = B*Vm + A + C
	*/
	MSGLOOP(compartment,msg){
	    case CHANNEL:			/* channel */
		/*
		** 0 = Gk 	1 = Ek
		*/
		A += MSGVALUE(msg,1)*MSGVALUE(msg,0);
		B += MSGVALUE(msg,0);
		break;
	    case RAXIAL:			/* resistive axial */
		/*
		** 0 = Ra(n-1) 	1 = Vm(n-1)
		*/
		Vm1 = MSGVALUE(msg,1);
		Ra1 = MSGVALUE(msg,0);
		C += Vm1/Ra1;
		B += 1.0/Ra1;
		/* inward positive current convention */
		compartment->Im += (Vm1 - compartment->Vm)/Ra1;
		break;
	    case AXIAL:			/* non-resistive axial */
		/*
		** 0 = Vm(n+1)
		*/
		Vm1 = MSGVALUE(msg,0);
		C += Vm1/ Field(Ra);
		B += 1.0/Field(Ra);
		/* inward positive current convention */
		compartment->Im += (Vm1 - compartment->Vm)/Field(Ra);
		break;
	    case INJECT:			/* current injection */
		/*
		** 0 = inject
		*/
		compartment->Im += MSGVALUE(msg,0);
		inject += MSGVALUE(msg,0);
		useinject++;
		break;
	    case RANDINJECT:			/* random current injection */
		/*
		** 0 = probability (1/time)   1 = amplitude
		*/
		if(urandom() <= MSGVALUE(msg,0)*Clockrate(compartment)){
		    compartment->Im += MSGVALUE(msg,1);
		    A += MSGVALUE(msg,1);
		}
		break;
		case EREST:        /* change resting membrane potential */
		/* 
		** 0 = Em 
		*/
		compartment->Em = MSGVALUE(msg,0);
		break;
	}
    if (useinject)
	compartment->inject = inject;
    A += Field(Em)/Field(Rm) + Field(inject);
    /*
    ** Integrate the component membrane currents
    ** to get the net membrane potential
    */
    compartment->Vm =
	    IntegrateMethod(compartment->object->method,compartment,
	    compartment->Vm,(A+C)/Field(Cm),B/Field(Cm),dt,"Vm");
	break;
    case RESET:
	compartment->Vm = compartment->initVm;
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
    **  1 rule is  enforced at the tail of the compartment:
    **      for every incoming message, there should be an outgoing */
    MSGLOOP(compartment,msg){
        case RAXIAL:         /* incoming axial resistance */
        if(MSGVALUE(msg,0) <= 0.0)
            ErrorMessage("Compartment","Incoming Ra invalid.", compartment);
        break;

        case AXIAL:         /* check whether valid local Ra */
        if(Field(Ra) <= 0.0)
            ErrorMessage("Compartment", "Invalid Ra.", compartment);
        break;
    }

    MSGOUTLOOP(compartment, msgout) {
	MsgIn*	msgin;

        if(strcmp(msgout->dst->object->type,"compartment_type") != 0)
            continue;

	msgin = GetMsgInByMsgOut(msgout->dst, msgout);
        switch (msgin->type) {
            case AXIAL:         /* outgoing axial resistance */
                /* find corresponding incoming message */
                failed = 1;
		MSGLOOP(compartment, msg) {
		  case 1:
                    if (msg->src == msgout->dst) {
                        failed = 0;
                        goto success;
                    }
                }
		success:

                if (failed)
                    ErrorMessage("Compartment",
                        "Missing RAXIAL IN msg.", compartment);
                break;
        }
    }
	break;
    case SET:
	/*
	** initVm will follow setting of the value of Em as long as the
	** current values of initVm and Em are the same.  Once the user
	** sets initVm != Em, then setting Em does not cause initVm to
	** follow.  To get initVm to follow again, user cab set initVm
	** to Em.
	**
	** In order to avoid roundoff errors, the check for initVm and Em
	** being the same is that the difference between initVm and Em is
	** less than 0.1% of Em.
	*/
	if (strcmp(action->argv[0], "Em") == 0)
	  {
	    double	newEm;

	    newEm = Atof(action->argv[1]);
	    if (fabs(Field(Em) - Field(initVm)) <= fabs(Field(Em)/1000.0))
		Field(initVm) = newEm;
	    Field(Em) = newEm;

	    return 1;
	  }
	return 0;
    case SAVE2:
	savedata[0] = compartment->Vm;
	savedata[1] = compartment->previous_state;
	/*
	** action->data contains the file pointer
	*/
	n=2;
	fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),2,(FILE*)action->data);
	break;
    case RESTORE2:
	/*
	** action->data contains the file pointer
	*/
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 2) {
	    ErrorMessage("Compartment","Invalid savedata length",compartment);
	    return(n);
	}
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	compartment->Vm = savedata[0];
	compartment->previous_state = savedata[1];
	break;
    }
    return(0);
}

#undef Field

