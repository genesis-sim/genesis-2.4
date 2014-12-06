static char rcsid[] = "$Id: randomspike.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: randomspike.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:44  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1997/08/08 23:42:23  dhb
** Added SET action to randomspike to calculate realrate whenever
** rate or abs_refract are changed.
**
** Revision 1.3  1997/08/01 18:57:44  dhb
** Fix to randomspike to adjust for the rate damping effect of
** the refractory period.  (From Erik De Schutter)
**
 * Version EDS21e4  1997/07/30
 * Erik De Schutter: correct for refractory period during high rates and
 *                   improved initialization for high firing rates.
 *
 * Revision 1.2  1996/07/22  22:17:02  dhb
 * Whitespace changes (Mike Vanier)
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "newconn_ext.h"

static int	DEBUG_Randomspike = 0;

/* 
 * Randomspike
 * 
 * Produces random point events, i.e. a Poisson-distributed series 
 * of events that can be used as a random spike train.
 */

/* 9/88 Matt Wilson, 1/94 Dave Bilitch 5/94 Mike Vanier */
/* And none of you saw fit to fix the control falling off the bottom
** of this non-void function!!! svitak
*/

int Randomspike(random,action)
register struct Randomspike_type *random;
Action *action;
{
  MsgIn	 *msg;
  MsgOut *mo;
  char** argv = action->argv;
  float	  p,m;

  if (Debug(DEBUG_Randomspike) > 1)
    {
      ActionHeader("Randomspike",random,action);
    }

  switch (action->type)
    {  
    case RESET:
      random->state     = random->reset_value;
      if (random->rate<=0.0) {
	random->lastevent = 0.0;
	random->realrate = 0.0;
      } else {
      /* EDS change 30.07.97
      ** This determines how soon we can fire after reset (refractory period).
      ** For high rates it is important that we initialize sometimes for
      ** a lastevent that is still within the refractory period, otherwise
      ** Randomspike is guaranteed to spike at time zero (i.e. not random).
      ** Therefore we intialize at - (random interspike interval from
      ** Poisson distribution).
      */
        for (p=0.0; p==0.0; ) p=urandom(); /* get random number larger than 0 */
	m = 1.0/random->rate;		  /* mean frequency */
        random->lastevent = m * log(p);
      }
      break;
      
    case PROCESS:
      if (random->reset)
	random->state = random->reset_value;

      MSGLOOP(random,msg)
	{
	case RATE:       
	  random->rate = MSGVALUE(msg,0);
	  p = 1.0 - random->rate*random->abs_refract;
	  if (p<=0.0) {
	    ErrorMessage("Randomspike","Rate is too high compared to abs_refract",random);
	    random->realrate = random->rate;
	  } else {
	    random->realrate = random->rate / p;
	  }
	  break;
	case MINMAX: 
	  random->min_amp = MSGVALUE(msg,0);
	  random->max_amp = MSGVALUE(msg,1);
	  break;
	}
      
      /* if we're still in the refractory period then return */

      if (random->abs_refract > (SimulationTime() - random->lastevent))
	  return 1;
	  
      /* send spike event at random times */
      
      p = random->realrate * Clockrate(random);
      if (p >= 1 || p >= urandom()) 
	{
	  random->lastevent = SimulationTime();
	  if (random->min_amp != random->max_amp)
	    random->state = frandom(random->min_amp,random->max_amp);
	  else 
	    random->state = random->min_amp;
	  
	  /* Send event to destination elements */
	  
	  MSGOUTLOOP(random, mo) 
	    {
	      CallEventAction(random, mo);
	    }
	}
      break;
      
    case SET:
      /*
      ** Moved from RESET to SET action.  Update realrate on change in
      ** rate or abs_refract.  ---dhb
      */

      /* EDS change 30.07.97
      ** For high firing rates we need to correct for the refractory
      ** period, otherwise the effective firing rate is (much) lower.
      */
	if (strcmp(argv[0],"rate") == 0)
	    random->rate = Atof(argv[1]);
	else if (strcmp(argv[0], "abs_refract") == 0)
	    random->abs_refract = Atof(argv[1]);
	else
	    return 0; /* setfield sets some other field */

	/* recalculate realrate */
	p = 1.0 - random->rate*random->abs_refract;
	if (p<=1e-7) {
	  ErrorMessage("Randomspike","Rate is too high compared to abs_refract",random);
	  random->realrate = random->rate;
	} else {
	  random->realrate = random->rate / p;
	}

	return 1; /* values already set here */
      
    default:
      InvalidAction("Randomspike",random,action);
      break;
    }

    return 1;
}
