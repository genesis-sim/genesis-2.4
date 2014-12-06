static char rcsid[] = "$Id: table.c,v 1.3 2005/07/20 20:02:02 svitak Exp $";

/*
** $Log: table.c,v $
** Revision 1.3  2005/07/20 20:02:02  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/01/01 01:27:32  mhucka
** 1) Added a missing break statement, reported by Upi Bhalla.
** 2) Added minor cosmetic change by Chris Butson of moving declaration
**    of "double x".
**
** Revision 1.9  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.8  2000/06/12 04:47:15  mhucka
** 1) Fixed trivial errors in some printf format strings.
** 2) Added NOTREACHED comments where appropriate.
**
** Revision 1.7  2000/06/07 06:55:59  mhucka
** Changes from Upi Bhalla to add a TAB_DELAY step_mode in order to implement
** a delay line.
**
** Revision 1.6  1997/07/18 23:57:10  dhb
** Updated ShowInterpol() call to include new dimension argument.
**
** Revision 1.5  1997/05/29 08:37:36  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS20i revison: EDS BBF-UIA 95/06/08
 * Added olf_defs.h
 *
 * Revision 1.4  1995/09/26  22:03:45  dhb
 * Changes from Upi Bhalla to use the simulation step size if the table
 * stepsize field is zero.
 *
 * Revision 1.3  1995/01/07  01:20:59  venkat
 * changed nint() to the round macro for portability to Ultrix
 *
 * Revision 1.2  1994/08/08  22:32:33  dhb
 * Changes from Upi.
 *
 * Revision 1.4  1994/06/13  22:50:26  bhalla
 * incorporated the ShowInterpol for the SHOW action
 * Added the TABOP action
 * added the DUMP action
 *
 * Revision 1.3  1994/05/25  13:59:54  bhalla
 * Added TAB_FIELDS message for filling up fields and also for going
 * backwards to restore original values.
 * Added  TAB2FIELDS action for going backwards. This is  a somewhat dangerous
 * action.
 * Added TABOP action for doing assorted operations on table entries. This
 * is useful mainly for param searches.
 *
 * Revision 1.2  1994/05/09  20:08:23  bhalla
 * Corrected major omission: the TAB_BUF and TAB_SPIKE modes were missing
 *
 * Revision 1.1  1992/11/14  00:37:23  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "olf_ext.h"
#include "olf_defs.h"
/*
** Does a table lookup with interpolation. Also permits one to modify
** the table with sum and product messages, so as to extend the
** dimensionality of the table.
** Additional features:
**  Stepping through a table emitting values
**  Filling up the table with values via msgs
**  Filling up the table with spike times using an internal threshold
*/

int TableFunc(table,action)
register struct table_type *table;
Action		*action;
{
MsgIn	*msg;
double	sy,py;
int		xdivs;
double	xmin,xmax;
Interpol *create_interpol();
short	fill_mode;
double	temp;
double x;

    if(debug > 1){
	ActionHeader("VDepGate",table,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
		switch(table->step_mode){
			case TAB_IO :
        		/*
        		** check all of the messages to the table
        		*/
				sy = 0;
				py = 1;
        		MSGLOOP(table,msg) {
            		case 0:				/* index */
        			table->input = MSGVALUE(msg,0);
        			break;
					case 1:				/* summed y */
						sy += MSGVALUE(msg,0);
					break;
					case 2:				/* product y */
						py *= MSGVALUE(msg,0);
					break;
        		}
				if (table->alloced)
        			table->output =
						TabInterp(table->table,table->input) * py + sy;
				else
					table->output = py + sy;
        	break;
			case TAB_LOOP :
				/* ignore inputs, just step through table values */
				if (table->alloced) {
					if (table->stepsize == 0) {
						double looplen =
							table->table->xmax - table->table->xmin;
						temp = table->input + SimulationTime();
						temp = temp - looplen * ((int)(temp/looplen));
       					table->output =
							TabInterp(table->table,temp);
					} else {
						temp = table->input + table->stepsize; 
						if (temp > table->table->xmax)
							temp = table->table->xmin;
        					table->output =
								TabInterp(table->table,temp);
						table->input = temp;
					}
				} else {
					table->output = table->input = SimulationTime();
				}
			break;
			case TAB_ONCE :
				if (table->alloced) {
					if (table->stepsize == 0) {
        				table->output =
							TabInterp(table->table,
							table->input + SimulationTime());
					} else {
						table->input += table->stepsize;
        				table->output =
							TabInterp(table->table,table->input);
					}
				} else {
					table->output = table->input = SimulationTime();
				}
			break;
			case TAB_BUF :
				/* fills a table with values one by one */
				/* Output value is current sample number, can be set */
        		MSGLOOP(table,msg) {
            		case 0:				/* index */
        			table->input = MSGVALUE(msg,0);
					if (table->alloced) {
						if (table->output<table->table->xdivs &&
							round(table->output)>=0) {
							table->table->table[round(table->output)]=
								table->input;
							table->output+=1;
							table->table->xmax=table->output;
						}
					}
				}
			break;
 			case TAB_DELAY : {
 				/* Implements a delay line. Input is from a msg,
 				** Output value is delayed by the size 'xdivs' of
 				** the table. Unlike TAB_BUF, the current sample
 				** number is stored in 'input' rather than output.
 				*/
         		MSGLOOP(table,msg) {
             		case 0:
         			x = MSGVALUE(msg,0);
 					if (table->alloced) {
 						int i = round(table->input);
 						if (i < table->table->xdivs && i >= 0) {
 							table->output = table->table->table[i];
 							table->table->table[i++] = x;
 							table->input = (i >= table->table->xdivs) ?
 								0 : i;
 						}
 					}
 				}
 			}
			break;
			case TAB_SPIKE :
				/* fills a table with spike times, using a thresh */
				/* Output value is current sample number, can be set */
				if (table->alloced) {
        			MSGLOOP(table,msg) {
            			case 0:
						if((temp=MSGVALUE(msg,0)) > table->stepsize) {
							if (table->output<table->table->xdivs &&
								table->output>=0) {
								/* Check if new spike */
								if (table->input < table->stepsize) {
									table->table->table[round(table->output)]=
										SimulationTime();
									table->output+=1;
									table->table->xmax=table->output;
								}
							}
						}
						table->input = temp;
						break;
					}
				}
			break;
			case TAB_FIELDS:
				/* Fills a table with values from many incoming
				** messages. See the TAB2FIELDS action below
				*/
				if (table->alloced) {
					int i = 0;
					MSGLOOP(table,msg) {
						case 0:			/* just the value, no index */
							if (i < table->table->xdivs && i >=0) {
								table->table->table[i] =
									MSGVALUE(msg,0);
									i++;
							}
						break;
						case 3:			/* Value plus index */
							i = MSGVALUE(msg,1);
							if (i < table->table->xdivs && i >=0)
								table->table->table[i] =
									MSGVALUE(msg,0);
						break;
					}
				}
			break;
		}
		break;
    case RESET:
        table->output = 0;
		if (table->step_mode == TAB_FIELDS) {
			int i = 0;
			int j = 0;
			int maxj = 0;
			MSGLOOP(table,msg) {
				case 0:		/* just the value, no index */
						i++;
				break;
				case 3:			/* Value plus index */
					j = MSGVALUE(msg,1);
					if (j > maxj)
						j = maxj;
				break;
			}
			if (i > 0 && j > 0) {
				Warning();
				printf("In table %s with stepmode TAB_FIELDS, msgtypes 0 and 1 \nshould not be combined\n",Pathname(table));
			}
			if (table->alloced && (i > table->table->xdivs ||
				maxj > table->table->xdivs)) {
				Warning();
				printf("In table %s with stepmode TAB_FIELDS, \n input msg index %d exceeds allocated table size %d\n",Pathname(table),
					(i>j) ? i : j, table->table->xdivs);
			}
		}
        break;
    case SET :
        if (action->argc != 2)
            return(0); /* do the normal set */
		if (SetTable(table,2,action->argv,action->data,"table"))
			return(1);
        if (strncmp(action->argv[0],"table",5) == 0)
            scale_table(table->table,action->argv[0] + 7,action->argv[1]);
        return(0); /* do the normal set */

	case SHOW:
		return(ShowInterpol(action,"table",table->table,table->alloced,600));
		/* NOTREACHED */
		break;

    case TABCREATE:
		if (action->argc < 3) {
			printf("usage : %s xdivs xmin xmax\n","tabcreate");
			return(0);
		}
		xdivs = atoi(action->argv[0]);
		xmin = Atod(action->argv[1]);
		xmax = Atod(action->argv[2]);
		table->table = create_interpol(xdivs,xmin,xmax);
		table->alloced = 1;
        break;

    case TABFILL:
		if (action->argc < 2) {
			printf("usage : %s xdivs fill_mode\n","tabfill");
			return(0);
		}
		xdivs = atoi(action->argv[0]);
		fill_mode = atoi(action->argv[1]);
		fill_table(table->table,xdivs,fill_mode);
        break;

	case TAB2FIELDS: /* scans through all messages and sets up
					 ** originating field values from the table
					 */
		if (table->alloced && table->step_mode == TAB_FIELDS) {
			int i = 0;
			double	tableval;
			Slot *slot;
			char *addr;
			int GetSlotType();
			MSGLOOP(table,msg) {
				case 3:		/* The first slot has the value, the
							** second the index. We do a fallthrough
							** to case 0 just below.
							*/
					i = MSGVALUE(msg,1);
				case 0:		/* just the value, no index */
					if (i >= 0 && i < table->table->xdivs) {
						/* here we just take the pointer to the
						** location indicated in the msg, and
						** the type info also in the msg, and
						** dump our info back into it.
						** This is risky in that it will fail
						** with different implementations
						** of messaging, and also with
						** parallel simulations.
						** However, it is even harder to
						** get the name of the src field from
						** the message, so I will leave things
						** like this for now.
						*/
						tableval = table->table->table[i];
						slot = (Slot *)(msg + 1);
						/* data is in slot[0] */
						addr = slot[0].data;
						switch(GetSlotType(slot,0)) {
							case DOUBLE:
								*((double *)addr) = tableval;
							break;
							case FLOAT:
								*((float *)addr) = (float)tableval;
							break;
							case INT:
								*((int *)addr) = (int)tableval;
							break;
							case SHORT:
								*((short *)addr) = (short)tableval;
							break;
							default:
								Warning();
								printf("in %s: TAB2FIELDS can only convert doubles, floats, ints and shorts\n", Pathname(table));
							break;
						}
					}
					i++;
				break;
			}
		}
		break;
		case TABOP:
			if (!table->alloced)
				return(0);
			if (action->argc < 1) {
				printf("usage : %s op [min max [divs]]\n","TABOP");
				printf("op must be one of:\n");
				printf("a = average, m = min, M = Max, r= range\n");
				printf("s = slope, i = intercept, f = freq\n");
				printf("S = Sqrt(sum of squares)\n");
				printf("Divs is the # of table entries to be used. It defaults to xdivs");
				return(0);
			} else {
				int op = action->argv[0][0];
				int istart = 0;
				int istop = table->table->xdivs;
				double fstart = table->table->xmin;
				double fstop = table->table->xmax;
				Interpol *ip = table->table;
				double sum;
				double temp;
				int i;

				if (action->argc >= 3) {
					/* Note that someone may have fiddled with
					** the various parms internal to ip. Since
					** xmin and xmax must be correct for this
					** to function, we will use those to calculate
					** our own invdx.
					*/
					double invdx;
					int xdivs = ip->xdivs;
					if (action->argc == 4) {
						xdivs = atoi(action->argv[3]);
						if (xdivs <= 0 || xdivs > ip->xdivs) {
							Warning();
							printf("in %s, divs %d is out of range\n",
								Pathname(table),xdivs);
							return(0);
						}
					}
					invdx = xdivs / (ip->xmax - ip->xmin);
					fstart = Atof(action->argv[1]);
					fstop = Atof(action->argv[2]);
					istart = (fstart - ip->xmin) * invdx;
					if (istart<0) istart = 0;
					if (istart>xdivs) istart = xdivs;

					istop = (fstop - ip->xmin) * invdx;
					if (istop<0) istop = 0;
					if (istop>xdivs) istop = xdivs;

					if (istart >= istop) {
						table->output = ip->table[istart];
						table->input = -1;
						break;
					}
				}
				switch(op) {
					case 'a': /* average */
						sum = 0;
						for(i=istart; i< istop; i++)
							sum += ip->table[i];
						table->output = sum/(istop-istart);
					break;
					case 'm': /* minimum */
						sum = ip->table[istart];
						for(i=istart+1; i< istop; i++) {
							if (sum > ip->table[i])
								sum = ip->table[i];
						}
						table->output = sum;
					break;
					case 'M': /* maximum */
						sum = ip->table[istart];
						for(i=istart+1; i< istop; i++) {
							if (sum < ip->table[i])
								sum = ip->table[i];
						}
						table->output = sum;
					break;
					case 'r': /* range */
						temp = sum = ip->table[istart];
						for(i=istart+1; i< istop; i++) {
							if (sum < ip->table[i])
								sum = ip->table[i];
							if (temp > ip->table[i])
								temp = ip->table[i];
						}
						table->output = temp - sum;
					break;
					case 's': /* slope - Just an easy hack for now */
						sum = ip->table[istart];
						temp = ip->table[istop];
						table->output = (temp - sum)/(fstop - fstart);
					break;
					case 'S': /* Sumsq - Just an easy hack for now */
						sum = 0;
						for(i=istart+1; i< istop; i++)
							sum += ip->table[i] * ip->table[i];
						table->output = sqrt(sum);
					break;
					default: 
						Warning();
					printf("Intercept and freq not yet implemented\n");
					break;
				}
			}
		break;
		case DUMP:
			if (action->argc == 1) {
				if (strcmp(action->argv[0],"pre") == 0) {
					return(0);
				}
				if (strcmp(action->argv[0],"post") == 0) {
					FILE *fp = (FILE *)action->data;
					DumpInterpol(fp,table,table->table,"table");
					return(0);
				}
			}
		break;
    }
    return(0);
}

