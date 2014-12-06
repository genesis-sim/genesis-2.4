static char rcsid[] = "$Id: ghk.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/* Version EDS21c, Erik De Schutter, Caltech & BBF-UIA 4/92-10/96 */

/*
** $Log: ghk.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1997/05/29 09:00:30  dhb
** Initial revision
**
*/

 /* EDS 9/93: **  eq. z^2 */
 /* EDS 4/94: removed all scaling factors: assumes mM/Volt/sec units */
 /* EDS 7/94: added Alex' second ion stuff */
 /* EDS 8/94: checked accuracy and removed Alex' second ion stuff,
 **  because it created discontinuties around the reversal potential 
 ** Note also that our use of p==Ca_Gk is not entirely correct, as the
 ** units (m/s versus S/cm^2) are quite different.
 ** The resulting current is more than a factor 10^6 larger, so Gbar
 ** must be MUCH smaller */
 /* EDS 6/95: removed 'factor 10 normalization' */
 /* A thought about the units, by EDS 6/95 (based on Hille 92):
 **  Units: p in m/s
 **         EF/RT and z are dimensionless
 **         F in C/mol
 **         Cin and Cout in mM==mol/m^3
 **  Then Ik is really in units of C/s/m^2==A/m^2, so we compute a
 **  current density.  As we replace in practice p by Gk, which has
 **  already been scaled for surface, we get rid of the density factor.
 ** EDS 10/96: corrected Taylor expansion
 */

/* ghk.c Subba Shankar 8/25/91
 * This object calculates the current through a membrane,
 * the reversal potential, and the chord conductance
 * from the Goldman Hodgkin Katz equation (constant field equation).
 *
 * modified from nernst.c written by Matt Wilson
 *
 *  The equation used is:
 *
 *                                       Cin*exp(K * Vm) - Cout
 *   Ik    =  p * valency * F * K * Vm  ------------------------
 *                                          exp(K * Vm) - 1
 *
 *            valency * F
 * where K = -------------
 *           R * (T + 273)
 *
 *      F = Faraday's constant (internal)
 *
 *      R = universal gas constant (internal)
 *
 *
 * Ek is the Nernst (or reversal) potential, and Gk is the chord
 * conductance, calculated from Ohm's Law.
 *
 * A full explanation of the constant field equn can be found in
 *    1) _From Neuron to Brain_, Kuffler, et al., Sinauer Assoc.
 *        2nd edition, (1984) p. 123, 130.
 *    2)  _Electric Current Flow in Excitable Cells_, Jack, et al.,
 *        Oxford Press, (1983) p. 237.
 *    3)  _Ionic Channels of Ecitable Membranes_, Hille, Sinauer Assoc.
 *        2nd edition, (1992) pp 345-347.
 *
 * Note that these three references differ in their use of the
 * valence, and that the Kuffler equation would cause outward currents
 * to be negative.
 *
 * The current field can be used directly, but if the
 * object that you are passing this information to
 * does not understand current messages (such as compartment)
 * then pass the Ek and Gk values (e.g. in a CHANNEL mesg).
 * This will yield the equivalent current flow from Ohm's law.
 *
 * For Ek we use the Nernst equation.
 */

#include <math.h>
#include "seg_ext.h"


/* define universal constants needed for
 * calculations.  Taken from _Ionic Channels of
 * Excitable Membranes_ by Bertil Hille (1984)
 * Sinauer Associates, Sunderland MA, pg. 7.
 *  Note that GAS_CONSTANT is usually called R.
 */
/* SI units */
#define GAS_CONSTANT	8.314			/* (V * C)/(deg K * mol) */
#define FARADAY		9.6487e4			/* C / mol */
#define ZERO_CELSIUS	273.15			/* deg */
#define R_OVER_F        8.6171458e-5		/* volt/deg */
#define F_OVER_R        1.1605364e4		/* deg/volt */

/* define the message types that are accepted */

#define CIN 0
#define COUT 1
#define VOLTAGE 3
#define PERMEABILITY 4

/*
** a single type of ion
** T has units of degrees Celsius
**
** In order to save time the common expression:
**     [valency*F]/[R*(temp in deg K)]
**  is calculated and saved in ghk->constant
*/

void Ghk(ghk,action)
     register struct ghk_type *ghk;
     Action		*action;
{
  MsgIn 	*msg;

       /* following are used for common expression reduction */
  double         exponent, e_to_exponent;

  /* code: */

  if(debug > 1){
    ActionHeader("Ghk",ghk,action);
  }


  SELECT_ACTION(action){
  case PROCESS:
	ghk->p = 0;   /* allows us to add permeabilities */

    MSGLOOP(ghk,msg){
    case CIN:		/* intracellular ionic concentration */
      ghk->Cin = MSGVALUE(msg,0);
	  break;
	case COUT:		/* extracellular ionic concentration */
	  ghk->Cout = MSGVALUE(msg,0);
	  break;
	case VOLTAGE:           /* transmembrane voltage */
	  ghk->Vm = MSGVALUE(msg,0);
	  break;
	case PERMEABILITY:           /* permeability,    */
	  ghk->p += MSGVALUE(msg,0);
	  break;
	} /* end of MSGLOOP */

        /*  Now calculate the output fields */


	/*   Calculate the reversal potential: */

	ghk->Ek = log(ghk->Cout/ghk->Cin) / ghk->constant;

	/*  calculate the current:  
	 *     1) check for singularities near zero voltage
	 *     2) calculate the common expressions
	 *     3) calculate the current and the chord conductance
	 */

    exponent = ghk->constant*(ghk->Vm);

    e_to_exponent = exp(-exponent);


    if ( fabs(exponent) < .00001 ) {
	   /* exponent near zero, calculate current some other way */

      /* take Taylor expansion of V'/[exp(V') - 1], where
	  * V' = constant * Vm
	  *  First two terms should be enough this close to zero
	  */

      ghk->Ik = -ghk->valency * ghk->p * FARADAY * 
		(ghk->Cin - (ghk->Cout * e_to_exponent)) / (1-0.5 * exponent);
      
    } else {       /* exponent far from zero, calculate directly */

      ghk->Ik = -ghk->p * FARADAY * ghk->valency * exponent * 
	       	 (ghk->Cin - (ghk->Cout * e_to_exponent)) / (1.0 - e_to_exponent);

    }

    /* Now calculate the chord conductance, but
     * check the denominator for a divide by zero.  */

	exponent=ghk->Ek - ghk->Vm;
    if ( fabs(exponent) < 1e-12 ) {
      /* we are very close to the rest potential, so just set the 
       * current and conductance to zero.  */

      ghk->Ik = ghk->Gk = 0.0;
    } else { /* calculate in normal way */
      ghk->Gk = ghk->Ik / exponent;
    } 
    break;  /* end of case PROCESS */

  case RESET:
	/* Calculate the constant once. */
    ghk->constant = F_OVER_R*ghk->valency/ (ghk->T + ZERO_CELSIUS);
    break;  /* end of case RESET */


  case CHECK:

    if(ghk->valency == 0){
      ErrorMessage("Ghk", "Invalid ionic valency z.", ghk);
    }
    if(ghk->Cin < 0.0){
      ErrorMessage("Ghk", "Invalid intracellular ionic concentration.",
		   ghk);
    }
    if(ghk->Cout < 0.0){
      ErrorMessage("Ghk", "Invalid extracellular ionic concentration.",
		   ghk);
    }
    if(ghk->T + ZERO_CELSIUS <= 0){
      ErrorMessage("Ghk", "Invalid temperature parameter.", ghk);
    }
    if(ghk->p < 0.0){
      ErrorMessage("Ghk", "Invalid permeability parameter.", ghk);
    }
    break;  /* end of case CHECK */

  } /* end of SELECT_ACTION */
} /* end of function Ghk */


