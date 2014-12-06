static char rcsid[] = "$Id: hines_chip_init.c,v 1.4 2006/01/10 19:56:39 svitak Exp $";

/* Version EDS22h* 99/09/13, Erik De Schutter, Caltech & BBF-UIA 4/92-9/99 */

/*
** $Log: hines_chip_init.c,v $
** Revision 1.4  2006/01/10 19:56:39  svitak
** Changes to allow voltage-dependent Z-gate.
**
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
** Revision 1.14  2003/03/28 21:13:46  gen-dbeeman
** Fix submitted by Hugo Cornelis.  Instantaneous tabchannel gates
** (i.e. those that use the instant field) produced inaccurate results
** under hsolve chanmodes 2-5.
**
** Revision 1.13  2001/06/29 21:14:56  mhucka
** Removed a nested comment to quiet the compiler.
**
** Revision 1.12  2001/04/18 22:39:36  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.10  2001/01/01 01:19:06  mhucka
** Fix from Hugo Cornelis for a bug in hsolve in which Vm is loaded too late
** under sone circumstances.
**
** Revision 1.9  2000/09/21 19:31:27  mhucka
** One of the printfs in h_hh_chip_init() didn't cast an argument properly.
**
** Revision 1.8  2000/09/11 15:49:10  mhucka
** I botched the merger of Hugo's changes on one line.  Now fixed.
**
** Revision 1.7  2000/09/07 02:22:55  mhucka
** Fixes from Hugo Cornelis for various initialization problems.
**
** Revision 1.6  1999/10/18 00:10:31  mhucka
** Additional fixes from Erik De Schutter, for a bug in calculating dt.
**
** Revision 1.5  1999/10/17 23:15:40  mhucka
** Attempted to merge in changes from Greg Hood for fixing problems on
** the Cray T3E.  The same changes also make it possible to compile with
** optimization turned on under Red Hat 6.0's default version of gcc/egcs.
**
** Revision 1.4  1999/10/17 21:39:36  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
** EDS22k revision HC BBF-UIA 00/07/24
** no normalization for incoming ACTIVATION msg's on hsolve'd synchans
** Fix for case when Vm was loaded to late (for Ca channel only with nernst Ek)
**
** EDS22h revison: EDS BBF-UIA 99/07/14-99/09/13
** Corrected error in in tables when dt was changed 
**
** EDS22f revison: EDS BBF-UIA 98/10/28-98/11/23
** Minor bug corrections: Mg_block, taupump
**
** EDS22d revison: EDS BBF-UIA 98/05/04-98/09/30
** Added support for single shells and instant gates
**
** EDS22c revison: EDS BBF-UIA 97/12/04-97/12/05
** Deleted use of values array
** Correct SYNCHAN time normalization
**
** EDS22b revison: EDS BBF-UIA 97/11/17-97/11/25
** Added concpool and hillpump code, reorganized some arrays
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* initializes chip and ops array and for these chanmodes funcs and ravals 
** arrays */
/*
 * 95/06/02 eds
 * removed Vm_flag, do_chip_hreset now calls a compartment RESET to update Vm
 *
 * Revision 1.4  1994/03/22 eds
 * Included error checking code for *power in tabchannel
*/


#include <math.h>
#include "hines_ext.h"

/* Set of routines to make composite table for tab(2D)channels */
/* In hsolve, voltage dependent tables are always indexed as [x], concentration
**  dependent as [y], 2D tables as [x,y] */
static int nctables,nvtables,ncols,nstables;
static double *tablist[MAXTABV];
static int tabindex[MAXTABV],tabcols[MAXTABV],tabintegrate[MAXTABV];
static double stablist[MAXSYN];

void printtaberror(hsolve)
	Hsolve	*hsolve;

{
	Error();
	printf(" during SETUP of %s: all tables must have same size.\n",Pathname(hsolve));
	return;
}


double *tab2Dptr(ip2)
	Interpol2D *ip2;
{
	if (ip2->xdivs==0) {    /* 1D inside 2D */
	    return(ip2->table[0]);
	} else {                /* real 2D */
	    return((double *)ip2->table);
	}

}


/* check_tab() is used to check and store (pointers to) tables */
/* from tabulated elements.  Always called multiple of two times for a */
/* single element : first call for B table, second call for A table. */
/* Additional maintains some tables with metadata describing the type */
/* of table. */
/* tabcols[] : number of columns for table ? */
/* tabintegrate[] : stores flag formal, converts flag == 0 to 1. */

/* ip : interpol table to store/check */
/* hsolve : hsolve being compiled */
/* flag : -2 == table for current or GHK */
/* 	: -1 == instant gate (channel) */
/*	:  0 == normal integration (channel) */
/* must : index into table array, -1 for any */
/* 	: this may only be set for the second table (A table), */
/* 	: never set this for the first table (B table) */
/* type : 1 == 1D, 2 == 2D */
/* index_type : one of VOLT_INDEX, C1_INDEX and related */

/* returns index into table array, -1 for failure. */

int check_tab(ip,hsolve,must,flag,type,index_type)
	Interpol *ip;
	Hsolve 	*hsolve;
	int	must,flag,type,index_type;
{
	double	*table;
	Interpol2D *ip2 = NULL;
	int	ntable,conc,j,xdivs=0,ydivs=0;
	double	xmin,invdx = 0.0,ymin,invdy = 0.0;
	double *tab2Dptr();

	if (!ip) {
	    printf("** Warning - during SETUP of %s: missing tab[2D]channel table.\n",Pathname(hsolve));
	    return(ERR);
	}

	if (type==2) {	/* Interpol2D */
	    ip2=(Interpol2D *)ip;
	    table=tab2Dptr(ip2);
	} else {		/* Interpol */
	    table=ip->table;
	}
	conc=(index_type<VOLT_DOMAIN_INDEX)||(index_type>VOLT_INDEX);
	if (must>0) {
	    if (tablist[must]==table) {
		/* it has already been defined */
		return(must);
	    } else if ((conc&&(must<nctables)) || (!conc&&(must<nvtables))) {
		printf("** Warning - during SETUP of %s: different A-tables for same B-table in tab[2D]channel.\n",Pathname(hsolve));
		return(ERR);
	    }
	} else {
	    for(j=0; j<nvtables; j++) {
		if (tablist[j]==table)
		{
		    /*! this check is important, duplicate_tables() */
		    /*! will recalculate the tables according to this */
		    /*! flag. */

		    if ((flag < 0
			 && flag == tabintegrate[j])
			|| (flag == 0
			    && 1 == tabintegrate[j]))
		    {
			/* it has already been defined */
			return(j);
		    }
		}
	    }
	}
	if ((conc&&(nctables==MAXTABC)) || (!conc&&(nvtables==MAXTABV))) {
	    Error();
	    printf(" during SETUP of %s: too many tab[2D]channel tables.\n",Pathname(hsolve));
	    return(ERR);
	}
	/* store voltage dependent tables separate from C-dependent ones */
	/*  in final storage 1D tables are stored before 2D tables */
	if (conc) {
	    ntable=nctables++;
	} else {	/* C-dependent */
	    ntable=nvtables++;
	}
	/* extract the table sizes */
	ymin=xmin=0;
	if (index_type<=VOLT_INDEX) { 
	    /* 1D volt dependent or 2D */
	    xdivs=ip->xdivs;
	    if ((index_type==VOLT_INDEX)&&(type==2)&&(xdivs==0)) {
		/* 1D table embedded in Interpol2D */
		xdivs=ip2->ydivs;
		xmin=ip2->ymin;
		invdx=ip2->invdy;
	    } else {
		xmin=ip->xmin;
		invdx=ip->invdx;
	    }
	} else if (index_type>=C1_INDEX) {
	    /* 1D concentration dependent */
	    ydivs=ip->xdivs;
	    if ((type==2)&&(ydivs==0)) {
		/* 1D table embedded in Interpol2D */
		ydivs=ip2->ydivs;
		ymin=ip2->ymin;
		invdy=ip2->invdy;
	    } else {
		ymin=ip->xmin;
		invdy=ip->invdx;
	    }
	}
	if (xdivs && ydivs && (xdivs!=ydivs)) {
	    printtaberror(hsolve);
	    printf("  xdivs=%d while ydivs=%d\n",xdivs,ydivs);
	    return(ERR);
	}
	if (index_type<=VOLT_C1_INDEX) {
	    /* 2D concentration dependent */
	    if (ip2->ydivs==0) {
		Error();
		printf(" during SETUP of %s: tab2Dchannel ydivs must be larger than zero.\n",Pathname(hsolve));
		return(ERR);
	    }
	    ydivs=ip2->ydivs;
	    ymin=ip2->ymin;
	    invdy=ip2->invdy;
	}
	if (hsolve->xdivs == 0) {
	    hsolve->xdivs=xdivs;
	    hsolve->xmin=xmin;
	    hsolve->invdx=invdx;
	} else if (xdivs) {
	    if ((xdivs) != hsolve->xdivs) {
		printtaberror(hsolve);
		printf("  hsolve xdivs=%d is != ip divs=%d.\n",hsolve->xdivs,xdivs);
		return(ERR);
	    }
	    if ((xmin) != hsolve->xmin) {
		printtaberror(hsolve);
		printf("  hsolve xmin=%f is != ip min=%f\n",hsolve->xmin,xmin);
		return(ERR);
	    }
	    if ((invdx) != hsolve->invdx) {
		printtaberror(hsolve);
		printf("  hsolve invdx=%f is != ip invd=%f\n",hsolve->invdx,invdx);
		return(ERR);
	    }
	}
	if (hsolve->ydivs == 0) {
	    hsolve->ydivs=ydivs;
	    hsolve->ymin=ymin;
	    hsolve->invdy=invdy;
	} else if (ydivs) {
	    if ((ydivs) != hsolve->ydivs) {
		printtaberror(hsolve);
		printf("  hsolve ydivs=%d is != ip divs=%d.\n",hsolve->ydivs,ydivs);
		return(ERR);
	    }
	    if ((ymin) != hsolve->ymin) {
		printtaberror(hsolve);
		printf("  hsolve ymin=%f is != ip min=%f\n",hsolve->ymin,ymin);
		return(ERR);
	    }
	    if ((invdy) != hsolve->invdy) {
		printtaberror(hsolve);
		printf("  hsolve invdy=%f is != ip invd=%f\n",hsolve->invdy,invdy);
		return(ERR);
	    }
	}
	if (type==2) {   			/* Interpol2D */
	    if (ip2->xdivs==0) {	/* 1D inside 2D */
		j=1;
	    } else {				/* real 2D */
		j=ip2->ydivs+1;
	    }
	} else {					/* Interpol */
	    j=1;
	}
	tablist[ntable]=table;
	tabcols[ntable]=j;
	if (flag<0) {	/* TABCURR_T */
	    tabintegrate[ntable]=flag;
	} else {		/* TABCHAN_T or TAB2CHAN_T */
	    tabintegrate[ntable]=1;
	}
	ncols+=j;
	return(ntable);
}

int check_chanX(chan,hsolve,type)
	Element	*chan;
	Hsolve	*hsolve;
	int	type;
{
	int	ia = 0,ib,must;
	Tchan	*tchan;
	T2chan	*t2chan;

	tchan=(Tchan *)chan;
	if (tchan->instant & INSTANTX) {
	    must=-1;	/* instant gate */
	} else {
	    must=0;	/* normal integration */
	}
	if ((tchan->Xpower<1) || (tchan->Xpower>6)) {
	    Error();
	    printf(" during SETUP of %s: Xpower of %s should be in range 1-6 for chanmode >= 2.\n",Pathname(hsolve),Pathname(chan));
	    return(ERR);
	}
	if (type==TABCHAN_T) {
	    ib=check_tab(tchan->X_B,hsolve,-1,must,1,VOLT_INDEX);
	    if (ib!=ERR) ia=check_tab(tchan->X_A,hsolve,ib+1,must,1,VOLT_INDEX);
	} else {
	    t2chan=(T2chan *)chan;
	    ib=check_tab(t2chan->X_B,hsolve,-1,must,2,t2chan->Xindex);
	    if (ib!=ERR) ia=check_tab(t2chan->X_A,hsolve,ib+1,must,2,t2chan->Xindex);
	}
	if ((ib==ERR) || (ia==ERR)) {
	    Error();
	    printf(" during SETUP of %s: failed to set up X table of %s.\n",Pathname(hsolve),Pathname(chan));
	    return(ERR);
	}
	return(0);
}

int check_chanY(chan,hsolve,type)
	Element	*chan;
	Hsolve	*hsolve;
	int	type;
{
	int	ia = 0,ib,must;
	Tchan	*tchan;
	T2chan	*t2chan;

	tchan=(Tchan *)chan;
	if (tchan->instant & INSTANTY) {
	    must=-1;	/* instant gate */
	} else {
	    must=0;	/* normal integration */
	}
	if ((tchan->Ypower<1) || (tchan->Ypower>6)) {
	    Error();
	    printf(" during SETUP of %s: Ypower of %s should be in range 1-6 for chanmode >= 2.\n",Pathname(hsolve),Pathname(chan));
	    return(ERR);
	}
	if (type==TABCHAN_T) {
	    ib=check_tab(tchan->Y_B,hsolve,-1,must,1,VOLT_INDEX);
	    if (ib!=ERR) ia=check_tab(tchan->Y_A,hsolve,ib+1,must,1,VOLT_INDEX);
	} else {
	    t2chan=(T2chan *)chan;
	    ib=check_tab(t2chan->Y_B,hsolve,-1,must,2,t2chan->Yindex);
	    if (ib!=ERR) ia=check_tab(t2chan->Y_A,hsolve,ib+1,must,2,t2chan->Yindex);
	}
	if ((ib==ERR) || (ia==ERR)) {
	    Error();
	    printf(" during SETUP of %s: failed to set up Y table of %s.\n",Pathname(hsolve),Pathname(chan));
	    return(ERR);
	}
	return(0);
}

int check_chanZ(chan,hsolve,type)
	Element	*chan;
	Hsolve	*hsolve;
	int	type;
{
	int	ia = 0,ib,must;
	Tchan	*tchan;
	T2chan	*t2chan;
	int index_type;

	tchan=(Tchan *)chan;
	if (tchan->instant & INSTANTZ) {
	    must=-1;	/* instant gate */
	} else {
	    must=0;	/* normal integration */
	}
	if ((tchan->Zpower<1) || (tchan->Zpower>6)) {
	    Error();
	    printf(" during SETUP of %s: Zpower of %s should be in range 1-6 for chanmode >= 2.\n",Pathname(hsolve),Pathname(chan));
	    return(ERR);
	}
	if (type==TABCHAN_T) {
	    index_type = tchan->Z_conc ? C1_INDEX : VOLT_INDEX;
	    ib=check_tab(tchan->Z_B,hsolve,-1,must,1,index_type);
	    if (ib!=ERR) ia=check_tab(tchan->Z_A,hsolve,ib+1,must,1,index_type);
	} else {
	    t2chan=(T2chan *)chan;
	    ib=check_tab(t2chan->Z_B,hsolve,-1,must,2,t2chan->Zindex);
	    if (ib!=ERR) ia=check_tab(t2chan->Z_A,hsolve,ib+1,must,2,t2chan->Zindex);
	}
	if ((ib==ERR) || (ia==ERR)) {
	    Error();
	    printf(" during SETUP of %s: failed to set up Z table of %s.\n",Pathname(hsolve),Pathname(chan));
	    return(ERR);
	}
	return(0);
}

int check_stab(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	int	j;
	S2chan	*schan;

	schan=(S2chan *)hsolve->children[cindex];
	if (!schan)
	    return(ERR);

	for(j=0;j<nstables*SYNSIZE;j+=SYNSIZE) {
	    if ((fabs(stablist[j] - schan->xconst2) < VTINY) &&
		(fabs(stablist[j+1] - schan->yconst1) < VTINY) &&
		(fabs(stablist[j+2] - schan->yconst2) < VTINY)) {
		if ((fabs(stablist[j+3] - schan->xconst1) < VTINY) || 
		    (fabs(stablist[j+3] - schan->xconst1/hsolve->dt) < VTINY)) {
		    /* it has already been entered */
		    return(j);
		}
	    }
	}
	if (nstables*SYNSIZE == MAXSYN) {
	    Error();
	    printf(" during SETUP of %s: too many synaptic channel types.\n",Pathname(hsolve));
	    return(ERR);
	} 
	stablist[j] = schan->xconst2;
	stablist[j+1] = schan->yconst1;
	stablist[j+2] = schan->yconst2;
	if (hsolve->childtypes[cindex]==SYNCHANC_T) {
	    /* normalize to step size by dividing by dt */
	    stablist[j+3] = schan->xconst1 / hsolve->dt;
	} else {
	    stablist[j+3] = schan->xconst1;
	}
	nstables++;
	return(j);
}

int h_hh_chip_init(hsolve)
/* Creates the chip and ops arrays and fills the ops array.
** Also finds out the tablist and stablist layout.  */
	Hsolve	*hsolve;
{
	int 	ncompts=hsolve->ncompts;
	int 	nchildren=hsolve->nchildren;
	Element	**compts,*compt,**children,*child;
	int 	i,j,k,l = 0,m,n;
	int * piCompFlags = hsolve->piCompFlags;
	short   *childtypes;
	int	*childstart,*childcode,*childmsg,*childpos,*childnames,*childlink;
	int	ct,cm,cl,cp,cc;
	float	*childdata;
	int	*elmnum,*compchips,*compops,*childops = NULL,*childchips = NULL;
	int	nchip,nop;
	int     lastconc,voltindex,lastc1index,lastc2index;
	float	Ek,lastEk;
	int     gindex = 0,gpower = 0;
	double	*chip;
	int	*ops;
	int	storeall;
	Tchan   *tchan = NULL;
	T2chan  *t2chan = NULL;
	Tcurr   *tcurr = NULL;
	double	*table = NULL;
	S2chan	*schan;
	double	*tab2Dptr();

	compts=hsolve->compts;
	children=hsolve->children;
	elmnum=hsolve->elmnum;
	childtypes=hsolve->childtypes;
	childcode=hsolve->childcode;
	childmsg=hsolve->childmsg;
	childlink=hsolve->childlink;
	childpos=hsolve->childpos;
	childdata=hsolve->childdata;
	childstart=hsolve->childstart;
	childnames=hsolve->childnames;
	storeall=hsolve->storemode;

	if (storeall) {
	    if (hsolve->no_elminfo) {
		printf("** Warning - during SETUP of %s: no_elminfo should be false if storemode is on.\n",Pathname(hsolve));
		hsolve->storemode=0;
		storeall=0;
	    }
	    if (hsolve->chanmode<4) {
		printf("** Warning - during SETUP of %s: chanmode should be 4 if storemode is on.\n",Pathname(hsolve));
		hsolve->storemode=0;
		storeall=0;
	    }
	} else if (storeall>2) {
	    printf("** Warning - during SETUP of %s: storemode should be in range 0-2.\n",Pathname(hsolve));
	    storeall=0;
	}

	hsolve->ntab=0;
	hsolve->sntab=0;
	nctables=ncols=nstables=0;
	nvtables=MAXTABC;
	for(i=0;i<MAXTABV;i++) {
	    tablist[i]=(double *)NULL;
	    tabcols[i]=0;
	}
	for(i=0;i<MAXSYN;i++) stablist[i]=0.0;

/* First find the number of elements that the chip and ops arrays will
**   have to deal with.  
*/
	nop=1;	/* LCOMPT_OP */
	nchip=0;
	lastconc=-1;    /* none selected, may be shared between compts  */
	for (i=0;i<ncompts;i++) {
	    nop++;		/* COMPT_OP or LCOMPT_OP */
	    nchip+=4;	/* tybc, diagterm, Em/Rm and inject */
	    n=childstart[i];
	    voltindex=-1;    /* none selected */
	    for (j=n; j<childstart[i+1]; j++) {
		child=children[j];
		ct=childtypes[j];
		cc=childcode[j];
		cm=childmsg[j];
		cl=childlink[j];
		cp=childpos[j];
		/* make space for CONC_VAL_OP and GET_CONC_OP if necessary */
		switch (ct) {
		    case TABCHAN_T:
		    case TAB2CHAN_T:
		    case TABCURR_T:
		    case TABGHK_T:
			if (cp!=0) {		/* CONCEN msg */
			    k=abs(cp);
			    k=childmsg[k];
			    if (k!=lastconc) {
				nop+=2;		/* CONC_VAL_OP and index */
				lastconc=k;
			    }
			}
			/* no break !*/

		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
		    case MGBLOCK_T:
			if (cl!=0) {		/* flux */
			    k=childmsg[abs(cl)];
			    if (k!=lastconc) {
				nop+=2;	  /* CONC_VAL_OP and index */
				lastconc=k;
			    }
			}
			break;

		    case NERNST_T:
		    case GHK_T:
		    case ELMMPUMP_T:
			if (cm!=lastconc) {
			    nop+=2;		/* CONC_VAL_OP and index */
			    lastconc=cm;
			}
			break;

		    default:
			break;
		}
		switch (ct) {
		    case SPIKEGEN_T:
			nop+=3;		/* SPIKE_OP,counter,refractory period */
			nchip++;	/* voltage threshold */
			break;

		    case ELMMPUMP_T:
			nop++;		/* ELMMPUMP_OP */
			nchip+=3;	/* Kd, dt.vmax.surf.val.F, dt.vmax.surf/vol */
			if (storeall==1)  nop+=2;  /* STORE_ALLI_OP and index */
			break;

		    case TABCHAN_T:
		    case TAB2CHAN_T:
		    case TABCURR_T:
		    case TABGHK_T:
		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
			if ((cm!=0)&&(abs(cm)!=j-1)) {
			    /* no Ek setting child before -> we need a gmax */
			    nchip++;	/* gmax */
			    nop++;		/* SAME_GHK_OP or CHAN_OP */
			}
			if (cm==0) {
			    if ((ct!=TABGHK_T)&&(ct!=TABCURR_T)) {
				nchip+=2;	/* Ek and gmax */
			    } else {
				nchip++;	/* gmax */
			    }
			    nop++;		/* CHAN_EK_OP */
			}
			if (ct==TABCHAN_T) {
			    nop++;		/* ADD_CURR_OP */
			    if (cc<0) nchip++;	/* DOMAINCONC scale factor */
			    tchan=(Tchan *)child;
			    if (tchan->X_alloced) {
				nchip++;	/* X */
				nop+=4;     /* worst case: NEWVOLT_OP,IPOL1V_OP,index,Xpower */
				if (check_chanX(child,hsolve,ct))  return(ERR);
			    }
			    if (tchan->Y_alloced) {
				nchip++;	/* Y */
				nop+=4;     /* worst case: NEWVOLT_OP,IPOL1V_OP,index,Ypower */
				if (check_chanY(child,hsolve,ct))  return(ERR);
			    }
			    if (tchan->Z_alloced) {
				nchip++;	/* Z */
				nop+=4;     /* worst case: NEWCONC1_OP,IPOL1C_OP,index,Zpower */
				if (check_chanZ(child,hsolve,ct))  return(ERR);
			    }
			} else if (ct==TAB2CHAN_T) {
			    nop++;		/* ADD_CURR_OP */
			    if (cc<0) nchip++; /* DOMAINCONC scale factor */
			    t2chan=(T2chan *)child;
			    /* assume worst case for nop:
			    ** C1_C2_INDEX: NEWCONC1_OP,NEWCONC2_OP,conc,IPOL2C_OP,index,power */
			    if (t2chan->X_alloced) {
				nchip++;	/* X */
				nop+=6;
				if (check_chanX(child,hsolve,ct))  return(ERR);
			    }
			    if (t2chan->Y_alloced) {
				nchip++;	/* Y */
				nop+=6;
				if (check_chanY(child,hsolve,ct))  return(ERR);
			    }
			    if (t2chan->Z_alloced) {
				nchip++;	/* Z */
				nop+=6;
				if (check_chanZ(child,hsolve,ct))  return(ERR);
			    }
			} else if ((ct==TABCURR_T)||(ct==TABGHK_T)) {
			    if (cc<0) nchip++;	/* DOMAINCONC scale factor */
			    tcurr=(Tcurr *)child;
			    /* assume worst case for nop:
			    ** C1_C2_INDEX: NEWCONC1_OP,NEWCONC2_OP,conc,IPOL2C_OP,index,power */
			    if (tcurr->alloced) {
				nop+=5;
				k=check_tab(tcurr->G_tab,hsolve,-1,-2,2,tcurr->Gindex);
				if (k==ERR)
				{
				    return(ERR);
				}
				else
				{
				    if (check_tab(tcurr->I_tab,hsolve,k+1,-2,2,tcurr->Gindex)==ERR)
				    {
					return(ERR);
				    }
				}
			    }
			} else { /* CHANC2_T, CHANC3_T and SYNCHANC_T */
			    schan=(S2chan *)child;
			    k=check_stab(hsolve,j);
			    if (k<0) {
				return(ERR);
			    } else {
				childpos[j]=k;	/* temporary store for index */
			    }
			    nop+=4;	/* SYN2/3_OP,stablist_index,counter,child_ind */
			    if (ct==CHANC2_T) {
				nchip+=2;		/* X and Y */
			    } else if (schan->frequency > TINY) {
				nchip+=3;		/* freq, X and Y */
			    } else {
				nchip+=2;		/* X and Y */
			    }
			    if (childtypes[j+1]==MGBLOCK_T) {
				nchip+=3;	/* KMg_A 1/KMg_B and CMg */
				nop++;		/* MGBLOCK_OP */
				cl=childlink[j+1]; /* for flux from Mg_block */
			    }
			}
			if ((cl!=0)&&(ct!=TABGHK_T)) {        /* flux */
			    if (cl>0) {		/* simple flux */
				nop++;		/* FLUX_OP */
			    } else if (cl<0) {	/* scaled flux */
				nop++;		/* SCALE_FLUX_OP */
				nchip++;	/* flux_scale */
			    }
			}
			if (storeall)  nop+=2;	/* STORE_ALLC/I_OP and index */
			break;

		    case MGBLOCK_T:
			/* done before in CHANC[23]_T and SYNCHANC_T code */
			break;

		    case NERNST_T:
			nchip+=2;	/* constant and gmax */
			nop++;		/* NERNST_OP */
			break;

		    case GHK_T:
			nchip+=3;	/* zF/RT, -ZF, and gmax */
			nop++;		/* GHK_OP */
			break;
		    
		    default:
			break;
		}
	    }
	}

	hsolve->ntab=nctables+nvtables-MAXTABC;
	hsolve->sntab=nstables;
	hsolve->nchips=nchip;
	hsolve->nops=nop;

	if (duplicate_tables(hsolve)) return (ERR);
	if (hsolve->ntab==0) {
	    hsolve->invdx=0;
	    hsolve->xmin=0;
	    hsolve->invdy=0;
	    hsolve->ymin=0;
	}

	chip = hsolve->chip=(double *)calloc(nchip,sizeof(double));
	ops = hsolve->ops=(int *)calloc(nop,sizeof(int));
	if (!hsolve->no_elminfo) {
	    if (storeall) hsolve->itotal=(double *)calloc(hsolve->nelm_names,sizeof(double));
	}
	compchips=hsolve->compchips=(int *)calloc(ncompts,sizeof(int));
	compops=hsolve->compops=(int *)calloc(ncompts,sizeof(int));
	if (hsolve->nchildren) {
	    childops=hsolve->childops=(int *)calloc(nchildren+1,sizeof(int));
	    childchips=hsolve->childchips=(int *)calloc(nchildren+1,sizeof(int));
	}
	/* copy ops and chips arrays */
/* This second loop fills up the arrays allocated above */
	nop=nchip=0;
	lastEk=1.0e30;
	lastconc=-1;    /* none selected, may be shared between compts  */
	for (i=0;i<ncompts;i++) {
	    compt = compts[elmnum[i]];
	    compops[i]=nop;		/* needed in a lot of places */
	    compchips[i]=nchip;	/* needed in a lot of places */
	    n=childstart[i];
	    /* do first passive compartment structure (is also the boundary) */
	    h_check_msgs(hsolve,compt,COMPT_OP,COMPT_T,i,nop,nchip);
	    if (h_has_output(compt)) h_store_out(hsolve,COMPT_T,i,0);
	    if (i==0) {
		ops[nop]=FCOMPT_OP;
		nop++;
	    } else {
		ops[nop]=COMPT_OP;
		nop++;
	    }
	    nchip+=2;
	    voltindex=-1;    /* none selected */
	    lastc1index=lastc2index=0;

	    /*- if this compartment has voltage dependent gates */

	    if (piCompFlags && piCompFlags[i])
	    {
		/*- operations to compute voltage index */

		ops[nop] = NEWVOLT_OP;
		nop++;

		/*- remember volt index (is index of comp) */

		voltindex = i;
	    }

	    for (j=n; j<childstart[i+1]; j++) {
		child=children[j];
		ct=childtypes[j];
		cc=childcode[j];
		cm=childmsg[j];
		cl=childlink[j];
		cp=childpos[j];
		if (h_has_output(child)) h_store_out(hsolve,ct,i,j);
		/* store childops and childchips for any non-channel 
		** child and insert CONC_VAL_OP if we need to access 
		** a new conc */
		switch (ct) {
		    case TABCHAN_T:
		    case TAB2CHAN_T:
		    case TABCURR_T:
		    case TABGHK_T:
			if (cp!=0) {
			    k=childmsg[abs(cp)];
			    if (k!=lastconc) {
				ops[nop]=CONC_VAL_OP;
				ops[nop+1]=k;
				nop+=2;
				lastconc=k;
			    }
			}
			break;

		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
			break;

		    case NERNST_T:
		    case GHK_T:
		    case ELMMPUMP_T:
			if (cm!=lastconc) {
			    ops[nop]=CONC_VAL_OP;
			    ops[nop+1]=cm;
			    nop+=2;
			    lastconc=cm;
			}
			/* no break ! */

		    default:
			childops[j]=nop;
			childchips[j]=nchip;
			break;
		}
		/* fill ops array */
		switch (ct) {
		    case SPIKEGEN_T:
			h_check_msgs(hsolve,child,SPIKE_OP,ct,i,nop,nchip);
			ops[nop]=SPIKE_OP;
			nop+=3;		/* SPIKE_OP, counter, refractory period */
			nchip++;	/* voltage threshold */
			break;

		    case ELMMPUMP_T:
			h_check_msgs(hsolve,child,MMPUMP_OP,ct,i,nop,nchip);
			ops[nop]=ELMMPUMP_OP;
			nop++;		/* ELMMPUMP_OP */
			nchip+=3;	/* Kd, dt.vmax.surf.val.F, dt.vmax.surf/vol */
			if (storeall==1) {
				ops[nop]=STORE_ALL_OP+storeall;
				ops[nop+1]=childnames[j];
				nop+=2;
			}
			break;

		    case TABCHAN_T:
		    case TAB2CHAN_T:
		    case TABCURR_T:
		    case TABGHK_T:
		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
			/* determine indexing of tabulated channels first */
			if ((ct<=TABCURR_T) || (ct==TABGHK_T)) {
			    tchan=(Tchan *)child;
			    if (ct>=TABCURR_T) {
				    tcurr=(Tcurr *)child;
				    l=1;
			    } else if (ct==TAB2CHAN_T) {
				    t2chan=(T2chan *)child;
				    l=3;
			    } else {
				    l=3;
			    }
			    /* cycle through the X,Y,Z gates */
			    for (m=0; m<l; m++) {
				/* determine type of gate */
				switch (m) {
				    case 0:   /* X gate */
					if (ct>=TABCURR_T) {	/* TABCURR_T or TABGHK_T */
					    gindex=tcurr->Gindex;
					} else {
					    if (!tchan->X_alloced) continue;
					    if (ct==TAB2CHAN_T) {
						gindex=t2chan->Xindex;
					    } else {
						gindex=VOLT_INDEX;
					    }
					}
					break;
				    case 1:   /* Y gate */
					if (!tchan->Y_alloced) continue;
					if (ct==TAB2CHAN_T) {
					    gindex=t2chan->Yindex;
					} else {
					    gindex=VOLT_INDEX;
					}
					break;
				    case 2:   /* Z gate */
					if (!tchan->Z_alloced) continue;
					if (ct==TAB2CHAN_T) {
					    gindex=t2chan->Zindex;
					} else {
					  if (tchan->Z_conc) {
					    if (cc<0) {
						gindex=DOMAIN_INDEX;
					    } else {
						gindex=C1_INDEX;
					    }
					  } else {
					    gindex = VOLT_INDEX;
					  }
					}
					break;
				}
				switch (gindex) {
				    case DOMAIN_INDEX:
				    case VOLT_DOMAIN_INDEX:
				    case DOMAIN_C2_INDEX:
					if (j!=(1-cc)) {
					    printf("Error in %s: Bug #4 in h_hh_chip_init %d.\n",hsolve->name,j);
					    return(ERR);
					}
					break;
				    default:
					break;
				}
				/* do voltage first: no checks required */
/* 				switch (gindex) { */
/* 				    case VOLT_INDEX: */
/* 				    case VOLT_C1_INDEX: */
/* 				    case VOLT_C2_INDEX: */
/* 				    case VOLT_DOMAIN_INDEX: */
/* 					if (voltindex!=i) {  */
/* 					    / * will be done once for all gates* */
/* 					    ops[nop]=NEWVOLT_OP; */
/* 					    nop++; */
/* 					    voltindex=i; */
/* 					} */
/* 					break; */
				    
/* 				    default: */
/* 					break; */
/* 				} */
				/* do C1 concentration second:checks required */
				switch (gindex) {
				    case VOLT_INDEX:
				    case C2_INDEX:
				    case VOLT_C2_INDEX:
					break;	/* done elsewhere */

				    case C1_INDEX:
				    case VOLT_C1_INDEX:
				    case C1_C2_INDEX:
					if (lastc1index<0) {
					    Error();
					    printf(" during SETUP of %s: cannot handle %s because it combines DOMAIN_INDEX and C1_INDEX modes.\n",Pathname(hsolve),Pathname(child));
					    return(ERR);
					} else if (!lastc1index) {
					    ops[nop]=NEWCONC1_OP;
					    nop++;
					    lastc1index=1;
					}
					break;

				    case DOMAIN_INDEX:
				    case VOLT_DOMAIN_INDEX:
				    case DOMAIN_C2_INDEX:
					if (lastc1index>0) {
					    Error();
					    printf(" during SETUP of %s: cannot handle %s because it combines DOMAIN_INDEX and C1_INDEX modes.\n",Pathname(hsolve),Pathname(child));
					    return(ERR);
					} else if (!lastc1index) {
					    ops[nop]=DOMAIN_OP;
					    nop++;
					    chip[nchip]=childdata[j]; /* scaling factor*/
					    nchip++;
					    lastc1index=-1;
					}
					break;
				    default:
					Error();
					printf(" during SETUP of %s: cannot handle %s because it uses unknown index mode %d.\n",Pathname(hsolve),Pathname(child),gindex);
					return(ERR);
				}
				/* do C2 concentration last:checks required */
				switch (gindex) {
				    case C2_INDEX:
				    case VOLT_C2_INDEX:
				    case C1_C2_INDEX:
					if (!lastc2index) {
					    ops[nop]=NEWCONC2_OP;
					    ops[nop+1]=childmsg[cc];
					    nop+=2;
					    lastc2index=1;
					}
					break;

				    case DOMAIN_C2_INDEX:
					ops[nop]=NEWCONC2_OP;
					ops[nop+1]=childmsg[cp];
					nop+=2;
					break;

				    default:
					break;
				}
			    } /* for */
			    if (voltindex && lastc1index && lastc2index) {
				Error();
				printf(" during SETUP of %s: cannot handle %s because it uses both a VOLT_C1_INDEX and a VOLT_C2_INDEX index mode.\n",Pathname(hsolve),Pathname(child));
				return(ERR);
			    }
			}

			/* determine source of Ek */
			if (cm==0) { /* define Ek */
			    Ek=((Tchan *)child)->Ek;
			    if (fabs(Ek-lastEk)<VTINY) {
				ops[nop]=CHAN_OP;
				nop++;
				nchip++;    /* gmax */
			    } else if (ct==TABCURR_T) {
				ops[nop]=CHAN_OP;
				nop++;
				nchip++;    /* gmax */
				lastEk=1.0e30;	/* Ek is erased */
			    } else if (ct==TABGHK_T) {
				ops[nop]=DO_TABGHK_OP;
				nop++;
				lastEk=1.0e30;	/* Ek is erased */
			    } else {
				ops[nop]=CHAN_EK_OP;
				nop++;
				nchip+=2;   /* Ek and gmax */
				lastEk=Ek;
			    }
			} else if ((abs(cm)!=j-1)||(childtypes[j-1]==TABGHK_T)) {	/* use Nernst or GHK Ek */
			    lastEk=1.0e30;
			    if (cm<0) {
				ops[nop]=SAME_GHK_OP;
				nop++;
				nchip++;    /* gmax */
			    } else {
				ops[nop]=CHAN_OP;
				nop++;
				nchip++;    /* gmax */
			    }
			}
			childops[j]=nop;	/* point after CHAN_OP etc */
			childchips[j]=nchip;
			/* determine table pointer for tabulated channels */
			if ((ct<=TABCURR_T) || (ct==TABGHK_T)) {
			    if (ct<TABCURR_T) {
				h_check_msgs(hsolve,child,CHAN_OP,ct,i,nop,nchip);
			    } else {
				h_check_msgs(hsolve,child,TABCURR_OP,ct,i,nop,nchip);
			    }
			    /* cycle through the X,Y,Z gates */
			    for (m=0; m<l; m++) {
				/* determine type of gate */
				switch (m) {
				    case 0:   /* X gate */
					if ((ct==TABCURR_T) || (ct==TABGHK_T)) {
					    gindex=tcurr->Gindex;
					    table=tab2Dptr(tcurr->G_tab);
					    gpower=TABCURR_OP;
					} else {
					    if (!tchan->X_alloced) continue;
					    if (ct==TAB2CHAN_T) {
						gindex=t2chan->Xindex;
						table=tab2Dptr(t2chan->X_B);
					    } else {
						gindex=VOLT_INDEX;
						table=tchan->X_B->table;
					    }
					    gpower=(int)(tchan->Xpower+1e-6);
					    if (tchan->instant & INSTANTX)
							    gpower=-gpower;
					}
					break;
				    case 1:   /* Y gate */
					if (!tchan->Y_alloced) continue;
					if (ct==TAB2CHAN_T) {
					    gindex=t2chan->Yindex;
					    table=tab2Dptr(t2chan->Y_B);
					} else {
					    gindex=VOLT_INDEX;
					    table=tchan->Y_B->table;
					}
					gpower=(int)(tchan->Ypower+1e-6);
					if (tchan->instant & INSTANTY)
							    gpower=-gpower;
					break;
				   case 2:   /* Z gate */
					if (!tchan->Z_alloced) continue;
					if (ct==TAB2CHAN_T) {
					    gindex=t2chan->Zindex;
					    table=tab2Dptr(t2chan->Z_B);
					} else {
					    gindex= tchan->Z_conc ? C1_INDEX : VOLT_INDEX;
					    table=tchan->Z_B->table;
					}
					gpower=(int)(tchan->Zpower+1e-6);
					if (tchan->instant & INSTANTZ)
							    gpower=-gpower;
					break;
				}
				switch (gindex) {
				    case VOLT_INDEX:
					ops[nop]=IPOL1V_OP;
					nop++;
					break;
				    case C1_INDEX:
				    case C2_INDEX:
				    case DOMAIN_INDEX:
					ops[nop]=IPOL1C_OP;
					nop++;
					break;
				    case VOLT_C1_INDEX:
				    case VOLT_C2_INDEX:
				    case VOLT_DOMAIN_INDEX:
					ops[nop]=IPOL2V_OP;
					nop++;
					break;
				    case C1_C2_INDEX:
				    case DOMAIN_C2_INDEX:
					ops[nop]=IPOL2C_OP;
					nop++;
					break;
				}
				for(k=0;k<nvtables;k++) {
				    if (tablist[k]==table) {
					ops[nop]=tabindex[k];
					break;
				    }
				}
#ifdef LONGWORDS
				if (k==nvtables) printf("Error in %s: Bug #5 in h_hh_chip_init %ld.\n",hsolve->name, (ADDR) table);
#else
				if (k==nvtables) printf("Error in %s: Bug #5 in h_hh_chip_init %d.\n",hsolve->name, (ADDR) table);
#endif
				ops[nop+1]=gpower;
				nop+=2;
				if (ct<TABCURR_T) nchip++;	/* X, Y or Z value */
			    }	/* for */
			    if ((ct!=TABCURR_T)&&(ct!=TABGHK_T)) {		/* done automatically for CHANC2_T, CHANC3_T and SYNCHANC_T */
				ops[nop]=ADD_CURR_OP;
				nop++;
			    }
			/* determine pointers for synchans */
			} else { /* CHANC2_T, CHANC3_T and SYNCHANC_T */
			    schan=(S2chan *)child;
			    if (ct==CHANC2_T) {
				ops[nop]=SYN2_OP;
			    } else if (schan->frequency > TINY) {
				ops[nop]=SYN3_OP;
				nchip++;        /* freq */
			    } else {
				ops[nop]=SYN2_OP;
			    }
			    ops[nop+1]=cp;	/* stablist index */
			    ops[nop+2]=-1;	/* initialize as no-event */
			    ops[nop+3]=j;	/* pointer to child */
			    h_check_msgs(hsolve,child,SYN2_OP,ct,i,nop+1,nchip);
			    nop+=4;	/* SYN2/3_OP,stablist_index,counter,child_ind */
			    nchip+=2;       /* X and Y */
			    if (childtypes[j+1]==MGBLOCK_T) {
				h_check_msgs(hsolve,children[j+1],MGBLOCK_OP,MGBLOCK_T,i,nop,nchip);
				childops[j+1]=nop;
				childchips[j+1]=nchip;
				ops[nop]=MGBLOCK_OP;
				nop++;
				nchip+=3;   /* KMg_A 1/KMg_B and CMg */
				j++;
				cl=childlink[j]; /* for flux from Mg_block */
			    }
			}
			/* take care of flux storage */
			if ((cl!=0)&&(ct!=TABGHK_T)) {       
			    k=childmsg[abs(cl)];
			    if (k!=lastconc) {
				ops[nop]=CONC_VAL_OP;
				ops[nop+1]=k;
				nop+=2;
				lastconc=k;
			    }
			    if (cl>0) {     /* simple flux */
				ops[nop]=FLUX_OP;
				nop++;
			    } else {	/* scaled flux */
				ops[nop]=SCALE_FLUX_OP;
				nop++;
				chip[nchip]=childdata[j];	/* flux_scale */
				nchip++;
			    }
			}
			/* take care of summated currents/conductances */
			if (storeall>0) {
			    ops[nop]=STORE_ALL_OP+storeall;
			    ops[nop+1]=childnames[j];
			    nop+=2;
			}
			break;
		
		    case NERNST_T:
			h_check_msgs(hsolve,child,NERNST_OP,ct,i,nop,nchip);
			ops[nop]=NERNST_OP;
			nop++;
			nchip+=2;	/* constant and gmax */
			lastEk=1.0e30;	/* Ek is erased */
			break;

		    case GHK_T:
			h_check_msgs(hsolve,child,GHK_OP,ct,i,nop,nchip);
			ops[nop]=GHK_OP;
			nop++;
			nchip+=3;	/* zF/RT, -ZF and gmax */
			lastEk=1.0e30;	/* Ek is erased */
			break;

		    default:
			break;
		}
	    }	/* j loop */
	    nchip+=2;	/* tybc and diagterm fields for COMPT_OP or LCOMPT_OP */
	}	/* i loop */
	ops[nop]=LCOMPT_OP;

	/*- free compartment flags */

	if (hsolve->piCompFlags)
	{
	    free(hsolve->piCompFlags);

	    hsolve->piCompFlags = NULL;
	}

	if (nop>=hsolve->nops) {
		printf("Error in %s: Bug #1 in h_hh_chip_init %d.\n",hsolve->name,nop);
	}
	if (nchip>hsolve->nchips) {
		printf("Error in %s: Bug #2 in h_hh_chip_init %d.\n",hsolve->name,nchip);
	}
	/* hines_data needs correct chip number */
	hsolve->nchips=nchip;
	return(0);
}

int duplicate_tables(hsolve)
	Hsolve  *hsolve;
{
	int i,j,k,l,n;
	int tabsize;
	double	x,dx;
	double  dt,tby2,Gk;
	double	*table,*htable,*gtable,**atable2,**btable2,*xvals;

/* Here we duplicate the interpolation tables so that
** far memory accesses are minimized, and cache use maximized.  */
/* We use the trapezoidal rule to compute rate factors */
	dt = hsolve->dt;
	tby2 = dt/2.0;
	hsolve->tables=(double **)calloc(nvtables,sizeof(double *));
	hsolve->tabcols=(int *)calloc(nvtables,sizeof(int));
	hsolve->tabinteg=(int *)calloc(nvtables,sizeof(int));
	if (hsolve->ntab) {
	    hsolve->ncols=ncols;
	    if (hsolve->readflag>=HSETUP_T) {	/* copy and store the data */
		tabsize=hsolve->xdivs+1;
		htable=hsolve->tablist=(double *)calloc(ncols*tabsize,sizeof(double));
		xvals=hsolve->xvals=(double *)calloc(tabsize,sizeof(double));
		x=hsolve->xmin;
		dx=1.0/hsolve->invdx;
		for(j=0; j<tabsize; j++) {
		    xvals[j]=x;
		    x+=dx;
		}
		if (hsolve->ydivs) {
		    hsolve->yvals=(double *)calloc(hsolve->ydivs+1,sizeof(double));
		    x=hsolve->ymin;
		    dx=1.0/hsolve->invdy;
		    for(j=0; j<hsolve->ydivs+1; j++) {
			hsolve->yvals[j]=x;
			x+=dx;
		    }
		}
		l=0;	/* counts columns */
		/* first all 1D tables */
		for(i=0; i<nvtables; i++) {	
		    if (tabcols[i]==1) {	/* 1D */
			tabindex[i]=l;
			hsolve->tables[l]=tablist[i];
			hsolve->tabcols[l]=tabcols[i];
			hsolve->tabinteg[l]=tabintegrate[i];
			if (tabintegrate[i]>0) {
			    /* B table */
			    table=tablist[i];
			    for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				htable[k]=1.0+tby2*table[j];
			    }
			    l++;
			    /* A table */
			    i++;	/* this table also */
			    table=tablist[i];
			    for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				htable[k]=dt*table[j];
			    }
			    l++;
			} else {	/* TABCURR_T or instant gate */
			    /* G_tab or B table: copy unaltered */
			    gtable=tablist[i];
			    for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				htable[k]=gtable[j];
			    }
			    l++;
			    i++;	/* this table also */
			    table=tablist[i];
			    if (tabintegrate[i]==-1) { /* instant gate */
				/* A table: copy unaltered */
				for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				    htable[k]=table[j];
				}
			    } else {	/* TABCURR_T */
				/* I_tab: convert to an E_tab containing Ek */
				for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				    Gk=gtable[j];
				    /* Ek=(Ik + Gk*Vm)/Gk */
				    htable[k]=(table[j] + Gk*xvals[j])/Gk;
				}
			    }
			    l++;
			}
		    }
		}
		/* then all 2D tables */
		for(i=0; i<nvtables; i++) {	
		    if (tabcols[i]>1) {		/* 2D */
			hsolve->tables[l]=tablist[i];
			hsolve->tabcols[l]=tabcols[i];
			hsolve->tabinteg[l]=tabintegrate[i];
			tabindex[i]=l;
			btable2=(double **)tablist[i];
			atable2=(double **)tablist[i+1];
			if (tabintegrate[i]>0) {
			    for (n=0; n<tabcols[i]; n++) {
				/* B table */
				for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				    htable[k]=1.0+tby2*btable2[j][n];
				}
				l++;
				/* A table */
				for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				    htable[k]=dt*atable2[j][n];
				}
				l++;
			    }
			    i++;	/* we did 2 tables */
			} else {	/* TABCURR_T or instant gate */
			    for (n=0; n<tabcols[i]; n++) {
				for(j=0,k=l;j<tabsize;j++,k+=ncols) {
				    /* G_tab or B table: copy unaltered */
				    Gk=htable[k]=btable2[j][n];
				    if (tabintegrate[i]==-1) { /* instant gate*/
					/* A table: copy unaltered */
					htable[k]=atable2[j][n];
				    } else {	/* TABCURR_T */
					/* I_tab: convert to E_tab with Eks */
					/* Ek=(Ik + Gk*Vm)/Gk */
					htable[k+1]=(atable2[j][n] +Gk*xvals[j])/Gk;
				    }
				}
				l+=2;
			    }
			    i++;	/* we did 2 tables */
			}
		    }
		}
	    }
	}
	if (nstables) {
	    htable=hsolve->stablist=(double *)calloc(nstables*SYNSIZE,sizeof(double));
	    for(i=0;i<nstables*SYNSIZE;i++) {
		htable[i]=stablist[i];
	    }
	}
	return(0);
}

void do_chip_hreset(hsolve,actionstr)
/* Function for putting data values from elements into chip array */
	Hsolve	*hsolve;
	char    *actionstr;
{
	int 	ncompts=hsolve->ncompts;
	Element **compts=hsolve->compts;
	int     *elmnum=hsolve->elmnum;
	Element **children=hsolve->children;
	short   *childtypes=hsolve->childtypes;
	int     *childlink=hsolve->childlink;
	int     *childmsg=hsolve->childmsg;
	int     *childstart=hsolve->childstart;
	int	*tabinteg=hsolve->tabinteg;
	double	*tablist=hsolve->tablist;
	int	ncols=hsolve->ncols;
	int	tabsize=hsolve->xdivs+1;
	int 	i,j,k,n;
	int	ct,cl,cm;
	double  dt,ratio;
	Action  *action;

	if ((hsolve->readflag==HSETUP_T)&&(IsSilent()<0))
		printf("transferring element field values into solve arrays\n");

	action = GetAction(actionstr);
	dt=Clockrate(hsolve);
	if (dt != hsolve->dt) {
	    /* user changed the clocks after SETUP call */
	    ratio=dt/hsolve->dt;
	    for(i=0;i<hsolve->ntab;i=i+2) {
		if (tabinteg[i]>0) {
		    for (j=0,k=i;j<tabsize;j++,k+=ncols) {
			tablist[k]=1.0+((tablist[k]-1.0)*ratio);
			tablist[k+1]=tablist[k+1]*ratio;
		    }
		}
	    }
	    hsolve->dt = dt;
	}

	if (hsolve->readflag==HREAD_T) {
	    if (strcmp(actionstr,"RESET")==0) {
		Error();
		printf(" no reset possible on hsolve\n");
	    }
	    return;
	}

	/* put field values into chip array */
	for (i=0;i<ncompts;i++){
	    CallElement(compts[elmnum[i]],action,0);	/* set Vm=initVm if RESET */
	    chip_put_compt(hsolve,i);
	    n=childstart[i];
	    /* do RESET in opposite order, otherwise nernst will bomb */
	    for (j=childstart[i+1]-1; j>=n; j--) CallElement(children[j],action,0);
	    for (j=n; j<childstart[i+1]; j++) {
		ct=childtypes[j];
		cl=childlink[j];
		cm=childmsg[j];
		switch (ct) {
		    case SPIKEGEN_T:
			chip_put_spike(hsolve,j);
			break;

		    case TABCHAN_T:
		    case TAB2CHAN_T:
			chip_put_tabchannel(hsolve,j);
			break;

		    case TABCURR_T:
		    case TABGHK_T:
			chip_put_tabcurrent(hsolve,j);
			break;

		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
			chip_put_channelc3(hsolve,j);
			break;
		    
		    case MGBLOCK_T:
			chip_put_mgblock(hsolve,j);
			break;

		    case NERNST_T:
			chip_put_nernst(hsolve,j,cm);
			/* this overwrites conc arrays, but if necessary, conc 
			**  will be reset next */
			break;

		    case GHK_T:
			chip_put_ghk(hsolve,j,cm);
			/* this overwrites conc arrays, but if necessary, conc 
			**  will be reset next */
			break;

		    case CACONCEN_T:
			chip_put_concen(hsolve,j,cm);
			chip_put_Ca_concen(hsolve,i,j);
			break;

		    case POOL_T:
			chip_put_pool(hsolve,j,cm);
			chip_put_flux(hsolve,i,j);
			break;

		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
			chip_put_shell(hsolve,j,cm);
			chip_put_flux(hsolve,i,j);
			break;

		    case FIXBUFF_T:
		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
			chip_put_buffer(hsolve,j,cm);
			break;

		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
			chip_put_d2buffer(hsolve,j,cm);
			break;

		    case TAUPUMP_T:
			chip_put_taupump(hsolve,j);
			break;

		    case MMPUMP_T:
		    case ELMMPUMP_T:
			chip_put_mmpump(hsolve,j,cl);
			break;

		    case HILLPUMP_T:
			chip_put_hillpump(hsolve,j,cm);
			break;

		    case POOLBUFF_T:
			chip_put_poolbuffer(hsolve,j,cm);
			break;

		    default:
			break;
		}
	    }	/* j loop */
	}	/* i loop */
	return;
}

void do_chip_hsave(hsolve)
/* Function for putting data values back into elements from chip array */
    Hsolve	*hsolve;
{
	int 	ncompts=hsolve->ncompts;
	int 	i,j;
	short   *childtypes;
	int	*childstart,*childlink,*childmsg;
	int	ct,cl,cm;

	if (hsolve->readflag==HREAD_T)  return;

	if (IsSilent()<0) printf("restoring all solve computed fields\n");
	childtypes=hsolve->childtypes;
	childlink=hsolve->childlink;
	childmsg=hsolve->childmsg;
	childstart=hsolve->childstart;

	for (i=0;i<ncompts;i++){
	    chip_get_compt(hsolve,i);
	    for (j=childstart[i]; j<childstart[i+1]; j++) {
		ct=childtypes[j];
		cl=childlink[j];
		cm=childmsg[j];
		switch (ct) {
		    case TABCHAN_T:
		    case TAB2CHAN_T:
			chip_get_tabchannel(hsolve,j);
			break;

		    case TABCURR_T:
		    case TABGHK_T:
			chip_get_tabcurrent(hsolve,j);
			break;

		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
			chip_get_channelc3(hsolve,j);
			break;
		    
		    case MGBLOCK_T:
			chip_get_mgblock(hsolve,j);
			break;

		    case NERNST_T:
			chip_get_nernst(hsolve,j,cm);
			break;
		    
		    case GHK_T:
			chip_get_ghk(hsolve,j,cm);
			break;

		    case CACONCEN_T:
			chip_get_concen(hsolve,j,cm);
			break;

		    case POOL_T:
			chip_put_pool(hsolve,j,cm);
			break;

		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
			chip_get_shell(hsolve,j,cm);
			break;

		    case POOLBUFF_T:
		    case FIXBUFF_T:
		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
			chip_get_buffer(hsolve,j,cm);
			break;

		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
			chip_get_d2buffer(hsolve,j,cm);
			break;

		    case ELMMPUMP_T:
			chip_get_mmpump(hsolve,j);
			break;

		    case TAUPUMP_T:
			chip_get_taupump(hsolve,j);
			break;

		    default:
			break;
		}
	    }	/* j loop */
	}	/* i loop */
	return;
}
