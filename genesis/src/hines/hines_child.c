static char rcsid[] = "$Id: hines_child.c,v 1.4 2006/01/10 19:56:39 svitak Exp $";

/* Version EDS22i 99/12/16, Erik De Schutter, BBF-UIA 7/94-12/99 */

/*
** $Log: hines_child.c,v $
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
** Revision 1.10  2001/01/01 01:18:45  mhucka
** Fix from Hugo Cornelis for a bug in hsolve in which Vm is loaded too late
** under sone circumstances.
**
** Revision 1.9  1999/12/29 10:26:14  mhucka
** New updates from Erik De Schutter
**
** EDS22k revision HC BBF-UIA 00/07/24
** Fix for case when Vm was loaded to late (for Ca channel only with nernst Ek)
** 
** EDS22i revision: HC BBF-UIA 99/12/16-99/12/16
** Allocated childtypes for one more child type (nchildren + 2)
** 
** EDS22h revison: EDS BBF-UIA 99/03/10-99/03/10
** Improved concpool code and hillpump code 
**
** EDS22g revison: EDS BBF-UIA 99/02/11-99/02/11
** Added chanmode 5
**
** EDS22f revison: EDS BBF-UIA 98/10/28-98/10/29
** Minor bug corrections: Mg_block, taupump
**
** EDS22d revison: EDS BBF-UIA 98/06/16-98/06/19
** Minor bug corrections: TABGHK_T
**
** EDS22c revison: EDS BBF-UIA 97/11/28-97/12/04
** Misc small changes to make symmetric compartments work
**
** EDS22b revison: EDS BBF-UIA 97/11/12-97/11/19
** Corrected chanmode 4 code for passive compartments
** Added concpool and hillpump code, reorganized some arrays
**
** Revision 1.2  1997/07/24 00:18:42  dhb
** Fixes from PSC:
**   replaced erroneous abs() with fabs()
**   replaced two occurances of = with == in conditionals
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* TO CHANGE: NaCa comes now in between KM/K2 and KC: clears Ek 
**            add dual flux links for fluxes between stores */
/* Initialization routine.
** This version assumes that ALL children of a compartment need to be
**   executed by the hines solver.  This makes it easy to include elms
**   that are not directly related to the compartment membrane.  
** All child elms are identified (any subtree structure is allowed), 
**   categorized into specific arrays and sorted by their final order 
**   of execution.  All relationships between child elms are resolved.
** Links between children are sometimes stored in childmsg, childlink and 
**   childpos.  They are initially stored as absolute indexes, but after
**   final sorting of the children they are converted into indexes
**   relative to first child of each compartment.
*/

#include <math.h>
#include "hines_ext.h"


int do_hget_children(hsolve)
	Hsolve	*hsolve;
{
	int	g,i,j,k = 0,l,m,n,temp = 0;
	int     ncompts=hsolve->ncompts;
	int	nchildren,nconcs,nsynchans=0;
	Element **compts,*compt,*child = NULL,**children = NULL,*elm,*elm2;
	int	*elmnum,*firstconc;
	int *piCompFlags = NULL;
	short	*childtypes = 0;
	int	*childstart,*childmsg = NULL,*childpos = NULL,*childnames = NULL,*childlink = NULL,*childcode = NULL;
	int	*childindex,*childrank,*childgiv,*compgiv;
	short	nameused[MAXNAMES];
	int	ct = 0,cc = 0,cm = 0,cp = 0,cn = 0,cl = 0;
	float	*childdata,cd = 0.0;
	int	concID=0;
	int	nnames;
	char    *names[MAXNAMES],**elm_names = NULL;
	double	msgdata;
	Tchan	*tchan = NULL;
	T2chan	*t2chan;
	Tcurr	*tcurr;
	Concen	*concen;
	Dshell	*shell;
	Dbuffer	*buffer;
	D2buffer *buffer2;
	Mpump	*mpump;
	Hpump	*hpump;
	Tpump	*tpump;
	Cpool	*pool;
	int	firstDOMAIN=1,firstSPIKE=1;
	Element *hget_msg_elm(),*hget_msg_dst();
	int	hget_child(),hhas_msg_toelm();
	void	h_sort_children();
	int	Strindex();	
	char*   oname;
	int	duplicate_flag;
	Action  *action;
	
	compts=hsolve->compts;
	elmnum=hsolve->elmnum;
	duplicate_flag=hsolve->readflag==HDUPLICATE_T;

	/* First count all non-compartment children */
	nchildren=0;
	if(!duplicate_flag) hsolve->childstart=(int *)calloc(ncompts+1,sizeof(int));
	childstart=hsolve->childstart;
	for (i=0; i<ncompts; i++) {
	/* loop through all compartments by elmnum */
	    compt=compts[elmnum[i]];
	    child=compt->child;
	    while (child) {
		/* skip any compartments and hsolvers */
		oname = BaseObject(child)->name;
		if (Strindex(oname,"compartment") >= 0) break;	
		if (strcmp(oname,"hsolve")==0)  break;
		nchildren++;
		if (child->child) {		/* do subtree first */
		    child=child->child;
		} else if (child->next) {  /* continue this subtree */
			child=child->next;
		} else {			/* subtrees done */
		    while (1) {			/* find other subtree */
			child=child->parent;	/* move one up */
			if (child==compt) {	/* done all subtrees */
			    child=NULL;	/* finish loop */
			    break;
			} else if (child->next) {
			    child=child->next;
			    break;
			}
		    }
		}
	    }
	}
	if (duplicate_flag) {
	/* we only change the pointers, nothing else */
	    if (nchildren==!hsolve->nchildren) return(ERR);
	    nnames=hsolve->nelm_names;
	    elm_names=hsolve->elm_names;
	    childtypes=hsolve->childtypes;
	    childnames=hsolve->childnames;
	} else {
	    if (hsolve->chanmode>=4) {	/* store 'leak' as name */
		names[0]=(char *)calloc(5,sizeof(char));
		strcpy(names[0],"leak");
		nnames=1;
	    } else {
		nnames=0;
	    }
	    if (nchildren) {
		hsolve->nchildren=nchildren;
		childtypes=hsolve->childtypes=(short *)calloc(nchildren+2,sizeof(short));
		if (hsolve->no_elminfo) {
		    hsolve->childnames=NULL;
		} else {
		    childnames=hsolve->childnames=(int *)calloc(nchildren+1,sizeof(int));
		}
	    }
	}
	if (nchildren) {
	  children=hsolve->children=(Element **)calloc(nchildren+1,sizeof(Element *));
	  for (j=0; j<MAXNAMES; j++) nameused[j]=0;

	  /* Store and identify all the types of children */
	  nchildren=1;	/* note that index zero is NOT used */
	  for (i=0; i<ncompts; i++) {
	    for (j=0; j<nnames; j++) nameused[j]=0;
	    /* loop through all compartments by elmnum */
	    compt=compts[elmnum[i]];
	    childstart[i]=nchildren;
	    child=compt->child;
	    while (child) {
		oname = BaseObject(child)->name;
		/* skip any compartments */
		if (Strindex(oname,"compartment") >= 0) break;	
		if (strcmp(oname,"hsolve")==0)  break;
		/* identify type of child */
		if (strcmp(oname,"tabchannel")==0) {
			ct=TABCHAN_T;
		} else if (strcmp(oname,"tab2Dchannel")==0) {
			ct=TAB2CHAN_T;
		} else if (strcmp(oname,"tabcurrent")==0) {
			ct=TABCURR_T;
		} else if (strcmp(oname,"Ca_concen")==0) {
			ct=CACONCEN_T;
		} else if (strcmp(oname,"concpool")==0) {
			ct=POOL_T;
		} else if (strcmp(oname,"difshell")==0) {
			shell=(Dshell *)child;
			ct=DIFSHELL_T+shell->shape_mode;
		} else if (strcmp(oname,"taupump")==0) {
			ct=TAUPUMP_T;
		} else if (strcmp(oname,"mmpump")==0) {
			ct=MMPUMP_T;
		} else if (strcmp(oname,"hillpump")==0) {
			ct=HILLPUMP_T;
		} else if (strcmp(oname,"fixbuffer")==0) {
			ct=FIXBUFF_T;
		} else if (strcmp(oname,"difbuffer")==0) {
			buffer=(Dbuffer *)child;
			ct=BUFSHELL_T+buffer->shape_mode;
		} else if (strcmp(oname,"dif2buffer")==0) {
			buffer2=(D2buffer *)child;
			ct=BUF2SHELL_T+buffer2->shape_mode;
		} else if (strcmp(oname,"fura2")==0) {
			ct=FURA2_T;
		} else if (strcmp(oname,"nernst")==0) {
			ct=NERNST_T;
		} else if (strcmp(oname,"ghk")==0) {
			ct=GHK_T;
		} else if (strcmp(oname,"channelC2")==0) {
			ct=CHANC2_T;
			nsynchans++;
		} else if (strcmp(oname,"channelC3")==0) {
			ct=CHANC3_T;
			nsynchans++;
		} else if (strcmp(oname,"synchan")==0) {
			ct=SYNCHANC_T;
			nsynchans++;
		} else if (strcmp(oname,"synchan2")==0) {
			ct=SYNCHANC_T;
			nsynchans++;
		} else if (strcmp(oname,"Mg_block")==0) {
			ct=MGBLOCK_T;
		} else if (strcmp(oname,"spikegen")==0) {
			ct=SPIKEGEN_T;
			hsolve->spikegen=child;
		} else if (strcmp(oname,"neutral")==0) {
			ct=NEUTRAL_T;
		} else {
			Error();
			printf(" during SETUP of %s: unknown element type %s.\n",Pathname(hsolve),oname);
			return(ERR);
		}
		if (duplicate_flag) {
		    /* find corresponding elm_name */
		    oname=child->name;
		    for (k=0;k<nnames;k++){
			if (strcmp(oname,elm_names[k])==0) break;
		    }
		    if ((k==ERR) || nameused[k])  return(ERR);	/* duplicate or not found */
		    nameused[k]=1;
		    /* find corresponding sorted child */
		    for (j=childstart[i]; j<childstart[i+1]; j++) {
			if (childnames[j]==k) break;
		    }
		    if (j==childstart[i+1]) return(ERR);/* not found */
		    if (childtypes[j]!=ct) return(ERR);	/* not identical */
		    children[j]=child;
		} else {
		    children[nchildren]=child;
		    /* store index to child name */
		    if (!hsolve->no_elminfo) {
			k=hstore_name(hsolve,child,&nnames,names,NULL);
			if (k==ERR) return(ERR);
			if (nameused[k]) {
			    printf("** Warning - during SETUP of %s: name %s is used for two different children in compartment %s.\n",Pathname(hsolve),child->name,compt->name);
			    printf("DUPLICATE and findsolvefield operations may fail.\n");
			} else {
				nameused[k]=1;
			}
			childnames[nchildren]=k;
		    }
		    childtypes[nchildren]=ct;
		}
		HsolveBlock(child);
		nchildren++;
		if (child->child) {		/* do subtree first */
		    child=child->child;
		} else if (child->next) {   /* continue this subtree */
		    child=child->next;
		} else {			/* subtrees done */
		    while (1) {			/* find other subtree */
			child=child->parent;	/* move one up */
			if (child==compt) {	/* done all subtrees */
			    child=NULL;		/* finish loop */
			    break;
			} else if (child->next) {
			    child=child->next;
			    break;
			}
		    }
		}
	    }
	  }
	}

	childstart[ncompts]=nchildren;
	if (duplicate_flag) {
	    if (hsolve->chanmode>=4) {
		l=hsolve->ovals-hsolve->givals;
		hsolve->givals=(double *)calloc(hsolve->ngivals,sizeof(double));
		hsolve->ovals=hsolve->givals+l;
	    }
	    return(0);	/* done */
	}

	if (nchildren) {
	  hsolve->nelm_names=nnames;
	  if (!hsolve->no_elminfo) {
	    hsolve->elm_names=(char **)calloc(nnames,sizeof(char *));
	    for (i=0;i<nnames;i++){
		hsolve->elm_names[i]=names[i];
	    }
	  }

	  childdata=hsolve->childdata=(float *)calloc(nchildren+1,sizeof(float));
	  childmsg=hsolve->childmsg=(int *)calloc(nchildren+1,sizeof(int));
	  childcode=hsolve->childcode=(int *)calloc(nchildren+1,sizeof(int));
	  childlink=hsolve->childlink=(int *)calloc(nchildren+1,sizeof(int));
	  childpos=hsolve->childpos=(int *)calloc(nchildren+1,sizeof(int));
	  for (i=0; i<=nchildren; i++) {
	    childdata[i]=0.0;
	    childmsg[i]=0;
	    childcode[i]=0;
	    childlink[i]=0;
	    childpos[i]=0;
	  }

	  /*- allocate storage for compartment flags */

	  hsolve->piCompFlags
	      = piCompFlags
	      = (int *)calloc(ncompts,sizeof(int));

	  /* Collect relevant data about children, necessary for creating
	  **  the chip array.  To be able to combine functions for different
	  **  element types, we do three separate loops through all the
	  **  children of each compartment. */
	  action = GetAction("RECALC");
	  for (i=0; i<ncompts; i++) {
	    /* loop through all compartments by elmnum */
	    compt=compts[elmnum[i]];
	    n=childstart[i];
	    for (j=n; j<childstart[i+1]; j++) {
		/* loop through all the children of this comp */
		child=children[j];
		CallElement(child,action,0);
		ct=childtypes[j];
		switch (ct) {
		    case TABCHAN_T:
		    /* childdata: flux_scale or DOMAINCONC scale
		    ** childcode: <0: child of DOMAINCONC
		    ** childmsg:   0: use in element Ek
		    **            >0: get Ek from this child (nernst)
		    **            <0: get Ek from this child (ghk)
		    ** childlink:  0: no flux contribution
		    **            >0: contribute simple flux to this conc
		    **            <0: contribute scaled flux to this conc
		    ** childpos:   0: Vm dependent	(1D)
		    **            >0: conc dep, get conc from this child (1D)
		    **            <0: Vm and conc dependent	(2D)
		    */
			tchan=(Tchan *)child;
			/* find Vm/conc dependency */
			if (tchan->Z_alloced) {
			  if (tchan->Z_conc) {
			    elm=hget_msg_elm(hsolve,j,DOMAINCONC,&msgdata,NOERR);
			    if (!elm) {
				/* conc dependent, find the conc */
				elm=hget_msg_elm(hsolve,j,CONCEN,&msgdata,MONE);
				if (elm==NULL) return(ERR);
				k=hget_child(hsolve,elm,CACONCEN_T,n);
				if (k==ERR) return(ERR);
				if (tchan->X_alloced || tchan->Y_alloced) {
				    childpos[j]=-k;	/* also Vm dependent */
				} else {
				    childpos[j]=k;
				}
			    }
			  } else { /* Voltage-dependent Z gate (!tchan->Z_conc) */
			    childpos[j] = 0;
			  }
			}

			/*- if Vm dependent */

			if (tchan->X_alloced || tchan->Y_alloced || 
			    (tchan->Z_alloced && ! tchan->Z_conc))
			{
			    /*- remember : Vm needed in ops */

			    piCompFlags[i] = 1;
			}

			/* childdata and childlink filled in by CACONCEN_T elm */
			break;

		    case TAB2CHAN_T:
		    /* childdata: flux_scale or DOMAINCONC scale
		    ** childcode: >0: child of second conc (C2_INDEX)
		    **            <0: child of DOMAINCONC
		    ** childmsg:   0: use in element Ek
		    **            >0: get Ek from this child (nernst)
		    **            <0: get Ek from this child (ghk)
		    ** childlink:  0: no flux contribution
		    **            >0: contribute simple flux to this conc
		    **            <0: contribute scaled flux to this conc
		    ** childpos:   0: Vm dependent
		    **            >0: conc dependent, get conc from this child
		    **            <0: Vm and conc dependent
		    */
			t2chan=(T2chan *)child;
			m=k=0;
			/* find Vm/conc dependency */
			if (t2chan->X_alloced)  m=t2chan->Xindex;
			if (t2chan->Y_alloced)  {
			    k=t2chan->Yindex;
			    if (m==0) {
				m=k;
				k=0;
			    } else if (m==k) {
				k=0;
			    }
			}
			if ((k==0)&&(t2chan->Z_alloced))  {
			    k=t2chan->Zindex;
			}
			if ((m==C1_C2_INDEX)||(k==C1_C2_INDEX)) {
			    m=C1_INDEX;
			    k=C2_INDEX;
			}
			if ((abs(m)==C1_INDEX)||(abs(k)==C1_INDEX)) {
			    /* conc dependent, find the conc */
			    elm=hget_msg_elm(hsolve,j,CONCEN1,&msgdata,MONE);
			    if (elm==NULL) return(ERR);
			    l=hget_child(hsolve,elm,CACONCEN_T,n);
			    if (l==ERR) return(ERR);
			    if ((m<0)||(k<0)) {
				childpos[j]=-l;	/* also Vm dependent */
			    } else {
				childpos[j]=l;
			    }
			}
			if ((abs(m)==C2_INDEX)||(abs(k)==C2_INDEX)||
			    (m==DOMAIN_C2_INDEX)||(k==DOMAIN_C2_INDEX)) {
			    /* conc dependent, find the conc */
			    elm=hget_msg_elm(hsolve,j,CONCEN2,&msgdata,MONE);
			    if (elm==NULL) return(ERR);
			    l=hget_child(hsolve,elm,CACONCEN_T,n);
			    if (l==ERR) return(ERR);
			    if ((m==DOMAIN_C2_INDEX)||(k==DOMAIN_C2_INDEX)) {
				childpos[j]=l;
				/* childcode will be set in 2nd switch */
			    } else {
				childcode[j]=l;
			    }
			}

			/*- if Vm dependent */

			if (m == VOLT_INDEX
			    || m == VOLT_C1_INDEX
			    || m == VOLT_C2_INDEX
			    || m == VOLT_DOMAIN_INDEX
			    || k == VOLT_INDEX
			    || k == VOLT_C1_INDEX
			    || k == VOLT_C2_INDEX
			    || k == VOLT_DOMAIN_INDEX)
			{
			    /*- remember : Vm needed in ops */

			    piCompFlags[i] = 1;
			}

			break;


		    case TABCURR_T:
		    case TABGHK_T: /* type detected in next piece of code */
		    /* childdata: flux_scale or DOMAINCONC scale
		    ** childcode: >0: child of second conc (C2_INDEX)
		    **            <0: child of DOMAINCONC
		    ** childmsg:  not used
		    ** childlink:  0: no flux contribution
		    **            >0: contribute simple flux to this conc
		    **            <0: contribute scaled flux to this conc
		    ** childpos:   0: Vm dependent
		    **            >0: conc dependent, get conc from this child
		    **            <0: Vm and conc dependent
		    */
			tcurr=(Tcurr *)child;
			/* find Vm/conc dependency */
			if (!tcurr->alloced) {
			    Error();
			    printf(" during SETUP of %s: %s has no G_tab and I-tab tables.\n",Pathname(hsolve),Pathname(child));
			    return(ERR);
			}
			m=tcurr->Gindex;

			/*- if Vm dependent */

			if (m == VOLT_INDEX
			    || m == VOLT_C1_INDEX
			    || m == VOLT_C2_INDEX
			    || m == VOLT_DOMAIN_INDEX)
			{
			    /*- remember : Vm needed in ops */

			    piCompFlags[i] = 1;
			}

			elm=hget_msg_elm(hsolve,j,ADD_GBAR,&msgdata,NOERR);
			if (elm) {
			    childtypes[j]=TABGHK_T;	/* retype */
			    if ((m<VOLT_C2_INDEX)&&(m>VOLT_INDEX)) {
				Error();
				printf(" during SETUP of %s: %s has the wrong Gindex to implement a GHK function.\n",hsolve->name,Pathname(child));
			    }
			}
			if ((m==C1_C2_INDEX)||(k==C1_C2_INDEX)) {
			    m=C1_INDEX;
			    k=C2_INDEX;
			} else {
			    k=0;
			}
			if (abs(m)==C1_INDEX) {
			    /* conc dependent, find the conc */
			    elm=hget_msg_elm(hsolve,j,CONCEN1,&msgdata,MONE);
			    if (elm==NULL) return(ERR);
			    l=hget_child(hsolve,elm,CACONCEN_T,n);
			    if (l==ERR) return(ERR);
			    if (m<0) {
				childpos[j]=-l;	/* also Vm dependent */
			    } else {
				childpos[j]=l;
			    }
			}
			if ((abs(m)==C2_INDEX)||(abs(k)==C2_INDEX)) {
			    /* conc dependent, find the conc */
			    elm=hget_msg_elm(hsolve,j,CONCEN2,&msgdata,MONE);
			    if (elm==NULL) return(ERR);
			    l=hget_child(hsolve,elm,CACONCEN_T,n);
			    if (l==ERR) return(ERR);
			    childcode[j]=l;
			}
			break;

		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
		    /* childdata: flux_scale
		    ** childcode:  0: no Mg_block
		    **            >0: child of Mg_block
		    ** childmsg:   0: use in element Ek
		    **            >0: get Ek from this child (nernst)
		    **            <0: get Ek from this child (ghk)
		    ** childlink:  0: no flux contribution
		    **            >0: contribute positive flux to this conc
		    **            <0: contribute scaled flux to this conc
		    ** childpos: index into stablist (filled in later)
		    */
			/* childdata and childlink filled in by CACONCEN_T elm*/
			break;

		    case HILLPUMP_T:
		    /* childdata: not used
		    ** childcode: first: ID code
		    **            later: index to difshell child
		    ** childmsg:  first: index of concpool child if influx
		    **            later: index to difshell conc
		    ** childlink: first: index of difshell child
		    **            later: index to concpool conc if influx
		    ** childpos:  first: position of corresponding conc
		    **            later: index to concpool child
		    */
			/* childmsg and childlink filled in next loop */
			hpump=(Hpump *)child;
			if ((hpump->Hill<1) || (hpump->Hill>4)) {
			    Error();
			    printf(" during SETUP of %s: hill factor should be in range 1-4 for %s\n",Pathname(hsolve),Pathname(child));
			    return(ERR);
			}
			break;

		    case MMPUMP_T:
		    case ELMMPUMP_T: /* type detected in next piece of code */
		    /* childdata: not used
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg:  index conc array (determined later)
		    ** childlink: index of conc child
		    ** childpos:  position of corresponding conc
		    */
			mpump=(Mpump *)child;
			if (mpump->val) {	/* electrogenic */
			    childtypes[j]=ELMMPUMP_T;
			}
			/* conc found in next switch */					
			break;

		    case MGBLOCK_T:
		    /* childdata: not used
		    ** childcode: pointer to blocked CHANC[23]_T or SYNCHANC_T
		    ** childmsg: not used
		    ** childlink:  0: no flux contribution
		    **            >0: contribute positive flux to this conc
		    **            <0: contribute scaled flux to this conc
		    ** childpos: not used into stablist (filled in later)
		    */
			/* find the blocked channel */
			elm=hget_msg_elm(hsolve,j,CHANNEL2,&msgdata,MONE);
			if (elm==NULL) return(ERR);
			k=hget_child(hsolve,elm,SYNCHAN_T,n);
			if (k==ERR) return(ERR);
			childcode[j]=k;
			childpos[k]=j;
			/* NO BREAK */

		    case TAUPUMP_T:
		    /* childdata: not used
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg:  index conc array (determined later)
		    ** childlink: index of conc child
		    ** childpos:  position of corresponding conc
		    */
			/* conc found in next switch */
			tpump=(Tpump *)child;
			/* if constant tau-> no voltage dependence */
			if ((tpump->T_A==0.0)&&(tpump->T_B==0.0)) break;
			/* NO BREAK */

		    case GHK_T:
		    /* childdata: flux_scale
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg:  index conc array (determined later)
		    ** childlink: index of conc child
		    ** childpos:  position of corresponding conc
		    */
			/* Vm dependent, check source of msg */
			if (ct==GHK_T) {
			    elm=hget_msg_elm(hsolve,j,GVOLTAGE,&msgdata,MONE);
			} else {	/* TAUPUMP_T */
			    elm=hget_msg_elm(hsolve,j,VOLTAGE,&msgdata,MONE);
			}
			if (elm && (elm!=compt)) {
			    Error();
			    printf(" during SETUP of %s: %s is wrong src VOLTAGE msg to %s.\n",Pathname(hsolve),Pathname(elm),Pathname(child));
			    return(ERR);
			}
			break;

		    case NERNST_T:
		    /* childdata: not used
		    ** childcode: not used
		    ** childmsg:  index conc array (determined later)
		    ** childlink: index of submembrane conc (or supra if not
					   defined) child
		    ** childpos:  position of corresponding conc
		    */
			/* conc found in next switch */
			break;

		    case POOL_T:
		    /* childdata:  not used
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg:  index conc array (determined later)
		    ** childlink: index to corresponding difshell
		    ** childpos:  indicates position relative to other shells.
		    **            these numbers are not necessarily consecutive
		    **		   negative: below submembrane
		    **             -1: submembrane
		    **              1: supramembrane
		    **		   positive: above supramembrane
		    */
			concID++; /* childcode: give unique ID */
			childlink[j]=0;
			childcode[j]=concID;
			childpos[j]=ANYPOOL;
			elm=hget_msg_elm(hsolve,j,STOREOUTFLUX,&msgdata,k);
			if (elm) {
			    Error();
			    printf(" during SETUP of %s: STOREOUTFLUX msg not implemented for %s.\n",Pathname(hsolve),Pathname(child));
			} else {
			    elm=hget_msg_elm(hsolve,j,STOREINFLUX,&msgdata,k);
			    if (elm) {
				Error();
				printf(" during SETUP of %s: STOREINFLUX msg not implemented for %s.\n",Pathname(hsolve),Pathname(child));
			    }
			}
			pool=(Cpool *)child;
			if (pool->leak!=0.0) {
			    Error();
			    printf(" during SETUP of %s: leak not implemented for %s.\n",Pathname(hsolve),Pathname(child));
			}
			break;

		    case CACONCEN_T:
		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
		    /* childdata: shell thickness
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg: index conc array (determined later)
		    **            temporary flag to signal use by NERNST_T (-1)
		    ** childlink:  type of diffusion and number of fluxes
		    ** childpos:  indicates position relative to other shells.
		    **            these numbers are not necessarily consecutive
		    **		   negative: below submembrane
		    **             -1: submembrane
		    **              1: supramembrane
		    **		   positive: above supramembrane
		    */
			if (ct==CACONCEN_T) {
			    concen=(Concen *)child;
			    childdata[j]=concen->thick;
			    /* childcode: give unique ID */
			    concID++;
			    childcode[j]=concID;
			    /* position: at present always submembrane */
			    childpos[j]=SUBMEMBRANE;
			    g=fI_Ca;
			} else {				/* DIFSHELL_T */
			    shell=(Dshell *)child;
			    childdata[j]=shell->thick;
			    /* position: determine if in or out */
			    if (shell->dia <= ((Ncomp *)compt)->dia) {
				if (childpos[j]==0) {
				    childpos[j]=ANYSUBSHELL;
				} else if (childpos[j]>=SUPRAMEMBRANE) {
				    Error();
				    printf(" during SETUP of %s: dia of inside shell %s is larger than dia of %s.\n",Pathname(hsolve),Pathname(child),Pathname(compt));
				    return(ERR);
				}
			    } else {
				if (childpos[j]==0) {
				    childpos[j]=ANYSUPRASHELL;
				} else if (childpos[j]<=SUBMEMBRANE) {
				    Error();
				    printf(" during SETUP of %s: dia of outside shell %s is smaller than dia of %s.\n",Pathname(hsolve),Pathname(child),Pathname(compt));
				    return(ERR);
				}
			    }
			    /* childlink: find diffusion sources */
			    m=k=0;
			    elm=hget_msg_elm(hsolve,j,DIFF_UP,&msgdata,NOERR);
			    if (elm) {
				m=hget_child(hsolve,elm,DIFSHELL_T,n);
				if (m==ERR) return(ERR);
			    }
			    if (m) childlink[j]+=DIFFDOWN;
			    elm=hget_msg_elm(hsolve,j,DIFF_DOWN,&msgdata,NOERR);
			    if (elm) {
				k=hget_child(hsolve,elm,DIFSHELL_T,n);
				if (k==ERR) return(ERR);
			    }
			    if (k) childlink[j]+=DIFFUP;
			    /* childcode: find source which has already concID*/
			    l=0;	/* concID */
			    if (m) l=childcode[m];
			    if (k && (l==0)) {
				m=k;	/* src index */
				l=childcode[m];
				k=-1;	/* position offset */
			    } else {
				k=1;	/* position offset */
			    }
			    if (l==0) {	/* generate new concID */
				/* either no diffusion or first shell found */
				concID++;
				childcode[j]=concID;
				/* no diffusion! -> change into a pool */
				if (childlink[j]==0) childtypes[j]=POOL_T;
			    } else {	/* determine position based on diff */
				childcode[j]=l;
				if ((childpos[j]<SUPRAMEMBRANE) &&
						(childpos[m]>SUBMEMBRANE)) {
				    Error();
				    printf(" during SETUP of %s: no diffusion across cell membrane possible for %s in %s.\n",Pathname(hsolve),Pathname(child),Pathname(compt));
				    /* printf("%d %d %d %d %s %s\n",j,m,childpos[j],childpos[m],children[j]->name,children[m]->name);*/
				    return(ERR);
				}
				if ((childpos[j]==ANYSUBSHELL)||(childpos[j]==
							    ANYSUPRASHELL)) 
				    childpos[j]=childpos[m]+k;
				if (childpos[j]==0) {
				    Error();
				    printf(" during SETUP of %s: more than %d shells in a single compartment at %s.\n",Pathname(hsolve),ANYSUPRASHELL,Pathname(child));
					/* printf("%d %d %d %d  %d %d\n",j,m,k,l,childpos[j],childpos[m]); */
				    return(ERR);
				}
			    }
			    g=STOREOUTFLUX;
			    if (fabs(shell->leak)>MTINY) childlink[j]+=FLUXOFFSET;
			}
			/* take care of all in/out flows */
			for (l=0; l<=g; l++) {
			    k=1;
			    while (1) {
				elm=hget_msg_elm(hsolve,j,l,&msgdata,k);
				if (!elm) break;
				m=hget_child(hsolve,elm,ANYCHAN_T,n);
				if (m==ERR) return(ERR);
				if (childlink[m]!=0) {
				    Error();
				    printf(" during SETUP of %s: %s sends more than one I_Ca msg.\n",Pathname(hsolve),Pathname(children[m]));
				    return(ERR);
				}
				childlink[j]+=FLUXOFFSET;
				switch (l) {
				    case INFLUX:
					temp=SUBMEMBRANE;
					/* NO BREAK */

				    case STOREINFLUX:
					childlink[m]=j;
					childdata[m]=1.0;	/* flux_scale */
					break;

				    case OUTFLUX:
					temp=SUBMEMBRANE;
					/* NO BREAk */

				    case STOREOUTFLUX:
					if (ct==CACONCEN_T) {	/* is a fI_Ca */
					    childlink[m]=-j;
					    childdata[m]=msgdata; /*flux_scale*/
					} else {	/* is a OUTFLUX */
					    childlink[m]=-j;
					    childdata[m]=-1.0;	/* flux_scale */
					    temp=SUPRAMEMBRANE;
					}
					break;

				    case FINFLUX:
					temp=SUBMEMBRANE;
					childlink[m]=-j;
					childdata[m]=msgdata;	/* flux_scale */
					break;

				    case FOUTFLUX:
					temp=SUPRAMEMBRANE;
					childlink[m]=-j;
					childdata[m]=-msgdata;	/* flux_scale */
					break;
				}
				/* check if inflow OK with position of shell */
				switch (childpos[j]) {
				    case SUBMEMBRANE:
				    case SUPRAMEMBRANE:
					if (((childlink[m]>0)||(childdata[m]>0))&&(childtypes[m]<TABCURR_T)&&(childpos[j]!=SUBMEMBRANE)) {
					    Error();
						printf(" during SETUP of %s: conflict between position of %s and flux sign.\n",Pathname(hsolve),Pathname(child));
						return(ERR);
					}
					break;

				    default:
					/* now identify this shell's position */
					if (ct==CACONCEN_T) {	/* Ca_concen */
					    if (childpos[j]<SUPRAMEMBRANE) {
						childpos[j]=SUBMEMBRANE;
					    } else {
						childpos[j]=SUPRAMEMBRANE;
					    }
					} else if (childtypes[m]<TABCURR_T) {
					    if (l<STOREINFLUX) childpos[j]=temp;
					}
				}
				k++;
			    }
			}
			break;

		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
		    /* childdata: shell thickness
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg:  first ID code buffer (kBf+11*kBb+13e11*D), 
		    **            later buffer index conc array
		    ** childlink: first index of conc, then index conc array
		    ** childpos:  first conc childpos, later ID code 
		    */
			if (ct<BUF2SHELL_T) {
				buffer=(Dbuffer *)child;
				childdata[j]=buffer->thick;
				childmsg[j]=(int)(13e11*buffer->D);
			} else {
				buffer2=(D2buffer *)child;
				childdata[j]=buffer2->thick;
				childmsg[j]=(int)((13e11*buffer2->Dfree) +
					  (17e11*buffer2->Dbound));
			}
			/* NO BREAK ! */

		    case FIXBUFF_T:
		    case POOLBUFF_T:  /* type detected in next piece of code */
		    /* childdata: not used
		    ** childcode:  ID code, all concens that are functionally
		    **             linked (e.g. same Nernst, diffusion, etc.)
		    **             have an identical unique code
		    ** childmsg:  first ID code buffer (kBf+11*kBb), 
		    **            later buffer index conc array
		    ** childlink: first index of conc, then index conc array
		    ** childpos:  first conc childpos, later ID code 
		    */
			/* generate buffer ID-code */
			buffer=(Dbuffer *)child;
			childmsg[j]+=(int )(buffer->kBf + 11*buffer->kBb);
			/* find the conc that is buffered */
			elm=hget_msg_elm(hsolve,j,BCONCEN,&msgdata,MONE);
			if (elm) {
			    k=hget_child(hsolve,elm,POOL_T,n);
			    if (k==ERR) return(ERR);
			    if ((k<n) || (k>=childstart[i+1]) ||
			        ((childtypes[k]<DIFSHELL_T)&&(ct>FIXBUFF_T))) {
				Error();
				printf(" during SETUP of %s: %s receives CONCEN msg from wrong conc %s.\n",Pathname(hsolve),Pathname(child),Pathname(children[k]));
				return(ERR);
			    }
			    if (childtypes[k]<DIFSHELL_T) childtypes[j]=POOLBUFF_T;
			    childlink[j]=k;
			}
			break;

		    case SPIKEGEN_T:
		    /* childdata: not used
		    ** childcode: not used
		    ** childmsg:  not used
		    ** childlink:  not used
		    ** childpos:  not used
		    */
			if (firstSPIKE) {
			    firstSPIKE=0;
			} else {
			    Error();
			    printf(" during SETUP of %s: second spikegen %s not allowed.\n",Pathname(hsolve),Pathname(child));
			    return(ERR);
			}
			break;

		    case NEUTRAL_T:
		    /* childdata: flux_scale
		    ** childcode: not used
		    ** childmsg:  not used
		    ** childlink:  0: no flux contribution
		    **            >0: contribute simple flux to this conc
		    **            <0: contribute scaled flux to this conc
		    ** childpos:  not used
		    */
			break;

		    default:
			Error();
			printf(" during SETUP of %s: bug #2 in do_hget_children.\n",Pathname(hsolve));
			return(ERR);
		}
	    }
	    /* second switch: resolve conc pointers, do DOMAINCONC */
	    g=0;
	    for (j=n; j<childstart[i+1]; j++) {
		/* loop through all the children of this comp */
		child=children[j];
		ct=childtypes[j];
		switch (ct) {
		    case TABCHAN_T:
		    case TAB2CHAN_T:
		    case TABCURR_T:
		    case TABGHK_T:
			/* check for DOMAINCONC msg */
			elm=hget_msg_elm(hsolve,j,DOMAINCONC,&msgdata,NOERR);
			if (elm) {
			    if ((g>0)&&firstDOMAIN) {
				printf("** Warning - during SETUP of %s: it is not guaranteed that Nernst or GHK potentials will work correctly if you have more than one DOMAINCONC dependent channel.\n",Pathname(hsolve));
				firstDOMAIN=0;
			    }
			    g++;
			    if (childmsg[j]&&firstDOMAIN) {
				printf("** Warning - during SETUP of %s: it is not guaranteed that a DOMAINCONC dependent channel will receive the correct Nernst or GHK potential.\n",Pathname(hsolve));
				firstDOMAIN=0;
			    }
			    if (childlink[j]<0) {
				Error();
				printf(" during SETUP of %s: cannot handle %s because it receives both a DOMAINCONC and send a scaled flux (fI_Ca, FINFLUX, (F)OUTFLUX) msg.\n",Pathname(hsolve),Pathname(child));
				return(ERR);
			    }
			    if (childcode[j]>0) {
				Error();
				printf(" during SETUP of %s: cannot handle %s because it receives both a DOMAINCONC and a CONCEN2 msg.\n",Pathname(hsolve),Pathname(child));
				return(ERR);
			    }
			    k=hget_child(hsolve,elm,NEUTRAL_T,n);
			    if (k==ERR) return(ERR);
			    childcode[j]=-k;
			    if ((ct==TABCURR_T) || (ct==TABGHK_T)) {
				/* scaling factor */
				childdata[j]=msgdata/((Tcurr *)child)->surface;
			    } else {
				/* scaling factor */
				childdata[j]=msgdata/((Tchan *)child)->surface;
			    }
			}
			break;
		    
		    case GHK_T:
		    case NERNST_T:
			/* find the conc determining Nernst potential */
			m=0;
			elm=hget_msg_elm(hsolve,j,CIN,&msgdata,NOERR);
			if (elm) {
			    m++;
			    k=hget_child(hsolve,elm,CACONCEN_T,n);
			    if (k==ERR) return(ERR);
			    if (childpos[k]>SUBMEMBRANE) {
				Error();
				printf(" during SETUP of %s: %s receives CIN msg from wrong conc.\n",Pathname(hsolve),Pathname(child));
				return(ERR);
			    }
			    childlink[j]=k;
			    childcode[j]=childcode[k];
			    childmsg[k]=-1;	/* flag use of CONC */
			    childpos[j]=childpos[k]=SUBMEMBRANE;
			}
			elm=hget_msg_elm(hsolve,j,COUT,&msgdata,NOERR);
			if (elm) {
			    m++;
			    k=hget_child(hsolve,elm,CACONCEN_T,n);
			    if (k==ERR) return(ERR);
			    if (childpos[k]<SUPRAMEMBRANE) {
				Error();
				printf(" during SETUP of %s: %s receives COUT msg from wrong conc.\n",Pathname(hsolve),Pathname(child));
				return(ERR);
			    }
			    l=childlink[j];
			    if (l==0) {	/* no SUBMEMBRANE conc defined */
				childlink[j]=k;
			    } else { /* establishes a link between two concs */
				childcode[k]=childcode[l];  /* same concID */
			    }
			    childcode[j]=childcode[k];
			    childmsg[k]=-1;	/* flag use of CONC */
			    childpos[j]=childpos[k]=SUPRAMEMBRANE;
			}
			if (m==0) {
			    Error();
			    printf(" during SETUP of %s: %s receives no CIN or COUT msg.\n",Pathname(hsolve),Pathname(child));
			    return(ERR);
			}
			break;

		    case MMPUMP_T:
		    case ELMMPUMP_T:
		    case HILLPUMP_T:
		    case TAUPUMP_T:
			/* find conc */
			if (ct==TAUPUMP_T) {
			    elm=hget_msg_dst(hsolve,j,TAUPUMP,NOERR);
			    if (!elm) elm=hget_msg_dst(hsolve,j,EQTAUPUMP,MONE);
			} else if (ct== HILLPUMP_T) {
			    /* Two HILLPUMP msgs allowed, order free */
			    cm=0;	/* flag as no pool used */
			    elm=hget_msg_dst(hsolve,j,HILLPUMP,0);
			    elm2=hget_msg_dst(hsolve,j,HILLPUMP,1);
			    if (elm) {
				k=hget_child(hsolve,elm,POOL_T,n);
				if (k==ERR) return(ERR);
				if (childtypes[k]==POOL_T) cm=k;
			    }
			    if (elm2) {
				k=hget_child(hsolve,elm2,POOL_T,n);
				if (k==ERR) return(ERR);
				if (childtypes[k]==POOL_T) {
				    if (cm) {
					Error();
					printf(" during SETUP of %s: %s cannot have msgs to two concpool elements.\n",Pathname(hsolve),Pathname(child));
					return(ERR);
				    } else {
					cm=k;
				    }
				} else if (!cm) {
				    Error();
				    printf(" during SETUP of %s: %s cannot have msgs to two difshell elements.\n",Pathname(hsolve),Pathname(child));
				    return(ERR);
				} else {	/* elm2 is difshell */
				    elm=elm2;
				}
			    }
			    childmsg[j]=cm;
			    if (cm) {
				/* needs message from difshell */
				elm2=hget_msg_elm(hsolve,cm,BCONCEN,&msgdata,MONE);
				if (elm2) {
				    k=hget_child(hsolve,elm,POOL_T,n);
				    if (k==ERR) return(ERR);
				    /* store difshell index in concpool */
				    childlink[cm]=k;	
				}
			    }
			} else {	/* MMPUMP_T, ELMMPUMP_T */
			    elm=hget_msg_dst(hsolve,j,MMPUMP,MONE);
			}
			if (elm) {
			    /* store difshell index */
			    k=hget_child(hsolve,elm,POOL_T,n);
			    if (k==ERR) return(ERR);
			    childlink[j]=k;
			}
			if (ct==ELMMPUMP_T) {
			    elm2=hget_msg_elm(hsolve,j,BCONCEN,&msgdata,MONE);
			    if (elm!=elm2) {
				Error();
				printf(" during SETUP of %s: wrong source of CONCEN msg to %s.\n",Pathname(hsolve),Pathname(child));
				return(ERR);
			    }
			}
			break;

		    case POOL_T:
			/* copy childcode and childpos if functionally
			** linked to DIFSHELL */
			elm=hget_msg_elm(hsolve,j,CONCEN,&msgdata,NOERR);
			if (elm) {
			    k=hget_child(hsolve,elm,POOL_T,n);
			    if (k==ERR) return(ERR);
			    childlink[j]=k;
			    childcode[j]=childcode[k];
			    childpos[j]-=childpos[k];
			}
		    default:
			break;
		}
	    }
	    /* third switch: resolve channel, pump and buffer stuff */
	    for (j=n; j<childstart[i+1]; j++) {
		/* loop through all the children of this comp */
		child=children[j];
		ct=childtypes[j];
		switch (ct) {
		    case TABCHAN_T:
		    case TAB2CHAN_T:
			if (!tchan->X_alloced && !tchan->Y_alloced &&
							!tchan->Z_alloced) {
			    Error();
			    printf(" during SETUP of %s: No tables defined in %s.\n",Pathname(hsolve),Pathname(child));
			    return(ERR);
			}
			/* NO BREAK ! */

		    case CHANC2_T:
		    case CHANC3_T:
		    case SYNCHANC_T:
			/* check for EK msg to nernst */
			elm=hget_msg_elm(hsolve,j,EK,&msgdata,NOERR);
			if (elm) {
			    k=hget_child(hsolve,elm,NERNST_T,n);
			    if (k==ERR) return(ERR);
			    childmsg[j]=k;
			}
			/* check for PERMEABILITY msg to ghk */
			elm=hget_msg_dst(hsolve,j,PERMEABILITY,NOERR);
			if (elm) {
			    k=hget_child(hsolve,elm,0,n);
			    if ((k>0)&&(childtypes[k]==GHK_T)) {
				childmsg[j]=-k;
				/* copy the flux info */
				childlink[j]=childlink[k];
				childdata[j]=childdata[k];
			    }
			}
			/* check for ADD_GBAR msg to tabcurrent */
			elm=hget_msg_dst(hsolve,j,ADD_GBAR,NOERR);
			if (elm) {
			    k=hget_child(hsolve,elm,0,n);
			    if ((k>0)&&(childtypes[k]==TABGHK_T)) {
				childmsg[j]=-k;
				/* copy the flux info */
				childlink[j]=childlink[k];
				childdata[j]=childdata[k];
			    }
			}
			/* check for CHANNEL msg */
			if ((childmsg[j]>=0) && 
					((ct<CHANC2_T)||(childpos[j]==0))) {
			    /* no GHK and no Mg_blocked synchan */
			    if (hhas_msg_toelm(hsolve,child,compt,CHANNEL,1)) 
								return(ERR);
			}
			/* for non-ghk: childdata and childlink filled in during
			**  first switch by CACONCEN_T elm */
			/* NO BREAK ! */

		    case TABCURR_T:
		    case TABGHK_T:
			/* Vm dependent, check source of msg */
			if (childpos[j]<=0) {
			    elm=hget_msg_elm(hsolve,j,VOLTAGE,&msgdata,MONE);
			    if (elm && (elm!=compt)) {
				    Error();
				    printf(" during SETUP of %s: %s is wrong src of VOLTAGE msg to %s.\n",Pathname(hsolve),Pathname(elm),Pathname(child));
				    return(ERR);
			    }
			}
			break;

		    case MMPUMP_T:
		    case ELMMPUMP_T:
		    case TAUPUMP_T:
		    case POOLBUFF_T:
		    case FIXBUFF_T:
		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
			/* copy childcode and childpos from DIFSHELL */
			m=childlink[j];
			childcode[j]=childcode[m];
			childpos[j]=childpos[m];
			break;

		    case HILLPUMP_T:
			/* copy childcode and childpos from DIFSHELL */
			m=childmsg[j];
			childcode[j]=childcode[m];
			childpos[j]=childpos[m];
			break;

		    default:
			break;
		}
	    }
	  }
	  /* Now sort all the children in each compartment.
	  ** create index, we will sort the index only (for now) */
	  childindex=(int *)calloc(nchildren+1,sizeof(int));
	  for (i=1; i<=nchildren; i++) childindex[i]=i;
	  for (i=0; i<ncompts; i++) {
	    /* Shell sort */
	    k=childstart[i];
	    n=childstart[i+1]-k;
	    k--;	/* offset */
	    for (j=2; j<=n; j++) {
		temp=childindex[j+k];
		m=j-1;
		while ((m>0)&&!hchild_compare(hsolve,childindex[m+k],temp)) {
		    childindex[m+k+1]=childindex[m+k];
		    m--;
		}
		childindex[m+k+1]=temp;
	    }
	  }
	  childrank=(int *)calloc(nchildren+1,sizeof(int));
	  for (i=1; i<=nchildren; i++) childrank[childindex[i]]=i;
  
	  /* second: insert NERNST_T, GHK_T and TABGHK_T in between ANYCHAN_T and
	  **  MGBLOCK_T after SYNCHAN_T as necessary, both childindex and 
	  **  childrank are changed! */
	  n=childstart[0];

	  for (i=0; i<ncompts; i++) {
	  /* loop through all compartments by elmnum */
	    m=childstart[i+1];
	    temp=0;	/* last Ek used */
	    for (j=n; j<m; j++) {
		k=childindex[j];
		ct=childtypes[k];
		if ((ct>=TABCHAN_T)&&(ct<NERNST_T)) {
		    cm=childmsg[k];
		    if ((cm<0)&&(childlink[k]==0)) {
			printf("** Warning - during SETUP of %s: %s should send a I_Ca or INFLUX msg because it gets Ek from a ghk.\n",Pathname(hsolve),Pathname(children[k]));
		    }
		    cm=abs(cm);
		    if ((cm!=0)&&(cm!=temp)) {	/* uses different nernst or ghk */
			temp=cm;
			/* move cm to present location in the index */
			h_sort_children(hsolve,childrank,childindex,cm,j,n,m);
		    }
		}
		if (ct==MGBLOCK_T) {	/* will be behind any SYNCHAN */
		    g=childcode[k];
		    /* move MGBLOCK_T after cm */
		    cm=childrank[g]; /* new, sorted location of SYNCHAN */
		    if (childrank[k]!=cm+1) {	/* need to sort it */
			h_sort_children(hsolve,childrank,childindex,k,cm+1,n,m);
		    }
		}
	    }	/* j loop */
	    n=m;
	  } /* i loop */

	  /* third: move DOMAINCONC dependent channels immediately after
	  **  source channel -> only one relation allowed!! */
	  n=childstart[0];
	  for (i=0; i<ncompts; i++) {
	  /* loop through all compartments by elmnum */
	    m=childstart[i+1];
	    temp=0;	/* last source used */
	    for (j=n; j<m; j++) {
		k=childindex[j];
		ct=childtypes[k];
		if (((ct>=TABCHAN_T)&&(ct<=TABCURR_T))||(ct==TABGHK_T)) {
		    cc=-childcode[k];
		    if (cc>0) {	/* receives DOMAINCONC */
			if (temp==cc) {
			    Error();
			    printf(" during SETUP of %s: %s is the source of more than one DOMAINCONC msg.\n",Pathname(hsolve),Pathname(children[cc]));
			    return(ERR);
			}
			temp=cc;
			cm=childmsg[cc];
			g=childrank[cc]; /* new, sorted location of src channel*/
			l=childindex[g+1]; /* index of next child */
			/* we need to check if this channel depends on a
			** a Nernst or GHK element, if so it needs to be the
			** last in the series of such channels */
			if ((cm!=0)&&(childmsg[l]==cm) &&
			    ((childtypes[l]>=TABCHAN_T) &&
			     (childtypes[l]<TABCURR_T)) ||
			    (childtypes[l]==TABGHK_T)) {
			    /* move src channel to back in line for Ek */
			    g++;
			    h_sort_children(hsolve,childrank,childindex,cc,g,n,m);
			}
			if (childrank[k]!=g+1) {	/* need to sort it */
			    /* move DOMAINCONC channel after cc */
			    h_sort_children(hsolve,childrank,childindex,k,g+1,n,m);
			    if (j>n) j--;  /* let's check this index again */
			}
		    }
		}
	    }	/* j loop */
	    n=m;
	  } /* i loop */

	  /* now that everything is sorted, put the arrays themselves in order 
	  ** by switching the elements */
	  n=childstart[0];
	  for (i=0; i<ncompts; i++) {
	    j=n;
	    n=childstart[i+1];
	    l=0;
	    while (j<n) {
		k=childindex[j];
		if (k<=0) {	/* already done?? */
			k=j+1;	/* try next slot */
		} else if (k==j) {	/* no change necessary */
			k=j+1;	/* try next slot */
		} else if (k==l) {		/* restore, finished a loop */
			children[j]=child;
			childtypes[j]=ct;
			childdata[j]=cd;
			childcode[j]=cc;
			childmsg[j]=cm;
			childlink[j]=cl;
			childpos[j]=cp;
			childnames[j]=cn;
			k=l+1;	/* try next slot */
			l=0;
		} else {
		    if (l==0) {	/* store this position */
			l=j;
			child=children[l];
			ct=childtypes[l];
			cd=childdata[l];
			cc=childcode[j];
			cm=childmsg[l];
			cl=childlink[l];
			cp=childpos[l];
			cn=childnames[l];
		    }
		    children[j]=children[k];
		    childtypes[j]=childtypes[k];
		    childdata[j]=childdata[k];
		    childcode[j]=childcode[k];
		    childmsg[j]=childmsg[k];
		    childlink[j]=childlink[k];
		    childpos[j]=childpos[k];
		    childnames[j]=childnames[k];
		}
		childindex[j]=-k;	/* flag as done */
		j=k;
	    }
	  }

	  /* Update the indexes to other children and convert them into
	  **  relative indexes.  Also copy bufID (chilkmsg) into childpos.  */
	  for (i=0; i<ncompts; i++) {
	    /* loop through all compartments by elmnum */
	    n=childstart[i];
	    for (j=n; j<childstart[i+1]; j++) {
	    /* loop through all the children of this comp */
		if (childindex[j]>=0) {
		    Error();
		    printf(" during SETUP of %s: bug #3 in do_hget_children.\n",Pathname(hsolve));
		    return(0);
		}
		switch (childtypes[j]) {
		    case CACONCEN_T:
		    case POOL_T:
		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
		    case SPIKEGEN_T:
			/* don't change anything, no indexes */
			break;
		    
		    case MGBLOCK_T:
		    case POOLBUFF_T:
		    case FIXBUFF_T:
		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
		    case NEUTRAL_T:
			childpos[j]=childmsg[j];
			childmsg[j]=0;	/* no change in HILLPUMP_T */
			/* NO BREAK */

		    case HILLPUMP_T:
			/* change link to pool */
			if (childmsg[j]>0) {
			    childmsg[j]=childrank[childmsg[j]];
			} else if (childmsg[j]<0) {
			    childmsg[j]=-childrank[-childmsg[j]];
			}
			/* NO BREAK */

		    case MMPUMP_T:
		    case ELMMPUMP_T:
		    case TAUPUMP_T:
			/* change only childlink: difshell */
			if (childlink[j]>0) {
				childlink[j]=childrank[childlink[j]];
			} else if (childlink[j]<0) {
				childlink[j]=-childrank[-childlink[j]];
			}
			break;
		    
		    default:
			/* assume everything are indexes */
			if (childcode[j]>0) {
			    childcode[j]=childrank[childcode[j]];
			} else if (childcode[j]<0) {
			    childcode[j]=-childrank[-childcode[j]];
			}
			if (childmsg[j]>0) {
			    childmsg[j]=childrank[childmsg[j]];
			} else if (childmsg[j]<0) {
			    childmsg[j]=-childrank[-childmsg[j]];
			}
			if (childlink[j]>0) {
			    childlink[j]=childrank[childlink[j]];
			} else if (childlink[j]<0) {
			    childlink[j]=-childrank[-childlink[j]];
			}
			if (childpos[j]>0) {
			    childpos[j]=childrank[childpos[j]];
			} else if (childpos[j]<0) {
			    childpos[j]=-childrank[-childpos[j]];
			}
			break;
		}
	    }
	  }
	  free(childindex);	/* no longer needed */
	  free(childrank);	/* no longer needed */
	}	/* end of children specific code */

	if (hsolve->chanmode>=4) {
	/* resolve givals indexes */
	    compgiv=hsolve->compgiv=(int *)calloc(ncompts,sizeof(int));
	    childgiv=hsolve->childgiv=(int *)calloc(nchildren+1,sizeof(int));
	    k=1;	/* index givals positions: starts at 1 */
	    for (i=0; i<ncompts; i++) {
	    /* loop through all compartments by elmnum */
		n=childstart[i];
		k+=2;	/* space for Rm and Ileak */
		for (j=n; j<childstart[i+1]; j++) {
		    switch (childtypes[j]) {
			case TABCHAN_T:
			case TAB2CHAN_T:
			case CHANC2_T:
			case CHANC3_T:
			case SYNCHANC_T:
			case MGBLOCK_T:
			case TABCURR_T:
			    childgiv[j]=k;
			    k+=3;
			    break;
				
			case ELMMPUMP_T:
			    childgiv[j]=k;
			    k+=1;
			    break;

			default:
			    childgiv[j]=0;  /* a bit of core dump prevention */
			    break;
		    }
		}
		compgiv[i]=k;
		k++;	/* space for Im value */
	    }
	    l=k;	/* offset for ovals */
	    for (j=1; j<=nchildren; j++) {
		switch (childtypes[j]) {
		    case TAUPUMP_T:
			childgiv[j]=k;
			k+=1;
			break;
			    
		    default:
			break;
		}
	    }
	    hsolve->ngivals=k;
	    hsolve->givals=(double *)calloc(k,sizeof(double));
	    hsolve->givals[0]=0.0;	/* all children without gival */
	    hsolve->ovals=hsolve->givals+l*sizeof(double);
	}

	if (nchildren) {
	  /* resolve indexes to to conc array */
	  firstconc=hsolve->fconc=(int *)calloc(ncompts,sizeof(int));
	  nconcs=0;
	  for (i=0; i<ncompts; i++) {
	  /* loop through all compartments by elmnum */
	    firstconc[i]=nconcs;
	    n=childstart[i];
	    /* find all the ordered conc children in this compartment */
	    /* we do three switches: first one allocates conc entries for
	    **  non-diffusion elms */
	    /* TO CHANGE: this is optimized for one diffusion system */
	    for (j=n; j<childstart[i+1]; j++) {
		child=children[j];
		switch (childtypes[j]) {
		    case CACONCEN_T:
			if (childmsg[j]==-1) {	/* is used by a nernst or ghk */
			    m=childcode[j];	/* concID */
			    if (childpos[j]==SUPRAMEMBRANE) {
				if ((childtypes[j-1]<CACONCEN_T) ||
				    (childcode[j-1]!=m) ||
				    (childpos[j-1]!=SUBMEMBRANE)) {
				    /* need to insert a dummy SUBMEMBRANE conc*/
				    nconcs++;
				    childmsg[j]=nconcs;
				}
			    } else if (childpos[j]==SUBMEMBRANE) {
				if ((childtypes[j+1]<CACONCEN_T) ||
				    (childcode[j+1]!=m) ||
				    (childpos[j+1]!=SUPRAMEMBRANE)) {
				    /* need to append dummy SUPRAMEMBRANE conc*/
				    childmsg[j]=nconcs;	
				    nconcs++;
				}
			    }
			} else {
				childmsg[j]=nconcs;
			}
			nconcs++;
			break;

		    case POOL_T:
		    case POOLBUFF_T:
			/* concs are defined later */
			childmsg[j]=nconcs;
			nconcs++;	/* free buffer */
			break;

		    default:
			break;
		}
	      }
	    /* find all the ordered conc children in this compartment */
	    /* second one allocates conc entries for diffusion elms */
	    for (j=n; j<childstart[i+1]; j++) {
		child=children[j];
		switch (childtypes[j]) {
		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
			if (childmsg[j]==-1) {	/* is used by a nernst or ghk */
			    m=childcode[j];	/* concID */
			    if (childpos[j]==SUPRAMEMBRANE) {
				if ((childtypes[j-1]<CACONCEN_T) ||
				    (childcode[j-1]!=m) ||
				    (childpos[j-1]!=SUBMEMBRANE)) {
				    /* need to insert a dummy SUBMEMBRANE conc*/
				    nconcs++;
				    childmsg[j]=nconcs;
				}
			    } else if (childpos[j]==SUBMEMBRANE) {
				if ((childtypes[j+1]<CACONCEN_T) ||
				    (childcode[j+1]!=m) ||
				    (childpos[j+1]!=SUPRAMEMBRANE)) {
				    /* need to append dummy SUPRAMEMBRANE conc*/
				    childmsg[j]=nconcs;	
				    nconcs++;
				}
			    }
			} else {
				childmsg[j]=nconcs;
			}
			nconcs++;
			break;

		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
			childmsg[j]=nconcs;
			nconcs+=2;	/* free and bound buffer */
			break;

		    case FIXBUFF_T:
		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
			/* concs are defined later */
			childmsg[j]=nconcs;
			nconcs++;	/* free buffer */
			break;

		    default:
			break;
		}
	      }
	      /* update non-concen pointers pointing to concen and
	      ** check if conc and buffer diffusion are consistent */
	      for (j=n; j<childstart[i+1]; j++) {
		child=children[j];
		ct=childtypes[j];
		switch (ct) {
		    case NERNST_T:
		    case GHK_T:
			m=childlink[j];	/* index of conc */
			if (childpos[m]==SUBMEMBRANE) {
			    childmsg[j]=childmsg[m];
			} else {     /* (dummy) SUBMEMBRANE conc was defined */	
			    childmsg[j]=childmsg[m]-1;
			}
			break;

		    case DIFSHELL_T:
		    case DIFSLAB_T:
		    case DIFUSER_T:
			m=childcode[j];
			l=childlink[j] % FLUXOFFSET;	/* type of diffusion */
			if (((childtypes[j-1]==ct)&& 
			   ((childcode[j-1]==m)&&(l!=DIFFDOWN)&&(l!=DIFFBOTH))||
			  ((childcode[j-1]!=m)&&((l==DIFFDOWN)||(l==DIFFBOTH))))
			   || ((childtypes[j+1]==ct)&& 
			   ((childcode[j+1]==m)&&(l!=DIFFUP)&&(l!=DIFFBOTH)) ||
			   ((childcode[j+1]!=m)&&((l==DIFFUP)||(l==DIFFBOTH))))) {
			    Error();
			    printf(" during SETUP of %s: bug #6 in do_hget_children.\n",Pathname(hsolve));
			    return(ERR);
			}
			break;

		    case HILLPUMP_T:
			m=childlink[j];	/* index of difshell */
			childcode[j]=m;	/* store it */
			k=childmsg[j];
			if (k) {	/* msg to concpool present */
			    childlink[j]=childmsg[k];
			    childpos[j]=k;
			} else {
			    childlink[j]=0;
			    childpos[j]=0;
			}
			childmsg[j]=childmsg[m];
			break;

		    case MMPUMP_T:
		    case ELMMPUMP_T:
		    case TAUPUMP_T:
			m=childlink[j];	/* index of difshell */
			childmsg[j]=childmsg[m];
			break;

		    case POOLBUFF_T:
		    case FIXBUFF_T:
			m=childlink[j];	/* index of difshell */
			k=childlink[j]=childmsg[m];
			break;

		    case BUFSHELL_T:
		    case BUFSLAB_T:
		    case BUFUSER_T:
		    case BUF2SHELL_T:
		    case BUF2SLAB_T:
		    case BUF2USER_T:
			m=childlink[j];	/* index of difshell */
			k=childlink[j]=childmsg[m];
			l=childlink[m] % FLUXOFFSET;	/* type of diffusion */
			elm=hget_msg_elm(hsolve,j,BDIFF_UP,&msgdata,NOERR);
			elm2=hget_msg_elm(hsolve,j,BDIFF_DOWN,&msgdata,NOERR);
			if ((elm&&(l!=DIFFDOWN)&&(l!=DIFFBOTH)) ||
			    (!elm&&((l==DIFFDOWN)||(l==DIFFBOTH))) ||
			    (elm2&&(l!=DIFFUP)&&(l!=DIFFBOTH)) ||
			    (!elm2&&((l==DIFFUP)||(l==DIFFBOTH)))) {
			    Error();
			    printf(" during SETUP of %s: inconsistency between diffusion of %s and difshell diffusion.\n",hsolve->name,child->name);
			    return(ERR);
			}
			if (!elm && !elm2) {
			/* no diffusion at all! -> change to a FIXBUFF_T */
			    childtypes[j]=FIXBUFF_T;
			}
			break;

		    default:
			break;
		}
	    }
	  }
	  hsolve->nconcs=nconcs;
	}
	return(0);
}

int hstore_name(hsolve,elm,nnames,names,elmname)
/* Compares name of elm with list of stored names and returns index of
**  the name.  Stores the name if new (allocates memory).
** The name can be elm->name or a *char elmname
*/
	Hsolve	*hsolve;
	Element	*elm;
	int     *nnames;
	char    **names;
	char	*elmname;
{
	int     i;
	char	*name;

	if (hsolve->no_elminfo) return(NOT);

	if (elm) {	/* called by h_hh_chip_init */
	    name=elm->name;
	} else {	/* called by h_rchip_init */
	    name=elmname;
	}
	if (!name) return(NOT);

	for (i=0;i<*nnames;i++) {
	    if (strcmp(name,names[i])==0) {
		return(i);
	    }
	}

	/* not found -> store it */
	(*nnames)++;
	if (*nnames==MAXNAMES) {	/* only one error message! */
	    Error();
	    printf(" during SETUP of %s: too many element names.\n",Pathname(hsolve));
	    return(ERR);
	}
	if (*nnames<MAXNAMES) {
	    names[i]=(char *)calloc(strlen(name)+1,sizeof(char));
	    strcpy(names[i],name);
	    return(i);
	}
	return(ERR);		/* added by Greg Hood, PSC, 9/14/98 */
}

Element *hget_msg_elm(hsolve,nchild,msgtype,msgdata,msgnum)
/* Returns the source of the message of msgtype send to elm and returns
**   pointer in msgfound.
** If msgnum==MONE then only one message of msgtype allowed.
** If msgnum>0 find this number of msgtype.
*/
	Hsolve	*hsolve;
	int	nchild,msgtype,msgnum;
	double	*msgdata;
{
	MsgIn   *msgin;
	Element *elm,*src;
	int	n=0;

	src=NULL;
	elm=hsolve->children[nchild];
	MSGLOOP(elm, msgin) {
	    default:
		if (msgin->type == msgtype) {
		    n++;
		    if ((msgnum<=0)||(msgnum==n)) {
			src=msgin->src;
			if (msgin->nslots>=2) *msgdata=MSGVALUE(msgin,1);
		    }
		}
		break;
	}
	if (msgnum==MONE) {
	    if (n>1) {
		Error();
		printf(" during SETUP of %s: multiple messages type #%d to %s.\n",Pathname(hsolve),msgtype,Pathname(elm));
	    } else if (n==0) {
		Error();
		printf(" during SETUP of %s: message type #%d to %s not found.\n",Pathname(hsolve),msgtype,Pathname(elm));
	    }
	}
	return(src);
}

int hhas_msg_toelm(hsolve,src,dst,msgtype,errormsg)
/* Returns false if one message msgtype from src to dst exists 
** Outputs error message if errormsg==true */

	Hsolve	*hsolve;
	Element	*src,*dst;
	int	msgtype,errormsg;
{
	MsgIn   *msgin;
	int	n=0;

	MSGLOOP(dst, msgin) {
	    default:
		if ((msgin->src == src) && (msgin->type == msgtype)) n++;
		break;
	}
	if (n>1) {
	    if (errormsg) {
		Error();
		printf(" during SETUP of %s: multiple messages type #%d from %s to %s.\n",Pathname(hsolve),msgtype,Pathname(src),Pathname(dst));
	    }
	    return (ERR);
	} else if (n==0) {
	    if (errormsg) {
		Error();
		printf(" during SETUP of %s: no message type #%d from %s to %s.\n",Pathname(hsolve),msgtype,Pathname(src),Pathname(dst));
	    }
	    return (ERR);
	}
	return(0);
}

Element *hget_msg_dst(hsolve,nchild,msgtype,msgnum)
/* Returns the destination of the message of msgtype send to elm 
** If msgnum==MONE then only one message of msgtype allowed.
** If msgnum>0 find this number of msg with msgtype.
*/
	Hsolve	*hsolve;
	int	nchild,msgtype,msgnum;
{
	MsgIn   *msgin;
	MsgOut  *msgout;
	Element *child,*dst;
	int	n=0;

	dst=NULL;
	child=hsolve->children[nchild];
	MSGOUTLOOP(child, msgout) {
	    msgin = GetMsgInByMsgOut(msgout->dst, msgout);
	    if (msgin->type == msgtype) {
		n++;
		if ((msgnum<=0)||(msgnum==n)) dst=msgout->dst;
	    }
	}
	if ((msgnum==MONE)&&(n>1)) {
	    Error();
	    printf(" during SETUP of %s: multiple messages type #%d from %s.\n",Pathname(hsolve),msgtype,Pathname(child));
	} else if ((msgnum>NOERR)&&(msgnum<1)&&(n==0)) {
	    Error();
	    printf(" during SETUP of %s: message type #%d from %s not found.\n",Pathname(hsolve),msgtype,Pathname(child));
	}
	return(dst);
}

int hget_child(hsolve,elm,type,start)
/* Returns the index in children of elm.
** If type>0 then checks if elm is of appropriate type.
** Starts looking at index start (which can be childstart if available or 1)
** Returns ERR if not found or wrong type.
*/
	Hsolve	*hsolve;
	Element *elm;
	int	type,start;
{
	int	i;
	Element **children;
	short	*childtypes;
	char*   oname;

	if (!elm) {
	    Error();
	    printf(" during SETUP of %s: bug #4 in do_hget_children.\n",Pathname(hsolve));
	    return(ERR);
	}
	if (start<1) start=1;
	children=hsolve->children;
	childtypes=hsolve->childtypes;

	for (i=start; i<=hsolve->nchildren; i++) {
	    if (children[i]==elm) {
		oname = BaseObject(elm)->name;
		switch (type) {
		    case ANYCHAN_T:
			if ((strcmp(oname,"tabchannel")==0) ||
			    (strcmp(oname,"tab2Dchannel")==0) ||
			    (strcmp(oname,"tabcurrent")==0) ||
			    (strcmp(oname,"channelC2")==0) ||
			    (strcmp(oname,"channelC3")==0) ||
			    (strcmp(oname,"synchan")==0) ||
			    (strcmp(oname,"synchan2")==0) ||
			    (strcmp(oname,"Mg_block")==0) ||
			    (strcmp(oname,"neutral")==0) ||
			    (strcmp(oname,"ghk")==0)) return(i);
			break;
		    case SYNCHAN_T:
			if ((strcmp(oname,"channelC2")==0) ||
			    (strcmp(oname,"channelC3")==0) ||
			    (strcmp(oname,"synchan2")==0) ||
			    (strcmp(oname,"synchan")==0)) return(i);
			break;
		    case CACONCEN_T:
			if ((strcmp(oname,"Ca_concen")==0) ||
			    (strcmp(oname,"difshell")==0)) return(i);
			break;
		    case POOL_T:
			if ((strcmp(oname,"Ca_concen")==0) ||
			    (strcmp(oname,"difshell")==0) ||
			    (strcmp(oname,"concpool")==0)) return(i);
			break;
		    case DIFSHELL_T:
			if (strcmp(oname,"difshell")==0) return(i);
			break;
		    case BUFSHELL_T:
			if (strcmp(oname,"difbuffer")==0) return(i);
			break;
		    case NERNST_T:
			if (strcmp(oname,"nernst")==0) return(i);
			break;
		    case GHK_T:
			if (strcmp(oname,"ghk")==0) return(i);
			break;
		    case TABGHK_T:
			if (strcmp(oname,"tabcurrent")==0) return(i);
			break;
		    case NEUTRAL_T:
		    default:	/* not checked */
			return(i);
		}
	    }
	}
	Error();
	if (i<=hsolve->nchildren) {
	    printf(" during SETUP of %s: %s is wrong msg source.\n",Pathname(hsolve),Pathname(elm));
	} else {
	    printf(" during SETUP of %s: %s is not a part of this neuron.\n",Pathname(hsolve),Pathname(elm));
	}
	return(ERR);
}

/* Function that compares two children, returns true if first<second
** The order is: SPIKEGEN_T
**               TABCHAN_T, ordered by conc related links, for same conc:
**                          descending by source of CONCEN first 
**                          and by Ek second
**               CHANC2_T, CHANC3_T, SYNCHANC_T, ordered by Ek
**               NERNST_T and GHK_T (will be interspersed later)
**  all in order of concID (==childcode) and mixed in order of childpos:
**               CACONCEN_T-FURA2_T
**		 POOLBUFF_T-POOL_T
**               DIFSHELL_T-DIFUSER_T
**               FIXBUFF_T in order of childmsg
**               BUFSHELL_T-BUFUSER_T in order of childmsg
**               BUF2SHELL_T-BUF2USER_T in order of childmsg
*/
int hchild_compare(hsolve,first,sec)
/* returns true if first belongs before sec */
	Hsolve	*hsolve;
	int	first,sec;
{
	Element **children;
	short	*childtypes;
	int	*childlink,*childpos,*childmsg,*childcode;
	int	ct1,ct2,conc1,conc2;

	children=hsolve->children;
	childtypes=hsolve->childtypes;
	childlink=hsolve->childlink;
	childpos=hsolve->childpos;
	childmsg=hsolve->childmsg;
	childcode=hsolve->childcode;
	ct1=childtypes[first];
	ct2=childtypes[sec];
	if (ct2==TAB2CHAN_T) ct2=TABCHAN_T;	/* no distinction for ordering */
	if (ct2==TABCURR_T) ct2=TABCHAN_T;

	if ((ct1>=CACONCEN_T)&&(ct2>=CACONCEN_T)) {
	    if (childcode[first]<childcode[sec]) {
		return(1);
	    } else if (childcode[first]==childcode[sec]) {
		/* store all childs from same shell consecutively */
		if (childpos[first]<childpos[sec]) {
		    return(1);
		} else if (childpos[first]>childpos[sec]) {
		    return(0);
		} else {
		    if (ct1<ct2) {
			return(1);
		    } else if (ct1==ct2) {
			if (ct1<DIFSHELL_T) {	/* BUF */
			    if (childmsg[first]<childmsg[sec]) {
				return(0);
			    } else if (childmsg[first]>childmsg[sec]) {
				return(1);
			    } else {
				Error();
				printf(" during SETUP of %s: Buffer %s is identical to buffer %s. %d %d %d %d\n",hsolve->name,children[first]->name,children[sec]->name,childcode[first],childcode[sec],childpos[first],childpos[sec]);
				return(0);
				    }
			} else {
			    Error();
			    printf(" during SETUP of %s: bug #1 in do_hget_children.\n",Pathname(hsolve));
			    return(0);
			}
		    } 
		}
	    }
	} else if (ct1<ct2) {
	    return(1);
	} else if (ct1==ct2) {
	    if ((ct1>=TABCHAN_T)&&(ct1<=TABCURR_T)) {
		conc1=abs(childpos[first]);	/* CONCEN msg */
		if (conc1==0) {
		    if (childmsg[first]!=0) {
			conc1=childlink[abs(childmsg[first])]; /*nernst or ghk*/
		    } else {
			conc1=abs(childlink[first]);	/* flux */
		    }
		}
		conc2=abs(childpos[sec]);	/* CONCEN msg */
		if (conc2==0) {
		    if (childmsg[sec]!=0) {
			conc2=childlink[abs(childmsg[sec])]; /* nernst or ghk */
		    } else {
			conc2=abs(childlink[sec]);	/* flux */
		    }
		}
		if (conc1<conc2) {
		    return(1);
		} else if (conc1==conc2) {
		    if (abs(childmsg[first])<abs(childmsg[sec])) {
			return(1);
		    } else if ((childmsg[first]==0)&&(childmsg[sec]==0)) {
			if (((Tchan *)children[first])->Ek<((Tchan *)children[sec])->Ek) return(1);
		    }
		}
	    } else if ((ct1>=CHANC2_T)&&(ct1<NERNST_T)) {
		if (abs(childmsg[first])<abs(childmsg[sec])) return(1);
	    }
	}
	return(0);
}

/* Routine to move children around after they have been sorted initially.
** now: real index of child
** goal: childrank location child should move to
** start,end: range of child indexes for this compartment
*/

void h_sort_children(hsolve,childrank,childindex,now,goal,start,end)
	Hsolve	*hsolve;
	int	*childrank,*childindex;
	int	now,goal,start,end;
{
	int	i,n,rank;
	Element **children;
	short	*childtypes;
	int		*childlink,*childpos,*childmsg,*childcode;
	children=hsolve->children;
	childtypes=hsolve->childtypes;
	childlink=hsolve->childlink;
	childpos=hsolve->childpos;
	childmsg=hsolve->childmsg;
	childcode=hsolve->childcode;

	rank=childrank[now];
	if (rank==goal) return;
	if (rank<goal-1) goal--;	/* we will shift this child upward */
/*
if (start<100) {
printf("sort %d: %d->%d (%d-%d)\n",now,rank,goal,start,end);
for (i=start; i<end; i++) {
k=i;
printf("%d %d %s: %d %d %d %d %d %d %d\n",i,k,children[k]->name,childtypes[k],childcode[k],childmsg[k],childlink[k],childpos[k],childindex[k],childrank[k]);
}
}
*/
	for (i=start; i<end; i++) {
	    n=childrank[i];
	    if ((n>rank)&&(n<=goal)) {
		childrank[i]-=1;
	    } else if ((n>=goal)&&(n<rank)) {
		childrank[i]+=1;
	    }
	}
	childrank[now]=goal;
	for (i=start; i<end; i++) childindex[childrank[i]]=i;
/*
if (start<100) {
for (i=start; i<end; i++) {
k=i;
printf("%d %d %s: %d %d %d %d %d %d %d\n",i,k,children[k]->name,childtypes[k],childcode[k],childmsg[k],childlink[k],childpos[k],childindex[k],childrank[k]);
}
printf("\n");
}
*/
}
