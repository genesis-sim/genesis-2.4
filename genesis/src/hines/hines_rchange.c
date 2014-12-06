static char rcsid[] = "$Id: hines_rchange.c,v 1.2 2005/07/20 20:02:00 svitak Exp $";

/* Version EDS20i 95/06/02, Erik De Schutter, Caltech & BBF-UIA 4/92-8/94 */
/* NOT FUNCTIONAL */

/*
** $Log: hines_rchange.c,v $
** Revision 1.2  2005/07/20 20:02:00  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

#include <math.h>
#include "hines_ext.h"

int h_rchip_change(hsolve,field,offset,oldvalue,newvalue)
/* Changes all instances of chip entry offset distance from field coded in 
**  ops by 'field' and having value==oldvalue to newvalue. 
** Implementation of HCHANGE action */

	Hsolve	*hsolve;
    int     field,offset;
	double	oldvalue,newvalue;
{
	int		i,n,nops,op,nchip;
    double  *chip;
    int     *ops;

	chip = hsolve->chip;
	ops = hsolve->ops;
	nops=hsolve->nops;
	
	nchip=0;
	n=0;
/* scan the ops and chip arrays till we find an instance of field */
	for (i=0; i<nops; i++) {
		op=ops[i];
		if ((op==field)&&(fabs(chip[nchip+offset]-oldvalue)<1.0e-30)) {
			chip[nchip+offset]=newvalue;	/* found and updated one field */
			n++;
		}
		/* now update nchip and i pointers */
		/*
		switch (op) {
			case COMPT_OP:
			case FCOMPT_OP:
				nchip+=2;
				break;
			case CONC_VAL_OP:
				i++;
				break;
			case GET_CONC_OP:
				break;
			case NERNST_OP:
				nchip+=2;
				break;
			case GHK_OP:
				nchip+=3;
				break;
			case CHAN_EK_OP:
				nchip+=2;
				break;
			case SAME_GHK_OP:
			case CHAN_OP:
				nchip++;
				break;
			case XGATE_OP:
			case D2GATE_OP:
			case ZGATE_OP:
				i+=2;
				nchip++;
				break;
			case ADD_CURR_OP:
				break;
			case FLUX_OP:
				break;
			case SCALE_FLUX_OP:
				nchip++;
				break;
			case SYN3_OP:
				nchip++;
			case SYN2_OP:
				i++;
				nchip+=2;
				break;
			case STORE_ALLI_OP:
			case STORE_ALLC_OP:
				i++;
				break;
			case CONC_OP:
				nchip+=3;
				break;
		}
			*/
	}
	if (IsSilent() < 2)
			printf("changed %d fields\n",n);
	return(0);
}

