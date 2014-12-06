static char rcsid[] = "$Id: interspike.c,v 1.3 2005/07/01 10:03:01 svitak Exp $";

/*
** $Log: interspike.c,v $
** Revision 1.3  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:36  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:50  dhb
** Initial revision
**
*/

#include "dev_ext.h"

/* Computes a interspike interval distribution.  The last bin contains
**  also all values larger than the bin-range. */
/* E. De Schutter, Caltech 8/91,1/92 */
void InterSpikeInterval(count,action)
register struct interspike_type *count;
Action		*action;
{
	MsgIn	*msg;
	float   voltage = 0.0;
	float	time;
	int     trigger;
	int		bin_index;
	int     i;

    if(debug > 1){
		ActionHeader("InterSpikeInterval",count,action);
    }

    SELECT_ACTION(action){
		case PROCESS:
			/* get the messages */
			MSGLOOP(count,msg) {
				case 0:				/* INPUT */
					voltage = MSGVALUE(msg,0);
					break;
                case 1:             /* TRIGGER */
                    trigger = (MSGVALUE(msg,0) == 0);
                    if (trigger) {
						count->triggered = 1;
						count->prev_time = SimulationTime();
                    }
                    break;
                case 2:             /* SPIKE */
                    time = MSGVALUE(msg,0);
					/* compute interval if preceding spike available */
					if (count->prev_time >= 0.0) {
						count->output = time - count->prev_time;
						bin_index = count->output / count->binwidth;
						if (bin_index >= count->num_bins) {
							/* bin overflow -> add to last bin */
							bin_index = count->num_bins - 1;
						}
						count->table[bin_index] += 1;
					}
					count->prev_time = time;
                    break;
			}
			if (count->triggered) {
				/* look for a new spike */
				if (count->spiking) {	/* check whether this spike is done */
					if (voltage < count->threshold) {
						count->spiking = 0;	/* spike is done */
					}
				} else {		/* check whether spike started */
					if (voltage >= count->threshold) {
						/* compute interval if preceding spike available */
						time = SimulationTime();
						if (count->prev_time >= 0.0) {
							count->output = time - count->prev_time;
							bin_index = count->output / count->binwidth;
							if (bin_index >= count->num_bins) {
								/* bin overflow -> add to last bin */
								bin_index = count->num_bins - 1;
							}
							count->table[bin_index] += 1;
						}
						count->prev_time = time;
						count->spiking = 1;	/* spike started */
					}
				}
			}
			break;
		case RESET:
			if (!count->allocated && (count->num_bins > 0)) {
				/* allocate memory for the bin arrays */
				count->table = (int *)calloc(count->num_bins,sizeof(int));
				for (i=0; i<count->num_bins; i++) {
					count->table[i] = 0.0;
				}
				count->allocated = 1;
			} else if (count->reset_mode == CLEAR_BINS) {
				/* clear the bin arrays */
				for (i=0; i<count->num_bins; i++) {
					count->table[i] = 0.0;
				}
			}
            if (count->trigger_mode == FREE_RUN) {
                count->triggered = 1;   /* binning is active */
            } else {
                count->triggered = 0;  /* no binning till triggered */
            }
			count->output = 0.0;
			count->prev_time = -1.0; /* no previous spike available! */
			count->spiking = 0;
			break;
		case CHECK:
			i = 0;
			MSGLOOP(count,msg) {
				case 0:     /* INPUT */
					i = 1;
					break;
			}
			if (i != 1) {
				ErrorMessage("peristim","Wrong number of INPUT msg.",count);
			}
			if (count->num_bins < 5) {
				ErrorMessage("peristim","Bad num_bins value.",count);
			}
			if (count->binwidth <=0) {
				ErrorMessage("peristim","Bad binwidth value.",count);
			}
			break;
    }
}
