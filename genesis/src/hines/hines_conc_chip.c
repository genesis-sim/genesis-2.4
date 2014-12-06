/* $Id: hines_conc_chip.c,v 1.3 2005/07/20 20:02:00 svitak Exp $
**
** This files is included in several other C files so the RCS Id is
** in a comment rather than compiled in as a static variable.
*/

/* Version EDS22h 99/03/12, Erik De Schutter, Caltech & BBF-UIA 8/92-3/99 */

/*
** $Log: hines_conc_chip.c,v $
** Revision 1.3  2005/07/20 20:02:00  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  1999/10/17 23:15:40  mhucka
** Attempted to merge in changes from Greg Hood for fixing problems on
** the Cray T3E.  The same changes also make it possible to compile with
** optimization turned on under Red Hat 6.0's default version of gcc/egcs.
**
** Revision 1.5  1999/10/17 21:39:36  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22h revison: EDS BBF-UIA 99/03/10-99/03/12
 * Corrected concpool code, improved hillpump code 
 *
 * EDS22f revison: EDS BBF-UIA 98/10/30-98/11/27
 * Improved ctaupump code
 *
 * EDS22d revison: EDS BBF-UIA 98/05/05-98/05/05
 * Added support for single shells
 *
 * EDS22b revison: EDS BBF-UIA 97/11/15-97/11/20
 * Added concpool and hillpump code, reorganized some arrays
 *
** Revision 1.2  1997/05/29 18:37:01  dhb
** Moved RCS Id tag into a comment
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/*  Contains the routine which updates the concchip array (Ca_concen,
**   flux and pumps). */

#include <math.h>
#include "hines_ext.h"

#ifdef CHANMODE4
int do_h4_conc_chip_update(hsolve)
#else
int do_h2_conc_chip_update(hsolve)
#endif
	Hsolve	*hsolve;
{
	register int	k,index = 0;
	register double	c,result = 0.0,X,itau;
	register double *chip=hsolve->concchip;
	register double *coresult=hsolve->coresult;
	register double *codiag=hsolve->codiag;
	register int    *op=hsolve->concops;
	register double	*conc=hsolve->conc,*concin;
	register double	*flux=hsolve->flux,*influx;
	register double dt=hsolve->dt;
#ifdef CHANMODE4
	register double *givals=hsolve->ovals;
#endif
	double	 *vm=hsolve->vm;

	while (1) {
	    switch (*op++) {
		case CONC_OP:
		    /* store results */
		    coresult[index]=result;
		    /* NO BREAK */

		case FCONC_OP:
		    index=*op++;
		    result=0.0;
		    break;

		case FLUXCONC_OP:
		    /* get and scale flux */
		    /* chip[n]==dt.leak, chip[n+1]==dt/zFV */
		    influx=flux+index;
		    result+=*chip +  *influx * *(chip+1);
#ifdef CHANMODE4
#else
		    *influx=0.0;
#endif
		    chip+=2;
		    break;

		case CTAUPUMP_OP:
		    /* taupump */
		    result+=*chip++;	/* chip[n]==Ceq*dt[/2]/T_C */
		    codiag[index]+=*chip++;	/* chip[n]==dt[/2]/T_C */
		    break;

		case TAUPUMP_OP:
		    /* taupump */
		    /* chip: T_C, T_A, T_V, 1/T_B */
#ifdef CHANMODE4
		    *givals++=itau=dt /(*chip + *(chip+1) *
				(exp((*(vm + *op++) - *(chip+2)) * *(chip+3))));
#else
		    itau=dt / (*chip + *(chip+1) * (exp((*(vm + *op++) 
				- *(chip+2)) * *(chip+3))));
#endif
		    chip+=4;
		    result+=*chip++ * itau;	/* chip[n]==Ceq */
		    codiag[index]+=itau;
		    break;

		case MMPUMP_OP:
		    /* Michaelis Menten pump: electrogenic ones are 
		    **  computed in do_chip_hh2_update */
		    /* chip[n]==dt[/2].vmax.surf/vol, chip[n+1]==Kd */
		    codiag[index]+=*chip/(conc[index] + *(chip+1));
		    chip+=2;
		    break;

		case HILLPUMP_OP:
		    /* Michaelis Menten pump with Hill coefficient */
		    X=conc[index];
		    k=*op++;	/* Hill coeff */
		    if (k==2) {
			c=X;
		    } else if (k==3) {
			c=X*X;
		    } else if (k==4) {
			c=X*X*X;
		    } else  {  /* k==1 */
			c=1.0;
		    }
		    c= c /(c*X + *chip++);	/* chip[n]==Kd */
		    /* this is a true hack: linear approximation... */
		    /* chip[n]==dt[/2].vmax.surf/vol_shell */
		    codiag[index]+=*chip++ * c;
		    break;

		case PHILLPUMP_OP:
		    /* Michaelis Menten pump with Hill coefficient: influx
		    ** into a pool */
		    k=*op++;	/* Hill coeff */
		    X=conc[*op];
		    if (k==2) {
			c=X;
		    } else if (k==3) {
			c=X*X;
		    } else if (k==4) {
			c=X*X*X;
		    } else  {  /* k==1 */
			c=1.0;
		    }
		    c= c /(c*X + *chip++);	/* chip[n]==Kd */
		    /* this is a true hack: linear approximation... */
		    /* chip[n]==dt[/2].vmax.surf/vol_shell */
		    codiag[*op++]+=*chip++ * c;
		    /* influx into concpool: chip[n]==dt.vmax.surf/vol_pool */
		    result+=*chip++ * c*X;	/* add to concpool */
		    break;

		case LCONC_OP:
		    /* store results and finish */
		    coresult[index]=result;
		    return(0);	/* 0 added by Greg Hood, PSC, 9/14/98 */

		case BUFFER_OP:
		    /* update and integrate pool buffer: trapezoidal method */
		    concin=conc + *op++;	/* Bfree concentration */
		    X=*chip++;		/* b.dt/2 */
			/* chip[n]= dt.b.Btot;  chip[n+1]= f.dt */
		    /* compute Bfree at t + dt */
		    c=((1.0 - X) * *concin + *chip) / 
				    (1.0 + X + *(chip+1) * conc[index]);
		    result+=c - *concin;	/* add dB to concpool */
		    *concin=c;			/* update BFree */
		    chip+=2;
		    break;

		case POOL_OP:
		    /* compute changes to pool */
		    conc[index]+=result;
		    break;

		case CACONC_OP:
		    /* compute Ca_concen: use op instead of index so that we
		    **  can mix SHELLs and CACONCs freely */
		    concin=conc + *op;
		    influx=flux + *op++;
		    X=*chip++;      /* Ca_base */
		    itau=*chip++;      /* tau */
		    *concin=X+((*concin-X)*(2.0-itau)+(*influx * *chip++))/itau;
#ifdef CHANMODE4
#else
		    *influx=0.0;
#endif
		    break;

		case LCACONC_OP:
		    /* finish */
		    return(0);	/* 0 added by Greg Hood, PSC, 9/14/98 */

		default:
		    Error();
		    printf("Error hsolve: undefined concchip operation: %d %d %d\n",*(op-2),*(op-1),*op);
		    return(ERR);
	    }
	}
    return(0);
}

