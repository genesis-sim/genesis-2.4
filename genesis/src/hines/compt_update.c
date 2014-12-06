static char rcsid[] = "$Id: compt_update.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/*  Version EDS22c 98/03/07, Erik De Schutter, December 1991-March 1998
** Upinder S. Bhalla Caltech May-December 1991 */

#include "hines_ext.h"

/*
** $Log: compt_update.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1999/10/17 21:39:36  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22c revison: EDS BBF-UIA 97/11/28-97/12/20
 * Made symmetric compartments work
 * Deleted use of values array, folded diagonals into results array like
 *  for chnamode>1
 *
** Revision 1.3  1997/05/28 22:27:14  dhb
** Update from Antwerp GENESIS 21e
**
** Revision 1.2  1995/07/21  23:56:20  dhb
** Changed Ca_shell_type to Ca_concen_type.
**
** Revision 1.1  1992/12/11  19:03:06  dhb
** Initial revision
*/


/* Simple function for putting data values back into elements */
/* Chanmode 0-1 only */
void do_compt_update(hsolve)
	Hsolve	*hsolve;
{
	int i;
	struct compartment_type **compts;
	int ncompts = hsolve->ncompts;
	int	*elmnum;
	double *results;
	double *v;

	compts = (struct compartment_type **)(hsolve->compts);
	elmnum = hsolve->elmnum;
	results = hsolve->results;

	if (BaseObject(hsolve)->method == CRANK_INT) {
		for(i=0;i<ncompts;i++) {
			v = &(compts[elmnum[i]]->Vm);
			*v = 2.0*results[2*i]-(*v);
		}
	} else { /* BEULER by default */
		for(i=0;i<ncompts;i++) {
			compts[elmnum[i]]->Vm = results[2*i];
		}
	}
}
