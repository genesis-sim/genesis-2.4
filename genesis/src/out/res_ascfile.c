static char rcsid[] = "$Id: res_ascfile.c,v 1.6 2006/03/08 05:03:39 svitak Exp $";

/* Version EDS17, Erik De Schutter, Caltech 1991 */

/*
** $Log: res_ascfile.c,v $
** Revision 1.6  2006/03/08 05:03:39  svitak
** Changed output values to double precision.
**
** Revision 1.5  2006/03/08 04:13:05  svitak
** Apply float_format field to simulation_time.
**
** Revision 1.4  2006/02/17 07:42:01  svitak
** Added float_format field and CREATE action to initialize it.
**
** Revision 1.3  2005/07/20 20:02:02  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1997/05/29 08:45:29  dhb
** Initial revision
**
** Revision 1.1  1997/05/29 08:45:29  dhb
** Initial revision
**
*/

#include <math.h>
#include "out_ext.h"

void ResAscWriteOutput(output)
struct res_ascfile_type *output;
{
FILE *fp;
MsgIn	*msg;
double	dVal;
double	cur_values[100];
int		i;
short	do_it;

	do_it=0;
	i=0;
    MSGLOOP(output,msg){
	case 0:
	    dVal = MsgValue(msg,double,0);
		if (((SimulationTime()-output->last_time)>output->time_res) ||
		    (fabs(dVal-output->last_values[i])>output->value_res))
/*printf("time:%d %f  %f\n",i,SimulationTime()-output->last_time,output->time_res);
printf("val:%d %f  %f\n",i,fabs(dVal-output->last_values[i])>output->value_res);*/
			do_it=1;
		cur_values[i]=dVal;
		i++;
	    break;
    }
	if (do_it) {
		output->last_time=SimulationTime();
		fp = output->fp;
		fprintf(fp,output->float_format,simulation_time);
		fprintf(fp," ");
		for (i=0; i<output->nvalues; i++) {
			fprintf(fp,output->float_format,cur_values[i]);
			fprintf(fp," ");
			output->last_values[i]=cur_values[i];
		}
		fprintf(fp,"\n");
		/* force it to disk if actioned */
		if(output->flush)
			fflush(fp);
	}
}


void ResAscFileOutput(output,action)
struct res_ascfile_type *output;
Action		*action;
{
MsgIn	*msg;

    switch(action->type){
    case CREATE:
	output->float_format = "%g";
	break;

    case RESET:
		/* set the initialization flag */
		if(!output->append){
			output->initialize = TRUE;
		}
		/* force output of first value */
		if (output->time_res>0) {
			output->last_time=-2*output->time_res;
		} else {
			output->last_time=-1000;
		}
		output->nvalues=0;
		MSGLOOP(output,msg){
		case 0:
			output->nvalues++;
			break;
		}
		if (output->nvalues>100)
		   ErrorMessage("Res_ascfile", "Too many SAVE msgs.",output);
		if (output->last_values) free(output->last_values);
		output->last_values=(double *)calloc(output->nvalues,sizeof(double));
		break;
    case PROCESS:
		/* has the file been initialized? */
		if(output->initialize){
			AscSetupFile(output);
		}
		/* open the file and append data to it */
		if(OpenOutputFile(output,"a")){
			ResAscWriteOutput(output);
			ConditionalCloseOutputFile(output);
		}
		break;
	case CHECK:
		if ((output->time_res<=0)||(output->value_res<=0)) 
		   ErrorMessage("Res_ascfile", "time_res or value_res not set.",output);
		break;
    case DELETE:
    case SAVE:
		CloseOutputFile(output);
		break;
    default:
		InvalidAction("FileOutput",output,action);
		break;
    }
}
