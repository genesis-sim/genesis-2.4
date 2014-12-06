/*
** $Id: syn_struct.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
** $Log: syn_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:23  dhb
** Initial revision
**
*/

#include "struct_defs.h"

/*
*******************************
**       CONNECTION          **
*******************************
*/
struct connection_synapse_type {
    CONNECTION_TYPE
    short		plastic_weight;	
    short		base_weight;
    unsigned char	transient_weight;
    unsigned char	state_age;
    short		state;
    short		activity_level;
};

struct synapseA_type {
    CONNECTION_TYPE
    float	time;
    float	state;
};

struct synapseB_type {
    CONNECTION_TYPE
    float	time;
    float	state;
    float	rate;
    float	scale;
    float	maxweight;
};

struct synapseB1_type {
    CONNECTION_TYPE
    float	state;
    float	rate;
    float	scale;
    float	maxweight;
};

struct synapseC_type {
    CONNECTION_TYPE
    float	time;
    float	state;
    float	rate;
    float	scale;
    float	maxweight;
    float	orig_gmax;
};

struct rel_connection_synapse_type {
	short			base_weight;
};
