static char rcsid[] = "$Id: hines_data.c,v 1.3 2005/07/20 20:02:00 svitak Exp $";

/* Version EDS22i, 99/11/30 Erik De Schutter, Caltech & BBF-UIA 11/92-11/99 */

/*
** $Log: hines_data.c,v $
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
** Revision 1.8  2000/09/11 15:50:30  mhucka
** Not sure what happened, but some of the calls to chip_ functions had the
** wrong number of arguments again.
**
** Revision 1.6  2000/06/12 04:43:15  mhucka
** Some calls to chip_put_tabchannel() had the wrong number of arguments.
**
** Revision 1.5  1999/12/29 10:26:14  mhucka
** New updates from Erik De Schutter
**
 * EDS22i revison: HC BBF-UIA 99/11/30-99/11/30
 * Corrected find_solve_field
 *
 * EDS22h revison: EDS BBF-UIA 99/03/10-99/03/23
 * Cleaned up time step code, updated find_solve_field, improved hillpump
 *
 * EDS22f revison: EDS BBF-UIA 98/10/30-98/11/19
 * Improved ctaupump code
 *
 * EDS22d revison: EDS BBF-UIA 98/05/05-98/06/18
 * Added support for single shells
 * Added do_solvechildname
 *
 * EDS22c revison: EDS BBF-UIA 97/12/01-97/12/01
 * Added error detection code to findsolvefield
 *
 * EDS22b revison: EDS BBF-UIA 97/11/14-97/11/20
 * Corrected chanmode 3 output features
 * Added poolbuffer and hillpump code
 *
** Revision 1.2  1997/08/01 00:03:30  dhb
** Now use G_getopt() in do_findsolvefield() and do_solvecompname().
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/*
 * 95/07/24 eds
 * updated findsolvefield for new interpolation ops
 *
 * 95/07/19 eds
 * corrected bug in findsolvefield: behaves graciously if hsolve==NULL
 *
 * 95/06/02 eds
 * removed Vm_flag
*/


#include "hines_ext.h"

#include <math.h>
#include <stdio.h>
#include <string.h>


float calc_shell_vol();

/* Performs the HGET and HPUT functions for the different object types:
** a HPUT: element fields -> chip_array
** a HGET: chip_array -> element fields */

int chip_put_compt(hsolve,comptno)
	Hsolve	*hsolve;
	int	comptno;
{
	Ncomp   *compt;
	Compinfo *comp;
	int	nchip,nchip2,ngiv = 0;
	double	*chip=hsolve->chip;
	int	chanmode=hsolve->chanmode;
	double	dt;

	if (BaseObject(hsolve)->method == CRANK_INT)
	    dt = hsolve->dt/2.0;
	else /* BEULER by default */
	    dt = hsolve->dt;

	nchip=hsolve->compchips[comptno];
	if (chanmode>=4) {
	    /* compute place of Rm in givals */
	    if (comptno) {
		ngiv=hsolve->compgiv[comptno-1]+1;
	    } else {	/* comptno==0 */
		ngiv=1;
	    }
	}
	if (comptno==hsolve->ncompts-1) {
		nchip2=hsolve->nchips-2;
	} else {
		nchip2=hsolve->compchips[comptno+1]-2;
	}
	if (hsolve->readflag==HREAD_T) {	/* no compartment elements */
	    comp=hsolve->elmcomps[comptno];
	    chip[nchip]=comp->Em/comp->Rm;
	    chip[nchip+1]=comp->inject;
	    chip[nchip2]=dt/comp->Cm;
	    if (chanmode>=4) hsolve->givals[ngiv]=comp->Rm;	/* to compute leak current */
	} else {
	    compt=(Ncomp *)hsolve->compts[hsolve->elmnum[comptno]];
	    chip[nchip]=compt->Em/compt->Rm;
	    chip[nchip+1]=compt->inject;
	    hsolve->vm[comptno]=compt->Vm;
	    chip[nchip2]=dt/compt->Cm;
	    if (chanmode>=4) hsolve->givals[ngiv]=compt->Rm;	/* to compute leak current */
	}
	return(0);
}

int chip_put_spike(hsolve,cindex)
	Hsolve  *hsolve;
	int     cindex;
{
	Spike   *spike;
	int	nop,nchip;
	int     *ops;
	double  *chip;

	chip=hsolve->concchip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	ops=hsolve->ops;
	chip=hsolve->chip;
	spike=(Spike *)hsolve->children[cindex];
	ops[nop+1]=0;   /* clear refractory period */
	/* always one subtracted from refractory period */
	ops[nop+2]=(int)((spike->abs_refract/hsolve->dt)+1.9);
	chip[nchip]=spike->thresh;
	return(0);
}

int chip_put_concen(hsolve,cindex,concind)
	Hsolve	*hsolve;
	int	cindex;
	int	concind;
{
	Concen  *concen;

	concen=(Concen *)hsolve->children[cindex];
	hsolve->conc[concind]=concen->Ca_base+concen->C;
	hsolve->flux[concind]=0.0;
	return(0);
}

int chip_put_Ca_concen(hsolve,comptno,cindex)
	Hsolve	*hsolve;
	int	comptno,cindex;
{
	Concen  *concen;
	double	*chip;
	int	nop,nchip;
	Ncomp	*compt;
	double	dt;

	dt=hsolve->dt;
	chip=hsolve->concchip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	concen=(Concen *)hsolve->children[cindex];
	chip[nchip]=concen->Ca_base;
	chip[nchip+1]=1.0 + dt/(2*concen->tau);
	if (hsolve->readflag==HPROTO_T) {	/* hines_parser prototype hsolve init */
	    /* unscale B */
	    compt=(Ncomp *)hsolve->compts[hsolve->elmnum[comptno]];
	    chip[nchip+2]=dt*concen->B * calc_shell_vol(compt->len,compt->dia,concen->thick);
	} else {				/* standard initialization */
	    chip[nchip+2]=dt*concen->B;
	}
	return(0);
}

int chip_put_pool(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex,concindex;
{
	Cpool   *pool;

	pool=(Cpool *)hsolve->children[cindex];
	hsolve->conc[concindex]=pool->C;
	return(0);
}

int chip_put_shell(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex;
	int	concindex;
{
	Dshell  *shell;

	shell=(Dshell *)hsolve->children[cindex];
	hsolve->conc[concindex]=shell->C;
	hsolve->flux[concindex]=0.0;
	return(0);
}

int chip_put_flux(hsolve,comptno,cindex)
/* works for both concpools and difshells */
	Hsolve	*hsolve;
	int	comptno,cindex;
{
	Dshell  *shell;
	double	*chip;
	int	nop,nchip;
	Ncomp	*compt;
	double	dt,dt2,vol;

	dt = dt2 = hsolve->dt;
        if ((hsolve->nshrows)&&(hsolve->nshrows[hsolve->childcode[cindex]]==1)&&
				    (BaseObject(hsolve)->method==CRANK_INT)) 
	    /* diffusion but no buffers and Crank-Nicholson */
        	dt2 = dt/2.0;
	chip=hsolve->concchip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	shell=(Dshell *)hsolve->children[cindex];
	/* check if any concops allocated */
	if ((cindex==hsolve->nchildren)||(nop!=hsolve->childops[cindex+1])) {
	    if (hsolve->concops[nop]==FLUXCONC_OP) {
		chip[nchip]=dt2*shell->leak;
		if (hsolve->readflag==HPROTO_T) {	/* hines_parser prototype hsolve init */
		    /* unscale B */
		    compt=(Ncomp *)hsolve->compts[hsolve->elmnum[comptno]];
		    /* NEED TO ADD FUNCTION FOR DEEPER SHELL */
		    vol=calc_shell_vol(compt->len,compt->dia,shell->thick);
		} else {
		    vol=shell->vol;
		}
		if ((fabs(vol)<UTINY)||(fabs(shell->val)<UTINY)) {
		    Error();
		    printf(" zero vol or val field in %s\n",Pathname(shell));
		} else {
		    chip[nchip+1]=dt2/(shell->val*FARADAY*vol);
		}
	    }
	}
	return(0);
}

int chip_put_poolbuffer(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex;
	int	concindex;
{
	Fbuffer *buffer;
	double	*chip=hsolve->concchip;
	double  dt=hsolve->dt;
	int	nchip;

	buffer=(Fbuffer *)hsolve->children[cindex];
	hsolve->conc[concindex]=buffer->Bfree;
	nchip=hsolve->childchips[cindex];
	chip[nchip]=dt*buffer->kBb/2.0;
	chip[nchip+1]=dt*buffer->kBb*buffer->Btot;
	chip[nchip+2]=dt*buffer->kBf;
	return(0);
}

int chip_put_buffer(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex;
	int	concindex;
{
	Fbuffer *buffer;
	double	*chip;

	buffer=(Fbuffer *)hsolve->children[cindex];
	hsolve->conc[concindex]=buffer->Bfree;
	return(0);
}

int chip_put_d2buffer(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex;
	int	concindex;
{
	D2buffer *buffer;
	double	*chip,*conc;

	buffer=(D2buffer *)hsolve->children[cindex];
	conc=hsolve->conc;
	conc[concindex]=buffer->Bfree;
	conc[concindex+1]=buffer->Bbound;
	return(0);
}

int chip_put_taupump(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Tpump   *pump;
	double	*chip;
	int	nop,nchip;
	double	dt;

        if ((hsolve->nshrows)&&(hsolve->nshrows[hsolve->childcode[cindex]]==1)&&
				    (BaseObject(hsolve)->method==CRANK_INT)) {
	    /* diffusion but no buffers and Crank-Nicholson */
	    dt = hsolve->dt/2.0;
	} else {	/* BEULER by default */
	    dt = hsolve->dt;
	}
	chip=hsolve->concchip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	pump=(Tpump *)hsolve->children[cindex];
	if (hsolve->concops[nop]==TAUPUMP_OP) {
	    chip[nchip]=pump->T_C;
	    chip[nchip+1]=pump->T_A;
	    chip[nchip+2]=pump->T_V;
	    chip[nchip+3]=1.0/pump->T_B;
	    chip[nchip+4]=pump->Ceq;
	} else {
	    chip[nchip]=pump->Ceq*dt/pump->T_C;
	    chip[nchip+1]=dt/pump->T_C;
	}
	return(0);
}

int chip_put_mmpump(hsolve,cindex,concchild)
	Hsolve	*hsolve;
	int	cindex,concchild;
{
	Mpump   *pump;
	Dshell  *shell;
	double	*chip;
	int	nchip;
	double	dt;
	short	electro;

        electro=hsolve->childtypes[cindex]==ELMMPUMP_T;
	if ((BaseObject(hsolve)->method == CRANK_INT) &&
		(hsolve->nshrows[hsolve->childcode[cindex]]==1))
	    dt = hsolve->dt/2.0;
	else /* BEULER by default */
	    dt = hsolve->dt;
	nchip=hsolve->childchips[cindex];
	pump=(Mpump *)hsolve->children[cindex];
	shell=(Dshell *)hsolve->children[concchild];
	if (electro) {	/* ELMMPUMP_OP: electrogenic pump */
	    chip=hsolve->chip;
	    chip[nchip]=pump->Kd;
	    chip[nchip+1]=pump->vmax*pump->val*FARADAY;
	    chip[nchip+2]=dt*pump->vmax/shell->vol;
	} else {	/* MMPUMP_OP: standard pump */
	    chip=hsolve->concchip;
	    chip[nchip]=dt*pump->vmax/shell->vol;
	    chip[nchip+1]=pump->Kd;
	}
	return(0);
}

int chip_put_hillpump(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Element **children=hsolve->children;
	Hpump   *pump;
	Dshell  *shell;
	Cpool   *pool;
	double	*chip=hsolve->concchip;
	int	cc,nop,nchip;
	double	dt,dt2;

	dt=dt2=hsolve->dt;
	cc=hsolve->childcode[cindex]; /* hillpump childcode overwritten -> use difshell childcode */
        if ((hsolve->nshrows) && (hsolve->nshrows[hsolve->childcode[cc]]==1) &&
			(BaseObject(hsolve)->method==CRANK_INT)) {
	    /* diffusion but no buffers and Crank-Nicholson */
        	dt2 = dt/2.0;
    	} 
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	pump=(Hpump *)children[cindex];
	hsolve->concops[nop+1]=pump->Hill;
	chip[nchip]=pump->Kd;
	shell=(Dshell *)children[cc];
	chip[nchip+1]=dt2*pump->vmax/shell->vol;
	if (hsolve->childlink[cindex]) {	/* msg to concpool present */
	    pool=(Cpool *)children[hsolve->childpos[cindex]];
	    chip[nchip+2]=dt*pump->vmax/pool->vol;
	}
	return(0);
}

int chip_put_nernst(hsolve,cindex,concindex)
/* WARNING: this also changes conc array, may overwrite computed values */
/* We also assume constant is correct value */
	Hsolve	*hsolve;
	int	cindex;
{
	Nernst  *nernst;
	double	*conc;
	int	nchip;

	nchip=hsolve->childchips[cindex];
	nernst=(Nernst *)hsolve->children[cindex];
	hsolve->chip[nchip]=nernst->constant;
	conc=hsolve->conc;
	conc[concindex]=nernst->Cin;
	conc[concindex+1]=nernst->Cout;
	return(0);
}

int chip_put_ghk(hsolve,cindex,concindex)
/* WARNING: this also changes conc array, may overwrite computed values */
	Hsolve	*hsolve;
	int	cindex;
{
	Ghk	*ghk;
	double	*chip,*conc;
	int	nchip;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	ghk=(Ghk *)hsolve->children[cindex];
	chip[nchip]=ghk->valency*FARADAY/GAS_CONSTANT/(ghk->T+ZERO_CELSIUS);	/* zF/RT */
	chip[nchip+1]=ghk->valency*FARADAY;	/* zF */
	conc=hsolve->conc;
	conc[concindex]=ghk->Cin;
	conc[concindex+1]=ghk->Cout;
	return(0);
}

int chip_put_tabchannel(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Tchan   *chan;
	double	*chip;
	int	nop,nchip;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	chan=(Tchan *)hsolve->children[cindex];
	nop=hsolve->childops[cindex];
	if (hsolve->ops[nop-1]==CHAN_EK_OP) chip[nchip-2]=chan->Ek;
	chip[nchip-1]=chan->Gbar;
	if (chan->X_alloced) {
	    chip[nchip]=chan->X;
	    nchip++;
	}
	if (chan->Y_alloced) {
	    chip[nchip]=chan->Y;
	    nchip++;
	}
	if (chan->Z_alloced) {
	    chip[nchip]=chan->Z;
	    nchip++;
	}
	return(0);
}

int chip_put_tabcurrent(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Tcurr   *curr;
	double	*chip;
	int	nchip,nop;

	nop=hsolve->childops[cindex];
	if (hsolve->ops[nop-1]==CHAN_OP) {
	    chip=hsolve->chip;
	    nchip=hsolve->childchips[cindex];
	    curr=(Tcurr *)hsolve->children[cindex];
	    chip[nchip-1]=curr->Gbar;
	}
	return(0);
}

int chip_put_channelc3(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Schan   *chan;
	double	*chip;
	int	nop,nchip;
	double  *tablist;
	S2chan  *sychan;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	tablist=hsolve->stablist;
	chan=(Schan *)hsolve->children[cindex];
	if (hsolve->ops[nop-1]==CHAN_EK_OP) chip[nchip-2]=chan->Ek;
	chip[nchip-1]=chan->norm;
	if (hsolve->ops[nop]==SYN3_OP) {
	    chip[nchip]=chan->frequency * hsolve->dt;
	    nchip++;
	}
	hsolve->ops[nop+2]=-1;	/* initialize as no-event */
	chip[nchip]=chan->X;
	chip[nchip+1]=chan->Y;
	if (hsolve->childtypes[cindex]==SYNCHANC_T) {
	    sychan=(S2chan *)chan;
	    sychan->hsolve=(Element *)hsolve;
	    sychan->solve_index=cindex;
	}
	return(0);
}

int chip_put_mgblock(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Mblock   *block;
	double	*chip;
	int	nchip;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	block=(Mblock *)hsolve->children[cindex];
	chip[nchip]=block->KMg_A;
	chip[nchip+1]=1.0/block->KMg_B;
	chip[nchip+2]=block->CMg;
	return(0);
}

int chip_get_compt(hsolve,comptno)
	Hsolve	*hsolve;
	int	comptno;
{
	Ncomp 	*compt;

	compt=(Ncomp *)hsolve->compts[hsolve->elmnum[comptno]];
	compt->Vm=hsolve->vm[comptno];
	compt->inject=hsolve->chip[hsolve->compchips[comptno]+1];
	if (hsolve->chanmode<4) {
	    compt->Im=0.0;	/* not available */
	} else {
	    compt->Im=hsolve->givals[hsolve->compgiv[comptno]];
	}
	return(0);
}

int chip_get_concen(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex,concindex;
{
	Concen  *concen;

	concen=(Concen *)hsolve->children[cindex];
	concen->Ca=hsolve->conc[concindex];
	concen->C=concen->Ca-concen->Ca_base;
	return(0);
}

int chip_get_pool(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex,concindex;
{
	Cpool   *pool;

	pool=(Cpool *)hsolve->children[cindex];
	pool->C=hsolve->conc[concindex];
	return(0);
}

int chip_get_shell(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex,concindex;
{
	Dshell  *shell;

	shell=(Dshell *)hsolve->children[cindex];
	shell->C=hsolve->conc[concindex];
	return(0);
}

int chip_get_buffer(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex,concindex;
{
	Fbuffer *buffer;

	buffer=(Fbuffer *)hsolve->children[cindex];
	buffer->Bfree=hsolve->conc[concindex];
	buffer->Bbound=buffer->Btot-buffer->Bfree;
	return(0);
}

int chip_get_d2buffer(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex,concindex;
{
	D2buffer *buffer;

	buffer=(D2buffer *)hsolve->children[cindex];
	buffer->Bfree=hsolve->conc[concindex];
	buffer->Bbound=hsolve->conc[concindex+1];
	return(0);
}

int chip_get_taupump(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Tpump   *pump;

	pump=(Tpump *)hsolve->children[cindex];
	if (hsolve->chanmode<4) {
	    pump->kP=0.0;	/* not available */
	} else {
	    pump->kP=hsolve->givals[hsolve->childgiv[cindex]]/hsolve->dt;
	}
	return(0);
}

int chip_get_nernst(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex;
	int	concindex;
{
	Nernst  *nernst;
	int	nchip;
	double	*conc,*chip;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	conc=hsolve->conc;
	nernst=(Nernst *)hsolve->children[cindex];
	nernst->Cin=conc[concindex];
	nernst->Cout=conc[concindex+1];
	nernst->E=hsolve->chip[nchip]*log(conc[concindex+1]/conc[concindex]);
	return(0);
}

int chip_get_ghk(hsolve,cindex,concindex)
	Hsolve	*hsolve;
	int	cindex;
	int	concindex;
{
	Ghk	*ghk;
	int	nchip;
	double	*conc,*chip;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	conc=hsolve->conc;
	ghk=(Ghk *)hsolve->children[cindex];
	ghk->Cin=conc[concindex];
	ghk->Cout=conc[concindex+1];
	ghk->Gk=0.0;
	ghk->Ik=0.0;
	ghk->Ek=0.0;
	return(0);
}

int chip_get_tabchannel(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Tchan   *chan;
	double	*chip,*givals;
	int	nchip,ng;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	chan=(Tchan *)hsolve->children[cindex];
	if (chan->X_alloced) {
		chan->X=chip[nchip];
		nchip++;
	}
	if (chan->Y_alloced) {
		chan->Y=chip[nchip];
		nchip++;
                    }
	if (chan->Z_alloced) {
		chan->Z=chip[nchip];
		nchip++;
	}
	if (hsolve->chanmode<4) {
		chan->Gk=0.0;    /* no data were stored */
		chan->Ik=0.0;	/* not available */
	} else {
		givals=hsolve->givals;
		ng=hsolve->childgiv[cindex];
		chan->Gk=givals[ng];
		chan->Ik=givals[ng+1];
		chan->Ek=givals[ng+2];
	}
	return(0);
}

int chip_get_mmpump(hsolve,cindex)	/* assumes ELMMPUMP_T */
	Hsolve	*hsolve;
	int	cindex;
{
	Mpump   *pump;

	pump=(Mpump *)hsolve->children[cindex];
	if (hsolve->chanmode<4) {
	    pump->Ik=0.0;	/* not available */
	} else {
	    pump->Ik=hsolve->givals[hsolve->childgiv[cindex]];
	}
	return(0);
}

int chip_get_tabcurrent(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Tcurr   *curr;
	double	*givals;
	int	ng;

	curr=(Tcurr *)hsolve->children[cindex];
	if ((hsolve->childtypes[cindex]!=TABCURR_T)||(hsolve->chanmode<4)) {
	    curr->Gk=0.0;    /* no data were stored */
	    curr->Ik=0.0;	/* not available */
	    curr->Ek=0.0;	/* not available */
	} else {
	    givals=hsolve->givals;
	    ng=hsolve->childgiv[cindex];
	    curr->Gk=givals[ng];
	    curr->Ik=givals[ng+1];
	    curr->Ek=givals[ng+2];
	}
	return(0);
}

int chip_get_channelc3(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Schan   *chan;
	double	*chip,*givals;
	int	nop,nchip,ng;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	chan=(Schan *)hsolve->children[cindex];
	if (hsolve->ops[nop]==SYN3_OP)  nchip++;
	chan->X=chip[nchip];
	chan->Y=chip[nchip+1];
	if (hsolve->chanmode<4) {
	    chan->Gk=0.0;    /* no data were stored */
	    chan->Ik=0.0;	/* not available */
	    chan->Ek=0.0;	/* not available */
	} else {
	    givals=hsolve->givals;
	    ng=hsolve->childgiv[cindex];
	    chan->Gk=givals[ng];
	    chan->Ik=givals[ng+1];
	    chan->Ek=givals[ng+2];
	}
	return(0);
}

int chip_get_mgblock(hsolve,cindex)
	Hsolve	*hsolve;
	int	cindex;
{
	Mblock   *block;
	double	*chip,*givals;
	int	nop,nchip,ng;

	chip=hsolve->chip;
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	block=(Mblock *)hsolve->children[cindex];
	if (hsolve->chanmode<4) {
		block->Gk=0.0;    /* no data were stored */
		block->Ik=0.0;	/* not available */
	} else {
		givals=hsolve->givals;
		ng=hsolve->childgiv[cindex];
		block->Gk=givals[ng];
		block->Ik=givals[ng+1];
		block->Ek=givals[ng+2];
	}
	return(0);
}

int hfind_elm(hsolve,elm,comptno,cindex)
	Hsolve  *hsolve;
	Element *elm;
	int	*comptno,*cindex;
{
	Element **compts,*compt,**children;
	int     *hnum,*childstart;
	int		ncompts;
	int     i;
	int		Strindex();

	if (!hsolve->compts || !hsolve->children) return(ERR);
	ncompts = hsolve->ncompts;
	compts = hsolve->compts;
	children = hsolve->children;
	hnum = hsolve->hnum;
	childstart = hsolve->childstart;

	if (elm) {
	    for (compt=elm; Strindex(BaseObject(compt)->name,"compartment")<0 ; compt=compt->parent);
	    for (i=0; i<ncompts; i++) {
		if (compts[i]==compt) {
		    break;
		}
	    }
	    if (i<ncompts) {
		*comptno=hnum[i];
		if (Strindex(BaseObject(elm)->name,"compartment") >= 0) {
		/* is a compartment */
		    *cindex=0;
		    return(0);
		} else {
		/* is a child */
		    for (i=childstart[*comptno]; i<childstart[*comptno+1]; i++) {
			if (children[i]==elm) {
			    *cindex=i;
			    return(0);
			}
		    }
		    }
	    } 
	}
	return(ERR);
}

int hput_elm(hsolve,comptno,cindex)
	Hsolve  *hsolve;
	int	comptno,cindex;
{
	short   *childtypes;
	int     *childlink,*childmsg;
	int	ct,cm,cl;
	int Strindex();

	childtypes=hsolve->childtypes;
	childlink=hsolve->childlink;
	childmsg=hsolve->childmsg;

	if (cindex== 0) {
	/* is a compartment */
	    chip_put_compt(hsolve,comptno);
	} else {
	/* is a child */
	    ct=childtypes[cindex];
	    cl=childlink[cindex];
	    cm=childmsg[cindex];
	    switch (ct) { 
		case TABCHAN_T:
		case TAB2CHAN_T:
			chip_put_tabchannel(hsolve,cindex);
			break;
		case TABCURR_T:
		case TABGHK_T:
			chip_put_tabcurrent(hsolve,cindex);
			break;
		case CHANC2_T:
		case CHANC3_T:
		case SYNCHANC_T:
			chip_put_channelc3(hsolve,cindex);
			break;
		case NERNST_T:
			chip_put_nernst(hsolve,cindex,cm);
			break;
		case GHK_T:
			chip_put_ghk(hsolve,cindex,cm);
			break;
		case CACONCEN_T:
			chip_put_concen(hsolve,cindex,cm);
			chip_put_Ca_concen(hsolve,comptno,cindex);
			break;
		case POOL_T:
			chip_put_pool(hsolve,cindex,cm);
			chip_put_flux(hsolve,comptno,cindex);
		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
			chip_put_shell(hsolve,cindex,cm);
			chip_put_flux(hsolve,comptno,cindex);
			break;
		case FIXBUFF_T:
		case BUFSHELL_T:
		case BUFSLAB_T:
		case BUFUSER_T:
			chip_put_buffer(hsolve,cindex,cm);
			break;
		case BUF2SHELL_T:
		case BUF2SLAB_T:
		case BUF2USER_T:
			chip_put_d2buffer(hsolve,cindex,cm);
			break;
		case TAUPUMP_T:
			chip_put_taupump(hsolve,cindex);
			break;
		case MMPUMP_T:
		case ELMMPUMP_T:
			chip_put_mmpump(hsolve,cindex,cl);
			break;
		case HILLPUMP_T:
			chip_put_hillpump(hsolve,cindex);
			break;
		case POOLBUFF_T:
			chip_put_poolbuffer(hsolve,cindex,cm);
			break;
		default:
			break;
	    }
	}
	return(0);
}

int hget_elm(hsolve,comptno,cindex)
	Hsolve  *hsolve;
	int	comptno,cindex;
{
	short   *childtypes;
	int     *childmsg;
	int	ct,cm;
	int 	Strindex();

	childtypes=hsolve->childtypes;
	childmsg=hsolve->childmsg;

	if (cindex== 0) {
	/* is a compartment */
	    chip_get_compt(hsolve,comptno);
	} else {
	/* is a child */
	    ct=childtypes[cindex];
	    cm=childmsg[cindex];
	    switch (ct) { 
		case TABCHAN_T:
		case TAB2CHAN_T:
			chip_get_tabchannel(hsolve,cindex);
			break;
		case TABCURR_T:
		case TABGHK_T:
			chip_get_tabcurrent(hsolve,cindex);
			break;
		case CHANC2_T:
		case CHANC3_T:
		case SYNCHANC_T:
			chip_get_channelc3(hsolve,cindex);
			break;
		case NERNST_T:
			chip_get_nernst(hsolve,cindex,cm);
			break;
		case GHK_T:
			chip_get_ghk(hsolve,cindex,cm);
			break;
		case CACONCEN_T:
			chip_get_concen(hsolve,cindex,cm);
			break;
		case POOL_T:
			chip_get_pool(hsolve,cindex,cm);
			break;
		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
			chip_get_shell(hsolve,cindex,cm);
			break;
		case POOLBUFF_T:
		case FIXBUFF_T:
		case BUFSHELL_T:
		case BUFSLAB_T:
		case BUFUSER_T:
			chip_get_buffer(hsolve,cindex,cm);
			break;
		case BUF2SHELL_T:
		case BUF2SLAB_T:
		case BUF2USER_T:
			chip_get_d2buffer(hsolve,cindex,cm);
			break;
		case ELMMPUMP_T:
			chip_get_mmpump(hsolve,cindex);
			break;
		case TAUPUMP_T:
			chip_get_taupump(hsolve,cindex);
			break;
		default:
			break;
	    }
	}
	return(0);
}

/* straight out of Kernighan and Ritchie, p. 67 */
int Strindex(s,t)
/* finds first occurrence of t in s */
	char	s[],t[];
{
	int 	i,j,k;

	for (i=0; s[i]!='\0'; i++) {
		for (j=i,k=0; t[k]!='\0' && s[j]==t[k]; j++,k++);
		if (t[k]=='\0') return(i);
	}	
	return(-1);
}

int Lastindex(s,t)
/* finds last occurrence of t in s */
	char	s[],t[];
{
	int 	i,j,k;

	for (i=strlen(s)-(strlen(t)-1); i>0; i--) {
		for (j=i,k=0; t[k]!='\0' && s[j]==t[k]; j++,k++);
		if (t[k]=='\0') return(i);
	}	
	return(-1);
}

/* Function which returns the array location of a particular variable in
** the hines solver.  Used for input/output of values 
*/
char *do_findsolvefield(argc,argv)
	int argc;
	char **argv;
{
	int 	i,k,n,cindex;
	int	ncompts,nchip,nop,comptno;
	short   *childtypes;
	int 	*ops,*hnum;
	int	*compchips,*childstart,*childnames,*childmsg,*childgiv;
	Hsolve 	*hsolve;
	Compinfo **ecomps;
	Element **compts;
	int	thisnode;
	char	elmname[NAMELEN],solvename[NAMELEN];
	char	*compname,*childname = NULL,name[20],*cname;
	int	notfound=1;
	int	Strindex();

	strcpy(name,"");
	initopt(argc, argv, "hsolve element field");
	if (G_getopt(argc, argv) != 0) {
		printoptusage(argc, argv);
		return(CopyString(name));
	}

	/* syntax of element is either:
	**   pathname/compname
	**   pathname/compname/childname
	**   pathname/compname/compname/childname
	**   compname
	**   compname/childname
	**   compname/compname/childname
	** where pathname==hsolve->name
	*/
	hsolve = (Hsolve *)GetElement(optargv[1]);
	if (!hsolve || (strcmp(BaseObject(hsolve)->name,"hsolve") != 0)) {
	    Error();
	    printf(" %s is not an hsolve\n",optargv[1]);
	    return(CopyString(name));
	}
	ncompts=hsolve->ncompts;
	if (!ncompts) {
	    Error();
	    printf(" %s has not been SETUP\n",optargv[1]);
	    return(CopyString(name));
	}
	if (hsolve->chanmode<2) {
	    Error();
	    printf(" %s chanmode should be 2 or larger to use the findsolvefield command\n",optargv[1]);
	    return(CopyString(name));
	}
	compts = hsolve->compts;
	hnum = hsolve->hnum;
	compchips=hsolve->compchips;
	ecomps=hsolve->elmcomps;
	ops=hsolve->ops;
	childnames=hsolve->childnames;
	childstart=hsolve->childstart;
	childtypes=hsolve->childtypes;
	childmsg=hsolve->childmsg;
	childgiv=hsolve->childgiv;

	if (strlen(optargv[2])>=NAMELEN) {
	    Error();
	    printf(" element name too long: %s\n",optargv[2]);
	    return(CopyString(name));
	}
	/* get the compname first */
	strcpy(elmname,optargv[2]);
	strcpy(solvename,Pathname(hsolve));
	/* find part that is the pathname */
	compname=elmname;	/* default */
	for (n=0,k=1; k<strlen(solvename); k++) {
	    if (strncmp(elmname,solvename,k)==0) {
		n=k+1;
		if (elmname[n]=='/') compname=elmname+n; /* skip partial pathname */
	    } else {
		break;
	    }
	}
	while (compname[0]=='/') compname=compname+1;
	/* find compartment */
	if (hsolve->readflag != HREAD_T) {	/* created with SETUP call */

	    char *pcEnding = strchr(compname,'/');

	    int iEnding = -1;

	    if (pcEnding)
	    {
		iEnding = pcEnding - compname;
	    }
	    else
	    {
		iEnding = strlen(compname);
	    }
	    n=GetIndexComponent(compname);

	    if ((n > 0) || (Strindex(compname,"[0]") > 0))
	    {
		iEnding -= 2;
		k = 0;
		while (compname[k] != '[')
		{
		    k++;
		}
		while (compname[k] != ']')
		{
		    k++;
		    iEnding--;
		}
	    }

	    for (i=0; i<ncompts; i++) {
		cname=compts[i]->name;
		k=strlen(cname);
		k = k > iEnding ? k : iEnding;
		if ((strncmp(cname,compname,k)==0) && (compts[i]->index==n)){
		    if ((n>0)||(Strindex(compname,"[0]")>0)) { 
			/* has index in name */
			while (compname[k]!=']') k++;
			k++;
		    }
		    break;
		}
	    }
	} else {			/* created by read_hines call */
	    for (i=0; i<ncompts; i++) {
		cname=ecomps[i]->name;
		k=strlen(cname);
		if (strncmp(cname,compname,k)==0)  break;
	    }
	}

	if (i==ncompts) {
	    Error();
	    printf(" compartment %s not found\n",compname);
	    return(CopyString(name));
	}
	comptno=hnum[i];
	if (hsolve->numnodes>0) {
	    /* parallel mode -> check if on this node */
	    thisnode=hsolve->compnodes[comptno]==hsolve->nodenumber;
	} else {
	    thisnode=1;
	}
	n=strlen(compname);
	if (n==k) {
	/* Is a compartment */
	    childname=compname;
	    if (strcmp(optargv[3],"Vm") == 0) {
		if (hsolve->numnodes>0) {
		    /* parallel mode -> output Vm only from master node */
		    if (hsolve->nodenumber!=hsolve->masternode) return(CopyString(name));
		}
		strcpy(name,"vm[");
		nchip=comptno;
		notfound=0;
	    } else if (thisnode) {
		/* other values may not be present if parallel mode */
		if (strcmp(optargv[3],"inject") == 0) {
		    strcpy(name,"chip[");
		    nchip=compchips[comptno]+1;
		    notfound=0;
		} else if (hsolve->chanmode>=4) {
		    if (strcmp(optargv[3],"Im") == 0) { 
			strcpy(name,"givals[");
			nchip=hsolve->compgiv[comptno];
			notfound=0;
		    } else if (strcmp(optargv[3],"leak") == 0) { 
			strcpy(name,"givals[");
			if (comptno) {
			    nchip=hsolve->compgiv[comptno-1]+2;
			} else {	/* comptno==0 */
			    nchip=2;
			}
			notfound=0;
		    }
		}
	    }
	} else if (thisnode) {	/* not a compartment, is on this node */
	/* Is a child */
	    /* find child name */
	    childname=compname+k+1;
	    compname[k]='\0';
	    for (cindex=childstart[comptno];cindex<childstart[comptno+1];cindex++){
		if (strcmp(hsolve->elm_names[childnames[cindex]],childname)==0) {
			break;
		}
	    }
	    if (cindex==childstart[comptno+1]) {
		Error();
		printf(" element %s not found in %s\n",childname,compname);
		return(CopyString(name));
	    }
	    nop=hsolve->childops[cindex];
	    nchip=hsolve->childchips[cindex];
	    /* process Gk, Ik and Ek firest */
	    switch (childtypes[cindex]) {
		case TABCHAN_T:
		case TAB2CHAN_T:
		case TABCURR_T:
		case CHANC2_T:
		case CHANC3_T:
		case SYNCHANC_T:
		case MGBLOCK_T:
		    if (hsolve->chanmode>=4) {
			strcpy(name,"givals[");
			if (strcmp(optargv[3],"Gk") == 0) { 
			    nchip=childgiv[cindex];
			    notfound=0;
			} else if (strcmp(optargv[3],"Ik") == 0) { 
			    nchip=childgiv[cindex]+1;
			    notfound=0;
			} else if (strcmp(optargv[3],"Ek") == 0) { 
			    nchip=childgiv[cindex]+2;
			    notfound=0;
			} 
		    }
		    break;
		default:
		    break;
	    }
	    /* process other fields */
	    if (notfound) switch (childtypes[cindex]) {
		case TABCHAN_T:
		case TAB2CHAN_T:
		    /* figure out how many gates we are using */
		    i=0;
		    for (; ops[nop]!=ADD_CURR_OP; nop++){
			if ((ops[nop]>=IPOL1V_OP)&&(ops[nop]<=IPOL2C_OP)) i++;
		    }	
		    strcpy(name,"chip[");
		    if (strcmp(optargv[3],"X") == 0) { 
			if (i>0) {
			    notfound=0;
			    break;
			}
		    } else if (strcmp(optargv[3],"Y") == 0) { 
			if (i>1) nchip++;	/* assume X gate is used also */
			if (i>0) {
			    notfound=0;
			    break;
			}
		    } else if (strcmp(optargv[3],"Z") == 0) { 
			if (i==2) nchip++;	/* assume X gate is used also */
			if (i==3) nchip+=2;   /* assume X & Y gates used also */
			if (i>0) {
			    notfound=0;
			    break;
			}
		    }
		    break;

		case CHANC2_T:
		case CHANC3_T:
		case SYNCHANC_T:
		    if (hsolve->ops[nop+1]==SYN3_OP) nchip++; /*skip frequency*/
		    strcpy(name,"chip[");
		    if (strcmp(optargv[3],"X") == 0) { 
			nchip++;	/* skip gmax */
			notfound=0;
		    } else if (strcmp(optargv[3],"Y") == 0) { 
			nchip+=2;	/* skip gmax and X */
			notfound=0;
		    }
		    break;

		case CACONCEN_T:
		    nchip=childmsg[cindex];
		    if (strcmp(optargv[3],"Ca") == 0) {
			strcpy(name,"conc[");
			notfound=0;
		    } else if (strcmp(optargv[3],"flux") == 0) {
			strcpy(name,"flux[");
			notfound=0;
		    }
		    break;

		case POOL_T:
		    if (strcmp(optargv[3],"C") == 0) {
			nchip=childmsg[cindex];
			strcpy(name,"conc[");
			notfound=0;
		    } else if (strcmp(optargv[3],"leak") == 0) {
			if (hsolve->concops[nop]==FLUXCONC_OP) {
			    strcpy(name,"concchip[");
			    notfound=0;
			}
		    }
		    break;

		case DIFSHELL_T:
		case DIFSLAB_T:
		case DIFUSER_T:
		    if (strcmp(optargv[3],"C") == 0) {
			nchip=childmsg[cindex];
			strcpy(name,"conc[");
			notfound=0;
		    } else if (strcmp(optargv[3],"flux") == 0) {
			nchip=childmsg[cindex];
			strcpy(name,"flux[");
			notfound=0;
		    } else if (strcmp(optargv[3],"leak") == 0) {
			if (hsolve->concops[nop]==FLUXCONC_OP) {
			    strcpy(name,"concchip[");
			    notfound=0;
			}
		    }
		    break;

		case POOLBUFF_T:
		case FIXBUFF_T:
		    if (strcmp(optargv[3],"Bfree") == 0) {
			strcpy(name,"conc[");
			nchip=childmsg[cindex];
			notfound=0;
		    }
		    break;

		case BUFSHELL_T:
		case BUFSLAB_T:
		case BUFUSER_T:
		    if (strcmp(optargv[3],"Bfree") == 0) {
			strcpy(name,"conc[");
			nchip=childmsg[cindex];
			notfound=0;
		    } else if (strcmp(optargv[3],"Bbound") == 0) {
			strcpy(name,"conc[");
			nchip=childmsg[cindex]+1;
			notfound=0;
		    }
		    break;

		case TAUPUMP_T:
		    if ((strcmp(optargv[3],"kP") == 0) && (hsolve->chanmode>=4)) {
			strcpy(name,"givals[");
			nchip=childgiv[cindex];
			notfound=0;
		    }
		    break;

		case ELMMPUMP_T:
		    if ((strcmp(optargv[3],"Ik") == 0) && (hsolve->chanmode>=4)) {
			strcpy(name,"givals[");
			nchip=childgiv[cindex];
			notfound=0;
		    }
		    break;

		case TABCURR_T:
		case MGBLOCK_T:
		    break;

		default:
		    Error();
		    printf(" no fields available from element %s.\n",childname);
		    return(CopyString(name));
	    }
	}
	if (notfound) {
	    strcpy(name,"");
	    Error();
	    printf(" unknown or unavailable field %s for %s.\n",optargv[3],childname);
	    if (hsolve->chanmode<4)
		printf(" Use chanmode 4 to output Gk, Ik, Ek. Im or leak fields.\n");
	} else {
	    strcat(name,itoa(nchip));
	    strcat(name,"]");
	}
	return(CopyString(name));
}

/* Function which returns the name of the compartment stored at the
** index value inside the hines solver.  Mainly used for debugging.
*/
char *do_solvecompname(argc,argv)
	int argc;
	char **argv;
{
	int 	index;
	Hsolve 	*hsolve;
	Element	*compt;
	char	name[NAMELEN];

	strcpy(name,"");
	initopt(argc, argv, "hsolve index");
	if (G_getopt(argc, argv) != 0) {
	    printoptusage(argc, argv);
	    return(CopyString(name));
	}

	hsolve = (Hsolve *)GetElement(optargv[1]);
	if (!hsolve || (strcmp(BaseObject(hsolve)->name,"hsolve") != 0)) {
	    Error();
	    printf(" %s is not an hsolve\n",optargv[1]);
	    return(CopyString(name));
	}
	index=atoi(optargv[2]);
	if ((index<0)||(index>hsolve->ncompts)) {
	    Error();
	    printf(" %d is not a valid compartment index for %s\n",index,optargv[1]);
	    return(CopyString(name));
	}
	if (hsolve->readflag != HREAD_T) {	/* created with SETUP call */
	    compt=hsolve->compts[hsolve->elmnum[index]];
	    strcpy(name,compt->name);
	    strcat(name,"[");
	    strcat(name,itoa(compt->index));
	    strcat(name,"]");
	} else {			/* created by read_hines call */
	    strcpy(name,hsolve->elmcomps[index]->name);
	}
	return(CopyString(name));
}

/* Function which returns the name of the child stored at the
** index value inside the hines solver.  Mainly used for debugging.
*/
char *do_solvechildname(argc,argv)
	int argc;
	char **argv;
{
	int 	index;
	Hsolve 	*hsolve;
	char	name[2*NAMELEN];

	strcpy(name,"");
	initopt(argc, argv, "hsolve index");
	if (G_getopt(argc, argv) != 0) {
	    printoptusage(argc, argv);
	    return(CopyString(name));
	}

	hsolve = (Hsolve *)GetElement(optargv[1]);
	if (!hsolve || (strcmp(BaseObject(hsolve)->name,"hsolve") != 0)) {
	    Error();
	    printf(" %s is not an hsolve\n",optargv[1]);
	    return(CopyString(name));
	}
	index=atoi(optargv[2]);
	if ((index<0)||(index>hsolve->nchildren)) {
	    Error();
	    printf(" %d is not a valid child index for %s\n",index,optargv[1]);
	    return(CopyString(name));
	}
	if (hsolve->readflag != HREAD_T) {	/* created with SETUP call */
	    strcpy(name,Pathname(hsolve->children[index]));
	} else {			/* created by read_hines call */
	    return(CopyString(name));	/* not implemented */
	}
	return(CopyString(name));
}
