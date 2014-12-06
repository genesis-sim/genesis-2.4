static char rcsid[] = "$Id: calculator.c,v 1.2 2005/06/27 19:00:35 svitak Exp $";

/* Version EDS22d 98/06/16, Erik De Schutter, BBF-UIA 6/98-6/98 */

/*
** Performs simple calculations on input from messages.  Allows summation
** over time intervals.
**
** $Log: calculator.c,v $
** Revision 1.2  2005/06/27 19:00:35  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1998/06/30 22:09:30  dhb
** Initial revision
**
*/

#include "dev_ext.h"

void CalculatorFunc(calc,action)
register struct calc_type *calc;
Action		*action;
{
MsgIn	*msg;
double	output;
extern   short   clock_active[NCLOCKS];

    if(debug > 1){
	ActionHeader("Calculator",calc,action);
    }

    SELECT_ACTION(action){
		case PROCESS:
			output=calc->next_init;

			/* compute output */
			MSGLOOP(calc,msg) {
				case 0:				/* SUM */
					output += MSGVALUE(msg,0);
					break;
				case 1:				/* SUBTRACT */
					output -= MSGVALUE(msg,0);
					break;
				case 2:				/* MULTIPLY */
					output *= MSGVALUE(msg,0);
					break;
				case 3:				/* DIVIDE */
					output /= MSGVALUE(msg,0);
					break;
			}

			calc->output=output;
			if (clock_active[calc->resetclock]) {
				calc->next_init=calc->output_init;
			} else {
				calc->next_init=calc->output;
			}
			break;

		case RESET:
			calc->next_init=calc->output_init;
			calc->output=calc->output_init;
			break;

	}
}
