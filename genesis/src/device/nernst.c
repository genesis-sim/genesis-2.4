static char rcsid[] = "$Id: nernst.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: nernst.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:36  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:51  dhb
** Initial revision
**
*/

#include <math.h>
#include "dev_ext.h"
#define GAS_CONSTANT	1.9872			/* cal/mol deg */
#define FARADAY		23061			/* cal/volt mol */
#define ZERO_CELSIUS	273.15			/* deg */
#define R_OVER_F	8.6171458e-5		/* volt/deg */

/*
** calculates ionic equilibrium potential based on the concentration of
** a single type of ion
** E is calculated in Volts
** scale is in units of voltage conversion
** T has units of degrees Celsius
*/
/* 3/89 Matt Wilson */
void Nernst(nernst,action)
register struct nernst_type *nernst;
Action		*action;
{
MsgIn 	*msg;

    if(debug > 1){
	ActionHeader("Nernst",nernst,action);
    }
    SELECT_ACTION(action){
    case PROCESS:
	MSGLOOP(nernst,msg){
	    case 0:		/* intracellular ionic concentration */
		nernst->Cin = MSGVALUE(msg,0);
		break;
	    case 1:		/* extracellular ionic concentration */
		nernst->Cout = MSGVALUE(msg,0);
		break;
	    case 2:		/* temperature in degrees Celsius */
		nernst->T = MSGVALUE(msg,0);
		/* recalculate the constant */
		nernst->constant = nernst->scale*R_OVER_F*
		(nernst->T + ZERO_CELSIUS)/nernst->valency;
		break;
	}
	/*
	** E = constant*ln([out]/[in])
	** constant = RT/zF
	*/
	nernst->E = nernst->constant*log(nernst->Cout/nernst->Cin);
	break;
    case RESET:
	MSGLOOP(nernst,msg){
	    case 0:		/* intracellular ionic concentration */
		nernst->Cin = MSGVALUE(msg,0);
		break;
	    case 1:		/* extracellular ionic concentration */
		nernst->Cout = MSGVALUE(msg,0);
		break;
	    case 2:		/* temperature in degrees Celsius */
		nernst->T = MSGVALUE(msg,0);
		break;
	}
	nernst->constant = nernst->scale*R_OVER_F*
	(nernst->T + ZERO_CELSIUS)/nernst->valency;
	nernst->E = nernst->constant*log(nernst->Cout/nernst->Cin);
	break;
    case CHECK:
	MSGLOOP(nernst,msg){
	    case 0:		/* intracellular ionic concentration */
		nernst->Cin = MSGVALUE(msg,0);
		break;
	    case 1:		/* extracellular ionic concentration */
		nernst->Cout = MSGVALUE(msg,0);
		break;
	}
	if(nernst->valency == 0){
	    ErrorMessage("Nernst", "Invalid ionic valency z.", nernst);
	}
	if(nernst->Cin == 0){
	    ErrorMessage("Nernst", "Invalid intracellular ionic concentration.",
	    nernst);
	}
	if(nernst->Cout == 0){
	    ErrorMessage("Nernst", "Invalid extracellular ionic concentration.",
	    nernst);
	}
	if(nernst->scale <= 0){
	    ErrorMessage("Nernst", "Invalid scale parameter.", nernst);
	}
	break;
    }
}
