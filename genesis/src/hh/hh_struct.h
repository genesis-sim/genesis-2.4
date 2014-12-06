/*
** $Id: hh_struct.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
** $Log: hh_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1998/06/30 21:04:58  dhb
** Moved define for HH_CHAN_TYPE to hh_struct_defs.h and in clude that
** here.  Avoids multiple includes of sim headers when including hh_struct.h
** in olf.
**
** Revision 1.2  1998/04/29 12:55:05  dhb
** Pulled HH specific parameter fields out into a define so they
** could be used in the tabchannel version of hh_channel.
**
** Revision 1.1  1992/12/11 19:03:03  dhb
** Initial revision
**
*/

#include "struct_defs.h"
#include "hh_struct_defs.h"

struct vdep_gate_type {
    SEGMENT_TYPE
    double	m;
    float   	alpha_A;
    float   	alpha_B;
    float   	alpha_C;
    float   	alpha_D;
    float   	alpha_F;
    float   	beta_A;
    float   	beta_B;
    float   	beta_C;
    float   	beta_D;
    float   	beta_F;
    short	instantaneous;
};

struct vdep_channel_type {
    CHAN_TYPE
	/*
    double	Gk;
    float	Ik; Note that this is changed to a double and repositioned
    float	Ek;
	*/
    float	gbar;
};

struct hh_channel_type {
    CHAN_TYPE
	/*
    double 	Gk;
    float 	Ik; Note that this is changed to a double and repositioned
    float 	Ek;
	*/
    float   	Gbar;
    double	X;
    double 	Y;
    float   	Xpower;
    float	Ypower;
    HH_CHAN_TYPE
};

