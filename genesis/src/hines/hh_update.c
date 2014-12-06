static char rcsid[] = "$Id: hh_update.c,v 1.5 2006/01/17 15:05:00 svitak Exp $";

/* Version EDS22i 99/12/21, Erik De Schutter, Caltech & BBF-UIA 4/92-12/99 */
/* Upinder S. Bhalla Caltech May-December 1991 */

/*
** $Log: hh_update.c,v $
** Revision 1.5  2006/01/17 15:05:00  svitak
** Changes for voltage-dependent Z gate and new purkinje tutorial.
**
** Revision 1.4  2006/01/10 19:56:39  svitak
** Changes to allow voltage-dependent Z-gate.
**
** Revision 1.3  2005/07/20 20:01:59  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  1999/12/29 10:26:13  mhucka
** New updates from Erik De Schutter
**
 * EDS22i revison: EDS BBF-UIA 99/12/20-99/12/21
 * Added instant gates
**
** Revision 1.5  1997/07/23 21:58:56  dhb
** Added use of INLINE macro to control use of inline for
** upi_pow().  Defaults to empty resulting in no inlining.
** Set to inline to get inlining.
**
** Revision 1.4  1997/05/28 22:47:58  dhb
** Replaced with version from Antwerp GENESIS 21e
**
** Revision 1.3  1995/12/06 00:43:15  venkat
** Moved the non-hsolved channel elements into a seperately allocated
** element instead of using the int ops array. Was causing core-dumps on
** the 64-bit alpha machines - This version now uses a separately allocated
** element pointer array and the ops array indexes the right element.
**
** Revision 1.2  1995/07/18  17:56:53  dhb
** Changed all uses of struct Ca_shell_type to struct Ca_concen_type.
**
** Revision 1.1  1992/12/11  19:03:10  dhb
** Initial revision
**
*/

#include <math.h>
#include "hines_ext.h"

/* This file contains the routines for updating the coeffs of the 
** hh channels. These routines are non-object-oriented. These routines
** typically use the most cpu time.
*/ 

/*
** A utility function for do_hh_update()
*/

#ifndef INLINE
#define INLINE
#endif

INLINE double upi_pow(x,y)
	double x;
	float  y;
{
	double x1;

	if (y==2.0)
		return(x*x);
	if (y==1.0)
		return(x);
	if (y==3.0)
		return(x*x*x);
	if (y==4.0) {
		x*=x;
		return(x*x);
	}
	if (y==5.0) {
		x1=x;
		x*=x;
		x*=x*x1;
		return(x);
	}
	if (fabs(y)<VTINY) return(1.0);

	return(pow(x,y));
}


/* this is the plain vanilla, default version */

void do_hh_update(hsolve)
	Hsolve	*hsolve;
{
	int	ncompts = hsolve->ncompts;
	int	*elmnum,instant;
	struct compartment_type	**compts;
	Tcinfo	**hh;
	Tcinfo	*hentry;
	register int		i;
	double	dt;
	double	tby2;
	double	c,g,temp;
	double	TabInterp();
	Tchan	*h;
	register double v;
	double upi_pow();
	double X,Y,Z;
	double  A,B;
	float Xpow,Ypow,Zpow;

	elmnum = hsolve->elmnum;
	compts = (struct compartment_type **)hsolve->compts;
	hh=hsolve->hh;

	dt = Clockrate(hsolve);
	tby2 = dt/2.0;
	
	for(i=0;i<ncompts;i++) {
		if (!(hentry=hh[i]))
			continue;
		v = compts[elmnum[i]]->Vm;
		/* looping over all hh chans on this compt */
		for(;hentry;hentry=hentry->next) {
			h=(Tchan *)(hentry->chan);
			g=h->Gbar;
			instant = h->instant;
			if ((Xpow = h->Xpower) > TINY) {
			    A = TabInterp(h->X_A,v);
			    B = TabInterp(h->X_B,v);
			    if (instant & INSTANTX) {
				X=h->X = A / B;
			    } else {
				/* Trapezoid method of integration */
				temp=1.0+tby2*B;
				X=h->X=(h->X*(2.0-temp)+dt*A)/temp;
			    }
			    g*=upi_pow(X,Xpow);
			}
			if ((Ypow = h->Ypower) > TINY) {
			    A = TabInterp(h->Y_A,v);
			    B = TabInterp(h->Y_B,v);
			    if (instant & INSTANTY) {
				Y=h->Y = A / B;
			    } else {
				/* Trapezoid method of integration */
				temp=1.0+tby2*B;
				Y=h->Y=(h->Y*(2.0-temp)+dt*A)/temp;
			    }
			    g*=upi_pow(Y,Ypow);
			}
			if ((Zpow = h->Zpower) != 0.0){
			  if (h->Z_conc) {
			    c = MSGVALUE(hentry->conc,0);
			    if (Zpow > TINY) {
				A = TabInterp(h->Z_A,c);
				B = TabInterp(h->Z_B,c);
			    } else { /* Zpow is negative or positive very close to zero */
				A = c * TabInterp(h->Z_A,v);
				B = TabInterp(h->Z_B,v);
			    } 
			  } else { /* Z_conc is unset, use voltage */
			    A = TabInterp(h->Z_A,v);
			    B = TabInterp(h->Z_B,v);
			  }
			    if (instant & INSTANTZ) {
				Z=h->Z = A / B;
			    } else {
				temp=1.0+tby2*B;
				Z=h->Z=(h->Z*(2.0-temp)+dt*A)/temp;
			    }
			    g*=upi_pow(Z,fabs(Zpow));
			}
			if(hentry->nernst) {
			    h->Ek = hentry->nernst->E;
			}
			if (g==0.0) {
				h->Gk=0.0;
				h->Ik=0.0;
			} else {
				h->Gk=g;
				h->Ik=g*(h->Ek - v);
			}
		}
	}
}

