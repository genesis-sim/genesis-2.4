/* rhodopsin.c */
#include "chem_ext.h"

/*
** Avrama Blackwell, GMU, March 2002:

Stochastic activation of rhodopsin molecules by Poisson generator 
(randomspike) which also indicates which villi is activated.

Keeps track of: 
   Number of rhodopsin molecules per villi
       which allows decrease effectiveness with additional villi
   time of rhodopsin activation
       which allows decrease effectiveness over time due to depletion
     
   Inputs:
       from "light" Poisson generator - indicates when isomerization occurs
       multiply value by total number of villi to determine which villus.

       from "inactivation" Poisson generator - indicates when inactivation
       occurs.  Multiply value by active villi in slice to determine which rho

       feedback for changing rate of inactivation 
 
   Output units:   
       effective metarhodopsin molecules - to determine G prot activation
       inactivation rate - to set rate of inactivation Poisson generator


*/

#define AVOGADRO 6.023e23         /* units are molecules per M */

rhodopsin(rho,action)
register struct rhodopsin_type *rho;
Action		*action;
{
  MsgIn	*msg;
  double	dt;
  double temp, deltatime;
  int i;
  double float_isom;
  double float_inact;
  int num_isom;

  if(debug > 1){
    ActionHeader("rhodopsin",rho,action);
  }

  SELECT_ACTION(action){
  case INIT:
    rho->effective=0;
    rho->villus=-1;
    rho->isom=-1;
    rho->input_slice=-1;
    break;
    
  case PROCESS:
    dt = Clockrate(rho);
    rho->total_time+=dt;
    MSGLOOP(rho,msg) 
      {
      case 0:	/* isomerization */
	/* 0 = value between 0 and 1*/
	float_isom = MSGVALUE(msg,0);

	/* is isomerization in this slice? and is this first dt that message was sent? */
	if (float_isom > 0 && float_isom !=rho->last_isom) {
	  rho->last_isom=float_isom;
	  rho->isom=rho->total_villi*float_isom;
	  rho->input_slice=rho->isom/rho->slice_villi;
	  
	  /* determine which villus; increment number of isomerizations*/
	  if (rho->input_slice+1==rho->slice) {
	    rho->villus=rho->isom%rho->slice_villi;
	    rho->villi_isom[rho->villus]++;
	    rho->total_isom++;

	    /* if incrementing from 0 to 1, add this villus to list */   
	    if (rho->villi_isom[rho->villus]==1) {
	      rho->villi_list[rho->active_villi]=rho->villus;
	      rho->active_villi++;
	    }

	    /* if this is first time this villus has isom, store 
	       isomerization time, increment total volume and area */
	    if (rho->isom_time[rho->villus]==-1) {
	      rho->isom_time[rho->villus]=rho->total_time;
	      rho->slice_vol+=rho->villus_vol;	
	      rho->slice_xarea+=rho->villus_xarea;
	      rho->slice_sa+=rho->villus_sa;
	    }
	  }
	}
 	break;

      case 1: /* inactivation */
	/* 0 = value between 0 and 1 */
	float_inact = MSGVALUE(msg,0);

	/* identify which villus has inactivation, decrement */
	if ((float_inact > 0) && (float_inact !=rho->last_inact)) {
	  rho->last_inact=float_inact;
	  rho->inact=rho->active_villi*float_inact;
	  rho->villi_isom[rho->villi_list[rho->inact]]--;
	  rho->total_isom--;
	  rho->total_inact++;

	  /* if villi no longer has active mrhod, remove from list */
	  if (rho->villi_isom[rho->villi_list[rho->inact]]==0) {
	    for (i=rho->inact; i< rho->active_villi-1; i++) 
	      rho->villi_list[i] = rho->villi_list[i+1];
	    rho->active_villi--;
	    rho->villi_list[rho->active_villi]=0;
	  }

	}
	break;

      }

	/* compute effective number of rhodopsin molecules */

    for (i=0; i<rho->active_villi; i++) {
      num_isom=rho->villi_isom[rho->villi_list[i]];
      deltatime=(rho->total_time - rho->isom_time[rho->villi_list[i]]);
      rho->effective+=rho->factor[((num_isom>9)?9:num_isom)]/((deltatime>1.0) ? pow(deltatime,rho->depletion) : 1.0);
    }
      rho->inact_rate = rho->inact_const*rho->total_isom;

    /* compute concentration of metarhodopsin */
    rho->quantity=rho->total_isom/(AVOGADRO*rho->units);
    rho->conc=(rho->quantity > 0) ? rho->quantity/rho->slice_vol : 0;
    break;

  case RESET:
    for (i=0; i<rho->slice_villi; i++) {
      rho->isom_time[i]=-1;
      rho->villi_isom[i]=0;
      rho->villi_list[i]=0;
    }
    rho->active_villi=0;
    rho->total_isom=0;
    rho->slice_vol=0;
    rho->slice_xarea=0;
    rho->slice_sa=0;
    rho->effective =0;
    rho->total_time=0;
    rho->quantity=0;
    rho->conc=0;
    rho->total_inact=0;
    break;

  case SET :
    return(0); /* do the normal set */
    break;

  case CHECK :
    if (rho->slice_villi > 420) {
      ErrorMessage("rhodopsin", "villi per slice must be < 420", rho);
    }
    break;

  }
}
