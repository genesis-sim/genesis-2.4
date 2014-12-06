static char rcsid[] = "$Id: Mg_block.c,v 1.3 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: Mg_block.c,v $
** Revision 1.3  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:57  dhb
** Initial revision
**
*/

#include <math.h>
#include "seg_ext.h"


/* 11/90 Erik De Schutter */
/* Implementation of voltage dependent, [Mg] dependent channel
**	blocking.  */

void MgBlock(block,action)
struct Mg_block_type *block;
Action		*action;
{
double	V = 0.0,KMg,G= 0.0;
int	has_state;
MsgIn	*msg;

    if(debug > 1){
	ActionHeader("Mg_block",block,action);
    }
    SELECT_ACTION(action){
    case INIT:
	break;

    case PROCESS:
	MSGLOOP(block,msg){
	    case 0:			/* membrane potential */
		/* 0 = Vm */
		V = MSGVALUE(msg,0);
		break;

	    case 1:			/* unblocked channel conductance */
		/* 0 = Gk */
		G = MSGVALUE(msg,0);
		break;

	    case 2:			/* unblocked channel conductance */
		/* 0 = Gk	1 = Ek */
		G = MSGVALUE(msg,0);
		block->Ek = MSGVALUE(msg,1);
		break;

	    case 3:			/* set Zk value */
		/* 0 = (scaled) charge */
		block->Zk = MSGVALUE(msg,0);
		break;
	}
	/* Gunblocked/Gtotal = KMg / (KMg + [Mg])
	**	KMg = A * exp(V/B) 
	** calculate activation = unblocked conductance */
	KMg = block->KMg_A * exp(V/block->KMg_B);
	block->Gk = G * KMg / (KMg + block->CMg);
	block->Ik = block->Gk * (block->Ek - V);
	break;

    case RESET:
	block->Gk = 0.0;
	break;

    case CHECK:
	/* read the msg */
	has_state = 0;
	MSGLOOP(block,msg){
	    case 0:	/* membrane potential */
		has_state = 1;
		break;
	}
	if(has_state == 0){
	    ErrorMessage("Mg_block","No membrane potential.",block);
	} 
	break;
    }
}

