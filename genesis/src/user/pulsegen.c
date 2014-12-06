static char rcsid[] = "$Id: pulsegen.c,v 1.3 2005/07/20 20:02:04 svitak Exp $";

/*
** $Log: pulsegen.c,v $
** Revision 1.3  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:23  dhb
** Initial revision
**
*/

#include <math.h>
#include "user_ext.h"
/*
************************************************************
**                     Pulse Generator
**
**  This device can generate a variety of pulse patterns:
**  single pulses, double pulses (each with its own level 
**  and width), and pulse trains (of single pulses or pairs).
**  It can be triggered, gated, or allowed to free run.
**
**  ADJUSTABLE PARAMETERS:	
**	level1		level of pulse1 
**	width1		width of pulse1 
**	delay1		delay of pulse1 
**			(wrt ext. input - trig or gate mode)
**                      (wrt pulse2     - free run mode)
**	level2		level of pulse2 
**	width2		width of pulse2 
**	delay2          delay of pulse2 
**			(wrt pulse1) 
**	baselevel	baseline level
**
**  TRIGGER MODES: 	
**	trig_mode = 0	free run
**	trig_mode = 1	ext. trig
**	trig_mode = 2	ext. gate
**     
**  INPUT MESSAGES:   NSLOTS       
**	TRIG  (0)	1	trig/gate (0 = low, non-0 = high)
**	LEVEL (1)       2	pulse# level 
**	WIDTH (2)	2	pulse# width
**	DELAY (3)	2	pulse# delay 
**
**  OUTPUT:
**	output
** 
**  INTERNAL STATE VARIABLE:
**	previous_input 
**	trig_time;
**
** M. Nelson Caltech 4/89 
************************************************************
*/

#define FREE_RUN	0
#define EXT_TRIG	1
#define EXT_GATE	2

void PulseGen(p,action)
register struct pulsegen_type *p;
Action		*action;
{
MsgIn	*msg;
int	input, pulse_num;
float   tcycle, tt = 0.0;

    if(debug > 1){
	ActionHeader("PulseGen",p,action);
    }

    SELECT_ACTION(action){
    case PROCESS:

	if(p->width1 > (p->delay2 + p->width2))
	    tcycle = p->delay1 + p->width1;
	else
	    tcycle = p->delay1 +  p->delay2 + p->width2;

	input = 0;
	MSGLOOP(p,msg) {
	    case 0: 	/* INPUT */
		if(MSGVALUE(msg,0) != 0) input = 1;
		break;
	    case 1: 	/* LEVEL */
		pulse_num = MSGVALUE(msg,0);
		if(pulse_num < 1 || pulse_num > 2){
		    ErrorMessage("PulseGen","Invalid pulse number in LEVEL msg sent to",p);
		}
		else{
		    if(pulse_num == 1)p->level1 = MSGVALUE(msg,1);
		    else              p->level2 = MSGVALUE(msg,1);
		}
		break;
	    case 2: 	/* WIDTH */
		pulse_num = MSGVALUE(msg,0);
		if(pulse_num < 1 || pulse_num > 2){
		    ErrorMessage("PulseGen","Invalid pulse number in WIDTH msg sent to",p);
		}
		else{
		    if(pulse_num == 1)p->width1 = MSGVALUE(msg,1);
		    else              p->width2 = MSGVALUE(msg,1);
		}
		break;
	    case 3:	/* DELAY */ 
		pulse_num = MSGVALUE(msg,0);
		if(pulse_num < 1 || pulse_num > 2){
		    ErrorMessage("PulseGen","Invalid pulse number in DELAY msg sent to",p);
		}
		else{
		    if(pulse_num == 1)p->delay1 = MSGVALUE(msg,1);
		    else              p->delay2 = MSGVALUE(msg,1);
		}
		break;
	}

	switch(p->trig_mode) {
	    case FREE_RUN:
		tt = fmod(simulation_time,tcycle);
		break;

	    case EXT_GATE:
		if(input == 0){			/* gate low */
		    tt = tcycle;		/* output = baselevel */
		}
		else{				/* gate high */ 
		    if(p->previous_input == 0){	/* low -> high */
			p->trig_time = simulation_time;
		    }
		    tt = fmod(simulation_time - p->trig_time,tcycle);
		}
		p->previous_input = input;
		break;

	    case EXT_TRIG:
		if(input == 0){		 	/* trig low */
		    if(p->trig_time < 0){ 	/* never triggered */
			tt = tcycle;	 	/* output = baselevel */
		    }
		    else{
			tt = simulation_time - p->trig_time;
		    }
		}
		else{			 	/* trig high */ 
		    if(p->previous_input == 0){	/* low -> high */
			p->trig_time = simulation_time;
		    }
		    /* don't use fmod here */
		    tt = simulation_time - p->trig_time;
		}
		p->previous_input = input;
		break;
	}

						/* set output level */
	if(tt < p->delay1 || tt >= tcycle)
	    p->output = p->baselevel;
	else if(tt < p->delay1 + p->width1)
	    p->output = p->level1;
	else if(tt < p->delay1 + p->delay2)
	    p->output = p->baselevel;
	else 
	    p->output = p->level2;

	break;

    case RESET:
	p->previous_input = 0;
	p->trig_time = -1;
	p->output = p->baselevel;
	break;
    }
}
