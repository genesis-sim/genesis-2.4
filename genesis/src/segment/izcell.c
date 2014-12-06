/* Intitial version, Dave Beeman 2006/03/02
   Implementation of the Izhikevich simple spiking cell model

   Reference: Eugene M. Izhikevich, "Simple Model of Spiking Neurons",
   IEEE Trans. Neural Networks 14:1569-1572 (2003).

   Vm is  determined from the equation:

   dVm/dt = coeff2*Vm*Vm + coeff1*Vm + coeff0 - u + I

   where I is the total incoming 'current' from injection or other sources,
   with du/dt = A*(B*Vm - u), and the spike reset condition:

   if (Vm >= Vmax) then Vm = C; u = u + D

*/

#include <math.h>
#include "seg_ext.h"

IzCell(izcell,action)
register struct izcell_type *izcell;
Action          *action;
{
MsgIn   *msg;
double	dv_dt;
double	du_dt;
double	dt;
double I;
/* used only if Crank-Nicholson is imlemented
double Vtemp, utemp;  
*/

    SELECT_ACTION(action){

    case INIT:
	izcell->state = 0; /* cell isn't firing */
        break;

    case PROCESS:
	dt = Clockrate(izcell);
	I = izcell->inject; /* total current into cell */
        MSGLOOP(izcell,msg){
	  case 0: /* INJECT inject */
	  I += MSGVALUE(msg,0);
	  break;

	  case 1: /* CHANNEL Gk Ek --> current = Gk*(Ek - Vm) */
	    I += MSGVALUE(msg,0)*(MSGVALUE(msg,1) - izcell->Vm);
	  break;
	}
/* simple forward Euler method - not used
    dv_dt = izcell->Vm*(izcell->coeff2*izcell->Vm + izcell->coeff1)
	+ izcell->coeff0 - izcell->u + I;
    du_dt = izcell->A * (izcell->B * izcell->Vm - izcell->u);
    izcell->Vm += dt*dv_dt;
    izcell->u += dt*du_dt;
*/

/* Crank-Nicholson for both Vm and u (not used)
    dv_dt = izcell->Vm*(izcell->coeff2*izcell->Vm + izcell->coeff1)
	+ izcell->coeff0 - izcell->u + I;
    du_dt = izcell->A * (izcell->B * izcell->Vm - izcell->u);
    Vtemp = izcell->Vm + dt*dv_dt;
    utemp = izcell->u + dt*du_dt;
    izcell->u += 0.5*dt*(du_dt + izcell->A * (izcell->B * Vtemp - utemp));
    izcell->Vm += 0.5*dt*(dv_dt + Vtemp*(izcell->coeff2*Vtemp +
	izcell->coeff1)	+ izcell->coeff0 - utemp + I);
*/

/* second order Taylor's series - tests showed this to be more accurate
   than Crank-Nicholson for long runs with large networks
   Vm(new) = Vm + dt*dv_dt + 0.5*dt*dt*(d(dv_dt)/dt); similarly for u
*/

    dv_dt = izcell->Vm*(izcell->coeff2*izcell->Vm + izcell->coeff1)
        + izcell->coeff0 - izcell->u + I;
    du_dt = izcell->A * (izcell->B * izcell->Vm - izcell->u);
    izcell->Vm += dt*dv_dt*(1 + dt*(izcell->coeff2*izcell->Vm +
	izcell->coeff1/2)) - dt*dt*du_dt/2;
    izcell->u += dt*du_dt + dt*dt*izcell->A *(izcell->B*dv_dt - du_dt);

    /* Reset the action potential to its falling side when it passes Vmax */

    if (izcell->Vm >= izcell->Vmax) {
	izcell->Vm = izcell->C;
	izcell->u += izcell->D;
	izcell->state = 1;
    }
    break;  /* end of PROCESS ACTION */
    case RESET:
	izcell->Vm = izcell->C;
	izcell->u = izcell->B * izcell->Vm; /* sets du_dt = 0 */
      break;
    case CHECK:
      break;
    }
}
