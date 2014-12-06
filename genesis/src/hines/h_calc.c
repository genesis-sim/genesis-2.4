static char rcsid[] = "$Id: h_calc.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/*  Version EDS22j 00/03/07, Erik De Schutter, Caltech & BBF-UIA 12/91-3/00
** Upinder S. Bhalla Caltech May-December 1991 */

/*
** $Log: h_calc.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/05/26 22:26:03  mhucka
** Latest version from EDS, adds support for calculating Im.
**
** EDS22j revision: HC&EDS BBF-UIA 00/03/06-00/03/07
** computation of Im
**
** Revision 1.6  1999/10/17 21:39:36  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
** EDS22c revison: EDS BBF-UIA 97/11/28-97/12/20
** Made symmetric compartments work
** Deleted use of values array, folded diagonals into results array like
**  for chnamode>1
**
** Revision 1.4	 1997/05/28 22:34:40  dhb
** Moved Antwerp version header up in file; removed redundant log message
**
** Revision 1.3	 1997/05/28 22:32:50  dhb
** Replaced with Antwerp GENESIS 21e version
**
** Revision 1.2	 1995/08/01  19:29:03  dhb
** Changes from Erik DeSchutter described below.
** Also fixed bug in initialization of useinject.
**
** Revision 1.4	 1994/03/22 eds
** Included special code for INJECT/EREST msg in chanmode 0/1
**
** Revision 1.1	 1992/12/11  19:03:08  dhb
** Initial revision
**
*/

#include "hines_ext.h"

/* This routine uses a large chunk of CPU time, calculating coeffs
** This version disobeys all rules of object orientedness, taking
** over all the calculations that should be done by the compartments */
/* Chanmode 0-1 only */

void do_hcalc(hsolve)
    Hsolve	*hsolve;
{
    int	ncompts=hsolve->ncompts;
    int	chanmode=hsolve->chanmode;
    int	*elmnum=hsolve->elmnum;
    double	*results=hsolve->results;
    double	*diagonals=hsolve->diagonals;
    Comp	**compts=(Comp **)hsolve->compts;
    Tcinfo	**hh=hsolve->hh;
    Cinfo	**chan=hsolve->chan;
    Comp	*compt;
    Tchan	*h;
    Tcinfo	*hentry;
    Cinfo	*centry;
    struct channelA_type *ch; /* basic channel */
    register int	i;
    int	comptindex;
    double	dt,sumgchan,ichan,tbyc;
    double	Rm,Ra,Em,inject,Vm;
    MsgIn	*msgin;
    short	*msgcompts = hsolve->msgcompts;
    int	useinject;

    if (BaseObject(hsolve)->method == CRANK_INT)
	dt = Clockrate(hsolve)/2.0;
    else /* BEULER by default */
	dt = Clockrate(hsolve);

    for(i=0;i<ncompts;i++)
    {
	comptindex=elmnum[i];
	compt = compts[comptindex];
	Vm = compt->Vm; /* perhaps */
	Rm = compt->Rm; /* definitely */
	Ra = compt->Ra; /* definitely */
	tbyc=dt/compt->Cm;

	if (msgcompts[comptindex])
	{
	    double dIm = 0.0;
	    inject = 0.0;
	    useinject = 0;

	    /* copy part of the compartment MSGLOOP: yes it is another
	    **	hack folks, and it is slow to boot! */

	    MSGLOOP(compt, msgin) {
	    case INJECT:
		inject += MSGVALUE(msgin,0);
		useinject++;
		dIm += MSGVALUE(msgin,0);
		break;
	    case EREST:
		compt->Em = MSGVALUE(msgin,0);
		break;
	    case RAXIAL:  /* resistive axial */
	    {
		if (!hsolve->symflag)
		{
		    Comp * adj_compt = (Comp *)(msgin->src);
		    dIm += (adj_compt->Vm - Vm) / adj_compt->Ra;
		    break;
		}
		else
		{
		    Ncomp * adj_compt = (Ncomp *)(msgin->src);
		    dIm += (adj_compt->Vm - Vm) / (adj_compt->Ra
						   * ((Ncomp *)compt)->coeff);
		    break;
		}
	    }
	    case AXIAL:	 /* non-resistive axial */
	    {
		if (!hsolve->symflag)
		{
		    Comp * adj_compt = (Comp *)(msgin->src);
		    dIm += (adj_compt->Vm - Vm) / Ra;
		    break;
		}
		else
		{
		    Ncomp * adj_compt = (Ncomp *)(msgin->src);
		    dIm += (adj_compt->Vm - Vm) / (adj_compt->Ra
						   * ((Ncomp *)compt)->coeff2);
		    break;
		}
	    }
	    case CONNECTCROSS:
	    {
		if (!hsolve->symflag)
		{
		    /*
		    **t handle RANDINJECT msg here
		    **t this is not yet handled in hines_msgs.c/h_check_msgs()
		    **t so probably this code is never reached
		    */

		    break;
		}
		else
		{
		    Ncomp * adj_compt = (Ncomp *)(msgin->src);
		    dIm += (adj_compt->Vm - Vm) / (adj_compt->Ra
						   * ((Ncomp *)compt)->coeff2);
		    break;
		}
	    }
	    default:
		break;
	    }

	    if (useinject) compt->inject = inject;
	    compt->Im = dIm;
	}
	Em = compt->Em; /* perhaps */
	ichan=sumgchan=0.0;
	if (chanmode) {
	    /* CHANMODE 1 */
	    /* Calculating contribution due to tabchannels */
	    for (hentry=hh[i];hentry;hentry=hentry->next) {
		h=(struct tab_channel_type *)(hentry->chan);
		sumgchan+=h->Gk;
		ichan+=h->Ek*h->Gk;
	    }
	}
	/* Calculating contribution due to other channels */
	for (centry=chan[i];centry;centry=centry->next) {
	    ch=(struct channelA_type *)(centry->chan);
	    sumgchan+=ch->Gk;
	    ichan+=ch->Ek*ch->Gk;
	}

	results[2*i]=Vm+tbyc*(compt->inject+Em/Rm+ichan);
	results[2*i+1]=diagonals[i]+tbyc*sumgchan;
    }
}

