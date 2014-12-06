static char rcsid[] = "$Id: symcompart.c,v 1.2 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: symcompart.c,v $
** Revision 1.2  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.7  1998/06/30 23:30:50  dhb
** EDS 22d merge
**
 * EDS22c revison: EDS BBF-UIA 97/11/28-97/12/03
 * Corrected the code for connections to spheres.  This is now handled
 * completely inside readcell and does not require any specific messages
 * anymore. CONNECTSPHERE msg has been deleted.
 *
** Revision 1.6  1995/03/23 15:17:00  dhb
** Added number of values to SAVE2 file format
**
 * Revision 1.5  1994/06/02  20:39:20  dhb
 * Changed RESET code to initialize Vm from initVm rather than Em.
 *
 * Revision 1.4  1994/06/02  17:28:21  dhb
 * Added SET action to have initVm value follow Em unless initVm is set
 * directly.
 *
 * Revision 1.3  1994/05/27  21:35:54  dhb
 * Changes by Dave Beeman (modified by Dave Bilitch) to have INJECT messages
 * set the inject field.  This is much the same as the EREST message which
 * sets the Em field.  Injection currents from multiple inject messages are
 * summed.
 *
 * Revision 1.2  1992/10/29  18:37:28  dhb
 * Replaced explicit msgin and msgout looping with MSGLOOP and MSGOUTLOOP
 * macros.
 *
 * Note: The CHECK action code doesn't look right to me.
 *
 * Revision 1.1  1992/10/29  18:29:30  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "seg_ext.h"

int	DEBUG_SymCompartment = 0;
static double	savedata[2];

#define	Field(F) (compartment->F)

/* 7/88 Matt Wilson */
/* 11/90 and 12/97 Erik De Schutter: 
** CONNECTTAIL message: same as RAXIAL msg, but more functional name.
** CONNECTHEAD message: same as AXIAL msg, but more functional name.
** CONNECTCROSS message (type 5): in a correct symmetric implementation
**      of connections, compartments should coumpute the resistive flow
**      to ALL compartments which connect to the same node, this includes
**      therefore other children of the parent compartment (to which no
**      connections are made for the asymmetric case).  If you do this
**      naively with the AXIAL or CONNECTHEAD msg, the xcell drawing
**      routine will crash as it uses the AXIAL msg to establish the
**      topology of the dendritic tree.  Therefore a separate msg, 
**      CONNECTCROSS has been created.
** 
** 12/97 Erik De Schutter: a note on spherical compartments.
** If you use readcell spherical compartments will have zero length (a
** GENESIS convention) and the Ra field will NOT be zero.  This means
** that the (small) resistive component of the spherical compartment
** will be used in computing the axial current between soma and dendrite.
** This is very different from the asymmetric case (where soma Ra never
** counts) and may look weird for the case of a single dendrite.  It does
** make a lot of sense in the case of an apical and basal dendrite: if 
** synaptic current flows from one ot the other, one imagines that the
** soma has some effect...
** Note also that whether you use a CONNECTCROSS msg between cylindrical
** compartments onto the same sphere really should depend on their 
** topological relation.  For example, basal dendrites should probably
** be cross-connected to each other but not to an apical one.  readcell
** will issue all the possible CONNECTCROSS automatically, so you may
** want to delete a few.
**
** Added EREST message, E De Schutter 5/91
*/

int SymCompartment(compartment,action)
register struct symcompartment_type *compartment;
Action		*action;
{
double		A;
double		inject;
double		B;
double		dt;
double		Vm1,Ra1;
double 		Ra_sum,Ra_sum2;
MsgIn		*msg;
MsgOut		*msgout;
short		failed;
int		useinject;
int		n;

    if(Debug(DEBUG_SymCompartment) > 1){
	ActionHeader("SymCompartment",compartment,action);
    }
    SELECT_ACTION(action){
    case INIT:
	compartment->previous_state = compartment->Vm;
	break;

    case PROCESS:
	dt = Clockrate(compartment);
	/* calculate the integration parameters */
	A = 0;
	B = 1.0/Field(Rm);
	compartment->Im = 0;
	inject = 0;
	useinject = 0;
	/* Read the msgs. These are factors used in the integration
	** across the membrane capacitance
	**
	** A = SUM(Ek*Gk) + I
	** B = SUM(Gk)	 */
	MSGLOOP(compartment,msg){
	    case 0:		/* channel */
		/* 0 = Gk 	1 = Ek */
		A += MSGVALUE(msg,1)*MSGVALUE(msg,0);
		B += MSGVALUE(msg,0);
		break;

	    case 1:		/* resistive axial tail */
		/* 0 = Ra(n-1) 	1 = Vm(n-1) */
		Vm1 = MSGVALUE(msg,1);
		Ra1 = MSGVALUE(msg,0)*compartment->coeff;
		A += Vm1/Ra1;
		B += 1.0/Ra1;
		/* inward positive current convention */
		compartment->Im += (Vm1 - compartment->Vm)/Ra1;
		break;

	    case 2:		/* resistive axial head */
	    case 5:		/* resistive crossing axial head */
		/* 0 = Ra(n-1) 	1 = Vm(n-1) */
		Vm1 = MSGVALUE(msg,1);
		Ra1 = MSGVALUE(msg,0)*compartment->coeff2;
		A += Vm1/Ra1;
		B += 1.0/Ra1;
		/* inward positive current convention */
		compartment->Im += (Vm1 - compartment->Vm)/Ra1;
		break;

	    case 3:		/* current injection */
		/* 0 = inject */
		compartment->Im += MSGVALUE(msg,0);
		inject += MSGVALUE(msg,0);
		useinject++;
		break;

	    case 6:           /* change resting membrane potential */
		/* 0 = Em */
		compartment->Em = MSGVALUE(msg,0);
		break;

	}
	if (useinject)
	    compartment->inject = inject;
	A += Field(Em)/Field(Rm) + Field(inject);
	/* Integrate the component membrane currents
	** to get the net membrane potential */
	compartment->Vm =
	IntegrateMethod(compartment->object->method,compartment,
	compartment->Vm,A/Field(Cm),B/Field(Cm),dt,"Vm");
	break;

    case RESET:
	compartment->Vm = compartment->initVm;
	Ra_sum = Ra_sum2 = 0;
	MSGLOOP(compartment,msg){
	case 1:		/* incoming tail axial resistance */
	    Ra1 = MSGVALUE(msg,0);
	    if(Ra1 <= 0.0){
		ErrorMessage("SymCompartment","Incoming Ra invalid.",
		compartment);
	    }
	    Ra_sum += compartment->Ra/Ra1;
	    break;

	case 2:		/* incoming head axial resistance */
	case 5:		/* incoming crossing head axial head resistance */
	    Ra1 = MSGVALUE(msg,0);
	    if(Ra1 <= 0.0){
		ErrorMessage("SymCompartment","Incoming Ra invalid.",
		compartment);
	    }
	    Ra_sum2 += compartment->Ra/Ra1;
	    break;
	}
	/* divide the axial resistance into 2 */
	compartment->coeff = (1 + Ra_sum)/2.0;
	compartment->coeff2 = (1 + Ra_sum2)/2.0;
	break;

    case CHECK:
	/* membrane leakage resistance */
	if(Field(Rm) <= 0.0)
	    ErrorMessage("SymCompartment", "Invalid Rm.", compartment);

	/* membrane capacitance */
	if(Field(Cm) <= 0.0)
	    ErrorMessage("SymCompartment", "Invalid Cm.", compartment);

	/* axial resistance */
	if(Field(Ra) <= 0.0)
	    ErrorMessage("SymCompartment", "Invalid Ra.", compartment);
	
	/* check messages:
	**  all incoming messages are checked for legal Ra values
	**	2 rules are enforced at the tail of the compartment:
	**	- for every incoming message, there should be an outgoing
	**	- if more than one compartment is connected to the
	**	  tail, they should be crossconnected. */
	MSGLOOP(compartment,msg){
	    case 1:		/* incoming tail axial resistance */
	    case 2:		/* incoming head axial resistance */
	    case 5:		/* incoming cross axial resistance */
		if(MSGVALUE(msg,0) <= 0.0)
		    ErrorMessage("Compartment","Incoming Ra invalid.", compartment);
		if(Field(Ra) <= 0.0)
	   		ErrorMessage("Compartment", "Invalid Ra.", compartment);
		break;
	}


	MSGOUTLOOP(compartment, msgout) {
		MsgIn*	msgin;

		if (strcmp(msgout->dst->object->type,"symcompartment_type")!=0)
			continue;

		msgin = GetMsgInByMsgOut(msgout->dst, msgout);
		switch (msgin->type) {
	    	case 2:	/* outgoing head axial resistance */
			/* find corresponding incoming message */
			failed = 1;
			MSGLOOP(compartment, msg) {
			    case 1:
				if (msg->src == msgout->dst) {
					failed = 0;
					goto success2;
				}
				break;
			}
			success2:

			if (failed)
			ErrorMessage("SymCompartment", 
				"Missing CONNECTTAIL IN msg.", compartment);
			break;
			
	    	
		case 5:	/* outgoing cross axial resistance */
			/* find corresponding incoming message */
			failed = 1;
			MSGLOOP(compartment, msg) {
			    case 5:
				if (msg->src == msgout->dst) {
					failed = 0;
					goto success5;
				}
				break;
			}
			success5:

			if (failed)
			ErrorMessage("SymCompartment", 
				"Missing CONNECTCROSS IN msg.", compartment);
			break;
		}
	}
	break;	/* case CHECK */

    case SET:
	/*
	** initVm will follow setting of the value of Em as long as the
	** current values of initVm and Em are the same.  Once the user
	** sets initVm != Em, then setting Em does not cause initVm to
	** follow.  To get initVm to follow again, user can set initVm
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
	/* action->data contains the file pointer */
	n=2;
	fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),2,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 2) {
	    ErrorMessage("SymCompartment","Invalid savedata length",compartment);
	    return(n);
	}
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	compartment->Vm = savedata[0];
	compartment->previous_state = savedata[1];
	break;
	}		/* SELECT_ACTION(action) */
    return(0);
}

#undef Field

