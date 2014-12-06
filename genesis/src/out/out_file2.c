static char rcsid[] = "$Id: out_file2.c,v 1.3 2005/07/20 20:02:02 svitak Exp $";

/*
** $Log: out_file2.c,v $
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
** Revision 1.1  1992/12/11 19:03:23  dhb
** Initial revision
**
*/

#include <math.h>
#include "sim_ext.h"
#include "out_struct.h"

void SetupXviewFile(output)
struct file_type *output;
{
float	fval;
int 	ndata;
int	xdim;
int	ydim;
float	xmax = 0.0;
float	xmin = 0.0;
int 	datatype;
MsgIn	*msg;
float	x = 0.0;
float	delta_x;
float	dx;

    /*
    ** try to close up the file if it has been left open
    */
    CloseOutputFile(output);
    /*
    ** try to open the file
    */
    if(OpenOutputFile(output,"w")){
	/*
	** go to the beginning of the file
	*/
	fseek(output->fp, 0L, 0);
	ndata = 0;
	delta_x = 0;
	MSGLOOP(output,msg){
	    case 0:
		/*
		** determine the x spacing by finding the minimum
		** delta x
		*/
		if(ndata == 0){
		    /*
		    ** initial values
		    */
		    xmax = xmin = x = msg->src->x;
		} else {
		    if((dx = fabs(msg->src->x - x)) > 0){
			/*
			** if non-zero spacing
			*/
			if(delta_x == 0){
			    /*
			    ** assign delta_x to the first non-zero
			    ** x spacing
			    */
			    delta_x = dx;
			} else {
			    /*
			    ** find the minimum spacing
			    */
			    delta_x = MIN(dx,delta_x);
			}
		    }
		    xmax = MAX(msg->src->x,xmax);
		    xmin = MIN(msg->src->x,xmin);
		}
		ndata++;
		break;
	}
	/*
	** write out the file header
	*/
	/*
	** xmax 
	*/
	if(delta_x > 0){
	    xdim = round((xmax - xmin)/delta_x);
	} else {
	    /*
	    ** if that fails then just assume a 1D array
	    */
	    xdim = ndata -1;
	}
	if(xdim < 0){
	    xdim = 0;
	}
	fwrite (&xdim, sizeof (int),1,output->fp);
	/*
	** ymax
	*/
	ydim = ndata/(xdim +1) - 1;
	if(ydim < 0){
	    ydim = 0;
	} 
	fwrite (&ydim, sizeof (int),1,output->fp);
	/*
	** time step
	*/
	fval = Clockrate(output);
	fwrite (&fval, sizeof (float),1,output->fp);
	/*
	** data type
	*/
	/*
	datatype = output->datatype;
	*/
	datatype = FLOAT;
	fwrite (&datatype,sizeof (int),1,output->fp);
	ConditionalCloseOutputFile(output);
	/*
	** indicate that the file has been initialized
	*/
	output->initialize = FALSE;
    }
}


void XviewFileOutput(output,action)
struct file_type *output;
Action		*action;
{
    SELECT_ACTION(action){
    case RESET:
	/*
	** set the initialization flag
	*/
	if(!output->append){
	    output->initialize = TRUE;
	}
	break;
    case PROCESS:
	/*
	** has the file been initialized?
	*/
	if(output->initialize){
	    SetupXviewFile(output);
	}
	/*
	** open the file and append data to it
	*/
	if(OpenOutputFile(output,"a")){
	    WriteOutput(output);
	    ConditionalCloseOutputFile(output);
	}
	break;
    case DELETE:
    case SAVE:
	CloseOutputFile(output);
	break;
    default:
	InvalidAction("XviewFileOutput",output,action);
	break;
    }
}

