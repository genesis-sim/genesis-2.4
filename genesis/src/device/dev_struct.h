/*
** $Id: dev_struct.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: dev_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1998/06/30 22:14:21  dhb
** EDS 22d merge
**
** Revision 1.2  1998/04/01 00:01:59  dhb
** Incorporated spike analysis library by Dieter Jaeger.  Added
** include of spike_struct.h.
**
** Revision 1.1.1.1  1998/06/30 22:12:05  dhb
** EDS 22d
**
 * EDS22d revison: EDS BBF-UIA 98/06/16-98/06/16
 * Added calculator
 *
** Revision 1.1  1992/12/11 19:02:54  dhb
** Initial revision
**
*/

#include "spike_struct.h"

struct efield_type {
    ELEMENT_TYPE
    double	field;
    float	scale;
};

struct expthresh_type {
    ELEMENT_TYPE
    float	theta_s;
    float	theta_0;
    float	tau_theta;
    float	state;
};

struct diffamp_type {
    ELEMENT_TYPE
    float	plus;
    float	minus;
    float	gain;
    float	saturation;
    double	output;
};

struct funcgen_type {
    ELEMENT_TYPE
    short	mode;
    float	amplitude;
    float	frequency;
    float	phase;
    float	dc_offset;
    double	output;
};

struct freq_mon_type {
    BUFFER_TYPE
    float	exponent;
    float	frequency;
};

struct injection_type {
    ELEMENT_TYPE
    double	i;
    float	Imax;
    float	Tmin;
    float	Tmax;
    float   	Period;
    float   	Width;
    short   	inj_method;
};

struct nernst_type {
    ELEMENT_TYPE
    float	constant;
    float	E;
    float	T;
    short	valency;
    float	Cin;
    float	Cout;
    float	scale;
};

#define NO_CLEAR_BINS 0
#define CLEAR_BINS 1
#define NEW_TABLE 2
#define FREE_RUN 0
#define TRIGGERED 1
#define SPIKES 2

struct peristim_type {
    ELEMENT_TYPE
	int 	output;
	float   threshold;
	float	binwidth;
	int		num_bins;
	short   trigger_mode; /* FREE_RUN or TRIGGERED or SPIKES*/
	short   reset_mode;  /* NO_CLEAR_BINS or CLEAR_BINS */
	int     *table;
	int		bin_index;
	int     bin_steps;
	short   spiking;
	short   allocated;
};

struct interspike_type {
    ELEMENT_TYPE
	double	output;
	float   threshold;
	float	binwidth;
	int		num_bins;
	short   trigger_mode; /* FREE_RUN or TRIGGERED or SPIKES*/
	short   reset_mode;  /* NO_CLEAR_BINS or CLEAR_BINS */
	int     *table;
	float  prev_time;
	short   triggered;
	short	spiking;
	short   allocated;
};

struct new_frequency_type {
	ELEMENT_TYPE
	float 	frequency;
	float   threshold;
	float	num_spikes;
	double	prev_time;
	short   spiking;
};

struct calc_type {
    ELEMENT_TYPE
    double	output;
    int		resetclock;
	float	output_init;
	double	next_init;
};
