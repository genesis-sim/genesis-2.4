/*
 *
 * FILE: fac_struct.h
 *
 *      Struct definitions for the fac library.
 *
 *
 * AUTHOR: Mike Vanier
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

#ifndef FAC_STRUCT_H
#define FAC_STRUCT_H

#include "struct_defs.h"

/*
 * This macro is copied from struct_defs.h
 */

#define MY_CHANNELC2_TYPE  \
    double  X;             \
    double  Y;             \
    float   tau1;          \
    float   tau2;          \
    float   gmax;          \
    double  xconst1;       \
    double  xconst2;       \
    double  yconst1;       \
    double  yconst2;       \
    float   norm;

/*
 * These macros are copied from newconn_struct.h and will have to be recopied
 * every time someone changes these definitions. 
 */

#define MY_SYNCHAN_TYPE                      \
    CHAN_TYPE                                \
    MY_CHANNELC2_TYPE                        \
    float       frequency;                   \
    short       nsynapses;                   \
    short       normalize_weights;           \
    double      time_last_event;             \
    Element    *hsolve;                      \
    int         solve_index;                 \
    unsigned short synapse_size;             \
    int         event_buffer_size;           \
    int         pending_events;              \
    SynapticEventPtr FreeSynapticEvents;     \
    SynapticEventPtr PendingSynapticEvents;  \
    float       nodes_per_synapse;           \
    short       list_alloced;                \
    short       allocednodes;       

#define MY_SYNAPSE_TYPE  \
    MsgIn*  mi;          \
    float   weight;      \
    float   delay;  


/*
 * The `facsynchan' object.
 *
 *     This object is essentially identical to the standard `synchan' object
 *     except that it contains a simple model for synaptic facilitation
 *     and/or depression.
 *
 */

struct Fac_Synapse_type
{
    MY_SYNAPSE_TYPE
    float fac;      /* Synaptic facilitation state variable. */
    float depr;     /* Synaptic depression state variable.   */
};

typedef struct Fac_Synapse_type Fac_Synapse;


struct fac_synchan_type
{
    MY_SYNCHAN_TYPE
    struct Fac_Synapse_type *synapse;  /* Synapse buffer. */

    /*
     * Facilitation/depression-related fields.
     *
     * NOTE:  I use floats here because I usually need to save space, and
     *        high accuracy is not that critical.  I use floats for the 
     *        internal state variables kept in the synapses for the same 
     *        reason; there, the savings will be even greater.
     *
     */

    short      fac_depr_on;     /* Flag: no fac/depr if 0.                  */
    float      max_fac;         /* Upper limit on facilitation.            */
    float      fac_per_spike;   /* Synaptic facilitation per spike.        */
    float      fac_tau;         /* Time constant of synaptic facilitation. */
    float      fac_const;       /* = exp(-dt/fac_tau).                     */
    float      depr_per_spike;  /* Synaptic depression per spike.          */
    float      depr_tau;        /* Time constant of synaptic depression.   */
    float      depr_const;      /* = exp(-dt/depr_tau).                    */
};


#endif  /* FAC_STRUCT_H */
