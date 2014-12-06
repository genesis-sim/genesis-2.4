/* SynS_ron.c modified from synchan.c */

static char rcsid[] = "$Id: SynS.c,v 1.1 2006/01/09 16:35:29 svitak Exp $";
#define VOLTAGE 0

#include <math.h>
#include "SynGS_ext.h" 
static int      DEBUG_Synchan = 0;
static double         savedata[2];

/*
 * This object calculates the voltage dependent modulation of spike mediated 
 * transmission in the leech HN cells.  Therefore, it needs the presynaptic 
 * membrane potential.
 */

int SynS_f(channel,action)
register struct SynS_type *channel;
Action  *action;
{
double  dt;
int     has_state;
MsgIn   *spikemsg;
MsgIn   *msg;
double  V, modinf;
double modtau =0.2;
int	n;
  
   if(Debug(DEBUG_Synchan) > 1){
      ActionHeader("SynS",channel,action);
      }
   SELECT_ACTION(action){
   
   case INIT:
      break;
 
   case PROCESS:
      MSGLOOP(channel,msg)
         {
         case VOLTAGE:                 /* membrane potential */
            V = MSGVALUE(msg,0);
            break;
	 } 

         dt = Clockrate(channel); /* Sec */
	 /* update modulation factor using exponetial euler
          * 
	  */      
       
	 modinf = channel->A+channel->B/(1+exp(channel->C*(V+channel->D))); 
	 /* e.g. modinf = 0.1+0.9/(1+exp(-1000*(V+0.040))); */
	 modtau = channel->E; 
         /* exponetial euler */
         channel->m_SynS=(channel->m_SynS)*exp(-dt/modtau)+ \
            modinf*(1.0-exp(-dt/modtau)) ;
         /*printf ("Voltage %f dt %f m_SynS %f \n", V, dt, channel->m_SynS);*/
         break;     

   case CHECK:
      has_state = 0;

      MSGLOOP(channel,msg)
         {
	 case VOLTAGE: 
	    has_state = 1;
	    break;
	 }  

         if (has_state == 0)
	    ErrorMessage("SynS","No membrane potential.", channel);
	   
      break; /* CHECK */  

   case RESET:
      channel->m_SynS = 0; 
      break;
    
   case SAVE2:
      savedata[0]=(double)(channel->m_SynS);
      n=1;
      fwrite(&n,sizeof(int),1,(FILE*)action->data);
      fwrite(savedata,sizeof(double),1,(FILE*)action->data);
      break;
    
   case RESTORE2:
      fread(&n,sizeof(int),1,(FILE*)action->data);
      if (n != 1) {
      ErrorMessage("SynS_ron","Invalid savedata length",channel);
      return(n);
      }
      fread(savedata,sizeof(double),1,(FILE*)action->data);
      channel->m_SynS =(float)savedata[0];
      break;
      }

    return(0);
}






