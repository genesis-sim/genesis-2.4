static char rcsid[] = "$Id: tab2channel.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/* Version EDS22d 97/05/04, written by Erik De Schutter, BBF-UIA 8/94-5/98 */

/*
** $Log: tab2channel.c,v $
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
** Revision 1.5  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.4  2000/06/12 05:07:03  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.3  2000/03/27 10:30:21  mhucka
** Fix from Hugo Cornelis: declare return type of GetScriptDouble().
**
** Revision 1.2  1998/06/30 23:25:43  dhb
** EDS 22d merge
**
** Revision 1.1  1997/05/29 08:21:37  dhb
 * EDS22d revison: EDS BBF-UIA 98/04/30-98/05/04
 * Added instantaneous gates to tab(2)channels
 *
** Initial revision
**
*/

#include <math.h>
#include "olf_ext.h"
#include "olf_defs.h"
#include "result.h"
#include "symtab.h"
static double       savedata[3];

double        GetScriptDouble();

void interpAB(indextype,CA,CB,v,c1,c2,A,B)
	short	indextype;
	Interpol2D *CA,*CB;
	double	v,c1,c2,*A,*B;
{
	double  v1=0.0;

	switch (indextype) {
		case VOLT_INDEX:
			*A = Tab2DInterp(CA,v1,v);
			*B = Tab2DInterp(CB,v1,v);
			break;

		case C1_INDEX:
		case DOMAIN_INDEX:
			*A = Tab2DInterp(CA,v1,c1);
			*B = Tab2DInterp(CB,v1,c1);
			break;

		case C2_INDEX:
			*A = Tab2DInterp(CA,v1,c2);
			*B = Tab2DInterp(CB,v1,c2);
			break;

		case VOLT_C1_INDEX:
		case VOLT_DOMAIN_INDEX:
			*A = Tab2DInterp(CA,v,c1);
			*B = Tab2DInterp(CB,v,c1);
			break;

		case VOLT_C2_INDEX:
			*A = Tab2DInterp(CA,v,c2);
			*B = Tab2DInterp(CB,v,c2);
			break;

		case C1_C2_INDEX:
		case DOMAIN_C2_INDEX:
			*A = Tab2DInterp(CA,c1,c2);
			*B = Tab2DInterp(CB,c1,c2);
			break;
	}
}

/* Tabulated hh-like channel.  
** Can have multiple gates: activation (X) and inactivation (Y) factors. 
** All are looked up in two-dimensional table.
** One-dimensional tables should have xdivs==0 and are indexed in y only.
*/

int Tab2DChannel(channel,action)
T2chan 	*channel;
Action	*action;
{
double 	v = 0.0,c1 = 0.0,c2 = 0.0;
double	A,B;
double	g;
double	dt;
int     int_method;
char    *field;
Interpol2D *create_interpol(),*create_interpol2d();
int     xdivs,ydivs,n0,instant;
double  xmin,xmax,ymin = 0.0,ymax = 0.0;
MsgIn	*msg;
Result  *rp,*SymtabLook();
extern Symtab GlobalSymbols;

    if(debug > 1){
	ActionHeader("tab2Dchannel",channel,action);
    }

    SELECT_ACTION(action){
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
	case CONCEN1:		/* second value */
	    /* 0 = concentration or whatever */
	    c1 = MSGVALUE(msg,0);
	    break;
	case CONCEN2:		/* third value */
	    /* 0 = concentration or whatever */
	    c2 = MSGVALUE(msg,0);
	    break;
	case DOMAINCONC:	/* domain CONCENTRATION: is a scaled Ik */
	    /* 0 = Ik */
	    /* 1 = scaling factor */
	    c1 = MSGVALUE(msg,0)*MSGVALUE(msg,1)/channel->surface;
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
	    interpAB(channel->Xindex,channel->X_A,channel->X_B,v,c1,c2,&A,&B);
	    if (instant & INSTANTX) {
		channel->X = A / B;
	    } else {
		channel->X=IntegrateMethod(int_method,channel,channel->X,A,B,dt,"X");
	    }
	    g *= pow(channel->X,channel->Xpower);
	}
	if(channel->Ypower != 0){
	    interpAB(channel->Yindex,channel->Y_A,channel->Y_B,v,c1,c2,&A,&B);
	    if (instant & INSTANTY) {
		channel->Y = A / B;
	    } else {
		channel->Y=IntegrateMethod(int_method,channel,channel->Y,A,B,dt,"Y");
	    }
	    g *= pow(channel->Y,channel->Ypower);
	}
	if(channel->Zpower > 0){
	    interpAB(channel->Zindex,channel->Z_A,channel->Z_B,v,c1,c2,&A,&B);
	    if (instant & INSTANTZ) {
		channel->Z = A / B;
	    } else {
		channel->Z=IntegrateMethod(int_method,channel,channel->Z,A,B,dt,"Z");
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
	c1=c2=0;
	/* calculate the conductance at Erest */
	g = channel->Gbar;
	MSGLOOP(channel,msg) {
	    case VOLTAGE:		/* Vm */
			v = MSGVALUE(msg,0);
			break;
	    case CONCEN1:		/* Co */
			c1 = MSGVALUE(msg,0);
			break;
	    case CONCEN2:		/* third value */
			c2 = MSGVALUE(msg,0);
			break;
	    case DOMAINCONC:		/* domain CONCENTRATION: is a scaled Ik */
			c1 = MSGVALUE(msg,0)*MSGVALUE(msg,1)/channel->surface;
			break;
            case ADD_GBAR:
      			g = MSGVALUE(msg,0) + g;
       			break;
	}
	/* calculate the active state variables x, y and z at steady 
	**  state and and calculate the conductance */
	if(channel->Xpower != 0){
		interpAB(channel->Xindex,channel->X_A,channel->X_B,v,c1,c2,&A,&B);
	    channel->X = A / B;
	    g = g * pow(channel->X,channel->Xpower);
	}
	if(channel->Ypower != 0){
		interpAB(channel->Yindex,channel->Y_A,channel->Y_B,v,c1,c2,&A,&B);
	    channel->Y = A / B;
	    g = g * pow(channel->Y,channel->Ypower);
	}
	if(channel->Zpower > 0){
		interpAB(channel->Zindex,channel->Z_A,channel->Z_B,v,c1,c2,&A,&B);
	    channel->Z = A / B;
	    g *= pow(channel->Z,channel->Zpower);
	}
	channel->Gk = g;
	channel->Ik = (channel->Ek - v) * g;
	break;

    case CHECK:
	v=c1=c2=0;
	MSGLOOP(channel,msg) {
	    case VOLTAGE:		/* Vm */
			v = 1;
			break;
	    case CONCEN1:		/* Co */
			c1 += 1;
			break;
	    case CONCEN2:		/* Co */
			c2 = 1;
			break;
	    case DOMAINCONC:		/* DCo */
			c1 += 1;
			if (channel->surface<=0) {
				ErrorMessage("tab2Dchannel","surface field not set.",channel);
			}
			break;
	} 
	if (c1>1) ErrorMessage("tab2Dchannel","CONCEN2 msg cannot be combined with DOMAINCONC msg.",channel);
	if (((channel->Xindex==VOLT_INDEX) ||
		 (channel->Xindex==VOLT_C1_INDEX) ||
		 (channel->Xindex==VOLT_C2_INDEX) ||
		 (channel->Yindex==VOLT_INDEX) ||
		 (channel->Yindex==VOLT_C1_INDEX) ||
		 (channel->Yindex==VOLT_C2_INDEX) ||
		 (channel->Zindex==VOLT_INDEX) ||
		 (channel->Zindex==VOLT_C1_INDEX) ||
		 (channel->Zindex==VOLT_C2_INDEX)) && (v<1))
			ErrorMessage("tab2Dchannel","Missing VOLTAGE msg.",channel);
	if (((channel->Xindex==C1_INDEX) ||
		 (channel->Xindex==VOLT_C1_INDEX) ||
		 (channel->Xindex==C1_C2_INDEX) ||
		 (channel->Yindex==C1_INDEX) ||
		 (channel->Yindex==VOLT_C1_INDEX) ||
		 (channel->Yindex==C1_C2_INDEX) ||
		 (channel->Zindex==C1_INDEX) ||
		 (channel->Zindex==VOLT_C1_INDEX) ||
		 (channel->Zindex==C1_C2_INDEX)) && (c1<1))
			ErrorMessage("tab2Dchannel","Missing CONCEN1 msg.",channel);
	if (((channel->Xindex==C2_INDEX) ||
		 (channel->Xindex==VOLT_C2_INDEX) ||
		 (channel->Xindex==C1_C2_INDEX) ||
		 (channel->Yindex==C2_INDEX) ||
		 (channel->Yindex==VOLT_C2_INDEX) ||
		 (channel->Yindex==C1_C2_INDEX) ||
		 (channel->Zindex==C2_INDEX) ||
		 (channel->Zindex==VOLT_C2_INDEX) ||
		 (channel->Zindex==C1_C2_INDEX)) && (c2<1))
			ErrorMessage("tab2Dchannel","Missing CONCEN2 msg.",channel);
	if (((channel->Xindex==DOMAIN_INDEX) ||
		 (channel->Xindex==VOLT_DOMAIN_INDEX) ||
		 (channel->Xindex==DOMAIN_C2_INDEX) ||
		 (channel->Yindex==DOMAIN_INDEX) ||
		 (channel->Yindex==VOLT_DOMAIN_INDEX) ||
		 (channel->Yindex==DOMAIN_C2_INDEX) ||
		 (channel->Zindex==DOMAIN_INDEX) ||
		 (channel->Zindex==VOLT_DOMAIN_INDEX) ||
		 (channel->Zindex==DOMAIN_C2_INDEX)) && (c1<1))
			ErrorMessage("tab2Dchannel","Missing DOMAINCONC msg.",channel);
	if ((channel->Xindex<DOMAIN_C2_INDEX)||(channel->Xindex>DOMAIN_INDEX)) {
		ErrorMessage("tab2Dchannel","Unknown Xindex.",channel);
	}
	if ((channel->Yindex<DOMAIN_C2_INDEX)||(channel->Yindex>DOMAIN_INDEX)) {
		ErrorMessage("tab2Dchannel","Unknown Yindex.",channel);
	}
	if ((channel->Zindex<DOMAIN_C2_INDEX)||(channel->Zindex>DOMAIN_INDEX)) {
		ErrorMessage("tab2Dchannel","Unknown Zindex.",channel);
	}
	if ((channel->Xpower != 0 && !channel->X_alloced) || 
	    (channel->Ypower != 0 && !channel->Y_alloced) ||
	    (channel->Zpower != 0 && !channel->Z_alloced)) { 
            ErrorMessage("tab2Dchannel","Factor tables not allocated.",channel);
	}
	break;
    case SET :
        if (action->argc != 2) return(0); /* do the normal set */
	if (strncmp(action->argv[0],"X_A",3) == 0)
		scale_table2d(channel->X_A,action->argv[0] + 5,action->argv[1]);
	else if (strncmp(action->argv[0],"X_B",3) == 0)
		scale_table2d(channel->X_B,action->argv[0] + 5,action->argv[1]);
	if (strncmp(action->argv[0],"Y_A",3) == 0)
		scale_table2d(channel->Y_A,action->argv[0] + 5,action->argv[1]);
	else if (strncmp(action->argv[0],"Y_B",3) == 0)
		scale_table2d(channel->Y_B,action->argv[0] + 5,action->argv[1]);
	if (strncmp(action->argv[0],"Z_A",3) == 0)
		scale_table2d(channel->Z_A,action->argv[0] + 5,action->argv[1]);
	else if (strncmp(action->argv[0],"Z_B",3) == 0)
		scale_table2d(channel->Z_B,action->argv[0] + 5,action->argv[1]);
	return(0); /* do the normal set */
	/* NOTREACHED */
	break;
    case SHOW:
	if (ShowInterpol(action,"X_A",channel->X_A,2,20)) return(1);
	if (ShowInterpol(action,"X_B",channel->X_B,2,20)) return(1);
	if (ShowInterpol(action,"Y_A",channel->Y_A,2,20)) return(1);
	if (ShowInterpol(action,"Y_B",channel->Y_B,2,20)) return(1);
	if (ShowInterpol(action,"Z_A",channel->Z_A,2,20)) return(1);
	return(ShowInterpol(action,"Z_B",channel->Z_B,2,20));
	/* NOTREACHED */
	break;
    case SAVE2:
	savedata[0] = channel->X;
	savedata[1] = channel->Y;
	savedata[2] = channel->Z;
	/* action->data contains the file pointer */
	n0=3;
	fwrite(&n0,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),n0,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n0,sizeof(int),1,(FILE*)action->data);
	if (n0!=3) ErrorMessage("tab2Dchannel","Invalid savedata length.",channel);
	fread(savedata,sizeof(double),3,(FILE*)action->data);
	channel->X = savedata[0];
	channel->Y = savedata[1];
	channel->Z = savedata[2];
	break;
 
    case TABCREATE:
	if (action->argc < 7) {
		printf("usage : %s field xdivs xmin xmax ydivs ymin ymax\n","TABCREATE");
		printf("  set xdivs to zero for 1-dimensional table\n");
		return(0);
	}
	field = action->argv[0];
	xdivs = atoi(action->argv[1]);
	xmin = Atod(action->argv[2]);
	xmax = Atod(action->argv[3]);
	if (action->argc >= 7) {
		ydivs = atoi(action->argv[4]);
		ymin = Atod(action->argv[5]);
		ymax = Atod(action->argv[6]);
	} else {
		ydivs=0;
	}
	if (strcmp(field,"X") == 0) {
		channel->X_A = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		channel->X_B = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		if (channel->X_A && channel->X_B) channel->X_alloced = 2;
	} else if (strcmp(field,"Y") == 0) {
		channel->Y_A = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		channel->Y_B = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		if (channel->Y_A && channel->Y_B) channel->Y_alloced = 2;
	} else if (strcmp(field,"Z") == 0) {
		channel->Z_A = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		channel->Z_B = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		if (channel->Z_A && channel->Z_B) channel->Z_alloced = 2;
	} else {
		printf("field '%s' not known\n",field);
		return(0);
	}
	break;
	}
	return(0);
}
