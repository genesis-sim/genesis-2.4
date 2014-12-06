static char rcsid[] = "$Id: out_file.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_file.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2000/06/12 04:48:38  mhucka
** Added missing type specifier in a variable declaration in ParWriteOutput().
**
** Revision 1.8  2000/06/12 04:48:38  mhucka
** Added missing type specifier in a variable declaration in ParWriteOutput().
**
** Revision 1.7  2000/04/03 21:05:48  mvanier
** Added error check in some helper functions in out_file.c, which prevent
** us from trying to close a NULL file pointer.
**
** Revision 1.6  2000/03/31 03:44:34  mvanier
** Minor modifications.
**
** Revision 1.5  1997/07/18 19:56:23  dhb
** Merged in 1.3.1.1 changes
**
** Revision 1.4  1997/07/18 03:06:15  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3.1.1  1997/07/18 15:59:10  dhb
** Changes from PSC: parallel versions of disk_out output object,
**
** Revision 1.3  1993/02/26 18:03:19  dhb
** 1.4 to 2.0 command argument changes.
**
 * Revision 1.2  1992/10/29  18:01:28  dhb
 * Replaced explicit msgin looping with the MSGLOOP macro.
 *
 * Revision 1.1  1992/10/29  17:52:11  dhb
 * Initial revision
 *
*/

#include "out_ext.h"

/*
** try to open the file associated with the output
** return the state of the file
*/
int OpenOutputFile(output,mode)
struct file_type *output;
char    *mode;
{
char *filename;

    if(output->filename){
    filename = output->filename;
    } else {
    filename = output->name;
    }
    if(!output->is_open && (output->fp = fopen(filename,mode)) == NULL){
    Error();
    printf ("unable to open file %s with mode %s\n", 
    filename,mode);
    output->is_open = FALSE;
    return(0);
    } else {
    output->is_open = TRUE;
    }

    return(output->is_open);
}

void ConditionalCloseOutputFile(output)
struct file_type *output;
{
    if (output->is_open && !output->leave_open)
    {
        if (output->fp != NULL)  /* This shouldn't happen. */
            fclose(output->fp);

        output->fp = NULL;
        output->is_open = FALSE;
    }
}

void CloseOutputFile(output)
struct file_type *output;
{
    if (output->is_open)
    {
        if (output->fp != NULL)  /* This shouldn't happen. */
            fclose(output->fp);

        output->fp = NULL;
        output->is_open = FALSE;
    }
}

void SetupFile(output)
struct file_type *output;
{
char    label[100];
float   fval;
int     ndata;
int     datatype;
MsgIn   *msg;
float   coord;

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
    /*
    ** count the data points
    */
    ndata = 0;
    MSGLOOP(output, msg) {
        /*
        ** SAVE 
        */
        case 0:
        ndata++;
        break;
    }
    /*
    ** write out the file header
    */
    strcpy(label,"FMT1");
    fwrite (label, sizeof (char),80, output->fp);
    /*
    ** starting time
    */
    fval = SimulationTime();
    fwrite (&fval, sizeof (float),1, output->fp);
    /*
    ** time step
    */
    fval = Clockrate(output);
    fwrite (&fval, sizeof (float),1,output->fp);
    /*
    ** number of data points
    */
    fwrite (&ndata, sizeof (int),1, output->fp);
    /*
    ** data type
    */
    /*
    datatype = output->datatype;
    */
    datatype = FLOAT;
    fwrite (&datatype, sizeof (int),1, output->fp);
    /*
    ** write out xyz coords of the data
    */
    MSGLOOP(output, msg) {
        /*
        ** SAVE 
        */
        case 0:
        coord = msg->src->x;
        fwrite(&coord,sizeof(float), 1, output->fp);
        coord = msg->src->y;
        fwrite(&coord,sizeof(float), 1, output->fp);
        coord = msg->src->z;
        fwrite(&coord,sizeof(float), 1, output->fp);
        break;
    }
    ConditionalCloseOutputFile(output);
    /*
    ** indicate that the file has been initialized
    */
    output->initialize = FALSE;
    }
}

void WriteOutput(output)
struct file_type *output;
{
FILE *fp;
int     datasize;
MsgIn   *msg;
float   fval;

    fp = output->fp;
    /*
    datasize = output->datatype;
    */
    datasize = sizeof(float);
    MSGLOOP(output,msg){
    case 0:
        fval = MsgValue(msg,float,0);
        fwrite(&fval,datasize,1,fp);
        break;
    }
    /*
    ** force it to disk if actioned
    */
    if(output->flush)
    fflush(fp);
}


void FileOutput(output,action)
struct file_type *output;
Action      *action;
{
    switch(action->type){
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
        SetupFile(output);
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
    InvalidAction("FileOutput",output,action);
    break;
    }
}

void ParWriteOutput(output)
struct file_type *output;
{
  FILE  *fp;
  int   datasize;
  Msg   *msg;
  int   index;
  int   nvals;
  static float *buffer = NULL;
  static int buffer_size = 0;

  fp = output->fp;
  /*
    datasize = output->datatype;
    */
  datasize = sizeof(float);
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
    buffer = (float *) realloc(buffer, buffer_size*datasize);
      }
    buffer[index] = MsgValue(msg, float, 1);
    if (index >= nvals)
      nvals = index + 1;
    break;
  }
  fwrite(buffer,datasize,nvals,fp);

  /*
  ** force it to disk if actioned
  */
  if(output->flush)
    fflush(fp);
}

/* Par* functions added by ghood (PSC) */
void ParFileOutput(output,action)
struct file_type *output;
Action      *action;
{
    switch(action->type){
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
        SetupFile(output);
    }
    /*
    ** open the file and append data to it
    */
    if(OpenOutputFile(output,"a")){
        ParWriteOutput(output);
        ConditionalCloseOutputFile(output);
    }
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

void do_read_header(argc,argv)
int argc;
char    **argv;
{
float   fval;
int ndata;
int datatype;
FILE    *fp;

    initopt(argc, argv, "file-name");
    if (G_getopt(argc, argv) != 0)
      {
    printoptusage(argc, argv);
    return;
      }

    if((fp = fopen(optargv[1],"r")) == NULL){
    Error();
    printf("unable to find file '%s'\n",optargv[1]);
    return;
    }
    /*
    ** go to the beginning of the file
    */
    fseek(fp, 0L, 0);
    /*
    ** read in the file header
    */
    /*
    ** starting time
    */
    fread (&fval, sizeof (float),1,fp);
    printf("%-20s = %e\n","start time",fval);
    /*
    ** time step
    */
    fread (&fval, sizeof (float),1,fp);
    printf("%-20s = %e\n","time step",fval);
    /*
    ** number of data points
    */
    fread (&ndata, sizeof (int),1,fp);
    printf("%-20s = %d\n","ndata points",ndata);
    /*
    ** data type
    */
    fread (&datatype, sizeof (int),1,fp);
    printf("%-20s = %d\n","data type",datatype);
    fclose(fp);
}
