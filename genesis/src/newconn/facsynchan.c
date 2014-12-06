/*
 *
 * FILE: facsynchan.c
 *
 *     This file contains the definition of the `facsynchan' synaptic
 *     channel object.  This object is essentially identical to the
 *     standard `synchan' object except that it contains a simple model for
 *     synaptic facilitation and/or depression.
 *
 * AUTHOR: Mike Vanier
 *
 * Removed "inline" declaration of functions to fix errors with MacOS
 * July 2014 for GENESIS 2.4
 * 
 * $Log: facsynchan.c,v $
 * Revision 1.2  2005/07/20 20:02:01  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:29  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2001/06/29 21:16:10  mhucka
 * Loads of un-ANSIfication to support old compilers like SunOS 4's cc.
 *
 * Revision 1.3  2000/04/27 07:29:24  mhucka
 * Added fix from Mike Vanier for synchan objects involving SET and SHOW
 * operations.  The problem was that they failed to trap attempts to set
 * fields in nonexistent synapses.
 *
 * Revision 1.2  1999/12/29 10:28:49  mhucka
 * Removed (admittedly in an ugly way) depency on GCC.
 *
 */

/*
 *
 * Copyright (c) 1999 Michael Christopher Vanier
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without license
 * or royalty fees, to use, copy, modify, and distribute this software and its
 * documentation for any purpose, provided that the above copyright notice and
 * the following two paragraphs appear in all copies of this software.
 *
 * In no event shall Michael Christopher Vanier (hereinafter referred to as
 * "the author") or the Genesis Developer's Group be liable to any party for
 * direct, indirect, special, incidental, or consequential damages arising out
 * of the use of this software and its documentation, even if the author
 * and/or the Genesis Developer's Group have been advised of the possibility
 * of such damage.
 *
 * The author and the Genesis Developer's Group specifically disclaim any
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose.  The software
 * provided hereunder is on an "as is" basis, and the author and the Genesis
 * Developer's Group have no obligation to provide maintenance, support,
 * updates, enhancements, or modifications.
 *  
 */

#include <math.h>
#include "fac_ext.h"

#define MYNAME "facsynchan"

static double
savedata[3];


/* ----------------------------------------------------------------- */
/*                FACILITATION-RELATED FUNCTIONS                     */
/* ----------------------------------------------------------------- */

/*
 * All the code that handles facilitation and depression of the synaptic
 * strengths is located here.
 */

/*
 * FUNCTION
 *     FacSynchan_initialize_facilitation
 *
 * DESCRIPTION
 *     Initialize facilitation/depression-related fields on the synchan.
 *     Called when the synchan is created only (i.e. on a CREATE action).
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

void FacSynchan_initialize_facilitation(element)
struct fac_synchan_type *element;
{
    /* The default is no synaptic facilitation or depression. */
    element->fac_depr_on     =  1;
    element->max_fac         = -1.0;  /* The default upper limit is no limit. */
    element->fac_per_spike   =  0.0;
    element->fac_tau         =  0.0;
    element->depr_per_spike  =  0.0;
    element->depr_tau        =  0.0;
}




/*
 * FUNCTION
 *     FacSynchan_reset_facilitation
 *
 * DESCRIPTION
 *     Reset facilitation/depression-related fields on the synapses to
 *     their default values.  Called on a RESET action.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

void FacSynchan_reset_facilitation(element)
struct fac_synchan_type *element;
{
    int i;

    for (i = 0; i < element->nsynapses; i++)
    {
        element->synapse[i].fac  = 0.0;
        element->synapse[i].depr = 0.0;
    }
}




/*
 * FUNCTION
 *     FacSynchan_recalc_facilitation
 *
 * DESCRIPTION
 *     Recalculate facilitation/depression-related fields on the synchan.
 *     Called on a RECALC (and hence on a RESET) action.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

void FacSynchan_recalc_facilitation(element)
struct fac_synchan_type *element;
{
    double dt = Clockrate(element);

    /*
     * If {fac, depr}_tau = 0.0 there is no synaptic facilitation/
     * depression respectively.  This is a micro-hack.
     */

    if (element->fac_tau > 0.0)
        element->fac_const = exp(-dt / element->fac_tau);
    else
        element->fac_const = 0.0;

    if (element->depr_tau > 0.0)
        element->depr_const = exp(-dt / element->depr_tau);
    else
        element->depr_const = 0.0;
}




/*
 * FUNCTION
 *     FacSynchan_check_facilitation
 *
 * DESCRIPTION
 *     Check facilitation/depression-related fields on the synchan.
 *     Called on a CHECK action.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

int FacSynchan_check_facilitation(element)
struct fac_synchan_type *element;
{
    ERROR_IF (element->fac_per_spike < 0.0,
              "%s object %s: CHECK: fac_per_spike must be >= 0.0.\n",
              MYNAME, MYPATH);

    ERROR_IF (element->fac_tau < 0.0,
              "%s object %s: CHECK: fac_tau must be >= 0.0.\n",
              MYNAME, MYPATH);

    ERROR_IF (element->depr_per_spike < 0.0,
              "%s object %s: CHECK: depr_per_spike must be >= 0.0.\n",
              MYNAME, MYPATH);

    ERROR_IF (element->depr_tau < 0.0,
              "%s object %s: CHECK: depr_tau must be >= 0.0.\n",
              MYNAME, MYPATH);

    return 1;
}




/*
 * FUNCTION
 *     FacSynchan_initialize_synapse
 *
 * DESCRIPTION
 *     Initialize the synaptic weights and delays of new synapses.
 *     Also initialize facilitation/depression-related fields.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *     int i                            -- index of synapse
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

void FacSynchan_initialize_synapse(element, i)
struct fac_synchan_type *element;
int i;
{
#ifdef PRECONDITIONS
    assert(element->synapse != NULL);
    assert(element->nsynapses > i);
#endif

    /* Initialize standard synapse fields. */
    element->synapse[i].weight = 1.0;
    element->synapse[i].delay  = 0.0;

    /* Initialize facilitation/depression-related fields. */
    element->synapse[i].fac    = 0.0;
    element->synapse[i].depr   = 0.0;
}




/*
 * FUNCTION
 *     FacSynchan_update_facilitation
 *
 * DESCRIPTION
 *     Update the facilitation/depression state of all the
 *     synapses at the end of the PROCESS step of the synchan.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

int FacSynchan_update_facilitation(element)
struct fac_synchan_type *element;
{
    int i;

#ifdef PRECONDITIONS
    assert(element->synapse != NULL);
#endif

    /*
     * Synaptic facilitation/depression decays exponentially after the
     * initial onset.  If {fac, depr}_tau = 0.0 then there is no
     * facilitation/depression respectively.
     */

    if (element->fac_tau > 0.0)
    {
        for (i = 0; i < element->nsynapses; i++)
            element->synapse[i].fac *= element->fac_const;
    }

    if (element->depr_tau > 0.0)
    {
        for (i = 0; i < element->nsynapses; i++)
            element->synapse[i].depr *= element->depr_const;
    }

    return 1;
}




/*
 * FUNCTION
 *     FacSynchan_update_facilitation_spike
 *
 * DESCRIPTION
 *     Update the facilitation/depression state of a synapse
 *     when a spike hits.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *     int i                            -- index of synapse
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

int FacSynchan_update_facilitation_spike(element, i)
struct fac_synchan_type *element;
int i;
{
#ifdef PRECONDITIONS
    assert(element->synapse != NULL);
    assert(element->nsynapses > i);
#endif

    element->synapse[i].fac  += element->fac_per_spike;
    element->synapse[i].depr += element->depr_per_spike;

    return 1;
}




/*
 * FUNCTION
 *     FacSynchan_get_effective_weight
 *
 * DESCRIPTION
 *     Calculate the effective synaptic weight when a spike hits,
 *     taking into account all synaptic facilitations and depressions.
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *     int i                            -- index of synapse
 *
 * RETURN VALUE
 *     double -- the effective weight for synapse i.
 *
 * AUTHOR
 *     Mike Vanier
 *
 */

double FacSynchan_get_effective_weight(element, i)
struct fac_synchan_type *element;
int i;
{
    double weight_multiplier;
    double eff_weight;         /* Effective synaptic weight. */

#ifdef PRECONDITIONS
    assert(element->synapse != NULL);
    assert(element->nsynapses > i);
#endif

    /*
     * Facilitation and depression values are real numbers > 0.0.
     * Facilitation causes a multiplicative increase in the synaptic weight
     * such that a facilitation value of 1.0 doubles the synaptic weight
     * (i.e. it goes up by 100%).  Depression causes a multiplicative
     * decrease in the synaptic weight such that a depression value of 1.0
     * halves the synaptic weight.
     */

    weight_multiplier = (1.0 + element->synapse[i].fac)
        / (1.0 + element->synapse[i].depr);

    /* 
     * Limit the value of weight_multiplier to max_fac.
     * If max_fac is < 0.0 there is no limit.
     */

    if ((element->max_fac >= 0.0) 
        && (weight_multiplier > element->max_fac))
    {
        weight_multiplier = element->max_fac;
    }

    eff_weight = element->synapse[i].weight * weight_multiplier;

    return eff_weight;
}


/* ----------------------------------------------------------------- */
/*            END OF FACILITATION-RELATED FUNCTIONS                  */
/* ----------------------------------------------------------------- */





/*
 * OBJECT
 *     facsynchan
 *         This object is the same as the `synchan' object except that
 *         it implements a model for synaptic facilitation and depression.
 *
 * FUNCTION
 *     FacSynchan
 *
 * ARGUMENTS
 *     struct fac_synchan_type *element -- address of element
 *     Action                  *action  -- address of action
 *
 * RETURN VALUE
 *     int -- 0 = failure and 1 = success (usually; for RESTORE2
 *            0 = success and nonzero = failure).
 *
 * AUTHOR
 *     Mike Vanier
 *
 */


int
FacSynchan(element, action)
struct fac_synchan_type *element;
Action *action;
{
    double  x;
    double  dt;
    double  eventtime;
    int     nvoltage_msgs_in;
    float   tpeak;
    Msg    *spikemsg;
    Msg    *msg;
    double  V = 0.0;
    double  eff_weight;
    int     i, time, allocnodes, num_nodes, syn;
    short   synindex;
    SynapticEvent *event, *next_event, *pending_event,
        *last_event, *firstnode, *lastnode;
    int     n, nr;
    short   nsynapses;
    int     status;  /* Return value: 1 is OK, 0 means an error occurred. */


    SELECT_ACTION(action)
    {
    case CREATE:
        /* Set up default values. */
        element->synapse_size      = (unsigned short)(sizeof(Fac_Synapse));
        element->synapse           = NULL; /* no synapses to start with */
        element->event_buffer_size = 0;    /* ditto */
        element->pending_events    = 0;
        element->nodes_per_synapse = 1.0;
        element->list_alloced      = 0;
        element->allocednodes      = 0;
        FacSynchan_initialize_facilitation(element);

        break;


    case EVENT:
        /*
         * This action is executed when a spike occurs at any of the
         * spikegen elements which are presynaptic to the synchan.
         */

        spikemsg = (Msg*)action->data;

        for (i = 0; i < element->nsynapses; i++)
        {
            /*
             * Scan array of spike messages in synapse array, looking for
             * the one whose msgin field (mi) is the same as the action
             * message.
             */

            if (element->synapse[i].mi == spikemsg)
            {
                ++element->pending_events;

                eventtime = *(double*)action->argv;
                dt = Clockrate(element);

                /*
                 * Calculate the time until the spike hits in time steps.
                 * The 0.9 is to make sure the time is rounded up to the
                 * nearest step.  The eventtime is normally the same as
                 * SimulationTime() so (SimulationTime() - eventtime) is
                 * normally zero; however, parallel implementations of this
                 * code may bunch several events together and send them at
                 * a later time step if they can, so in that case eventtime
                 * may not be the same as SimulationTime().
                 */

                time = (int)((element->synapse[i].delay -
                              (SimulationTime() - eventtime)) / dt + 0.9);

                /*
                 * Grab a node from the FreeSynapticEvents list and record
                 * the synapse number of this event in the node.  Scan
                 * through the PendingSynapticEvents list, figure out where
                 * this node goes and link it in to the list.  Decrement
                 * the time variable by the time fields of each event
                 * before it so the time field will be the number of time
                 * steps relative to the last event.  If the freelist is
                 * empty allocate more nodes.
                 */

                if (element->FreeSynapticEvents == NULL)
                {
                    /*
                     * Allocate more nodes in chunks of
                     * (synapses + (nodes_per_synapse/2) + 1)
                     */

                    allocnodes = (int)(element->nsynapses *
                                       element->nodes_per_synapse / 2.0) + 1;

                    if (AllocateSynapticEventNodes(
                        element, allocnodes,
                        &(element->FreeSynapticEvents),
                        &lastnode) == 0)
                    {
                        return 0;
                    }
                }

                /* Grab a new node and set its syn_num field. */

                event = element->FreeSynapticEvents;
                element->FreeSynapticEvents = event->next;
                event->syn_num = i;

                /*
                 * Look for the position of the new event in the pending
                 * list.
                 */

                if (element->PendingSynapticEvents == NULL)
                {
                    /* The new event is the only one. */
                    event->time = time;
                    event->next = NULL;
                    element->PendingSynapticEvents = event;
                }
                else
                {
                    last_event = NULL;

                    for (pending_event = element->PendingSynapticEvents;
                         pending_event != NULL;
                         pending_event = pending_event->next)
                    {
                        if (((unsigned int)time) <= pending_event->time)
                        {
                            /*
                             * Insert the new event in the pending
                             * list.  Also adjust the time field for
                             * the event after the new event.
                             */

                            if (last_event == NULL)
                            {
                                /* The new event is the first event. */
                                element->PendingSynapticEvents = event;
                            }
                            else
                            {
                                last_event->next = event;
                            }

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

                    if (pending_event == NULL)
                    {
                        /* The new event is the last event. */
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
         * value to the activation (to be normalized later).  Since the
         * next event after that could hit at the same time, check the time
         * field for the next event.  If it's also zero then add its weight
         * to the activation, and so on.  If the first event didn't hit
         * this time step then decrement its time field.
         */

        dt = Clockrate(element);
        element->activation = 0.0;

        /* Get next_event from the top of the pending event list. */
        next_event = element->PendingSynapticEvents;

        if (next_event != NULL) /* skip if no events are pending */
        {
            while ((next_event != NULL) && (next_event->time == 0))
            {
                /* execute the event */
                --element->pending_events;
                syn = next_event->syn_num;
                last_event = next_event;
                element->PendingSynapticEvents = next_event
                    = next_event->next;

                /* Put the event back onto the free list. */

                last_event->next = element->FreeSynapticEvents;
                element->FreeSynapticEvents = last_event;

                /*
                 * Get the effective synaptic weight, and also adjust the
                 * facilitation/depression state to take the new spike into
                 * account.  Note that the facilitation/depression must be
                 * updated AFTER calculating the effective weight.  Note also
                 * that the effective weight is not stored anywhere; it is
                 * calculated only when a spike hits.  This reduces the
                 * computational demand and the storage requirements.  On the
                 * other hand, it would be nice to be able to plot the
                 * effective weight as a function of time.
                 *
                 */

                if (element->fac_depr_on)
                {
                    eff_weight
                        = FacSynchan_get_effective_weight(element, syn);
                    FacSynchan_update_facilitation_spike(element, syn);

                    /* Normalize to step size by dividing by dt. */
                    element->activation += eff_weight / dt;
                }
                else
                {
                    /* Normalize to step size by dividing by dt. */
                    element->activation += element->synapse[syn].weight / dt;
                }
            }

            if (next_event != NULL)
                --next_event->time;
        }

        MSGLOOP(element, msg)
        {
        case VOLTAGE:                 /* membrane potential */
            V = MSGVALUE(msg, 0);
            break;
        }

        /*
         * Calculate the `activation' level of the channel:
         *
         * dx/dt = (activation * tau1 - x) / tau1
         * x = activation * tau1 * (1 - exp(-dt / tau1))
         *     + x * exp(-dt / tau1))
         *
         * or
         *
         * x = activation * xconst1 + x * xconst2
         *
         */

        x = element->activation * element->xconst1
            + element->X * element->xconst2;

        /*
         * Calculate the conductance of the channel:
         *
         * dy/dt = (x * tau2 - y) / tau2
         * y = x * tau2 * (1 - exp(-dt / tau2)) + y * exp(-dt / tau2))
         *
         * or
         *
         * y = x * yconst1 + y * yconst2
         *
         */

        element->Y = element->X * element->yconst1
            + element->Y * element->yconst2;

        /*
         * Set the new values of the state variables.
         */

        element->X  = x;
        element->Gk = element->Y * element->norm;

        /*
         * Calculate the channel current based on the channel conductance,
         * the ion equilibrium potential and the membrane potential.
         * NOTE: this is just for bookkeeping and isn't needed for the
         *       simulation.
         */

        element->Ik = (element->Ek - V) * element->Gk;

        /*
         * Update the facilitation/depression state of the synapses;
         * note that this has no effect on the synaptic conductance
         * whatsoever until a spike hits.
         */

        if (element->fac_depr_on)
        {
            status = FacSynchan_update_facilitation(element);

            ERROR_IF (status == 0,
                      "%s object %s: PROCESS: facilitation update failed.\n",
                      MYNAME, MYPATH);
        }

        break; /* PROCESS */


    case RESET:
        element->activation = 0.0;
        element->Gk         = 0.0;
        element->X          = 0.0;
        element->Y          = 0.0;

        /*
         * If the free list has not been allocated, allocate it based
         * on the nodes_per_synapse field.
         */

        allocnodes = (int)(element->nsynapses
                           * element->nodes_per_synapse);

        if (allocnodes != 0)
        {
            if (!element->list_alloced)
            {
                if (AllocateSynapticEventNodes(
                    element,
                    allocnodes,
                    &(element->FreeSynapticEvents),
                    &lastnode) == 0)
                {
                    return 0;
                }

                element->PendingSynapticEvents = NULL;
                element->list_alloced = 1;
            }
            else
            {
                /*
                 * If it has, move all pending events back on to the
                 * free list.  Note that the time and syn_num contents
                 * of the free list nodes are meaningless.
                 */

                RemovePendingSynapticEvents(element);
            }
        }

        /*
         * Reset the facilitation-related fields on the synapses
         * to the default values.
         */

        if (element->fac_depr_on)
            FacSynchan_reset_facilitation(element);

        /* RECALC is automatically done on a RESET. */
        /*FALLTHROUGH*/

    case RECALC:
        dt = Clockrate(element);

        element->xconst1
            = element->tau1 * (1.0 - exp(-dt / element->tau1));
        element->xconst2 = exp(-dt / element->tau1);

        element->yconst1
            = element->tau2 * (1.0 - exp(-dt / element->tau2));
        element->yconst2 = exp(-dt / element->tau2);

        if (element->tau1 == element->tau2)
        {
            element->norm = element->gmax * M_E / element->tau1;
        }
        else
        {
            tpeak = element->tau1 * element->tau2
                * log(element->tau1 / element->tau2)
                / (element->tau1 - element->tau2);

            element->norm = element->gmax
                * (element->tau1 - element->tau2)
                / (element->tau1 * element->tau2
                   * (exp(-tpeak / element->tau1)
                      - exp(-tpeak / element->tau2)));
        }

        /* Recalculate facilitation/depression-related fields. */

        if (element->fac_depr_on)
            FacSynchan_recalc_facilitation(element);

        break; /* RECALC */


    case RESETBUFFER:
        /*
         * Shrinks the event buffer down to its original size as
         * specified by the nodes_per_synapse field.
         */

        if (!element->list_alloced)
            break;  /* This is only meaningful if the lists exist. */

        /*
         * If there are events on the pending list, put all
         * pending event nodes back onto the free list.
         */

        RemovePendingSynapticEvents(element);

        allocnodes = (int)(element->nsynapses
                           * element->nodes_per_synapse);

        if (allocnodes == element->event_buffer_size)
            break;  /* The buffer is already the right size. */

        if (allocnodes > element->event_buffer_size)
        {
            /*
             * We need to allocate more nodes and link them in to the
             * free list.
             */

            num_nodes = allocnodes - element->event_buffer_size;

            if (AllocateSynapticEventNodes(element,
                                           num_nodes,
                                           &firstnode,
                                           &lastnode) == 0)
            {
                return 0;
            }

            lastnode->next = element->FreeSynapticEvents;
            element->FreeSynapticEvents = firstnode;

            break;
        }

        /*
         * Otherwise free the event lists and allocate the list based
         * on the nodes_per_synapse field.
         */

        FreeSynapticEventLists(element);

        if (AllocateSynapticEventNodes(element,
                                       allocnodes,
                                       &(element->FreeSynapticEvents),
                                       &lastnode) == 0)
        {
            return 0;
        }

        element->PendingSynapticEvents = NULL;
        element->list_alloced = 1;

        break; /* RESETBUFFER */


    case CHECK:
        nvoltage_msgs_in = 0;

        MSGLOOP(element, msg)
        {
        case VOLTAGE:
            ++nvoltage_msgs_in;

            break;
        }

        ERROR_IF (nvoltage_msgs_in == 0,
                  "%s object %s: CHECK: no membrane potential.\n",
                  MYNAME, MYPATH);

        /*
         * Check that there is only one VOLTAGE message coming in.
         */

        ERROR_IF (nvoltage_msgs_in > 1,
                  "%s object %s: CHECK: Too many VOLTAGE messages.\n",
                  MYNAME, MYPATH);

        ERROR_IF ((element->tau1 <= 0) || (element->tau2 <= 0),
                  "%s object %s: CHECK: taus must be > 0.\n",
                  MYNAME, MYPATH);

        ERROR_IF (element->gmax < 0.0,
                  "%s object %s: CHECK: gmax must be >= 0.\n",
                  MYNAME, MYPATH);

        ERROR_IF (element->nodes_per_synapse <= 0,
                  "%s object %s: CHECK: nodes_per_synapse must be > 0.\n",
                  MYNAME, MYPATH);

        /* Check facilitation/depression-related fields. */

        if (element->fac_depr_on)
        {
            if (FacSynchan_check_facilitation(element) == 0)
                return 0;
        }

        break; /* CHECK */


    case ADDMSGIN:
        /*
         * When a SPIKE message is added, the fields of the
         * other messages have to be adjusted to take this into
         * account.  If a different message is added do nothing.
         */

        spikemsg = (Msg *)action->data;

        if (spikemsg->type == SPIKE)
            element->nsynapses++;

        if (element->nsynapses == 0)
            return 1;

        /* Allocate or reallocate space for the synapse buffer. */

        if (element->synapse == NULL)
        {
            element->synapse = (Fac_Synapse *)
                calloc((size_t)element->nsynapses,
                       sizeof(Fac_Synapse));
        }
        else
        {
            char *old_synapse_buffer = (char *)element->synapse;

            element->synapse = (Fac_Synapse *)
                realloc(element->synapse,
                        element->nsynapses
                        * sizeof(Fac_Synapse));

            /*
             * At this point, since the synapse fields have moved, we have
             * to remap the message data pointers that point to locations
             * inside the synapse buffer.
             */

            RemapMsgData(element, old_synapse_buffer,
                         (element->nsynapses * sizeof(Fac_Synapse)),
                         (char *)(element->synapse));
        }

        /*
         * The synapse Msg pointers must be updated due to the
         * reallocation of memory.  This is why we need a MSGLOOP here.
         */

        i = 0;

        MSGLOOP(element, msg)
        {
        case SPIKE:
            element->synapse[i].mi = msg;

            if (i == (element->nsynapses - 1))
            {
                /* set up defaults for new message */

                FacSynchan_initialize_synapse(element, i);
            }

            i++;
            break;

        default:
            break;
        }

        break; /* ADDMSGIN */


    case DELETEMSGIN:
        /*
         * When a SPIKE message is deleted, the fields of the other
         * messages have to be adjusted to take this into account.  If a
         * different message is deleted do nothing.  Note that the synapse
         * array is not freed in anticipation of possible future message
         * additions.  Note also that MSGINDELETED gets called later to
         * make sure the mi pointers of the synapses are pointing to the
         * right messages.
         */

        if (element->synapse == NULL) /* There is no synapse array. */
            return 1;

        spikemsg = (Msg *)action->data;

        if (spikemsg->type != SPIKE) /* Not a SPIKE message. */
            return 1;

        if (element->nsynapses == 1)
        {
            /* There was only one SPIKE message. */
            element->nsynapses--;
            FreeSynapticEventLists(element);
            return 1;
        }

        /*
         * This copies the synapse above the deleted one to the space
         * occupied by the deleted one (direct struct copy).  This is done
         * for all synapses above the deleted one.  The "missing memory" is
         * not freed, but is available when new messages are added due to
         * the realloc() in the ADDMSGIN code.  Note that the mi pointers
         * are incorrect at this point; they get set correctly in the
         * MSGINDELETED action (see below).
         */

        for (i = 0, synindex = -1; i < element->nsynapses; i++)
        {
            if (spikemsg != NULL)
            {
                if (element->synapse[i].mi == spikemsg)
                {
                    /* Found the message so don't repeat this. */
                    spikemsg = NULL;
                    synindex = i;
                }
            }
            else
            {
                element->synapse[i-1] = element->synapse[i];
            }
        }

        if (synindex < 0)
        {
            ERROR2 ("%s object %s: DELETEMSGIN: Could not find SPIKE message to delete.\n",
                   MYNAME, MYPATH);
        }

        element->nsynapses--;

        /*
         * Now scan the list of pending events and remove all of those
         * corresponding to the synapse that has been deleted.  Then adjust
         * the time fields and next pointers of the adjacent events.  Also
         * decrement the syn_num of any event whose syn_num is greater than
         * synindex.
         */

        last_event = NULL;

        for (event = element->PendingSynapticEvents;
             event != NULL;
             event = next_event)
        {
            next_event = event->next;

            if (event->syn_num == synindex)
            {
                /* Put the node onto the free list. */

                event->next = element->FreeSynapticEvents;
                element->FreeSynapticEvents = event;

                if (last_event == NULL)
                    element->PendingSynapticEvents = next_event;
                else
                    last_event->next = next_event;

                if (next_event != NULL)
                    next_event->time += event->time;
            }
            else
            {
                if (event->syn_num > ((unsigned short)synindex))
                    --event->syn_num;

                last_event = event;
            }
        }

        break; /* DELETEMSGIN */


    case MSGINDELETED:
        /*
         * This gets called AFTER the Msg is deleted so that the mi field
         * in the synapses is readjusted to the correct value.  The synapse
         * Msg pointers must be updated because the position of the
         * messages in memory changes when a message is deleted.  This is
         * why we need a MSGLOOP here.
         */

        if (element->synapse == NULL)  /* There is no synapse array. */
            return 1;

        i = 0;

        MSGLOOP(element, msg)
        {
        case SPIKE:
            element->synapse[i++].mi = msg;
            break;

        default:
            break;
        }

        break; /* MSGINDELETED */


    case DELETE:
        FreeSynapticEventLists(element);

        if (element->synapse != NULL)
        {
            free(element->synapse);
            element->synapse   = NULL;
            element->nsynapses = 0;
        }

        break; /* DELETE */


    case COPY:
        ERROR_IF (element->synapse != NULL,
                  "%s object %s: COPY:\n\tcannot copy synchan-type objects with SPIKE messages in current implementation.\n\tPlease add SPIKE messages after copying synchan-type objects, not before!\n",
                  MYNAME, MYPATH);

        break; /* COPY */


    case SAVE2:
        /*
         * Save Gk, X, Y and pending event data.
         */

        savedata[0] = element->Gk;
        savedata[1] = element->X;
        savedata[2] = element->Y;

        /* 
         * CHECKME: this looks very dangerous; what does `n' stand for?
         * The number of doubles?  Pointers?  I suspect there's a word-size
         * dependency problem lurking here...
         */

        n = 3 + SaveSizeSynapticEvents(element);

        fwrite(&n,       sizeof(int),    1, (FILE *)action->data);
        fwrite(savedata, sizeof(double), 3, (FILE *)action->data);

        SaveSynapticEvents(element, (FILE *)action->data);

        /*
         * Save number of synapses, for error-checking on a RESTORE2
         * action.
         */

        fwrite(&(element->nsynapses), sizeof(short), 1, 
               (FILE *)action->data);

        /*
         * Save facilitation/depression levels for all synapses.
         */

        for (i = 0; i < element->nsynapses; i++)
        {
            fwrite(&(element->synapse[i].fac), sizeof(float), 1, 
               (FILE *)action->data);            
            fwrite(&(element->synapse[i].depr), sizeof(float), 1, 
               (FILE *)action->data);            
        }

        break; /* SAVE2 */


    case RESTORE2:
        n = 0;

        ERROR_IF ((fread(&n, sizeof(int), 1, (FILE *)action->data) != 1)
                  || (n < 4),
                  "%s object %s: RESTORE2: invalid savedata length.\n",
                  MYNAME, MYPATH);

        nr = fread(savedata, sizeof(double), 3, (FILE *)action->data);

        if (nr != 3)
        {
            Error();
            printf("%s object %s: RESTORE2: invalid savedata.\n",
                   MYNAME, MYPATH);
            return n - nr;
        }

        element->Gk = savedata[0];
        element->X  = savedata[1];
        element->Y  = savedata[2];
        n -= 3;

        if (RestoreSynapticEvents(element, &n, (FILE *)action->data) == 0)
            return n;

        nr = fread(&nsynapses, sizeof(short), 1, (FILE *)action->data);

        if ((nr != 1) || (nsynapses != element->nsynapses))
        {
            Error();
            printf("%s object %s: RESTORE2: invalid number of synapses.\n",
                   MYNAME, MYPATH);
            return 1;  /* Return anything but zero... */
        }

        /*
         * Restore facilitation/depression levels for all synapses.
         */

        for (i = 0; i < element->nsynapses; i++)
        {
            fread(&(element->synapse[i].fac), sizeof(float), 1, 
               (FILE *)action->data);            
            fread(&(element->synapse[i].depr), sizeof(float), 1, 
               (FILE *)action->data);            
        }

        return 0; /* RESTORE2 */


    case SET:
        return Synchan_SET("facsynchan", (struct Synchan_type *) element, action);
	/*NOTREACHED*/
	break;


    case SHOW:
	return Synchan_SHOW("facsynchan", (struct Synchan_type *) element, action);
	/*NOTREACHED*/
	break;


#ifdef HAVE_NEW_CELLREADER
    case READCELL:
        return (Cellreader_read_channel(action,
                                        &(element->gmax),
                                        FLOAT,
                                        Pathname(element),
                                        1));

        /*NOTREACHED*/
        break;
#endif

    default:
        ERROR3 ("%s object %s: invalid action %d.\n",
               MYNAME, MYPATH, action->type);
        /*NOTREACHED*/
        break;
    }

    return 1;
}

