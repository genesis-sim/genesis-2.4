static char rcsid[] = "$Id: funcgen.c,v 1.3 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: funcgen.c,v $
** Revision 1.3  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:35  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:49  dhb
** Initial revision
**
*/

#include <math.h>
#include "dev_ext.h"

/*
** waveform generator 
*/
/* M.Wilson Caltech 1/89 */
void FunctionGenerator(gen,action)
register struct funcgen_type *gen;
Action		*action;
{
MsgIn	*msg;
float	amplitude;
float	dc_offset;
float	phase;
float	frequency;
float	cycles;

    if(debug > 1){
	ActionHeader("FunctionGenerator",gen,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
	amplitude = gen->amplitude;
	dc_offset = gen->dc_offset;
	phase = gen->phase;
	frequency = gen->frequency;
	MSGLOOP(gen,msg) {
	    case 0:				/* gain */
		amplitude = MSGVALUE(msg,0);
		break;
	    case 1:				/* dc offset */
		dc_offset = MSGVALUE(msg,0);
		break;
	    case 2:				/* phase (degrees) */
		phase = MSGVALUE(msg,0);
		break;
	    case 3:				/* frequency (Hz) */
		frequency = MSGVALUE(msg,0);
		break;
	}
	switch(gen->mode){
	case 0:			/* sine */
	    gen->output = amplitude * 0.5*sin(2*M_PI*(simulation_time*
	    frequency + phase/360.0)) + dc_offset;
	    break;
	case 1:			/* square */
	    /*
	    ** calculate the number of half cycles
	    */
	    cycles = simulation_time*frequency*2 + phase/180.0;
	    /*
	    ** determine whether it is on an even or odd half cycle
	    */
	    if((int)(cycles)%2 == 0){
		/* even */
		gen->output = dc_offset + amplitude;
	    } else {
		/* odd */
		gen->output = dc_offset;
	    }
	    break; 
	case 2:			/* triangle */
	    /*
	    ** calculate the number of half cycles
	    */
	    cycles = simulation_time*frequency*2 + phase/180.0;
	    /*
	    ** determine whether it is on an even or odd half cycle
	    */
	    if((int)(cycles)%2 == 0){
		/* even */
		gen->output = dc_offset + amplitude*(cycles - (int)cycles);
	    } else {
		/* odd */
		gen->output = dc_offset + amplitude*(1-(cycles - (int)cycles));
	    }
	    break; 
	case 3:			/* constant */
	    gen->output = amplitude;
	    break;
	}
	break;
    case CHECK:
	frequency = gen->frequency;
	MSGLOOP(gen,msg) {
	    case 3:				/* frequency (Hz) */
		frequency = MSGVALUE(msg,0);
		break;
	}
	switch(gen->mode){
	case 0:
	case 1:
	case 2:
	    if(frequency <= 0){
		ErrorMessage("FunctionGenerator", "Invalid frequency.",
		gen);
	    }
	    break;
	}
	break;
    case RESET:
	gen->output = 0;
	break;
    }
}
