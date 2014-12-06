static char rcsid[] = "$Id: diffamp.c,v 1.2 2005/06/27 19:00:35 svitak Exp $";

/*
** $Log: diffamp.c,v $
** Revision 1.2  2005/06/27 19:00:35  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:47  dhb
** Initial revision
**
*/

#include "dev_ext.h"

/*
** difference amplifier that takes in two inputs and produces
** a proportional output
*/
/* M.Wilson Caltech 1/89 */
void DifferenceAmp(amp,action)
register struct diffamp_type *amp;
Action		*action;
{
MsgIn	*msg;
double	plus;
double	minus;

    if(debug > 1){
	ActionHeader("DifferenceAmp",amp,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
	plus = amp->plus;
	minus = amp->minus;
	MSGLOOP(amp,msg) {
	    case 0:				/* + terminal */
		plus += MSGVALUE(msg,0);
		break;
	    case 1:				/* - terminal */
		minus += MSGVALUE(msg,0);
		break;
	    case 2:				/* gain */
		amp->gain = MSGVALUE(msg,0);
		break;
	}
	amp->output = amp->gain*(plus - minus);

	if(amp->output > amp->saturation)
	    amp->output = amp->saturation;
	else if(amp->output < -amp->saturation) 
	    amp->output = -amp->saturation;
	break;
    case RESET:
	amp->output = 0;
	break;
    }
}
