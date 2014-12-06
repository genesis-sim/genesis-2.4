/* reaction.c */
#include "chem_ext.h"

/*
** Avrama Blackwell, ERIM 
** Modified December, 1997, GMU
**
** An element to compute kf*substrates and kb*products 
** for in dC/dt = kf*subs*subs - kb*prods for
** nth order, multiple stage reactions.
** for each product in reaction, a message is sent (n times if n moles required)
** for each substrate in reaction, a message is sent to react object.
** react object sends messages back to each substrate and product:
** kbprod goes to A of substrate rxnpool and B of product rxnpool.
** kfsubs goes to B of substrate rxnpool and A of product rxnpool.
*/

/* Input Units:
   Substrates and products: either mM or mmoles.  Must be consistent with
   kf: either per (msec mM) or per (msec mmol)
   kb: per sec

   Output Units:
   Either mmol or mM, depending on input units.
   mmol - use message DIFFUSION of reaction pool, to convert to conc
   mM - use message RXN2 of reaction pool. */

reaction(react,action)
register struct react_type *react;
Action		*action;
{
MsgIn	*msg;
double	dt;

    if(debug > 1){
		ActionHeader("reaction",react,action);
    }

    SELECT_ACTION(action){
	case INIT:
		react->kbprod = 0;
		react->kfsubs = 0;
		break;
		
    	case PROCESS:
    		react->kbprod = react->kb;
    		react->kfsubs = react->kf;
		MSGLOOP(react,msg) {
            		case 0:	/*  substrate */
				/* 0 = Conc of Substrate */
				react->kfsubs *= MSGVALUE(msg,0);
				break;
			case 1: /* product */
				/* 0 = Conc of Product */
				react->kbprod *= MSGVALUE(msg,0);
				break;
  		}
		break;
    	case RESET:
		react->kbprod = 0;
		react->kfsubs = 0;
        	break;

    	case SET :
        	return(0); /* do the normal set */
		break;
	}
}
