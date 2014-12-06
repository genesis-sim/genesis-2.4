static char rcsid[] = "$Id: prim.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: prim.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/09/21 19:34:52  mhucka
** Indented the code so I could read it, then included math.h and removed
** explicit type declarations for some standard math functions.
**
** Revision 1.2  1996/06/06 20:03:36  dhb
** Paragon port.
**
** Revision 1.1  1992/12/11  19:03:51  dhb
** Initial revision
**
*/

#include <math.h>
#include "sim_ext.h"

/***************************************************************************/
/*Adam Franklin Strassberg*/
/*March 15, 1992*/
/*prim.c contains the primitive functions for the pore population objects*/
/***************************************************************************/


/***************************************************************************/
/*RAN -prints a random 0-1 double value*/

#ifdef OLDVERSION
double RAN()
{
    long random();
    double dum;

    dum = ((double) random())/2147483647;
    return(dum);
}
#else
double RAN()
{
    double dum;

    dum = (double) urandom();
    return(dum);
}
#endif
/****************************************************************************/


/********************************************************************************/
/*RATE - prints the HH rate function*/
double RATE(V,A,B,C,D,F)
	float V;
	float A;
	float B;
	float C;
	float D;
	float F;

{
    double dum;
    double OUT;

    dum = exp((D+V)/F);
    if (C==-1*dum) OUT = -1*B*F/C;
    else OUT = ( A + B*V ) / ( dum + C);
    return(OUT);
}
/********************************************************************************/


/********************************************************************************/
/*POWER - takes n to the kth power (where n is an integer and k is a float)*/
double POWER(n,k)
	int n;
	float k;

{
    double a,b,OUT;

    a = n;
    b = k;
    OUT = pow(a,b);
    return(OUT);
}
/********************************************************************************/
