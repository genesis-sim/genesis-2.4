static char rcsid[] = "$Id: hines_conc_init.c,v 1.3 2005/07/20 20:02:00 svitak Exp $";

/* Version EDS22k 00/07/05 Erik De Schutter, BBF-UIA 8/94-7/00 */

/*
** $Log: hines_conc_init.c,v $
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
** Revision 1.5  2000/09/07 02:22:55  mhucka
** Fixes from Hugo Cornelis for various initialization problems.
**
** EDS22k revision HC BBF-UIA 00/07/24
** no normalization for incoming ACTIVATION msg's on hsolve'd synchans
**
** Revision 1.4  2000/07/09 23:47:36  mhucka
** Fix from Erik De Schutter to solve a problem in calculations for diffusion.
** The bug involved the calculation of surface area of contacts of shells.
**
** EDS22k revison: EDS BBF-UIA 00/07/05
** Checks contact surfaces of shells
**
** Revision 1.3  1999/10/17 21:39:36  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
** EDS22h revison: EDS BBF-UIA 99/03/10-99/03/24
** Corrected concpool code, improved hillpump code 
**
** EDS22f revison: EDS BBF-UIA 98/10/30-98/11/27
** Added support for buffer concentration differences between shells
**
** EDS22d revison: EDS BBF-UIA 98/05/05-98/05/07
** Added support for single shells
**
** EDS22b revison: EDS BBF-UIA 97/11/15-97/11/19
** Added concpool and hillpump code, reorganized some arrays
** Improved indentation
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* initializes all arrays for concentrations */

#include <math.h>
#include "hines_ext.h"

int h_init_conc_solve(hsolve)
/* Creates and initializes fdiffs, numshells, fcoval,
** fbufval arrays, creates covals, bufvals,  coresult, 
** cores and codiag arrays.  */
	Hsolve	*hsolve;
{
	int 	ncompts=hsolve->ncompts;
	int 	nconcs=hsolve->nconcs;
	Element	**children;
	int 	i,j,mr,ms,n,nb,nr = 0,nv;
	int	nshells,nrows,nelms = 0,nfix = 0;
	int	*numshrows,*numshelms,*numshfix;
	short   *childtypes;
	int	*childstart,*childmsg,*childcode,*childpos;
	int	cm,ct;
	int	ndiffs,*firstdiffs,*numshells,*diffconc;
	int	*firstcoval,*firstbufval,*diffchild;
	double	*flux,*coresult,*codiag;
	int	concID;

	children=hsolve->children;
	childtypes=hsolve->childtypes;
	childcode=hsolve->childcode;
	childmsg=hsolve->childmsg;
	childpos=hsolve->childpos;
	childstart=hsolve->childstart;

	/* Count number of diffusion systems in each compartment 
	** A diffusion systems is defined as any set of concentrations +
	**  buffers, etc having the same concID */
	ndiffs=0;	/* count total */
	for (i=0;i<ncompts;i++){
	    n=childstart[i];
	    concID=-1;
	    for (j=n; j<childstart[i+1]; j++) {
		ct=childtypes[j];
		switch (ct) {
		    case CACONCEN_T:
			if (childcode[j]!=concID)  concID=-1;
			break;

		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
			if (childcode[j]!=concID) {
				ndiffs++;
				concID=childcode[j];
			}
			break;

		    default:
			break;
		}
	    }
	}
	/* Allocate arrays */
	hsolve->conc=(double *)calloc(nconcs,sizeof(double));
	flux=hsolve->flux=(double *)calloc(nconcs,sizeof(double));

	hsolve->ndiffs=ndiffs;
	coresult=hsolve->coresult=(double *)calloc(nconcs,sizeof(double));
	codiag=hsolve->codiag=(double *)calloc(nconcs,sizeof(double));
	hsolve->cores=(double *)calloc(nconcs,sizeof(double));
	for (i=0; i<nconcs; i++) {
	    /* a lot of these values will never be changed! */
	    flux[i]=coresult[i]=0.0;
	    codiag[i]=1.0;
	}
	if (ndiffs==0) return(0);	/* no diffusion */
	if (hsolve->readflag==HDUPLICATE_T) return(0);	/* rest is identical */

	firstdiffs=hsolve->fdiffs=(int *)calloc(ncompts+1,sizeof(int));
	firstcoval=hsolve->fcoval=(int *)calloc(ndiffs+1,sizeof(int));
	firstbufval=hsolve->fbufval=(int *)calloc(ndiffs+1,sizeof(int));
	diffchild=hsolve->dchild=(int *)calloc(ndiffs+1,sizeof(int));
	diffconc=hsolve->dconc=(int *)calloc(ndiffs+1,sizeof(int));
	numshells=hsolve->nshells=(int *)calloc(ndiffs+1,sizeof(int));
	numshrows=hsolve->nshrows=(int *)calloc(ndiffs+1,sizeof(int));
	numshelms=hsolve->nshelms=(int *)calloc(ndiffs+1,sizeof(int));
	numshfix=hsolve->nshfix=(int *)calloc(ndiffs+1,sizeof(int));
	/* Count number of shells and buffers in each diffusion system */
	mr=ms=ndiffs=0;
	for (i=0;i<ncompts;i++){
		n=childstart[i];
		firstdiffs[i]=ndiffs;
		concID=-1;
		nshells=nrows=0;	
		for (j=n; j<childstart[i+1]; j++) {
		    ct=childtypes[j];
		    cm=childmsg[j];	/* -> conc */
		    switch (ct) {
			case CACONCEN_T:
			    if (childcode[j]!=concID)  concID=-1;
			    break;

			case FIXBUFF_T:
			case BUFSHELL_T:
			case BUFSLAB_T:
			case BUFUSER_T:
			case BUF2SHELL_T:
			case BUF2SLAB_T:
			case BUF2USER_T:
			    if (BaseObject(hsolve)->method == BEULER_INT) {
				ErrorMessage("SETUP","Cannot solve buffer equations with backward Euler method.  Use Crank-Nicholson for",hsolve);
				return(ERR);
			    }

			case DIFSHELL_T:
			case DIFSLAB_T:
			case DIFUSER_T:
			    if (childcode[j]==concID) {
				switch (ct) {
				    case FIXBUFF_T:
					if (nshells==0) nfix++;
					/* no break! */

				    case BUFSHELL_T:
				    case BUFSLAB_T:
				    case BUFUSER_T:
					if (nshells==0) {
					    nrows++; /* one line in array */
					    nelms++;
					} else {
					    nr++;
					}
					break;

				    case BUF2SHELL_T:
				    case BUF2SLAB_T:
				    case BUF2USER_T:
					if (nshells==0) {
					    nrows+=2; /* two lines in array */
					    nelms++;
					} else {
					    nr+=2;
					}
					break;

				    case DIFSHELL_T:
				    case DIFSLAB_T:
				    case DIFUSER_T:
					if (nshells==0) {
					    nrows++; /* one line in array */
					    nelms++;
					} else {
					    nr++;
					}
					nshells++;
					break;
				}
			    } else {	/* new conc */
				if ((concID!=-1)&&(nshells>0)) {
				/* this is first conc new system, store old one */
				    if (nshells==0) {
					fprintf(stderr,"Error while creating %s: diffusion system without shells!\n",hsolve->name);
					return(ERR);
				    }
				    if (nr!=nrows) {
					fprintf(stderr,"Error while creating %s: %s has different number of shells than rest\n",hsolve->name,children[j]->name);
					return(ERR);
				    }
				    numshells[ndiffs]=nshells;
				    numshrows[ndiffs]=nrows;
				    numshelms[ndiffs]=nelms;
				    numshfix[ndiffs]=nfix;
				    if (nshells>ms) ms=nshells;	/* maxshells */
				    if (nrows>mr) mr=nrows;	/* maxrows */
				    ndiffs++;
				}
				diffconc[ndiffs]=cm;	/* first conc */
				diffchild[ndiffs]=j;
				concID=childcode[j];
				nelms=nr=nrows=1;
				switch (ct) {
				    case FIXBUFF_T:
					nfix=1;
					nshells=0;
					break;

				    case BUFSHELL_T:
				    case BUFSLAB_T:
				    case BUFUSER_T:
				    case BUF2SHELL_T:
				    case BUF2SLAB_T:
				    case BUF2USER_T:
					nfix=0;
					nshells=0;
					break;

				    case DIFSHELL_T:
				    case DIFSLAB_T:
				    case DIFUSER_T:
					nfix=0;
					nshells=1;
					break;
				}
			    }
			    childcode[j]=ndiffs;
			    break;

			default:
			    break;
		    }
		}	/* j loop */
		if ((concID!=-1)&&(nshells>0)) {
		/* store last one */
			if (nshells==0) {
				fprintf(stderr,"Error while creating %s: diffusion system without shells!\n",hsolve->name);
				return(ERR);
			}
			numshells[ndiffs]=nshells;
			numshrows[ndiffs]=nrows;
			numshelms[ndiffs]=nelms;
			numshfix[ndiffs]=nfix;
			if (nshells>ms) ms=nshells;	/* maxshells */
			if (nrows>mr) mr=nrows;	/* maxrows */
			ndiffs++;
		}
	}
	if (ndiffs!=hsolve->ndiffs) {
		fprintf(stderr,"Error in %s: Bug #3 in h_init_conc_solve %d.\n",hsolve->name,ndiffs);
		return(ERR);
	}
	firstdiffs[i]=ndiffs;
	/* create covals array */
	nb=nv=0;
	/* count ncovals */
	for (i=0; i<ndiffs; i++) {	
	    /* diffusion equation */
	    firstcoval[i]=nv;
	    firstbufval[i]=nb;
	    if (numshrows[i]>1) {	/* has buffers */
		nv+=numshells[i]*(2*numshrows[i]-numshfix[i])-1;
		nb+=3*(numshrows[i]-1);
	    } else {			/* pure diffusion */
		nv+=3*numshells[i]-2;
	    }
	}
	firstcoval[i]=nv;
	firstbufval[i]=nb;
	hsolve->maxshells=ms;
	hsolve->maxshcols=mr;
	mr=mr*ms;		/* max number of rows in covals array */
	hsolve->covals0=(double *)calloc(nv,sizeof(double));
	hsolve->covals=(double *)calloc(mr*mr,sizeof(double));
	hsolve->cores=(double *)calloc(mr,sizeof(double));
	hsolve->bufvals=(double *)calloc(nb,sizeof(double));
	return(0);
}

int h_setup_conc_solve(hsolve)
/* Fills covals0 arrays */
/* BUF2SHELL_T NOT IMPLEMENTED */
	Hsolve	*hsolve;
{
	int 	ndiffs=hsolve->ndiffs;
	Element	**children;
	short   *childtypes;
	int	*childcode;
	int	ct;
	float	*childdata;
	int 	i,j,k,m,ms,ne,nb,nv,nbt;
	int	*numshells,*numshelms,*diffchild,*diffwidth;
	double	Diff,btot;
	double	*cov0,*bufvals;
	double	dt,dt2;
	Dbuffer	*buffer;
	Dshell	*shell;
	int	concID;
	Action  *action;

	children=hsolve->children;
	childtypes=hsolve->childtypes;
	childcode=hsolve->childcode;
	childdata=hsolve->childdata;
	numshells=hsolve->nshells;
	numshelms=hsolve->nshelms;
	diffwidth=hsolve->nshrows;
	diffchild=hsolve->dchild;
	cov0=hsolve->covals0;
	bufvals=hsolve->bufvals;
	dt = hsolve->dt;
	if (BaseObject(hsolve)->method == CRANK_INT)
		dt2 = dt/2.0;
	else /* BEULER by default */
		dt2 = dt;

	action = GetAction("RECALC");
	
	nv=nb=0;
	for (i=0; i<ndiffs; i++) {
	  ms=numshells[i];
	  j=diffchild[i];		/* index first element */
	  concID=childcode[j];
	  ne=numshelms[i];	/* number of rows */
	  /* BANDED MATRIX */
	  /* cycle through all children of this diffusion system */
	  for (m=0; m<ms ; m++) {	/* all shells */
	    nbt=2;
	    while (childtypes[j]<FIXBUFF_T) j++;
	    for (k=0; k<ne ; k++,j++) {	/* all rows of each shell */
		ct=childtypes[j];
		if (childcode[j]!=concID) {
		    fprintf(stderr,"Error in %s: Bug #1 in h_setup_conc_solve %d %d.\n",hsolve->name,i,j);
		    return(ERR);
		}
		CallElement(children[j],action);  /* if necessary do 2nd init */
		switch (ct) {
		    case FIXBUFF_T:
			/* fixed buffer: buffer rates */
			buffer=(Dbuffer *)children[j];
			btot=dt*buffer->kBb*buffer->Btot;
			if (m==0) {
				bufvals[nb]=dt2*buffer->kBf;
				bufvals[nb+1]=dt2*buffer->kBb;
				bufvals[nb+2]=btot;
				nb+=3;
			}
			/* Buffer concentration can vary between shells */
			cov0[nv++]=btot;
			nbt+=3;
			break;

		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
			/* diffusible buffer: buffer rates */
			buffer=(Dbuffer *)children[j];
			btot=dt*buffer->kBb*buffer->Btot;
			/* Buffer concentration cannot vary between shells */
			if (m==0) {
			    bufvals[nb]=dt2*buffer->kBf;
			    bufvals[nb+1]=dt2*buffer->kBb;
			    bufvals[nb+2]=btot;
			    nb+=3;
			}
			if (fabs(btot-bufvals[nbt])>VTINY) {
			    Error();
			    printf(" during SETUP of %s: %s should have same concentration as in first shell.\n",Pathname(hsolve),Pathname(children[j]));
			    printf("%d %d\n",nb,nbt);
			    return(ERR);
			}
			nbt+=3;
			/* diffusible buffer: diffusion */
			Diff=2.0*dt2*buffer->D/buffer->vol;	/* 2/(thick+thick) */
			/* first shell: we diffuse to mid-point of tube -> total
			**  thickness=(2*thick0 + thick1)/2 */
			if (m<ms-1) {	/* not outer shell: upward diffusion */
			    if (m) { /* not first shell: downward diffusion */
				if (m==1) {	/* connected to first shell */
				    cov0[nv++]=Diff*buffer->surf_down/
					    (2*childdata[j-ne]+childdata[j]);
				} else {
				    cov0[nv++]=Diff*buffer->surf_down/
					    (childdata[j-ne]+childdata[j]);
				}
				cov0[nv++]=Diff*buffer->surf_up/
					(childdata[j]+childdata[j+ne]);
			    } else { /* inner shell: place in mid-point */
				cov0[nv++]=Diff*buffer->surf_up/
					(2*childdata[j]+childdata[j+ne]);
			    }
			} else {	/* outer shell */
			    cov0[nv++]=Diff*buffer->surf_down/
				    (childdata[j-ne]+childdata[j]);
			}
			break;

		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
			break;

		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
			/* shell diffusion: is always last in sequence */
			shell=(Dshell *)children[j];
			Diff=2.0*dt2*shell->D/shell->vol; /* 2/(thick+thick) */
			/* first shell: we diffuse to mid-point -> total
			**  thickness=(2*thick0 + thick1)/2 */
			if (m<ms-1) {	/* not outer shell: upward diffusion */
			    if (m) {  /* not first shell: downward diffusion */
				if (m==1) {	/* connected to first shell */
				    cov0[nv++]=Diff*shell->surf_down/
					    (2*childdata[j-ne]+childdata[j]);
				} else {
				    cov0[nv++]=Diff*shell->surf_down/
					    (childdata[j-ne]+childdata[j]);
				}
				cov0[nv++]=Diff*shell->surf_up/
					    (childdata[j]+childdata[j+ne]);
			    } else { /* inner shell: place in mid-point */
				cov0[nv++]=Diff*shell->surf_up/
					    (2*childdata[j]+childdata[j+ne]);
			    }
			} else {	/* outer shell */
			    cov0[nv++]=Diff*shell->surf_down/
					(childdata[j-ne]+childdata[j]);
			}
			break;

		    default:
			fprintf(stderr,"Error in %s: Bug #2 in h_setup_conc_solve %d %d.\n",hsolve->name,j,ct);
			return(ERR);
		}
	    }	/* k loop */
	}	/* m loop */
    }		/* i loop */
    if (nv>hsolve->fcoval[ndiffs]) {
	fprintf(stderr,"Error in %s: Bug #4 in h_setup_conc_solve %d.\n",hsolve->name,nv);
    }
    if (nb>hsolve->fbufval[ndiffs]) {
	fprintf(stderr,"Error in %s: Bug #5 in h_setup_conc_solve %d.\n",hsolve->name,nb);
    }
    return(0);
}

int h_init_conc_chip(hsolve)
/* Creates the cchip and cops arrays and fills the cops array. */
    Hsolve  *hsolve;
{
    int     i,j,k,l,n;
    int     ncompts=hsolve->ncompts;
    int     nconcs=hsolve->nconcs;
    Element **compts,*compt,**children,*child;
    short   *childtypes;
    int     *childstart,*childlink,*childmsg;
    int     ct,cm,cl;
    int     *elmnum,*childops,*childchips;
    int     nchip,nop,lastconc,lasttype;
    Action  *action;
    double  *chip;
    Hpump   *hpump;
    Tpump   *tpump;
    int     *ops;

    compts=hsolve->compts;
    children=hsolve->children;
    elmnum=hsolve->elmnum;
    childtypes=hsolve->childtypes;
    childlink=hsolve->childlink;
    childmsg=hsolve->childmsg;
    childstart=hsolve->childstart;
    childops=hsolve->childops;
    childchips=hsolve->childchips;

    action = GetAction("RECALC");

    /* First find the number of elements that the chip and ops arrays will
    **   have to deal with.  */
    nop=1;	/* LCONC_OP or LCACONC_OP */
    nchip=0;
    lastconc=-1; /* none selected */
    for (i=0;i<ncompts;i++){
        n=childstart[i];
        for (j=n; j<childstart[i+1]; j++) {
            child=children[j];
            CallElement(child,action);  /* update fields of element */
            ct=childtypes[j];
            cl=childlink[j];
            cm=childmsg[j];
            switch (ct) {
		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
		    if (cl>=FLUXOFFSET) {
			if (cm!=lastconc) {
			    nop+=2;	  /* CONC_OP */
			    lastconc=cm;
			}
			nop++;	/* FLUXCONC_OP */
			nchip+=2;
		    }
		    break;

		case TAUPUMP_T:
		    if (cm!=lastconc) {
			nop+=2;	  /* CONC_OP */
			lastconc=cm;
		    }
		    tpump=(Tpump *)child;
		    if ((tpump->T_A>0.0)||(tpump->T_B!=0.0)) {
			nop+=2;	/* TAUPUMP_OP & index */
			nchip+=5;
		    } else {
			nop++;	/* CTAUPUMP_OP */
			nchip+=2;
		    }
		    break;

		case MMPUMP_T:
		    if (cm!=lastconc) {
			nop+=2;	  /* CONC_OP */
			lastconc=cm;
		    }
		    nop++;	/* MMPUMP_OP */
		    nchip+=2;
		    break;

		case HILLPUMP_T:
		    if (cl) { /* pump to a concpool */
			if (cl!=lastconc) {
			    nop+=2;	  /* CONC_OP */
			    lastconc=cl;
			}
			nop+=3;	/* PHILLPUMP_OP, Hill coeff & diffshell index */
			nchip+=3;
		    } else {
			if (cm!=lastconc) {
			    nop+=2;	  /* CONC_OP */
			    lastconc=cm;
			}
			nop+=2;	/* HILLPUMP_OP & Hill coeff */
			nchip+=2;
		    }
		    break;

		case POOL_T:
		    if (cm!=lastconc) {
			nop+=2;	  /* FCONC_OP */
			lastconc=cm;
		    }
		    if (cl>=FLUXOFFSET) {
			nop++;	/* FLUXCONC_OP */
			nchip+=2;
		    }
		    nop++;	/* POOL_OP */
		    break;

		case POOLBUFF_T:
		    if (cl!=lastconc) {
			nop+=2;	  /* FCONC_OP */
			lastconc=cl;
		    }
		    nop+=2;	/* BUFFER_OP, index to buffer */
		    nchip+=3;
		    break;

		case CACONCEN_T:
		    nop+=2;      /* CONC_OP & index */
		    nchip+=3;
		    break;

		default:
		    break;
	    }
	}
    } /* i loop */
    /* Allocate chip and ops array */
    hsolve->nconcchips=nchip;
    chip=hsolve->concchip=(double *)calloc(nchip,sizeof(double));
    hsolve->nconcops=nop;
    ops=hsolve->concops=(int *)calloc(nop,sizeof(int));
    nop=nchip=0;
    lastconc=-nconcs; /* none selected */
    lasttype=0;
    for (i=0;i<ncompts;i++){
        compt = compts[elmnum[i]];
        n=childstart[i];
        for (j=n; j<childstart[i+1]; j++) {
            child=children[j];
            ct=childtypes[j];
            cm=childmsg[j];
            cl=childlink[j];
	    /* check if conc index needs to be updated */
            switch (ct) {
		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
		    if (cl<FLUXOFFSET) break;
		    /* NO BREAK */

		case TAUPUMP_T:
		case MMPUMP_T:
		case HILLPUMP_T:
		    if ((ct==HILLPUMP_T) && cl) {	/* linked to concpool */
			k=cl;
			l=POOL_T;
		    } else {
			k=cm;
			l=DIFSHELL_T;
		    }
		    if (k!=abs(lastconc)) {
			if (lastconc==-nconcs) { /* first one */
				ops[nop]=FCONC_OP;
			} else {
				ops[nop]=CONC_OP;
			}
			ops[nop+1]=k;
			nop+=2;
			lastconc=k;
			lasttype=l;
		    }
		    break;

		case POOL_T:
		case POOLBUFF_T:
		    if (ct==POOL_T) {
			k=cm;
		    } else {
			k=cl;
		    }
		    if (k!=abs(lastconc)) {
			if (lastconc==-nconcs) { /* first one */
			    ops[nop]=FCONC_OP;
			} else if (lasttype==POOL_T) {
			    ops[nop]=FCONC_OP;	/* no update coresult needed */
			} else {
			    ops[nop]=CONC_OP;
			}
			ops[nop+1]=k;
			nop+=2;
			lastconc=-k;
			lasttype=POOL_T;
		    }
		    break;

		default:
		    break;
	    }
            /* store childops and childchips for any concen child */
            switch (ct) {
		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
		case TAUPUMP_T:
		case MMPUMP_T:
		case HILLPUMP_T:
		case POOL_T:
		case POOLBUFF_T:
		case CACONCEN_T:
		    if (!hsolve->no_elminfo) {
			    childops[j]=nop;
			    childchips[j]=nchip;
		    }
		    break;

		default:
		    break;
	    }
	    /* store ops */
	    switch (ct) {
		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
		    h_check_msgs(hsolve,child,CONC_OP,ct,i,nop,nchip);
		    if (cl>=FLUXOFFSET) {
			/* has in/outflows and/or leak */
			ops[nop]=FLUXCONC_OP;
			nop++;
			nchip+=2;
		    }
		    break;

		case FIXBUFF_T:
		case BUFSHELL_T:
		case BUFSLAB_T:
		case BUFUSER_T:
		    h_check_msgs(hsolve,child,BUFFER_OP,ct,i,nop,nchip);
		    break;

		case TAUPUMP_T:
		    h_check_msgs(hsolve,child,TAUPUMP_OP,ct,i,nop,nchip);
		    tpump=(Tpump *)child;
		    if ((tpump->T_A>0.0)||(tpump->T_B!=0.0)) {
			ops[nop]=TAUPUMP_OP;
			ops[nop+1]=i;	/* index into vm */
			nop+=2;
			nchip+=5;
		    } else {
			ops[nop]=CTAUPUMP_OP;
			nop++;
			nchip+=2;
		    }
		    break;

		case MMPUMP_T:
		    h_check_msgs(hsolve,child,MMPUMP_OP,ct,i,nop,nchip);
		    ops[nop]=MMPUMP_OP;
		    nop++;
		    nchip+=2;
		    break;

		case HILLPUMP_T:
		    hpump=(Hpump *)child;
		    if (cl) { /* pump to a concpool */
			ops[nop]=PHILLPUMP_OP;
			ops[nop+1]=hpump->Hill;
			ops[nop+2]=cm;	/* difshell conc */
			nop+=3;
			nchip+=3;
		    } else {	/* pump to nowhere */
			ops[nop]=HILLPUMP_OP;
			ops[nop+1]=hpump->Hill;
			nop+=2;
			nchip+=2;
		    }
		    break;

		case POOL_T:
		    h_check_msgs(hsolve,child,CONC_OP,ct,i,nop,nchip);
		    if (cl>=FLUXOFFSET) {
			/* has in/outflows and/or leak */
			ops[nop]=FLUXCONC_OP;
			nop++;
			nchip+=2;
		    }
		    ops[nop]=POOL_OP;
		    nop++;
		    break;

		case POOLBUFF_T:
		    h_check_msgs(hsolve,child,BUFFER_OP,ct,i,nop,nchip);
		    ops[nop]=BUFFER_OP;
		    ops[nop+1]=cm;
		    nop+=2;
		    nchip+=3;
		    break;

		case CACONCEN_T:
		    h_check_msgs(hsolve,child,CACONC_OP,ct,i,nop,nchip);
		    ops[nop]=CACONC_OP;
		    ops[nop+1]=cm;
		    nop+=2;
		    nchip+=3;
		    break;

		default:
		    break;
	    }
	}
    } /* i loop */
    if (lastconc>-1) {	/* no difshells */
	ops[nop]=LCONC_OP;
    } else {
	ops[nop]=LCACONC_OP;
    }
    if (nop+1!=hsolve->nconcops) {
	fprintf(stderr,"Error in %s: Bug #1 in h_init_conc_chip %d.\n",hsolve->name,nop);
    }
    if (nchip!=hsolve->nconcchips) {
	fprintf(stderr,"Error in %s: Bug #2 in h_init_conc_chip %d.\n",hsolve->name,nchip);
    }
    return(0);
}

