static char rcsid[] = "$Id: tabchannel.c,v 1.6 2006/02/03 21:24:26 svitak Exp $";

/*
** $Log: tabchannel.c,v $
** Revision 1.6  2006/02/03 21:24:26  svitak
** Fix for Voltage-dependent Z gate when power is greater than zero. Part of
** Jagerlab's additions.
**
** Revision 1.5  2006/01/17 15:05:00  svitak
** Changes for voltage-dependent Z gate and new purkinje tutorial.
**
** Revision 1.4  2006/01/10 19:56:39  svitak
** Changes to allow voltage-dependent Z-gate.
**
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
** Revision 1.12  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.11  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.10  1999/10/17 22:30:09  mhucka
** Added changes from Erik De Schuter dated circa March 1999.
** The definitions of TabChannel_SAVE, TabChannel_READ, TabChannel_ptrs have
** been moved to tabfiles.c.
**
 * EDS22h revison: EDS BBF-UIA 99/03/17-99/03/25
 * Moved TAB_SAVE and TAB_READ actions to tabfiles.cMovedfiles.c
 *
 * EDS22d revison: EDS BBF-UIA 98/04/30-98/05/04
 * Added instantaneous gates to tab(2)channels
 *
** Revision 1.8  1997/05/29 08:35:19  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21e revison: EDS BBF-UIA 97/05/03
 * Added code to check if EREST_ACT exists
 *
 * EDS21c revison: EDS BBF-UIA 95/08/02-95/10/05
 * Added ADD_GBAR msg and updated tabcurrent in TabChannel_SAVE and TabChannel_READ
 *
 * EDS20j revison: EDS BBF-UIA 95/07/24-95/08/11
 * Added DOMAINCONC msg and surface field
 * Updated ShowInterpol function call
 * Minor corrections to TabChannel_READ and TabChannel_SAVE
 *
 * EDS20i revison: EDS BBF-UIA 95/06/02
 * Added TAB_SAVE and TAB_READ actions.
 * Added olf_defs.h
 *
 * Revision 1.7  1996/08/06  20:52:43  dhb
 * Use GetScriptDouble() rather than get_script_float().
 *
 * Revision 1.6  1995/03/23  01:37:06  dhb
 * Added number of values to SAVE2 file format
 *
 * Revision 1.5  1995/03/21  01:00:46  venkat
 * Upi changes: Typecasted second argument to TabInterp() to double
 *
 * Revision 1.4  1994/10/04  23:01:21  dhb
 * Added TABDELETE action.
 *
 * Revision 1.3  1994/08/08  22:31:03  dhb
 * Changes from Upi.
 *
 * Revision 1.4  1994/06/13  22:52:24  bhalla
 * Added the SHOW and DUMP actions
 *
 * Revision 1.3  1994/05/26  13:52:10  bhalla
 * Changed syntax for SetTable
 *
 * Revision 1.2  1994/03/22  18:13:18  dhb
 * Change by Upi Bhalla for setting tables.
 *
 * Revision 1.1  1992/11/14  00:37:22  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "olf_ext.h"
#include "olf_defs.h"
#include "olf_g@.h"
#include "result.h"
#include "symtab.h"

static double       savedata[3];

/* E. De Schutter Caltech 1/91, modified Z 4/91 */
/* Tabulated hh-like channel.  The standard voltage dependent 
**	activation (X) and inactivation (Y) factors are present. For
**	concentration dependent processes a third factor (Z) has
**	added.  Z can do a lot of things, most important is that 
**	it gets another message (type1: c) than X and Y (type 0: v). 
**	Standard use for Z is inactivation, just use a Zpower>0.  
**	Alternative is to use it for codependent activation, use
**	Zpower<0, z will equal c times v-dependent table-value.
** For computation efficiency the forward rate factor alpha (A) and 
**	1/tau (B) are tabulated. 
*/
/* debugged, TABFILL added, CALC_MINF, CALC_ALPHA, CALC_BETA added
** by U.S. Bhalla Caltech 2/91 */
/*
** Generalized integration scheme implemented July 1991 by Upi Bhalla 
*/
/*
** Cengiz Gunay - January 2006
** Added a Z_conc flag indicating that the Z gate is concentration dependent.
** If Z_conc is 0 then it is voltage-dependent. Both the hines solver and
** the original code is modified to handle these cases. Hines chanmodes
** 1 and 4 tested and validated.
*/

int TabChannel(channel,action)
register struct tab_channel_type *channel;
Action		*action;
{
double 	c = 0.0,v = 0.0;
double	A,B;
double	g;
double	dt;
int     int_method;
char    *field;
int     xdivs,instant;
double  xmin,xmax;
Interpol *create_interpol();
short	fill_mode;
MsgIn	*msg;
extern double GetScriptDouble();
int	n;
Result  *rp,*SymtabLook();
extern Symtab GlobalSymbols;

    if(debug > 1){
	ActionHeader("tabchannel",channel,action);
    }

    SELECT_ACTION(action){
    case CREATE:
      channel->Z_conc = 1;	/* By default, the Z gate is concentration dependent */
      return (1);
      break;

    case INIT:
	channel->activation = 0;
	break;

    case PROCESS:
	dt = Clockrate(channel);
	instant = channel->instant;
	g = channel->Gbar;
	MSGLOOP(channel,msg) {
	case VOLTAGE:		/* membrane VOLTAGE */
	    /* 0 = membrane potential */
	    v = MSGVALUE(msg,0);
	    break;
	case CONCEN1:		/* ionic CONCENTRATION */
	    /* 0 = concentration */
	    c = MSGVALUE(msg,0);
	    break;
	case DOMAINCONC:	/* domain CONCENTRATION: is a scaled Ik */
	    /* 0 = Ik */
	    /* 1 = scaling factor */
	    c = MSGVALUE(msg,0)*MSGVALUE(msg,1)/channel->surface;
	    break;
	case EK:		/* reversal potential EK */
	    /* 0 = EK */
		channel->Ek = MSGVALUE(msg,0);
		break;
        case ADD_GBAR:
            /* 0 = scale_factor */
      	    g = MSGVALUE(msg,0) + g;
       	    break;
	}

	/* calculate the active state variables x, y and z and
	**  and calculate the conductance */
	int_method = (int)(channel->object->method);
	if(channel->Xpower != 0){
	    A = TabInterp(channel->X_A,v);
	    B = TabInterp(channel->X_B,v);
	    if (instant & INSTANTX) {
		channel->X = A / B;
	    } else {
		channel->X = IntegrateMethod(int_method,channel,
			channel->X,A,B,dt,"X");
	    }
	    g *= pow(channel->X,channel->Xpower);
	}
	if(channel->Ypower != 0){
	    A = TabInterp(channel->Y_A,v);
	    B = TabInterp(channel->Y_B,v);
	    if (instant & INSTANTY) {
		channel->Y = A / B;
	    } else {
		channel->Y = IntegrateMethod(int_method,channel,
			channel->Y,A,B,dt,"Y");
	    }
	    g *= pow(channel->Y,channel->Ypower);
	}
	if(channel->Zpower > 0 && channel->Z_conc){
	    A = TabInterp(channel->Z_A,c);
	    B = TabInterp(channel->Z_B,c);
	    if (instant & INSTANTZ) {
		channel->Z = A / B;
	    } else {
		channel->Z = IntegrateMethod(int_method,channel,
			channel->Z,A,B,dt,"Z");
	    }
	    g *= pow(channel->Z,channel->Zpower);
	} else if(channel->Zpower < 0) {
	    A = c * TabInterp(channel->Z_A,v);
	    B = TabInterp(channel->Z_B,v);
	    if (instant & INSTANTZ) {
		channel->Z = A / B;
	    } else {
		channel->Z = IntegrateMethod(int_method,channel,
			channel->Z,A,B,dt,"Z");
	    }
	    g *= pow(channel->Z,-channel->Zpower);
	} else if( channel->Zpower > 0 && ! channel->Z_conc ){
	    A = TabInterp(channel->Z_A,v);
	    B = TabInterp(channel->Z_B,v);
	    if (instant & INSTANTZ) {
		channel->Z = A / B;
	    } else {
		channel->Z = IntegrateMethod(int_method,channel,
			channel->Z,A,B,dt,"Z");
	    }
	    g *= pow(channel->Z,channel->Zpower);
	}

	/* calculate the transmembrane current */
	channel->Gk = g;
	channel->Ik = (channel->Ek - v) * g;
	break;

    case RESET:
	channel->activation = 0;
	rp=SymtabLook(&GlobalSymbols,"EREST_ACT"); /* check if script global exists */
	if (rp) v = GetScriptDouble("EREST_ACT");
	c = 0;
	/* calculate the conductance at Erest */
	g = channel->Gbar;
	MSGLOOP(channel,msg) {
	    case VOLTAGE:		/* Vm */
			v = MSGVALUE(msg,0);
			break;
	    case CONCEN1:		/* Co */
			c = MSGVALUE(msg,0);
			break;
	    case DOMAINCONC:		/* domain CONCENTRATION: is a scaled Ik */
			c = MSGVALUE(msg,0)*MSGVALUE(msg,1)/channel->surface;
			break;
            case ADD_GBAR:
      			g = MSGVALUE(msg,0) + g;
       			break;
	}
	/* calculate the active state variables x, y and z at steady 
	**  state and and calculate the conductance */
	if(channel->Xpower != 0){
	    A = TabInterp(channel->X_A,v);
	    B = TabInterp(channel->X_B,v);
	    channel->X = A / B;
	    g = g * pow(channel->X,channel->Xpower);
	}
	if(channel->Ypower != 0){
	    A = TabInterp(channel->Y_A,v);
	    B = TabInterp(channel->Y_B,v);
	    channel->Y = A / B;
	    g = g * pow(channel->Y,channel->Ypower);
	}
	if(channel->Zpower > 0 && channel->Z_conc){
	    A = TabInterp(channel->Z_A,c);
	    B = TabInterp(channel->Z_B,c);
	    channel->Z = A / B;
	    g *= pow(channel->Z,channel->Zpower);
	} else if(channel->Zpower < 0 && channel->Z_conc) {
	    A = c * TabInterp(channel->Z_A,v);
	    B = TabInterp(channel->Z_B,v);
	    channel->Z = A / B;
	    g *= pow(channel->Z,-channel->Zpower);
	} else if( channel->Zpower > 0 && ! channel->Z_conc ) {
	    A = TabInterp(channel->Z_A,v);
	    B = TabInterp(channel->Z_B,v);
	    channel->Z = A / B;
	    g *= pow(channel->Z,channel->Zpower);
	}
	channel->Gk = g;
	channel->Ik = (channel->Ek - v) * g;
	break;

    case CHECK:
	v = 0;
	c = 0;
	MSGLOOP(channel,msg) {
	    case VOLTAGE:		/* Vm */
			v = 1;
			break;
	    case CONCEN1:		/* Co */
			c = c+1;
			break;
	    case DOMAINCONC:		/* Co */
			c = c+1;
			if (channel->surface<=0) {
				ErrorMessage("tabchannel","surface field not set.",channel);
			}
			break;
	}
	if(v == 0){
	    ErrorMessage("tabchannel","Missing VOLTAGE msg.",channel);
	}
	if(c > 1){
	    ErrorMessage("tabchannel","CONCEN msg cannot be combined with DOMAINCONC msg.",channel);
	}
	if((channel->Zpower != 0) && channel->Z_conc && (c == 0)){
	    ErrorMessage("tabchannel","Missing CONCEN msg.",channel);
	}
	if ((channel->Xpower != 0 && !channel->X_alloced) || 
	    (channel->Ypower != 0 && !channel->Y_alloced) ||
	    (channel->Zpower != 0 && !channel->Z_alloced)) { 
            ErrorMessage("tabchannel", "Factor tables not allocated.",channel);
	}
	break;
    case SET :
        if (action->argc != 2)
            return(0); /* do the normal set */
		if (SetTable(channel,2,action->argv,action->data,
			"X_A X_B Y_A Y_B Z_A Z_B"))
			return(1);
        if (strncmp(action->argv[0],"X_A",3) == 0)
            scale_table(channel->X_A,action->argv[0] + 5,action->argv[1]);
        else if (strncmp(action->argv[0],"X_B",3) == 0)
            scale_table(channel->X_B,action->argv[0] + 5,action->argv[1]);
        else if (strncmp(action->argv[0],"Y_A",3) == 0)
            scale_table(channel->Y_A,action->argv[0] + 5,action->argv[1]);
        else if (strncmp(action->argv[0],"Y_B",3) == 0)
            scale_table(channel->Y_B,action->argv[0] + 5,action->argv[1]);
            return(0); /* do the normal set */
	    /* NOTREACHED */
	    break;
    case SHOW:
	if (ShowInterpol(action,"X_A",channel->X_A,channel->X_alloced,100)) 
		return(1);
	if (ShowInterpol(action,"X_B",channel->X_B,channel->X_alloced,100)) 
		return(1);
	if (ShowInterpol(action,"Y_A",channel->Y_A,channel->Y_alloced,100)) 
		return(1);
	if (ShowInterpol(action,"Y_B",channel->Y_B,channel->Y_alloced,100)) 
		return(1);
	if (ShowInterpol(action,"Z_A",channel->Z_A,channel->Z_alloced,100)) 
		return(1);
	return(ShowInterpol(action,"Z_B",channel->Z_B,channel->Z_alloced,100));
	/* NOTREACHED */
	break;

    case SAVE2:
	savedata[0] = channel->X;
	savedata[1] = channel->Y;
	savedata[2] = channel->Z;
	/* action->data contains the file pointer */
        n=3;
        fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),3,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
        fread(&n,sizeof(int),1,(FILE*)action->data);
        if (n != 3) {
            ErrorMessage("TabChannel", "Invalid savedata length", channel);
            return n;
        }
	fread(savedata,sizeof(double),3,(FILE*)action->data);
	channel->X = savedata[0];
	channel->Y = savedata[1];
	channel->Z = savedata[2];
	break;
	case DUMP:
	/* action->data contains the file pointer */
    	if (action->argc == 1) {
        	if (strcmp(action->argv[0],"pre") == 0) {
            	return(0);
        	}
        	if (strcmp(action->argv[0],"post") == 0) {
            	FILE *fp = (FILE *)action->data ;
            	DumpInterpol(fp,channel,channel->X_A,"X_A");
            	DumpInterpol(fp,channel,channel->X_B,"X_B");
            	DumpInterpol(fp,channel,channel->Y_A,"Y_A");
            	DumpInterpol(fp,channel,channel->Y_B,"Y_B");
            	DumpInterpol(fp,channel,channel->Z_A,"Z_A");
            	DumpInterpol(fp,channel,channel->Z_B,"Z_B");
            	return(0);
        	}
    	}
	break;
    case TABCREATE:
	if (action->argc < 4) {
		printf("usage : %s field xdivs xmin xmax\n","tabcreate");
		return(0);
	}
	field = action->argv[0];
	xdivs = atoi(action->argv[1]);
/* mds3 changes */
	xmin = Atod(action->argv[2]);
	xmax = Atod(action->argv[3]);
	if (strcmp(field,"X") == 0) {
		channel->X_A = create_interpol(xdivs,xmin,xmax);
		channel->X_B = create_interpol(xdivs,xmin,xmax);
		channel->X_alloced = 1;
	} else if (strcmp(field,"Y") == 0) {
		channel->Y_A = create_interpol(xdivs,xmin,xmax);
		channel->Y_B = create_interpol(xdivs,xmin,xmax);
		channel->Y_alloced = 1;
	} else if (strcmp(field,"Z") == 0) {
		channel->Z_A = create_interpol(xdivs,xmin,xmax);
		channel->Z_B = create_interpol(xdivs,xmin,xmax);
		channel->Z_alloced = 1;
	} else {
		printf("field '%s' not known\n",field);
		return(0);
	}
	break;
    case TABDELETE: /* MCV's fix to delete tabchannel interpol memory
		    ** NOTE: Can't do this in a DELETE action since we
		    ** don't know if some other table might be a reference
		    ** to this one.  Need reference counting.
		    */
	if (channel->X_alloced) {
	    free(channel->X_A->table);
	    free(channel->X_A);
	    channel->X_A = NULL;
	    free(channel->X_B->table);
	    free(channel->X_B);
	    channel->X_B = NULL;
	    channel->X_alloced = 0;
	}
	if (channel->Y_alloced) {
	    free(channel->Y_A->table);
	    free(channel->Y_A);
	    channel->Y_A = NULL;
	    free(channel->Y_B->table);
	    free(channel->Y_B);
	    channel->Y_B = NULL;
	    channel->Y_alloced = 0;
	}
	if (channel->Z_alloced) {
	    free(channel->Z_A->table);
	    free(channel->Z_A);
	    channel->Z_A = NULL;
	    free(channel->Z_B->table);
	    free(channel->Z_B);
	    channel->Z_B = NULL;
	    channel->Z_alloced = 0;
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
        if (strcmp(field,"X") == 0) {
            fill_table(channel->X_A,xdivs,fill_mode);
            fill_table(channel->X_B,xdivs,fill_mode);
        } else if (strcmp(field,"Y") == 0) {
            fill_table(channel->Y_A,xdivs,fill_mode);
            fill_table(channel->Y_B,xdivs,fill_mode);
        } else if (strcmp(field,"Z") == 0) {
            fill_table(channel->Z_A,xdivs,fill_mode);
            fill_table(channel->Z_B,xdivs,fill_mode);
        } else {
            printf("field '%s' not known\n",field);
            return(0);
        }
        break;
    }
    return(0);
}

void TabChannel_CALC_MINF(channel,action)
register struct tab_channel_type *channel;
Action      *action;
{
double	alpha,beta;
char	*gate;
Interpol	*ipa = NULL,*ipb = NULL;
double	m;

    if(action->argc == 2){
	gate = action->argv[0];
		if (strcmp(gate,"X") == 0) {
			ipa = channel->X_A;
			ipb = channel->X_B; 
		} else if (strcmp(gate,"Y") == 0) {
			ipa = channel->Y_A;
			ipb = channel->Y_B;
		} else if (strcmp(gate,"Z") == 0) {
			ipa = channel->Z_A;
			ipb = channel->Z_B;
		}
    	channel->activation = Atof(action->argv[1]);
    } else {
    Error();
    printf("usage : CALC_MINF gate voltage\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	alpha = TabInterp(ipa,(double)(channel->activation));
	/* remember that in tabchannels beta is 1/tau */
	beta = TabInterp(ipb,(double)(channel->activation));
    m = alpha/beta;
    action->passback = ftoa(m);
}


void TabChannel_CALC_ALPHA(channel,action)
register struct tab_channel_type *channel;
Action		*action;
{
double	alpha;
char	*gate;
Interpol	*ip = NULL;

    if(action->argc == 2){
	gate = action->argv[0];
		if (strcmp(gate,"X") == 0) {
			ip = channel->X_A;
		} else if (strcmp(gate,"Y") == 0) {
			ip = channel->Y_A;
		} else if (strcmp(gate,"Z") == 0) {
			ip = channel->Z_A;
		}
    	channel->activation = Atof(action->argv[1]);
    } else {
    Error();
    printf("usage : CALC_ALPHA gate voltage\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	alpha = TabInterp(ip,(double)(channel->activation));
    action->passback = ftoa(alpha);
}

void TabChannel_CALC_BETA(channel,action)
register struct tab_channel_type *channel;
Action		*action;
{
double	alpha,beta;
char	*gate;
Interpol	*ipa = NULL,*ipb = NULL;

    if(action->argc == 2){
	gate = action->argv[0];
		if (strcmp(gate,"X") == 0) {
			ipa = channel->X_A;
			ipb = channel->X_B; 
		} else if (strcmp(gate,"Y") == 0) {
			ipa = channel->Y_A;
			ipb = channel->Y_B;
		} else if (strcmp(gate,"Z") == 0) {
			ipa = channel->Z_A;
			ipb = channel->Z_B;
		}
    	channel->activation = Atof(action->argv[1]);
    } else {
    Error();
    printf("usage : CALC_MINF gate voltage\n");
    }
    /*
    ** calculate the steady state value of the state variable
    */
	alpha = TabInterp(ipa,(double)(channel->activation));
	/* remember that in tabchannels beta is 1/tau */
	beta = TabInterp(ipb,(double)(channel->activation));
    beta -= alpha;
    action->passback = ftoa(beta);
}
