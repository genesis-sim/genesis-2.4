#include <math.h>
#include "newconn_ext.h"  /* will give all the synchan and synapse fields */

/*
 *
 * FILE: stdp_rules.c
 *
 * This file contains the definition of the 'stdp_rules' object and
 * auxilary functions.  This is a clocked object to modify synaptic
 * weights for all synapses of a specified (wildcarded) cell and
 * synchan. The spike timing dependent plasticity (STDP) update
 * algorithm is from Song, Miller, and Abbott (2000).
 *
 * The object fields (defined in stdp_rules_struct.h) are:
 * char *cellpath;  full wildcard path to the postsynaptic cells,
 * e.g. '/layer4/pyrcell[]'
 * char *synpath;    relative path to the cell synchan to be modified
 * char *spikepath;  relative path to the cell spikegen
 * float tau_pre;    decay constant for pre before post spike
 * float tau_post;   decay constant for post before pre spike
 * float min_dt;     minimum time diff to account for latency
 * float max_weight; maximum synaptic weight allowed
 * float min_weight; minimum synaptic weight allowed
 * float dAplus;     increment for postive weight changes
 * float dAminus;    increment for negative weight changes
 * short change_weights;  flag: 0 = plasticity off, otherwise on
 * short debug_level; flag: 0 = no messages, 1 = some messages, 2 = more messages
 * These fields are set on RESET:
 * ElementList *celllist; GENESIS ElementList of cells
 *
 * Author: Dave Beeman, June 2014.
 *
 */

/* stdp_rules_reset is called on RESET, and intializes fields in the
   stdp_rules object.
*/

void stdp_rules_reset(stdpelement)
  struct stdp_rules_type *stdpelement;
 {
   char  *cellpath, *synpath, *spikepath;
   ElementList *celllist;
   short debug_level;
   debug_level = stdpelement->debug_level;
   cellpath = stdpelement->cellpath;
   synpath = stdpelement->synpath;  /* relative path from cell */
   spikepath = stdpelement->spikepath; /* relative path from cell */

   if ((cellpath == NULL) ||  (spikepath == NULL) ||  (synpath == NULL)) {
     printf("WARNING - The fields 'cellpath', 'spikepath' and 'synpath' must be set\n");
      return;
     }

   /* Need some explanation of 'blockmode' argument */
   celllist = WildcardGetElement(cellpath, 1);

  /* store the constructed element lists in the structure */
   stdpelement->celllist = celllist;

}

/* do_stdp_update(stdpelement, synchan, spikegen) is called by the
   stdp_rules object PROCESS action.  It performs the actual update of
   a particular synchan, with information from the spikegen of the same cell.
*/

int do_stdp_update(stdpelement, synchan, spikegen)
    register struct stdp_rules_type *stdpelement;
    register struct Synchan_type *synchan;
    register struct Spikegen_type *spikegen;
{
    short debug_level;
    double  dt, stdp_dt, curr_time, last_post_spike, last_pre_spike;
    double  lastupdate, prev_lastupdate, delta_t, min_dt;
    float   dAplus, dAminus, tau_pre, tau_post, max_weight, min_weight;
    float   Aplus, Aminus, c_weight;
    int    curr_step, nsynapses;
    int i, n;
    short  change_weights, post_fired;

    debug_level = stdpelement->debug_level;
    last_post_spike = spikegen->lastevent;

    /* If the cell has never fired, try again later */
    if (last_post_spike < 0.0) return(1);

    stdp_dt = Clockrate(stdpelement);
    dt = ClockValue(0);
    curr_time = SimulationTime();
    curr_step = GetCurrentStep();

    /* has the postsynaptic cell fired since the last update step ? */
    if (curr_step < round((last_post_spike + stdp_dt)/dt))
	post_fired = 1;
    else
	post_fired = 0;

    nsynapses = synchan->nsynapses;
    if (debug_level > 1) {
	printf("current time = %f current step = %d\n", curr_time, curr_step);
	printf("spikegen last_post_spike =  %f\n", last_post_spike);
	printf("post_fired =  %d\n", post_fired);
	printf("synchan nsynapses =  %d\n", nsynapses);
    }

    /* Now do the actual update of the synapses of the cell [n] synchan */
    dAplus = stdpelement->dAplus; dAminus = stdpelement->dAminus;
    max_weight = stdpelement->max_weight; min_weight = stdpelement->min_weight;
    tau_pre =  stdpelement->tau_pre; tau_post =  stdpelement->tau_post;
    /* min_dt is the minimum delta_t to allow a pre before post spike to cause
       a weight increase.  This is to account for the greater latency that
       realistic cell models have over the usual integrate and fire models.
       The default value is 0.0 (no latency correction).
    */
    min_dt = stdpelement->min_dt;

    /* Loop over the synaptic connections */
    for (i=0; i < nsynapses; i++) {
	prev_lastupdate = -1.0; // initialize
	last_pre_spike = synchan->synapse[i].last_spike_time;
	if (debug_level > 1) {
	    printf("synchan %d last_pre_spike =  %f\n", i, last_pre_spike);
	}
	if (last_pre_spike > 0.0) /* otherwise skip the rest - never fired */
	{
	  /* when were the synapse weight, Aplus, and Aminus fields
	     last updated?
	  */
	    lastupdate =  synchan->synapse[i].lastupdate;
	    if (lastupdate < 0.0) /* initialize lastupdate, go on to next synapse */
		synchan->synapse[i].lastupdate = last_pre_spike;
	    else /* perform the update */
	    {
		delta_t = curr_time - lastupdate;
		/* was it received within current stdp_dt step? */
		if ((curr_step < round ((last_pre_spike + stdp_dt)/dt))
		    && (curr_step >= round (last_pre_spike/dt)))
		{
		    /* get current values of weight, Aplus, Aminus */
		    c_weight = synchan->synapse[i].weight;
		    Aplus = synchan->synapse[i].Aplus;
		    Aminus = synchan->synapse[i].Aminus;
		    if (debug_level > 2)
		    {
			printf("delta_t = %f  Aplus = %f\n", delta_t, Aplus);
		    }
		    Aplus = Aplus * (float) exp(-delta_t/tau_pre) + dAplus;
		    Aminus = Aminus * (float) exp(-delta_t/tau_post);
		    /*            c_weight = fmax(c_weight + Aminus, min_weight); */
		    c_weight = c_weight + Aminus;
		    if (c_weight < min_weight) c_weight = min_weight;
		    synchan->synapse[i].weight = c_weight;
		    synchan->synapse[i].Aplus = Aplus;
		    synchan->synapse[i].Aminus = Aminus;
		    synchan->synapse[i].lastupdate = curr_time;
		    prev_lastupdate = lastupdate;
		    if (debug_level > 2) {
			printf("time = %f  synchan %d weight =  %f\n", curr_time, i, c_weight);
			printf("delta_t = %f  Aplus = %f Aminus = %f\n", delta_t, Aplus, Aminus);
		    }
		} /* test for existence of last presynaptic spike */
	    } /* end else perform update */

	    /* check for generation of postsynaptic spike */
	    if(post_fired) /* It has fired within current stdp_dt step */
	    {
		lastupdate  = synchan->synapse[i].lastupdate;
		if (lastupdate < 0.0) /* initialize lastupdate and goto next synapse */
		    synchan->synapse[i].lastupdate = last_post_spike;
		else /* perform update */
		{
		    /* get current values of weight, Aplus, Aminus */
		    c_weight= synchan->synapse[i].weight;
		    Aplus = synchan->synapse[i].Aplus;
		    Aminus = synchan->synapse[i].Aminus;
		    if (prev_lastupdate > 0.0) /* if just updated, use prev_lastupdate */
			lastupdate = prev_lastupdate;
		    delta_t = curr_time - lastupdate;
		    if (delta_t < min_dt)
			/* do an update, but don't count this event in Aplus */
			delta_t =  curr_time - prev_lastupdate;
		    if (debug_level > 2)
		    {
			printf("delta_t = %f  Aplus = %f\n", delta_t, Aplus);
		    }
		    Aplus = Aplus * (float) exp(-delta_t/tau_pre);
		    Aminus = Aminus * (float) exp(-delta_t/tau_post) + dAminus;
		    /* c_weight = fmin(c_weight + Aplus, max_weight); */
		    c_weight = c_weight + Aplus;
		    if (c_weight > max_weight) c_weight = max_weight;
		    synchan->synapse[i].weight = c_weight;
		    synchan->synapse[i].Aplus = Aplus;
		    synchan->synapse[i].Aminus = Aminus;
		    synchan->synapse[i].lastupdate = curr_time;
		    if (debug_level > 1) {
			printf("time = %f  synchan %d weight =  %f\n", curr_time, i, c_weight);
			printf("delta_t = %f  Aplus = %f Aminus = %f\n", delta_t, Aplus, Aminus);
		    }
		} /* else perform update */
	    } /* test for postsynaptic spike */
	} /* if the cell has fired before */
    }  /* loop over synapses */
    return(1);
}


int StdpRules(stdpelement, action)
    register struct stdp_rules_type *stdpelement;
    Action *action;
{
    double  dt;
    int    nsynapses, ncells, n, i, j;
    int    status;  /* Return value: 1 is OK, 0 means an error occurred. */
    short  debug_level, change_weights;
    double  curr_time, last_post_spike, last_pre_spike;
    int    curr_step;
    char *cellpath, *synpath, *spikepath;
    ElementList *celllist;

    struct Synchan_type* synchan;
    struct Spikegen_type* spikegen;

   char fullsynpath[256];  /* Instead of using calloc() assume 255 char max */  
   char fullspikepath[256];
   short output_len;
   char *pathstr;

    SELECT_ACTION(action)
    {
    case CREATE:
	stdpelement->change_weights = 1;
	stdpelement->debug_level = 0;
	stdpelement->min_weight = 0.0;
	stdpelement->max_weight = 1.0;
	stdpelement->tau_pre = 0.020;
	stdpelement->tau_post = 0.020;
	stdpelement->min_dt = 0.0;
	stdpelement->dAplus = 0.010;
	stdpelement->dAminus = -0.0105;
	break;

    case INIT:
	break;

    case RESET:
	stdp_rules_reset(stdpelement);
	break;

    case PROCESS:
	if (stdpelement->change_weights == 0) break;

        cellpath = stdpelement->cellpath;
        synpath = stdpelement->synpath;  /* relative path from cell */
        spikepath = stdpelement->spikepath; /* relative path from cell */

	celllist = stdpelement->celllist;
	debug_level = stdpelement->debug_level;
	/* Loop over all cells to get the spikegen and synchan to modify */
        if (debug_level > 0) printf("Number of cells =: %d\n", celllist->nelements);

	for (n = 0; n <  celllist->nelements; n++)
	{

	  /* Turn the element list into strings   */
	  pathstr = Pathname(celllist->element[n]);
	  output_len = snprintf(fullsynpath, 256, "%s/%s", pathstr, synpath);
	  output_len = snprintf(fullspikepath, 256, "%s/%s", pathstr, spikepath);
	  /* now get the spikegen and synchan element for the cell */

	  spikegen = (struct Spikegen_type*) GetElement(fullspikepath);
	  synchan = (struct Synchan_type*) GetElement(fullsynpath);

	    if (debug_level > 0) {
		printf("cell number: %d\n", n);
		printf("full synpath = %s\n",fullsynpath );
		printf("full spikepath = %s\n",fullspikepath );
	    }
	    do_stdp_update(stdpelement, synchan, spikegen);
	}
	break;

    case CHECK:
	/* should warn if path fields are not properly initialized */
	break;

    }
    return (1);
}
