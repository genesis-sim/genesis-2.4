/*
** $Id: buf_struct.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: buf_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:36  dhb
** Initial revision
**
*/

#include "struct_defs.h"

/*
*******************************
**         EVENT             **
*******************************
*/
struct variable_type {
	float 	value;
};

/*
*******************************
**         BUFFER            **
*******************************
*/

struct random_type {
    BUFFER_TYPE
    float	state;
    float	rate;
    float	max_amp;
    float	min_amp;
    short	reset;
    float	reset_value;
};

struct playback_type {
    BUFFER_TYPE
    float	state;
};

struct periodic_type {
    BUFFER_TYPE
    float	state;
    float	stime;
    float	scale;
    short	mode;
};

struct spike_type {
    BUFFER_TYPE
    float	state;
    float 	thresh;
    float 	abs_refract;
    float 	output_amp;
};

struct state_type {
    BUFFER_TYPE
    float	state;
};

struct graded_type {
    BUFFER_TYPE
    float	state;
    float 	baseline;
    float 	scale;
    float 	rectify;
};

struct sigmoid_type {
    BUFFER_TYPE
    float	input;
    float	state;
    float 	gain;
    float 	amplitude;
    float 	thresh;
};

struct linear_type {
    BUFFER_TYPE
    float	state;
    float 	gain;
    float 	thresh;
};

struct inject_type {
    BUFFER_TYPE
    float 	inject;
};

/* New buffer type for use with integration methods (BPG 15-5-91) */
struct passive_type {
    BUFFER_TYPE
};
