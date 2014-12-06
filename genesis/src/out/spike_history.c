static char rcsid[] = "$Id: spike_history.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/* Reinoud MAEX - UIA-BBF - december 95 */

/*
** $Log: spike_history.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2000/03/08 20:03:07  mhucka
** Updated version from Antwerp group, sent by Hugo Cornelis.
** Reportedly fixes a memory leak and improves accuracy.
**
** Revision 1.1  1997/05/29 08:45:29  dhb
** Initial revision
**
*/

#include "out_ext.h"
#include "out_defs.h"

void SpikeHistory(output,action)

struct spikehistory_type *output;
Action		      *action;
{
MsgIn           *msg;
FILE            *fp;
int             index;

    switch(action->type){ /* the RESET, DELETE, SAVE and FLUSH actions are
			  ** copied from AscFileOutput in out_ascfile.c
                          ** the EVENT action is based on synchan.c
                          */
    case RESET: 
	/*
	** set the initialization flag
	*/
	if(!output->append){
	    output->initialize = TRUE;
	}
	break;
    case EVENT:	
        /* 
         * This action is executed when a spike occurs at any of the spikegen 
         * elements which are presynaptic to the spikehistory element.
         */
        /*
         ** has the file been initialized?
	 */
         if(output->initialize){
            AscSetupFile(output);
	 };
        /*
         ** open the file and append data to it
         */
         if(OpenOutputFile(output,"a")){
            fp = output->fp;
            index = 0;
            MSGLOOP(output,msg) 
               {
               case SPIKESAVE :
                 index++;
                 if (msg == (MsgIn*) action->data){ 
	             if (!(output->ident_toggle)) 
                           fprintf (fp, "%6d", index);
                     else  fprintf (fp, "%s", Pathname(msg->src));
                     fprintf (fp, "     %.6f\n", SimulationTime());
	         };
                 break;
               };
            if(output->flush)
	       fflush(fp);
            ConditionalCloseOutputFile(output);
         };
         break;
    case SAVE:
	CloseOutputFile(output);  /* in out_ascfile.c */
	break;
/*    case FLUSH:
	if (output->is_open)
	    fflush(output->fp);
	break;

    case DECODE: ** this action could ideally be called from the genesis command 
		 ** line with the output piped to an ascii file
                 **
         index = 0;
         MSGLOOP(output,msg)
          {
           case SPIKESAVE :
             index++;
             printf ("%d    %s\n", index, Pathname(msg->src));
             break;
          };
         break;	   
*/
    default:
	InvalidAction("FileOutput",output,action);
	break;
    }
}



