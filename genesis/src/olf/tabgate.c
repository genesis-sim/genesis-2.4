static char rcsid[] = "$Id: tabgate.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: tabgate.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.7  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.6  1997/05/29 08:36:31  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS20i revison: EDS BBF-UIA 95/06/08
 * Added olf_defs.h
 *
 * Revision 1.5  1995/03/23  01:37:06  dhb
 * Added number of values to SAVE2 file format
 *
 * Revision 1.4  1995/03/21  01:03:06  venkat
 * Upi changes: Typecasted second argument to TabInterp() to double
 *
 * Revision 1.3  1994/08/08  22:31:35  dhb
 * Changes from Upi.
 *
 * Revision 1.4  1994/06/13  22:52:41  bhalla
 * Added the DUMP action
 *
 * Revision 1.3  1994/05/26  13:53:10  bhalla
 * Changed syntax for SetTable
 *
 * Revision 1.2  1994/03/22  18:12:21  dhb
 * Changes by Upi Bhalla for setting tables.
 *
 * Revision 1.1  1992/11/14  00:37:23  dhb
 * Initial revision
 *
*/

#include "olf_ext.h"
#include "olf_defs.h"

static double       savedata;
Interpol *create_interpol();

/*
** Calculates a gate state using tabulated values. The only assumption
** is that the gate obeys the following differential equation :
**
**	dm/dt = alpha * (1 - m) - beta * m
**
** where alpha and beta are interpolated tables giving the dependency
** on the activation field.
** The form of the object is lifted directly from Matt Wilson's
** Vdep gate object. The exponential calculation routines have been
** replaced with lookup tables instead.
*/


int TabGate(gate,action)
register struct tab_gate_type *gate;
Action		*action;
{
static MsgIn	*msg;
static double	alpha,beta;
static double	salpha,sbeta;
static double	palpha,pbeta;
static char		*field;
static int		xdivs;
static short	fill_mode;
static double	xmin,xmax;
int		n;

    if(debug > 1){
	ActionHeader("VDepGate",gate,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
        /*
        ** check all of the messages to the gate
        */
		salpha = 0;
		sbeta = 0;
		palpha = 1;
		pbeta = 1;
        MSGLOOP(gate,msg) {
            case 0:				/* compartment */
        	/*
        	** 0 = membrane potential
        	*/
        	gate->activation = MSGVALUE(msg,0);
        	break;
			case 2:				/* summed alphas */
				salpha += MSGVALUE(msg,0);
			break;
			case 3:				/* product alphas */
				palpha *= MSGVALUE(msg,0);
			break;
			case 4:				/* summed betas */
				sbeta += MSGVALUE(msg,0);
			break;
			case 5:				/* product betas */
				pbeta *= MSGVALUE(msg,0);
			break;
        }

        /* 
        ** calculate the voltage dependent state variable m
        */
		if (gate->alpha_alloced)
        	alpha = TabInterp(gate->alpha,(double)(gate->activation)) * palpha + salpha;
		else
			alpha = palpha + salpha;

		if (gate->beta_alloced)
        	beta = TabInterp(gate->beta,(double)(gate->activation)) * pbeta + sbeta;
		else
			beta = pbeta + sbeta;

        if(gate->instantaneous){
            /*
            ** calculate the steady state value of the state variable
            */

            gate->m = alpha/(alpha+beta);
        } else {
        	gate->m = IntegrateMethod((int)(gate->object->method),
				gate,gate->m,alpha,alpha+beta,Clockrate(gate),"m");
        }
        break;
    case CHECK:
	if (!(gate->alpha_alloced && gate->beta_alloced)) {
		ErrorMessage("TabGate", "Rate tables not allocated.", gate);
	}
	break;
    case RESET:
        /*
        ** check all of the messages to the gate 
		** Hack : if there is no explicit message 6, it uses the
		** default which is just the memb potential
        */
        MSGLOOP(gate,msg) {
			case 0: 
        		gate->activation = MSGVALUE(msg,0);
			break;
        }
        MSGLOOP(gate,msg) {
            case 6:				/* Erest */
        		gate->activation = MSGVALUE(msg,0);
        	break;
        }
	if (gate->alpha_alloced && gate->beta_alloced) {

		/*
		** calculate the steady state value of the state variable
		*/
		alpha = TabInterp(gate->alpha,(double)(gate->activation));
		beta = TabInterp(gate->beta,(double)(gate->activation));
		gate->m = alpha/(alpha+beta);
	} else {
		gate->m = 0;
	}
        break;

	case SET :
		if (action->argc != 2)
			return(0); /* do the normal set */
		if (SetTable(gate,2,action->argv,action->data,"alpha beta"))
			return(1);

		if (strncmp(action->argv[0],"alpha",5) == 0)
			scale_table(gate->alpha,action->argv[0] + 7,action->argv[1]);
		else if (strncmp(action->argv[0],"beta",4) == 0)
			scale_table(gate->beta,action->argv[0] + 6,action->argv[1]);
		return(0); /* do the normal set */
		/* NOTREACHED */
		break;
    case SAVE2:
    	savedata = gate->m;
    	/* action->data contains the file pointer */
        n=1;
        fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(&savedata,sizeof(double),1,(FILE*)action->data);
    	break;

	case DUMP:
		if (action->argc == 1) {
        	if (strcmp(action->argv[0],"pre") == 0) {
            	return(0);
        	}
        	if (strcmp(action->argv[0],"post") == 0) {
            	FILE *fp = (FILE *)action->data ;
            	DumpInterpol(fp,gate,gate->alpha,"alpha");
            	DumpInterpol(fp,gate,gate->beta,"beta");
            	return(0);
        	}
    	}
		break;
    case RESTORE2:
    	/* action->data contains the file pointer */
        fread(&n,sizeof(int),1,(FILE*)action->data);
        if (n != 1) {
            ErrorMessage("TabGate", "Invalid savedata length", gate);
            return n;
        }
        fread(&savedata,sizeof(double),1,(FILE*)action->data);
    	gate->m = savedata;
    	break;

        /* Specially for the array fields */
    case TABCREATE:
		if (action->argc < 4) {
			printf("usage : %s field xdivs xmin xmax\n","tabcreate");
			return(0);
		}
		field = action->argv[0];
		xdivs = atoi(action->argv[1]);
		xmin = Atod(action->argv[2]);
		xmax = Atod(action->argv[3]);
		if (strcmp(field,"alpha") == 0) {
			gate->alpha = create_interpol(xdivs,xmin,xmax);
			gate->alpha_alloced = 1;
		} else if (strcmp(field,"beta") == 0) {
			gate->beta = create_interpol(xdivs,xmin,xmax);
			gate->beta_alloced = 1;
		} else {
			printf("field '%s' not known\n",field);
			return(0);
		}
        break;
    case TABFILL:
		if (action->argc < 3) {
			printf("usage : %s field xdivs fill_mode\n","tabfill");
			return(0);
		}
		field = action->argv[0];
		xdivs = atoi(action->argv[1]);
		fill_mode = atoi(action->argv[2]);
		if (strcmp(field,"alpha") == 0) {
			fill_table(gate->alpha,xdivs,fill_mode);
		} else if (strcmp(field,"beta") == 0) {
			fill_table(gate->beta,xdivs,fill_mode);
		} else {
			printf("field '%s' not known\n",field);
			return(0);
		}
        break;
    }
    return(0);
}

void TabGate_CALC_MINF(gate,action)
register struct tab_gate_type *gate;
Action      *action;
{
double	alpha,beta;

    if(action->argc > 0){
    gate->activation = Atof(action->argv[0]);
    } else {
    Error();
    printf("CALC_MINF action requires voltage argument\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	alpha = TabInterp(gate->alpha,(double)(gate->activation));
	beta = TabInterp(gate->beta,(double)(gate->activation));
    gate->m = alpha/(alpha+beta);
    action->passback = ftoa(gate->m);
}


void TabGate_CALC_ALPHA(gate,action)
register struct tab_gate_type *gate;
Action		*action;
{
double	RateState();
double	alpha;

    if(action->argc > 0){
	gate->activation = Atof(action->argv[0]);
    } else {
	Error();
	printf("CALC_ALPHA action requires voltage argument\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	alpha = TabInterp(gate->alpha,(double)(gate->activation));
    action->passback = ftoa(alpha);
}

void TabGate_CALC_BETA(gate,action)
register struct tab_gate_type *gate;
Action		*action;
{
double	RateState();
double	beta;

    if(action->argc > 0){
	gate->activation = Atof(action->argv[0]);
    } else {
	Error();
	printf("CALC_BETA action requires voltage argument\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	beta = TabInterp(gate->beta,(double)(gate->activation));
    action->passback = ftoa(beta);
}
