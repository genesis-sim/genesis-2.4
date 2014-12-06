static char rcsid[] = "$Id: dd_syn.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: dd_syn.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/06/19 06:14:30  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.6  2000/06/12 05:07:03  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.5  1997/05/29 07:55:42  dhb
** Updates from Antwerp GENESIS version 21e
**
 * EDS20i revison: EDS BBF-UIA 95/06/08
 * Added olf_defs.h
 *
 * Revision 1.4  1995/03/23  01:37:06  dhb
 * Added number of values to SAVE2 file format
 *
 * Revision 1.3  1994/08/08  22:27:57  dhb
 * Changes from Upi.
 *
 * Revision 1.4  1994/06/13  22:52:12  bhalla
 * Added the dump action
 *
 * Revision 1.3  1994/05/25  13:56:47  bhalla
 * Changed syntax for SetTable
 *
 * Revision 1.2  1994/03/22  18:10:29  dhb
 * Changes by Upi Bhalla for setting tables.
 *
 * Revision 1.1  1992/11/14  00:37:18  dhb
 * Initial revision
 *
*/

/*
#include "seg_ext.h"
*/
#include <math.h>
#include "olf_ext.h"
#include "olf_defs.h"

int	DEBUG_DDSyn = 0;
static double		savedata[3];
Interpol	*create_interpol();


#define Bfield(F)	(ddsyn->F)

/*
** modified channelC2 with table front end to transform voltage
** to activation, for dendro-dendritic synapses.
** Calculates channel current therefore needs membrane state.
*/
/* 5/91 Upinder S. Bhalla */
int DDSyn(ddsyn,action)
register struct dd_syn_type *ddsyn;
Action 	*action;
{
double 	x;
double 	dt;
int	has_state;
float	tpeak;
MsgIn 	*msg;
double	V = 0.0;
double xmin,xmax;
int xdivs,fill_mode;
int	n;

    if(Debug(DEBUG_DDSyn) > 1){
	ActionHeader("DDSyn",ddsyn,action);
    }
    SELECT_ACTION(action){
    case INIT:
	ddsyn->activation = 0;
	break;
    case PROCESS:
	MSGLOOP(ddsyn,msg){
	case 0:			/* membrane potential */
	    V = MSGVALUE(msg,0);
	    break;
	case 1:			/* usual activation */
		ddsyn->activation += MSGVALUE(msg,0);
	    break;
	case 2:			/* ddsyn activation */
		if (ddsyn->transf_alloced)
		    ddsyn->activation+=TabInterp(ddsyn->transf,MSGVALUE(msg,0));
	    break;
	case 3:			/* random ddsyn activation */
	    /*
	    ** 0 = probability (1/time)   1 = amplitude
	    */
	    if(urandom() <= MSGVALUE(msg,0)*Clockrate(ddsyn)){
		ddsyn->activation += MSGVALUE(msg,1);
	    }
	    break;
	}
	/*
	** calculate the activation level of the ddsyn
	** dx/dt = (activation*tau1 - x)/tau1
	** x = activation*tau1*(1-exp(-dt/tau1)) + x*exp(-dt/tau1))
	** or
	** x = activation*xconst1 +x*xconst2
	*/
	x = ddsyn->activation*Bfield(xconst1) +
		    Bfield(X)*Bfield(xconst2);
	/*
	** calculate the conductance of the ddsyn
	** dy/dt = (x*tau2 - y)/tau2
	** y = x*tau2*(1-exp(-dt/tau2)) + y*exp(-dt/tau2))
	** or
	** y = x*yconst1 +y*yconst2
	*/
	Bfield(Y) = Bfield(X)*Bfield(yconst1) +
		    Bfield(Y)*Bfield(yconst2);
	/*
	** set the new values of the state variables
	*/
	Bfield(X) = x;
	Bfield(Gk) = Bfield(Y)*Bfield(norm);
	/*
	** calculate the ddsyn current based on the ddsyn conductance
	** the ion equilibrium potential and the membrane potential
	*/
	Bfield(Ik) = (Bfield(Ek) - V) * Bfield(Gk);
	break;
    case RESET:
	ddsyn->activation = 0;
	Bfield(Gk) = 0;
	Bfield(X) = 0;
	Bfield(Y) = 0;
    case RECALC:
	dt = Clockrate(ddsyn);
	Bfield(xconst1) = Bfield(tau1)*(1-exp(-dt/Bfield(tau1)));
	Bfield(xconst2) = exp(-dt/Bfield(tau1));
	Bfield(yconst1) = Bfield(tau2)*(1-exp(-dt/Bfield(tau2)));
	Bfield(yconst2) = exp(-dt/Bfield(tau2));
	if(Bfield(tau1) == Bfield(tau2)){
	    Bfield(norm) = Bfield(gmax)*M_E/Bfield(tau1);
	} else {
	    tpeak = Bfield(tau1)*Bfield(tau2) *
		log(Bfield(tau1)/Bfield(tau2))/
		(Bfield(tau1) - Bfield(tau2));
	    Bfield(norm) = Bfield(gmax)*(Bfield(tau1) - Bfield(tau2))/
		(Bfield(tau1)*Bfield(tau2)*
		(exp(-tpeak/Bfield(tau1)) - exp(-tpeak/Bfield(tau2))));
	}
	break;
    case CHECK:
	has_state = 0;
	MSGLOOP(ddsyn,msg){
	    case 0:
		has_state = 1;
		break;
	}
	if(!has_state){
	    ErrorMessage("DDSyn","No membrane potential.",ddsyn);
	}
	if(Bfield(tau1) <= 0 || Bfield(tau2) <= 0){
	    ErrorMessage("DDSyn","Invalid tau parameters.",ddsyn);
	}
	if (!ddsyn->transf_alloced) {
	    ErrorMessage("DDSyn","Transform table not allocated.",ddsyn);
	}
	break;
	case SET:
        if (action->argc != 2)
            return(0); /* do the normal set */
		if (SetTable(ddsyn,action->argc,action->argv,action->data,
			"transf"))
			return(1);
        if (strncmp(action->argv[0],"transf",6) == 0)
            scale_table(ddsyn->transf,
				action->argv[0] + 8,action->argv[1]);
		return(0);
		/* NOTREACHED */
	break;
    case SAVE2:
	savedata[0] = ddsyn->Gk;
	savedata[1] = ddsyn->X;
	savedata[2] = ddsyn->Y;
        n=3;
        fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),3,(FILE*)action->data);
	break;
	case DUMP:
    	if (action->argc == 1) {
        	if (strcmp(action->argv[0],"pre") == 0) {
            	return(0);
        	}
        	if (strcmp(action->argv[0],"post") == 0) {
            	FILE *fp = (FILE *)action->data ;
            	DumpInterpol(fp,ddsyn,ddsyn->transf,"transf");
            	return(0);
        	}
    	}
	break;
    case RESTORE2:
        fread(&n,sizeof(int),1,(FILE*)action->data);
        if (n != 3) {
            ErrorMessage("DDSyn", "Invalid savedata length", ddsyn);
            return n;
        }
	fread(savedata,sizeof(double),3,(FILE*)action->data);
	ddsyn->Gk = savedata[0];
	ddsyn->X = savedata[1];
	ddsyn->Y = savedata[2];
	break;
        /* Specially for the array fields */
    case TABCREATE:
		if (action->argc < 3) {
			printf("usage : %s xdivs xmin xmax\n","tabcreate");
			return(0);
		}
		xdivs = atoi(action->argv[0]);
		xmin = Atod(action->argv[1]);
		xmax = Atod(action->argv[2]);
		ddsyn->transf = create_interpol(xdivs,xmin,xmax);
		ddsyn->transf_alloced = 1;
        break;
    case TABFILL:
		if (action->argc < 2) {
			printf("usage : %s xdivs fill_mode\n","tabfill");
			return(0);
		}
		xdivs = atoi(action->argv[0]);
		fill_mode = atoi(action->argv[1]);
		fill_table(ddsyn->transf,xdivs,fill_mode);
        break;
    }
    return(0);
}
