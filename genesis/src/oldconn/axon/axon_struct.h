/*
** $Id: axon_struct.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
** $Log: axon_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:30  dhb
** Initial revision
**
*/

#include "struct_defs.h"

struct delay_projection_type {
    PROJECTION_TYPE
    float	tmax; 
    float	duration; 
    float	latency; 
};

struct modifiability_type {
    PROJECTION_TYPE
    float	plasticity;
    float	sensitivity;
    short	modify;
};

struct axon_type {
    PROJECTION_TYPE
    float	*Ctable;
    float	G_amp;
    float	gamma;
    PFF		conduct_func;
    PFI		connect_func;
    PFI		plastic_weight_func;
    PFI		base_weight_func;
    PFI		modify_func;
};

