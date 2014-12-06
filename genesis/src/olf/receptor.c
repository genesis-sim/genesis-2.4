static char rcsid[] = "$Id: receptor.c,v 1.2 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: receptor.c,v $
** Revision 1.2  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1995/03/23 01:37:06  dhb
** Added number of values to SAVE2 file format
**
 * Revision 1.1  1992/11/14  00:37:21  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "olf_ext.h"

int	DEBUG_OlfReceptor = 0;
static double		savedata[6];


/*
** Olfactory Receptor. Takes weighted sum of upto 10 messages, to 
** represent components of olfactory stimulus. The weights are 
** defined in an array on the element, this enables one to construct
** any combination of odor sensitivities on the neuron. The output
** is both in the form of a membrane potential and a spike.
*/
/* Jan 90 Upinder S. Bhalla */
int OlfReceptor(rec,action)
register struct olf_receptor_type *rec;
Action 	*action;
{
int	has_state;
MsgIn 	*msg;
double	V;
int	i;
float	dt;
int	n;

    if(Debug(DEBUG_OlfReceptor) > 1){
		ActionHeader("OlfReceptor",rec,action);
    }
    SELECT_ACTION(action){
    case INIT:
		rec->Vm = rec->Vmin;
		rec->state = 0;
		break;
    case PROCESS:
		i = 0;
		V = 0.0;
		MSGLOOP(rec,msg){
		case 0:			/* ODOR: Summing the olfactory stimuli */
	    	V += MSGVALUE(msg,0) * rec->response[i];
			i++;
			if (i > NODORS)
				i = 0;
	    	break;
		case 1:			/* GAIN: Setting the gain of the receptor */
			rec->gain = MSGVALUE(msg,0);
			break;
		}
		V *= rec->gain;
		dt = Clockrate(rec);
		if ((rec->min_rate + V * (rec->max_rate - rec->min_rate)) >
			frandom(0,1))
			rec->state = 1;
		else
			rec->state = 0;

		V = (rec->Vmin + V/(rec->Vmax - rec->Vmin)) ;
		if (V < rec->Vmin)
			V = rec->Vmin;
		if (V > rec->Vmax)
			V = rec->Vmax;
		rec->Vm = V;
		break;
    case RESET:
		rec->Vm = rec->Vmin;
		rec->state = 0;
    case RECALC:
		break;
    case CHECK:
		has_state = 0;
	MSGLOOP(rec,msg){
	    case 0:
		has_state = 1;
		break;
	}
	if(!has_state){
	    ErrorMessage("OlfReceptor","no odorant input",rec);
	}
	break;
    case SAVE2:
	savedata[0] = rec->Vm;
	savedata[1] = rec->Vmin;
	savedata[2] = rec->Vmax;
	savedata[3] = rec->max_rate;
	savedata[4] = rec->min_rate;
	savedata[5] = rec->state;
        n=6;
        fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),6,(FILE*)action->data);
	break;
    case RESTORE2:
        fread(&n,sizeof(int),1,(FILE*)action->data);
        if (n != 6) {
            ErrorMessage("OlfReceptor", "Invalid savedata length", rec);
            return n;
        }
	fread(savedata,sizeof(double),6,(FILE*)action->data);
	savedata[0] = rec->Vm = savedata[0];
	savedata[1] = rec->Vmin = savedata[0];
	savedata[2] = rec->Vmax = savedata[0];
	savedata[3] = rec->max_rate = savedata[0];
	savedata[4] = rec->min_rate = savedata[0];
	savedata[5] = rec->state = savedata[0];
	break;
    }
    return(0);
}

