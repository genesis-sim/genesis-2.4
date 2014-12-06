/* $Id: hines_chip.c,v 1.2 2005/07/01 10:03:04 svitak Exp $
**
** This files is included in several other C files so the RCS Id is
** in a comment rather than compiled in as a static variable.
*/

/* Version EDS22g 99/02/11, Erik De Schutter, Caltech & BBF-UIA 4/92-2/99 */

/*
** $Log: hines_chip.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/04/01 19:26:35  mhucka
** Correction from Hugo to the order of when certain calculations are
** made in do_chip_hh2_update().
**
** Revision 1.7  2000/09/07 01:49:28  mhucka
** Fix from Hugo Cornelis: added initialization of some variables in
** do_chip_hh4_update().
**
** Revision 1.6  1999/10/17 23:15:40  mhucka
** Attempted to merge in changes from Greg Hood for fixing problems on
** the Cray T3E.  The same changes also make it possible to compile with
** optimization turned on under Red Hat 6.0's default version of gcc/egcs.
**
** EDS22g revison: EDS BBF-UIA 99/02/11-99/02/11
** Added chanmode 5
**
** EDS22d revison: EDS BBF-UIA 98/05/04-98/06/22
** Made instant gates work.
** Corrected GHK
**
** EDS22b revison: EDS BBF-UIA 97/11/24-97/11/24
** Corrected bug in calcmode 0 code for TABGHK_T
**
** Revision 1.2  1997/05/29 18:27:05  dhb
** Moved RCS Id tag into a comment
**
** Revision 1.1  1997/05/29 18:03:31  dhb
** Initial revision
**
*/

/* Contains the first loops executed when chanmode>1:
** - a loop which updates the chip array (do_chip_hh_update): evaluate all
**    voltage-dependent processes and compute parameters for diagonal and
**    right side of eq. 13.18 of Mascagni.
** Second loop is in hines_solve.c
** Arrays: ravals array (derived from values) contains absolutely constant
**   values from the left side of the equation (i.e. the lambdas from eq.
**   13.18 of Mascagni).  results array contains two entries for each 
**   compartment: first the right hand side of equation 13.18, then the
**   diagonal part (1+2lambda+gamma).  Because only gamma changes (depends 
**   on ionic channel conductance, we compute gamma in do_chip_hh_update and 
**   add it to the rest (which is stored in the chip array).  chip array
**   also contains Gbar and Ek values for all channels and parameters for
**   nernst and ghk elements.  ops array contains operation codes and 
**   indexes to other arrays for do_chip_hh_update.  funcs contains
**   operation codes and indexes to results array for do_crank_hsolve.
*/

/* This is the fastest version, but uses a lot of memory and 
** makes several assumptions :
** All powers are integers
** All tables have the same upper and lower limits and the same size */
/* One can speed up things even more by commenting out the cases that
** are not used in the type of simulations you run (the size of this loop
** determines whether it fits into the progam cache memory */

#ifdef CHANMODE4
/* chanmode 4 version */
#ifdef SOLVEINTERPOL
int do_chip_hh4_update(hsolve)
#else
int do_chip_hh4ni_update(hsolve)
#endif
#else
/* chanmode 2/3 version */
#ifdef SOLVEINTERPOL
int do_chip_hh2_update(hsolve)
#else
int do_chip_hh2noi_update(hsolve)
#endif
#endif
	Hsolve	*hsolve;
{
	register int *op=hsolve->ops;
	register int k = 0,cfilo = 0,ncols = 0;		/* k is a protected variable */
	register double *chip=hsolve->chip;
	register double	Vm,sumgchan,ichan,Gk = 0,X,Y;
#ifdef SOLVEINTERPOL
	register double   vipol = 0.0,cipol = 0.0;
	register int filo = 0;
	double	ipol = 0.0;
#endif
	register double	*results=hsolve->results;
	register double *vm=hsolve->vm;
	register double *conc = NULL,*flux = NULL;
	register double	concen,Ek = 0.0,GHK = 0.0;
	register double	*tab = NULL,*tabn,*stablist;
 	double	*vtab = NULL;
 	double	*ctab = NULL;
	register double	*tablist = NULL,*concin = NULL,*fluxindex = NULL;
	int	xdivs = 0,ydivs = 0,doGHK=0;
	double	invdx = 0.0,invdy = 0.0,xmin = 0.0,ymin = 0.0,xlo,xhi,ylo = 0.0,yhi = 0.0,Ik = 0.0;
	double  *xvals = NULL,*yvals = NULL,*concout,*codiag = NULL;
#ifdef CHANMODE4
	double  *itotal = NULL;
	register double *givals=hsolve->givals+1;
	double	Im = 0.0,gmax = 0.0;
	int	chanmode5=(hsolve->chanmode==5);
	int	storeall=hsolve->storemode;
#endif

	if (hsolve->tablist) {
	    tablist=hsolve->tablist;
	    ncols=hsolve->ncols;
	    invdx=hsolve->invdx;
	    xdivs=hsolve->xdivs;
	    xmin=hsolve->xmin;
	    xvals=hsolve->xvals;
	    xlo=xhi=0.0;
	} else {	/* passive model: ensure that ilo is never computed */
	    xlo=-1e20;
	    xhi=1e20;
	}
	stablist=hsolve->stablist;
#ifdef CHANMODE4
	if (hsolve->storemode) {
	    itotal=hsolve->itotal;
	    for (k=0; k<hsolve->nelm_names; k++) itotal[k]=0.0;
	}
#endif
	if (hsolve->nconcs) {
	    conc=hsolve->conc;
	    flux=hsolve->flux;
	    yvals=hsolve->yvals;
	    invdy=hsolve->invdy;
	    ydivs=hsolve->ydivs;
	    ymin=hsolve->ymin;
	    ylo=yhi=0.0;
	    codiag=hsolve->codiag;
#ifdef CHANMODE4
	    /* reset codiag: quite an expensive operation, as most of codiag is
	    ** never used */
	    /* Debugging only
	    for (k=0; k<hsolve->nconcs; k++) codiag[k]=1.0;
	    */
#endif
	}
	/* loop through all ops, compartment by compartment */
	while (*op++ != LCOMPT_OP) {
	/* start new comp */
	    Vm=*vm++;
	    sumgchan=0.0;
	    ichan=*chip + *(chip+1); /* chip[n]==Em/Rm,chip[n+1]==inject */
#ifdef CHANMODE4
	    /* compute leak current */
	    Im=*chip - Vm / *givals++;	/* Em/Rm - Vm/Rm */
	    *givals++=Im;	/* leak current */
	    if (storeall==1) itotal[0]+=Im;         
#endif
	    chip+=2;

	    /* Split into two switch statements -> faster */
	    /* loop through non-conductance ops for this compartment */
	    while (*op>LCOMPT_OP) {
		switch (*op++) {
		    case NEWVOLT_OP:
		    /* find position in voltage-dependent table */
#ifdef SOLVEINTERPOL
			vipol=(Vm-xmin)*invdx;
			if ((xlo>Vm)||(Vm>=xhi)) {
			    if (vipol<0.0)
				vipol=0.0;
			    else if (vipol>xdivs)
				vipol=xdivs;
			    filo=vipol;
			    vipol=vipol - (double)filo;
			    xlo=xvals[filo];
			    xhi=xvals[filo+1];
			    vtab=tablist + filo*ncols;
			} else {
			    vipol=vipol - (double)filo;
			}
#else
			if ((xlo>Vm)||(Vm>=xhi)) {
			    k=(Vm-xmin)*invdx;
			    if (k<0)
				k=0;
			    else if (k>xdivs)
				k=xdivs;
			    xlo=xvals[k];
			    xhi=xvals[k+1];
			    vtab=tablist + k*ncols;
			}
#endif
			continue;	/* go back to this switch statement */

		    case CONC_VAL_OP:
		    /* get concentration and pointers to flux */
			k=*op++;    /* we need this unchanged in ELMMPUMP_OP */
			concin=conc + k;		/* Cin */
			fluxindex=flux + k;
#ifdef CHANMODE4
			*fluxindex=0.0;
#endif
			continue;	/* go back to this switch statement */

		    case DOMAIN_OP:
		    /* compute domain concentration */
#ifdef CHANMODE4
			concen=*chip++ * Ik;
#else
			concen=*chip++ * Gk * (Ek - Vm);
#endif
			goto CINTERPOL;

		    case NEWCONC2_OP:
		    /* initialize for dual concentration-dependent table:
		    **  CONC2 uses the 'voltage' positions */
			concen=*(conc + *op++);	/* get value out conc array */
			tab=ctab;		/* store the C1 data first */
#ifdef SOLVEINTERPOL
			ipol=cipol;
#endif
			goto CINTERPOL;

		    case NEWCONC1_OP:
		    /* find position in concentration-dependent table */
			concen=*concin;	/* get value out of conc array */
#ifdef SOLVEINTERPOL
CINTERPOL:		cipol=(concen-ymin)*invdy;
			if ((ylo>concen)||(concen>=yhi)) {
			    if (cipol<0.0)
				cipol=0.0;
			    else if(cipol>ydivs)
				cipol=ydivs;
			    cfilo=cipol;
			    cipol=cipol - (double)cfilo;
			    ylo=yvals[cfilo];
			    yhi=yvals[cfilo+1];
			    ctab=tablist + cfilo*ncols;
			} else {
			    cipol=cipol-cfilo;
			}
#else
CINTERPOL:		if ((ylo>concen)||(concen>=yhi)) {
			    cfilo=(concen-ymin)*invdy;
			    if (cfilo<0)
				    cfilo=0;
			    else if(cfilo>ydivs)
				cfilo=ydivs;
			    ylo=yvals[cfilo];
			    yhi=yvals[cfilo+1];
			    ctab=tablist + cfilo*ncols;
			}
#endif
			continue;	/* go back to this switch statement */

		    case SPIKE_OP:
		    /* check for spike triggering */
			*op-=1;		/* decrement refractory period */
			if (Vm>*chip++) {  /* chip[n]=thresh */
			    if (*op<=0) {        /* check refractory period */
				h_dospike_event(hsolve);
				*op=*(op+1);    /* reset refractory period */
			    }
			}
			op+=2;
			continue;	/* go back to this switch statement */

		    case ELMMPUMP_OP:
		    /* electrogenic Michaelis Menten pump */
		    /* depends on k being set correctly in CONC_VAL_OP! */
			/* chip[n]==Kd */
			X=1.0/(*concin + *chip++);
			/* chip[n]==vmax.surf.val.F */
#ifdef CHANMODE4
/*
k=(chip-hsolve->chip)/8;
if (k>90) printf("ELMMPUMP_OP: %d %.20g %.20g %.20g\n",k,*(chip-1),*chip,*(chip+1));
*/
			ichan+=Ik=*chip * *concin * X;
			if (chanmode5) {
			    *givals++=Ik / *chip++;	/* normalize */
			} else {
			    *givals++=Ik;
			    chip++;
			}
			Im+=Ik;
#else
			ichan+=*chip++ * *concin * X;
#endif
			/* chip[n]==dt.vmax.surf/vol */
			codiag[k]+=*chip++ * X;
			continue;	/* go back to this switch statement */

		    case FLUX_OP:
		    /* add current to total flux for this compartment */
#ifdef CHANMODE4
			*fluxindex+=Ik;
#else
			*fluxindex+=Gk * (Ek - Vm);
#endif
			continue;	/* go back to this switch statement */

#ifdef CHANMODE4
		    case STORE_ALLI_OP:
		    /* add current to total for neuron */
			itotal[*op++]+=Ik;         
			continue;	/* go back to this switch statement */

		     case STORE_ALLC_OP:
		    /* add conductance to total for neuron */
			itotal[*op++]+=Gk;
			continue;	/* go back to this switch statement */

#endif
		    case SCALE_FLUX_OP:
		    /* add current to flux */
#ifdef CHANMODE4
			*fluxindex+=*chip++ * Ik;
#else
			*fluxindex+=*chip++ * Gk * (Ek - Vm);
#endif
			continue;	/* go back to this switch statement */

		    case GHK_OP:
		    /* compute Goldman Hodgkin Katz field equation using 
		    ** the slope conductance */
			concout=concin+1;		/* Cout */
			GHK=*chip++;	 /* chip[n] == zF/RT */
			X=Vm*GHK;	/* zVF/RT */
			Y=exp(-X);	/* exp(-zVF/RT) */
			Ek=*chip++;	/* not true Ek, chip[n]==zF*/
			Ik=Ek * (*concin - (Y * *concout)); 
			if (fabs(X)<TINY) {
			    Gk=1.0 - 0.5*X;	/* not true Gk */
			    /* the GHK slope conductance */
			    GHK=0.5*GHK*Ek*(*concin + *concout);
			    Ik=-Ik/Gk;
			} else {
			    Gk=1.0-Y;	/* not true Gk */
			    /* the GHK slope conductance */
			    GHK=GHK*(Ik/Gk - Ek*GHK *
					     (*concin - *concout)*Vm*Y/(Gk*Gk));
			    Ik=-X*Ik/Gk;	/* the GHK Ik */
			}
			Ek=(Ik + GHK*Vm)/GHK;	/* Ek=(Ik + Gk*Vm)/Gk is independent of permeability */
			/* no break! */

		    case SAME_GHK_OP:
#ifdef CHANMODE4
			gmax=*chip++;
			Gk=gmax * GHK;	/* gmax * slope conductance */
#else
			Gk=*chip++ * GHK;	/* gmax * slope conductance */
#endif
			break;	/* go to next switch: start computing current */

		    case DO_TABGHK_OP:
			doGHK=1;
			break;	/* go to next switch: start computing current */

		    case NERNST_OP:
		    /* compute nernst reversal potential */
			Ek=log(*(concin+1) / *concin) * *chip++; /* chip[n]==constant */
#ifdef CHANMODE4
			gmax=Gk=*chip++;
#else
			Gk=*chip++;
#endif
			break;	/* go to next switch: start computing current */

		    case CHAN_EK_OP: 
		    /* initialize channel with fixed Ek */
			Ek=*chip++;
			/* no break! */

		    case CHAN_OP: 
		    /* initialize channel with variable Ek */
#ifdef CHANMODE4
			gmax=Gk=*chip++;
#else
			Gk=*chip++;
#endif
			break;	/* go to next switch: start computing current */

		    default:
			Error();
			printf("Error hsolve: undefined chip operation #1: %d %d %d\n",*(op-2),*(op-1),*op);
			return(ERR);
		  }	/* end switch */

	      /* loop continuously through conductance ops till current is
	      **  computed */
	      while (1) {
		k=*op++;
/*
printf("Entering while %d %d %d: %g\n",*(op-1),k,*(op+1),*chip);
*/
		/* we need to interpolate a table, first we compute the
		** index(es), then we interpolate */
		/* codes:
		** interpolation: ipol, tab -> 1D table
		**                ipol (X), cipol (Y), cfilo, tab -> 2D table
		** NEWVOLT_OP: interpolate voltage table and set vipol & ipol
		** NEWCONC1_OP: interpolate conc table based on concin, set
		**             set cipol, cfilo
		** CONC2_OP: same, but overwrites vipol & ipol first
		** DOMAIN_OP: same, converts inflow into domain conc
		*/
		if (k==IPOL1V_OP) {
		/* compute a voltage dependent gate from 1D table */
		    tab =  vtab + *op++;
#ifdef SOLVEINTERPOL
		    /* interpolate B-table or G_tab */
		    X=*tab;
		    tabn = tab + ncols;
		    X=X+vipol*(*tabn-X);
		    /* interpolate A-table or E_tab */
		    tab++;
		    tabn++;
		    Y=*tab;
		    Y=Y+vipol*(*tabn-Y);
		    goto DOPOWER;
#else
		    goto DOROUNDOFF;
#endif
		} else if (k==IPOL1C_OP) {
		/* compute a conc1 dependent gate from 1D table */
		    tab = ctab + *op++;
#ifdef SOLVEINTERPOL
		    /* interpolate B-table or G_tab */
		    X=*tab;
		    tabn = tab + ncols;
		    X=X+cipol*(*tabn-X);
		    /* interpolate A-table or E_tab */
		    tab++;
		    tabn++;
		    Y=*tab;
		    Y=Y+cipol*(*tabn-Y);
		    goto DOPOWER;
#else
		    goto DOROUNDOFF;
#endif
		} else if (k==IPOL2V_OP) {
		/* compute a voltage/conc1 dependent gate from 2D table */
		    tab =  vtab + cfilo + cfilo + *op++;
#ifdef SOLVEINTERPOL
		    /* interpolate 2D B-table or G_tab */
		    tabn = tab + ncols;
		    X=(1.0-vipol)*((1.0-cipol)*(*tab)+cipol*(*(tab+2))) +
		      vipol*((1.0-cipol)*(*tabn)+cipol*(*(tabn+2)));
		    /* interpolate 2D A-table or E_tab */
		    tab++;
		    tabn++;
		    Y=(1.0-vipol)*((1.0-cipol)*(*tab)+cipol*(*(tab+2))) +
		      vipol*((1.0-cipol)*(*tabn)+cipol*(*(tabn+2)));
DOPOWER:	    k=*op++;    /* exponent */
#else
DOROUNDOFF:	    X=*tab++;
		    Y=*tab;
		    k=*op++;    /* exponent */
#endif
		    if (k==0) { /* a tabcurrent, maybe a GHK as a tabcurrent */
			Ek=Y;
			if (doGHK) {	/* TABGHK_T */  
			    GHK=X;	/* slope conductance */
			    doGHK=0;
			    break;  /* while loop: no current computed */
			} else {	/* TABCURR_T */
			    Gk*=X;
			    goto DOADDCURR;
			}
		    } else {
			if (k>0) {	
			    X=*chip=(*chip*(2.0-X)+Y)/X;
			} else {	/* instantaneous gate */
			    X=*chip=Y/X;
			    k=-k;
			}
			chip++;
			if (k==1) {
			    Gk*=X;
			} else if (k==2) {
			    Gk*=X*X;
			} else if (k==3) {
			    Gk*=X*X*X;
			} else if (k==4) {
			    X*=X;
			    Gk*=X*X;
			} else if (k==5) {
			    Gk*=X;
			    X*=X;
			    X*=X;
			    Gk*=X;
			} else {    /* k==6 */
			    X*=X;
			    Gk*=X;
			    X*=X;
			    Gk*=X;
			}
		    }
		} else if (k>=SYN2_OP) {
		/* compute synaptic conductance */
		/* channelC3: trigger randomly */
		    if (k==SYN3_OP) {
		    /* channelC3: determine whether it fires */
			if (urandom() <= *chip++) { /* chip[n]==dt*frequency */
				*chip+=stablist[*op+3]; 
			}
		    }
		    k=*op++;		/* op[i] == pointer to table */
		    *chip *= stablist[k];	/* X*xconst2 */
		    /* synchan: check for next synaptic event */
		    if (*op==0)
			h_dosynchan(hsolve,k,*(op+1));/* synapse fires*/
		    *op-=1; /* decrement by one time step */
		    op+=2;	/* skip index to child */
		    /* channelC2: compute the conductance */
		    X = *chip;
		    chip++;			 /* X*yconst1 + Y*yconst2 */
		    Gk *= (*chip=X*stablist[k+1] + *chip*stablist[k+2]);
		    chip++;
		    if (*op==MGBLOCK_OP) {
		    /* compute Mg2+ block of synaptic conductance */
#ifdef CHANMODE4
			if (chanmode5) {	/* normalize */
			    *givals++=Gk/gmax;
			    *givals++=Gk*(Ek-Vm)/gmax;
			} else {
			    *givals++=Gk;		/* unblocked */
			    *givals++=Gk*(Ek-Vm);	/* unblocked */
			}
			*givals++=Ek;
#endif
			/* chip[n]=KMg_A  chip[n+1]=1/KMg_B */
			X=*chip *exp(Vm * *(chip+1));	
			chip+=2;
			Gk *=X / (X + *chip++);	/* chip[n]=KMg_C */
			op++;
		    }
		    goto DOADDCURR;
		} else if (k==ADD_CURR_OP) {
		/* compute the current */
DOADDCURR:	    sumgchan+=Gk;
		    ichan+=Ek*Gk;
#ifdef CHANMODE4
		    if (chanmode5) {	/* normalize */
			*givals++=Gk/gmax;
			Ik=Gk*(Ek-Vm);
			*givals++=Ik/gmax;
		    } else {
			*givals++=Gk;
			*givals++=Ik=Gk*(Ek-Vm);
		    }
		    *givals++=Ek;
		    Im+=Ik;
#endif
		    break;  /* while loop */
		} else if (k==IPOL2C_OP) {
		/* compute a conc1/conc2 dependent gate from 2D table */
		    tab +=  cfilo + cfilo + *op++;	/* tab is overwritten */
#ifdef SOLVEINTERPOL
		    /* interpolate 2D B-table or G_tab */
		    tabn = tab + ncols;
		    X=(1.0-ipol)*((1.0-cipol)*(*tab)+cipol*(*(tab+2))) +
		      ipol*((1.0-cipol)*(*tabn)+cipol*(*(tabn+2)));
		    /* interpolate 2D A-table or E_tab */
		    tab++;
		    tabn++;
		    Y=(1.0-ipol)*((1.0-cipol)*(*tab)+cipol*(*(tab+2))) +
		      ipol*((1.0-cipol)*(*tabn)+cipol*(*(tabn+2)));
		    goto DOPOWER;
#else
		    goto DOROUNDOFF;
#endif
		} else {
		    Error();
		    printf("Error hsolve: undefined chip operation #2: %d %d %d\n",*(op-2),k,*op);
		    return(ERR);
		}
	      }
	    }
	    /* store results prev comp */
	    *results++=Vm + ichan * *chip;		/* Vm + ichan * tbyc */
		    /* chip[n]=tybc, chip[n+1]=diagterm */
	    *results++= sumgchan * *chip + *(chip+1);
	    chip+=2;
#ifdef CHANMODE4
	    *givals++=Im;
#endif
	}
	return(0);		/* added by Greg Hood, PSC, 9/14/98 */
}
