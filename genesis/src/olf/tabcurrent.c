static char rcsid[] = "$Id: tabcurrent.c,v 1.3 2005/07/20 20:02:02 svitak Exp $";

/* Version EDS22d 98/06/16, Erik De Schutter, BBF-UIA 2/95-6/98 */

/*
** $Log: tabcurrent.c,v $
** Revision 1.3  2005/07/20 20:02:02  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.6  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.5  1998/06/30 23:25:43  dhb
** EDS 22d merge
**
 * EDS22d revison: EDS BBF-UIA 98/05/04-98/06/16
 * Corrected SetupGHK
 *
** Revision 1.4  1997/07/18 20:08:45  dhb
** Change from PSC: fix for missing dereference of xdivs pointer in
** getsetuptabcurr().
**
** Revision 1.3  1997/07/18 03:07:15  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1997/06/03 05:55:31  dhb
** Added missing value to return statement in getsetuptabcurr().
**
** Revision 1.1  1997/05/29 08:21:37  dhb
** Initial revision
**
*/

#include <math.h>
#include "olf_ext.h"
#include "olf_defs.h"

/* Tabulated ionic current: this requires values for Ik and for the
**  slope conductance Gk'.  Ek is computed assuming linearity over one 
**  timestep.  Changes are instantenous.  Can also be used to compute
**  the Goldman-Hodgkin-Katz equation using the ADD_GBAR message to
**  change the calcium permeability. */

int TabCurrent(current,action)
    Tcurr 	*current;
    Action	*action;
{
    double 	v = 0.0,c1 = 0.0,c2 = 0.0;
    double	Gbar,I=0.0,G=0.0;
    int    	xdivs,ydivs,index;
    double   	xmin,xmax,ymin,ymax;
    char    	*field;
    Interpol2D 	*create_interpol2d();
    double  	interpF();
    MsgIn	*msg;

    if(debug > 1){
		ActionHeader("tabcurrent",current,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
    case RESET:
    	Gbar=current->Gbar;
	MSGLOOP(current,msg) {
		case VOLTAGE:
			/* 0 = membrane potential or other first index variable */
			v = MSGVALUE(msg,0);
			break;
		case CONCEN1:
			/* 0 = concentration or other second index variable */
			c1 = MSGVALUE(msg,0);
			break;
		case CONCEN2:
			/* 0 = concentration or other second index variable */
			c2 = MSGVALUE(msg,0);
			break;
        	case DOMAINCONC:     /* domain CONCENTRATION: is a scaled Ik */
			/* 0 = concentration, 1 = scale_factor */
            		c1 = MSGVALUE(msg,0)*MSGVALUE(msg,1)/current->surface;
        	case ADD_GBAR:
        	        /* 0 = scale_factor */
            		Gbar = MSGVALUE(msg,0) + Gbar;
            		break;
	}

	/* calculate Gk  */
	if (current->alloced) G=interpF(current->Gindex,current->G_tab,v,c1,c2);
	current->Gk = Gbar * G;
	/* calculate Ik  */
	if (current->alloced) I=interpF(current->Gindex,current->I_tab,v,c1,c2);
	current->Ik = Gbar * I;
	/* calculate Ek */
	if (fabs(G)>10e-30) current->Ek = (I + G*v)/G;
	break;

    case SHOW:
	if (ShowInterpol(action,"G_tab",current->G_tab,2,20)) return(1); 
	return(ShowInterpol(action,"I_tab",current->I_tab,2,20)); 

    case CHECK:
	v = 0;
	MSGLOOP(current,msg) {
	case VOLTAGE:		/* Vm */
	    v = 1;
	    break;
        case CONCEN1:     /* Co */
            c1 += 1;
            break;
        case CONCEN2:     /* third value */
            c2 =  1;
            break;
        case DOMAINCONC:     /* DoCo */
            c1 += 1;
	    if (current->surface<=0) {
			ErrorMessage("tabcurrent","surface field not set.",current);
	    }
            break;
        default:
            break;

	}
    
    if (v==0) ErrorMessage("tabcurrent","Missing VOLTAGE msg: Ek will not be computed correctly.",current);
    if (c1>1) ErrorMessage("tabcurrent","CONCEN2 msg cannot be combined with DOMAINCONC msg.",current);
	index=current->Gindex;
    if (((index==VOLT_INDEX) || (index==VOLT_C1_INDEX) ||
         (index==VOLT_C2_INDEX)) && (v<1))
            ErrorMessage("tabcurrent","Missing VOLTAGE msg.",current);
    if (((index==C1_INDEX) || (index==VOLT_C1_INDEX) ||
         (index==C1_C2_INDEX)) && (c1<1))
            ErrorMessage("tabcurrent","Missing CONCEN1 msg.",current);
    if (((index==C2_INDEX) || (index==VOLT_C2_INDEX) ||
         (index==C1_C2_INDEX)) && (c2<1))
            ErrorMessage("tabcurrent","Missing CONCEN2 msg.",current);
    if (((index==DOMAIN_INDEX) || (index==VOLT_DOMAIN_INDEX) ||
         (index==DOMAIN_C2_INDEX)) && (c1<1))
            ErrorMessage("tabcurrent","Missing DOMAINCONC msg.",current);
    if (current->alloced == 0) {
        ErrorMessage("tabcurrent", "Itab and G-tab tables not allocated.",current);
    }
    if ((index<DOMAIN_C2_INDEX)||(index>DOMAIN_INDEX)) {
	ErrorMessage("tabcurrent","Unknown Gindex.",current);
    }

	break;

    case TABCREATE:
	if (action->argc < 7) {
		printf("usage : %s field xdivs xmin xmax ydivs ymin ymax\n","TABCREATE");
		printf("  G_tab and I_tab are always created together\n");
		printf("  set xdivs to zero for 1-dimensional table\n");
		return(0);
	}
	if (current->alloced) {
		Error();
		printf(" G_tab and I_tab have already been created in %s.",Pathname(current));
		return 0;
	}
	field = action->argv[0];
	xdivs = atoi(action->argv[1]);
	xmin = Atod(action->argv[2]);
	xmax = Atod(action->argv[3]);
	ydivs = atoi(action->argv[4]);
	ymin = Atod(action->argv[5]);
	ymax = Atod(action->argv[6]);
	if ((strcmp(field,"I_tab") == 0)||(strcmp(field,"G_tab") == 0)) {
		current->I_tab = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		current->G_tab = create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax);
		current->alloced = 2;
	} else {
		printf("field '%s' not known\n",field);
	}

	break;
	}
	return(0);
}

/* Routine to fill a tabcurrent with GHK tables */
void SetupGHK(argc,argv)
    int argc;
    char	**argv;

{
    int		i,j;
    float 	charge,temp;
    double 	const1,const2,v_const,e_const,v,v0,dv,ci,c1,dc1,co,c2,dc2;
    double	numer,denom;
    int    	xdivs,ydivs;
    Tcurr 	*current,*getsetuptabcurr();
    double	**itable,**gtable;
    Interpol2D 	*ip;
    int		getsetuptabcurrent();

	/* get user variables */
	initopt(argc, argv, "tabcurrent-element charge Celcius [Cin,0] [Cout,0] -xsize n -xrange min max -ysize n -yrange min max");
	current=getsetuptabcurr(argc,argv,&xdivs,&ydivs);
	if (!current) return;
	charge=Atof(optargv[2]);
	temp=Atof(optargv[3]) + ZERO_CELSIUS;
	c1=Atof(optargv[4]);
	c2=Atof(optargv[5]);
	if (((c1==0.0)&&(c2==0.0))||((xdivs>0)&&(c1>0.0)&&(c2>0.0))) {
		Error();
		printf(" either Cin or Cout should be a constant larger than zero.\n");
		return;
	}
	
	/* determine basic variables */
	ip=current->I_tab;
	if (xdivs) {	/* 2D table */
		v0=ip->xmin;
		dv=ip->dx;
		if (c1 > 0.0) {		/* Cout is the variable */
			c2=ip->ymin;
			dc2=ip->dy;
			dc1=0.0;
		} else  {		/* Cin is the variable */
			c1=ip->ymin;
			dc1=ip->dy;
			dc2=0.0;
		}
	} else {	/* 1D table */
		v0=ip->ymin;
		dv=ip->dy;
		dc1=0.0;
		dc2=0.0;
	}

	/* fill the tables */
	itable=current->I_tab->table;
	gtable=current->G_tab->table;
	const1=charge*FARADAY/(GAS_CONSTANT*temp);
	const2=charge*FARADAY;	/* correct sign convention */
	v=v0;
	for (i=0; i<=xdivs; i++) {
		ci=c1;
		co=c2;
		v_const=v*const1;
		e_const=exp(-v_const);
		for (j=0; j<=ydivs; j++) {
			numer=const2 * (ci - co*e_const);
			if (fabs(v_const)<SING_TINY) {
			    /* sign reversed to keep Gk positive */ 
			    /* =1/2(Co+Ci)*z^2*F^2/RT */
			    gtable[i][j]=0.5*(ci+co)*const2*const1;
			    /* inward current positive */
			    itable[i][j]=-numer/(1.0 - 0.5*v_const);
			} else {
			    denom=1.0 - e_const;
			    /* sign reversed to keep Gk positive */ 
			    gtable[i][j]=const1*(numer/denom - 
			    	const1*const2*(ci-co)*v*e_const/(denom*denom));	
			    /* inward current positive */
			    itable[i][j]=-v_const*numer/denom;
			}
			ci+=dc1;
			co+=dc2;
		}
		v+=dv;
	}
}

/* Routine to fill a tabcurrent with NaCa current tables
** Reference: DiFrancesco, D., and Noble, D. (1985) A model of cardiac electrical 
**            activity incorporating ionic pumps and concentration changes. 
**            Phil. Trans. Roy. Soc. London Ser. B 307: 353-398.
** Gamma is normally 0.38
*/
void SetupNaCa(argc,argv)
    int argc;
    char	**argv;

{
    int		i,j;
    float 	temp;
    double 	gamma,gamma_1,const1,v_const,e_const1,e_const2;
    double	v,v0,dv,ci,c1,dc1,co,c2,dc2,n1,n2;
    int    	xdivs,ydivs;
    Tcurr 	*current,*getsetuptabcurr();
    double	**itable,**gtable;
    Interpol2D 	*ip;
    int		getsetuptabcurrent();

	/* get user variables */
	initopt(argc, argv, "tabcurrent-element gamma Celcius [Cain,0] [Caout,0] Nain Naout -xsize n -xrange min max -ysize n -yrange min max");
	current=getsetuptabcurr(argc,argv,&xdivs,&ydivs);
	if (!current) return;
	gamma=Atof(optargv[2]);
	temp=Atof(optargv[3]) + ZERO_CELSIUS;
	c1=Atof(optargv[4]);
	c2=Atof(optargv[5]);
	n1=Atof(optargv[6]);
	n2=Atof(optargv[7]);
	if (gamma<0.0) {
		Error();
		printf(" gamma should be a positive value.\n");
		return;
	}
	if (((c1==0.0)&&(c2==0.0))||((xdivs>0)&&(c1>0.0)&&(c2>0.0))) {
		Error();
		printf(" either Cin or Cout should be a constant larger than zero.\n");
		return;
	}
	if ((n1<0.0)||(n2<0.0)) {
		Error();
		printf(" Nain and Naout should be positive values.\n");
		return;
	}
	
	/* determine basic variables */
	ip=current->I_tab;
	if (xdivs) {	/* 2D table */
		v0=ip->xmin;
		dv=ip->dx;
		if (c1 > 0.0) {		/* Cout is the variable */
			c2=ip->ymin;
			dc2=ip->dy;
			dc1=0.0;
		} else  {		/* Cin is the variable */
			c1=ip->ymin;
			dc1=ip->dy;
			dc2=0.0;
		}
	} else {	/* 1D table */
		v0=ip->ymin;
		dv=ip->dy;
		dc1=0.0;
		dc2=0.0;
	}

	/* fill the tables */
	itable=current->I_tab->table;
	gtable=current->G_tab->table;
	const1=FARADAY/(GAS_CONSTANT*temp);
	v=v0;
	n1=n1*n1*n1;	/* [Na]i^3 */
	n2=n2*n2*n2;	/* [Na]o^3 */
	gamma_1=gamma-1.0;
	for (i=0; i<=xdivs; i++) {
		ci=c1;
		co=c2;
		v_const=v*const1;
		e_const1=exp(gamma*v_const);
		e_const2=exp(gamma_1*v_const);
		for (j=0; j<=ydivs; j++) {
			/* sign reversed to keep Gk positive */ 
			gtable[i][j]=const1*(gamma*co*n1*e_const1
						- gamma_1*ci*n2*e_const2);	
			/* inward current positive */
			itable[i][j]=ci*n2*e_const2-co*n1*e_const1;
			ci+=dc1;
			co+=dc2;
		}
		v+=dv;
	}
}

/* Routine to prepare for SetupGHK and SetupNaCa */
Tcurr *getsetuptabcurr(argc,argv,xdivs,ydivs)
    int		argc;
    char	**argv;
    int    	*xdivs,*ydivs;

{
    Element	*elm;
    double   	xmin,xmax,ymin,ymax;
    Interpol2D 	*ip,*create_interpol2d();
    Tcurr 	*current;
    int		status;

	*xdivs=*ydivs=DEFAULT_2DIVS;
	xmin=DEFAULT_XMIN;
	xmax=DEFAULT_XMAX;    
	ymin=DEFAULT_YMIN;
	ymax=DEFAULT_YMAX;

	/* get user variables */
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-xsize") == 0) {
		*xdivs = atoi(optargv[1]);
	    } else if (strcmp(G_optopt, "-xrange") == 0) {
		xmin = Atod(optargv[1]);
		xmax = Atod(optargv[2]);
	    } else if (strcmp(G_optopt, "-ysize") == 0) {
		*ydivs = atoi(optargv[1]);
	    } else if (strcmp(G_optopt, "-yrange") == 0) {
		ymin = Atod(optargv[1]);
		ymax = Atod(optargv[2]);
	    }
	  }

	if (status < 0)
	  {
	    printoptusage(argc, argv);
	    return(NULL);
	  }

	elm = GetElement(optargv[1]);
	if (!elm) {
		Error();
		printf(" element '%s' not found\n",optargv[1]);
		return(NULL);
	}
	if (strcmp(BaseObject(elm)->name,"tabcurrent")!=0) {
		Error();
		printf(" element '%s' is not a tabcurrent!\n",optargv[1]);
		return(NULL);
	}
	current=(Tcurr *)elm;
	if ((current->Gindex<VOLT_C1_INDEX)||(current->Gindex>VOLT_INDEX)) {
		Error();
		printf(" Gindex should be VOLT_INDEX or VOLT_C1_INDEX in %s.\n",Pathname(elm));
		return(NULL);
	}
	if ((current->Gindex==VOLT_INDEX)&&(*xdivs>0)) {
		Error();
		printf(" xdivs should be 0 if Gindex is VOLT_INDEX in %s.\n",Pathname(elm));
		return(NULL);
	}
	
	/* create the tables */
	if (current->alloced) {
		printf(" using existing I_tab and G_tab in %s.\n",Pathname(elm));
		if (current->I_tab) {
			ip=current->I_tab;
		} else {
			ip=current->G_tab;
		}
		*xdivs=ip->xdivs;
		xmin=ip->xmin;
		xmax=ip->xmax;
		*ydivs=ip->ydivs;
		ymin=ip->ymin;
		ymax=ip->ymax;
	}
	if (!current->I_tab) 
		current->I_tab = create_interpol2d(*xdivs,xmin,xmax,*ydivs,ymin,ymax);
	if (!current->G_tab) 
		current->G_tab = create_interpol2d(*xdivs,xmin,xmax,*ydivs,ymin,ymax);
	current->alloced = 2;
	return(current);
}

double interpF(indextype,tab,v,c1,c2)
	short   indextype;
	Interpol2D *tab;
	double  v,c1,c2;
{
	double  v1=0.0;

	switch (indextype) {
		case VOLT_INDEX:
			return(Tab2DInterp(tab,v1,v));

		case C1_INDEX:
		case DOMAIN_INDEX:
			return(Tab2DInterp(tab,v1,c1));

		case C2_INDEX:
			return(Tab2DInterp(tab,v1,c2));

		case VOLT_C1_INDEX:
		case VOLT_DOMAIN_INDEX:
			return(Tab2DInterp(tab,v,c1));

		case VOLT_C2_INDEX:
			return(Tab2DInterp(tab,v,c2));

		case C1_C2_INDEX:
		case DOMAIN_C2_INDEX:
			return(Tab2DInterp(tab,c1,c2));
	}
	printf("interpF: falling off switch, returning 0.0\n");
    return 0.0;
}
