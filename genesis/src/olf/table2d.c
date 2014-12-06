static char rcsid[] = "$Id: table2d.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/* Version EDS20i 95/06/08 */

/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/*
** This file has been modified from its original form as indicated below.
** The original version lives in the kinetics library, though it is not
** compiled into genesis.  ---dhb
*/

/*
** $Log: table2d.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.3  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.2  1997/07/29 14:40:56  dhb
** Added copyright notice referencing LGPL.
**
** Revision 1.1  1997/05/29 08:21:37  dhb
** Initial revision
**
*/

/* Code written by Upi Bhalla */

#include "olf_ext.h"
#include "olf_defs.h"

/*
** Does a table2d lookup with interpolation. Also permits one to modify
** the table2d with sum and product messages, so as to extend the
** dimensionality of the table2d. Most unlikely to ever use that.
*/

int Table2DFunc(table2d,action)
register struct table2d_type *table2d;
Action		*action;
{
MsgIn	*msg;
double	sy,py;
int		xdivs,ydivs;
double	xmin,xmax,ymin,ymax;
Interpol2D *create_interpol2d();

    if(debug > 1){
	ActionHeader("VDepGate",table2d,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
        		/*
        		** check all of the messages to the table2d
        		*/
				sy = 0;
				py = 1;
        		MSGLOOP(table2d,msg) {
            		case 0:				/* index */
	        			table2d->input = MSGVALUE(msg,0);
   		     			table2d->input2 = MSGVALUE(msg,1);
        			break;
					case 1:				/* summed y */
						sy += MSGVALUE(msg,0);
					break;
					case 2:				/* product y */
						py *= MSGVALUE(msg,0);
					break;
					case 3:				/* x input alone */
        				table2d->input = MSGVALUE(msg,0);
					break;
					case 4:				/* y input alone */
        				table2d->input2 = MSGVALUE(msg,0);
					break;
        		}
				if (table2d->alloced) {
        			table2d->output =
						Tab2DInterp(table2d->table,
							table2d->input,table2d->input2) * py + sy;
				} else {
					table2d->output = py + sy;
				}
				table2d->negoutput = -table2d->output;
		break;
    case RESET:
        table2d->output = 0;
        break;
    case SET :
        if (action->argc != 2)
            return(0); /* do the normal set */
        if (strncmp(action->argv[0],"table",5) == 0)
            scale_table2d(table2d->table,action->argv[0] + 7,
				action->argv[1]);
        return(0); /* do the normal set */


    case TABCREATE:
		if (action->argc < 3) {
			printf("usage: TABCREATE xdivs xmin xmax [ydivs ymin ymax]\n");
			return(0);
		}
		ydivs = xdivs = atoi(action->argv[0]);
		ymin = xmin = Atod(action->argv[1]);
		ymax = xmax = Atod(action->argv[2]);
		if (action->argc == 6) {
			ydivs = atoi(action->argv[3]);
			ymin = Atod(action->argv[4]);
			ymax = Atod(action->argv[5]);
		}
		table2d->table = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		table2d->alloced = 1;
        break;
	}
	return(0);
}
