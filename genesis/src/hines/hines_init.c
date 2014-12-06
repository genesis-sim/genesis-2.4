/* Version EDS22d 98/08/14, Erik De Schutter, Caltech & BBF-UIA 4/94-8/98 */
/* Original version by Upi Bhalla, Caltech 1991 */

/*
** EDS22j revison: HC BBF-UIA 00/03/13
** default value of ->msgcompts[i] dependent on ->computeIm field
**
** EDS22c revison: EDS BBF-UIA 97/11/28-98/08/14
** Extensively edited original h_init.c code
** Made symmetric compartments work
** Deleted use of values array
** Improved error messages
**
*/

#include "hines_ext.h"

int h_init(hsolve)
	Hsolve	*hsolve;
{
	ElementList *list;
	int 	ncompts;
	Comp	**compts = NULL;
	Comp	*compt;
	int 	i,j,k,n,f;
	MsgIn	*msgin;
	int	hnumcount;
	int	*parents,**kids,*nkids,*elmnum,*hnum;
	int	temp;
	int	chanmode,symflag;
	Element *elm;
	Element *saveelm;
	char*   oname;
	int	hhas_msg_toelm();

	double		Ra1;
	double 		Ra_sum,Ra_sum2;

	chanmode=hsolve->chanmode;
	if (hsolve->readflag < HREAD_T) { /* standard initialization */
	    if (!(hsolve->path)) {
		Error();
		printf(" during SETUP of %s: no path defined.\n",Pathname(hsolve));
		return(ERR);
	    }
	    /* Allow path relative to hsolve to work always */
	    saveelm = WorkingElement();
	    SetWorkingElement(hsolve);
	    list = WildcardGetElement(hsolve->path,0);
	    SetWorkingElement(saveelm);

	    hsolve->ncompts = ncompts = list->nelements;
	    hsolve->compts = list->element;
	    compts = (Comp **)hsolve->compts;
	} else {
	    ncompts=hsolve->ncompts;
	}
	if (ncompts==0) {
	    elm=WorkingElement();
	    Error();
	    printf(" during SETUP of %s: empty path.\n",Pathname(hsolve));
	    return(ERR);
	}
	hsolve->parents = parents = (int *)calloc(ncompts, sizeof(int));
	hsolve->nkids = nkids = (int *)calloc(ncompts, sizeof(int));
	hsolve->kids = kids = (int **)calloc(ncompts, sizeof(int *));
	hsolve->hnum = hnum = (int *)calloc(ncompts,sizeof(int));
	hsolve->elmnum = elmnum = (int *)calloc(ncompts,sizeof(int));
	if (chanmode<2)
	    hsolve->msgcompts = (short *)calloc(ncompts,sizeof(short));
	hsolve->outinfo = NULL;

	if (hsolve->readflag==HREAD_T) { /* called by readsolve */
	    /*
	    if (init_hparents(hsolve))
		    return(ERR);
	    */
	}
	/* Determine if symmetric or not */
	compt = compts[0];
	oname = BaseObject(compt)->name;
	if (strcmp(oname,"symcompartment") == 0) {
	    symflag=1;
	} else if (strcmp(oname,"compartment") == 0) {
	    symflag=0;
	} else {
	    Error();
	    printf(" during SETUP of %s: type '%s' not supported as path.\n",Pathname(hsolve),oname);
	    return(ERR);
	}
	hsolve->symflag=symflag;
	/* AXIALs go from parent to kid compts */
	for (i=0;i<ncompts;i++){
	    compt = compts[i];
	    oname = BaseObject(compt)->name;
	    if ((!symflag && (strcmp(oname,"compartment") != 0)) ||
		(symflag && (strcmp(oname,"symcompartment") != 0))) {
		Error();
		printf(" during SETUP of %s: %s is not not same type of compartment as the others.\n",Pathname(hsolve),Pathname(compt));
		return(ERR);
	    }
	    /* disable element to prevent it from doing anything. */
	    HsolveBlock(compt);
	    parents[i] = -1;
	    if (chanmode<2)
	    {
		if (!hsolve->computeIm)
		{
		    hsolve->msgcompts[i] = 0;
		}
		else
		{
		    hsolve->msgcompts[i] = 1;
		}
	    }

	    Ra_sum = 0.0;
	    Ra_sum2 = 0.0;

	    /* look for parent compts */
	    MSGLOOP(compt, msgin) {
	    case AXIAL: /* 2 : is same as CONNECTHEAD */
		if (Strindex(BaseObject(msgin->src)->name,"compartment")>=0)
		{
		    for (j=0;j<ncompts;j++)
		    {
			if (msgin->src == (Element *)compts[j])
			{
			    /* the jth elment is a parent of the ith element */
			    if (parents[i] != -1)
			    {
				Error();
				printf
				    (" during SETUP of %s: multiple parents for compartment '%s'.\n",
				     Pathname(hsolve),
				     Pathname(compt));
				return(ERR);
			    }
			    parents[i] = j;
			    nkids[j] += 1;
			    break;
			}
		    }
		}

		if (symflag)
		{
		    Ra1 = MSGVALUE(msgin,0);
		    if (Ra1 <= 0.0)
		    {
			ErrorMessage
			    ("hsolve",
			     "Incoming Ra invalid.",
			     compt);
		    }
		    Ra_sum2 += compt->Ra/Ra1;
		}
		break;

	    case RAXIAL:  /* 1 : resistive axial */
		if (symflag)
		{
		    Ra1 = MSGVALUE(msgin,0);
		    if (Ra1 <= 0.0)
		    {
			ErrorMessage
			    ("hsolve",
			     "Incoming Ra invalid.",
			     compt);
		    }
		    Ra_sum += compt->Ra/Ra1;
		}
		break;
	    case CONNECTCROSS: /* 5 : branch of symcompartments */
		if (symflag)
		{
		    Ra1 = MSGVALUE(msgin,0);
		    if (Ra1 <= 0.0)
		    {
			ErrorMessage
			    ("hsolve",
			     "Incoming Ra invalid.",
			     compt);
		    }
		    Ra_sum2 += compt->Ra/Ra1;
		}
		break;
	    case INJECT:
	    case EREST:
		if (chanmode<2) {
		    /* we need to update internal fields in compartment */
		    (hsolve->msgcompts[i])++;
		}
		break;

	    default:
		break;
	    }

	    if (symflag)
	    {
		((Ncomp *)compt)->coeff = (1 + Ra_sum)/2.0;
		((Ncomp *)compt)->coeff2 = (1 + Ra_sum2)/2.0;
	    }
	}
	/* Fill in indices for kids */
	for (i=0;i<ncompts;i++){
	    k=0;
	    if (nkids[i] == 0) {
		kids[i] = NULL; /* a terminal branch */
	    } else {
		kids[i]=(int *)calloc(nkids[i],sizeof(int));
		for (j=0;j<ncompts;j++) {
		    if (parents[j] == i) {
			kids[i][k] = j;
			k++;
		    }
		}
	    }
	}

	/* find the soma */
	for (i=0;i<ncompts;i++){
	    if (parents[i] == -1) {
	    /* This is the trunk element. Usually it will be the soma */
		hnumcount = ncompts-1;
		/* Do Hines numbering */
		do_hnum(hsolve,i,&hnumcount,elmnum);
		break;
	    }
	}
	/* sort the kids by Hines number (useful for h_funcs_init ) */
	for (i=0;i<ncompts;i++){
	    j=elmnum[i];
	    n=nkids[j];
	    /* do bubble sort */
	    f=1;
	    while (f) {
		f=0;
		for(k=1;k<n;k++) {
		    if (hnum[kids[j][k-1]]>hnum[kids[j][k]]) {
			temp = kids[j][k-1];
			kids[j][k-1] = kids[j][k];
			kids[j][k] = temp;
			f=1;
		    }
		}
	    }
	}
	return(0);
}

int h2_init(hsolve)
	Hsolve	*hsolve;
{
	int ncompts;

	ncompts=hsolve->ncompts;
	hsolve->vm = (double *) calloc(ncompts,sizeof(double));
	if (hsolve->numnodes==0) {	/* uniprocessor mode */
	    /* diag also stored in results */
	    hsolve->results=(double *)calloc(2*ncompts,sizeof(double));
	    hsolve->vmlast=hsolve->vm + hsolve->ncompts - 1;  /* pointer to end of vm array */
	    if (hsolve->chanmode<2)
		hsolve->diagonals=(double *)calloc(ncompts,sizeof(double));
	}
	return(0);
}

/* Doing hines numbering */
int do_hnum(hsolve,comptno,hnum,elmnum)
	Hsolve	*hsolve;
	int	comptno;
	int	*hnum;
	int	*elmnum;
{
	int i;
	int kidno;

	hsolve->hnum[comptno]= *hnum;
	elmnum[*hnum]=comptno;
	*hnum -= 1;
	for(i=0;i<hsolve->nkids[comptno];i++) {
	    /* Numbering kids of this elm which have no kids of their own */
	    kidno = hsolve->kids[comptno][i];
	    if (hsolve->nkids[kidno]==0) {
		hsolve->hnum[kidno] = *hnum;
		elmnum[*hnum]=kidno;
		*hnum -= 1;
	    }
	}
	for(i=0;i<hsolve->nkids[comptno];i++) {
	    /* Numbering kids of this elm which do have kids of their own */
	    kidno = hsolve->kids[comptno][i];
	    if (hsolve->nkids[kidno]>0) {
		do_hnum(hsolve,kidno,hnum,elmnum);
	    }
	}
}

/* This routine sets up the diagonals (or results if chanmode>1), funcs and
**  ravals arrays.  It does this in two passes : first, it finds the number
**  of entries in the funcs and ravals arrays, second (during RESET) it
**  fills them up.
** funcs: array with instruction codes for fast solution of the
**  sparse matrix. Is created only if comptmode>0 (default)
** diagonals: array with the constant part of the diagonal values.
** ravals: contains the off-diagonal parameters in order of funcs access.  Is
**  allways created.  For symmetric compartments it will be changed during
**  the simulation.
** If chanmode >1: stores diag elements in results array
** The code contains many controls which produce "Bug" errors if triggered.
**  This was done to protect agains untested complex situtations (e.g. many
**  children). */
int h_funcs_init(hsolve)
	Hsolve	*hsolve;
{
	int     i,j,k,l = 0,m,n;
	int	ncompts=hsolve->ncompts;
	int     symflag=hsolve->symflag;
	int	*elmnum=hsolve->elmnum;
	int	*hnum=hsolve->hnum;
	int	*parents=hsolve->parents;
	int	*nkids=hsolve->nkids;
	int	**kids=hsolve->kids;
	int	*compchips=hsolve->compchips;
	double  *diagonals=hsolve->diagonals;
	double  *chip=hsolve->chip;
	double  *results=hsolve->results;
	int	parentno,nkid,row,row2,maxkids=1,self = 0;
	int     comptindex;
	int     justcount=1,skipdiag=0,hassiblefts=0,found;
	int     nfuncs=0,nravals=0;
	int	*funcs = NULL,**ravptr = NULL;
	double  *ravals = NULL,*coeffs,*diagterms;
	double  temp,dt,Cm,Ra;
	Comp	**compts=(Comp **)hsolve->compts;
	Comp	*compt,*parent = NULL,*link,*link2;
	int	hhas_msg_toelm();

	coeffs=NULL;
	diagterms=NULL;
	if (BaseObject(hsolve)->method == CRANK_INT)
	    dt = hsolve->dt/2.0;
	else /* BEULER by default */
	    dt = hsolve->dt;

        if (hsolve->nfuncs == 0) {
	    if (symflag) {
		/* ravptr orders: 0:                   right parent off-diagonal
		**                1 - maxkid:          left kid off-diagonals
		**                maxkid+1 - 2*maxkid: all sib off-diagonals
		**  All ordered as in kids
		**  Initialized to -1, then set to a funcs index and cleared (0)
		**  or used as storage for a ravals index (<-1).
		*/
		/* create pointer array for ravals */
		for (i=0;i<ncompts;i++) {
		    nkid=nkids[i];
		    if (nkid>maxkids) maxkids=nkid;
		}
		hsolve->ravptr=ravptr=(int **)calloc(ncompts,sizeof(int *));
		n=maxkids*2+1;
		for (i=0;i<ncompts;i++) {
		    ravptr[i]=(int *)calloc(n,sizeof(int));
		    for (j=0;j<n;j++) ravptr[i][j]=-1;
		}
	    }
	} else {
            justcount = 0;
            if (hsolve->funcs)  {
		funcs=hsolve->funcs;
            } else {
                hsolve->funcs=funcs=(int *)calloc(hsolve->nfuncs,sizeof(int));
	    }
	    if (hsolve->ravals) {
		ravals=hsolve->ravals;
	    } else {
                if (hsolve->nravals) hsolve->ravals=ravals=(double *)calloc(hsolve->nravals,sizeof(double));
            }
	    /* create temporary storage for diagterms */
	    diagterms=(double *)calloc(ncompts,sizeof(double));
	    /* if symmetric compute the coefficients for the Ra values */
	    if (symflag) {
		ravptr=hsolve->ravptr;
		for (i=0;i<ncompts;i++) ravptr[i][0]=-1;  /* clear again */
		/* create temporary storage */
		coeffs=(double *)calloc(ncompts*2,sizeof(double));
		for (i=0;i<ncompts;i++) {
		    comptindex=elmnum[i];
		    compt=compts[comptindex];
		    Ra=compt->Ra;
		    /* coeffs for head Ra */
		    temp=0.0;
		    parentno=parents[comptindex];
		    if (parentno>=0) {
			temp=Ra/compts[parentno]->Ra;
			for (j=0;j<nkids[parentno];j++) {
			    k=kids[parentno][j];
			    if (k!=comptindex) temp+=Ra/compts[k]->Ra;
			}
		    }
		    coeffs[i*2]=(1.0 + temp)/2.0;
		    /* coeff for tail Ra */
		    temp=0.0;
		    nkid=nkids[comptindex];
		    if (nkid>maxkids) maxkids=nkid;
		    for (j=0;j<nkid;j++) {
			k=kids[comptindex][j];
			temp+=Ra/compts[k]->Ra;
		    }
		    coeffs[i*2+1]=(1.0 + temp)/2.0;
		}
	    }
        }

	/* Looping over all rows, doing forward substitution:
	**   we remove the lower left triangle of the matrix
	*/
/* OPTIMIZATION: do all COPY_ARRAYs in a separate loop first */
	for (i=0;i<ncompts;i++) {
	    comptindex=elmnum[i]; /* i is the hnum of the elm */
	    compt=compts[comptindex];
	    Cm=compt->Cm;
	    if (!justcount) diagterms[i]=1.0 + dt/(Cm*compt->Rm);

	    /* Start eliminating the PRECEDING ROWS */
	    parentno=parents[comptindex];
	    if (parentno>-1)
	    {
		parent=compts[parentno];
	    }
	    nkid=nkids[comptindex];
	    if (!justcount && symflag) {
		for (l=nkid+1;l<=maxkids;l++) ravptr[i][l]=0; /* not used */
	    }

	    /* Do children first */
	    /* get diagonal and result for this row:
	    **   code to execute: store previous resultvalue and diavalue
	    **               and: resultvalue=results[i];
	    **               and: diavalue = diagonal[i];
	    */
	    hassiblefts=0;
	    if (symflag) {
	      if (parentno>-1) {
		/* find our own sib number */
		n=nkids[parentno];
		for (j=0; j<n; j++) {
		    if (kids[parentno][j]==comptindex) {
			self=j;
			break;
		    }
		}
		if (n && (j==n)) {
		    printf("Error in %s: Bug #0 in hines_init %d %d.\n",hsolve->name,i,j);
		    return(ERR);
		}
		/* copy parent right-off diagonal this row if it will change:
		**  if it has siblings and it is not the first one */
		if (n>1) {	/* has sibs */
		    if (self>0) {	/* is not first sib */
			if (justcount) {
			    nfuncs+=2;
			    nravals++;
			} else {
			    funcs[nfuncs++]=COPY_ARRAY;
			    ravptr[i][0]=nfuncs;
			    funcs[nfuncs++]=-1;	/* mark as unresolved */
/* test2.analysis: store p4 on row 4 for row 5 */
/* test3.analysis: store P8 on row 8 for row 14 */
/* test3.analysis: store q13 on row 13 for row 14 */
			    /* kid: head connection to parent */
			    temp=-dt/(Cm*parent->Ra*coeffs[i*2]);
			    ravals[nravals++]=temp;
			    diagterms[i]-=temp;
			    hassiblefts=1;
			}
		    }
		} else {
		    if (!justcount) ravptr[i][0]=0;	/* not used */
		}
		if (i==0) {	/* not much done on first row... */
		    /* this code is needed for the control functions only */
		    if (!justcount) {	/* clear unused and sib indices*/
			ravptr[0][maxkids+1]=0;
			for (j=maxkids+n+1;j<=maxkids*2;j++) ravptr[0][j]=0;
		    }
		    continue;
		}
	      } else {
		if (!justcount) ravptr[i][0]=0;	/* not used */
	      }
	    }
	    if (skipdiag) {
		skipdiag=0;
/*  OPTIMIZE FOR DOUBLE SKIP */
	    } else if (nkid || hassiblefts) {
		if (i>1) {	/* first SET_DIAG always done */
		    if (justcount) {
			nfuncs++;
		    } else {
			funcs[nfuncs++]=SET_DIAG;
		    }
		}
	    } else {	/* compt is end of unbranched cable without
			**  sib left-off diagonals */
		if (i>1) {	/* first SET_DIAG always done */
		    if (justcount) {
			nfuncs++;
		    } else {
			funcs[nfuncs++]=SKIP_DIAG;
		    }
		    skipdiag=1;
		}
	    }
	    for (j=0; j<nkid; j++) {
		k=kids[comptindex][j];
		link=compts[k];
		row=hnum[k];	/* convert to hines number */
		/* calculate scaling factor and eliminate left off-diag:
		**   code to execute: temp = ravals[n++]/diag[row];
		**               and: diag[i] -= ravals[n++]*temp;
		**               and: results[i] -= results[row]*temp;
		**   with ravals[n++] the left off-diagonal for row
		*/
		if (justcount){
		    nfuncs+=2;			/* FORWARD_ELIM */
		    nravals+=2;
		    if (symflag) {
			ravptr[row][0]=1;/* we need to know this exists later */
			nfuncs+=2*(nkid-1);	/* COPY_ARRAY */
			nravals+=nkid-1;
			nfuncs+=3*(nkid-j-1);	/* SIBARRAY_ELIM */
		    }
		} else {
/* POSSIBLE OPTIMIZATION: IN UNBRANCHED CABLE ROW = i - 1 */
		    funcs[nfuncs++]=FORWARD_ELIM;
		    funcs[nfuncs++]=2*row;
		    if (symflag) { /* symmetric elements */
/* test2.analysis: do row 5 */
/* test3.analysis: do row 14 */
			/* compute ravals for temp: left off-diag on row i
			**  parent: tail connection to kid */
			if (j==0) { /* first one never changes */
/* test2.analysis: eliminate P5, contributed by row 2 */
/* test3.analysis: eliminate Q14, contributed by row 2 */
			    temp=-dt/(Cm*link->Ra*coeffs[i*2+1]);
			    ravals[nravals++]=temp;
			    diagterms[i]-=temp;
			} else {   /* has changed: check if ptr correct! */
/* test2.analysis: eliminate p5', contributed by row 3 */
/* test3.analysis: eliminate P14', contributed by row 8 */
/* test3.analysis: eliminate q14", contributed by row 13 */
			    m=ravptr[i][j+1];
			    if ((m<=0)||(funcs[m]!=-1)) {
				printf("Error in %s: Bug #1 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    funcs[m]=nravals;	/* index is now resolved */
			    found=0;
			    for (l=m; l<nfuncs; l++) {
				/* resolve copies of index also */
				if (funcs[l]==-m) {
				    funcs[l]=nravals;
				    found=1;
				}
			    }
			    if (found==0) {
				printf("Error in %s: Bug #-1 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    nravals++;	/* computed during simulation */
			}
			ravptr[i][j+1]=0;	/* should be done */
			/* compute ravals for diag[i]: right off-diag on row row
			**  kid: head connection to parent */
/* test2.analysis: compute p1 on row 1 */
/* test3.analysis: compute P2 on row 2 */
			if (j==0) { /* first one never changes */
			    m=ravptr[row][0];
			    if (m<-1) {	/* already computed -> copy it */
				ravals[nravals]=ravals[-m];
			    } else {	/* compute it */
				temp=-dt/(link->Cm*compt->Ra*coeffs[row*2]);
				ravals[nravals]=temp;
				diagterms[row]-=temp;
			    }
			} else {   /* has changed: resolve funcs index */
/* test2.analysis: use p4' on row 4 */
/* test3.analysis: use P8' on row 8 */
/* test3.analysis: use q13" on row 13 */
			    m=ravptr[row][0];
			    if ((m<=0)||(funcs[m]!=-1)) {
				printf("Error in %s: Bug #2 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    funcs[m]=nravals;	/* index is now resolved */
/* Search is necessary */
			    for (l=m; l<nfuncs; l++) {
				/* resolve copies of index also */
				if (funcs[l]==-m) {
				    funcs[l]=nravals;
				}
			    }
			}
			ravptr[row][0]=-nravals;  /* store for back elim */
			nravals++;	/* computed during simulation */
		    } else { /* asymmetric elements: RAXIAL connection */
/* test2.analysis: do row 4 */
			/* compute ravals for temp: left off-diag on row i
			**  RAXIAL connection */
/* test2.analysis: eliminate P4, contributed by row 3 */
			temp=-dt/(Cm*link->Ra);
			ravals[nravals++]=temp;
			diagterms[i]-=temp;
			/* compute ravals for diag[i]: right off-diag on row row
			**  AXIAL connection */
/* test2.analysis: compute P3, contributed by row 3 */
			temp=-dt/(link->Cm*link->Ra);
			ravals[nravals++]=temp;
			diagterms[row]-=temp;
		    }
		    if (symflag) {
			if (j==0) {
			    /* copy all the left-off diagonals which will
			    ** be changed first: if more than one kid */
			    for (l=1; l<nkid; l++) {
				funcs[nfuncs++]=COPY_ARRAY;
				ravptr[i][l+1]=nfuncs;
				funcs[nfuncs++]=-1;	/* mark as unresolved */
				n=kids[comptindex][l];
				link2=compts[n];
				row2=hnum[n];	/* convert to hines number */
/* test2.analysis: store p5, contributed by row 4 */
/* test3.analysis: store P14, contributed by row 8 */
/* test3.analysis: store q14, contributed by row 13 */
				/* parent: tail connection to kid */
				temp=-dt/(Cm*link2->Ra*coeffs[i*2+1]);
				ravals[nravals++]=temp;
				diagterms[i]-=temp;
			    }
			}
			/* compute all the changes to off-diagonals right of
			**  this one on the same row caused by this kid */
			for (l=j+1; l<nkid; l++) {
/* test2.analysis: compute p5', elimination of row 2 (S2) */
/* test3.analysis: compute P14', elimination of row 2 (S2) */
/* test3.analysis: compute q14', elimination of row 2 (s2) */
/* test3.analysis: compute q14", elimination of row 8 (S8') */
/* OPTIMIZATION: can also be a FASTSIB: see elimination of p13 and q14 */
			    funcs[nfuncs++]=SIBARRAY_ELIM;
			    m=ravptr[row][maxkids+l+1];
			    /* m < 0: from first sib -> won't change
			    ** m > 0: from later sib -> mark unresolved */
			    funcs[nfuncs++]=-m;	/* store index */
			    if (m==0) {
				printf("Error in %s: Bug #3 in hines_init %d %d %d: %d %d.\n",hsolve->name,i,j,l,m,nravals);
				return(ERR);
			    }
			    m=ravptr[i][l+1];
			    if (m<=0) {
				printf("Error in %s: Bug #4 in hines_init %d %d %d: %d %d.\n",hsolve->name,i,j,l,m,nravals);
				return(ERR);
			    }
			    funcs[nfuncs++]=-m;	/* mark as copy of unresolved */
			}
		    }
		}
	    }
	    n=0;	/* number of sib off-diagonal for this row */
	    if (symflag) {
	      if (parentno>-1) {
		nkid=nkids[parentno];
		if (!justcount) {
		    ravptr[i][maxkids+1]=0;	/* not used */
		    ravptr[i][maxkids+1+self]=0;	/* not used */
		    for (l=maxkids+nkid+1;l<=2*maxkids;l++)
						ravptr[i][l]=0; /* not used */
		}
		if (nkid<2) continue;		/* no sibs */
		/* Do siblings next */
		for (j=0; j<nkid; j++) {
		    k=kids[parentno][j];
		    link=compts[k];
		    row=hnum[k];	/* convert to hines number */
		    if ((nkid>2) && (self>0) && (j>0)) {
			/* compute all off diagonals except first one for this
			**  row as they will all change.
			**  kid: head cross connection to sib */
			if (justcount){
			     nfuncs+=2;
			     nravals++;
			} else {
			    temp=-dt/(Cm*link->Ra*coeffs[i*2]);
			    ravals[nravals++]=temp;
			    diagterms[i]-=temp;
			    /* copy this right-off diagonal which will change */
			    funcs[nfuncs++]=COPY_ARRAY;
/* test3.analysis: store S8 on row 8 for row 13 */
			    ravptr[i][maxkids+j+1]=nfuncs;
			    funcs[nfuncs++]=-1;	/* mark as unresolved */
			}
		    }
		    if (row>=i) continue;	/* no elimination here */
		    /* calculate scaling factor and eliminate left off-diag:
		    **   code to execute: temp = ravals[n++]/diag[row];
		    **               and: diag[i] -= ravals[n++]*temp;
		    **               and: results[i] -= results[row]*temp;
		    **   with ravals[n++] the left off-diagonal for row
		    */
		    if (justcount){
			nfuncs+=2;			/* FORWARD_ELIM */
			nravals+=2;
		    } else {
			funcs[nfuncs++]=FORWARD_ELIM;
			funcs[nfuncs++]=2*row;
/* test2.analysis: do row 4 */
/* test3.analysis: do row 13 */
			/* compute ravals for temp: left off-diag on row i
			**  kid: head cross connection to sib */
			if (n==0) { /* first one never changes */
/* test2.analysis: eliminate S4, contributed by row 2 */
/* test3.analysis: eliminate s13, contributed by row 2 */
			    temp=-dt/(Cm*link->Ra*coeffs[i*2]);
			    ravals[nravals++]=temp;
			    diagterms[i]-=temp;
			} else {   /* has changed: check if ptr correct! */
/* test3.analysis: eliminate S13, contributed by row 8 */
			    m=ravptr[i][maxkids+j+1];
			    if ((m<=0)||(funcs[m]!=-1)) {
				printf("Error in %s: Bug #5 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    funcs[m]=nravals;	/* index is now resolved */
			    found=0;
			    for (l=m; l<nfuncs; l++) {
				/* resolve copies of index also */
				if (funcs[l]==-m) {
				    funcs[l]=nravals;
				    found=1;
				}
			    }
			    if (found==0) {
				printf("Error in %s: Bug #-5 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    ravptr[i][maxkids+j+1]=nravals;  /* store for backward elim */
			    nravals++;	/* computed during simulation */
			}
			/* compute ravals diag[i]: right off-diag on row row
			**  kid: cross connection to sib */
/* test2.analysis: compute S2 on row 2 */
/* test3.analysis: compute S2 on row 2 for elimination S8 */
/* test3.analysis: compute s2 on row 2 for elimination s13 */
			if (n==0) { /* first one never changes */
			    temp=-dt/(link->Cm*compt->Ra*coeffs[row*2]);
			    ravptr[row][maxkids+self+1]=-nravals;
			    ravals[nravals++]=temp;
			    diagterms[row]-=temp;
			} else {   /* has changed: resolve funcs index */
/* test3.analysis: use s8' on row 8 for elimination S13 */
			    m=ravptr[row][maxkids+j+1];
			    if ((m<=0)||(funcs[m]!=-1)) {
				printf("Error in %s: Bug #6 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    funcs[m]=nravals;	/* index is now resolved */
			    found=0;
			    for (l=m; l<nfuncs; l++) {
				/* resolve copies of index also */
				if (funcs[l]==-m) {
				    funcs[l]=nravals;
				    found=1;
				}
			    }
			    if (found==0) {
				printf("Error in %s: Bug #-6 in hines_init %d %d: %d %d.\n",hsolve->name,i,j,m,nravals);
				return(ERR);
			    }
			    nravals++;	/* computed during simulation */
			    ravptr[row][maxkids+j+1]=0;
			}
		    }
		    if (self>0) {
			/* compute the changes to right parent off-diag */
/* test2.analysis: compute p4', elimination of row 2 (P2) */
/* test3.analysis: compute P8', elimination of row 2 (P2) */
/* test3.analysis: compute q13', elimination of row 2 (P2) */
/* test3.analysis: compute q13", elimination of row 8 (P8') */
			m=ravptr[row][0];
			if (m==-1) {	/* not used yet -> compute */
			    if (justcount) {
				nfuncs+=2;		/* FASTSIBARRAY_ELIM */
				nravals++;
			    } else {
				funcs[nfuncs++]=FASTSIBARRAY_ELIM;
				ravptr[row][0]=-nravals;
				temp=-dt/(link->Cm*parent->Ra*coeffs[row*2]);
				ravals[nravals++]=temp;
				diagterms[row]-=temp;
				m=ravptr[i][0];
				if (m<=0) {
				    printf("Error in %s: Bug #-7 in hines_init %d %d %d: %d %d.\n",hsolve->name,i,j,l,m,nravals);
				    return(ERR);
				}
				funcs[nfuncs++]=-m; /* mark as copy unresolved*/
			    }
			} else {		/* use precomputed one */
			    if (justcount) {
				nfuncs+=3;		/* SIBARRAY_ELIM */
			    } else {
				funcs[nfuncs++]=SIBARRAY_ELIM;
				if ((m==-1)||(m==0)||((j>0)&&(m<0))) {
				    printf("Error in %s: Bug #7 in hines_init %d %d %d %d: %d.\n",hsolve->name,i,j,row,m,nravals);
				    return(ERR);
				}
				/* m < 0: from first sib -> won't change
				** m > 0: from later sib -> mark unresolved */
				funcs[nfuncs++]=-m;
				m=ravptr[i][0];
				if (m<=0) {
				    printf("Error in %s: Bug #-7 in hines_init %d %d %d: %d %d.\n",hsolve->name,i,j,l,m,nravals);
				    return(ERR);
				}
				funcs[nfuncs++]=-m; /* mark as copy unresolved*/
			    }
			}

			/* compute the changes to other sib off-diagonals
			**  right of this one on the same row */
			for (l=j+1; l<nkid; l++) {
			    if (l==self) continue;
/* test3.analysis: compute S8', elimination of row 2 (s2) */
/* test3.analysis: compute S13', elimination of row 2 (S2) */
/* OPTIMIZATION: if constant copy it to ravals++, make new funcs code */
			    if (justcount) {
				nfuncs+=3;		/* SIBARRAY_ELIM */
			    } else {
				funcs[nfuncs++]=SIBARRAY_ELIM;
				m=ravptr[row][maxkids+l+1];
				if ((m==-1)||(m==0)||((j>0)&&(m<0))) {
				    printf("Error in %s: Bug #8 in hines_init %d %d %d: %d %d.\n",hsolve->name,i,j,l,m,nravals);
				    return(ERR);
				}
				/* m < 0: from first sib -> won't change
				** m > 0: from later sib -> mark unresolved */
				funcs[nfuncs++]=-m;
				m=ravptr[i][maxkids+l+1];
				if (m<=0) {
				    printf("Error in %s: Bug #-8 in hines_init %d %d %d: %d %d.\n",hsolve->name,i,j,l,m,nravals);
				    return(ERR);
				}
				funcs[nfuncs++]=-m; /* mark as copy unresolved*/
			    }
			}
			n++;
		    }
		}
	      } else {
		for (l=maxkids+1;l<=2*maxkids;l++) ravptr[i][l]=0;/* not used */
	      }
	    }
	}
	/* Perform some controls */
	if (!justcount && symflag) {
	    /* Check if all ravptrs resolved */
	    for (l=0; l<nfuncs; l++) {
		if (funcs[l]<0) {
		    printf("Error in %s: Bug #9 in hines_init %d: %d.\n",hsolve->name,l,funcs[l]);
		    return(ERR);
		}
	    }
	    n=1;	/* ravptr[0][0]=-1 normally */
	    for (i=0;i<ncompts;i++) {
		for (l=n;l<=maxkids*2;l++) {
		    if ((((l==0)||(l>=maxkids))&&(ravptr[i][l]==-1))||
			(((l>0)&&(l<maxkids))&&(ravptr[i][l]))) {
			printf("Error in %s: Bug #10 in hines_init %d %d: %d.\n",hsolve->name,i,l,ravptr[i][l]);
			return(ERR);
		    }
		}
		n=0;
	    }
	}
        if (justcount){
            nfuncs++;
        } else {
            funcs[nfuncs++]=FINISH;	/* forward loop */
        }

	/* looping over all rows, doing backwards elimination */
	/* Eliminate contributions from upper rows: parents and sibs */
	for (i=ncompts-2;i>=0;i--) {	/* soma done automatically */
	    comptindex=elmnum[i];
	    compt=compts[comptindex];
	    Cm=compt->Cm;
	    parentno=parents[comptindex];
	    if (symflag) {
		nkid=nkids[parentno];
		for (j=0; j<nkid; j++) {
		    if (kids[parentno][j]==comptindex) {
			self=j;
			break;
		    }
		}
	    } else {
		nkid=0;	/* no contribution from sibs */
	    }

	    /* eliminate parent contribution first */
	    link=compts[parentno];
	    row=hnum[parentno];	/* convert to hines number */
	    /* Backwards elimination of a coupled row
	    **   code to execute: results[i] -=  ravals[n++]*results[row];
	    **   with ravals[n++] the right off-diagonal of row i
	    */
/* test2.analysis: i is row 11 or 13 */
	    if (symflag && (self>0)) {
		/* right off-diagonal has changed */
		if (justcount) {
		    nfuncs+=3;
		} else {
/* POSSIBLE OPTIMIZATION: IN UNBRANCHED CABLE ROW = i + 1 */
/* test2.analysis: q13' */
		    funcs[nfuncs++]=SIBARRAY_ELIM;
		    if (ravptr[i][0]>=0) {
			printf("Error in %s: Bug #11 in hines_init %d %d: %d.\n",hsolve->name,i,0,ravptr[i][0]);
			return(ERR);
		    }
		    funcs[nfuncs++]=-ravptr[i][0];
		    funcs[nfuncs++]=2*row;
		}
	    } else {
		/* standard case: compute the right off-diagonal */
		if (justcount) {
		    nfuncs+=2;
		    nravals++;
		} else {
/* POSSIBLE OPTIMIZATION: IN UNBRANCHED CABLE ROW = i + 1 */
		    funcs[nfuncs++]=BACKWARD_ELIM;
		    funcs[nfuncs++]=2*row;
		    if (symflag) { /* symmetric elements */
			/* to parent or sibling: head connection */
/* test2.analysis: P11 (contributed by row 13) */
			ravals[nravals++]=-dt/(Cm*link->Ra*coeffs[i*2]);
		    } else { /* asymmetric element: AXIAL connection */
			ravals[nravals++]=-dt/(Cm*compt->Ra);
		    }
		}
	    }

	    /* symmetric compartments only: eliminate the sibs next */
	    for (j=0; j<nkid; j++) {
		k=kids[parentno][j];
		/* For symmetric comparts the right off-diagonal for parent may
		**   have been changed due to the elimination of a sibling left
		**   off-diagonal element: we compute this first
		*/
		link=compts[k];
		row=hnum[k];	/* convert to hines number */
		if (row<=i) continue;	/* connection to sib done in forward */
		/* Backwards elimination of a coupled row
		**   code to execute: results[i] -=  ravals[n++]*results[row];
		**   with ravals[n++] the right off-diagonal of row i
		*/
/* test2.analysis: i is row 11 */
		if (j>0) {
		    /* right off-diagonal has changed */
		    if (justcount) {
			nfuncs+=3;
		    } else {
/* POSSIBLE OPTIMIZATION: IN UNBRANCHED CABLE ROW = i + 1 */
/* test3.analysis: S8' */
			funcs[nfuncs++]=SIBARRAY_ELIM;
			if (ravptr[i][maxkids+j+1]>=0) {
			    printf("Error in %s: Bug #12 in hines_init %d %d %d: %d.\n",hsolve->name,i,j,maxkids+j+1,ravptr[i][maxkids+j+1]);
			    return(ERR);
			}
			funcs[nfuncs++]=-ravptr[i][maxkids+j+1];
			funcs[nfuncs++]=2*row;
		    }
		} else {
		    /* standard case: compute the right off-diagonal */
		    if (justcount) {
			nfuncs+=2;
			nravals++;
		    } else {
/* POSSIBLE OPTIMIZATION: IN UNBRANCHED CABLE ROW = i + 1 */
			funcs[nfuncs++]=BACKWARD_ELIM;
			funcs[nfuncs++]=2*row;
			/* to parent or sibling: head connection */
/* test2.analysis: s11 (contributed by row 12) */
			ravals[nravals++]=-dt/(Cm*link->Ra*coeffs[i*2]);
		    }
		}
	    }
	    /*
	    **  Compute final result:
	    **   code to execute: results[i] =  results[i+1]/diagonal[i+1];
	    **  For last row this can be done immediately, for others we
	    **   need to eliminate right off-diagonals first
	    */
            if (justcount){
		nfuncs++;
            } else {
                funcs[nfuncs++]=CALC_RESULTS;
            }
	}
        if (justcount){
            nfuncs++;
            hsolve->nfuncs=nfuncs;
            hsolve->nravals=nravals;
        } else {
            funcs[nfuncs++]=FINISH;
	    if (coeffs) free(coeffs);
	    if (diagterms) {
		/* copy to final storage */
		if (hsolve->chanmode>1) {
		    /* store in chip array */
		    for (i=0;i<ncompts;i++) {
			if (i==ncompts-1) {
			    j=hsolve->nchips-1;
			} else {
			    j=compchips[i+1]-1;
			}
			chip[j]=diagterms[i];
		    }
		} else {
		    /* store in diagonals array */
		    for (i=0;i<ncompts;i++) diagonals[i]=diagterms[i];
		}
		free(diagterms);
	    }
	}
        return(0);
}

/* copies diagonals from original chip array to chip array of the duplicate */
void copychipdiags(hsolve)
	Hsolve  *hsolve;
{
	int	i,nchip;
	int     ncompts=hsolve->ncompts;
	double	*chip=hsolve->chip;
	int	*compchips = hsolve->compchips;
	double	*origchip;

	origchip=hsolve->origsolve->chip;
	for (i=0;i<ncompts;i++) {
	    if (i==ncompts-1) {
		nchip=hsolve->nchips-1;
	    } else {
		nchip=compchips[i+1]-1;
	    }
	    chip[nchip]=origchip[nchip];
	}
}
