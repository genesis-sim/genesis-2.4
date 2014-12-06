static char rcsid[] = "$Id: hines_solve.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/* Version EDS22d 98/09/30, Erik De Schutter, Caltech & BBF-UIA 4/92-9/98 */

/*
** $Log: hines_solve.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22c revison: EDS BBF-UIA 98/09/29-98/09/30
 * Corrected bug in do_fast_hsolve
 *
 * EDS22c revison: EDS BBF-UIA 97/11/28-98/05/04
 * Rewrote funcs array and corresponding hsolve code
 * Made symmetric compartments work
 *
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* Contains the second loops for different chanmodes and solution methods:
**  a loop which executes the funcs array (do_crank_hsolve or
**     do_euler_hsolve): solve 13.18 of Mascagni. 
** in case of chanmode==2, a last loop updates Vm values (do_vm_update)
** For reasons of efficiency we have separate routines for symmetric
**  versus non symmetric cases
*/

#include "hines_ext.h"

/* trivial function that just loops over the funcs array and 
** performs the ops.  Does not update vm array.
*/
void do_fast_hsolve(hsolve)
    Hsolve  *hsolve;
{
    register int     *funcs=hsolve->funcs;
    register int     op;
    register double  temp = 0.0,resultval,diaval;
    register double  *results=hsolve->results;
    register double  *ravals=hsolve->ravals;
    register double  *raval=hsolve->ravals;
    register double  *resultvalue;

    op= *funcs++;
    if (op == FINISH) {	/* one compartment only! */
	*results = *results / *(results+1);
	return;
    } else {
	/* Do forwards elimination starting at row 1 (row 0 skipped) */
	resultvalue=results+2;
	resultval=*resultvalue;
	diaval=*(resultvalue+1);
    }

   while(1) {
        if (op == FORWARD_ELIM) {
            temp = *ravals++ / results[*funcs + 1];
	    diaval -= *ravals++ * temp;
	    resultval -= results[*funcs++] * temp;
        } else if (op == SET_DIAG) {
	    *resultvalue++=resultval;
	    *resultvalue++=diaval;
	    resultval=*resultvalue;
	    diaval=*(resultvalue+1);
        } else if (op == SKIP_DIAG) {
	    *resultvalue++=resultval;
	    *resultvalue=diaval;
	    resultvalue+=3;
	    resultval=*resultvalue;
	    diaval=*(resultvalue+1);
	} else if (op == FASTSIBARRAY_ELIM) {
	    raval[*funcs++] -= *ravals++ * temp;
	} else if (op==COPY_ARRAY) {
	    raval[*funcs++] = *ravals++;
	} else if (op == SIBARRAY_ELIM) {
	    raval[*(funcs+1)] -= raval[*funcs] * temp;
	    funcs+=2;
        } else { /* FINISH */
            break;
        }
        op= *funcs++;
    }
    /* store result last row */
    *resultvalue=resultval/diaval;
    resultvalue-=2;
    resultval=*resultvalue;

    /* Do backwards elimination */
    while(1) {
        op= *funcs++;
        if (op == BACKWARD_ELIM) {
	    resultval -= *ravals++ * results[*funcs++];
        } else if (op == CALC_RESULTS) {
            *resultvalue = resultval / *(resultvalue+1);
            resultvalue-=2;
	    resultval=*resultvalue;
	} else if (op == SIBARRAY_ELIM) {
	    resultval -= raval[*funcs] * results[*(funcs+1)];
	    funcs+=2;
        } else { /* FINISH */
            break;
	}
    }
}

/* Simple function for putting data values back into elements,
** when the chip array is being used */
void do_vm_update(hsolve)
	Hsolve	*hsolve;
{
	register struct compartment_type **compts;
	register int	*elmnum;
	register double	*vm;

	compts = (struct compartment_type **)(hsolve->compts);
	elmnum = hsolve->elmnum;
	vm = hsolve->vm;

	/* update all comp values */
	for(vm=hsolve->vm;vm<&(hsolve->vm[hsolve->ncompts]);vm++) {
		compts[*elmnum++]->Vm = *vm;
	}
}

/* trivial function that just loops over the funcs array and
** performs the ops for symmetric compartments. */
/* performs also final Crank-Nicolson interpolation */
void do_crank_hsolve(hsolve)
    Hsolve  *hsolve;
{
    register int     *funcs=hsolve->funcs;
    register int     op;
    register double  temp = 0.0,resultval,diaval;
    register double  *results=hsolve->results;
    register double  *ravals=hsolve->ravals;
    register double  *raval=hsolve->ravals;
    register double  *vm=hsolve->vmlast;
    register double  *resultvalue;

    op= *funcs++;
    if (op == FINISH) {	/* one compartment only! */
	resultval = *results / *(results+1);
	*vm = resultval + resultval - *vm;
	return;
    } else {
	/* Do forwards elimination starting at row 1 (row 0 skipped) */
	resultvalue=results+2;
	resultval=*resultvalue;
	diaval=*(resultvalue+1);
    }

   while(1) {
        if (op == FORWARD_ELIM) {
            temp = *ravals++ / results[*funcs + 1];
	    diaval -= *ravals++ * temp;
	    resultval -= results[*funcs++] * temp;
        } else if (op == SET_DIAG) {
	    *resultvalue++=resultval;
	    *resultvalue++=diaval;
	    resultval=*resultvalue;
	    diaval=*(resultvalue+1);
        } else if (op == SKIP_DIAG) {
	    *resultvalue++=resultval;
	    *resultvalue=diaval;
	    resultvalue+=3;
	    resultval=*resultvalue;
	    diaval=*(resultvalue+1);
	} else if (op == FASTSIBARRAY_ELIM) {
	    raval[*funcs++] -= *ravals++ * temp;
	} else if (op==COPY_ARRAY) {
	    raval[*funcs++] = *ravals++;
	} else if (op == SIBARRAY_ELIM) {
	    raval[*(funcs+1)] -= raval[*funcs] * temp;
	    funcs+=2;
        } else { /* FINISH */
            break;
        }
        op= *funcs++;
    }
    /* store result last row */
    resultval=resultval/diaval;
    *vm = resultval + resultval - *vm;
    *resultvalue=resultval;
    resultvalue-=2;
    resultval=*resultvalue;

    /* Do backwards elimination */
    while(1) {
        op= *funcs++;
        if (op == BACKWARD_ELIM) {
	    resultval -= *ravals++ * results[*funcs++];
        } else if (op == CALC_RESULTS) {
            resultval = resultval / *(resultvalue+1);
	    vm--;
	    *vm = resultval + resultval - *vm;
	    *resultvalue=resultval;
            resultvalue-=2;
	    resultval=*resultvalue;
	} else if (op == SIBARRAY_ELIM) {
	    resultval -= raval[*funcs] * results[*(funcs+1)];
	    funcs+=2;
        } else { /* FINISH */
            break;
	}
    }
}

/* trivial function that just loops over the funcs array and
** performs the ops for symmetric compartments. */
/* performs also final Euler storage */
void do_euler_hsolve(hsolve)
    Hsolve  *hsolve;
{
    register int     *funcs=hsolve->funcs;
    register int     op;
    register double  temp = 0.0,resultval,diaval;
    register double  *results=hsolve->results;
    register double  *ravals=hsolve->ravals;
    register double  *raval=hsolve->ravals;
    register double  *vm=hsolve->vmlast;
    register double  *resultvalue;

    op= *funcs++;
    if (op == FINISH) {	/* one compartment only! */
	*vm = *results / *(results+1) - *vm;
	return;
    } else {
	/* Do forwards elimination starting at row 1 (row 0 skipped) */
	resultvalue=results+2;
	resultval=*resultvalue;
	diaval=*(resultvalue+1);
    }

   while(1) {
        if (op == FORWARD_ELIM) {
            temp = *ravals++ / results[*funcs + 1];
	    diaval -= *ravals++ * temp;
	    resultval -= results[*funcs++] * temp;
        } else if (op == SET_DIAG) {
	    *resultvalue++=resultval;
	    *resultvalue++=diaval;
	    resultval=*resultvalue;
	    diaval=*(resultvalue+1);
        } else if (op == SKIP_DIAG) {
	    *resultvalue++=resultval;
	    *resultvalue=diaval;
	    resultvalue+=3;
	    resultval=*resultvalue;
	    diaval=*(resultvalue+1);
	} else if (op == FASTSIBARRAY_ELIM) {
	    raval[*funcs++] -= *ravals++ * temp;
	} else if (op==COPY_ARRAY) {
	    raval[*funcs++] = *ravals++;
	} else if (op == SIBARRAY_ELIM) {
	    raval[*(funcs+1)] -= raval[*funcs] * temp;
	    funcs+=2;
        } else { /* FINISH */
            break;
        }
        op= *funcs++;
    }
    /* store result last row */
    *vm=*resultvalue=resultval/diaval;
    resultvalue-=2;
    resultval=*resultvalue;

    /* Do backwards elimination */
    while(1) {
        op= *funcs++;
        if (op == BACKWARD_ELIM) {
	    resultval -= *ravals++ * results[*funcs++];
        } else if (op == CALC_RESULTS) {
	    vm--;
            *vm=*resultvalue = resultval / *(resultvalue+1);
            resultvalue-=2;
	    resultval=*resultvalue;
	} else if (op == SIBARRAY_ELIM) {
	    resultval -= raval[*funcs] * results[*(funcs+1)];
	    funcs+=2;
        } else { /* FINISH */
            break;
	}
    }
}
