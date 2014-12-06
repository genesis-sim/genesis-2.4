/* diffusion.c */
#include "chem_ext.h"

/* Implementation of 1-D or 2-D diffusion across one surface between pools.
** This computes the flux between two pools.  It sends messages back to
** both pools, thus, no need to set up diffusion in both directions.
** Need one diffusion object for every surface of a pool.*/

/* Units:
   
   Output Units: molecules
   Message DIFFUSION to rxnpool
   difflux2 goes to A of pool2 rxnpool
   difflux1 goes to A of pool1 rxnpool */

/* Avrama Blackwell, GMU, Dec 1997 
   Modified Dec 1999
   and again Mar 2002:
   quantity in units of molecules
   use geometric mean for cross-sectional area of diffusion.
   Modified Aug 2013 to fix length units*/

#define AVOGADRO 6.023e23         /* units are molecules per mole */

int diffusion(diffus,action)
register struct diffusion_type *diffus;
Action		*action;
{
double		lenmean;	/* sum of pool lengths */
double		areamean;	/* coupling coefficient */
double		difarea;	/* diffusion coefficient * area / length */
double		len1, len2;	/* length of pools */
double		area1, area2;	/* area of pools */
double		conc1, conc2;	/* concentration in pools */
double		dt;
int		n1, n2;	        /* used to check messages */
MsgIn		*msg;
double		savedata[2];
int			n;

    if(debug > 1){
	ActionHeader("diffusion",diffus,action);
    }

    SELECT_ACTION(action){
    case INIT:
	diffus->difflux1 = 0;
	diffus->difflux2 = 0;
	break;

    case PROCESS:
	/* Read the msgs.  */
	dt = Clockrate(diffus);
	MSGLOOP(diffus,msg){
	case 0:	/* params of pool 1*/
	    	        /* 0 = Len */
	    	        /* 1 = area */
	    	        /* 2 = conc */
	      len1 = MSGVALUE(msg,0);
	      area1 = MSGVALUE(msg,1);
	      conc1 = MSGVALUE(msg,2);
	      break;
	case 1:		/* params of pool 2*/
				/* 0 = Len */
				/* 1 = area */
				/* 2 = conc */
	      len2 = MSGVALUE(msg,0);
	      area2 = MSGVALUE(msg,1);
	      conc2 = MSGVALUE(msg,2);
	      break;
	}
	    /* dConc/dt = -D/vol *((area*(Ca-Ca_j)/(len/2+len_j/2) 
	     ** compute flux = dmol/dt (don't divide by volume here)
	     ** area is mean X sectional area between two compartments. */

	   lenmean = (len1+len2)*diffus->Dunits / 2 ;     /* meters */
   	   areamean = sqrt(area1*area2)*(diffus->Dunits*diffus->Dunits);     /* meters^2 */
	   /* geometric mean more appropriate if areas drastically different */
	   difarea = diffus->D*(diffus->Dunits*diffus->Dunits) * (areamean/lenmean);    /*  / sec */
    if (debug > 0)
      printf("%s: lenmean= %g, areamean= %g, difarea=%g\n", diffus->name,lenmean, areamean,difarea);

	    diffus->difflux2 = difarea * (conc1-conc2) * AVOGADRO * diffus->units;
	    diffus->difflux1 = -diffus->difflux2;
 	break;	

    case RESET:
	diffus->difflux1 = 0;
	diffus->difflux2 = 0;
	break;

    case CHECK:
	/* scaled diffusion constant */
	if(diffus->D <= 0.0){
	    ErrorMessage("Diffusion", "Invalid D.", diffus);
	}
	n1=n2=0;
	MSGLOOP(diffus,msg){
	    case 0:			/* pool parameters */
		if((MSGVALUE(msg,0) <= 0.0)||(MSGVALUE(msg,1) < 0.0)){
	    	ErrorMessage("diffusion","Incoming len, area data invalid.",diffus);
		}
		n1 += 1;
		break;
	    case 1:			/* pool parameters */
		if((MSGVALUE(msg,0) <= 0.0)||(MSGVALUE(msg,1) < 0.0)){
	    	ErrorMessage("diffusion","Incoming len, area data invalid.",diffus);
		}
		n2 += 1;
		break;
	}
        if (n1 != 1)
            ErrorMessage("diffusion", "incorrect number of msg from pool 1", diffus);
        if (n2 != 1)
            ErrorMessage("diffusion", "incorrect number of msg from pool 2", diffus);        
	break;
    }
    return(0);
}

