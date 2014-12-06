/* conservepool.c */
#include "chem_ext.h"

/* Implementation of pool for conserved molecules */

/* computes concentration of molecule which is the total concentration
** less the concentration in all other states. */

/* Avrama Blackwell, ERIM 
   Modified Dec 1999 to compute quantities
   Modified March 2002, quantities are units of molecules
   Modified Aug 2013 to fix length units */

#define AVOGADRO 6.023e23         /* units are molecules per mole */
 
int conservepool(conspool,action)
register struct conserve_type *conspool;
Action		*action;
{
double	dt;
double conctot, moletot;
MsgIn 	*msg;
int	nvol, nc, nm;   /* number of volume, conc and moles messages */

    if(debug > 1){
	ActionHeader("conserverpool",conspool,action);
    }

    SELECT_ACTION(action){
    case INIT:
	conspool->previous_state = conspool->Conc;
	break;

    case PROCESS:
	dt = Clockrate(conspool);
	conctot = moletot = 0;

	/* Read the msgs to get the total conservation in other states */
	MSGLOOP(conspool,msg){
	    case 0:	/* conservation of mass */
	    	conctot += MSGVALUE(msg,0);
	    	break;
	    case 1:  /* moles of other forms */
	        moletot += MSGVALUE(msg,0);
	        break;
	    case 2: /* volume of compartment */
	      conspool->volume = MSGVALUE(msg,0);
	      conspool->Qtot=conspool->Ctot*(conspool->volume*conspool->VolUnitConv)*AVOGADRO*conspool->units;
	      break;
	}

	if (conspool->volume == 0)
	  conspool->Conc=conspool->quantity=0;
	else {
	  if (conspool->type == 0)   /* accumulates concentration */
	    {
	      conspool->Conc = conspool->Ctot - conctot;
	      conspool->quantity = conspool->Conc*(conspool->volume*conspool->VolUnitConv)*AVOGADRO*conspool->units;
	    }
	  else /* accumulates moles */
	    {
	      conspool->quantity = conspool->Qtot - moletot;
	      conspool->Conc = conspool->quantity/(AVOGADRO*conspool->units)/(conspool->volume*conspool->VolUnitConv);
	    }
	}
	if (conspool->Conc < conspool->Cmin)
	  {
	    conspool->Conc = conspool->Cmin;
	    conspool->quantity = conspool->Conc*(conspool->volume*conspool->VolUnitConv)*AVOGADRO*conspool->units;
	  }
	break;

	
    case RESET:
	MSGLOOP(conspool,msg){
	    case 2: /* volume of compartment */
	      conspool->volume = MSGVALUE(msg,0);
	      break;
	}
	conspool->VolUnitConv=conspool->Dunits*conspool->Dunits*conspool->Dunits;
	if (conspool->volume == 0) 
	  conspool->Conc=conspool->quantity=conspool->Qtot=0;
	else {
	  if (conspool->type == 0)
	    {
	      conspool->Conc = conspool->Cinit;
	      conspool->Qtot = conspool->Ctot*(conspool->volume*conspool->VolUnitConv)*AVOGADRO*conspool->units;
	      conspool->quantity = conspool->Conc*(conspool->volume*conspool->VolUnitConv)*AVOGADRO*conspool->units;
	    }
	  if (conspool->type == 1)
	    {
	      if (conspool->Ctot == 0)
		conspool->Ctot = conspool->Qtot/(AVOGADRO*conspool->units)/(conspool->volume*conspool->VolUnitConv);
	      if (conspool->Ctot > 0)
		conspool->Qtot=conspool->Ctot*(conspool->volume*conspool->VolUnitConv)*AVOGADRO*conspool->units;
	      conspool->quantity = conspool->Qinit;
	      conspool->Conc = conspool->quantity/(AVOGADRO*conspool->units)/(conspool->volume*conspool->VolUnitConv);
	    }
	}
	break;

    case CHECK:
      nvol=nc=nm=0;
      MSGLOOP(conspool,msg) {
      case 0:
	nc += 1;
	break;
      case 1:
	nm += 1;
	break;
      case 2:
	nvol += 1;
	break;
      }
      if (nvol > 1) 
	ErrorMessage("conservepool", "too many volume messages", conspool);
      else if (nvol == 1) {
	if (conspool->type == 0 || nc > 0)
	  ErrorMessage("conservepool", "must use quantity with variable volume", conspool);
      }
      else if (nvol == 0) {
	if (conspool->volume <= 0.0)
	  ErrorMessage("conservepool", "Invalid Vol.", conspool);
	if (nc > 1 && nm > 1)
	  ErrorMessage("conservepool", "Can't mix quant and conc messages", conspool);
      }
      break;

    }
    
    return(0);
    
}

