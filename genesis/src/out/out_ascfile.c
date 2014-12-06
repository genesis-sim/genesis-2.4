static char rcsid[] = "$Id: out_ascfile.c,v 1.6 2006/03/08 05:03:38 svitak Exp $";

/*
** $Log: out_ascfile.c,v $
** Revision 1.6  2006/03/08 05:03:38  svitak
** Changed output values to double precision.
**
** Revision 1.5  2006/03/08 04:13:04  svitak
** Apply float_format field to simulation_time.
**
** Revision 1.4  2006/02/22 03:59:02  svitak
** Added CREATE action to ParAscFileOutput.
**
** Revision 1.3  2006/02/17 07:42:01  svitak
** Added float_format field and CREATE action to initialize it.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2000/06/12 04:48:10  mhucka
** Added missing type specifier in a variable declaration in ParAscWriteOutput().
**
** Revision 1.6  2000/06/12 04:48:10  mhucka
** Added missing type specifier in a variable declaration in ParAscWriteOutput().
**
** Revision 1.5  2000/03/31 02:06:28  mvanier
** Fixed minor bug.
**
** Revision 1.4  2000/03/31 01:14:29  mvanier
** Added extra actions to asc_file object (OUT_OPEN and OUT_WRITE).  These
** allow you to add lines to a file that is being written to by an
** asc_file object without having to worry about buffering problems
** (as you do if you just do writefile, for instance).
**
** Revision 1.3  1997/07/18 19:55:38  dhb
** Changes from PSC: parallel versions of ascii and FMT1 I/O objects
**
** Revision 1.2  1994/10/04  23:01:43  dhb
** Added FLUSH action.
**
** Revision 1.1  1992/12/11  19:03:19  dhb
** Initial revision
**
*/

#include "out_ext.h"
#include "output_g@.h"

void AscSetupFile(output)
struct ascfile_type *output;
{

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
	ConditionalCloseOutputFile(output);
	/*
	** indicate that the file has been initialized
	*/
	output->initialize = FALSE;
    }
}


void AscWriteOutput(output)
struct ascfile_type *output;
{
FILE *fp;
MsgIn	*msg;
double	dVal;

    fp = output->fp;
    if(!output->notime){
	fprintf(fp,output->float_format,simulation_time);
	fprintf(fp," ");
    }
    MSGLOOP(output,msg){
	case 0:
	    dVal = MsgValue(msg,double,0);
	    fprintf(fp,output->float_format,dVal);
	    fprintf(fp," ");
	    break;
    }
    fprintf(fp,"\n");
    /*
    ** force it to disk if actioned
    */
    if(output->flush)
	fflush(fp);
}

int AscFileOutput(output,action)
struct ascfile_type *output;
Action		*action;
{
    int i;

    switch(action->type){
    case CREATE:
	output->float_format = "%g";
	break;

    case RESET:
	/*
	** set the initialization flag
	*/
	if(!output->append){
	    output->initialize = TRUE;
	}
	break;

    case OUT_OPEN:
        /* Manually initialize the file. */
        AscSetupFile(output);
        break;

    case OUT_WRITE:
        /* Write strings to a data file. */
        if (output->fp == NULL)
        {
            Error();
            printf("ascfile %s: output file is not open!\n",
                   Pathname(output));
            return 0;
        }

        fflush(output->fp);
        for (i = 0; i < action->argc; i++)
            fprintf(output->fp, "%s ", action->argv[i]);

        fprintf(output->fp, "\n");
        fflush(output->fp);

        break;

    case PROCESS:
	/*
	** has the file been initialized?
	*/
	if(output->initialize){
	    AscSetupFile(output);
	}
	/*
	** open the file and append data to it
	*/
	if(OpenOutputFile(output,"a")){
	    AscWriteOutput(output);
	    ConditionalCloseOutputFile(output);
	}
	break;
    case DELETE:
    case SAVE:
	CloseOutputFile(output);
	break;
    case FLUSH:
	if (output->is_open)
	    fflush(output->fp);
	break;
    default:
	InvalidAction("FileOutput",output,action);
	break;
    }
	return 1;
}

void ParAscWriteOutput(output)
struct ascfile_type *output;
{
  FILE *fp;
  Msg	*msg;
  int	index;
  int	nvals;
  static double *pdBuffer = NULL;
  static int buffer_size = 0;

  fp = output->fp;
  if(!output->notime){
    fprintf(fp,output->float_format,simulation_time);
    fprintf(fp," ");
  }
  nvals = 0;
  MSGLOOP(output,msg){
  case 0:
    index = MsgValue(msg, int, 0);
    if (index >= buffer_size)
      {
	if (buffer_size == 0)
	  buffer_size = 16;
	while (index >= buffer_size)
	  buffer_size *= 2;
	pdBuffer = (double *) realloc(pdBuffer, buffer_size*sizeof(double));
      }
    pdBuffer[index] = MsgValue(msg, double, 1);
    if (index >= nvals)
      nvals = index + 1;
	    break;
    }
  for (index = 0; index < nvals; ++index)
  {
    fprintf(fp,output->float_format,pdBuffer[index]);
    fprintf(fp," ");
  }
  fprintf(fp,"\n");
  /*
  ** force it to disk if actioned
  */
  if(output->flush)
    fflush(fp);
}


/* Par* functions added by ghood (PSC) */
void ParAscFileOutput(output,action)
struct ascfile_type *output;
Action		*action;
{
    switch(action->type){
    case CREATE:
	output->float_format = "%g";
	break;

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
	    AscSetupFile(output);
	}
	/*
	** open the file and append data to it
	*/
	if(OpenOutputFile(output,"a")){
	    ParAscWriteOutput(output);
	    ConditionalCloseOutputFile(output);
	}
	break;
    case DELETE:
    case SAVE:
	CloseOutputFile(output);
	break;
    case FLUSH:
	if (output->is_open)
	    fflush(output->fp);
	break;
    default:
	InvalidAction("FileOutput",output,action);
	break;
    }
}
