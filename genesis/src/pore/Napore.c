static char rcsid[] = "$Id: Napore.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: Napore.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/09/21 19:33:51  mhucka
** Included math.h and removed explicit type declarations for some standard
** math functions.
**
** Revision 1.2  1996/06/06 20:03:36  dhb
** Paragon port.
**
** Revision 1.1  1992/12/11  19:03:50  dhb
** Initial revision
**
*/

/***************************************************************/
/*Adam Franklin Strassberg*/
/*March 15, 1992*/
/*Nachan.c contains the defining action function*/
/*for the Sodium pore population object*/
/***************************************************************/

#include <math.h>
#include "pore_ext.h"

#define Ss(field) Napore->field

void NaPorePop (Napore, action)

struct   Na_pore_type *Napore;
Action   *action; 

{
	MsgIn    *msg;
	double   dt;
	double   malpha, mbeta, minf;
	double   halpha, hbeta, hinf;
	double	 rantemp, probtemp, temp1, temp2;
	int      m0h0new, m1h0new, m2h0new, m3h0new; 
	int      m0h1new, m1h1new, m2h1new, m3h1new; 
	int      tempsum;
	int 	 i;

	if (debug > 1)
		ActionHeader("NaPorePop", Napore, action);

	SELECT_ACTION(action) {

        /*****************************************************************************/
	case INIT:
		break;
        /*****************************************************************************/

        /*****************************************************************************/
	case PROCESS:
		dt = Clockrate(Napore);

	/*Update state*/
		MSGLOOP(Napore, msg)
			{case 0:
				Ss(Vk) = MSGVALUE(msg, 0);
				break;}

	/*Update Rate Constants for new Vk*/
		malpha = RATE(Ss(Vk),Ss(malpha_A),Ss(malpha_B),Ss(malpha_C),
			     Ss(malpha_D),Ss(malpha_F));
		mbeta = RATE(Ss(Vk),Ss(mbeta_A),Ss(mbeta_B),Ss(mbeta_C),
			     Ss(mbeta_D),Ss(mbeta_F));
		halpha = RATE(Ss(Vk),Ss(halpha_A),Ss(halpha_B),Ss(halpha_C),
			      Ss(halpha_D),Ss(halpha_F));
		hbeta = RATE(Ss(Vk),Ss(hbeta_A),Ss(hbeta_B),Ss(hbeta_C),
			     Ss(hbeta_D),Ss(hbeta_F));

	/*Compute New states m0h1new, m1h1new, m2h1new, m3h1new*/ 
      		m0h0new = m1h0new = m2h0new = m3h0new = 0; 
      		m0h1new = m1h1new = m2h1new = m3h1new = 0; 

	/*Within the time step dt, for each pore,
	1) What is the probability of that pore leaving the current state?
	2) If it does leave, to what state does it travel?
	As dt is made smaller and smaller, this approximation will converge
	unto the continuous dynamics.*/

	probtemp = (3.0*malpha+hbeta);
	temp1 = hbeta/probtemp;
	for ( i=0; i<Ss(m0h1); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m0h1new++;
	else
 		{if ( RAN() <= temp1 ) m0h0new++;
		else m1h1new++;}}

	probtemp = (2.0*malpha+mbeta+hbeta);
	temp1 = mbeta/probtemp;
	temp2 = hbeta/probtemp;
	for ( i=0; i<Ss(m1h1); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m1h1new++;
	else
		{rantemp = RAN();
		if ( rantemp <= temp1 ) m0h1new++;
		else
			{if (rantemp >= (temp1+temp2)) m2h1new++;
			else m1h0new++;}}}

	probtemp = (malpha+2.0*mbeta+hbeta);
	temp1 = 2.0*mbeta/probtemp;
	temp2 = hbeta/probtemp;
	for ( i=0; i<Ss(m2h1); i++ )		
			{if ( RAN() <= exp(-1.0*probtemp*dt) ) m2h1new++;
			else
			{rantemp = RAN();
			if ( rantemp <= temp1 ) m1h1new++;
			else
				{if (rantemp >= (temp1+temp2)) m3h1new++;
				else m2h0new++;}}}

	probtemp = (3.0*mbeta+hbeta);
	temp1 = 3.0*mbeta/probtemp;
	for ( i=0; i<Ss(m3h1); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m3h1new++;
	else
	{rantemp = RAN();
		if ( rantemp <= temp1 ) m2h1new++;
		else m3h0new++;}}

	probtemp = (3.0*mbeta+halpha);
	temp1 = 3.0*mbeta/probtemp;
	for ( i=0; i<Ss(m3h0); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m3h0new++;
	else
		{rantemp = RAN();
		if ( rantemp <= temp1 ) m2h0new++;
		else m3h1new++;}}

	probtemp = (malpha+2.0*mbeta+halpha);
	temp1 = 2.0*mbeta/probtemp;
	temp2 = halpha/probtemp;
	for ( i=0; i<Ss(m2h0); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m2h0new++;
	else
	{rantemp = RAN();
		if ( rantemp <= temp1 ) m1h0new++;
		else
			{if (rantemp >= (temp1+temp2)) m3h0new++;
			else m2h1new++;}}}

	probtemp = (2.0*malpha+mbeta+halpha);
	temp1 = mbeta/probtemp;
	temp2 = halpha/probtemp;
	for ( i=0; i<Ss(m1h0); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m1h0new++;
	else
		{rantemp = RAN();
		if ( rantemp <= temp1 ) m0h0new++;
		else
			{if (rantemp >= (temp1+temp2)) m2h0new++;
			else m1h1new++;}}}

	probtemp = (3.0*malpha+halpha);
	temp1 = 3.0*malpha/probtemp;
	for ( i=0; i<Ss(m0h0); i++ )		
	{if ( RAN() <= exp(-1.0*probtemp*dt) ) m0h0new++;
	else
		{rantemp = RAN();
		if ( rantemp <= temp1 ) m1h0new++;
		else m0h1new++;}}

	/*Update the old states to the new states*/
	Ss(m0h0) = m0h0new;
	Ss(m1h0) = m1h0new;
	Ss(m2h0) = m2h0new;
	Ss(m3h0) = m3h0new;
	Ss(m0h1) = m0h1new;
	Ss(m1h1) = m1h1new;
	Ss(m2h1) = m2h1new;
	Ss(m3h1) = m3h1new;
	/*state m3h1 is the only open state*/
	Ss(No) = Ss(m3h1);

	/*Update Current*/
	Ss(Gk) = Ss(Gmax)*Ss(No);
 	Ss(Ik) = (Ss(Ek) - Ss(Vk)) * Ss(Gk);

	break;
        /*****************************************************************************/

        /*****************************************************************************/
	case RESET:
	/*
	**Seeding not neccessary with internal Genesis random no generator
	*/
#ifdef OLDVERSION
	/*Seed Random Number generator*/
		srandom(time(0));
#endif

	/*Update state*/
		MSGLOOP(Napore, msg)
			{case 0:
				Ss(Vk) = MSGVALUE(msg, 0);
				break;}

	/*Update Rate Constants for new Vk*/
		malpha = RATE(Ss(Vk),Ss(malpha_A),Ss(malpha_B),Ss(malpha_C),
			      Ss(malpha_D),Ss(malpha_F));
		mbeta = RATE(Ss(Vk),Ss(mbeta_A),Ss(mbeta_B),Ss(mbeta_C),
			     Ss(mbeta_D),Ss(mbeta_F));
		halpha = RATE(Ss(Vk),Ss(halpha_A),Ss(halpha_B),Ss(halpha_C),
			      Ss(halpha_D),Ss(halpha_F));
		hbeta = RATE(Ss(Vk),Ss(hbeta_A),Ss(hbeta_B),Ss(hbeta_C),
			     Ss(hbeta_D),Ss(hbeta_F));
		minf = malpha/(malpha + mbeta);
		hinf = halpha/(halpha + hbeta);

	/*Set initial distribution of the pore population into the various
	kinetic states based upon the equilibrium solution of the Markov matrix*/

		Ss(m0h1) = (int) ( Ss(Nt)*(1.0-hinf)*pow((1.0-minf),3.0) );
		Ss(m1h1) = (int) ( Ss(Nt)*(1.0-hinf)
				   *3.0*pow((1.0-minf),2.0)*pow(minf,1.0) );
		Ss(m2h1) = (int) ( Ss(Nt)*(1.0-hinf)
				   *3.0*pow((1.0-minf),1.0)*pow(minf,2.0) );
		Ss(m3h1) = (int) ( Ss(Nt)*(1.0-hinf)*pow(minf,3.0) );

		Ss(m0h0) = (int) ( Ss(Nt)*hinf*pow((1.0-minf),3.0) );
		Ss(m1h0) = (int) ( Ss(Nt)*hinf
				   *3.0*pow((1.0-minf),2.0)*pow(minf,1.0) );
		Ss(m2h0) = (int) ( Ss(Nt)*hinf
				   *3.0*pow((1.0-minf),1.0)*pow(minf,2.0) );
		Ss(m3h0) = (int) ( Ss(Nt)*hinf
				   *pow(minf,3.0) );

		tempsum = Ss(m0h0) + Ss(m1h0) + Ss(m2h0) + Ss(m3h0) +
			  Ss(m0h1) + Ss(m1h1) + Ss(m2h1) + Ss(m3h1) ;
                if (Ss(Nt) > tempsum) Ss(m0h1) += (Ss(Nt) - tempsum);
	/*Makes sure that total population equals Nt
	enforce the constarint of an integer population*/

	/*The kinetic state m3h1 is the only open state*/
		Ss(No) = Ss(m3h1);

	/*Update Current*/
		Ss(Gk) = Ss(Gmax)*Ss(No);
 		Ss(Ik) = (Ss(Ek) - Ss(Vk)) * Ss(Gk);

		break;
        /*****************************************************************************/

        /*****************************************************************************/
	case CHECK:
		break;
	}
}
#undef Ss
