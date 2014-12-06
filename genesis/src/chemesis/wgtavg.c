/* wgtavg.c */
/* Avrama Blackwell GMU Oct 2011 */

#include "chem_ext.h"
/* Computes a weighted average, e.g. volume weighted average of calcium shells */


void wgtavg(wgtavg,action)
register struct wgtavg_type *wgtavg;
Action		*action;
{
  MsgIn 	*msg;
  double	Value,wgt;

    if(debug > 1){
		ActionHeader("wgtavg",wgtavg,action);
    }

    SELECT_ACTION(action){
    case INIT:
      wgtavg->totValue=0.0;
      wgtavg->totwgt=0.0;
      break;

    case PROCESS:
	/* Read the msgs to get the buffer concentrations */
	MSGLOOP(wgtavg,msg){
		case 0:  /* 0 = Value wgt */
		  Value = MSGVALUE(msg,0);
		  wgt = MSGVALUE(msg,1);
		  wgtavg->totValue+= Value*wgt;
		  wgtavg->totwgt+=wgt;
		  break;
	}
	if (wgtavg->totwgt>0.0) {
		wgtavg->meanValue=wgtavg->totValue/wgtavg->totwgt;
	} else {
		wgtavg->meanValue=0.0;
	}
	break;

    case RESET:
	wgtavg->meanValue=0.0;
        wgtavg->totValue=0.0;
        wgtavg->totwgt=0.0;
	break;
    }
}

