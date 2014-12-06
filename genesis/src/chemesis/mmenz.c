/* mmenz.c */
#include "chem_ext.h"

/*
** Avrama Blackwell, ERIM 
**
** Modified in 1998, at GMU, to allow for inverted U shaped feedback without
** modeling details of feedback reactions. E.g. effect of Ca on PLC
**
** Modified December 1999
**
** A Michaelis-Menton Enzymatic Rxn element. must be used 
** in conjunction with pools. */

/*			   kf1	     kf2
** [Enzyme] + [Substrate] <-> [E-S] -> [Enzyme] + [Product]
**			   kb1
**
**** ASSUME that E-S steady state always,  E = Etot - ES
**
**   dp/dt =  kf1*Etot*S/(S + KM)
**
***** KM = (kb1+kf2)/kf1
*/

#define HYPERBOLIC 0
#define SIGMOID 1

/* with feedback, decrease in enzyme activity is a function of concentration of
 * feedback substance:  
 *   hyperbolic form: fb propto 1 / conc(feedback substance) for neg feedback
 *                    fb propto conc (feeback substance) for positive feedback.
 *    Sigmoid form: fb propto conc/conc + halfmax (for neg feedback)
 *                  fb propto halfmax / conc + halfmax (for pos feedback)
*/

/* Inputs units:
   Substrate: units of concentration
   Half Conc: units of concentration
   Enzyme: units of molecules
   Vmax: inverse time
   Km: units of concentration
  
   Output units:   molecules, convert to concentration in rxnpool */

mmenz(enz,action)
register struct mmenz_type *enz;
Action		*action;
{
MsgIn	*msg;
double	dt;
float temp;
float fbconc;
float subs;
int i;

    if(debug > 1){
		ActionHeader("enz",enz,action);
    }

    SELECT_ACTION(action){
	case INIT:
		enz->product = 0;
		break;
		
    	case PROCESS:
	        enz->product = 0;
		enz->feedback = 1;
		enz->subs_rate = 1;
		MSGLOOP(enz,msg) 
		  {
		  case 0:	/* enzyme concentration */
		    /* 0 = Conc or Quantity of enzyme*/
		    enz->product += enz->Vmax*MSGVALUE(msg,0);
		    break;

    /***** ignore the following if no feedback **********/

		  case 1: /* feedback */
		    /* 0 = Conc of feedback substrate */
		    fbconc = MSGVALUE(msg,0);
		/* compute effect of feedback.  feedback always <= 1*/
		    if (fbconc < enz->thresh)
		      {
		      if (enz->pos_pow > 0)
			{
			  if (enz->pos_form == HYPERBOLIC)
			    temp = fbconc/enz->thresh;
			  if (enz->pos_form == SIGMOID)
			    temp = fbconc/(fbconc+enz->pos_halfmax);
			  for (i=1; i<=enz->pos_pow; i++)
			    enz->feedback = enz->feedback*temp;
			}
		      }
		      else if (fbconc > enz->thresh)
			{
			if (enz->neg_pow > 0)
			  {
			    if (enz->neg_form == HYPERBOLIC)
			      temp = enz->thresh/fbconc;
			    if (enz->neg_form == SIGMOID)
			      temp = enz->neg_halfmax / (fbconc + enz->neg_halfmax);
			    for (i=1; i<=enz->neg_pow; i++)
			      enz->feedback = enz->feedback*temp;
			  }
			}
		    else
		      enz->feedback = 1;
		    break;
    /***** end of feedback section  **********/


		  case 2: /* substrate */
		    /* 0 = Conc of enzyme substrate */
		    subs = MSGVALUE(msg,0);
		    enz->subs_rate = subs / ( subs + enz->Km);
		  }
		enz->product *= enz->feedback * enz->subs_rate;
		break;

    	case RESET:
		enz->product = 0;
        	break;

    	case SET :
        	return(0); /* do the normal set */
		break;
	}
}
