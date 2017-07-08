#include <math.h>
#include "newconn_ext.h"  /* will give all the synchan and synapse fields */

/*
 *
 * FILE: facdep_rules.c
 *
 * This file contains the definition of the 'facdep_rules' object and
 * auxilary functions.  This is a clocked object to modify synaptic
 * weights for all synapses of a specified (wildcarded) cell and
 * synchan in a manner similar to the 'stdp_rules' object. However, in
 * this case the weight modification depends only on the arrival times of
 * presynaptic spikes. The algorithm is based on that of Varela et al.
 * (1997), in a way that allows the use of hsolve.
 *
 * The object fields (defined in facdep_rules_struct.h) are:
 * char *cellpath;  full wildcard path to the postsynaptic cells,
 * e.g. '/layer4/pyrcell[]'
 * char *synpath;    relative path to the cell synchan to be modified
 * float dF  ;       increment for facilitating weight changes
 * float dD1  ;      increment for depression factor D1 weight changes
 * float dD2  ;      increment for depression factor D2 weight changes
 * float tau_F;      decay constant for facilitating weight changes
 * float tau_D1;     decay constant for depression factor D1 weight changes
 * float tau_D2;     decay constant for depression factor D2 weight changes
 * short use_depdep ; flag: 0 = use F and D1, otherwise use D1 and D2
 * short change_weights;  flag: 0 = plasticity off, otherwise on
 * short debug_level; flag: 0 = no messages, 1 = some messages, 2 = more messages
 * These fields are set on RESET:
 * ElementList *celllist; GENESIS ElementList of cells
 *
 * Author: Dave Beeman, June 2016.
 *
 */

/*
   A normal synchan RESET also initializes any synchan synapses to set
   the Aplus and Aminus fields to 0, for use with stdp_rules. However
   a RESET of a facdep_rules element requires that they be reset to
   1.0. This is done with the function facdep_rules_reset, which
   performs the RESET action for a facdep_rules element.
*/

void facdep_rules_reset2(facdepelement)
  struct facdep_rules_type *facdepelement;
  {
    char  *cellpath, *synpath;
    ElementList *celllist;
    struct Synchan_type* synchan;
    short debug_level;
    debug_level = facdepelement->debug_level;
    cellpath = facdepelement->cellpath;
    synpath = facdepelement->synpath;  /* relative path from cell */
    if ((cellpath == NULL) ||  (synpath == NULL)) {
        printf("WARNING - The fields 'cellpath' and 'synpath' must be set\n");
        return;
    }
   /* Need some explanation of 'blockmode' argument */
   celllist = WildcardGetElement(cellpath, 1);
   /* store the constructed element lists in the structure */
   facdepelement->celllist = celllist;
   int i, n, nsynapses;
   /* Now loop over all cells and synapses to reset Aplus and Aminus to 1.0 */
   cellpath = facdepelement->cellpath;
   synpath = facdepelement->synpath;  /* relative path from cell */
   short output_len;
   char *pathstr;
   char fullsynpath[256];  /* Instead of using calloc() assume 255 char max */  
   /* Loop over all cells to get the synchan to modify */
   for (n = 0; n <  celllist->nelements; n++)
     {
        /* Turn the element list into strings   */
        pathstr = Pathname(celllist->element[n]);
        output_len = snprintf(fullsynpath, 256, "%s/%s", pathstr, synpath);
        /* now get the synchan element for the cell */
        synchan = (struct Synchan_type*) GetElement(fullsynpath);
        /* now loop over synapses */
        nsynapses = synchan->nsynapses;
        for (i=0; i < nsynapses; i++) {
            synchan->synapse[i].Aplus = 1.0;
            synchan->synapse[i].Aminus = 1.0;
        }
     }
 }

/* do_facdep_update(facdepelement, synchan) is called by the
   facdep_rules object PROCESS action.  It performs the actual update of
   a particular synchan.
*/

/* This version of do_facdep_update(facdepelement, synchan) is called by the
   facdep_rules2 object PROCESS action.  It performs the actual update of
   a synchan with a weight decay at each time step, rather than when the next
   spike is received. This will be slower, but avoids a problem when spike come
   in pulses.
*/

int do_facdep_update2(facdepelement, synchan)
    register struct facdep_rules_type *facdepelement;
    register struct Synchan_type *synchan;
{
  short debug_level, use_depdep;
    double  dt, facdep_dt, curr_time, last_pre_spike;
    double  lastupdate, delta_t;
    float   F, D1, D2, dF, dD1, dD2, tau_F, tau_D1, tau_D2;

    float   orig_weight, c_weight;
    int    curr_step, nsynapses;
    int i, n;

    debug_level = facdepelement->debug_level;
    use_depdep = facdepelement->use_depdep;

    facdep_dt = Clockrate(facdepelement);
    dt = ClockValue(0);
    curr_time = SimulationTime();
    curr_step = GetCurrentStep();

    nsynapses = synchan->nsynapses;
    if (debug_level > 1) {
	printf("current time = %f current step = %d\n", curr_time, curr_step);
	printf("synchan nsynapses =  %d\n", nsynapses);
    }

    /* Now do the actual update of the synapses of the cell [n] synchan */
    dF = facdepelement->dF; dD1 = facdepelement->dD1;  dD2 = facdepelement->dD2;
    tau_F = facdepelement->tau_F; tau_D1 = facdepelement->tau_D1;
    tau_D2 = facdepelement->tau_D2;
    /* Loop over the synaptic connections */
    for (i=0; i < nsynapses; i++) {
	last_pre_spike = synchan->synapse[i].last_spike_time;
	if (debug_level > 1) {
	    printf("synchan %d last_pre_spike =  %f\n", i, last_pre_spike);
	}

	if (last_pre_spike >= 0.0) /* otherwise skip the rest - never fired */
	{
          lastupdate =  synchan->synapse[i].lastupdate;

	    /* when were the synapse weight, Aplus, and Aminus fields
	     last updated? */

	    if (lastupdate < 0.0) /* first spike to arrive - initialize lastupdate */
		synchan->synapse[i].lastupdate = last_pre_spike;
            /* go on to next synapse */
	    else /* perform the update */
	    {
		delta_t = curr_time - lastupdate;
	        /* get current values of weight, Aplus, Aminus */
		c_weight = synchan->synapse[i].weight;
		if (!use_depdep) /* use F (fac) for Aplus and D1 (dep 1) for Aminus */
		  {
		     F = synchan->synapse[i].Aplus;
		     D1 = synchan->synapse[i].Aminus;
		     orig_weight = c_weight/(F*D1);
   	             /* was the spike received in the current facdep_dt time step? */
	             if ((curr_step < round ((last_pre_spike + facdep_dt)/dt))
                          && (curr_step >= round (last_pre_spike/dt)))
	                {
          		     F = F + dF; D1 = D1*dD1; /* increment/scale the facdep variables */
                        }

	             /* In any case, apply decay to the fac/dep variables and adjust weight */
		     F = 1 + (F -1) * (float) exp(-delta_t/tau_F);
		     D1 = 1 - (1 -D1) * (float) exp(-delta_t/tau_D1); /*decay to this */
		     c_weight = orig_weight*F*D1; /* will be used for the next spike */
		     synchan->synapse[i].weight = c_weight;
		     synchan->synapse[i].Aplus = F;
		     synchan->synapse[i].Aminus = D1;
		     synchan->synapse[i].lastupdate = curr_time;
		     if (debug_level > 2) {
		       printf("F = %f  D1 = %f  weight =  %f\n", F, D1, c_weight);
		     }
                  }
		else  /* use D1 (dep 1) for Aplus and D2 (dep 2) for Aminus */
		    {
		     D1 = synchan->synapse[i].Aplus;
		     D2 = synchan->synapse[i].Aminus;
		     orig_weight = c_weight/(D1*D2);
	             if ((curr_step < round ((last_pre_spike + facdep_dt)/dt))
                          && (curr_step >= round (last_pre_spike/dt)))
	                {
			  D1 = D1*dD1;  D2=D2*dD2; /* scale the facdep variables */
                        }
		     /* In any case, apply decay to the fac/dep variables and adjust the weight */
		     D1 = 1 - (1 - D1) * (float) exp(-delta_t/tau_D1); /*decay to this */
		     D2 = 1 - (1 - D2) * (float) exp(-delta_t/tau_D2); /*decay to this */
		     c_weight = orig_weight*D1*D2; /* will be used for the next spike */
		     synchan->synapse[i].weight = c_weight;
		     synchan->synapse[i].Aplus = D1;
		     synchan->synapse[i].Aminus = D2;
		     synchan->synapse[i].lastupdate = curr_time;

		     if (debug_level > 2) {
		       printf("D1 = %f  D2 = %f  weight =  %f\n", D1, D2, c_weight);
		     }
	        } /* end else use depdep */
	    } /* end else perform update */
	} /* if the cell has fired before */
    }  /* loop over synapses */
    return(1);
}


/* This version of the object uses the improved (but slower) do_facdep_update2 */
int FacdepRules2(facdepelement, action)
    register struct facdep_rules_type *facdepelement;
    Action *action;
{
    double  dt;
    int    nsynapses, ncells, n, i, j;
    int    status;  /* Return value: 1 is OK, 0 means an error occurred. */
    short  debug_level, change_weights, use_depdep;
    double  curr_time, last_pre_spike;
    int    curr_step;
    char *cellpath, *synpath;
    ElementList *celllist;

    struct Synchan_type* synchan;

   char fullsynpath[256];  /* Instead of using calloc() assume 255 char max */  
   short output_len;
   char *pathstr;

    SELECT_ACTION(action)
    {
    case CREATE:
	facdepelement->change_weights = 1;
	facdepelement->debug_level = 0;
	facdepelement->use_depdep = 0;
	facdepelement->dF = 0.2;
	facdepelement->dD1 = 1.0;
	facdepelement->dD2 = 1.0;
	facdepelement->tau_F = 0.20;
	facdepelement->tau_D1 = 0.38;
	facdepelement->tau_D1 = 9.2;
	break;

    case INIT:
	break;

    case RESET:
	facdep_rules_reset2(facdepelement);
	break;

    case PROCESS:
	if (facdepelement->change_weights == 0) break;

        cellpath = facdepelement->cellpath;
        synpath = facdepelement->synpath;  /* relative path from cell */
	celllist = facdepelement->celllist;
	debug_level = facdepelement->debug_level;
	/* Loop over all cells to get the synchan to modify */
        if (debug_level > 0) printf("Number of cells = %d\n", celllist->nelements);

	for (n = 0; n <  celllist->nelements; n++)
	{

	  /* Turn the element list into strings   */
	  pathstr = Pathname(celllist->element[n]);
	  output_len = snprintf(fullsynpath, 256, "%s/%s", pathstr, synpath);
	  /* now get the synchan element for the cell */
	  synchan = (struct Synchan_type*) GetElement(fullsynpath);

	    if (debug_level > 0) {
		printf("cell number = %d\n", n);
		printf("full synpath = %s\n",fullsynpath );
	    }
	    do_facdep_update2(facdepelement, synchan);
	}
	break;

    case CHECK:
	/* should warn if path fields are not properly initialized */
	break;

    }
    return (1);
}
