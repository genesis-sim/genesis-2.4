/* feedback.c */
#include "chem_ext.h"

/*
**Avrama Blackwell, GMU
**
** Computes change in rate constant due to feedback
**
*/
#define HYPERBOLIC 0
#define SIGMOIDINC 1
#define SIGMOIDDEC 2

/* with feedback, decrease in enzyme activity is a function of concentration of
 * feedback substance:  
 *   hyperbolic form: fb propto 1 / conc(feedback substance) for neg feedback
 *                    fb propto conc (feeback substance) for positive feedback.
 *    Sigmoid form: fb = conc/conc + halfmax (for sigmoid increase feedback)
 *                  fb = halfmax / conc + halfmax (for sigmoid decrease feedback)
*/

feedback(fb,action)
register struct feedback_type *fb;
Action       *action;
{
  MsgIn  *msg;
  double temp;       /* temporary storage of feedback value */
  double fbconc;     /* concentration of feedback substance */
  int i;

    if(debug > 1){
		ActionHeader("feedback",fb,action);
    }

  SELECT_ACTION(action){
  case INIT:
    fb->feedback=1;
    break;
   
  case PROCESS:
    MSGLOOP(fb,msg) {
    case 0:	/*  feedback concentration */
      fbconc = MSGVALUE(msg,0);
      break;
    } /* end MSGLOOP */

    if ((fb->sign == -1) && (fbconc < fb->thresh)) /* modify rate const if conc < thresh */
	{
	  if (fb->form == HYPERBOLIC)
	    temp = fbconc/fb->thresh;
	  if (fb->form == SIGMOIDINC)
	    temp = fbconc/(fbconc+fb->halfmax);
	  if (fb->form == SIGMOIDDEC)
	    temp = fb->halfmax/(fbconc+fb->halfmax);
	  fb->feedback=1;
	  for (i=1; i<=fb->pow; i++)
	    fb->feedback = fb->feedback*temp;
	}
    else if ((fb->sign == 1) && (fbconc > fb->thresh)) /* modify rate const if conc > thresh */
	{
	  if (fb->form == HYPERBOLIC)
	    temp = fb->thresh/fbconc;
	  if (fb->form == SIGMOIDINC)
	    temp = fbconc/(fbconc+fb->halfmax);
	  if (fb->form == SIGMOIDDEC)
	    temp = fb->halfmax / (fbconc + fb->halfmax);
	  fb->feedback=1;
	  for (i=1; i<=fb->pow; i++)
	    fb->feedback = fb->feedback*temp;
	}
      else
	fb->feedback = 1;
    break;

  case RESET:
    fb->feedback=1;
    break;

  case SET :
    return(0); /* do the normal set */
    break;

  } /* end select_action */
} /* end function */

