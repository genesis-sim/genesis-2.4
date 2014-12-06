/* cicrflux.c */
#include "chem_ext.h"

/* Computes the CICR flux between two pools.  It sends messages back to
** both pools, thus, no need to set up flux in both directions.*/

/* Input Units:
   concentration of pool1 and pool2
   fraction of ip3 receptor-channels open
   maxflux is specified as distance/time, e.g. cm/sec.  
   When determining value to set flux->maxflux, multiply by area of membrane -> 
   then maxflux is in units of volume/time, e.g. m^3/sec.
   mutliplying by conc (mole / m^3) yields moles/sec.
   multiply by Avogadro and unit conversion yiels molecules per sec

   Output Units: molecules
   Message DIFFUSION to rxnpool
   deltaflux2 goes to A of pool2 rxnpool
   deltaflux1 goes to A of pool1 rxnpool

/* Avrama Blackwell, ERIM
   Modified at GMU 1999
   and again Mar 2002 */

#define AVOGADRO 6.023e23         /* units are molecules per mole */

int cicrflux(flux,action)
register struct flux_type *flux;
Action		*action;
{
double		fluxopen;	 /* composite cicrflux coefficient */
double		conc1, conc2;	/* calcium conc of pools connected by ip3r */
double		ip3r;		/* fraction open of IP3r channel */
double          openfraction;   /* ip3r^flux->power */
MsgIn		*msg;
double		savedata[2];
int		n1, n2, n3;	/* count messages during CHECK */
int             i;              /* loop variable */

    if(debug > 1){
	ActionHeader("cicrflux",flux,action);
    }

    SELECT_ACTION(action){
    case INIT:
	flux->deltaflux1 = 0;
	flux->deltaflux2 = 0;
	break;

    case PROCESS:
	/* Read the msgs.  */
	MSGLOOP(flux,msg){
	    case 0:	/* params of pool 1*/
	    	        /* 0= conc */
	    	 conc1 = MSGVALUE(msg,0);
	    	 break;
	    case 1:		/* params of pool 2*/
				/* 0= conc */
	    	 conc2 = MSGVALUE(msg,0);
	    	 break;
	    case 2:		/* fraction open of ip3r channel */
				/* 0 = x110 of ip3r */
	    	 openfraction = ip3r = MSGVALUE(msg,0);
	    	 break;
	}
		/* dconc/dt = -maxflux Xip3/vol * (C_i-C_j) 
		** compute flux = dconc/dt * Vi
		** flux->maxflux stores nchannels*flux per channel
		** flux->power stores exponent, fraction open = ip3r^power */

	  for (i=1; i<flux->power; i++)
	    openfraction *= ip3r;
	  fluxopen = flux->maxflux * openfraction;
	flux->deltaflux2 = fluxopen * (conc1-conc2)*(AVOGADRO*flux->units);
	flux->deltaflux1 = -flux->deltaflux2;
 	break;	

    case RESET:
	flux->deltaflux1 = 0;
	flux->deltaflux2 = 0;
	break;

    case CHECK:
	/* scaled max cond * n channels constant */
	if(flux->maxflux <= 0.0){
	    ErrorMessage("cicrflux", "Invalid D", flux);
	}
        n1=n2=n3=0;
        MSGLOOP(flux,msg){
             case 0:         /* one conc pool 1 msg required */
                n1 += 1;
                break;
             case 1:         /* one conc pool 2 msg required */
                n2 += 1;
                break;
             case 2:         /* one ip3r x fraction open required */
                n3 += 1;
                break;
         }
        if (n1 != 1 || n2 != 1 || n3 != 1)
            ErrorMessage("cicrflux", "incorrect number of msg.", flux);
	break;
    }
    return(0);
}
