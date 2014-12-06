/*
** $Id: seg_struct.h,v 1.2 2006/03/07 22:35:31 svitak Exp $
** $Log: seg_struct.h,v $
** Revision 1.2  2006/03/07 22:35:31  svitak
** Added izcell object to implement Izhikevich simple spiking cell model.
**
**
** Revision 2006/03/02 dbeeman
** Added izcell structure to implement Izhikevich simple spiking cell model
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1997/05/29 09:02:14  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21d revision: EDS BBF-UIA 97/01/28-97/01/28
 * Converted compartment to GSEGMENT
 *
 * Version EDS20l 95/12/19, Erik De Schutter, BBF-UIA 6/94-12/95
 * Introduced CHANNELC2_TYPE definition
 *
 * EDS20i revison: EDS BBF-UIA 95/06/08
 * (Fused Ca_pool and Ca_shell using 'thick' field name)
 * Added ghk_type
**
** Revision 1.4  1995/07/18  18:10:45  dhb
** Added thick field to Ca_concen_type and removed Ca_shell_type
** which is now completely implemented in Ca_concen object.
 *
** Revision 1.3  1994/06/02  17:27:10  dhb
** Added initVm field to compartments and symcompartments.
**
** Revision 1.2  1994/05/27  21:40:14  dhb
** Added a len field to the compartment and symcompartment objects.
**
** Revision 1.1  1992/12/11  19:04:17  dhb
** Initial revision
**
*/

#include "struct_defs.h"

/*
*******************************
**         SEGMENT           **
*******************************
*/

struct unit_type {
    SEGMENT_TYPE
    double	state;
    float	Rm;
    float	Cm;
    float	Em;
    float	inject;
};

struct RCunit_type {
    SEGMENT_TYPE
    double	state;
    float	R;
    float	C;
    float	V0;
    float	inject;
};

struct site_type {
    SEGMENT_TYPE
    double	Ik;
};

struct leakage_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
    float 	inject;
};

struct channelA_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
};

struct channelB_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
    double	Ak;
    double	Yk;
    float	tau_ak;
    float	tau_gk;
    float	gain_ak;
    float	gain_gk;
};

struct conductanceA_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
    double	Ak;
    double	Yk;
    float	tau1;
    float	tau2;
    float	gmax;
};

struct channelC_type {
    SEGMENT_TYPE
    double	Gk;
    float	Ek;
    double	X;
    double	Y;
    float	tau1;
    float	tau2;
    float	gmax;
    double	xconst1;
    double	xconst2;
    double	yconst1;
    double	yconst2;
    float	norm;
};

struct channelC2_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
	CHANNELC2_TYPE
};

struct channelC3_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
	CHANNELC2_TYPE
	float	frequency;
};

struct manuelconduct_type {
    CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
    double	H;
    double	Z;
    float	tau_g;
    float	tau_h;
    float	tau_d;
    float	gmax;
    float	K_r;
};

#define CA_CONCEN_TYPE \
    SEGMENT_TYPE \
    double  Ca; \
    double  C; \
    double  Ca_base; \
    float   tau; \
    float   B; \
    float   thick;

struct Ca_concen_type {
    CA_CONCEN_TYPE
};

struct Mg_block_type {
	CHAN_TYPE
	/*
    double	Ik;
    double	Gk;
    float	Ek;
	*/
	double  Zk;
	float	KMg_A;
	float	KMg_B;
	float	CMg;
};

struct membrane_type {
    float	Cm;
    float	Rm;
    float	Em;
    float	pot;
    float	inject;
    short	spikes;
};

#define COMPARTMENT_TYPE \
    GSEGMENT_TYPE \
    double	Vm; \
    double	previous_state; \
    float	Im; \
    float	Em; \
    float	Rm; \
    float	Cm; \
    float	Ra; \
    float	inject; \
    float	dia; \
    float	len; \
    float	initVm;

struct compartment_type {
	COMPARTMENT_TYPE
};

struct symcompartment_type {
	COMPARTMENT_TYPE
    float	coeff;
    float	coeff2;
};

struct ghk_type {
    CHAN_TYPE
    float   T;       /* temperature, deg C */
    double  p;       /* permeability */
    double  Vm;      /* membrane potential */
    float   Cin;     /* concentration inside cell */
    float   Cout;    /* concentration outside cell */
    short   valency; /* valence of ion */
    float   constant;
};

struct izcell_type {
    ELEMENT_TYPE
    double      Vm;
    double      u;
    float       A;
    float       B;
    float       C;
    float       D;
    float       Vmax;
    float       coeff0;
    float       coeff1;
    float       coeff2;
    float       inject;
    float       state;
};
