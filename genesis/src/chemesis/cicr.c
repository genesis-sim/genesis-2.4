/*cicr.c*/
#include "chem_ext.h"

/* Implementation of Calcium-Induced-Calcium Release, and IP3-Induced-Calcium Release */

/* computes state variables associated with ip3 receptor channel.  It needs
** messages about IP3 and cal conc, as well as other states.  It sends 
** messages to cicrflux, which computes flux between two pools connected by
** the channel (i.e. cytosol and ER). */

/* Avrama Blackwell, ERIM */
/* Revised in August of 1999 so that it can be used with CICR and IICR */

int cicrpool(ip3r,action)
register struct IP3R_type *ip3r;
Action		*action;
{
double	A,B;		/* integrate dx(a,b,c)/dt = A - B*x(a,b,c) */
double	dt;
double calcium, ip3;
double conc_alpha, conc_beta, conc_gamma;	/* concentration of other states */
double kalpha, kbeta, kgamma;			/* backward rate constants */
double sumtot;
MsgIn 	*msg;
double	savedata[2];
int	na, nb, ng, nc, ni;		/* used to check number of messages */
int	n;

    if(debug > 1){
	ActionHeader("cicrpool",ip3r,action);
    }

    SELECT_ACTION(action){
    case INIT:
	ip3r->previous_state = ip3r->fraction;
	break;

    case PROCESS:
	dt = Clockrate(ip3r);
	A = B = sumtot = 0;
	kalpha=0;

	/* Read the msgs to get the rate const and conc of other ip3r states */
	MSGLOOP(ip3r,msg){
	    case 0:	/* alpha direction */
	    		/* 0 = alpha rate constant */
	    		/* 1 = x(+alpha) concentrationi*/
	    	conc_alpha = MSGVALUE(msg,1);
	    	kalpha = MSGVALUE(msg,0);
	    	break;
	    case 1:	/* beta direction */
	    		/* 0 = beta rate constant */
	    		/* 1 = x(+beta) concentration */
	    	conc_beta = MSGVALUE(msg,1);
	    	kbeta = MSGVALUE(msg,0);
	    	break;	    	
	    case 2:	/* gamma direction */
	    		/* 0 = gamma rate constant */
	    		/* 1 = x(+gamma) concentration */
	    	conc_gamma = MSGVALUE(msg,1);
	    	kgamma = MSGVALUE(msg,0);
	    	break;    	
	    case 3:	/* IP3 concentration */
	    	ip3 = MSGVALUE(msg,0);
	    	break;
	    case 4:	/* Cytosolic Calcium concentration */
	    	calcium = MSGVALUE(msg,0);
	    	break;
	    case 5:	/* conservation of mass */
	    	sumtot += MSGVALUE(msg,0);
	    	break;
	}
/*                    a   1		      (1-j)
** dx(a,b,c)/dt = (-1) * sum alpha(j,b,c)*(-ip3) * x(j,b,c) +
**		         j=0
**
**                    b   1		      (1-j)
**                (-1) * sum beta(a,j,c)*(-cal) * x(a,j,c) +
**		         j=0
**
**                    c   1		      (1-j)
**                (-1) * sum gamma(a,b,j)*(-cal) * x(a,b,j)
**		         j=0
*/

/* if going from a (or b or c) = 0 to a (or b or c) = 1,
**	forward rxn depends on ip3 or ca, 
**	hence -k*x*(ip3 or ca), => B += k * (ip3 or ca)
**	backward rxn is independent of ip3 or ca, 
**	hence  +k*xother state, => A += kother state * xother state
** if going from 1 to 0, forward rxn indep of ip3 or ca, bk rxn depends ip3 or ca:
**	-k*x => B += k;	+k_os*x_os*(ip3 or ca) => A += k_os * x_ox*(ip3 or ca)
*/

	if (ip3r->conserve == 0)
	  {
	   if ((ip3r->alpha > 0) || (kalpha > 0))
	    {
	    if (ip3r->alpha_state == 0)
	     {
		A += kalpha*conc_alpha;
		B += ip3r->alpha*ip3;
	     }
	    else
	     {
		A += kalpha*conc_alpha*ip3;
		B += ip3r->alpha;
	     }
	    }

 	    if (ip3r->beta_state == 0)
	     {
		A += kbeta*conc_beta;
		B += ip3r->beta*calcium;
	     }
	    else
	     {
		A += kbeta*conc_beta*calcium;
		B += ip3r->beta;
	     }

	    if (ip3r->gamma_state == 0)
	     {
		A += kgamma*conc_gamma;
		B += ip3r->gamma*calcium;
	     }
	    else
	     {
		A += kgamma*conc_gamma*calcium;
		B += ip3r->gamma;
	     }
	     
/* After accumulating rxn in all 3 directions, integrate to get new state var. */
/*	    ip3r->fraction = IntegrateMethod(ip3r->object->method,ip3r,
				ip3r->fraction,A,B,dt,"state_fraction");  */
	    ip3r->fraction = IntegrateMethod((B < 1.0e-10) ?
			FEULER_INT: ip3r->object->method,ip3r,
				ip3r->fraction,A,B,dt,"state_fraction");
	  }
	else	/* conserve === 1 */
	    ip3r->fraction = 1 - sumtot;
	    
	if (ip3r->fraction < ip3r->xmin)
		ip3r->fraction = ip3r->xmin;
	if (ip3r->fraction > ip3r->xmax)
		ip3r->fraction = ip3r->xmax;

	break;

    case CHECK:
        na=nb=ng=nc=ni=0;
        MSGLOOP(ip3r,msg){
             case 0:         /* only one alpha messages required! */
                na += 1;
                break;
             case 1:         /* only one beta messages required! */
                nb += 1;
                break;
             case 2:         /* only one gamma messages required! */
                ng += 1;
                break;
             case 3:         /* only one ip3 conc message allowed! */
                ni += 1;
                break;
             case 4:         /* only one cal conc message allowed! */
                nc += 1;
                break;
        }
        if ((ip3r->conserve == 0) && (na > 1 || (na = 0 && ip3r->alpha>0) || nb != 1 || ng != 1 || nc != 1 || ni > 1 || (ni = 0 && ip3r->alpha>0)))
            ErrorMessage("cicr", "incorrect number of msg.", ip3r);
	break;
	
    case RESET:
	ip3r->fraction = ip3r->xinit;
	break;
    
    }

    return(0);
    
}

