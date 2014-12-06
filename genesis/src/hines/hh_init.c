static char rcsid[] = "$Id: hh_init.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/* Version EDS21e 97/05/05, Erik De Schutter, Caltech & BBF-UIA 4/92-5/97 */
/* Upinder S. Bhalla Caltech May-December 1991 */

/*
** $Log: hh_init.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/06/12 04:42:28  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.6  1997/05/28 22:45:07  dhb
** Replaced with version from Antwrep GENESIS 21e
**
** Revision 1.5  1995/12/06 00:46:02  venkat
** Changes to implement the seperately allocated element pointer array
** for non-hsolved channels.
**
 * Revision 1.4  1995/07/18  18:05:57  dhb
 * Changes all uses of struct Ca_shell_type to use struct Ca_concen_type
 * and tests for object named Ca_shell to only check for Ca_concen.
 *
 * Also, changed all check of object names to check for name of base
 * object.  This allows hsolve to solve for extended objects.
 *
 * Revision 1.3  1995/06/01  15:49:29  dhb
 * Fixes from Erik DeShutter described below.
 *
** Revision 1.5  1995/05/31 eds (bbf-uia)
 * Removed reset action to prevent division by zero in nernst
 *
** Revision 1.2  1992/10/29  17:20:26  dhb
 * Replaced explicit msgin and msgout looping with MSGLOOP and MSGOUTLOOP
 * macros.  Some indentation changes.
*/

#include "hines_ext.h"

/*
** Fills up hh array according to hines numbering of parent compts,
** and moves all vdep-channel tables into a colocated chunk so that
** page swaps are (hopefully) minimized.
*/

/* Fills up hh and chan linklists according to hines numbering of parent compts */

int h_hh_init(hsolve)
	Hsolve	*hsolve;
{
	int 	ncompts;
	Element	**compts;
	Element	*compt;
	int 	i;
	MsgIn	*msgin;
	int		*elmnum;
	Tcinfo	**hh = NULL;
	Tcinfo	*hentry;
	Cinfo	**chan;
	Cinfo	*centry;

	ncompts = hsolve->ncompts;
	compts = hsolve->compts;
	elmnum = hsolve->elmnum;
	if (hsolve->chanmode) hh = hsolve->hh = (Tcinfo **) calloc(ncompts,sizeof(Cinfo *));
	chan = hsolve->chan = (Cinfo **) calloc(ncompts,sizeof(Cinfo *));

	/* CHANNELs go from channels to compts */
	for (i=0;i<ncompts;i++){
	    compt = compts[elmnum[i]];
	    MSGLOOP(compt, msgin) {
		/* look for any channels */
		case CHANNEL:
		    if (hsolve->chanmode && (strcmp(BaseObject(msgin->src)->name,"tabchannel")==0)) {
			/* chanmode 1: we compute tabchannels separately */
			hentry = (Tcinfo *)calloc(1,sizeof(Tcinfo));
			hentry->next = hh[i];
			hh[i] = hentry;
			hentry->chan = msgin->src;
			HsolveBlock(hentry->chan);
			if (h_nernst_init(hsolve,msgin->src,hentry)) {
			    return(ERR);
			}
		    } else {
			if (strcmp(BaseObject(msgin->src)->name,"channelC")==0){
			    printf("** Warning - during SETUP of %s: cannot handle 'channelC' element '%s'. Ignoring it.\n",Pathname(hsolve),Pathname(msgin->src));
			    continue;
			}
			centry = (Cinfo *)calloc(1,sizeof(Cinfo));
			centry->next = chan[i];
			chan[i] = centry;
			centry->chan = msgin->src;
		    }
		    break;
	    }
	}
	return(0);
}

/* Finds nernst element for channel (if present) */
int h_nernst_init(hsolve,chan,hentry)
    Hsolve  *hsolve;
    Element *chan;
    Tcinfo  *hentry;
{
    MsgIn   *msgin;
    Element *nernst = NULL;
    MsgIn  *conc = NULL;


    hentry->nernst = NULL;
    hentry->conc = NULL;
    MSGLOOP(chan, msgin) {
        default:
	    if (msgin->type == EK) {
		/* Make sure that we have a nernst element */
		if (strcmp(BaseObject(msgin->src)->name,"nernst")==0) {
		    nernst = msgin->src;
		} else {
		    Error();
		    printf(" during SETUP of %s: EK message from unsupported object for %s\n",Pathname(hsolve),Pathname(chan));
		    return(ERR);
		}
	    } else if (msgin->type == CONCEN) {
		/* a concentration */
		conc = msgin;
	    }
    }
    if (nernst) {
	if (hentry->nernst) {
	    if ((Nernst *)nernst != hentry->nernst) {
		Error();
		printf(" during SETUP of %s: more than one nernst element for %s \n",Pathname(hsolve),Pathname(chan));
		return(ERR);
	    }
	} else {
	    hentry->nernst = (Nernst *)nernst;
	}
    }
    if (conc) {
	if (hentry->conc) {
	    if (conc != hentry->conc) {
		Error();
		printf(" during SETUP of %s: more than one CONCEN msg for %s\n",Pathname(hsolve),Pathname(chan));
		return(ERR);
	    }
	} else {
	    hentry->conc = conc;
	}
    }
    return(0);
}

