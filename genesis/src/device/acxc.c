static char* rcsid = "$Id: acxc.c,v 1.3 2005/07/01 10:03:01 svitak Exp $";

/*
** $Log: acxc.c,v $
** Revision 1.3  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:34  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1998/03/31 22:10:24  dhb
** Initial revision
**
*/

#include "spike_ext.h"
 
#define Field(F)   (doxc->F)
 
/* Dieter Jaeger 9/92  */
 
void Doxc(doxc, action)
register struct doxc_type *doxc;
Action  *action;
{
	int i,bin;
	float spikecon;
	float calcspike;
	int temp2idx;
	float currtime;
	MsgIn *msg;
 
	SELECT_ACTION(action) {
	case PROCESS:
		currtime = SimulationTime();
		MSGLOOP(doxc,msg) {
		case 0:                   /* spike from neuron 1 */
			spikecon = MSGVALUE(msg,0);
			if (doxc->allocated && spikecon > doxc->threshold) {
				doxc->curr1idx = (++(doxc->curr1idx))%(doxc->maxspikes/2);
				doxc->sp1times[doxc->curr1idx] = currtime;
				(doxc->no1spks)++;
				doxc->dontnow = 0;
			}
			break;
		case 1:                   /* spike from neuron 2 */
			spikecon = MSGVALUE(msg,0);
         if (doxc->allocated && spikecon > 0.5) {
				doxc->curr2idx = (++(doxc->curr2idx))%(doxc->maxspikes);
 				doxc->sp2times[doxc->curr2idx] = currtime;
				(doxc->no2spks)++;
			}
			break;
		} /* end MSGLOOP */
		if(!doxc->dontnow && doxc->curr1idx != -1 && doxc->curr2idx != -1) {
			if (currtime - doxc->wintime > doxc->sp1times[doxc->calc1idx])
			{
         	 /* calculate xc histogram */
  		       calcspike = doxc->sp1times[doxc->calc1idx];
				 temp2idx = doxc->calc2idx;
				 if(calcspike > doxc->wintime) {
     			    for(;;) {
						if(doxc->sp2times[temp2idx] < calcspike - doxc->wintime) {
							if(doxc->curr2idx == doxc->calc2idx) {
								break;
							}
           			   doxc->calc2idx = (++(doxc->calc2idx))%doxc->maxspikes;
							temp2idx = (++temp2idx)%(doxc->maxspikes);
						}
						else if(doxc->sp2times[temp2idx] < calcspike + doxc->wintime) {
							/* put event in histogram */
							bin = (int)((doxc->sp2times[temp2idx] - calcspike + doxc->wintime) * (1.0/doxc->binwidth));
     	      		   if(bin < 0 || bin >= doxc->num_bins) {
     	         		   printf("error in xcloop: bin = %d", bin);
     	         		}
     	       		   else {
     	         	   	(doxc->xcarray[bin]++);
							}
							if(doxc->curr2idx == temp2idx) break;
							temp2idx = (++temp2idx)%(doxc->maxspikes);
     		  	    	}
						else {
							break;
						}
					}
				}		
				if(doxc->calc1idx == doxc->curr1idx) doxc->dontnow = 1;
                doxc->calc1idx = (++(doxc->calc1idx))%(doxc->maxspikes/2);
         }
		} /* end do xcloop */
		break;	
	case RESET:
    doxc->no1spks = 0; doxc->no2spks = 0;
      doxc->curr1idx = -1; doxc->curr2idx = -1;
      doxc->calc1idx = 0;
      doxc->calc2idx = 0;
			doxc->dontnow = 1;
		if(!doxc->allocated && (doxc->num_bins > 0)) {
		/* allocate memory for the cross correlation arrays */
			doxc->xcarray = (int *)calloc(doxc->num_bins,sizeof(int));
			doxc->maxspikes = (int)(doxc->num_bins * doxc->binwidth * 1000);
			doxc->wintime = (float)(doxc->num_bins * doxc->binwidth / 2.0);
			doxc->sp1times = (float *)calloc(doxc->maxspikes/2, sizeof(float));
			doxc->sp2times = (float *)calloc(doxc->maxspikes, sizeof(float));
			doxc->allocated = 1;
			for (i=0; i < doxc->num_bins; i++) {
				doxc->xcarray[i] = 0;
			}
		}
		else if (doxc->reset_mode == 1) {
    /* create new table, size may have changed */
    free(doxc->xcarray);
    free(doxc->sp1times);
    free(doxc->sp2times);
    doxc->xcarray = (int *)calloc(doxc->num_bins,sizeof(int));
    doxc->maxspikes = (int)(doxc->num_bins * doxc->binwidth * 1000);
    doxc->wintime = (float)(doxc->num_bins * doxc->binwidth / 2.0);
    doxc->sp1times = (float *)calloc(doxc->maxspikes/2, sizeof(float));
    doxc->sp2times = (float *)calloc(doxc->maxspikes, sizeof(float));
    for (i=0; i < doxc->num_bins; i++) {
      doxc->xcarray[i] = 0;
    }

		/* clear spiketable */
    for (i=0; i < doxc->maxspikes; i++) {
		    doxc->sp2times[i] = 0.0;
    }
		for (i=0; i < doxc->maxspikes / 2; i++) {
			doxc->sp1times[i] = 0.0;
		}
		} /* end else if */
		break;
	}
}
	
void Doac(doac, action)
register struct doac_type *doac;
Action  *action;
{
	int i,bin;
	float spikecon;
	int tempidx;
	float calcspike;
	float currtime;
	MsgIn *msg;
 
	SELECT_ACTION(action) {
	case PROCESS:
		currtime = SimulationTime();
		MSGLOOP(doac,msg) {
		case 0:                   /* spike from neuron 1 */
			spikecon = MSGVALUE(msg,0);
			if (doac->allocated && spikecon > doac->threshold) {
				doac->curridx = (++(doac->curridx))%doac->maxspikes;
				doac->sptimes[doac->curridx] = currtime;
				(doac->nospks)++;
				doac->dont = 0;
			}
			break;
		} /* end MSGLOOP */
		if(doac->dont != 1 && doac->curridx > 0) {
			if(currtime - doac->wintime > doac->sptimes[doac->calcidx])
			{
	  			 /* calculate auto-correlation  */
 				calcspike = doac->sptimes[doac->calcidx];
				tempidx = ((doac->calcidx + 1)%doac->maxspikes);
				for(;;) {
					if(calcspike + doac->wintime > doac->sptimes[tempidx]) {
						bin = (int)((doac->sptimes[tempidx] - calcspike) * (1.0/doac->binwidth));				
						if(bin >= 0 && bin < doac->num_bins) {
							(doac->acarray[bin])++;
						}
						else {
							printf("Error in doac: bin = %d\n", bin);
						}
                  if(tempidx == doac->curridx) {
                       break;
                  }
									else
									tempidx = (++tempidx)%(doac->maxspikes);
					}
					else {
						break;	
					}
				}
				if(doac->calcidx >= doac->curridx-1) doac->dont = 1;
				doac->calcidx = (++(doac->calcidx))%doac->maxspikes;
			}
		} /* end do acloop */
		break;	
	case RESET:
		doac->dont = 1;
    doac->nospks = 0;
    doac->curridx = -1;
    doac->calcidx = 0;
		if(!doac->allocated && (doac->num_bins > 0)) {
			/* allocate memory for arrays */
			doac->maxspikes = (int)(doac->num_bins * doac->binwidth * 1000);
			doac->wintime = (float)(doac->num_bins * doac->binwidth);
			doac->acarray = (int *)calloc(doac->num_bins, sizeof(int));
			doac->sptimes = (float *)calloc(doac->maxspikes, sizeof(float));
			doac->allocated = 1;
      for (i=0; i < doac->num_bins; i++) {
         doac->acarray[i] = 0;
      }
		}
		else if(doac->reset_mode == 1) {
			/* recreate table */
			free(doac->acarray);
			free(doac->sptimes);
			doac->maxspikes = (int)(doac->num_bins * doac->binwidth * 1000);
      doac->wintime = (float)(doac->num_bins * doac->binwidth);
      doac->acarray = (int *)calloc(doac->num_bins, sizeof(int));
      doac->sptimes = (float *)calloc(doac->maxspikes, sizeof(float));
      for (i=0; i < doac->num_bins; i++) {
         doac->acarray[i] = 0;
      }
    }
    if(doac->allocated) {
      /* clear table */
      for (i=0; i < doac->maxspikes; i++) {
         doac->sptimes[i] = 0;
      }
    }
		break;
	}
}
 
#undef Field
