static char rcsid[] = "$Id: receptor2.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: receptor2.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/06/12 05:07:03  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.2  1995/03/23 01:37:06  dhb
** Added number of values to SAVE2 file format
**
 * Revision 1.1  1992/11/14  00:37:22  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "olf_ext.h"

int	DEBUG_OlfReceptor2 = 0;
static double		savedata[6];


/*
** Olfactory Receptor. Takes weighted sum of any number of messages
** to get the activation for a channel. The messages represent 
** components of an olfactory stimulus. 
*/
/* Jan 90 Upinder S. Bhalla */
int OlfReceptor2(rec,action)
register struct olf_receptor2_type *rec;
Action 	*action;
{
int	has_state;
MsgIn 	*msg;
double	V = 0.0;
int	i;
int	index;
float	dt;
double	x;
float	tpeak;
int		setval;
char	*field;
int	n;

    if(Debug(DEBUG_OlfReceptor2) > 1){
		ActionHeader("OlfReceptor",rec,action);
    }
    SELECT_ACTION(action){
    case INIT:
		rec->activation = 0;
		break;
    case PROCESS:
		i = 0;
		MSGLOOP(rec,msg){
		case 0:			/* membrane potential */
			V = MSGVALUE(msg,0);
			break;
		case 1:			/* ODOR: Summing the olfactory stimuli */
			if (rec->allocated == 0)
				return 0;
			index = MSGVALUE(msg,1);
			if (index < rec->nodors)
				rec->activation +=
					MSGVALUE(msg,0) * rec->response[index];
	    	break;
		case 2:			/* modulation of the sensitivity of the rec */
			rec->modulation = MSGVALUE(msg,0);
			break;
		}
		x = rec->activation * rec->modulation * rec->xconst1 +
			rec->X * rec->xconst2;
		rec->Y = rec->X * rec->yconst1 +
			rec->Y * rec->yconst2;

		rec->X = x;
		rec->Gk = rec->Y * rec->norm;
		rec->Ik = (rec->Ek - V) * rec->Gk;
		break;
    case RESET:
		rec->activation = 0;
		rec->Gk = 0;
		rec->X = 0;
		rec->Y = 0;
    case RECALC:
		dt = Clockrate(rec);
		rec->xconst1 = rec->tau1 * (1 - exp(-dt/rec->tau1));
		rec->xconst2 = exp(-dt/rec->tau1);
		rec->yconst1 = rec->tau2 * (1 - exp(-dt/rec->tau2));
		rec->yconst2 = exp(-dt/rec->tau2);
		if (rec->tau1 == rec->tau2) {
			rec->norm = rec->gmax * M_E/rec->tau1;
		} else {
			tpeak = rec->tau1 * rec->tau2 *
			log(rec->tau1/rec->tau2)/
        	(rec->tau1 - rec->tau2);
        	rec->norm = rec->gmax*(rec->tau1 - rec->tau2)/
        	(rec->tau1*rec->tau2*
        	(exp(-tpeak/rec->tau1) - exp(-tpeak/rec->tau2)));
		}
		break;
	case SET :
		if (action->argc != 2)
			return(0); /* do the normal set */
		field = action->argv[0];
		if (strcmp(field,"nodors") != 0)
			return(0); /* do the normal set */
		setval = atoi(action->argv[1]);

		if (setval < 0) {
			printf("Cannot assign a negative size to array");
			return(1);
		}
		/* Checking if change is necessary */
		if (setval == rec->nodors)
				return(1);

		/* cleaning up the existing array */
		if (rec->allocated) {
			free(rec->response);
			rec->allocated = 0;
		}

		/* Assigning new array */
		rec->nodors = setval;
		if (!rec->allocated) {
			if (rec->nodors <= 0)
				return(1);
			rec->response = (float *) calloc(rec->nodors,sizeof(float));
			rec->allocated = 1;
		}
		return(1);
		/* NOTREACHED */
		break;
    case CHECK:
		has_state = 0;
		MSGLOOP(rec,msg){
	    	case 0:
			has_state = 1;
			break;
		}
		if(!has_state){
	    	ErrorMessage("Receptor2","no membrane potential",rec);
		}
    	if(rec->tau1 <= 0 || rec->tau2 <= 0){
        	ErrorMessage("Receptor2","Invalid tau parameters.",rec);
    	}
		break;
    case SAVE2:
		savedata[0] = rec->Gk;
		savedata[1] = rec->X;
		savedata[2] = rec->Y;
		n=3;
		fwrite(&n,sizeof(int),1,(FILE*)action->data);
		fwrite(savedata,sizeof(double),3,(FILE*)action->data);
		break;
    case RESTORE2:
		fread(&n,sizeof(int),1,(FILE*)action->data);
		if (n != 3) {
		    ErrorMessage("OlfReceptor2","Invalid savedata length",rec);
		    return n;
		}
		fread(savedata,sizeof(double),3,(FILE*)action->data);
		rec->Gk = savedata[0];
		rec->X = savedata[1];
		rec->Y = savedata[2];
		break;
    }
    return(0);
}
