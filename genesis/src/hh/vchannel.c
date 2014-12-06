static char rcsid[] = "$Id: vchannel.c,v 1.4 2005/07/20 20:01:59 svitak Exp $";

/*
** $Log: vchannel.c,v $
** Revision 1.4  2005/07/20 20:01:59  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/07/01 10:03:03  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:38  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1996/10/07 21:15:06  dhb
** Added SAVE2 and RESTORE2 actions for vdep_channels
**
** Revision 1.1  1992/12/11  19:03:01  dhb
** Initial revision
**
*/

#include <math.h>
#include "hh_ext.h"

/* M.Wilson Caltech 2/89 */
int VDepChannel(channel,action)
register struct vdep_channel_type *channel;
Action		*action;
{
double	astate,mstate;
double	savedata[2];
int	n;
int	nr;
MsgIn	*msg;

    if(debug > 1){
	ActionHeader("VDepChannel",channel,action);
    }

    SELECT_ACTION(action){
    case RESET:
	channel->Ik = 0;
	channel->Gk = 0;
	ClearElementFlag(channel,0);
	MSGLOOP(channel,msg) {
	case 2:				/* 2nd mult gate term */
	    SetElementFlag(channel,0);
	    break;
	}
    case PROCESS:
	/* channel->activation is used as the membrane potential */
	mstate = 1;
	astate = CheckElementFlag(channel,0);
	MSGLOOP(channel,msg) {
	case 0:				/* compartment */
	    /*
	    ** 0 = membrane potential
	    */
	    channel->activation = MSGVALUE(msg,0);
	    break;
	case 1:				/* multiplicative gate term */
	    /*
	    ** 0 = state
	    ** 1 = power
	    */
	    mstate *= pow(MSGVALUE(msg,0),MSGVALUE(msg,1));
	    break;
	case 2:				/* 2nd mult gate term */
	    /*
	    ** 0 = state
	    ** 1 = power
	    */
	    astate *= pow(MSGVALUE(msg,0),MSGVALUE(msg,1));
	    break;
	case 3:				/* gmax term */
	    channel->gbar = MSGVALUE(msg,0);
	    break;
	case 4:				/* Ek term */
	    channel->Ek = MSGVALUE(msg,0);
	    break;
	}
	channel->Gk = (mstate + astate)*channel->gbar;

	/* 
	** calculate the transmembrane current 
	*/
	channel->Ik = (channel->Ek - channel->activation) * channel->Gk;
	break;
    case CHECK:
	break;

    case SAVE2:
        n = 2;
        fwrite(&n, sizeof(int), 1, (FILE*) action->data);
	savedata[0] = channel->Ik;
	savedata[1] = channel->Gk;
        fwrite(savedata, sizeof(double), 2, (FILE*) action->data);
        break;

    case RESTORE2:
        n = 0;
        if (fread(&n,sizeof(int),1,(FILE*)action->data) != 1 || n != 2)
	  {
	    ErrorMessage("Vdep_channel","Invalid savedata length",channel);
	    return ( n );
	  }

        nr = fread(savedata,sizeof(double),2,(FILE*)action->data);
        if (nr != 2)
	  {
	    ErrorMessage("Vdep_channel","Invalid savedata",channel);
	    return ( n - nr );
	  }

        channel->Ik = savedata[0];
        channel->Gk = savedata[1];
	break; /* RESTORE2 must return zero on success!!! */

    }

    return 0;
}
