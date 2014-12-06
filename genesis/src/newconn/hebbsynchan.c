static char rcsid[] = "$Id: hebbsynchan.c,v 1.4 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: hebbsynchan.c,v $
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
** Revision 1.2  2000/04/27 07:29:34  mhucka
** Added fix from Mike Vanier for synchan objects involving SET and SHOW
** operations.  The problem was that they failed to trap attempts to set
** fields in nonexistent synapses.
**
** Revision 1.1  1999/10/15 02:05:53  mhucka
** Added hebbsynchan.c, previously known as "hebbsynchan3.c".  I renamed it
** at the suggestion of Mike Vanier, who believes there is no reason to
** maintain the number "3" in the name.
**
** Revision 1.5  1997/10/03 00:00:20  dhb
** Fix to ADDMSGIN handler which was treating all messages as
** SPIKE messages (at least those following the first SPIKE messages)
** and which assigned the wrong value to the synapse.mi reference to
** the SPIKE message.  The later would prevent EVENTs from being placed
** in the pending event list.
**
** Revision 1.4  1997/05/29 07:28:08  dhb
** Added adjustment of outbounf message data pointer which reference
** synpase data in cases where synapses are reallocated or shifted
** in memory.  Also ensure that outbound messages which reference
** synapse data for a synapse which is being deleted are also deleted.
**
** Removed update of synapse pointers to associated inbound messages
** which was occuring when messages were added or deleted, as messages
** no longer move in memory during their lifetime.
**
 * Revision 1.3  1996/07/26  06:51:15  dhb
 * Changed SAVE2/RESTORE2 actions to save and restore synaptic
 * states (X, Y, pre_activity and weight), pending synaptic
 * events and the HebbSynchan avg_Vm.
 *
 * Revision 1.2  1996/07/23  19:35:58  dhb
 * Removed extraneous end of comment and added missing semicolon on
 * rcsid variable declaration line.
 *
 * Revision 1.1  1996/07/22  22:07:21  dhb
 * Initial revision
 *
**
** This code is the decendent of the original hebbsynchan2 code
** implementing multiple spikes in transit.
** 
** 8/95 Mike Vanier, Woods Hole, Mass. 
**
** Extensively revised by Mike Vanier 4/96.
*/

#include <math.h>
#include "fac_ext.h"
#include "newconn_ext.h"

static int      DEBUG_HebbSynchan = 0;
static double   savedata[4];

/* 
 * The following functions change the weights of all the synapses in
 * a particular hebbsynchan based on the presynaptic activity level
 * (reflected in the "pre_activity" field of the synapse) and the 
 * postsynaptic average Vm (the "avg_Vm" field of the hebbsynchan).
 * The presynaptic and postsynaptic "activities" are first calculated
 * based on those values and the low and high threshold values 
 * (presynaptic and postsynaptic).  These are then used to calculate
 * how much to change the weights by.  The use of two presynaptic and
 * two postsynaptic thresholds divides the activity space into nine
 * different zones.  By setting the two presynaptic thresholds
 * equal to each other and ditto for the postsynaptic thresholds
 * you can divide the space into four zones instead.
 *
 * If the weight change algorithm used here doesn't
 * agree with you, define (or add) your own function here instead.
 */


static void changeHebbWeights( channel, weight_change_mod, weight_change_mod_level )
     register struct HebbSynchan_type *channel;
     unsigned short weight_change_mod;
     double weight_change_mod_level;
{
  int    i;
  float  pre_activity, post_activity;
  float  delta, rate, change, max_change;
  float  min_weight, max_weight;
  float  norm_weight, scale;
  double dt = Clockrate( channel );


  min_weight  = channel->min_weight;
  max_weight  = channel->max_weight;
  rate        = channel->weight_change_rate;


  if ( weight_change_mod )
    rate *= weight_change_mod_level;
 
  /* 
   * Calculate the postsynaptic activity.  This will be the same
   * throughout the function.  It is just the difference between 
   * the avg_Vm and the nearest threshold, with the appropriate 
   * sign, unless the avg_Vm falls between the thresholds, in which 
   * case it's zero.  post_scale is a scaling factor with units of volts
   * which makes post_activity dimensionless.  You can think of it as
   * the voltage difference which gives rise to one unit of post_activity.
   */
 
  if ( channel->avg_Vm > channel->post_thresh_hi )
    post_activity = ( channel->avg_Vm - channel->post_thresh_hi ) / channel->post_scale;
  else if ( channel->avg_Vm < channel->post_thresh_lo )
    post_activity = ( channel->avg_Vm - channel->post_thresh_lo ) / channel->post_scale;
  else 
    post_activity = 0.0;

  /* 
   * Calculate the presynaptic activity and change the synaptic 
   * weight for each synapses.
   */
  
  for ( i = 0; i < channel->nsynapses; i++ ) 
    {
      /* 
       * Calculate the presynaptic activity, which is just the
       * difference between the (synapse's) pre_activity and the 
       * nearest threshold, unless the pre_activity falls between 
       * the thresholds, in which case it's zero.
       */
      
      if ( channel->synapse[i].pre_activity > channel->pre_thresh_hi )
	pre_activity = channel->synapse[i].pre_activity - channel->pre_thresh_hi;
      else if ( channel->synapse[i].pre_activity < channel->pre_thresh_lo )
	pre_activity = channel->synapse[i].pre_activity - channel->pre_thresh_lo;
      else 
	pre_activity = 0.0;

      /* 
       * Change the weight of the synapse based on the presynaptic
       * and postsynaptic activities.  You may wish to change this
       * to model your favorite weight change algorithm.
       */
      
      /*
       * 1) Don't change weights if either activity is zero or if 
       *    both activities are less than zero.  Otherwise calculate
       *    "delta" which is the product of the pre- and postsynaptic
       *    activity (which are both dimensionless).  Then calculate 
       *    "change" which is the absolute value of delta times rate, 
       *    scaled for the particular time step by multiplying by dt.  
       *    We use the absolute value since the sign is taken into account 
       *    below.  Since rate is in units of (1/sec) change is 
       *    dimensionless.
       */
 
      if ( ( post_activity == 0.0 ) || ( pre_activity == 0.0 ) || 
          ( ( pre_activity < 0.0 ) && ( post_activity < 0.0 ) ) ) 
        continue;   /* i.e. don't change weight of this synapse */
      
      delta  = pre_activity * post_activity;
      change = ( rate * dt ) * delta;

      /* 
       * 2) Prevent huge weight changes from taking place in one time step.  
       *    We somewhat artibitrarily limit the weight change in one time 
       *    step to 1/10 of the difference between the maximum and
       *    minimum weights.
       */
 
      max_change = ( channel->max_weight - channel->min_weight ) / 10.0;
 
      if ( fabs( change ) > max_change ) 
        {
          if ( change < 0.0 )
            change = -max_change;
          else
            change = max_change;
        }

      /* 
       * 3) Scale weight change according to distance to min_weight or 
       *    max_weight.  This gives a smooth approach to the limits.
       *    "norm_weight" is a "normalized" version of the synaptic
       *    weight which is in the range (0,1).  It is used to 
       *    calculate "scale", which scales the weight change.  For weight 
       *    increments, scale will approach 1 for weights far below max_weight 
       *    (i.e. norm_weight near 0) and will approach zero for weights close 
       *    to max_weight (i.e. norm_weight near 1).  For weight decrements, 
       *    scale will approach 1 for weights far above max_weight (i.e. 
       *    norm_weight near 1) and will approach 0 for weights close to 
       *    min_weight (i.e. norm_weight near 0).  I use the square root
       *    function for convenience; any power-law function with the power
       *    between 0 and 1 could be used.
       */
 
      norm_weight = ( channel->synapse[i].weight - min_weight ) /
                    ( max_weight - min_weight );

      if ( change < 0.0 )  /* weight decay */
	scale = sqrt( norm_weight );
      else                 /* weight increase */
	scale = sqrt( 1.0 - norm_weight );

      channel->synapse[i].weight += change * scale;

      /*
       * Make sure we're not over the limits.
       */
 
      if ( channel->synapse[i].weight > max_weight )
        channel->synapse[i].weight = max_weight;
      else if ( channel->synapse[i].weight < min_weight )
        channel->synapse[i].weight = min_weight;

    } 
}




/*
 * Generalized second order synaptic conductance transformation.
 * Implements alpha function/dual exponent conductance
 * waveforms for impulse spike input.
 * Calculates channel current therefore needs membrane state.
 *
 * This object also updates the weights as a function of the pre-
 * and postsynaptic activity in a Hebbian manner.
 */

int HebbSynchan( channel, action )
     register struct HebbSynchan_type *channel;
     Action  *action;
{
  double   x;
  double   dt;
  double   eventtime;
  int      nvoltage_msgs_in;
  float    tpeak;
  MsgIn   *spikemsg;
  MsgIn   *msg;
  double   V = 0.0;
  int      i, time, allocnodes, num_nodes, syn;
  short    synindex;
  SynapticEvent *event, *next_event, *pending_event, 
          *last_event, *firstnode, *lastnode;
  int	  n;
  int	  nr;
  unsigned short mod; /* flag for neuromodulation  */
  double  mod_level = 1.0;  /* degree of neuromodulation */

  /* HEBBIAN VARIABLES: */

  unsigned short weight_change_mod; /* flag for modulation of weight change rate  */
  double  weight_change_mod_level = 1.0;  /* degree of weight change modulation */
  double  pre_tpeak;


  if ( Debug( DEBUG_HebbSynchan ) > 1 )
    ActionHeader( "HebbSynchan", channel, action );

  SELECT_ACTION( action )
    {

    case CREATE:

      channel->synapse_size = (unsigned short) ( sizeof( HebbSynapse ) );
      channel->synapse = NULL; /* no synapses to start with */

      break;


    case INIT:

      channel->activation = 0;

      break;


    case EVENT:

      /* 
       * This action is executed when a spike occurs at any of the spikegen 
       * elements which are presynaptic to the synchan.  It is called before 
       * the PROCESS action as long as the spiking elements are simulated 
       * earlier in the schedule than the synchan is (which should be the case).
       */

      spikemsg = (MsgIn*) action->data; 

      for ( i = 0; i < channel->nsynapses; i++ )
	{
	  /* 
	   * Scan spike messages in synapse array, looking for the one whose 
	   * msgin field (mi) is the same as the action message.
	   */
	  
	  if ( channel->synapse[i].mi == spikemsg )
	    {
	      ++channel->pending_events;
	      
	      eventtime = *(double*) action->argv;
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

		  if ( AllocateSynapticEventNodes( (struct Synchan_type *) channel, allocnodes, 
						   &(channel->FreeSynapticEvents), &lastnode ) == 0 )
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
		}
	      else
		{
		  last_event = NULL;

		  for ( pending_event = channel->PendingSynapticEvents; 
		        pending_event != NULL; 
		        pending_event = pending_event->next )
		    {
		      if ( time <= pending_event->time )
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
	    }
	}
      
      break;  /* EVENT */ 


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
      
      dt                = Clockrate( channel );
      next_event        = channel->PendingSynapticEvents; /* top of pending event list */
      mod               = 0;        /* default is no neuromodulation                   */
      weight_change_mod = 0;        /* default is no modulation of weight change rate  */

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

	      /**************************************************************
	       *                    BEGIN HEBBIAN STUFF                     *    
	       **************************************************************/

	      SET_FLAG( SPIKE_THIS_TIME_STEP, channel->synapse[syn].spike_flags );

	      /**************************************************************
	       *                      END HEBBIAN STUFF                     *    
	       **************************************************************/
	      
	      /* normalize to step size by dividing by dt */
	      
	      channel->activation += channel->synapse[syn].weight / dt;
	    }
	  
	  if ( next_event != NULL )
	    --next_event->time;
	}
      
      
      MSGLOOP( channel, msg )
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


          case WEIGHT_CHANGE_MOD:       /* Modulation of weight_change_rate */
 
            /*
             * This varies the weight_change_rate of the hebbsynchan under control
             * of some external agent.
             */
 
            if ( weight_change_mod == 0 ) /* this is the first WEIGHT_CHANGE_MOD message */
              {
                weight_change_mod = 1;
                weight_change_mod_level = MSGVALUE( msg, 0 );
              }
            else /* this is not the first WEIGHT_CHANGE_MOD message */
              weight_change_mod_level *= MSGVALUE( msg, 0 );
 
            break;
 
	}
      
      /* 
       * 'endogenous' firing frequency: from channelC3;
       * don't invoke this unless the frequency is nonzero
       * since computing a random number takes time.
       */
      
      if ( channel->frequency > TINY ) 
	{
	  if ( urandom() <= channel->frequency * dt )
	    channel->activation += 1.0 / dt;
	}
      
      if ( mod ) /* there was neuromodulation */
	channel->activation *= mod_level;

      /*
       * Calculate the activation level of the channel.
       *
       * dx/dt = (activation*tau1 - x)/tau1
       * x = activation*tau1*(1-exp(-dt/tau1)) + x*exp(-dt/tau1))
       * or
       * x = activation*xconst1 + x*xconst2
       */
      
      x = channel->activation * channel->xconst1 +
	           channel->X * channel->xconst2;
      
      /*
       * Calculate the conductance of the channel.
       *
       * dy/dt = (x*tau2 - y)/tau2
       * y = x*tau2*(1-exp(-dt/tau2)) + y*exp(-dt/tau2))
       * or
       * y = x*yconst1 + y*yconst2
       */
      
      channel->Y = channel->X * channel->yconst1 +
	           channel->Y * channel->yconst2;

      /*
       * Set the new values of the state variables.
       */
      
      channel->X  = x;
      channel->Gk = channel->Y * channel->norm;
      
      /*
       * Calculate the channel current based on the channel conductance
       * the ion equilibrium potential and the membrane potential.
       */
      
      channel->Ik = ( channel->Ek - V ) * channel->Gk;

      /**************************************************************
       *                    BEGIN HEBBIAN STUFF                     *    
       **************************************************************/

      /* 
       * 1) Presynaptic activity calculations:	
       */

      /* 
       * Calculate the presynaptic X and Y values and pre_activity.
       * What we are doing here is using the spiking history of
       * the synapse as input to a function which generates
       * generalized alpha-function waveforms just like the 
       * channel itself does, but with different time constants.
       * This generalized alpha function will have a maximum value 
       * of 1.0 for a single spike input. Thus each synapse is like 
       * a mini-channel.  The idea is to set the time constants 
       * much slower than that of the real channel so that presynaptic 
       * "averaging" takes place over a longer time window. 
       * Note that this approach is rather similar to what really  
       * happens with an NMDA receptor, which has a slower time 
       * course than an AMPA receptor. 
       */

      for ( i = 0; i < channel->nsynapses; i++ )
	{
	  /*
	   * Calculate the value of activity variable X for the synapse.
	   * 
	   * dX/dt = (activation*tau1 - X)/tau1
	   * X(t+dt) = X(t) * exp(-dt/tau1) + { 1 if a spike hit this time step
	   *                                  { 0 if no spike hit
	   * or
	   *
	   * X(t+dt) = X(t) * pre_xconst + {0,1}
	   */

	  if ( IS_FLAG_SET( SPIKE_THIS_TIME_STEP, channel->synapse[i].spike_flags ) )
	    {
	      x = channel->synapse[i].X * channel->pre_xconst + 1.0;
	      CLEAR_FLAG( SPIKE_THIS_TIME_STEP, channel->synapse[i].spike_flags );
	    }
	  else
	    x = channel->synapse[i].X * channel->pre_xconst;
	  
	  /*
	   * Calculate the value of activity variable Y for the synapse.
	   *
	   * dY/dt = (X*tau2 - Y)/tau2
	   * Y(t+dt) = Y(t) * exp(-dt/tau2) + X(t) * exp(-dt/tau2) * b
	   *
	   * where b = { dt if tau1 == tau2
	   *           { 1/a * (1 - exp(-a dt) if tau1 != tau2, where a = 1/tau1 - 1/tau2
	   * or
	   *
	   * Y(t+dt) = Y(t) * yconst1 + X(t) * yconst2
	   */
	  
	  channel->synapse[i].Y = channel->synapse[i].Y * channel->pre_yconst1 +
	                          channel->synapse[i].X * channel->pre_yconst2;
	  
	  /*
	   * Set the new values of the state variables X and pre_activity.
	   */
	  
	  channel->synapse[i].X  = x;
	  channel->synapse[i].pre_activity = channel->synapse[i].Y * channel->pre_norm;
	}

      /*
       * 2) Postsynaptic activity calculations:
       */

      /*
       * Calculate the "averaged" postsynaptic Vm; in reality all 
       * we do is use the Vm as input to a leaky integrator with 
       * a time constant of post_tau.
       *
       * avg_Vm = (1 - exp(dt/post_tau)) * Vm + exp(dt/post_tau) * avg_Vm
       * or
       * avg_Vm = (1 - post_const)*Vm + post_const*avg_Vm
       *
       * Note that when time step is changed you must do a RECALC to
       * recalculate post_const; this is done automatically on a RESET.
       */
 
      channel->avg_Vm = ( 1.0 - channel->post_const ) * V +
	channel->post_const * channel->avg_Vm;
 
      /* 
       * 3) Weight updating:
       */

      if ( channel->change_weights )
	changeHebbWeights( channel, weight_change_mod, weight_change_mod_level );
      
      /**************************************************************
       *                    END HEBBIAN STUFF                       *    
       **************************************************************/

      break; /* PROCESS */

      
    case RESET:

      channel->activation = 0.0;
      channel->Gk         = 0.0;
      channel->X          = 0.0;
      channel->Y          = 0.0;

      /* 
       * If the free list has not been allocated, allocate it based on the
       * nodes_per_synapse field.
       */
      
      allocnodes = (int) ( channel->nsynapses * channel->nodes_per_synapse );

      if ( allocnodes != 0 )  
        {
          if ( !channel->list_alloced )
            {
              if ( AllocateSynapticEventNodes( (struct Synchan_type *) channel, allocnodes, 
                                               &(channel->FreeSynapticEvents), &lastnode ) == 0 )
                return ( 0 );
              
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
              
              RemovePendingSynapticEvents( (struct Synchan_type *) channel );
            }
        }
 
      /**************************************************************
       *                    BEGIN HEBBIAN STUFF                     *    
       **************************************************************/

      dt = Clockrate( channel );

      /* Presynaptic: */
      
      for ( i = 0; i < channel->nsynapses; i++ )
	{
	  channel->synapse[i].X = channel->synapse[i].Y = 0.0;
	  channel->synapse[i].pre_activity = 0.0;
	}

      /* Postsynaptic: */

      /* 
       * Set avg_Vm initially to be the voltage of the compartment the
       * channel is connected to.  The way we do it is to find the MsgIn pointer 
       * corresponding to the first (and hopefully, the only) VOLTAGE message 
       * received by the hebbsynchan, and then use that to explicitly grab the
       * initVm field from the source of the message.  
       *
       * Note that this is not a nice object-oriented solution to this problem 
       * as it requires remote memory access outside of the usual message-passing 
       * protocols, but it is the most efficient way to do this.
       */
      

      MSGLOOP( channel, msg )
	{
	  
	case VOLTAGE:                 /* membrane potential */
	  channel->avg_Vm = ( (struct compartment_type *) ( msg->src ) )->initVm;
	  break;
	  
	}

      /**************************************************************
       *                    END HEBBIAN STUFF                       *    
       **************************************************************/
	  
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
	          log( channel->tau1 / channel->tau2 ) / 
	             ( channel->tau1 - channel->tau2 );

	  channel->norm = channel->gmax * ( channel->tau1 - channel->tau2 ) /
	    ( channel->tau1 * channel->tau2 *
	      ( exp( -tpeak / channel->tau1 ) - 
	        exp( -tpeak / channel->tau2 ) ) );
	}

      /**************************************************************
       *                    BEGIN HEBBIAN STUFF                     *    
       **************************************************************/

      /*
       * Presynaptic activity calculations:
       */
 
      /* 
       * The constants used here are not all the same as they are for the 
       * regular synchan alpha function.  These give an exact solution to
       * the differential equation for the alpha function while the standard
       * constants give a solution which is O(dt^2) accurate.
       */

      channel->pre_xconst  = exp( -dt / channel->pre_tau1 );
      channel->pre_yconst1 = exp( -dt / channel->pre_tau2 );
      
      if ( channel->pre_tau1 == channel->pre_tau2 )
	{
	  channel->pre_yconst2 = exp( -dt / channel->pre_tau2 ) * dt;
	  channel->pre_norm    = M_E / channel->pre_tau1;
	} 
      else 
	{
	  double a = ( 1.0 / channel->pre_tau1 ) - ( 1.0 / channel->pre_tau2 );
	  channel->pre_yconst2 = exp( -dt / channel->pre_tau2 ) * 
	    ( 1.0 / a ) * ( 1.0 - exp( -a * dt ) );

	  pre_tpeak = channel->pre_tau1 * channel->pre_tau2 *
	    log( channel->pre_tau1 / channel->pre_tau2 ) /
	       ( channel->pre_tau1 - channel->pre_tau2 );

	  channel->pre_norm = ( channel->pre_tau1 - channel->pre_tau2 ) /
	    ( channel->pre_tau1 * channel->pre_tau2 *
	      ( exp( -pre_tpeak / channel->pre_tau1 ) - 
	        exp( -pre_tpeak / channel->pre_tau2 ) ) );
	}

      /*
       * Postsynaptic activity calculations:
       */

        if( channel->post_tau == 0.0 )
	  channel->post_const = 0.0;
	else
	  channel->post_const = exp( -dt / channel->post_tau );
          
      /**************************************************************
       *                    END HEBBIAN STUFF                       *    
       **************************************************************/

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
      
      RemovePendingSynapticEvents( (struct Synchan_type *) channel );

      allocnodes = (int) ( channel->nsynapses * channel->nodes_per_synapse );

      if ( allocnodes == channel->event_buffer_size )
	break;  /* The buffer is already the right size. */

      if ( allocnodes > channel->event_buffer_size )
	/* need to allocate more nodes and link them in to the free list */
	{
	  num_nodes = allocnodes - channel->event_buffer_size;

	  if ( AllocateSynapticEventNodes( (struct Synchan_type *) channel, num_nodes, 
					   &firstnode, &lastnode ) == 0 )
	    return ( 0 );
	  
	  lastnode->next = channel->FreeSynapticEvents;
	  channel->FreeSynapticEvents = firstnode;

	  break;
	}
      
      /*
       * Otherwise free the event lists and allocate the list based on 
       * the nodes_per_synapse field. 
       */

      FreeSynapticEventLists( (struct Synchan_type *) channel ); 

      if ( AllocateSynapticEventNodes( (struct Synchan_type *) channel, allocnodes, 
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
	ErrorMessage( "HebbSynchan", "No membrane potential.", channel );
      
      /*
       * Check that there is only one VOLTAGE message coming in.
       */

      if ( nvoltage_msgs_in > 1 )
	ErrorMessage( "HebbSynchan", "Too many VOLTAGE messages.", channel );

      if ( ( channel->tau1 < 0 ) || ( channel->tau2 < 0 ) )
	ErrorMessage( "HebbSynchan", "taus must be >= 0.", channel );
      
      if ( channel->gmax < 0 )
	ErrorMessage( "HebbSynchan", "gmax must be >= 0.", channel );
      
      if ( channel->frequency < 0 )
	ErrorMessage( "HebbSynchan", "frequency must be >= 0.", channel );

      if ( channel->nodes_per_synapse <= 0 )
	ErrorMessage( "HebbSynchan", "nodes_per_synapse must be > 0.", channel );

      /*************************************************
       *                HEBBIAN STUFF                  *
       *************************************************/

      if ( ( channel->pre_tau1 < 0 ) || ( channel->pre_tau2 < 0 ) )
	ErrorMessage( "HebbSynchan", "pre_taus must be >= 0.", channel );

      if ( channel->pre_thresh_lo > channel->pre_thresh_hi )
	ErrorMessage( "HebbSynchan", "pre_thresh_lo must be <= pre_thresh_hi.", channel );
      
      if ( channel->post_tau < 0 )
	ErrorMessage( "HebbSynchan", "post_tau must be >= 0.", channel );

      if ( channel->post_thresh_lo > channel->post_thresh_hi )
	ErrorMessage( "HebbSynchan", "post_thresh_lo must be <= post_thresh_hi.", channel );

      if ( channel->post_scale < 0.0 )
	ErrorMessage( "HebbSynchan", "post_scale must be >= 0.", channel );

      if ( channel->min_weight < 0 )
	ErrorMessage( "HebbSynchan", "min_weight must be >= 0.", channel );
      
      if ( channel->max_weight <= 0 )
	ErrorMessage( "HebbSynchan", "max_weight must be > 0.", channel );
      
      if ( channel->min_weight > channel->max_weight )
	ErrorMessage( "HebbSynchan", "min_weight must be <= max_weight.", channel );

      /*
       * Check that all synaptic weights are in the range [min_weight, max_weight].
       */

      for ( i = 0; i < channel->nsynapses; i++ )
	{
	  if ( ( channel->synapse[i].weight < channel->min_weight ) ||
	       ( channel->synapse[i].weight > channel->max_weight ) )
	    {
	      ErrorMessage( "HebbSynchan", "synaptic weights are not within [min_weight,max_weight] range.",
			    channel );
	    }
	}

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
	channel->synapse = (HebbSynapse *) calloc( channel->nsynapses, sizeof( HebbSynapse ) );
      else
	{
	  char *old_synapse_buffer = (char *) channel->synapse;
	  
	  channel->synapse = (HebbSynapse *) realloc( channel->synapse,
						  channel->nsynapses * sizeof( HebbSynapse ) );
	  
	  /*
	   * At this point, since the synapse fields have moved, we have to
	   * remap the message data pointers that point to locations inside
	   * the synapse buffer.
	   */
	  
	  RemapMsgData( channel, old_synapse_buffer,
			( channel->nsynapses * sizeof( HebbSynapse ) ),
			(char *) (channel->synapse) );
	}
      
      /* 
       * Init the new synapse data.
       */

      i = channel->nsynapses - 1;
      
      channel->synapse[i].mi = spikemsg;
      channel->synapse[i].weight =  1;
      channel->synapse[i].delay  =  0;
      
      break; /* ADDMSGIN */


    case DELETEMSGIN:

      /* 
       * When a SPIKE message is deleted, the fields of the other messages 
       * have to be adjusted to take this into account.  If a different message 
       * is deleted do nothing.  Note that the synapse array is not freed in 
       * anticipation of possible future message additions.
       */

      if ( channel->synapse == NULL ) /* There is no synapse array. */
	return ( 1 );
      
      spikemsg = (MsgIn *) action->data;

      if ( spikemsg->type != SPIKE ) /* Not a SPIKE message. */
	return ( 1 );
      
      if ( channel->nsynapses == 1 )  /* There was only one SPIKE message. */
	{
	  channel->nsynapses--;
	  FreeSynapticEventLists( (struct Synchan_type *) channel ); 
	  return ( 1 );
	}
      
      /* 
       * This copies the synapse above the deleted one to the space occupied by 
       * the deleted one (direct struct copy).  This is done for all synapses 
       * above the deleted one.  The "missing memory" is not freed, but is available 
       * when new messages are added due to the realloc() in the ADDMSGIN code.  
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
	  ErrorMessage( "HebbSynchan", "Could not find SPIKE message to delete.", channel );
	  return ( 0 );
	}

	/*
	 * If synapses have moved in memory (which will be the case
	 * unless the synapse being deleted is the last one) we need
	 * to remap references to synapse fields which might exist
	 * in outbould messages.
	 *
	 * We also have to handle outbound messages which refer to the
	 * deleted synapse, which we delete.
	 */

      DeleteMsgReferringToData( channel, (char*) &channel->synapse[synindex],
			sizeof( HebbSynapse ) );

	 if (synindex != channel->nsynapses - 1)
	   {
	     RemapMsgData( channel,
	       (char*) &channel->synapse[synindex+1],
	       ( channel->nsynapses - synindex - 1 ) * sizeof( HebbSynapse ),
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

      FreeSynapticEventLists( (struct Synchan_type *) channel );

      if ( channel->synapse != NULL )
	{
	  free( channel->synapse );
	  channel->synapse = NULL;
	  channel->nsynapses = 0;
	}

      break; /* DELETE */


    case COPY:

      if ( channel->synapse != NULL )
	{
	  Error();
	  printf( "hebbsynchan: COPY: cannot copy synchans with SPIKE messages in current implementation.\n" );
	  printf( "           Please add SPIKE messages after copying synchan objects, not before!\n" );

	  return ( 0 );
	}

      break; /* COPY */


    case SAVE2:

      /*
       * Writing Gk, X, Y, avg_Vm, synapse states and pending event data.
       * Synapse data saved are X, Y, pre_activity and weight.
       */

      n = 4 + 1 + 4*channel->nsynapses + SaveSizeSynapticEvents(channel);
      fwrite(&n,sizeof(int),1,(FILE*)action->data);

      /* Hebb synchan computed fields */
      savedata[0] = channel->Gk;
      savedata[1] = channel->X;
      savedata[2] = channel->Y;
      savedata[3] = channel->avg_Vm;
      fwrite(savedata,sizeof(double),4,(FILE*)action->data);

      savedata[0] = (double) channel->nsynapses + 0.5;
      fwrite(savedata,sizeof(double),1,(FILE*)action->data);

      /* Hebb synapse computed fields */
      for (i = 0; i < channel->nsynapses; i++)
	{
	  savedata[0] = channel->synapse[i].X;
	  savedata[1] = channel->synapse[i].Y;
	  savedata[2] = channel->synapse[i].pre_activity;
	  savedata[3] = channel->synapse[i].weight;
	  fwrite(savedata,sizeof(double),4,(FILE*)action->data);
	}

      SaveSynapticEvents(channel, (FILE*)action->data);
      break; /* SAVE2 */

      
    case RESTORE2:

      n = 0;
      if (fread(&n,sizeof(int),1,(FILE*)action->data) != 1 || n < 6)
	{
	  ErrorMessage("HebbSynchan","Invalid savedata length",channel);
	  return ( n );
	}

      nr = fread(savedata,sizeof(double),4,(FILE*)action->data);
      if (nr != 4)
	{
	  ErrorMessage("HebbSynchan","Invalid savedata",channel);
	  return ( n - nr );
	}

      channel->Gk = savedata[0];
      channel->X = savedata[1];
      channel->Y = savedata[2];
      channel->avg_Vm = savedata[3];
      n -= 4;

      /* read and verify synapse count */
      nr = fread(savedata,sizeof(double),1,(FILE*)action->data);
      if (nr != 1)
	{
	  ErrorMessage("HebbSynchan","Invalid savedata",channel);
	  return ( n - nr );
	}
      n--;

      if ((int)savedata[0] != channel->nsynapses)
	{
	  ErrorMessage("HebbSynchan","Synapse count in savedata does not match",channel);
	  return ( n );
	}

      if (n <= channel->nsynapses*4)
	{
	  ErrorMessage("HebbSynchan","Invalid savedata",channel);
	  return ( n );
	}

      for (i = 0; i < channel->nsynapses; i++)
	{
	  nr = fread(savedata,sizeof(double),4,(FILE*)action->data);
	  if (nr != 4)
	    {
	      ErrorMessage("HebbSynchan","Invalid savedata",channel);
	      return ( n - nr );
	    }
	  n -= 4;

	  channel->synapse[i].X = savedata[0];
	  channel->synapse[i].Y = savedata[1];
	  channel->synapse[i].pre_activity = savedata[2];
	  channel->synapse[i].weight = savedata[3];
	}

      if (RestoreSynapticEvents(channel, &n, (FILE*)action->data) == 0)
	  return ( n );

      return ( 0 ); /* RESTORE2 */


    case SET:
      return Synchan_SET("hebbsynchan", (struct Synchan_type *) channel, action);
      /*NOTREACHED*/
      break;


    case SHOW:
      return Synchan_SHOW("hebbsynchan", (struct Synchan_type *) channel, action);
      /*NOTREACHED*/
      break;

    }

    return ( 1 );
}
