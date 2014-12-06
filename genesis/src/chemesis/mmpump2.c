/* mmpump2.c*/
#include "chem_ext.h"

/*
** Avrama Blackwell, ERIM 
** Modified, GMU, 1999 
** and again on Mar 2002
**
** A Michaelis-Menton type of formulation of pump.  
** Either SERCA ATPase pump or Na-Ca ATPase cytosolic pump.
**
**	dC/dt = max_rate * Ca^n / (Ca^n + half_conc^n)
**
** where Ca is concentration in cytosol and C is
**    conc in cytosol or ER or Extracellular fluid
** 
*/

/* Inputs units:
   Concentration of substance
   half_conc: units of concentration
   max_rate: units of mmole per mmsec

   Output Units: mmole, must convert to concentration in rxnpool */

#define AVOGADRO 6.023e23         /* units are molecules per mole */

mmpump2(pump,action)
register struct pump_type *pump;
Action		*action;
{
MsgIn	*msg;
double	dt;
double	conc, conc_pow;
int     i;

    if(debug > 1){
		ActionHeader("MMpump2",pump,action);
    }

    SELECT_ACTION(action){
	case INIT:
		pump->moles_in = 0;
		pump->moles_out = 0;
		break;
		
    	case PROCESS:
	   MSGLOOP(pump,msg) {
       		case 0:	/* cytosolic Ca concentration */
			/* 0 = Conc */
		  conc = conc_pow = MSGVALUE(msg,0);
		  for (i=1; i<pump->power; i++)
		    conc_pow *= conc;
		  pump->moles_in = pump->max_rate*AVOGADRO*pump->units*conc_pow/(conc_pow+pump->half_conc_pow);
	          pump->moles_out = -pump->moles_in;
	          break;
  		}
	   break;

    	case RESET:
	  pump->half_conc_pow = pump->half_conc;
	  for (i=1; i<pump->power; i++)
	    pump->half_conc_pow *= pump->half_conc;

	   MSGLOOP(pump,msg) {
       		case 0:	/* cytosolic Ca concentration */
			/* 0 = Conc */
		  conc = conc_pow = MSGVALUE(msg,0);
		  for (i=1; i<pump->power; i++)
		    conc_pow *= conc;
		  pump->moles_in = pump->max_rate*AVOGADRO*pump->units*conc_pow/(conc_pow+pump->half_conc_pow);
	          pump->moles_out = -pump->moles_in;
	          break;
	   }
	   break;

    	case SET :
        	return(0); /* do the normal set */
		break;
	}
}
