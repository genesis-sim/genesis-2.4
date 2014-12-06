static char rcsid[] = "$Id: hines_conc.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/* Version EDS22f 98/10/30, Erik De Schutter, Caltech & BBF-UIA 8/92-10/98 */

/*
 * EDS22f revison: EDS BBF-UIA 98/10/30-98/10/30
 * Added support for buffer concentration differences between shells
 *
** $Log: hines_conc.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/01/01 01:15:21  mhucka
** Fix from E. De Schutter for a small bug that causes numerical errors in
** simulations without buffers.
**
** Revision 1.3  1999/10/17 21:39:36  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* Contains the 2 loops executed to compute changes in concentration
** - a loop which updates the cchip array (do_concchip__update)
** - a loop which executes the cfuncs array (do_h_conc_solve)
*/

#include "hines_ext.h"

void do_h_conc_solve(hsolve)
/* interated implicit solution of concentration and buffer diffusion */
	Hsolve	*hsolve;
{
	register int i,l,m,n;
	register int ms_1,dw_1,sw,nc,nb,nr,nv;
	register double co,diag,res,newel,temp,temp2,diagc,resc;
	int nco,ms,dw,sw_1,nb0,nd,conr0,conv,conv0,method;
	int ndiffs=hsolve->ndiffs;
	int *numshells=hsolve->nshells;
	int *numshfix=hsolve->nshfix;
	int *fbufval=hsolve->fbufval;
	int *diffwidth=hsolve->nshrows;
	int *diffconc=hsolve->dconc;
	register double *cov0=hsolve->covals0;
	register double *covals=hsolve->covals;
	register double *conc=hsolve->conc;
	register double *coresult=hsolve->coresult;
	register double *codiag=hsolve->codiag;
	register double *result=hsolve->cores;
	register double *bufvals=hsolve->bufvals;

	method=BaseObject(hsolve)->method;

	/* solve covals array:
	**  Do Crank Nicholson solution of diffusion equation:
	**  [1+dt/2(b+f.Ca)]Bf* + dt/2.f.Bf.Ca*  = [1 -dt/2.b]Bf + dt.b.Btot
	**  -dt/2.Ddo.BfD* + [1 +dt/2(Ddo+Dup+b+f.Ca)]Bf* + dt/2.f.Bf.Ca*
	**	   -dt/2.Dup.BfU* = dt/2.Ddo.BfD + [1 -dt/2(Ddo+Dup-b)]Bf + dt/2.Dup.BfU
	**                     + dt.b.Btot 
	**  -dt/2.Ddo.CaD* + dt/2(b+f.Ca)Bf* + [1 +dt/2(Ddo+Dup+f.Bf)]Ca*
	**		-dt/2.Dup.CaU* = dt/2.Ddo.CaD + [1 -dt/2(Ddo+Dup)]Ca + dt/2.Dup.CaU
	**                     + dt.b.Btot - dt/2.b.Bf + dt.flux/v
	**  Do Crank Nicholson solution of diffusible buffer equation:
	**  The forward elimination is done while filling the array, so
	**  it contains only the diagonal upper band.
	**  We will refer to a particular example of what the complete
	**  array would look like below.  In this example shell rows
	**  are in lower case, buffer rows in capitals.  Diagonals are
	**  b, diffusion coupling factors are a,c and buffer rate factors
	**  are f,g:
	**  diffwidth=3, the example assumes we are in row #3 and following
	**  and factors marked with " have already undergone forward elimination
	**  Type    nr  0   1   2   3   4   5   6   7   8   9   10  11  result
	**  fbuffer 0   B0      F0                                      R0
	**  dbuffer 1       B1  F1      C1                              R1
	**  shell   2   f2  g2  b2          c2                          r2
	**  fbuffer 3               B3      F3                          R3
	**  dbuffer 4       A4          B4  F4      C4                  R4
	**  shell   5           a5  f5  g5  b5          c5              r5
	**  fbuffer 6                           B6      F6              R6
	**  dbuffer 7                   A7          B7  F7      C7      R7
	**  shell   8                       a8  f8  g8  b8          c8  r8
	**  ...
	**  After forwards elimination:
	**  fbuffer 0   1       F0"                                     R0"
	**  dbuffer 1       1   F1"     C1"                             R1"
	**  shell   2           1       h2" c2"                         r2"
	**  fbuffer 3               1       F3"                         R3"
	**  dbuffer 4                   1   F4"     C4"                 R4"
	**  shell   5                       1       h5" c5"             r5"
	**  fbuffer 6                           1       F6"             R6"
	**  dbuffer 7                               1   F7"     C7"     R7"
	**  shell   8                                   1       h8" c8" r8"
	** covals array becomes:
	**   0: F0" .   .
	**   3: F1" C1" .
	** 	 6: f2  g2  .  -> (f2) h2" c2"
	**   9: F3" .   .
	**  12: F4" C4" .
	** 	15: f5  g5  .  -> (f5) h5" c5"
	**  ...
	*/

	  l=0;			/* cov0 index */
	  for (i=0; i<ndiffs; i++) {
	    ms=numshells[i];
	    ms_1=ms-1;
	    nc=diffconc[i];/* conc array index */
	    dw=diffwidth[i];/* max off-diagonal width == # objects in 1 shell */
	    dw_1=dw-1;
	    if (dw_1) {			/* mixed buffer system */
	    /* BANDED MATRIX ELIMINATION */
		/* fill and forward eliminate covals and result array */
		sw=dw*dw;	/* number of coval entries for one shell */
		sw_1=sw-1;
		nb0=fbufval[i];	/* buffer rate constants index */
		nd=numshfix[i];	/* number of fixed buffers */
		nr=0;		/* rows index, result index */
		conr0=-1;     	/* rows index preceding first shell entry */
		nv=0;		/* points to first off-diagonal*/
		conv=sw-dw;	/* index first shell entry in covals */
		conv0=conv-sw;  /* index preceding first shell entry in covals*/
		nco=nc+dw_1;	/* index to conc first shell */
		co=conc[nco];	/* first shell conc: [Ca]i,t */
		/* bufvals[nb]=dt*f/2, bufvals[nb+1]=dt*b/2, bufvals[nb+2]=dt*b*Btot */

		/* FORWARD ELIMINATION */
		for (m=0; m<ms; m++) {
		    diagc=codiag[nco];	/* b5: diagonal for the shell row */
#ifndef CHANMODE4
		    codiag[nco]=1.0;	/* clear diagonal for next step */
#endif
		    resc=coresult[nco];	/* r5: result for the shell row */
		    /* fill and eliminate the fixbuffer rows: 
		    **  no lower diagonal elements -> only eliminate diagonal */
		    for (n=0,nb=nb0; n<nd; n++,nb+=3) {
			/* compute B3=dt.b/2+dt.f/2.Ca, store also as f5, co=[Ca]i,t */
			covals[conv+n]=diag=bufvals[nb]*co+bufvals[nb+1]; 
			/* compute F3=dt.b/2.Bf and add it to b5 */
			diagc+=temp=bufvals[nb]*conc[nc];	/* conc[nc]=[Bf]i,t */
			/* eliminate B3 from F3 */
			diag=1.0/(1.0+diag);
			covals[nv]=temp*diag;
			/* compute R3=Bf+dt.b.Btot - dt.b/2.Bf and eliminate B3 */
			resc+=temp2=-bufvals[nb+1]*conc[nc]+cov0[l++];
			result[nr++]=(conc[nc++]+temp2)*diag;	/* conc[nc]=[Bf]i,t */
			nv+=dw;
		    }
		    /* fill and eliminate the difbuffer rows: 
		    **  one lower diagonal element A4, but in eliminating this 
		    **  we create another lower diagonal element H4=-A4*F1".
		    **  Including B4, three consecutive eliminations are needed!
		    */
		    for ( ; n<dw_1; n++,nb+=3) {
			/* compute B4=dt.b/2+dt.f/2.Ca, store also as g5 */
			covals[conv+n]=diag=bufvals[nb]*co+bufvals[nb+1]; /* co=[Ca]i,t */
			/* compute F4=dt.b/2.Bf and add it to b5 */
			diagc+=temp=bufvals[nb]*conc[nc];	/* conc[nc]=[Bf]i,t */
			resc+=temp2=-bufvals[nb+1]*conc[nc]+bufvals[nb+2];	/* result */
			if (m) {	/* not first shell */
			    newel=cov0[l]*covals[nv-sw];	/* H4=-A4*F1" */
			    if (m<ms_1) {		/* not last shell */
			    /* add 1+dt(Dup+Ddo) to diag and eliminate A4 and H4
			    **  from diagonal: B4"=B4-A4*C1"-H4*h2"
			    **  covals[nv-sw]=F1", covals[nv-sw_1]=C1"
			    **  covals[conv0+n]=h2"
			    **  cov0[l]=-A4=dt.Ddo, cov0[l+1]=dt.Dup */
				diag+=1.0+cov0[l+1]+
					cov0[l]*(1.0+covals[nv-sw_1])-
					    newel*covals[conv0+n];
				/* eliminate B4 from C4
				** cov0[l+1]=-C4=dt.Dup */
				covals[nv+1]=-cov0[l+1]/diag;
				/* compute R4= dt.b.Btot - dt.b/2.Bf +dt.Dup.BfU
				**          + [1 -dt(Dup+Ddo)]Bf + dt.Ddo.BfD */
				temp2+=cov0[l]*conc[nc-dw]+
					(1.0-cov0[l]-cov0[l+1])*
					    conc[nc]+cov0[l+1]*conc[nc+dw];
			    } else  {		/* last shell */
				diag+=1.0+cov0[l]*(1.0+covals[nv-sw_1])
					    -newel*covals[conv0+n];
				    temp2+=cov0[l]*conc[nc-dw]+(1.0-cov0[l])*conc[nc];
			    }
			    /* eliminate H4 and B4 from F4: F4"=(F4-H4*c2")/B4" 
			    **  covals[conv0+sw_1]=c2" */
			    covals[nv]=(temp-newel*covals[conv0+sw_1])/diag;
			    /* eliminate H4 and B4 from R4: 
			    **   R4"=(R4-A4*R1"-H4*r2")/B4"
			    **   result[nr-dw]=R1", result[conr0]=r2" */
			    result[nr]=(temp2+cov0[l]*result[nr-dw]-
						newel*result[conr0]) / diag;
			    if (m<ms_1) {		/* not last shell */
				l+=2;
			    } else {
				l++;
			    }
			} else {			/* first shell */
			    diag+=1.0+cov0[l];
			    covals[nv+1]=-cov0[l]/diag;
			    result[nr]=(temp2+(1.0-cov0[l])*conc[nc] +
					    cov0[l]*conc[nc+dw]) / diag;
			    l++;
			}
			covals[nv]=temp/diag;
			nr++;
			nc++;
			nv+=dw;
		    }
		    /* fill and eliminate the shell rows: 
		    **  many lower diagonal elements: a5 for diffusion and one 
		    **  for each buffer (dw_1: f5 & g5).  In eliminating this we
		    **  create another upper diagonal element h5=-g5*C4" for 
		    **  each diffusible buffer */
		    if (m) {	/* not first shell */
			if (m<ms_1) {		/* not last shell */
			    /* add 1+dt(Dup+Ddo) to diag and eliminate a5
			    **  from diagonal: b5'=b5-a5*c2"
			    **  covals[conv0+dw_1]=c2" 
			    **  cov0[l]=-a5=dt.Ddo, cov0[l+1]=-c5=dt.Dup */
			    diagc+=cov0[l+1]+cov0[l]*(1.0+covals[conv0+dw_1]);
			    /* compute r5= dt.b.Btot - dt.b/2.Bf + dt.Dup.CaU
			    **              + [1 -dt(Dup+Ddo)]Ca + dt.Ddo.CaD
			    ** and eliminate a5: r5'=r5-a5*r2" */
			    resc+=cov0[l]*(conc[nc-dw]+result[conr0])+
					    (1.0-cov0[l]-cov0[l+1])*co;
			    nco+=dw;
			    co=conc[nco];
			    resc+=cov0[l+1]*co;
			} else {			/* last shell */
				diagc+=cov0[l]*(1.0+covals[conv0+dw_1]);
				resc+=cov0[l]*(conc[nc-dw]+result[conr0])+
						(1.0-cov0[l])*co;
			}
		    } else {            	/* first shell */
			diagc+=cov0[l];
			resc+=(1.0-cov0[l])*co;
			nco+=dw;
			co=conc[nco];
			resc+=cov0[l]*co;
		    }
		    /* eliminate each of the buffer factors f5, g5...:
		    **  first fixbuffers: straightforward elimination */
		    for (n=0,nb=conv0; n<nd; n++) {
			nb+=dw;
			conr0++;
			/* eliminate f5: b5~=b5'-f5*F3"
			**  covals[nv+n]=f5, covals[nb]=F3" */
			diagc-=covals[nv+n]*covals[nb];
			/* eliminate f5: r5~=r5'-f5*R3"
			**  covals[nv+n]=f5, result[conr0]=R3" */
			resc-=covals[nv+n]*result[conr0];
		    }
		    /* eliminate difbuffers: more complex as the h2 term
		    **  was subtracted during the a5 elimination.  Additionally,
		    **  these elements create an upper diagonal element h5 */
		    for ( ; n<dw_1; n++) {
			nb+=dw;
			conr0++;
			/* eliminate g5: b5"=b5'-(g5-a5*h2")*F4"
			**  covals[nv+n]=g5, covals[nb]=F4", covals[conv0+n]=h2"
			**  cov0[l]=-a5=dt.Ddo */
			if (m) {	/* not first shell */
			    newel=covals[nv+n]+cov0[l]*covals[conv0+n];	/* g5-a5*h2" */
			} else {	/* first shell: no a5*h2" present */
			    newel=covals[nv+n];
			}
			diagc-=newel*covals[nb];
			/* eliminate g5: r5"=r5'-(g5-a5*h2")*R4"
			**   result[conr0]=R4" */
			resc-=newel*result[conr0];
			/* make h5 diag: h5"=-g5*C4" 
			**  covals[nv+n]=g5, covals[nb+1]=C4" */
			if (m<ms_1) covals[nv+n]*=-covals[nb+1];
		    }
		    /* diagonal is now complete */
		    if (m<ms_1) {		/* not last shell */
			/* eliminate diagonal: first from the upper
			**  diagonal element h5 caused by difbuffers */
			for (n=nd; n<dw_1; n++) {
			    /* eliminate diag: h5"=h5/b5" 
			    **  covals[nv+n]=h5 */
			    covals[nv+n]/=diagc;
			}
			/* eliminate diagonal from c5: c5"=c5/b5" 
			**  cov0[l+1]=-c5=dt.Dup */
			if (m) {
			    covals[nv+dw_1]=-cov0[l+1]/diagc;
			    l+=2;
			} else {
			    covals[nv+dw_1]=-cov0[l]/diagc;
			    l++;
			}
			/* eliminate diagonal from r5: r5"=r5/b5" */
			result[nr]=resc/diagc;
			conr0=nr++;
			nc++;
			conv0=nv;
			conv+=sw;
			nv+=dw;
		    } else {
			    co=conc[nc--]=resc/diagc;	
			    l++;
		    }
		}
		/* BACKWARD SUBSTITUTION */
		for (m=ms_1; m>=0; m--) {
		    if (m<ms_1) {		/* not last shell */
			/* compute shell concentration */
			/* Ca5'=r5"-c5"*Ca8 */
			nv-=dw;
			co=result[--nr]-covals[nv+dw_1]*conc[nc+dw];
			for (n=nd; n<dw_1; n++) {
			    /* h5 column: Ca5=Ca5'-h5"*Bf7 */
			    co-=covals[nv+n]*conc[nc+n+1];
			}
			conc[nc--]=co;
			for (n=dw_1; n>nd; n--) {
			/* compute diffusible buffer concentration 
			**  Bf4=R4"-F4"*Ca5-C4"*Bf9 */
			    nv-=dw;
			    conc[nc--]=result[--nr]-covals[nv]*co
						-covals[nv+1]*conc[nc+dw];
			}
		    } else { 			/* last shell */
			for (n=dw_1; n>nd; n--) {
			/* compute diffusible buffer concentration 
			**  Bf4=R4"-F4"*Ca5 */
			    nv-=dw;
			    conc[nc--]=result[--nr]-covals[nv]*co;
			}
		    }
		    for ( ; n>0; n--) {
		    /* compute fixed buffer concentration 
		    **  Bf3=R3"-F3"*Ca5 */
			nv-=dw;
			conc[nc--]=result[--nr]-covals[nv]*co;
		    }
		}
	    } else {
		/* SIMPLE TRIDIAGONAL ELIMINATION */
		/* modified tridag out of Numerical Recipes */
		temp2=cov0[l++];
		diag=temp2+codiag[nc];	/* :0 */
#ifndef CHANMODE4
		codiag[nc]=1.0;		/* clear diagonal for next step */
#endif
		result[0]=(conc[nc]+coresult[nc])/diag; /* r0"=r0/b0 */
		/* forward elimination */
		for (nv=1; nv<ms_1; nv++) {
		    newel=covals[nv]=-temp2/diag;  /* c[j-1]"=c[j-1]/b[j-1] */
		    temp=cov0[l];
		    temp2=cov0[l+1];
		    l+=2;
		    diag=codiag[++nc]+temp*(1.0+newel)+temp2;	/* b[j]  a[j] */
#ifndef CHANMODE4
		    codiag[nc]=1.0;	/* clear diagonal for next step */
#endif
		    result[nv]=(conc[nc]+coresult[nc]+temp*result[nv-1])/diag;
		    /* slower
		    diag=codiag[++nc]+temp*(1.0+covals[nv])+temp2;
		    res=result[nv]=(conc[nc]+coresult[nc]+temp*res)/diag;
		    */
		}
		newel=covals[nv]=-temp2/diag;	/* c[j-1]"=c[j-1]/b[j-1] */
		temp=cov0[l++];
		diag=codiag[++nc]+temp*(1.0+newel);	/* b[j]  a[j] */
#ifndef CHANMODE4
		codiag[nc]=1.0;				/* clear diagonal for next step */
#endif
		res=(conc[nc]+coresult[nc]+temp*result[nv-1])/diag;
		/* backward subsitution */
		if (method==CRANK_INT) {
		    conc[nc--]=res+res-conc[nc];
		    for (nv--; nv>=0; nv--,nc--) {
			res=result[nv]-covals[nv+1]*res;
			conc[nc]=res+res-conc[nc];
		    }
		} else {	/* BEULER */
		    conc[nc--]=res;
		    for (nv--; nv>=0; nv--,nc--) {
			res=conc[nc]=result[nv]-covals[nv+1]*res;
		    }
		}
	    }
	}
}

