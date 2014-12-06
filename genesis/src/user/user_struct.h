/*
** $Id: user_struct.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
** $Log: user_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:25  dhb
** Initial revision
**
*/

#include "struct_defs.h"

struct pulsegen_type {
    ELEMENT_TYPE
    double 	output;
    float	level1;
    float	width1;
    float	delay1;
    float	level2;
    float	width2;
    float	delay2;
    float	baselevel;
    float	trig_time;
    short 	trig_mode;
    short	previous_input;
};

struct PID_type {
    ELEMENT_TYPE
    double	cmd;
    double	sns;
    double	output;
    float	gain;
    float	tau_i;
    float	tau_d;
    float	saturation;
    double	e;
    double	e_integral;
    double	e_deriv;
    double	e_previous;
};
