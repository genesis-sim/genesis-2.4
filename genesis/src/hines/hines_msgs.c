static char rcsid[] = "$Id: hines_msgs.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/* Version EDS22i 99/12/24, Erik De Schutter, Caltech & BBF-UIA 4/92-12/99 */

/*
** $Log: hines_msgs.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2000/09/07 02:22:55  mhucka
** Fixes from Hugo Cornelis for various initialization problems.
**
** EDS22k revision HC BBF-UIA 00/07/24
** no normalization for incoming ACTIVATION msg's on hsolve'd synchans
**
** Revision 1.5  1999/12/29 10:26:14  mhucka
** New updates from Erik De Schutter
**
** EDS22i revison: EDS BBF-UIA 99/08/13-99/012/24
** Improved alert messages
**
** EDS22c revison: EDS BBF-UIA 97/11/28-97/12/05
** Made symmetric compartments work
**
** EDS22b revison: EDS BBF-UIA 97/09/05-97/11/27
** Corrected chanmode 4 behavior in h_storeout
** Reformatted and improved error msg
** Added HILLPUMP msg
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* Contains the routines that take care of incoming messages from objects
** not handled by hsolver and outgoing messages not handled by hsolver */

#include "hines_ext.h"

int h_check_msgs(hsolve,elm,op,sChildType,comptno,nop,nchip)
/* checks all incoming msgs and sets up the necessary Iinfo structures */
/* solved==1: message comes from child that is solved
** solved==2: message comes from compartment
** solved==3: message can come from solved or unsolved child
** solved==4: SPIKE message: may come from unsolved child */

    Hsolve  *hsolve;
    Element *elm;
    int     op,comptno,nop,nchip;
    short   sChildType;
{
    MsgIn   *msgin;
    Element *src,**children;
    int	    found,hasflux=0,solved=0,type,j,n0,n1;
    int     Strindex();

    n0=hsolve->childstart[comptno];
    n1=hsolve->childstart[comptno+1];
    children=hsolve->children;
    MSGLOOP(elm, msgin) {
	default:
	    /* check if src is a child */
	    src=msgin->src;
	    found=0;
	    for (j=n0; j<n1; j++) {
		if (src==children[j]) {
		    found=1;
		    break;
		}
	    }
	    /* check msg type */
	    type=msgin->type;
	    switch (op) {
		case COMPT_OP:
		    switch (type) {
			case CHANNEL:
			/* handled by the hsolve code */
			    solved=1;
			    break;
			case RAXIAL:
			case AXIAL:
			case CONNECTCROSS:
			/* all handled by the hsolve code */
			    solved=2;
			    break;
			case INJECT:
			    if (!found || (hsolve->childtypes[j]!=ELMMPUMP_T)) {
				h_add_iinfo(hsolve,msgin,INJECT_OP,sChildType,nop,nchip+1);
			    }
			    solved=3;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case SPIKE_OP:
		    switch (type) {
			case INPUT:
			/* handled by the hsolve code */
			    solved=2;
			    break;
			case THRESH:
			    h_add_iinfo(hsolve,msgin,THRESH_OP,sChildType,nop,nchip);
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case CACONC_OP:
		    switch (type) {
			case I_Ca:
			case fI_Ca:
			/* all handled by the hsolve code */
			    solved=1;
			    break;
			case BASE:
			    h_add_iinfo(hsolve,msgin,BASE_OP,sChildType,nop,nchip);
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case NERNST_OP:
		    switch (type) {
			case CIN:
			case COUT:
			/* all handled by the hsolve code */
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case GHK_OP:
		    switch (type) {
			case GVOLTAGE:
			/* all handled by the hsolve code */
			    solved=2;
			    break;
			case CIN:
			case COUT:
			case PERMEABILITY:
			/* all handled by the hsolve code */
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case CHAN_OP:
		    switch (type) {
			case VOLTAGE:
			/* all handled by the hsolve code */
			    solved=2;
			    break;
			case CONCEN1:
			case CONCEN2:
			case DOMAINCONC:
			case EK:
			/* all handled by the hsolve code */
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case TABCURR_OP:
		    switch (type) {
			case VOLTAGE:
			/* all handled by the hsolve code */
			    solved=2;
			    break;
			case CONCEN1:
			case CONCEN2:
			case DOMAINCONC:
			case ADD_GBAR:
			/* all handled by the hsolve code */
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case SYN2_OP:
		    switch (type) {
			case VOLTAGE:
			/* all handled by the hsolve code */
			    solved=2;
			    break;
			case SPIKE:
			    solved=4;
			    break;
			case ACTIVATION:
			    h_add_iinfo(hsolve,msgin,SYN2_OP,sChildType,hsolve->ops[nop],nchip);
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case MGBLOCK_OP:
		    switch (type) {
			case VOLTAGE:
			case CHANNEL2:
			/* all handled by the hsolve code */
			    solved=2;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case CONC_OP:
		    switch (type) {
			/* all handled by the hsolve code */
			case INFLUX:
			    /* special case for fake inflow */
			    if (found && (hsolve->childtypes[j]==NEUTRAL_T)) {
				/* concindex */
				if (hasflux) {
				    printf("** Warning - during SETUP of %s: only one INFLUX msg from %s per shell processed.\n",Pathname(hsolve),Pathname(children[j]));
				} else {
				    /* get conc index */
				    j=hsolve->childmsg[hsolve->childlink[j]];
				    h_add_iinfo(hsolve,msgin,FLUX_OP,sChildType,nop,j);
				}
				hasflux=1;
			    }
			    solved=1;
			    break;
			case OUTFLUX:
			case FINFLUX:
			case FOUTFLUX:
			    if (found && (hsolve->childtypes[j]==NEUTRAL_T)) {
				found=0;	/* no flux scaling possible */
			    }
			    solved=1;
			    break;
			case DIFF_UP:
			case DIFF_DOWN:
			case BUFFER:
			case TAUPUMP:
			case MMPUMP:
			case HILLPUMP:
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case BUFFER_OP:
		    switch (type) {
			/* all handled by the hsolve code */
			case BCONCEN:
			case BDIFF_UP:
			case BDIFF_DOWN:
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case TAUPUMP_OP:
		    switch (type) {
			/* all handled by the hsolve code */
			case VOLTAGE:
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
		case MMPUMP_OP:
		    switch (type) {
			/* all handled by the hsolve code */
			case BCONCEN:
			    solved=1;
			    break;
			default:
			    h_msg_alert(hsolve,type,elm);
		    }
		    break;
	    }
	if (!found) {
	  if (solved==1) {
	    printf("** Warning - during SETUP of %s: message type %d to %s cannot be executed as its src %s is not hsolved.\n",Pathname(hsolve),type,Pathname(elm),Pathname(src));
	    /* Check if src is child of elm or vice versa */
	    if (Strindex(BaseObject(src)->name,"compartment")>=0) {
		/* src is a compartment -> elm should be a child */
		printf("   %s should be a child of %s.\n",Pathname(elm),Pathname(src));
	    } else if (Strindex(BaseObject(elm)->name,"compartment")>=0) {
		/* elm is a compartment -> src should be a child */
		printf("   %s should be a child of %s.\n",Pathname(src),Pathname(elm));
	    } else {
		/* both are children */
		printf("   %s and %s should be children of the same compartment.\n",Pathname(src),Pathname(elm));
	    }
	  } else if (solved==3) {
	    printf("** Warning - during SETUP of %s: src %s of message %d to %s is not hsolved.\n",Pathname(hsolve),Pathname(src),type,Pathname(elm));
	  }
	}
    }
    return(0);
}

int h_has_output(elm)
/* Checks if elm has to be updated for output.
** At present only by checking HsolveBlock status.
*/
    Element *elm;
{
    MsgOut  *msgout;

    MSGOUTLOOP(elm, msgout) {
	if (!IsHsolved(msgout->dst)) return(1);
		/* msg to an element that is not executed by hsolve */
    }
    return (0);
}

int h_msg_alert(hsolve,type,elm)
    Hsolve  *hsolve;
    int     type;
    Element *elm;
{
    printf("** Warning - during SETUP of %s: message type %d to %s not supported by hsolver.\n",Pathname(hsolve),type,Pathname(elm));
    return(0);
}

int h_add_iinfo(hsolve,msgin,op,sChildType,nop,nchip)
    Hsolve  *hsolve;
    MsgIn   *msgin;
    int     op,nop,nchip;
    short   sChildType;
{
    Iinfo   *iinfo;

    iinfo = (Iinfo *)calloc(1,sizeof(Iinfo));
    iinfo->next = hsolve->ininfo;
    hsolve->ininfo = iinfo;
    iinfo->type = op;
    iinfo->msgin = msgin;
    iinfo->nop = nop;
    iinfo->sChildType = sChildType;
    iinfo->nchip = nchip;
    return(0);
}

int h_in_msgs(hsolve)
    Hsolve   *hsolve;
{
    register Iinfo   *iinfo;
    register double *chip;
    register double X,dt;

    chip=hsolve->chip;
    if (BaseObject(hsolve)->method == CRANK_INT)
        dt = 0.50 * hsolve->dt;
    else /* BEULER by default */
        dt = hsolve->dt;

    for (iinfo=hsolve->ininfo;iinfo;iinfo=iinfo->next){
      switch(iinfo->type) {
        case INJECT_OP:
        case BASE_OP:
        case THRESH_OP:
            chip[iinfo->nchip]=MSGVALUE(iinfo->msgin,0);
            break;
        case SYN2_OP:
            X=MSGVALUE(iinfo->msgin,0);
            if (X > VTINY)
	    {
		/*- for activation msg's to synchans (non-channelC[23]) */

		if (iinfo->msgin->type == ACTIVATION
		    && iinfo->sChildType == SYNCHANC_T)
		{
		    /*- add activation value without precalculated 
		     * normalization */

		    chip[iinfo->nchip]
			+= X * hsolve->stablist[iinfo->nop + 3] * hsolve->dt;
		}

		/*- else */

		else
		{
		    /*- add normalized-to-dt value */

		    chip[iinfo->nchip]
			+= X * hsolve->stablist[iinfo->nop + 3];
		}
            }
            break;
        case FLUX_OP:
		/* also clears flux (necessary if chanmode==4) */
			hsolve->flux[iinfo->nchip]=MSGVALUE(iinfo->msgin,0);
            break;
      }
    }
    return(0);
}

int h_store_out(hsolve,op,comptno,cindex)
    Hsolve   *hsolve;
    int	     op,comptno,cindex;
{
    Oinfo    *oinfo;

    if (hsolve->chanmode<3) return(0);	/* don't store anything */
    oinfo = (Oinfo *)calloc(1,sizeof(Oinfo));
    oinfo->next = hsolve->outinfo;
    hsolve->outinfo = oinfo;
    oinfo->op = op;
    oinfo->comptno = comptno;
    oinfo->cindex = cindex;
    return(0);
}

int h_out_msgs(hsolve)
    Hsolve   *hsolve;
{
    register Oinfo   *oinfo;
    extern   short   clock_active[NCLOCKS];
    int	     cindex,concindex;

    if (clock_active[hsolve->outclock]) {
      for (oinfo=hsolve->outinfo;oinfo;oinfo=oinfo->next){
	cindex=oinfo->cindex;
	switch(oinfo->op) {
	    case COMPT_T:
		    chip_get_compt(hsolve,oinfo->comptno);
		    break;
	    case CACONCEN_T:
		    concindex=hsolve->childmsg[cindex];
		    chip_get_concen(hsolve,cindex,concindex);
		    break;
	    case NERNST_T:
		    concindex=hsolve->childlink[cindex];
		    chip_get_nernst(hsolve,cindex,concindex);
		    break;
	    case GHK_T:
		    concindex=hsolve->childlink[cindex];
		    chip_get_ghk(hsolve,cindex,concindex);
		    break;
	    case TABCHAN_T:
		    chip_get_tabchannel(hsolve,cindex);
		    break;
	    case CHANC2_T:
	    case CHANC3_T:
	    case SYNCHANC_T:
		    chip_get_channelc3(hsolve,cindex);
		    break;
	    case DIFSHELL_T:
	    case DIFSLAB_T:
	    case DIFUSER_T:
		    concindex=hsolve->childmsg[cindex];
		    chip_get_shell(hsolve,cindex,concindex);
		    break;
	    case FIXBUFF_T:
	    case BUFSHELL_T:
	    case BUFSLAB_T:
	    case BUFUSER_T:
		    concindex=hsolve->childmsg[cindex];
		    chip_get_buffer(hsolve,cindex,concindex);
		    break;
	    case BUF2SHELL_T:
	    case BUF2SLAB_T:
	    case BUF2USER_T:
		    concindex=hsolve->childmsg[cindex];
		    chip_get_d2buffer(hsolve,cindex,concindex);
		    break;
	    case TAUPUMP_T:
		    chip_get_taupump(hsolve,cindex);
		    break;
	    default:
		break;
	}
      }
    }
    return(0);
}

