static char rcsid[] = "$Id: Kpore.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: Kpore.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/09/21 19:33:39  mhucka
** Included math.h and removed explicit type declarations for some standard
** math functions.
**
** Revision 1.2  1996/06/06 20:03:36  dhb
** Paragon port.
**
** Revision 1.1  1992/12/11  19:03:49  dhb
** Initial revision
**
*/

/***************************************************************/
/*Adam Franklin Strassberg*/
/*March 15, 1992*/
/*Kchan.c contains the defining action function*/
/*for the Potassium pore population object*/
/***************************************************************/

#include <math.h>
#include "pore_ext.h"

#define Ss(field) Kpore->field

void KPorePop(Kpore, action)

struct   K_pore_type *Kpore;
Action   *action; 

{
	MsgIn    *msg;
	double   dt;
	double   alpha, beta, ninf;
	int      n0new, n1new, n2new, n3new, n4new; 
	int      tempsum;
	int 	 i;

	if (debug > 1)
		ActionHeader("KPorePop", Kpore, action);

	SELECT_ACTION(action) {

        /**************************************************************************/
	case INIT:
		break;
        /**************************************************************************/

        /**************************************************************************/
	case PROCESS:
		dt = Clockrate(Kpore);

	/*Update state*/
		MSGLOOP(Kpore, msg)
			{case 0:
				Ss(Vk) = MSGVALUE(msg, 0);
				break;}

	/*Update Rate Constants for new Vk*/
		alpha = RATE(Ss(Vk),Ss(alpha_A),Ss(alpha_B),Ss(alpha_C),
			     Ss(alpha_D),Ss(alpha_F));
		beta = RATE(Ss(Vk),Ss(beta_A),Ss(beta_B),Ss(beta_C),
			    Ss(beta_D),Ss(beta_F));

 	/*Compute New states n0new, n1new, n2new, n3new, n4new */ 
		n0new = n1new = n2new = n3new = n4new = 0;

	/*Within the time step dt, for each pore, 
	1) What is the probability of that pore leaving the current state?
	2) If it does leave, to what state does it travel?
	As dt is made smaller and smaller, this approximation will converge
	unto the continuous kinetics.*/

		for ( i=0; i<Ss(n0); i++ )		
		{if ( RAN() <= exp(-1.0*(4.0*alpha)*dt) ) n0new++;
		else n1new++;}

		for ( i=0; i<Ss(n1); i++ )		
		{if ( RAN() <= exp(-1.0*(3.0*alpha+beta)*dt) ) n1new++;
		else 
			{if ( RAN() <= (beta/(3.0*alpha+beta)) ) n0new++;
			else n2new++;}}

		for ( i=0; i<Ss(n2); i++ )		
		{if ( RAN() <= exp(-1.0*(2.0*alpha+2.0*beta)*dt) ) n2new++;
		else 
			{if ( RAN() <= ((2.0*beta)/(2.0*alpha+2.0*beta)) ) n1new++;
			else n3new++;}}

		for ( i=0; i<Ss(n3); i++ )		
		{if ( RAN() <= exp(-1.0*(1.0*alpha+3.0*beta)*dt) ) n3new++;
		else 
			{if ( RAN() <= ((3.0*beta)/(1.0*alpha+3.0*beta)) ) n2new++;
			else n4new++;}}

		for ( i=0; i<Ss(n4); i++ )		
		{if ( RAN() <= exp(-1.0*(4.0*beta)*dt) ) n4new++;
		else n3new++;}

	/*Update the old states to the new states*/
		Ss(n0) = n0new;
		Ss(n1) = n1new;
		Ss(n2) = n2new;
		Ss(n3) = n3new;
		Ss(n4) = n4new;
	/*State n4 is the only open state*/
		Ss(No) = Ss(n4);

	/*Update Current*/
		Ss(Gk) = Ss(Gmax)*Ss(No);
 		Ss(Ik) = (Ss(Ek) - Ss(Vk)) * Ss(Gk);

		break;
        /**************************************************************************/

        /**************************************************************************/
	case RESET:
	/*Seed Random Number generator*/	
#ifdef OLDVERSION
		srandom(time(0));
#endif
	/*Update state*/
		MSGLOOP(Kpore, msg)
		{case 0:
			Ss(Vk) = MSGVALUE(msg, 0);
			break;}

	/*Update Rate Constants for new Vk*/
		alpha = RATE(Ss(Vk),Ss(alpha_A),Ss(alpha_B),Ss(alpha_C),
			     Ss(alpha_D),Ss(alpha_F));
		beta = RATE(Ss(Vk),Ss(beta_A),Ss(beta_B),Ss(beta_C),
			    Ss(beta_D),Ss(beta_F));
		ninf = alpha/(alpha + beta);

	/*Set initial distribution of the pore population into the various
	kinetic states based upon the equilbrium solution of the Markov matrix*/
		Ss(n0) = (int) ( Ss(Nt)*pow((1.0-ninf),4.0) );
		Ss(n1) = (int) ( Ss(Nt)*4.0*pow((1.0-ninf),3.0)*pow(ninf,1.0) );
		Ss(n2) = (int) ( Ss(Nt)*6.0*pow((1.0-ninf),2.0)*pow(ninf,2.0) );
		Ss(n3) = (int) ( Ss(Nt)*4.0*pow((1.0-ninf),1.0)*pow(ninf,3.0) );
		Ss(n4) = (int) ( Ss(Nt)*pow(ninf,4.0) );

		tempsum = Ss(n0) + Ss(n1) + Ss(n2) + Ss(n3) + Ss(n4);
		if (Ss(Nt) > tempsum) Ss(n0) += (Ss(Nt) - tempsum); 
	/*Makes sure that total population equals Nt
	enforce the constraint of an integer population*/

	/*The fourth kinetic state is the only open state*/
		Ss(No) = Ss(n4);

	/*Update Current*/
		Ss(Gk) = Ss(Gmax)*Ss(No);
 		Ss(Ik) = (Ss(Ek) - Ss(Vk)) * Ss(Gk);

		break;
        /**************************************************************************/

        /**************************************************************************/
	case CHECK:
		break;
	}
}
#undef Ss
