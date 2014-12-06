/* $Id: newconn_struct.h,v 1.3 2006/01/09 16:29:34 svitak Exp $
** $Log: newconn_struct.h,v $
** Revision 1.3  2006/01/09 16:29:34  svitak
** Added leech channels.
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
** Revision 1.8  1999/11/27 08:44:55  mhucka
** Added M. Vanier's facsynchan object.
**
** Revision 1.7  1997/08/01 18:57:44  dhb
** Fix to randomspike to adjust for the rate damping effect of
** the refractory period.  (From Erik De Schutter)
**
** Revision 1.6  1997/05/29 07:00:11  dhb
** Update from Antwerp GENESIS version 21e
**
 * Version EDS20l5 95/12/14, Erik De Schutter, BBF-UIA 6/94-1/97
 * Added communication with hines solver through HPROCESS and HSEVENT calls
 *
 * Modified MAEX 96/4/16, Reinoud Maex, BBF-UIA
 * Added a short field "normalize_weights" to SYNCHAN_TYPE. When set to zero
 * nothing changes. When set to 1, is divided (normalized) by nsynapses.
 *
 * Revision 1.5  1996/07/22  23:56:20  dhb
 * Merged in 1.3.1.1 changes
 *
 * Revision 1.4  1995/09/28  00:02:59  venkat
 * Changes from Mike Vanier:
 * Removed references to hebbsynchan2.
 *
 * Revision 1.3.1.1  1996/07/22  23:54:40  dhb
 * Changes from Make Vanier:
 *   Synchan2 --> synchan
 *   Generic handling of synchan types
 *
 * Revision 1.3  1995/03/27  20:26:40  mvanier
 * Changed the order of some fields in synchan and synchan2
 * to make the synaptic utilities work.
 *
 * Revision 1.2  1995/01/16  22:14:37  dhb
 * The time, prev and next fields in the Synapse structure were all
 * typed as unsigned.  These fields can have negative values so must
 * not be unsigned.
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#ifndef NEWCONN_STRUCT_H
#define NEWCONN_STRUCT_H

#include "struct_defs.h"

#define HPROCESS 250
#define HSEVENT  251

/* 
 * The following types and #defines define mask types 
 * used by planar_connect and volume_connect.
 */

#define CONNECTION_MASK       1 
#define CONNECTION_MASK_HOLE -1 

#define CONNECTION_MASK_BOX_SHAPE       0
#define CONNECTION_MASK_ELLIPSE_SHAPE   1
#define CONNECTION_MASK_ELLIPSOID_SHAPE 2


struct extended_mask_type 
{ 
  short		type;
  short               shape;
  double 		x1, y1, z1, x2, y2, z2; 
}; 


/*
 * The following types are spike-generating objects.
 */


struct Spikegen_type 
{
  ELEMENT_TYPE
  float	state;
  float thresh;
  float output_amp;
  float abs_refract; /* refractory period  */
  float	lastevent;   /* time of last spike */
};


struct Randomspike_type 
{
  ELEMENT_TYPE 
  float	state;
  float	rate;
  short	reset;
  float	reset_value;
  float	min_amp;
  float	max_amp;
  float abs_refract;  
  float lastevent;    
  float	realrate;
};


/*
 * The following types define various objects related to synapses
 * and synaptically-activated channels.
 */

/* For synchan: */

#define SYNCHAN_TYPE                         \
    CHAN_TYPE                                \
    CHANNELC2_TYPE                           \
    float       frequency;                   \
    short	nsynapses;                   \
    short   normalize_weights;               \
    double  time_last_event;                 \
    Element *hsolve;                         \
    int     solve_index;                     \
    unsigned short synapse_size;             \
    int         event_buffer_size;           \
    int         pending_events;              \
    SynapticEventPtr FreeSynapticEvents;     \
    SynapticEventPtr PendingSynapticEvents;  \
    float       nodes_per_synapse;           \
    short       list_alloced;                \
    short       allocednodes;       

/* New synapse fields added in GENESIS 2.4 (Hugo Cornelis and Dave Beeman)

   last_spike_time is a copy of ((struct Spikegen_type *) mi->src)->lastevent
   giving the the time of arrival (including delay) of the presynaptic spike.
   The Aplus and Aminus fields are available for storage of synaptic weight
   changes when plasticity functions are applied to a synchan synapse, but
   are not used by the normal synchan PROCESS action.

   lastupdate is used by the stdp_update.c code to keep track of when
   the synapse weight, Aplus, and Aminus fields were last updated.
   It is also not modified by the normal synchan PROCESS action.
*/

#define SYNAPSE_TYPE     \
    MsgIn*	mi;      \
    float	weight;  \
    float	delay;   \
    float 	last_spike_time; \
    float	Aplus; \
    float	Aminus; \
    float       lastupdate;

struct Synapse_type 
{
  SYNAPSE_TYPE
};

typedef struct Synapse_type Synapse;

typedef struct SynapticEvent_type* SynapticEventPtr;

struct Synchan_type 
{
    SYNCHAN_TYPE
    struct Synapse_type *synapse;
};



/* For hebbsynchan: */

struct HebbSynapse_type 
{
  SYNAPSE_TYPE
  double         X;
  double         Y;
  float          pre_activity;
  unsigned short spike_flags;
};
 
typedef struct HebbSynapse_type HebbSynapse;
 
struct HebbSynchan_type 
{
  SYNCHAN_TYPE
  struct HebbSynapse_type *synapse;
  /* The following fields are specific to the Hebbian synchans: */
  /* 1) presynaptic fields: */
  float       pre_tau1;  
  float       pre_tau2;  
  float       pre_thresh_lo; 
  float       pre_thresh_hi; 
  double      pre_xconst;
  double      pre_yconst1;
  double      pre_yconst2;
  float       pre_norm;
  /* 2) postsynaptic fields: */
  float       avg_Vm; 
  float       post_tau; 
  double      post_const;
  float       post_thresh_lo; 
  float       post_thresh_hi;
  float       post_scale;          /* scaling factor for postsynaptic activities */
  /* 3) other fields: */
  float       weight_change_rate; 
  float       min_weight;
  float       max_weight;
  short       change_weights;      /* flag: nonzero means weights can be changed */
};

#include "fac_struct.h" /* for facsynchan */

#include "SynGS_struct.h"

#include "stdp_rules_struct.h"

#endif
