static char rcsid[] = "$Id: SynE.c,v 1.1 2006/01/09 16:35:29 svitak Exp $";
#define VOLTAGE 0
#define POSTVOLTAGE 1
#define EK 4

#include <math.h>
#include "SynGS_ext.h" 
static int      DEBUG_SynE = 0;
static double   savedata[3];

/*
 * Electrical synaptic transmission designed specifically for leech HN cells.
 * Needs presynaptic and postsynaptic membrane potential as input.  The output
 * is the synaptic current that is added to the postsynaptic compartment via 
 * an inject message.  In addition, the pre- and postsynaptic membrane
 * potentials can be lowpass filtered with an RC circuit.  The time contants 
 * can be set for both potentials. The synapse is unidirectional, that is, the
 * current is injected into only the postsynaptic cell.  To make a 
 * bidirectional synapse, another element must be created.  The synapse can be
 * configured to be non-rectifying (passes both positive and negative current)  * or retifying (passes only either positive or negative current). 
 * This is done by setting the rectify field to 0 for non-retifying, 1 for 
 * passes positve current, or -1 for passes negative current.
 *
 * This object was modified from synchan.c by J. Lu and A. Hill
 */

int SynE(channel,action)
   register struct SynE_type *channel;
   Action  *action;
{
   double  dt;
   int volt_msg_in = 1,pvolt_msg_in =1; /*CHECK*/
   MsgIn   *msg;
   double  PREVM, POSTVM;
   int	n;
  
   if(Debug(DEBUG_SynE) > 1)
      ActionHeader("SynE",channel,action);
    
   SELECT_ACTION(action)
   {
   case INIT:
      channel->Ik = 0;
      break;
 
   case PROCESS:
      MSGLOOP(channel,msg)
         {
         case VOLTAGE:           /* get presynaptic membrane potential */
            PREVM = MSGVALUE(msg,0);
	    break;

         case POSTVOLTAGE:       /* get postsynaptic membrane potential */
            POSTVM = MSGVALUE(msg,0);
            break;

	 }  

      /* calculate state variables and conductance */ 
      dt = Clockrate(channel);

      /* exponential euler; TauPre units =sec */
      /* Vpre = filtered presynaptic Vm */
      channel->Vpre=(channel->Vpre)*exp(-dt/channel->TauPre)+ \
      PREVM*(1-exp(-dt/channel->TauPre));

      /* exponential euler; TauPost units =sec */
      /* Vpost = filtered presynaptic Vm */
      channel->Vpost=(channel->Vpost)*exp(-dt/channel->TauPost)+ \
      POSTVM*(1-exp(-dt/channel->TauPost));
    
      /* calculate the transmembrane current using Vpre and Vpost */
      channel->Ik = (channel->Vpre - channel->Vpost) * channel->Gbar;

      /* rectify
       * 0 = nonrectifying 
       * 1 = passes positive
       * -1 = passes negative
       */
      if ((channel->rectify ==1) && (channel->Ik <0)) channel->Ik=0.0;
      if ((channel->rectify ==-1) && (channel->Ik >0)) channel->Ik=0.0;

      break;      /* end of case PROCESS */

   case RESET:
      channel->Gk  = channel->Gbar;
      channel->Ik  = 0;
      break;

   case CHECK:
      volt_msg_in = 0;
      pvolt_msg_in = 0;

      MSGLOOP(channel,msg)
         {
	 case VOLTAGE: 
	    volt_msg_in = 1;
	    break;
          
         case POSTVOLTAGE:
            pvolt_msg_in = 1;
            break;

	 }  

         if (volt_msg_in == 0)
	    ErrorMessage("SynG","No presynaptic membrane potential.", channel);
	   
         if (pvolt_msg_in == 0)
            ErrorMessage("SynG","No postsynaptic membrane potential.", channel);
        
         if ((channel->Gbar) < 0)
            ErrorMessage("SynG","Gbar must be >= 0.", channel);
        

      break; /* CHECK */  

case SAVE2:
      savedata[0] = (double)(channel->Vpre);
      savedata[1] = (double)(channel->Vpost);
      n=2;
      fwrite(&n,sizeof(int),1,(FILE*)action->data);
      fwrite(savedata,sizeof(double),2,(FILE*)action->data);
      break; /* SAVE2 */

case RESTORE2:
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 2) {
	    ErrorMessage("SynE_ron","Invalid savedata length",channel);
	    return(n);
	}
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	channel->Vpre = savedata[0];
	channel->Vpost = savedata[1];
	break;  /* RESTORE2 */

default:
	InvalidAction("SynE_ron", channel ,action);
	break;

 
   }
      
      return (0);
}















