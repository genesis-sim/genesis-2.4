/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
static char rcsid[] = "$Id: concchan.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";
 
/*
 *$Log: concchan.c,v $
 *Revision 1.2  2005/07/01 10:03:04  svitak
 *Misc fixes to address compiler warnings, esp. providing explicit types
 *for all functions and cleaning up unused variables.
 *
 *Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 *Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 *Revision 1.2  2000/06/12 05:07:03  mhucka
 *Removed nested comments; added NOTREACHED comments where appropriate.
 *
 *Revision 1.1  1997/07/24 17:49:40  dhb
 *Initial revision
 *
 * Revision 1.1  1994/08/05  19:22:08  bhalla
 * Initial revision
 *
*/

#include "kin_ext.h"

/*
** Concchan element. Calculates conc gradient and scales by the
** permeability to give flux in terms of A and B. Optional message
** gives the # of concchans determined by the # of a pool, and
** further scales the conductance. In effect this is acting like
** a 1-d diffusion object as well.
** A further message adds a potential across the concchan. This
** alters the driving force on the ion according to the GHK eqn.
*/


int ConcchanFunc(concchan,action)
register struct concchan_type *concchan;
Action		*action;
{
MsgIn	*msg;
double	conc1 = 0.0;
double	conc2 = 0.0;

    if(debug > 1){
		ActionHeader("Concchan",concchan,action);
    }

    SELECT_ACTION(action){
		case INIT:
			break;
    	case PROCESS:
        			MSGLOOP(concchan,msg) {
            			case 0:		/* TYPE = SUBSTRATE. */
									/* msg 0 = # vol */
							conc1 = MSGVALUE(msg,1);
							if (conc1 > 0)
								conc1 = MSGVALUE(msg,0)/conc1;
							else
								conc1 = 0;
        				break;
						case 1:		/* TYPE = PRODUCT */
									/* msg 0 = # vol */
							conc2 = MSGVALUE(msg,1);
							if (conc2 > 0)
								conc2 = MSGVALUE(msg,0)/conc2;
							else
								conc2 = 0;
						break;
						case 2:		/* TYPE = NUMCHAN */
									/* msg 0 = n */
							concchan->n = MSGVALUE(msg,0);
						break;
						case 3:		/* TYPE = Vm */
									/* msg 0 = Vm */
							concchan->Vm = MSGVALUE(msg,0);
						break;
        			}
					if (concchan->is_active) {
						if (concchan->use_nernst) {
					/* I need to work out the math for this. It
					** looks like I might need to use the conductance
					** as well as the permeability, but surely there
					** is a simple relationship between them.
					*/
						} else {
							concchan->A = conc2 *
								concchan->perm * concchan->n;
							concchan->B = conc1 *
								concchan->perm * concchan->n;
						}
					}
			break;
    	case RESET: {
			int is_conc1 = 0;
			int is_conc2 = 0;
			concchan->use_nernst = 0;
        		MSGLOOP(concchan,msg) {
						case 0:
							is_conc1 += 1;
							break;
						case 1:
							is_conc2 += 1;
							break;
						case 2:		/* TYPE = NUMCHAN */
									/* msg 0 = # of channels */
							concchan->n = MSGVALUE(msg,0);
						break;
						case 3:		/* TYPE = Vm */
									/* msg 0 = Vm */
							concchan->use_nernst = 1;
							concchan->Vm = MSGVALUE(msg,0);
						break;
        		}
				concchan->is_active = (is_conc1 && is_conc2);
			}
        	break;
    	case CHECK: {
			int is_conc1 = 0;
			int is_conc2 = 0;
        		MSGLOOP(concchan,msg) {
						case 0:
							is_conc1 += 1;
							break;
						case 1:
							is_conc2 += 1;
							break;
						case 2:		/* TYPE = NUMCHAN */
									/* msg 0 = # of channels */
							concchan->n = MSGVALUE(msg,0);
						break;
						case 3:		/* TYPE = Vm */
									/* msg 0 = Vm */
							concchan->use_nernst = 1;
							concchan->Vm = MSGVALUE(msg,0);
						break;
        		}
				if (is_conc1 > 1)
					ErrorMessage("concchan","Multiple CONC1 messages.",
						concchan);
				if (is_conc2 > 1)
					ErrorMessage("concchan","Multiple CONC2 messages.",
						concchan);
			}
        	break;
    	case SET :
        	return(0); /* do the normal set */
		/* NOTREACHED */
			break;
	}
	return 0;
}
