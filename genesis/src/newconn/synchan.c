static char rcsid[] = "$Id: synchan.c,v 1.4 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: synchan.c,v $
** Revision 1.4  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/06/29 17:20:50  svitak
** Initializations of some variables to quiet compiler.
**
** Revision 1.2  2005/06/27 19:00:44  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.14  2001/03/17 20:33:11  mhucka
** Fix from Hugo Cornelis for problem involving calculation of synchans.
**
** Revision 1.13  2000/04/27 07:30:11  mhucka
** Added fix from Mike Vanier for synchan objects involving SET and SHOW
** operations.  The problem was that they failed to trap attempts to set
** fields in nonexistent synapses.
**
** Revision 1.12  1999/11/29 07:43:36  mhucka
** Renamed synchan2.c to synchan.c.
**
** Revision 1.12  1997/08/07 17:12:00  dhb
** Added check for hsolve processing enabled in EVENT action.  Was
** adjusting first pending event time even in the non-hsolve case.
**
** Revision 1.11  1997/07/22 01:29:58  dhb
** Bug in ADDMSGIN action; was treating all incoming messages as
** SPIKEs.
**
** Revision 1.10  1997/06/03 06:02:18  dhb
** Fixed incorrectly named variable in ADDMSGIN action handler for the
** incoming spike message pointer
**
** Revision 1.9  1997/05/29 07:42:54  dhb
** Merged in 1.7.1.1 changes
**
** Revision 1.8  1997/05/29 07:18:08  dhb
** Added code to adjust message data pointers for outbound messages
** which reference synapse data in cases where synapses are reallocated
** or shifted in memory.  Also delete outbound messages which reference
** a synapse which is being deleted.
**
** Also removed old code which updated the synapse's reference to its
** associated SPIKE message on every add or delete of a message, as
** messages no longer move in memory during their lifetime.
**
** Revision 1.7.1.1  1997/05/29 07:38:52  dhb
** Update from Antwerp GENESIS version 21e
**
 * Version EDS20l5 95/12/19, Erik De Schutter, BBF-UIA 6/94-1/97
 * Added communication with hines solver through HPROCESS and HSEVENT calls
 * Assumed that number of synaptic inputs does not change (no code for
 *  changes in PendingSynapticEvents other than EVENT msg) 
 *
 * Modified MAEX 96/4/16, Reinoud Maex, BBF-UIA
 * When field normalize_weights of a synchan element is set to zero,
 * nothing changes.  Otherwise, channel->norm is divided by
 * channel->nsynapses in RECALC.
 *
 * Revision 1.7  1996/07/25  20:00:36  dhb
 * Changed SAVE2/RESTORE2 actions to save current state of pending
 * events.
 *
 * Revision 1.6  1996/07/22  22:41:44  dhb
 * Merged in 1.4.1.1 changes
 *
 * Revision 1.5  1995/09/27  23:58:18  venkat
 * Fixed MOD message and normalization bug for frequency.
 * Fixed bug causing a core dump on RESET in some cases.
 *
 * Revision 1.4.1.1  1996/07/22  22:36:26  dhb
 * Changes from Mike Vanier:
 *   Synchan2 is now Synchan (e.g. all synchans allow multiple
 *     spikes in transit); all synchan objects are handled in
 *     a generic manner
 *
 * Revision 1.4  1995/07/29  00:35:15  venkat
 * Removed warning message
 *
 * Revision 1.3  1995/03/28  05:30:54  mvanier
 * Made Synchan2 always return a value.
 *
 * Revision 1.2  1995/03/23  15:32:48  dhb
 * Added number of values to SAVE2 file format
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "newconn_ext.h"
#include "fac_ext.h"

static int      DEBUG_Synchan = 0;
static double   savedata[3];  /* FIXME! */


/* 11/94 Mike Vanier */

/*
 * Generalized second order synaptic conductance transformation.
 * Implements alpha function/dual exponent conductance
 * waveforms for impulse spike input.
 * Calculates channel current therefore needs membrane state.
 *
 * This object is based on code by Matt Wilson and Dave Bilitch.
 * I would also like to acknowledge the input of Erik de Schutter,
 * Dave Bilitch and Nigel Goddard in the implementation of this object.
 */

int Synchan( channel, action )
     register struct Synchan_type *channel;
     Action  *action;
{
  double  x;
  double  dt;
  double  eventtime;
  int     nvoltage_msgs_in;
  float   tpeak;
  MsgIn   *spikemsg;
  MsgIn   *msg;
  double  V = 0.0;
  int     i, time, allocnodes, num_nodes, syn;
  short   synindex;
  SynapticEvent *event, *next_event, *pending_event, 
         *last_event, *firstnode, *lastnode;
  int	  n;
  int     nr;
  unsigned short mod; /* flag for neuromodulation  */
  double  mod_level = 1.0;  /* degree of neuromodulation */
  int     nothsolve=1;


  if (Debug( DEBUG_Synchan ) > 1)
    ActionHeader("Synchan",channel,action);
    
  SELECT_ACTION( action )
    {
    case CREATE:

      channel->synapse_size = (unsigned short) ( sizeof( Synapse ) );
      channel->synapse = NULL;  /* no synapses to start with */

      break;


    case INIT:

      channel->activation = 0;

      break;

      
    case EVENT:

      /* 
       * This action is executed when a spike occurs at any of the spikegen 
       * elements which are presynaptic to the synchan.
       */

      spikemsg = (MsgIn*) action->data;

      for ( i = 0; i < channel->nsynapses; i++ )
	{
	  /* 
	   * Scan array of spike messages in synapse array, looking for 
	   * the one whose msgin field (mi) is the same as the action message.
	   */
	  
	  if ( channel->synapse[i].mi == spikemsg )
	    {
	      channel->synapse[i].last_spike_time = SimulationTime() + channel->synapse[i].delay;

	      ++channel->pending_events;
	      
	      eventtime = *(double*)action->argv;
	      dt = Clockrate( channel );
	      
	      /*
	       * Calculate the time until the spike hits in time steps.  The
	       * 0.9 is to make sure the time is rounded up to the nearest step.
	       * The eventtime is normally the same as SimulationTime() so
	       * (SimulationTime() - eventtime) is normally zero; however, 
	       * parallel implementations of this code may bunch several events
	       * together and send them at a later time step if they can, 
	       * so in that case eventtime may not be the same as SimulationTime().
	       */
	      
	      time = (int) ( ( channel->synapse[i].delay -
			       ( SimulationTime() - eventtime ) ) / dt + 0.9 );
	      
	      /*
	       * Grab a node from the FreeSynapticEvents list and record the
	       * synapse number of this event in the node.  Scan through the
	       * PendingSynapticEvents list, figure out where this node 
	       * goes and link it in to the list.  Decrement the time variable
	       * by the time fields of each event before it so the time field
	       * will be the number of time steps relative to the last event.
	       * If the freelist is empty allocate more nodes.
	       */
	      
	      if ( channel->FreeSynapticEvents == NULL )
		{
		  /* 
		   * Allocate more nodes in chunks of 
		   * (synapses + (nodes_per_synapse/2) + 1)
		   */
		  
		  allocnodes = (int) ( channel->nsynapses * 
				       channel->nodes_per_synapse / 2.0 ) + 1;

		  if ( AllocateSynapticEventNodes( channel, allocnodes, 
						   &(channel->FreeSynapticEvents ), &lastnode ) == 0 )
		    return ( 0 );
		}

	      /* Grab a new node and set its syn_num field. */
	      
	      event = channel->FreeSynapticEvents;
	      channel->FreeSynapticEvents = event->next; 
	      event->syn_num = i;
	      
	      /* Look for the position of the new event in the pending list. */
	      
	      if ( channel->PendingSynapticEvents == NULL )
		/* The new event is the only one. */
		{
		  event->time = time;
		  event->next = NULL;
		  channel->PendingSynapticEvents = event;
		  if (IsHsolved(channel))
				h_eventaction(channel,channel->hsolve,channel->solve_index,time);
		}
	      else
		{
		  last_event = NULL;

		  pending_event = channel->PendingSynapticEvents;
		  if (IsHsolved(channel))
		      pending_event->time -= (int)((SimulationTime() - channel->time_last_event)/dt);
		  for ( ; 
		       pending_event != NULL; 
		       pending_event = pending_event->next )
		    {
		      if (time <= pending_event->time)
			{	    
			  /* 
			   * Insert the new event in the pending list. 
			   * Also adjust the time field for the event after the
			   * new event.
			   */
			  
			  if ( last_event == NULL )
			    /* The new event is the first event. */
			    {
			      channel->PendingSynapticEvents = event;
				/* new firstevent -> update hsolve time counter if present*/
                    if (IsHsolved(channel))
                        h_eventaction(channel,channel->hsolve,channel->solve_index,time);

			    }
			  else
			    last_event->next = event;
			  
			  event->time = time;
			  event->next = pending_event;
			  pending_event->time -= time;
			  break;
			}
		      else
			{
			  time -= pending_event->time;
			  last_event = pending_event;
			}
		    }	
		  
		  if ( pending_event == NULL )
		    /* The new event is the last event. */
		    {
		      last_event->next = event; 
		      event->time = time;
		      event->next = NULL;
		    }
		}

	      //- time_last_event needs to include the shortest delay
	      //- of all the synapses, but be larger than the current
	      //- simulation time.

	      channel->time_last_event=SimulationTime();
	    }
	}
      
      break;  /* EVENT */ 


    case HPROCESS:
        /* set flag: we will execute only part of the PROCESS cycle */
        nothsolve=0;
        /* hsolve determined that firstevent must be executed now */
        next_event = channel->PendingSynapticEvents; /* top of pending events */
		if (next_event) next_event->time=0;
        /* clear activation field */
        channel->activation=0;

    /*FALLTHROUGH*/

    case PROCESS:
      /*
       * Check the next event due to be simulated; if its time field = 0 
       * then the spike hits this time step.  In this case add the weight 
       * value to the activation (to be normalized later).
       * Since the next event after that could hit at the same time, check 
       * the time field for the next event.  If it's also zero then add its
       * weight to the activation, and so on.  If the first event didn't 
       * hit this time step then decrement its time field.
       */

      dt = Clockrate( channel );
      next_event = channel->PendingSynapticEvents; /* top of pending event list */
      mod = 0;   /* default is no neuromodulation */
      
      if ( next_event != NULL ) /* skip if no events are pending */
	{
	  while ( ( next_event != NULL ) && ( next_event->time == 0 ) ) /* execute the event */
	    {
	      --channel->pending_events;
	      syn = next_event->syn_num;
	      last_event = next_event;
	      channel->PendingSynapticEvents = next_event = next_event->next;
	      
	      /* Put the event back onto the free list. */
	      
	      last_event->next = channel->FreeSynapticEvents;
	      channel->FreeSynapticEvents = last_event;
	      
	      /* normalize to step size by dividing by dt */
	      
	      channel->activation += channel->synapse[syn].weight / dt;
	    }

	  if ( next_event != NULL) {	  
	    if (nothsolve) {
	      --next_event->time;
	    } else {
	      channel->time_last_event=SimulationTime();
	    }
	  }
	}
      

	if (nothsolve) {
      
      MSGLOOP(channel,msg)
	{
	case VOLTAGE:                 /* membrane potential */

	  V = MSGVALUE( msg, 0 );

	  break;


	case ACTIVATION:              /* direct channel activation */

	  /* 
	   * This is assumed to be activation by graded transmission, 
	   * so proper normalization does not require dividing by dt 
	   * (which is necessary for spike input).
	   */

	  channel->activation += MSGVALUE( msg, 0 );

	  break;


	case RAND_ACTIVATION:         /* random channel activation */

	  /* 
	   * This is assumed to be by spikes, so normalize by 
	   * dividing by dt.
	   *
	   * 0 = probability (1/time)   1 = amplitude
	   */

	  if ( urandom() <= MSGVALUE( msg, 0 ) * dt )
	    channel->activation += MSGVALUE( msg, 1 ) / dt;

	  break;


	case MOD:                     /* Neuromodulator input */

	  /*
	   * Note that this globally affects all the synapses in the
	   * synchan.  If you want synchans where some synapses are
	   * modifiable but not others you should divide it into two
	   * synchans and only have a MOD message on the modifiable one.
	   */

	  if ( mod == 0 ) /* this is the first MOD message */
	    {
	      mod = 1;
	      mod_level = MSGVALUE( msg, 0 );
	    }
	  else /* this is not the first MOD message */
	    mod_level *= MSGVALUE( msg, 0 );

	  break;

	}
      
      /* 
       * 'endogenous' firing frequency: from channelC3;
       * don't invoke this unless the frequency is nonzero
       * since computing a random number takes time.
       */

      if ( channel->frequency > TINY ) 
	{
	  if ( urandom() <= ( channel->frequency * dt ) )
	    channel->activation += 1.0 / dt;
	}

      if ( mod ) /* there was neuromodulation */
	channel->activation *= mod_level;
	      
      /*
       * calculate the activation level of the channel
       * dx/dt = (activation*tau1 - x)/tau1
       * x = activation*tau1*(1-exp(-dt/tau1)) + x*exp(-dt/tau1))
       * or
       * x = activation*xconst1 + x*xconst2
       */
      
      x = channel->activation * channel->xconst1 +
	channel->X * channel->xconst2;
      
      /*
       * calculate the conductance of the channel
       * dy/dt = (x*tau2 - y)/tau2
       * y = x*tau2*(1-exp(-dt/tau2)) + y*exp(-dt/tau2))
       * or
       * y = x*yconst1 + y*yconst2
       */
      
      channel->X  = x;
      channel->Y = channel->X * channel->yconst1 +
	channel->Y * channel->yconst2;

      /*
       * set the new values of the state variables
       */
      
      channel->Gk = channel->Y * channel->norm;
      
      /*
       * calculate the channel current based on the channel conductance
       * the ion equilibrium potential and the membrane potential
       */
      
      channel->Ik = ( channel->Ek - V) * channel->Gk;
	} /* end nothsolve */

      break; /* PROCESS */

      
    case RESET:

      channel->activation = 0;
      channel->Gk  = 0;
      channel->X   = 0;
      channel->Y   = 0;
      
      /* 
       * If the free list has not been allocated, allocate it based on the
       * nodes_per_synapse field.
       */
      
      allocnodes = (int) ( channel->nsynapses * channel->nodes_per_synapse);

      if ( allocnodes != 0 )  
	{
	  if ( !channel->list_alloced )
	    {
	      if ( AllocateSynapticEventNodes( channel, allocnodes, 
						 &(channel->FreeSynapticEvents), &lastnode ) == 0 )
		return( 0 );
	      
	      channel->PendingSynapticEvents = NULL;
	      channel->list_alloced = 1;
	    }
	  else
	    {
	      /* 
	       * If it has, move all pending events back on to the free list. 
	       * Note that the time and syn_num contents of the free list nodes
	       * are meaningless.
	       */
	      
	      RemovePendingSynapticEvents( channel );
	    }
	}

      {
	/* double dRefract = - ((struct Spikegen_type *)spikemsg->src)->abs_refract; */

	  int i;

	  for (i = 0 ; i < channel->nsynapses ; i++)
	  {
	      channel->synapse[i].last_spike_time = -1.0;
	      channel->synapse[i].lastupdate = -1.0;

	      channel->synapse[i].Aplus = 0.0;

	      channel->synapse[i].Aminus = 0.0;
	  }
      }

      /* RECALC is automatically done on a RESET */
      
      /*FALLTHROUGH*/


    case RECALC:

      dt = Clockrate( channel );
      channel->xconst1 = channel->tau1 * ( 1.0 - exp( -dt / channel->tau1 ) );
      channel->xconst2 = exp( -dt / channel->tau1 );
      channel->yconst1 = channel->tau2 * ( 1.0 - exp( -dt / channel->tau2 ) );
      channel->yconst2 = exp( -dt / channel->tau2 );

      if ( channel->tau1 == channel->tau2 )
	{
	  channel->norm = channel->gmax * M_E / channel->tau1;
	} 
      else 
	{
	  tpeak = channel->tau1 * channel->tau2 *
	    log(channel->tau1/channel->tau2)/
	      (channel->tau1 - channel->tau2);
	  channel->norm = channel->gmax * (channel->tau1 - channel->tau2)/
	    (channel->tau1 * channel->tau2 *
	     (exp(-tpeak/channel->tau1) - exp(-tpeak/channel->tau2)));
	}
        /* NORMALIZATION, MAEX 96/4/16  */
   	if ((channel->normalize_weights != 0)    && (channel->nsynapses > 0)) 
           channel->norm = channel->norm / channel->nsynapses;
      break; /* RECALC */


    case RESETBUFFER:

      /* 
       * Shrinks the event buffer down to its original size as specified by 
       * the nodes_per_synapse field.
       */
      
      if ( !channel->list_alloced )
	break;  /* This is only meaningful if the lists exist. */

      /* 
       * If there are events on the pending list, put all 
       * pending event nodes back onto the free list. 
       */
      
      RemovePendingSynapticEvents( channel );

      allocnodes = (int) ( channel->nsynapses * channel->nodes_per_synapse );

      if ( allocnodes == channel->event_buffer_size )
	break;  /* The buffer is already the right size. */

      if ( allocnodes > channel->event_buffer_size )
	/* need to allocate more nodes and link them in to the free list */
	{
	  num_nodes = allocnodes - channel->event_buffer_size;

	  if ( AllocateSynapticEventNodes( channel, num_nodes, &firstnode, &lastnode ) == 0 )
	    return ( 0 );
	  
	  lastnode->next = channel->FreeSynapticEvents;
	  channel->FreeSynapticEvents = firstnode;

	  break;
	}
      
      /*
       * Otherwise free the event lists and allocate the list based on 
       * the nodes_per_synapse field. 
       */

      FreeSynapticEventLists( channel ); 

      if ( AllocateSynapticEventNodes( channel, allocnodes, 
				     &(channel->FreeSynapticEvents), &lastnode ) == 0 )
	return ( 0 );

      channel->PendingSynapticEvents = NULL;
      channel->list_alloced = 1;
      
      break; /* RESETBUFFER */


    case CHECK:

      nvoltage_msgs_in = 0;
      
      MSGLOOP( channel, msg )
	{

	case VOLTAGE: 

	  ++nvoltage_msgs_in;

	  break;

	}

      if ( nvoltage_msgs_in == 0 )
	ErrorMessage( "Synchan","No membrane potential.", channel );

      /*
       * Check that there is only one VOLTAGE message coming in.
       */
      
      if ( nvoltage_msgs_in > 1 )
	ErrorMessage( "Synchan", "Too many VOLTAGE messages.", channel );

      if ( ( channel->tau1 <= 0 ) || ( channel->tau2 <= 0 ) )
	ErrorMessage( "Synchan", "taus must be > 0.", channel );
      
      if ( channel->gmax < 0 )
	ErrorMessage( "Synchan", "gmax must be >= 0.", channel );
      
      if ( channel->frequency < 0 )
	ErrorMessage( "Synchan", "frequency must be >= 0.", channel );

      if ( channel->nodes_per_synapse <= 0 )
	ErrorMessage( "Synchan", "nodes_per_synapse must be > 0.", channel );
      
      break; /* CHECK */


    case ADDMSGIN:

      /* 
       * When a SPIKE message is added, the fields of the 
       * other messages have to be adjusted to take this into 
       * account.  If a different message is added do nothing.
       */
      
      spikemsg = (MsgIn *) action->data;
      if ( spikemsg->type != SPIKE ) 
	return ( 1 );

      channel->nsynapses++;

      if ( channel->nsynapses == 0 )
	return ( 1 );
      
      /* Allocate or reallocate space for the synapse buffer. */
      
      if ( channel->synapse == NULL )
	{
	  channel->synapse = (Synapse *) calloc( channel->nsynapses, sizeof( Synapse ) );
	}
      else
	{
	  char *old_synapse_buffer = (char *) channel->synapse;
	  
	  channel->synapse = (Synapse *) realloc( channel->synapse,
						  channel->nsynapses * sizeof( Synapse ) );
	  
	  /*
	   * At this point, since the synapse fields have moved, we have to
	   * remap the message data pointers that point to locations inside
	   * the synapse buffer.
	   */
	  
	  RemapMsgData( channel, old_synapse_buffer,
			( channel->nsynapses * sizeof( Synapse ) ),
			(char *) (channel->synapse) );
	}
	  

      i = channel->nsynapses - 1;
      
      channel->synapse[i].mi = spikemsg;

      /* set up defaults for new message */
      channel->synapse[i].weight =  1;
      channel->synapse[i].delay  =  0;
      channel->synapse[i].last_spike_time  =  - ((struct Spikegen_type *)spikemsg->src)->abs_refract;
      channel->synapse[i].Aplus = 0.0;
      channel->synapse[i].Aminus = 0.0;
      channel->synapse[i].lastupdate = -1.0;

      break; /* ADDMSGIN */


    case DELETEMSGIN:

      /* 
       * When a SPIKE message is deleted, the fields of the other messages 
       * have to be adjusted to take this into account.  If a different message 
       * is deleted do nothing.  Note that the synapse array is not freed in 
       * anticipation of possible future message additions.  Note also that 
       * MSGINDELETED gets called later to make sure the mi pointers of the 
       * synapses are pointing to the right messages.
       */

      if ( channel->synapse == NULL ) /* There is no synapse array. */
	return ( 1 );
      
      spikemsg = (MsgIn *) action->data;
      if ( spikemsg->type != SPIKE ) /* Not a SPIKE message. */
	return ( 1 );
      
      if ( channel->nsynapses == 1 )  /* There was only one SPIKE message. */
	{
	  channel->nsynapses--;
	  FreeSynapticEventLists( channel ); 
	  return ( 1 );
	}
      
      /* 
       * This copies the synapse above the deleted one to the space occupied by 
       * the deleted one (direct struct copy).  This is done for all synapses 
       * above the deleted one.  The "missing memory" is not freed, but is available 
       * when new messages are added due to the realloc() in the ADDMSGIN code.  
       * Note that the mi pointers are incorrect at this point; they get set correctly 
       * in the MSGINDELETED action (see below).
       */

      for ( i = 0, synindex = -1; i < channel->nsynapses; i++ )
	{
	  if ( spikemsg != NULL ) 
	    {
	      if ( channel->synapse[i].mi == spikemsg )
		{
		  spikemsg = NULL;    /* found the message so don't repeat this */
		  synindex = i;
		}
	    }
	  else 
	    channel->synapse[i-1] = channel->synapse[i];
	}

      if ( synindex < 0 )
	{
	  ErrorMessage("Synchan", "Could not find SPIKE message to delete.", channel);
	  return ( 0 );
	}

	/*
	 * If synapses have moved in memory (which will be the case
	 * unless the synapse being deleted is the last one) we need
	 * to remap references to synapse fields which might exist
	 * in outbound messages.
	 *
	 * We also have to delete outbound messages which refer to the
	 * deleted synapse.  These *must* be deleted first so that we
	 * don't inadvertently delete messages which have just been
	 * remapped to the memory of the deleted synapse, which we
	 * are reusing.
	 */

      DeleteMsgReferringToData( channel, (char*) &channel->synapse[synindex],
			sizeof( Synapse ) );

	 if (synindex != channel->nsynapses - 1)
	   {
	     RemapMsgData( channel,
	       (char*) &channel->synapse[synindex+1],
	       ( channel->nsynapses - synindex - 1 ) * sizeof( Synapse ),
	       (char*) &channel->synapse[synindex] );
	   }


      channel->nsynapses--;

      /*
       * Now scan the list of pending events and remove all of those
       * corresponding to the synapse that has been deleted.  Then adjust the
       * time fields and next pointers of the adjacent events.  Also decrement
       * the syn_num of any event whose syn_num is greater than synindex.
       */      

      last_event = NULL;

      for ( event = channel->PendingSynapticEvents; event != NULL; event = next_event )
	{
          next_event = event->next;

	  if ( event->syn_num == synindex )
	    {
	      /* Put the node onto the free list. */
	      
	      event->next = channel->FreeSynapticEvents;
	      channel->FreeSynapticEvents = event;

	      if ( last_event == NULL )
		channel->PendingSynapticEvents = next_event;
	      else
		last_event->next = next_event;

	      if ( next_event != NULL )
		next_event->time += event->time;
	    }
	  else
	    {
	      if ( event->syn_num > ( (unsigned short) synindex ) )
		--event->syn_num;
	      last_event = event;
	    }
	}
      
      break; /* DELETEMSGIN */


    case DELETE:

      FreeSynapticEventLists( channel ); 

      if ( channel->synapse != NULL )
	{
	  free( channel->synapse );
	  channel->synapse   = NULL;
	  channel->nsynapses = 0;
	}

      break; /* DELETE */


    case COPY:

      if ( channel->synapse != NULL )
	{
	  Error();
	  printf( "synchan: COPY: cannot copy synchans with SPIKE messages in current implementation.\n" );
	  printf( "           Please add SPIKE messages after copying synchan objects, not before!\n" );

	  return ( 0 );
	}

      break; /* COPY */


    case SAVE2:

      /*
       * Writing Gk, X, Y and pending event data.
       */

      savedata[0] = channel->Gk;
      savedata[1] = channel->X;
      savedata[2] = channel->Y;

      n = 3 + SaveSizeSynapticEvents(channel);

      /* synchan computed fields */
      fwrite(&n,sizeof(int),1,(FILE*)action->data);
      fwrite(savedata,sizeof(double),3,(FILE*)action->data);

      SaveSynapticEvents(channel, (FILE*)action->data);
      break; /* SAVE2 */


    case RESTORE2:

      n = 0;
      if (fread(&n,sizeof(int),1,(FILE*)action->data) != 1 || n < 4)
	{
	  ErrorMessage("Synchan","Invalid savedata length",channel);
	  return ( n );
	}

      nr = fread(savedata,sizeof(double),3,(FILE*)action->data);
      if (nr != 3)
	{
	  ErrorMessage("Synchan","Invalid savedata",channel);
	  return ( n - nr );
	}

      channel->Gk = savedata[0];
      channel->X = savedata[1];
      channel->Y = savedata[2];
      n -= 3;

      if (RestoreSynapticEvents(channel, &n, (FILE*)action->data) == 0)
	  return ( n );

      return ( 0 ); /* RESTORE2 */


    case SET:
      return Synchan_SET("synchan", channel, action);
      /*NOTREACHED*/
      break;


    case SHOW:
      return Synchan_SHOW("synchan", channel, action);
      /*NOTREACHED*/
      break;

    }

    return ( 1 );
}


/*
 * This command executes RESETBUFFER on all synchan objects or derived objects.
 */

void ResetSynchanBuffers()
{
  Element       *element;
  ElementStack  *stk;
  Action        *action;
 
    action = GetAction( "RESETBUFFER" );
 
    stk = NewPutElementStack( RootElement() );
    while ( ( element = NewFastNextElement( 1, stk ) ) != NULL )
      {
	if ( CheckClass( element, ClassID( "synchannel" ) ) )
	  if ( !IsHsolved( element ) ) 
	    CallActionFunc( element, action );		  
      }

    NewFreeElementStack( stk );
}

