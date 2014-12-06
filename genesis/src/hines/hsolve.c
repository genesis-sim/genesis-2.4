static char rcsid[] = "$Id: hsolve.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $";

/* Version EDS22c 97/12/20, Erik De Schutter, Caltech & BBF-UIA 4/94-12/97 */
/* Upinder S. Bhalla Caltech May-December 1991 */

/*
** $Log: hsolve.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1999/10/17 23:15:40  mhucka
** Attempted to merge in changes from Greg Hood for fixing problems on
** the Cray T3E.  The same changes also make it possible to compile with
** optimization turned on under Red Hat 6.0's default version of gcc/egcs.
**
** Revision 1.4  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22c revison: EDS BBF-UIA 97/11/28-97/12/20
 * Made symmetric compartments work
 * Deleted use of values array
 *
** Revision 1.2  1997/05/28 23:01:09  dhb
** Replaced with version from Antwerp GENESIS 21e
**
** Revision 1.1  1992/12/11  19:03:11  dhb
** Initial revision
**
*/

#include "hines_ext.h"

/* trivial function that just loops over the funcs array and 
** performs the ops. I need to check if a case statement would
** be any faster, since this is the most intensively used function.
** I estimate about M * 17 operations, mainly the two ELIM ops */
do_fast_hsolve(hsolve)
    Hsolve  *hsolve;
{
    register int     *funcs;
    register double  temp,diavalue;
    register double  *resultvalue,*results;
    register double  *diavals = hsolve->diavals;
    register double  *ravals = hsolve->ravals;
    register int     op;
    register int     index,n=0;

    funcs = hsolve->funcs;
    /* first instructions is truncated SET_DIAG, but we do not want to change
    **  resultvalue yet */
    resultvalue = results = hsolve->results;
    diavalue = diavals[*funcs++];

    while(1) {
        op= *funcs++;
        index= *funcs++;
        if (op == FORWARD_ELIM){
            diavals[index] -= ravals[n++]*temp;
        } else if (op == BACKWARD_ELIM){
            results[index] -= ravals[n++]*temp;
        } else if (op == SCALE){
            temp = diavals[index]/diavalue;
            results[*funcs++] -= *resultvalue*temp;
        } else if (op == CALC_RESULTS){
            *resultvalue = temp = *resultvalue/diavals[index];
	    resultvalue--;
        } else if (op == SET_DIAG){
            resultvalue++;
            diavalue = diavals[index];
        } else if (op == FINISH){
            break;
        }
    }
    return(0);			/* added by Greg Hood, PSC, 9/14/98 */
}
