static char rcsid[] = "$Id: hines.c,v 1.4 2006/01/10 08:59:01 svitak Exp $";

/* Version EDS22h 99/06/08, Erik De Schutter, Caltech & BBF-UIA 4/92-6/99 */
/* Upinder S. Bhalla Caltech May-December 1991 */

/*
** $Log: hines.c,v $
** Revision 1.4  2006/01/10 08:59:01  svitak
** Reordered functions to match 2.2.1 tree. Changed function types back to int.
**
** Revision 1.3  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:40  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.16  2000/06/15 06:47:54  mhucka
** New update from Hugo Cornelis, makes hsolve give an error msg when it is
** run without a rest.  An associated fix for neurokit is included in
** neurokit/xrun.g.
**
** EDS22j revision: HC BBF-UIA 00/06/14
** Added checks for reset
**
** Revision 1.15  1999/10/17 23:15:39  mhucka
** Attempted to merge in changes from Greg Hood for fixing problems on
** the Cray T3E.  The same changes also make it possible to compile with
** optimization turned on under Red Hat 6.0's default version of gcc/egcs.
**
** EDS22g revison: EDS BBF-UIA 99/06/07-99/06/08
** Corrected chanmode 0 reset
**
** EDS22g revison: EDS BBF-UIA 99/02/11-99/02/11
** Implemented chanmode 5
**
** EDS22d revison: EDS BBF-UIA 98/05/04-98/09/30
** Corrected CREATE statement
**
** EDS22c revison: EDS BBF-UIA 97/11/28-98/03/09
** Made symmetric compartments work
** Deleted use of values array
**
** EDS22b revison: EDS BBF-UIA 97/09/05-97/11/25
** Corrected several small bugs (CREATE, chanmode 4 behavior)
**
** Revision 1.9  1997/08/01 17:50:34  dhb
** Fixed SAVE2 action which was calling write() instead of fwrite().
**
** Revision 1.8  1997/07/29 00:22:12  dhb
** Fix in h_delete() where using == where it should be an
** assignment.
**
** Revision 1.7  1997/06/03 18:34:45  dhb
** Bug fixes as directed from Erik DeShutter:
**   - fix to CREATE action code to set field defaults and break out of
**     CREATE case
**   - uncommented code in h_failed() to clean up after failed hsolve setup
**
** Revision 1.6  1997/05/28 22:50:10  dhb
** Replaced with version from Antwerp GENESIS 21e
**
** Revision 1.5  1995/12/06 00:46:57  venkat
** DESTROY method changed to free the element array if allocated
**
** Revision 1.4  1995/08/01  19:29:03  dhb
** Changes from Erik DeSchutter described below.
**
** updated h_delete routine for msgcompts
**
** Revision 1.3  1995/07/18  18:07:14  dhb
** Changed all check of object names to check for name of base
** object.  This allows hsolve to solve for extended objects.
**
** Revision 1.2  1992/10/29  16:36:50  dhb
** Changed to use MSGLOOP macro instead of explicit use of MsgIn fields.
** Also, did major reformating of indentation in affected areas.
**
*/

#include "hines_ext.h"

/* We would like to acknowlege the advice of Dr. Michael Mascagni
** during the development of this element.
** 
** Element for applying Hines numbering method to solving the cable
** properites of a cell in an implicit solution.
** The element sets up a data structure whereby the only change
** needed for other identical cells is to reassign pointers to
** the compartment and channel elements.
** This avoids having to reallocate and rederive the solving scheme.
** Solutions are done using gaussian forward  and backward elimination
** without pivoting. The numbering and evaluation sequence ensures that
** no new off-diagonal terms terms are formed which might mess up
** the sparse matrix representation.
*/

/* changed most uses of ERR (defined in hines_defs.h as -1) to FAILURE in
   this file to make consistent with the convention for object action return
   values in GENESIS  (Greg Hood, PSC, 9/14/98) */
#define SUCCESS		1
#define FAILURE		0

int DEBUG_HinesSolver=0;

int HinesSolver(hsolve,action)
Hsolve	*hsolve;
Action	*action;
{
	int	i,n;
	int	comptno,cindex;
	int	nop,time;
	double	v0,v1;
	Element *elm;

	if(Debug(DEBUG_HinesSolver) > 1){
	    ActionHeader("HinesSolver",hsolve,action);
	}
	SELECT_ACTION(action){
	    case CREATE:
		if (IsSilent()<0) printf("creating hsolve element %s\n",Pathname(hsolve));
		hsolve->calcmode=LIN_INTERP;	/* DEFAULT */
		/* comptmode is no longer relevant */
		hsolve->comptmode=1;		/* DEFAULT: only mode */
		if (!hsolve->path) {
		    hsolve->path=(char *)calloc(30,sizeof(char));
		    strcpy(hsolve->path,"../##[][TYPE=compartment]"); 
		}
		return(1);	/* needed to work properly! */

	    case PROCESS:
		    /* Doing the t(n+1/2) evaluation of hh channels */
		    switch (hsolve->chanmode) {
			case 0:
			case 1:
			    if (!hsolve->nfuncs || !hsolve->funcs)
			    {
			    	ErrorMessage
				    ("Solver PROCESS",
				     "reset required",
				     hsolve);
				return(ERR);
			    }
			    if (hsolve->chanmode) do_hh_update(hsolve);
			    do_hcalc(hsolve);
			    do_fast_hsolve(hsolve);
			    do_compt_update(hsolve);
			    break;
			case 2:
			    if (hsolve->ininfo) h_in_msgs(hsolve);
			    if (hsolve->calcmode) {
				do_chip_hh2_update(hsolve);
			    } else {
				do_chip_hh2noi_update(hsolve);
			    }
			    if (hsolve->nconcchips) 
					do_h2_conc_chip_update(hsolve);
			    if (hsolve->ndiffs) do_h_conc_solve(hsolve);
			    if (BaseObject(hsolve)->method == CRANK_INT) {
				do_crank_hsolve(hsolve);
			    } else {
				do_euler_hsolve(hsolve);
			    }
			    do_vm_update(hsolve);
			    break;
			case 3:
			    if (hsolve->ininfo) h_in_msgs(hsolve);
			    if (hsolve->calcmode) {
				do_chip_hh2_update(hsolve);
			    } else {
				do_chip_hh2noi_update(hsolve);
			    }
			    if (hsolve->nconcchips) 
					do_h2_conc_chip_update(hsolve);
			    if (hsolve->ndiffs) do_h_conc_solve(hsolve);
			    if (BaseObject(hsolve)->method == CRANK_INT) {
				do_crank_hsolve(hsolve);
			    } else {
				do_euler_hsolve(hsolve);
			    }
			    if (hsolve->outinfo) h_out_msgs(hsolve);
			    break;
			case 4:
			case 5:
			    if (hsolve->ininfo) h_in_msgs(hsolve);
			    if (hsolve->calcmode) {
				do_chip_hh4_update(hsolve);
			    } else {
				do_chip_hh4ni_update(hsolve);
			    }
			    if (hsolve->nconcchips) 
					do_h4_conc_chip_update(hsolve);
			    if (hsolve->ndiffs) do_h_conc_solve(hsolve);
			    if (BaseObject(hsolve)->method == CRANK_INT) {
				do_crank_hsolve(hsolve);
			    } else {
				do_euler_hsolve(hsolve);
			    }
			    if (hsolve->outinfo) h_out_msgs(hsolve);
			    break;
			default:
			    /*- give diagnostics */

			    ErrorMessage
				("Solver PROCESS",
				 "Illegal field value (chanmode)",
				 hsolve);

			    /*- return failure */

			    return(ERR);
		    }
		break;
	    case RESET:
		if ((hsolve->readflag>=HSETUP_T)&&hsolve->storemode&&(IsSilent()<0)) {
		    for (i=0; i<hsolve->nelm_names; i++) 
			    printf("storing %s in itotal[%d]\n",hsolve->elm_names[i],i);
		}
		if (hsolve->chanmode >= 4
		    && (!hsolve->givals)
		    && hsolve->chanmode != -1)
		{
		    hsolve->chanmode = 3;
		}
		if (hsolve->readflag==HREAD_T) {
		    if (hsolve->numnodes) {
			for (i=0; i<hsolve->numnodes; i++) 
				hsolve->sendid[i]=-1;	/* flag as empty */
		    }
		    return(SUCCESS);
		}
		switch (hsolve->chanmode) {
		    /*- for illegal values */

		    case -1:
		    {
			/*- give diagnostics */

			ErrorMessage
			    ("Solver RESET",
			     "Illegal field value (chanmode)",
			     hsolve);

			/*- return failure */

			return(FAILURE);
		    }

		    /*- for 0,1 */

		    case 0:
		    case 1:

			/*- reset the elements */

			do_hreset(hsolve);
			break;

		    case 2:
		    case 3:
		    case 4:
		    case 5:
			do_chip_hreset(hsolve,"RESET");
			if (hsolve->readflag==HDUPLICATE_T) copychipdiags(hsolve);
			if (hsolve->ndiffs) {
			    if (h_setup_conc_solve(hsolve)) {
				h_failed(hsolve);
				return(FAILURE);
			    }
			}
			break;
		}
		if (hsolve->readflag!=HDUPLICATE_T) {
		/* The second pass assigns the diagonals, funcs and ravals */
		    n=h_funcs_init(hsolve); 
		} else {
		    hsolve->funcs=hsolve->origsolve->funcs;
		    hsolve->ravals=hsolve->origsolve->ravals;
		}
		break;

	    case HSEVENT:
		/* firstevent in synchan #argc changed */
		time = * (int*) action->argv;
		nop=hsolve->childops[action->argc]+2;
		hsolve->ops[nop]=time;
		break;

	    case SETUP:
		if (hsolve->readflag==HREAD_T) {
		    ErrorMessage("Solver SETUP","SETUP not possible for solver created by readsolve.",hsolve);
		    return(FAILURE);
		}

		/*- if solver has illegal field values */

		if (hsolve->chanmode > 5
		    || hsolve->chanmode < 0)
		{
		    /*- give diagnostics */

		    ErrorMessage
			("Solver SETUP",
			 "Illegal field value (chanmode)",
			 hsolve);

		    /*- flag chanmode as invalid */

		    hsolve->chanmode = -1;

		    /*- return failure */

		    return(FAILURE);
		}

		/* all sorts of memory allocation */
		if (hsolve->ncompts) break;	/* already done */
		hsolve->dt=Clockrate(hsolve);
		if (!hsolve->dt) {
		    ErrorMessage("Solver SETUP","clock not set.",hsolve);
		    return(FAILURE);
		}
		if (h_init(hsolve)) {
		    h_failed(hsolve);
		    return(FAILURE);
		}
		hsolve->nfuncs = 0;
		if (hsolve->readflag>=HSETUP_T) {
		    /* The first pass counts the number of funcs and ravals
		    **  entries */
		    if (h_funcs_init(hsolve)) {
			h_failed(hsolve);
			return(FAILURE);
		    }
		}
		/* Set up tabchannels */
		if (h2_init(hsolve)) {
		    h_failed(hsolve);
		    return(FAILURE);
		}
		switch (hsolve->chanmode) {
		    case 0:
		    case 1:
			if (h_hh_init(hsolve)) {
			    h_failed(hsolve);
			    return(FAILURE);
			}
			break;
		    case 2:
		    case 3:
		    case 4:
		    case 5:
			if (do_hget_children(hsolve)) {
			    h_failed(hsolve);
			    return(FAILURE);
			}
			if (h_hh_chip_init(hsolve)) {
			    h_failed(hsolve);
			    return(FAILURE);
			}
			if (hsolve->nconcs) {
			    if (h_init_conc_chip(hsolve)) {
				h_failed(hsolve);
				return(FAILURE);
			    }
			    if (h_init_conc_solve(hsolve)) {
				h_failed(hsolve);
				return(FAILURE);
			    }
			}
			break;
		    default :
			if (h_hh_init(hsolve)) {
			    h_failed(hsolve);
			    return(FAILURE);
			}
			break;
		}
		if (hsolve->outinfo) {
		    if (hsolve->outclock) {
			v0=ClockValue(hsolve->outclock);
		    } else {
			v0=Clockrate(hsolve);
		    }
		    if (v0<hsolve->dt) {
			printf("** Warning - during SETUP of %s: outclock faster (%g) than solve clock (%g)!\n",Pathname(hsolve),v0,hsolve->dt);
			hsolve->outclock=Clock(hsolve);
		    }
		}
		break;

	    case DELETE:
		h_delete(hsolve);
		break;

	    case DUPLICATE:
		if (hsolve->readflag == HREAD_T) {
		    ErrorMessage("Solver DUPLICATE","not possible for solver created by readsolve.",hsolve);
		    return(FAILURE);
		}
		if (do_duplicate(hsolve,action->argv[0],action->argv[1])) {
		    ErrorMessage("hsolve DUPLICATE","cell is not identical.",hsolve);
		}
		break;

	    case HPUT:
	    case HGET:
		if (action->argc < 1) {
		    if (action->type == HPUT) {
			printf("usage: HPUT element_path\n");
			printf("updates any reference to element fields in hsolve\n");
		    } else {
			printf("usage: HGET element_path\n");
			printf("updates element fields from hsolve arrays\n");
		    }
		    return(FAILURE);
		}
		if (hsolve->no_elminfo) {
		    ErrorMessage("Solver","HPUT/HGET calls do not work if the no_elminfo field is set.",hsolve);
		    return(FAILURE);
		}
		if (hsolve->chanmode < 2) {
		    ErrorMessage("Solver","HPUT/HGET calls should only be used when chanmode >= 2.",hsolve);
		    return(FAILURE);
		}
		if (hsolve->readflag == HREAD_T) {
		    ErrorMessage("Solver","HPUT/HGET not possible for solver created by readsolve.",hsolve);
		    return(FAILURE);
		}
		if (hsolve->ncompts == 0) {
		    ErrorMessage("Solver","no SETUP call.",hsolve);
		    return(FAILURE);
		}
		elm = GetElement(action->argv[0]);
		if (!elm) {
		    ErrorMessage("Solver","HPUT/HGET element not found.",hsolve);
		    return(FAILURE);
		}
		if (!IsHsolved(elm)) {
		    ErrorMessage("Solver","HPUT/HGET element not handled by ",hsolve);
		    return(FAILURE);
		}
		if (hfind_elm(hsolve,elm,&comptno,&cindex)==ERR) {
		    ErrorMessage("Solver","HPUT/HGET cannot find element.",hsolve);
		    return(FAILURE);
		}
		if (action->type == HPUT) {
		    hput_elm(hsolve,comptno,cindex);
		} else {
		    hget_elm(hsolve,comptno,cindex);
		}
		break;

	    case HRESTORE:
	    case HSAVE:
		if (hsolve->chanmode < 2) {
		    ErrorMessage("Solver","HRESTORE/HSAVE calls should only be used when chanmode >= 2.",hsolve);
		    return(FAILURE);
		}
		if (hsolve->readflag == HREAD_T) {
		    ErrorMessage("Solver","HRESTORE/HSAVE not possible for solver created by readsolve.",hsolve);
		    return(FAILURE);
		}
		if (action->type == HRESTORE) {
		    do_chip_hreset(hsolve,"NONE");
		} else {
		    do_chip_hsave(hsolve);
		}
		break;

	    case HCHANGE:
		if (action->argc < 4) {
		    printf("usage: HCHANGE ops_code offset oldvalue newvalue\n");
		    return(FAILURE);
		}
		n=atoi(action->argv[0]);
		i=atoi(action->argv[1]);
		v0=(double)Atof(action->argv[2]);
		v1=(double)Atof(action->argv[3]);
		h_rchip_change(hsolve,n,i,v0,v1);
		break;

	    case SAVE2:
		n=hsolve->nchips+hsolve->ncompts+hsolve->nconcs;
		fwrite(&n,sizeof(int),1,(FILE*)action->data);
		fwrite(hsolve->chip,sizeof(double),hsolve->nchips,(FILE*)action->data);
		fwrite(hsolve->vm,sizeof(double),hsolve->ncompts,(FILE*)action->data);
		fwrite(hsolve->conc,sizeof(double),hsolve->nconcs,(FILE*)action->data);
		break;

	    case RESTORE2:
		n=hsolve->nchips+hsolve->ncompts+hsolve->nconcs;
		fread(&i,sizeof(int),1,(FILE*)action->data);
		if (i!=n) {
		    ErrorMessage("Solver","invalid savedata length.",hsolve);
		    return(FAILURE);
		}
		fread(hsolve->chip,sizeof(double),hsolve->nchips,(FILE*)action->data);
		fread(hsolve->vm,sizeof(double),hsolve->ncompts,(FILE*)action->data);
		fread(hsolve->conc,sizeof(double),hsolve->nconcs,(FILE*)action->data);
		break;
	}
	return(SUCCESS);
}

int h_failed(hsolve)
    Hsolve	*hsolve;
{
    ErrorMessage("Solver","SETUP failed.",hsolve);
    h_delete(hsolve);
    DeleteElement(hsolve);
}

/* Simple function for putting data values back into elements */
/* CHANMODE 0-1 */
int do_hreset(hsolve)
    Hsolve	*hsolve;
{
	int i;
	struct compartment_type **compts,*compt;
	int ncompts = hsolve->ncompts;
	int chanmode = hsolve->chanmode;
	int	*elmnum;
	double	*results;
	Action	*action;
	Tcinfo	**hh,*hentry;
	Cinfo   **chan,*centry;
	compts = (struct compartment_type **)(hsolve->compts);
	hh = hsolve->hh;
	chan = hsolve->chan;
	elmnum = hsolve->elmnum;
	results = hsolve->results;

	action = GetAction("RESET");
	hsolve->dt = Clockrate(hsolve);

	for(i=0;i<ncompts;i++) {
	    compt=compts[elmnum[i]];
	    compt->Vm = results[i] = compt->initVm;
	    if (chanmode) {
		for(hentry=hh[i];hentry;hentry=hentry->next) {
		    CallElement(hentry->chan,action,0);
		}
	    }
	    for(centry=chan[i];centry;centry=centry->next) {
		CallElement(centry->chan,action,0);
	    }
	}
}

int h_delete(hsolve)
/* includes corrections for memory leaks found by purify */
/* EDS20l version complete */

	Hsolve	*hsolve;
{
	int i;
	Tcinfo	*hentry,*hlast;
	Cinfo	*centry,*clast;
	Compinfo *comp,*lcomp;

	if (IsSilent() <= 0)
	    printf("deleting hsolve element : re-enabling path\n");
	if (hsolve->ncompts > 0) {
	    if (hsolve->readflag!=HREAD_T) {
		for(i=0;i<hsolve->ncompts;i++) {
		    HsolveEnable(hsolve->compts[i]);
		    if (hsolve->hh) {
			for (hentry=hsolve->hh[i];hentry;){
			    HsolveEnable(hentry->chan);
			    hlast=hentry;
			    hentry=hentry->next;
			    if (hlast) free(hlast);
			}
			hsolve->hh[i]=NULL;
		    }
		    if (hsolve->chan) {
			for(centry=hsolve->chan[i];centry;){
			    clast=centry;
			    centry=centry->next;
			    if (clast) free(clast);
			}
		    }
	    }
	    for(i=1;i<=hsolve->nchildren;i++) HsolveEnable(hsolve->children[i]);
	    if (hsolve->children) free(hsolve->children);
	       hsolve->children=NULL;
	    }
	    for(i=0;i<hsolve->ncompts;i++) {
		if (hsolve->nkids[i] > 0 && hsolve->kids[i]) {
		    free(hsolve->kids[i]);
		    hsolve->nkids[i]=0;
		}
	    }
	    if (hsolve->compts) free(hsolve->compts);
		hsolve->compts=NULL;
		hsolve->ncompts=0;
	    if (hsolve->parents) free(hsolve->parents);
		hsolve->parents=NULL;
	    if (hsolve->kids) free(hsolve->kids);
		hsolve->kids=NULL;
	    if (hsolve->nkids) free(hsolve->nkids);
		hsolve->nkids=NULL;
	    if (hsolve->chanmode >= 2) {
		if (hsolve->vm) free(hsolve->vm);
		    hsolve->vm=NULL;
		if (hsolve->nconcs>0 && hsolve->conc) {
		    free(hsolve->conc);
		    hsolve->conc=NULL;
		    if (hsolve->fconc) free(hsolve->fconc);
			hsolve->fconc=NULL;
		    if (hsolve->flux) free(hsolve->flux);
			hsolve->flux=NULL;
		    if (hsolve->concchip) free(hsolve->concchip);
			hsolve->concchip=NULL;
		    hsolve->nconcchips=0;
		    if (hsolve->concops) free(hsolve->concops);
			hsolve->concops=NULL;
		    hsolve->nconcops=0;
		    if (hsolve->fdiffs) free(hsolve->fdiffs);
			hsolve->fdiffs=NULL;
		    if (hsolve->dchild) free(hsolve->dchild);
			hsolve->dchild=NULL;
		    if (hsolve->dconc) free(hsolve->dconc);
			hsolve->dconc=NULL;
		    if (hsolve->nshells) free(hsolve->nshells);
			hsolve->nshells=NULL;
		    if (hsolve->nshrows) free(hsolve->nshrows);
			hsolve->nshrows=NULL;
		    if (hsolve->nshelms) free(hsolve->nshelms);
			hsolve->nshelms=NULL;
		    if (hsolve->nshfix) free(hsolve->nshfix);
			hsolve->nshfix=NULL;
		    hsolve->ndiffs=0;
		    if (hsolve->covals) free(hsolve->covals);
			hsolve->covals=NULL;
		    if (hsolve->covals0) free(hsolve->covals0);
			hsolve->covals0=NULL;
		    if (hsolve->coresult) free(hsolve->coresult);
			hsolve->coresult=NULL;
		    if (hsolve->cores) free(hsolve->cores);
			hsolve->cores=NULL;
		    if (hsolve->fcoval) free(hsolve->fcoval);
			hsolve->fcoval=NULL;
		    if (hsolve->bufvals) free(hsolve->bufvals);
			hsolve->bufvals=NULL;
		    if (hsolve->fbufval) free(hsolve->fbufval);
			hsolve->fbufval=NULL;
		    hsolve->nconcs=0;
		}
		if (hsolve->ngivals) {
		    if (hsolve->givals) free(hsolve->givals);
			 hsolve->givals=NULL;
			 hsolve->ovals=NULL;
		    if (hsolve->compgiv) free(hsolve->compgiv);
			 hsolve->compgiv=NULL;
		    if (hsolve->childgiv) free(hsolve->childgiv);
			 hsolve->childgiv=NULL;
		    hsolve->ngivals=0;
		}
		if (hsolve->ntab>0 && hsolve->tablist) {
		    free(hsolve->tablist);
		    hsolve->tablist=NULL;
		    if (hsolve->tabinteg) {
			free(hsolve->tabinteg);
			hsolve->tabinteg=NULL;
		    }
		    hsolve->ntab=0;
		}
		if (hsolve->sntab>0 && hsolve->stablist) {
		    free(hsolve->stablist);
		    hsolve->stablist=NULL;
		    if (hsolve->tables) free(hsolve->tables);
			hsolve->tables=NULL;
		    if (hsolve->tabcols) free(hsolve->tabcols);
			hsolve->tabcols=NULL;
		    hsolve->ncols=0;
		    if (hsolve->xvals) free(hsolve->xvals);
			hsolve->xvals=NULL;
		    hsolve->xdivs=0;
		    if (hsolve->yvals) free(hsolve->yvals);
			hsolve->yvals=NULL;
		    hsolve->ydivs=0;
		    hsolve->sntab=0;
		}
		if (hsolve->nops>0 && hsolve->ops) {
		    free(hsolve->ops);
		    hsolve->nops=0;
		    hsolve->ops=NULL;
		}
		if (hsolve->nchips>0 && hsolve->chip) {
			free(hsolve->chip);
			hsolve->nchips=0;
			hsolve->chip=NULL;
		}
		if (hsolve->nchildren) {
		    hsolve->nchildren=0;
		    if (hsolve->childtypes) free(hsolve->childtypes);
			hsolve->childtypes=NULL;
		    if (hsolve->childdata) free(hsolve->childdata);
			hsolve->childdata=NULL;
		    if (hsolve->childmsg) free(hsolve->childmsg);
			hsolve->childmsg=NULL;
		    if (hsolve->childlink) free(hsolve->childlink);
			hsolve->childlink=NULL;
		    if (hsolve->childcode) free(hsolve->childcode);
			hsolve->childcode=NULL;
		    if (hsolve->childpos) free(hsolve->childpos);
			hsolve->childpos=NULL;
		    if (hsolve->childstart) free(hsolve->childstart);
			hsolve->childstart=NULL;
		    if (hsolve->childnames) free(hsolve->childstart);
			hsolve->childnames=NULL;
		}
		if (hsolve->compchips) free(hsolve->compchips);
		    hsolve->compchips=NULL;
		if (hsolve->compops) free(hsolve->compops);
		hsolve->compops=NULL;
		if (hsolve->childops) free(hsolve->childops);
		    hsolve->childops=NULL;
		if (hsolve->childchips) free(hsolve->childchips);
		    hsolve->childchips=NULL;
		for (i=0; i<hsolve->nelm_names; i++){
		    if (hsolve->elm_names[i]) free(hsolve->elm_names[i]);
		    hsolve->elm_names[i]=NULL;
		}
		if (hsolve->elm_names) free(hsolve->elm_names);
		    hsolve->elm_names = NULL;
		hsolve->nelm_names=0;
		if (hsolve->itotal) free(hsolve->itotal);
		    hsolve->itotal=NULL;
		if (hsolve->ininfo) free(hsolve->ininfo);
		hsolve->ininfo = NULL;
		if (hsolve->outinfo) free(hsolve->outinfo);
		hsolve->outinfo = NULL;
		if (hsolve->readflag==HREAD_T) {
		    comp=hsolve->comps;
		    while (comp) {
			lcomp=comp;
			comp=comp->next;
			if (lcomp) free(lcomp);
		    }
		    hsolve->comps=NULL;
		    if (hsolve->elmcomps) free(hsolve->elmcomps);
			hsolve->elmcomps=NULL;
		}
		if (hsolve->numnodes>0) {
		    if (hsolve->compnodes) free(hsolve->compnodes);
			hsolve->compnodes=NULL;
		    if (hsolve->startnodes) free(hsolve->startnodes);
			hsolve->startnodes=NULL;
		    if (hsolve->ncnode) free(hsolve->ncnode);
			hsolve->ncnode=NULL;
		    if (hsolve->numsbytes) free(hsolve->numsbytes);
			hsolve->numsbytes=NULL;
		    if (hsolve->numrbytes) free(hsolve->numrbytes);
			hsolve->numrbytes=NULL;
		    if (hsolve->sendid) free(hsolve->sendid);
			hsolve->sendid=NULL;
		    if (hsolve->netid) free(hsolve->netid);
			hsolve->netid=NULL;
			hsolve->numnodes=0;
		}
	    } else {
		if (hsolve->hh) free(hsolve->hh);
		if (hsolve->msgcompts) free(hsolve->msgcompts);
		hsolve->msgcompts=NULL;
	    }
	    if (hsolve->chan) free(hsolve->chan);
		hsolve->chan=NULL;
	    if (hsolve->hnum) free(hsolve->hnum);
		hsolve->hnum=NULL;
	    if (hsolve->elmnum) free(hsolve->elmnum);
		hsolve->elmnum=NULL;
	    if (hsolve->results) free(hsolve->results);
	    if (hsolve->diagonals) free(hsolve->diagonals);
	    if (hsolve->nravals>0 && hsolve->ravals) {
		    free(hsolve->ravals);
		    hsolve->nravals=0;
		    hsolve->ravals=NULL;
	    }
	    if (hsolve->nfuncs > 0 && hsolve->funcs) {
		free(hsolve->funcs);
		hsolve->nfuncs=0;
	    }
            if (hsolve->ravptr) {
                for (i=0;i<hsolve->ncompts;i++) {
                    free(hsolve->ravptr[i]);
                }
                free(hsolve->ravptr);
            }
            hsolve->ravptr=NULL;
	}
}
