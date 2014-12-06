static char rcsid[] = "$Id: new_interp.c,v 1.3 2005/07/20 19:38:07 svitak Exp $";

/*
** $Log: new_interp.c,v $
** Revision 1.3  2005/07/20 19:38:07  svitak
** Removed pre-processor logic for GNUC 2.6.3 OPTIMIZE issue. Comment was added
** to README regarding this issue.
**
** math.h and stdio.h added when needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.27  2001/06/29 21:18:16  mhucka
** Added extra parens inside conditionals and { } groupings to quiet certain
** compiler warnings.
**
** Revision 1.26  2001/04/18 22:39:37  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.25  2000/07/08 01:46:19  mhucka
** Bug fix from Dave Beeman: TabToFile was adding B + A when it
** should have been subtracting B - A, in the case of beta.
**
** Revision 1.24  2000/06/19 06:14:30  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.23  2000/06/12 04:46:08  mhucka
** 1) Fixed trivial errors in some printf format strings.
** 2) Added NOTREACHED comments where appropriate.
**
** Revision 1.22  1999/10/17 22:26:37  mhucka
** Changes from Upi Bhalla dated circa Jan 1998, involving the addition of a
** check for very low values in scale_table().  It is not clear to me whether
** this fix should be incorporated, but it appears reasonable and so I'm adding
** it in.
**
** Revision 1.21  1998/07/15 06:49:48  dhb
** Merged 1.19.1.1
**
 * Revision 1.20  1998/06/30  21:27:49  dhb
 * Changed interface to setup_tab_values to make it more easily
 * callable from C programs.  SetupGeneric() now does what
 * setup_tab_values() used to do (e.g. take argc, argv, mode).
 *
 * Revision 1.19.1.1  1998/07/15  06:48:06  dhb
 * Upi update
 *
** Revision 1.19  1997/07/23 23:40:04  dhb
** Changed checks for == 0.0 to fabs() < SING_TINY to avoid NaN problems.
** Added div by zero check to tweak_tab_values().
**
** Revision 1.18  1997/07/18 03:07:15  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.17  1997/05/29 08:29:21  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21d revison: EDS BBF-UIA 97/01/28
 * setup_tab_values does correct thing
 *
 * EDS21c revison: EDS BBF-UIA 96/10/02
 * Removed inconsequential tab2DChannel from setup_tab_values.
 * Moved SING_TINY to olf_defs.h
 *
 * EDS20j revison: EDS BBF-UIA 95/08/11
 * Updated ShowInterpol function to display 
 *
 * EDS20i revison: EDS BBF-UIA 95/06/08
 * Added tab2Dchannel and Tchan typedefs to setup_tab_values
 * Moved #defines to olf_defs.h, added olf_defs.h
 *
 * Revision 1.16  1996/07/25  19:46:57  dhb
 * FIxed bug in SetTable() which wasn't checking for table names
 * correctly.
 *
 * Revision 1.15  1996/05/24  22:11:47  dhb
 * Removed 1.12 changes for setupalpha singularities
 *
 * Revision 1.14  1995/11/04  20:40:13  dhb
 * Changed to always use the exp10 macro as other systems (SGI) are
 * also missing the function.
 *
 * Revision 1.13  1995/11/04  08:08:28  dhb
 * Fix to bug in TabInterp() handling of LIN_INTERP case reported
 * by Erik DeSchuter.  Code was trying to interpolate when the
 * value maps between table[-1] and table[0] instead of pegging
 * as table[0].
 *
 * Revision 1.12  1995/09/27  22:43:38  dhb
 * Merged in Mike Vanier's setupalpha changes.
 *
 * Revision 1.11  1995/09/26  22:03:45  dhb
 * Changes from Upi Bhalla:
 *
 *    Fix to GNU OPTIMIZE define hack.  Old code neglected to include
 *    stdlib.h if GNU was not defined.
 *
 *    exp and log function on tables.
 *
 *    loadtab -continue option allows additional table data to be
 *    added to a table on dump restores.  This works around command
 *    length limitations in the script language.
 *
 *    -xy option now has an xdivs option.
 *
 *    -xy now loads into doubles rather than floats to preserve
 *    precision.
 *
 * Revision 1.10  1995/07/28  23:27:20  dhb
 * Undefine __OPTIMIZE__ macro for gcc which causes incompatible
 * redefinition of functions in stdlib.h.
 *
 * Revision 1.9  1995/07/08  00:33:02  venkat
 * alloc_interpoltable() definition - Needed for situations where only the table
 * of the interpol is alloced.
 *
 * Revision 1.8  1995/06/16  05:58:46  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.7  1995/03/21  00:58:47  venkat
 * A number of Upi changes
 *
 * Revision 1.6  1994/08/08  22:28:29  dhb
 * Changes from Upi.
 *
 * Revision 1.8  1994/06/13  22:49:24  bhalla
 * Put in major enhancment to the SetTable function
 * implemented ShowInterpol function
 *
 * Revision 1.7  1994/05/25  13:59:18  bhalla
 * Implemented new version of SetTable, with all sorts of enhancements
 *
 * Revision 1.6  1994/05/09  16:17:17  bhalla
 * Updated TabToFile
 *
 * Revision 1.5  1994/04/08  23:21:39  dhb
 * Update from Upi
 *
 * Revision 1.3  1994/04/06  23:03:56  bhalla
 * Updated FileToTab to be more general and to use the SetTable method
 * for setting table. Many options added.
 *
 * Revision 1.2  1994/04/06  20:59:41  bhalla
 * All sorts of stuff missed out here, in the log.
 * 1. Added TabToFile and FileToTab which themselves are 1991 vintage
 * 2. Added and replaced LoadTab used in simdump
 * 3. Added SetTable which allows tables to be treated like other fields
 *
 * Revision 1.1  1994/01/13  20:59:46  bhalla
 * Initial revision
 *
 * Revision 1.3  1993/07/09  14:45:51  dhb
 * (1) cosmetic changes to call the second argument "gate" instead of "table"
 * (2) change in the way the index "j" is computed.  This makes ox and sx
 *     behave in a way which is consistent with setting these fields using
 *     scale_table.  (However, as before, ScaleTabchan modifies table values
 *     instead of changing xmin and xmax.)
 * (3) Return with error for sx <= 0.  (sx == 0 would cause a divide by zero.)
 * (4) Fixed bug which kept it from working with tabgates.  In order to use the
 *     same code for scaling, the tabgate "beta" table is temporarily changed
 *     to alpha + beta.  As before, it is changed back after scaling.
 *
 * Revision 1.2  1993/02/25  22:18:45  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * Revision 1.1  1992/11/14  00:37:20  dhb
 * Initial revision
 *
*/

#include <math.h>
#include <stdio.h>	/* needed by stdlib.h on some archs */
#include <stdlib.h>
#include <string.h>
#include "olf_ext.h"
#include "olf_defs.h"

static Interpol *GetTabByName();

/* 
** Following calc_modes for interpolation : 
**	interpolation 
**		1 Linear (default, simpler to use and backwards compatible)
**		2 no interpolation. (more efficient)
**
**	extrapolation always uses
**		1 Truncation 
**
**	Special mode : Fixed value, which returns the y offset of the table
**
*/

/*
** The table consists of xdivs + 1 points. Table[0] and Table[xdivs]
** are used as truncation values.
*/

double TabInterp(ip,x)
	Interpol *ip;
	double x;
{
	register double lo;
	register int ilo;
	/*
	register double *table;
	*/

	switch(ip->calc_mode) {
		case NO_INTERP :
			/*
			if(table=ip->table) {
				ilo = ((x - ip->xmin)*ip->invdx);
				if (ilo<0) return(table[0]);
				if (ilo>ip->xdivs) return(table[ip->xdivs]);
				return(table[ilo]);
			} else {
				return(0.0);
			}
			*/
			if(ip->table) {
				ilo = ((x - ip->xmin)*ip->invdx);
				if (ilo<0) return(ip->table[0]);
				if (ilo>ip->xdivs) return(ip->table[ip->xdivs]);
				return(ip->table[ilo]);
			} else {
				return(0.0);
			}
			/* NOTREACHED */
			break;
		case LIN_INTERP :
			if(ip->table) { 
				ilo = lo = (x - ip->xmin)*ip->invdx;
				if (lo <= 0.0){
					return(ip->table[0]);
				} else if (lo >= ip->xdivs) {
					return(ip->table[ip->xdivs]);
				} else {
					x=ip->table[ilo];
					return((lo-(double)ilo)*(ip->table[ilo+1]-x)+x);
				}
			} else {
				return(0.0);
			}
			/*
			if((table = ip->table) == NULL) return(0.0);
			ilo = lo = (x - ip->xmin)*ip->invdx;
			if (ilo < 0){
				return(table[0]);
			} else if (ilo > ip->xdivs) {
				return(table[ip->xdivs]);
			} else {
				return((lo - (double)ilo)*(table[ilo+1] - table[ilo]) + table[ilo]);
			}
			*/
			/* NOTREACHED */
			break;
		case FIXED :
			return(ip->oy);
			/* NOTREACHED */
			break;
	}
	return(0.0);
}

Interpol *create_interpol(xdivs,xmin,xmax)
	int xdivs;
	double xmin,xmax;
{
	Interpol *ip;

	ip = (Interpol *) calloc(1,sizeof(Interpol));

	ip->xdivs = xdivs;
	ip->xmin = xmin;
	ip->xmax = xmax;
	ip->table = (double *)calloc(xdivs + 1,sizeof(double));
	ip->allocated = 1;
	ip->calc_mode = LIN_INTERP;
	ip->sx = ip->sy = 1.0;
	ip->ox = ip->oy = 0.0;

	ip->dx = (ip->xmax - ip->xmin) / (double)(ip->xdivs);
	if (fabs(ip->dx) > 1e-50)
                ip->invdx=1.0/ip->dx;
        else
                ip->invdx=1.0;
	return(ip);
}

int alloc_interpoltable(ip, xdivs)
	Interpol *ip;
	int xdivs;
{
	if((ip == NULL) || (xdivs < 0)) 
		return(0);
	ip->xdivs = xdivs;
	if(ip->table != NULL) free(ip->table);
	if((ip->table = (double *)calloc(xdivs+1, sizeof(double)))== NULL)
		return(0);
	else
		return(1);
}

/*
** Special function for filling table for nointerp :
** fill_table(interpol, npts, mode)
** where interpol gives the existing interpol pointer, and mode
** specifies linear, cubic spline or B_spline fitting
** The function takes the existing table in the interpol and uses it
** to create an expanded table with npts data values, using the
** specified interpolation method. The original table is then tossed.
*/

int fill_table(ip,npts,fill_mode)
	Interpol	*ip;
	int			npts;
	short		fill_mode;
{
	double	*old_tab;
	double	*new_tab;
	double	lo,dlo;
	double	t,t1,t2,t2a,t3,t3a;
	double	nc1,nc2,nc3,nc4;
	double	nsa,nsb;
	int		i,j;
	int		xxs;
	int		xdivs;
	int		bad_flag=0;

	if ((old_tab = ip->table) == NULL) {
		printf("Error : NULL table. Filling aborted\n");
		return(0);
	}
	if (npts < 3) {
		printf("Error : %d divisions for table is invalid. Filling aborted\n",npts);
		return(0);
	}

	new_tab = (double *)calloc(npts + 1, sizeof(double));
	xdivs = ip->xdivs;

	switch (fill_mode) {
		case LINEAR_FILL :
			dlo = (double)xdivs / (double)npts ;
			/* i is index for old array, j for new array */
			for (lo=0.0,i=0,j=0;j<=npts;lo+=dlo,i=(int)lo,j++) {
				if (i < xdivs)
					new_tab[j] = (lo - (double)i)*
						(old_tab[i+1] - old_tab[i]) + old_tab[i];
				else
					new_tab[j] = old_tab[xdivs];
			}
			break;
		case C_SPLINE_FILL :
			printf("C_SPLINE_FILL not yet implemented\n");
			bad_flag = 1;
			break;
		case B_SPLINE_FILL :
			xxs=xdivs-2;nsa=1.0/6.0;nsb=2.0/3.0;
			dlo = (double)xdivs / (double)npts ;
			/* i is index for old array, j for new array */
			/* filling up newtab till first element in oldtab */
			for (lo = 0.0,j=0,i=0;i<=1;lo+=dlo,i=(int)lo,j++) {
				new_tab[j] = (lo - (double)i)*
					(old_tab[i+1] - old_tab[i]) + old_tab[i];
			}
			/* lo,i, and j carry on from where they were last time.*/
			for(;i<=xxs;lo+=dlo,i=(int)lo,j++){
				t = lo - (double)i;
				t1=t/2.0;
				t2=t*t;
				t2a=t2/2.0;
				t3=t2*t;
				t3a=t3/2.0;
				nc1= -nsa*t3+t2a-t1+nsa;
				nc2=t3a-t2+nsb;
				nc3= -t3a+t2a+t1+nsa;
				nc4=nsa*t3;
				new_tab[j]=nc1*old_tab[i-1] + nc2*old_tab[i] +
					nc3*old_tab[i+1] + nc4*old_tab[i+2];
			}
			/* lo, i, and j carry on from where they were last time.*/
			for (; j <= npts ; lo += dlo,i=(int)lo , j++) {
				if (i < xdivs)
					new_tab[j] = (lo - (double)i)*
						(old_tab[i+1] - old_tab[i]) + old_tab[i];
				else
					new_tab[j] = old_tab[xdivs];
			}
			break;
		default :
			bad_flag = 1;
			break;
	}

	if (bad_flag) {
		free(new_tab);
		return(0);
	}
	ip->table = new_tab;
	ip->xdivs = npts;
	ip->dx = (ip->xmax - ip->xmin)/(double)npts;
	ip->invdx = 1.0/ip->dx;
	free(old_tab);
	return(1);
}

/*
** Special function for tweaking table :
** scale_table(interpol,sx,sy,ox,oy)
** where sx, sy are magnification factors on X and Y, and ox,oy are
** offsets. The new values are then installed in the interpol struct
*/

void scale_table(ip,field,value)
	Interpol	*ip;
	char		*field;
	char		*value;
{
	double	*table;
	int		i;
	int		xdivs;
	double	osy,ooy;
	double	sx,sy,ox,oy;

	sx = ip->sx; sy = ip->sy;
	ox = ip->ox; oy = ip->oy;

	if (strcmp(field,"sx") == 0)
		sscanf(value,"%lf",&sx);
	else if (strcmp(field,"sy") == 0)
		sscanf(value,"%lf",&sy);
	else if (strcmp(field,"ox") == 0)
		sscanf(value,"%lf",&ox);
	else if (strcmp(field,"oy") == 0)
		sscanf(value,"%lf",&oy);
	else 
		return;

	xdivs = ip->xdivs;
	table = ip->table;
	osy = ip->sy;
	ooy = ip->oy;
	if (oy != ooy || osy != sy) {
		osy = (fabs(ip->sy) < 1e-30) ? 1.0 : sy / ip->sy;
		for (i = 0 ; i <= xdivs ; i++) {
			table[i] = oy + (table[i] - ooy) * osy;
		}
	}
	ip->xmin = ox + (ip->xmin - ip->ox) * sx / ip->sx;
	ip->xmax = ox + (ip->xmax - ip->ox) * sx / ip->sx;
        ip->dx = (ip->xmax - ip->xmin) / (double)(ip->xdivs);
        ip->invdx = 1.0/ip->dx;
	ip->ox = ox;
	ip->oy = oy;
	ip->sx = sx;
	ip->sy = sy;
}

/*
** Special function for finding min/max values in table, between
** lox and hix.
** get_table_min(table,lox,hix)
** get_table_max(table,lox,hix)
**
** Special function for finding roots (x value for given y)
*/

#ifdef MAYBE

free_interpol(ip)
	Interpol	*ip;
{
	if (ip->allocated)
		free(ip->table);
	free(ip);
}

reset_interpol(ip)
	Interpol	*ip;
{
	switch (ip->extrapolation_mode) {
		case TRUNC_EXTRAP :
			ip->table[0] = ip->table[1];
			ip->table[xdivs + 1] = ip->table[xdivs];
		break;
		case LINEAR_EXTRAP :
		default : /* linear extrapolation */
			break;
	}
}

do_create_interpol(argc,argv)
	int argc;
	char **argv;
{
	tab_gate_struct	*elm;

	initopt(argc, argv, "element interp xdivs xmin xmax");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    printf("\n");
	    printf("Warning : interp must be an interpolation structure\n");
	    printf("in the element.\n");
	    return;
	  }

	elm = (tab_gate_struct *)(GetElement(optargv[1]));
	if (!elm) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}
	elm
}
#endif

/*
** Set up alpha and beta interpols from alpha-beta rate constants
** given in args array.  A and B are assumed to have been allocated
** as dictated by args value for xdivs (e.g. args[10]).
*/
void setup_tab_values(A,B,args,mode,tab_gate_flag)
	Interpol	*A,*B;
	double		args[15];
	int		mode;
	int		tab_gate_flag;
{
	double temp,x,dx,temp2;
	int i;

	dx=(args[12]-args[11])/args[10];
	/* EDS change: no longer take midpoint of interval */
	for(x=args[11],i=0;i<=args[10];i++,x+=dx) {
		/* Assorted ugly NaN cases */
		if (fabs(args[4])<SING_TINY) {
			temp=A->table[i]=0.0;
		} else {
			temp2=args[2]+exp((x+args[3])/args[4]);
			if (fabs(temp2)<SING_TINY)
				temp=A->table[i]=A->table[i-1];
			else
				temp=A->table[i]= (args[0]+args[1]*x)/temp2;
		}

		if (fabs(args[9])<SING_TINY) {
			B->table[i]=0.0;
		} else {
			temp2=args[7]+exp((x+args[8])/args[9]);
			if (fabs(temp2)<SING_TINY)
				B->table[i]=B->table[i-1];
			else
				B->table[i]= (args[5]+args[6]*x)/temp2;
		}
		if (!tab_gate_flag && mode==SETUP_ALPHA)
			B->table[i]+=temp;
	}
	if (mode==SETUP_TAU) {
		for(i=0;i<=args[10];i++) {
			temp=A->table[i];
			temp2=B->table[i];
			if (fabs(temp) < SING_TINY) {
				A->table[i]=0;
				B->table[i]=0;
			} else {
				A->table[i]=temp2/temp;
				if (!tab_gate_flag)
					B->table[i]=1.0/temp;
				else 
					B->table[i]=temp-temp2;
			}
		}
	}
}

/*
** Generic command interface code for alpha/beta table setup.  Actual
** setting of table values is now in setup_tab_values().
*/
void SetupGeneric(argc,argv,mode)
	int argc;
	char	**argv;
	int		mode;
{
	double args[15];	/* EDS change */
	Element	*elm;
	Interpol	*A,*B;
	int i;
	int	tab_gate_flag=0;
	Interpol *create_interpol();
	Tchan   *tchan;
	int	status;

	A=B=NULL;
	args[10]=DEFAULT_XDIVS;
	args[11]=DEFAULT_XMIN;
	args[12]=DEFAULT_XMAX;

	initopt(argc, argv, "channel-element gate AA AB AC AD AF BA BB BC BD BF -size n -range min max");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-size") == 0)
		args[10] = atoi(optargv[1]);
	    else if (strcmp(G_optopt, "-range") == 0)
	      {
		args[11] = Atod(optargv[1]);
		args[12] = Atod(optargv[2]);
	      }
	  }

	if (status < 0)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	elm = GetElement(optargv[1]);
	if (!elm) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}
	for(i=3;i<optargc;i++) args[i-3]=Atod(optargv[i]);
	/* for(i=3;i<16;i++) printf("%d %f\n",i-3,args[i-3]); */

	if (strcmp(elm->object->name,"tabgate")==0) {
		A=((struct tab_gate_type *)elm)->alpha=
			create_interpol((int)args[10],args[11],args[12]);
		((struct tab_gate_type *)elm)->alpha_alloced=1;
		B=((struct tab_gate_type *)elm)->beta=
			create_interpol((int)args[10],args[11],args[12]);
		((struct tab_gate_type *)elm)->beta_alloced=1;
		tab_gate_flag=0;
	}
	if (strcmp(elm->object->name,"tabchannel")==0) {
        tchan=(Tchan *)elm;
		if (strcmp(optargv[2],"X")==0) {
			A=tchan->X_A=create_interpol((int)args[10],args[11],args[12]);
			B=tchan->X_B=create_interpol((int)args[10],args[11],args[12]);
            tchan->X_alloced=1;
		} else if (strcmp(optargv[2],"Y")==0) {
			A=tchan->Y_A=create_interpol((int)args[10],args[11],args[12]);
			B=tchan->Y_B=create_interpol((int)args[10],args[11],args[12]);
            tchan->Y_alloced=1;
		} else if (strcmp(optargv[2],"Z")==0) {
			A=tchan->Z_A=create_interpol((int)args[10],args[11],args[12]);
			B=tchan->Z_B=create_interpol((int)args[10],args[11],args[12]);
            tchan->Z_alloced=1;
		} else {
			printf("Gate type %s not known in %s \n",optargv[2],optargv[0]);
			return;
		}
	}

	if (!A || !B) {
		printf("Error: Element %s should be a tabgate or tabchannel\n",
			optargv[1]);
		return;
	}

	setup_tab_values(A, B, args, mode, tab_gate_flag);
}

/* Set up a tabulated channel from alpha-beta rate constants */
void SetupAlpha(argc,argv)
	int argc;
	char	**argv;
{
	SetupGeneric(argc,argv,SETUP_ALPHA);
}

/* Set up a tabulated channel from tau-minf rate constants */
void SetupTau(argc,argv)
	int argc;
	char	**argv;
{
	SetupGeneric(argc,argv,SETUP_TAU);
}

/* Convert a sparsely filled table to tabchannel/tabgate format
** starting from alpha-beta or tau-minf values. Optionally
** do the TABFILL, but this is not yet implemented */
void tweak_tab_values(argc,argv,mode)
	int argc;
	char	**argv;
	int		mode;
{
	Element	*elm;
	Interpol	*A,*B;
	float temp,temp2;
	int i;
	int	tab_gate_flag=0;
	Interpol *create_interpol();

	A=B=NULL;

	initopt(argc, argv, "channel-element table");
	if (G_getopt(argc, argv) != 0)
	  {
		printoptusage(argc, argv);
		return;
	  }

	elm = GetElement(optargv[1]);
	if (!elm) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}
	if (strcmp(elm->object->name,"tabgate")==0) {
		A=((struct tab_gate_type *)elm)->alpha;
		B=((struct tab_gate_type *)elm)->beta;
		tab_gate_flag=1;
	}
	if (strcmp(elm->object->name,"tabchannel")==0) {
		if (strcmp(optargv[2],"X")==0) {
			A=((struct tab_channel_type *)elm)->X_A;
			B=((struct tab_channel_type *)elm)->X_B;
		} else if (strcmp(optargv[2],"Y")==0) {
			A=((struct tab_channel_type *)elm)->Y_A;
			B=((struct tab_channel_type *)elm)->Y_B;
		} else if (strcmp(optargv[2],"Z")==0) {
			A=((struct tab_channel_type *)elm)->Z_A;
			B=((struct tab_channel_type *)elm)->Z_B;
		} else {
			printf("Gate type %s not known in %s \n",optargv[2],optargv[0]);
			return;
		}
	}
	if (!A || !B) {
		printf("Error: Element %s should be a tabgate or tabchannel\n",
			optargv[1]);
		return;
	}
	if (mode==SETUP_ALPHA) {
		for(i=0;i<=A->xdivs;i++)
			B->table[i]+=A->table[i];
	} else if (mode==SETUP_TAU) {
		for(i=0;i<=A->xdivs;i++) {
			temp=A->table[i];
			temp2=B->table[i];

			/*
			** per discussion with Mike Vanier
			*/
			if (fabs(temp) < SING_TINY) {
			    if (temp < 0.0)
				temp = -SING_TINY;
			    else
				temp = SING_TINY;
			}

			A->table[i]=temp2/temp;
			B->table[i]=1.0/temp;
		}
	}
}

void TweakAlpha(argc,argv)
	int argc;
	char	**argv;
{
	tweak_tab_values(argc,argv,SETUP_ALPHA);
}

void TweakTau(argc,argv)
	int argc;
	char	**argv;
{
	tweak_tab_values(argc,argv,SETUP_TAU);
}


/*
** Sets up a single table with A B C D F values, not for tabchannels
*/
void SetupGate(argc,argv)
	int argc;
	char	**argv;
{
	Element	*elm;
	Interpol	*ip;
	float temp,x,dx,temp2;
	int i;
	int	tab_gate_flag=0;
	Interpol *create_interpol();
	int xdivs;
	double xmin,xmax;
	float A,B,C,D,F;
	int	noalloc_flag=0;
	int	status;

	xdivs = DEFAULT_XDIVS;
	xmin = DEFAULT_XMIN;
	xmax = DEFAULT_XMAX;
	ip=NULL;

	initopt(argc, argv, "channel-element table A B C D F -size n -range min max -noalloc");
	while ((status = G_getopt(argc, argv)) == 1) {
	    if (strcmp(G_optopt, "-size") == 0){
		xdivs = atoi(optargv[1]);
		if (xdivs<=9) {
                                printf("Error : table %s should have at least 10 elements\n", optargv[1]);
                                return;
                }
            }
	    else if (strcmp(G_optopt, "-range") == 0) 
	    {
		xmin = Atod(optargv[1]);
		xmax = Atod(optargv[2]);
		if (xmin >= xmax) {
                       printf("Error : xmin=%f should be less than xmax=%f\n", xmin,xmax);
                              return;
                }

	    }
	    else if (strcmp(G_optopt, "-noalloc") == 0)
		noalloc_flag = 1;
	}

	if (status < 0) {
		printoptusage(argc, argv);
		return;
	}

	elm = GetElement(optargv[1]);
	if (!elm) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}

	A=Atof(argv[3]);
	B=Atof(argv[4]);
	C=Atof(argv[5]);
	D=Atof(argv[6]);
	F=Atof(argv[7]);

	if (xdivs<=9) {
		printf("Error : table %s should have at least 10 elements\n", optargv[1]);
		return;
	}
	if (xmin >= xmax) {
		printf("Error : xmin=%f should be less than xmax=%f\n", xmin,xmax);
		return;
	}
	if (strcmp(elm->object->name,"tabgate")==0) {
		if (optargv[2][0]=='a') { /* Alpha */
			if (!noalloc_flag) {
				((struct tab_gate_type *)elm)->alpha=
					create_interpol(xdivs,xmin,xmax);
				((struct tab_gate_type *)elm)->alpha_alloced=1;
			}
			ip=((struct tab_gate_type *)elm)->alpha;
		} else if (optargv[2][0]=='b') { /* Beta */
			if (!noalloc_flag) {
				((struct tab_gate_type *)elm)->beta=
					create_interpol(xdivs,xmin,xmax);
				((struct tab_gate_type *)elm)->beta_alloced=1;
			}
			ip=((struct tab_gate_type *)elm)->beta;
		} else {
			printf("Error : table name invalid for tab_gates\n");
			return;
		}
		tab_gate_flag=1;
	}
	if (strcmp(elm->object->name,"tabchannel")==0) {
		printf("Error : Must set both A and B tables together\n");
		return;
	}
	if (strcmp(elm->object->name,"table")==0) {
		if (!noalloc_flag) {
			((struct table_type *)elm)->table=
				create_interpol(xdivs,xmin,xmax);
			((struct table_type *)elm)->alloced=1;
		}
			ip=((struct table_type *)elm)->table;
	}
	if (ip==NULL) {
		printf("Table not found, type is not handled yet\n");
		return;
	}
	 /* This is needed for the -noalloc case. */
        xdivs = ip->xdivs;
 
        /* Here one might also wish to set
        xmin = ip->xmin;
        xmax = ip->xmax;
        ** But the previous behavior was to rely on the hardcoded defaults.
        ** We'll leave it as is, for now.
        */

	dx=(xmax-xmin)/(float)xdivs;
	for(x=xmin+dx/2.0,i=0;i<=xdivs;i++,x+=dx) {
		/* Assorted ugly NaN cases */
		if (fabs(F)<SING_TINY) {
			temp=ip->table[i]=0.0;
		} else {
			temp2=C+exp((x+D)/F);
			if (fabs(temp2)<SING_TINY)
				temp=ip->table[i]=ip->table[i-1];
			else
				temp=ip->table[i]= (A+B*x)/temp2;
		}
	}
}

void tab_dup_func(pa,pb)
	Interpol	**pa,**pb;
{
	double	*temp;
	Interpol	*A,*B;
	int	i;

	A=*pa;
	B=*pb;

	*pa=create_interpol(A->xdivs,A->xmin,A->xmax);
	temp=(*pa)->table;
	**pa=*A;
	(*pa)->table=temp;
	/* Note that there are xdivs+1 entries in each table */
	for(i=0;i<=A->xdivs;i++)
		temp[i] = A->table[i];

	*pb=create_interpol(B->xdivs,B->xmin,B->xmax);
	temp=(*pb)->table;
	**pb=*B;
	(*pb)->table=temp;
	/* Note that there are xdivs+1 entries in each table */
	for(i=0;i<=B->xdivs;i++)
		temp[i] = B->table[i];
}


/*
** makes a BCOPY of the entire ip structure, so we can alter it without
** messing up all other ip structs.
*/
void DuplicateTable(argc,argv)
	int argc;
	char	**argv;
{
	Element	*elm;

	initopt(argc, argv, "channel-element table");
	if (G_getopt(argc, argv) != 0)
	  {
		printoptusage(argc, argv);
		return;
	  }

	elm = GetElement(optargv[1]);
	if (!elm) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}
    if (strcmp(elm->object->name,"table")==0) {
		double *temp;
		Interpol *old_ip;

        old_ip = ((struct table_type *)elm)->table;
        ((struct table_type *)elm)->table =
            create_interpol(old_ip->xdivs,old_ip->xmin,old_ip->xmax);
        temp = ((struct table_type *)elm)->table->table;
        *(((struct table_type *)elm)->table) = *old_ip;
        ((struct table_type *)elm)->table->table = temp;
        BCOPY(old_ip->table,temp,sizeof(double) * (old_ip->xdivs + 1));
    }
	if (strcmp(elm->object->name,"tabgate")==0) {
		tab_dup_func(&(((struct tab_gate_type *)elm)->alpha),
			&(((struct tab_gate_type *)elm)->beta));
	}
	if (strcmp(elm->object->name,"tabchannel")==0) {
		if (strcmp(optargv[2],"X")==0) {
			tab_dup_func(&(((struct tab_channel_type *)elm)->X_A),
				&(((struct tab_channel_type *)elm)->X_B));
		}
		if (strcmp(optargv[2],"Y")==0) {
			tab_dup_func(&(((struct tab_channel_type *)elm)->Y_A),
				&(((struct tab_channel_type *)elm)->Y_B));
		}
		if (strcmp(optargv[2],"Z")==0) {
			tab_dup_func(&(((struct tab_channel_type *)elm)->Z_A),
				&(((struct tab_channel_type *)elm)->Z_B));
		}
	}
}

/*
** ScaleTabchan(channel,gate,mode,sx,sy,ox,oy[-duplicate])
** Scales the values in the tables 
** according to the modes, so that the hh equations are still OK
** Modes : alpha,beta,tau,minf
** if the duplicate flag is set then it will copy it to 
** a new table before messing with it.
*/
void ScaleTabchan(argc,argv)
	int argc;
	char	**argv;
{
	Element	*elm;
	Interpol	*A,*B;
	float dx;
	int i,j;
	int	tab_gate_flag=0;
	Interpol *create_interpol();
	int xdivs;
	float sx,sy,ox,oy;
	float alpha,beta,tau,minf;
	float *holding1;
	float *holding2;
	double xmin,xmax;
	int duplicate;
	int status;

	holding1=holding2=NULL;
	A=B=NULL;
	duplicate = 0;

	initopt(argc, argv, "channel-element gate mode sx sy ox oy -duplicate");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-duplicate") == 0)
		duplicate = 1;
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		printf("\n");
		printf("Valid modes : a[lpha] b[eta] t[au] m[inf]\n");
		return;
	}

	elm = GetElement(optargv[1]);
	if (!elm) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}
	if (duplicate) /* do duplicate */
		DuplicateTable(3,argv);
	if (strcmp(elm->object->name,"tabgate")==0) {
		A=((struct tab_gate_type *)elm)->alpha;
		B=((struct tab_gate_type *)elm)->beta;
		tab_gate_flag=1;
	}
	if (strcmp(elm->object->name,"tabchannel")==0) {
		if (strcmp(argv[2],"X")==0) {
			A=((struct tab_channel_type *)elm)->X_A;
			B=((struct tab_channel_type *)elm)->X_B;
		} else if (strcmp(argv[2],"Y")==0) {
			A=((struct tab_channel_type *)elm)->Y_A;
			B=((struct tab_channel_type *)elm)->Y_B;
		} else if (strcmp(argv[2],"Z")==0) {
			A=((struct tab_channel_type *)elm)->Z_A;
			B=((struct tab_channel_type *)elm)->Z_B;
		} else {
			printf("Gate type %s not known in %s \n",argv[2],argv[0]);
			return;
		}
	}
	if (!A || !B) {
		printf("Error: Element %s should be a tabgate or tabchannel\n",
			optargv[1]);
		return;
	}
		
	sx=Atof(optargv[4]);
	sy=Atof(optargv[5]);
	ox=Atof(optargv[6]);
	oy=Atof(optargv[7]);
	if (sx <= 0.0) {
		printf("%s: Error: sx must be > 0\n", argv[0]);
		return;
	}
	xdivs=A->xdivs;
	xmin=A->xmin;
	xmax=A->xmax;
	dx=(xmax-xmin)/(float)xdivs;
	holding1=(float *)calloc(xdivs+2,sizeof(float));
	holding2=(float *)calloc(xdivs+2,sizeof(float));
	/* to use tabchannel code, temporarily change tabgate beta table */
	if (tab_gate_flag) {
		for(i=0;i<=xdivs;i++)
			B->table[i] += A->table[i];
	}
	switch(optargv[3][0]) {
		case 'a' : /* alpha */
			for(i=0;i<=xdivs;i++) {
				/* find beta */
				holding2[i]=B->table[i]-A->table[i];
				j=(xmin*(1.0-sx)/sx - ox)/dx + (float)i/sx;
				if (j>xdivs) j=xdivs;
				if (j<0) j=0;
				alpha=oy+A->table[j]*sy;
				if (alpha<0.0)
					holding1[i]=0.0;
				else
					holding1[i]=alpha;
			}
			for(i=0;i<=xdivs;i++) {
				A->table[i]=holding1[i];
				B->table[i]=holding1[i]+holding2[i];
			}
			break;
		case 'b' : /* beta */
			for(i=0;i<=xdivs;i++) {
				j=(xmin*(1.0-sx)/sx - ox)/dx + (float)i/sx;
				if (j>xdivs) j=xdivs;
				if (j<0) j=0;
				alpha=A->table[j];
				beta=B->table[j];
				beta-=alpha;
				beta=oy+beta*sy;
				if (beta<0.0)
					holding1[i]=0.0;
				else
					holding1[i]=beta;
			}
			for(i=0;i<=xdivs;i++) B->table[i]=holding1[i]+A->table[i];
			break;
		case 't' : /* tau */
			for(i=0;i<=xdivs;i++) {
				holding2[i]=A->table[i]/B->table[i];
				j=(xmin*(1.0-sx)/sx - ox)/dx + (float)i/sx;
				if (j>xdivs) j=xdivs;
				if (j<0) j=0;
				tau=1.0/B->table[j];
				tau=oy+tau*sy;
				if (tau<0.0)
					holding1[i]=0.00000001; /* totally random number */
				else
					holding1[i]=tau;
			}
			for(i=0;i<=xdivs;i++) {
				A->table[i]=holding2[i]/holding1[i];
				B->table[i]=1.0/holding1[i];
			}
			break;
		case 'm' : /* minf */
			for(i=0;i<=xdivs;i++) {
				j=(xmin*(1.0-sx)/sx - ox)/dx + (float)i/sx;
				if (j>xdivs) j=xdivs;
				if (j<0) j=0;
				minf=A->table[j]/B->table[j];
				minf=oy+minf*sy;
				if (minf<0.0)
					holding1[i]=0.0;
				else
					holding1[i]=minf;
			}
			for(i=0;i<=xdivs;i++) {
				A->table[i]=holding1[i]*B->table[i];
			}
			break;
	}
	if (tab_gate_flag) {
		for(i=0;i<=xdivs;i++) B->table[i] -= A->table[i];
	}
	if (holding1) free(holding1);
	if (holding2) free(holding2);
}

 
void one_tab_dup_func(pa)
    Interpol    **pa;
{
    double  *temp;
    Interpol    *A;
 
    A=*pa;
 
    *pa=create_interpol(A->xdivs,A->xmin,A->xmax); /* making a new one*/
    temp=(*pa)->table; /* saving the table */
    **pa=*A; /* Assigning the contents */
    (*pa)->table=temp; /* putting the table back */
    BCOPY(A->table,temp,(A->xdivs+1)*sizeof(double));
    /* copying the table contents back */
}


/* new implementation for the SetTable function:
** Utility function for all objects with tables. This makes it
** a lot easier to set and copy tables, using the 'SET' action
** and a variety of options.
** There are three ways in which the destination table can be
** manipulated:
**	1. The existing entries can have their values changed, without
**		affecting allocation in any way.
**		set foo table bar/table
**	2. The table array of the destination interpol is replaced, and
**		the old one freed. If necessary the xdivs field is changed
**		to reflect a change in the size of the array.
**		set foo &table bar/table
**	3. The entire destination interpol struct is replaced, and the
**		old one freed.
**		set foo &&table bar/table
**	The default is 1. To specify 2, prefix the name of the dest table
**	with an &. To specify 3, prefix the name of the dest table with &&.
**	
**	The second argument can be either another table, or a constant.
**	If it is a constant, then cases 2 and 3 above are errors. The
**	assignment in 'a' and arithmetical ops in 'c' below are legal,
**	but not 'b'.
**	If the second argument is a table, there are several possible
**	options:
**	a. There can be a direct value assignment. This is the default.
**		In the case of 1 above, this is trivial. In the case of 2 and 3,
**		new tables/interpols are allocated and the values copied over.
**		set foo table bar/table
**	b. There can be a pointer assignment. This is indicated by an &
**		prefix on the second argument. The case 1 above is illogical
**		for this option, and produces an error message.
**		For 2 and 3, there is a pointer copy to the table/interpol resp.
**		set foo &table &bar/table
**	c. There can be arithmetical operations. Due to parser problems,
**		all these must be placed between '=' signs, e.g. 
**		set /foo =+=/bar
**		Only option 1 above is allowed.
**		In all cases the results are placed in the first table (foo).
**		The following ops are recognized:
**		+: sums the two tables, placing results in the first.
**		-: subtracts the second table from the first.
**		*: does an item-by-item product.
**		/: divides the first table by the second.
**              =: Sets the first table to the second, using procrustean fitt
ing
**              f: Sets the first table to the second, using interpols to
**                      make sure the number of entries fit.
**		More ops may be added if they seem relevant.
**
**
** A major unresolved issue is how one should deal with the
** old interpols and arrays, in cases where new ones are assigned.
** Ideally there should be a memory-management arrangement which
** keeps track of the number of times an interpol is being used.
** For now, a #define switch will be used to decide whether the
** function frees old interpols or just lets them float off into
** limbo.
**
*/

#define FREEOLDINTERPOLS 0

int SetTable(elm,ac,av,data,tabnames)
	Element	*elm;
	int	ac;
	char	**av;
	char	*data;
	char	*tabnames;
{
	short	datatype;
	Element	*srcelm;
	Interpol **srcpip = NULL;
	Interpol **destpip;
	char	*srcname = NULL;
	char	*srctabname = NULL;
	char	*desttabname;
	char	*tabnamesep;
	char	*tabstr;
	char	*buf;
	int	namelen;
	int		dest_indirection = 0; /* 0 is value, 1,2 are ptrs */
	int		src_indirection = 0; /* 1 is ptr, -1 is op, -2 is constt */
	double	src_const = 0.0;
	char	src_op = '\0';
	int		i;
	int		destdivs;
	int		do_loadtab = 0;

	if (!elm) {
		return(1);
	}

	if (av[0][0] == '&') {
		if (av[0][1] == '&') {
			/* replacing the destination interpol entirely */
				desttabname = av[0] + 2;
				dest_indirection = 2;
		} else {
			/* replacing the array from the dest interpol */
				desttabname = av[0] + 1;
				dest_indirection = 1;
		}
	} else { /* filling in the entries of the dest, no memory changes */
			desttabname = av[0];
			dest_indirection = 0;
	}

	/* check that the desttabname is in fact a valid table */
    if (!desttabname)
        return(0);

    namelen = strlen(desttabname);
    if (namelen == 0)
        return(0);

    buf = (char*) malloc(strlen(tabnames)+1);
    if (buf == NULL)
      {
        perror("SetTable");
        return(0);
      }
    strcpy(buf, tabnames);

    tabnamesep = " \t,:;/.-|!"; /* allow for various name separators */
    tabstr = strtok(buf, tabnamesep);
    while (tabstr != NULL)
      {
        if (strcmp(tabstr, desttabname) == 0)
        break;

        tabstr = strtok(NULL, tabnamesep);
      }

    free(buf);
    if (tabstr == NULL)
        return(0);

	if (av[1][0] == '&') {
		/* using the pointer from the src interpol */
		src_indirection = 1;
		srcname = GetParentComponent(&(av[1][1]));
		srctabname = GetBaseComponent(&(av[1][1]));
	} else if (av[1][0] == '=') {
		if (strlen(av[1]) < 4) {
			Error();
			printf("Cannot set interpol %s %s to invalid value %s\n", 
				Pathname(elm), av[0],av[1]);
			return(1);
		}
		if (dest_indirection > 0) {
			Error();
			printf("Cannot set interpol pointer %s %s to value %s\n",
				Pathname(elm), av[0],av[1]);
			return(1);
		}
		if (!strchr((av[1]+3), '/')) {
			src_indirection = -2; /* constant arith op */
			src_const = Atof(av[1]+3);
		} else {
			srcname = GetParentComponent(&(av[1][3]));
			srctabname = GetBaseComponent(&(av[1][3]));
			src_indirection = -1; /* arith op */
		}
		src_op = av[1][1];
	} else if (av[1][0] == '*') { /* the loadtab case */
		dest_indirection = 2;
		src_indirection = 1;
		do_loadtab = 1;
	} else {
		srcname = GetParentComponent(&(av[1][0]));
		srctabname = GetBaseComponent(&(av[1][0]));
		src_indirection = 0;
	}


	/* getting the dest interpol structs */
	if ((destpip = (Interpol **)GetFieldAdr(elm,desttabname,&datatype))) {
		if (datatype != INVALID) {
			Error();
			printf("field %s on element %s is not a table\n",
				av[0], Pathname(elm));
			return(1);
		}
	} else {
		Error();
		printf("could not find table %s on element %s\n",
			desttabname, Pathname(elm));
		return(1);
	}

	/* getting the src interpol structs */
	if (do_loadtab) {
		srcpip = (Interpol **)&data;
	} else {
		if (src_indirection > -2) { /* -2 is a constt */
			if (!(srcelm = GetElement(srcname))) { /* couldnt find it */
				Error();
				printf("could not find element %s\n",srcname);
				return(1);
			}
			if ((srcpip=(Interpol **)GetFieldAdr(
				srcelm,srctabname,&datatype))){
				if (datatype != INVALID) {
					Error();
					printf("field %s on element %s is not a table\n",
						srctabname, srcname);
					return(1);
				}
			} else {
				Error();
				printf("could not find table %s on element %s\n",
					srctabname, srcname);
				return(1);
			}
		}
	}

	if (*destpip && (*destpip)->allocated)
		destdivs = (*destpip)->xdivs +1;
	else
		destdivs = 0;

	switch (dest_indirection) {
		case 0: /* assignment of array values */
			/* check that the dest is alloced for this */
			if (!(*destpip && (*destpip)->allocated)) {
				Error();
				printf("in SetTable, dest table %s %s not allocated\n",
					Pathname(elm),desttabname);
				break;
			}
			switch (src_indirection) {
				case -2:	/* constant */
					switch (src_op) {
						case '+':
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] += src_const;
						break;
						case '-':
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] -= src_const;
						break;
						case '*':
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] *= src_const;
						break;
						case '/':
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] /= src_const;
						break;
						case '=':
						case 'f':
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] = src_const;
						break;
						case 'i': /* increment */
                                                        {
                                                                double y = 0;
                                                            for (i = 0; i < destdivs; i++,y+= src_const)
                                                                        (*destpip)->table[i] = y;
                                                        }
                                                break;

					}
				break;
				case -1:	/* value ops */
					/* check that the src is alloced for this */
					if (!(*srcpip && (*srcpip)->allocated)) {
						Error();
						printf(
						 "in SetTable, src table %s %s not allocated\n",
							srcname,srctabname);
						break;
					}
					if (destdivs > (*srcpip)->xdivs + 1)
						destdivs = (*srcpip)->xdivs + 1;
					switch (src_op) {
						case '+':
							for (i = 0; i < destdivs; i++) {
								(*destpip)->table[i] += 
									(*srcpip)->table[i];
							}
						break;
						case '-':
							for (i = 0; i < destdivs; i++) {
								(*destpip)->table[i] -= 
									(*srcpip)->table[i];
							}
						break;
						case '*':
							for (i = 0; i < destdivs; i++) {
								(*destpip)->table[i] *= 
									(*srcpip)->table[i];
							}
						break;
						case '/':
							for (i = 0; i < destdivs; i++) {
								(*destpip)->table[i] /= 
									(*srcpip)->table[i];
							}
						break;
						case '=':
							for (i = 0; i < destdivs; i++) {
								(*destpip)->table[i] = 
									(*srcpip)->table[i];
							}
						break;

						case 'e': /* exponent */
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] = 
									exp((*srcpip)->table[i]);
						break;
						case 'E': /* 10th power */
							for (i = 0; i < destdivs; i++)
								(*destpip)->table[i] = 
									exp10((*srcpip)->table[i]);
						break;
						case 'l': /* natural log */
							for (i = 0; i < destdivs; i++)
								if ((*srcpip)->table[i] > 0)
									(*destpip)->table[i] = 
										log((*srcpip)->table[i]);
								else
									(*destpip)->table[i] = 0;
						break;
						case 'L': /* log to base 10 */
							for (i = 0; i < destdivs; i++)
								if ((*srcpip)->table[i] > 0)
									(*destpip)->table[i] = 
										log10((*srcpip)->table[i]);
								else
									(*destpip)->table[i] = 0;
						break;
						case 'f': /* fit the src table into
                        ** the dest, using interp to make sure
                        ** that we get a reasonable
                        ** approx to all values
                        ** We leave the xmin, xmax and dx
                        ** of dest alone, like with the other cases
                        */
							if ((*destpip)->xdivs > 0) {
								Interpol *ip = *srcpip;
								double x = ip->xmin;
								double dx = (ip->xmax - x)/(double)(
(*destpip)->xdivs);
								/* It turns out that the invdx
								** is not reliably set in the
								** tabchannels.
								ip->dx = (ip->xmax - ip->xmin) / (do
uble)(ip->xdivs);
								if (fabs(ip->dx) > 1e-50)
									ip->invdx=1.0/ip->dx;
								else
									ip->invdx=1.0;
								*/
								for (i = 0;i <= (*destpip)->xdivs; i
++, x+= dx) {
								  (*destpip)->table[i] = TabInterp(ip,x);
								}
							}
						break;
						case 'i': /* increment */
							{
								double y = 0;
								for (i = 0; i < destdivs; i++) {
									(*destpip)->table[i] = y;
									y += (*srcpip)->table[i];
								}
							}

					}
				break;
				case 0:	/* assignment of values */
					if (!(*srcpip && (*srcpip)->allocated)) {
						Error();
						printf(
						 "in SetTable, src table %s %s not allocated\n",
							srcname,srctabname);
						break;
					}
					if (destdivs > (*srcpip)->xdivs + 1)
						destdivs = (*srcpip)->xdivs + 1;
					for (i = 0; i < destdivs; i++)
						(*destpip)->table[i] = 
							(*srcpip)->table[i];
				break;
			}
		break;
		case 1: /* replacement of the table array in the dest */
			/* Check for the existence of the destpip */
			if (!(*destpip)) {
				Error();
				printf("in SetTable, dest table %s %s not allocated\n",
					Pathname(elm),desttabname);
				break;
			}
			switch (src_indirection) {
				case 0: /* A copy of the src array is allocated and
						** placed in the dest interpol */
						/* first check if the src array exists */
						if ((*srcpip) && 
							(*srcpip)->allocated &&
							(*srcpip)->xdivs > 0 &&
							(*srcpip)->table) {
							/* get rid of the dest table */
							if (FREEOLDINTERPOLS) {
								if ((*destpip)->allocated &&
									(*destpip)->xdivs > 0 &&
									(*destpip)->table) {
									free((*destpip)->table);
								}
							}
							(*destpip)->table = (double *) calloc(
								(*srcpip)->xdivs + 1, sizeof(double));
							for(i = 0; i <= (*srcpip)->xdivs; i++)
								(*destpip)->table[i] = 
									(*srcpip)->table[i];
							(*destpip)->allocated = (*srcpip)->allocated;
							(*destpip)->xdivs = (*srcpip)->xdivs;
						} else {
							Error();
							printf("SetTable: Src table %s %s not allocated\n",
								srcname, srctabname);
							return(1);
						}
				break;
				case 1: /* This option is very dangerous. It is
						** included for completeness, but not
						** recommended */
						/* Free the original dest array, if any */
						if (FREEOLDINTERPOLS) {
							if ((*destpip)->allocated &&
								(*destpip)->xdivs > 0 &&
								(*destpip)->table)
								free((*destpip)->table);
						}
						/* first check if the src ip exists */
						if (!(*srcpip)) {
							(*destpip)->table = NULL;
							(*destpip)->allocated = 0;
							(*destpip)->xdivs = 0;
							break;
						}
						/* we do the assignment regardless of
						** whether the srcpip has an allocated table */
						(*destpip)->table = (*srcpip)->table;
						(*destpip)->allocated = (*srcpip)->allocated;
						(*destpip)->xdivs = (*srcpip)->xdivs;
				break;
			}
		break;
		case 2: /* replacement of the entire interpol. */
			switch (src_indirection) {
				case 0: /* A copy of the src interpol is allocated and
						** placed in the dest interpol */
						/* first check if the src array exists */
						if ((*srcpip) &&
							(*srcpip)->allocated &&
							(*srcpip)->xdivs > 0 &&
							(*srcpip)->table) {
							/* get rid of the dest table */
							if (FREEOLDINTERPOLS) {
								if (*destpip) {
										if ((*destpip)->allocated &&
										(*destpip)->xdivs > 0 &&
										(*destpip)->table) {
										free((*destpip)->table);
									}
									free(*destpip);
								}
							}
							*destpip = create_interpol(
								(*srcpip)->xdivs,
								(*srcpip)->xmin,
								(*srcpip)->xmax);
							for(i = 0; i <= (*srcpip)->xdivs; i++)
								(*destpip)->table[i] = 
									(*srcpip)->table[i];
						} else {
							Error();
							printf("SetTable: Src table %s %s not allocated\n",
								srcname, srctabname);
							return(1);
						}
				break;
				case 1: /* This option is used to replace the pointer
						** to the interpol */
						/* Free the original dest array, if any */
						if (FREEOLDINTERPOLS) {
							if (*destpip) {
									if ((*destpip)->allocated &&
									(*destpip)->xdivs > 0 &&
									(*destpip)->table) {
									free((*destpip)->table);
								}
								free(*destpip);
							}
						}
						/* we do the assignment regardless of
						** whether the srcpip is allocated */
						*destpip = *srcpip;
				break;
			}
		break;
	}
	return(1);
}


#if 0
/* Utility function for all objects with tables. This makes it
** a lot easier to set and copy tables, using the 'SET' action
** and a variety of options.
** 1. Copy over the interpol of another element, doing appropriate 
**		allocation.
**		The foreign table is accessed as if it were an element. 
**		set foo table bar/table
** 2. Do a direct pointer assignment to the table of another element.
**		The foreign table is accessed as if it were an
**		element, but prefixed with an '&' to indicate that it is a
**		pointer assignment. The current interpol, if any, is freed.
**		set foo table &bar/table
** 3. Do a direct pointer assignment to the interpol specified in the
**		'data' argument, freeing the current interpol if necessary.
**		This option is normally called
**		only by internal C functions like loadtab.
**		set foo table loadtab
*/

/* av[0] has the name of the dest table */
/* av[1] has either loadtab or the srcpath/srctable */
void SetTable(elm,ac,av,data)
	Element	*elm;
	int	ac;
	char	**av;
	char	*data;
{
	struct interpol_struct *newip = (struct interpol_struct *) data;
	Interpol **pip;
	short	datatype;
	Element	*srcelm;
	Interpol **srcpip;
	char	*srcname;
	char	*srctabname;

	if (!elm) {
		return;
	}
	if (pip = (Interpol **)GetFieldAdr(elm,av[0],&datatype)) {
		if (datatype == INVALID) {
			if (*pip) { /* The interpol already exists */
				if ((*pip)->allocated && (*pip)->xdivs >= 0)
					free((*pip)->table);
				free(*pip);
				*pip = NULL;
			}
		} else {
			Error();
			printf("field %s on element %s is not a table\n",
				av[0], Pathname(elm));
			return;
		}
	} else {
		Error();
		printf("could not find table %s on element %s\n",
			av[0], Pathname(elm));
		return;
	}

	if (strcmp(av[1],"loadtab") == 0) {
		/* This option copies the ip to the specified interpol */
		*pip = newip;
		/* assignment successful */
		return;
	} else if (av[1][0] == '&') {
		/* this is the direct pointer assignment case */
		srcname = GetParentComponent(&(av[1][1]));
		srctabname = GetBaseComponent(&(av[1][1]));
		if (!(srcelm = GetElement(srcname))) {
			Error();
			printf("could not find element %s\n",srcname);
			return;
		}
		if (srcpip =
			(Interpol **)GetFieldAdr(srcelm,srctabname,&datatype)) {
			if (datatype == INVALID) {
				if (*srcpip) { /* The interpol exists */
					if ((*srcpip)->allocated && (*srcpip)->xdivs >= 0) {
						/* Go ahead with the assignment */
						*pip = *srcpip;
						return;
					}
				}
			}
		}
	} else {
		/* this is the case where we copy the previous ip */
		srcname = GetParentComponent(&(av[1][0]));
		srctabname = GetBaseComponent(&(av[1][0]));
		if (!(srcelm = GetElement(srcname))) {
			Error();
			printf("could not find element %s\n",srcname);
			return;
		}
		if (srcpip =
			(Interpol **)GetFieldAdr(srcelm,srctabname,&datatype)) {
			if (datatype == INVALID) {
				if (*srcpip) { /* The interpol exists */
					if ((*srcpip)->allocated && (*srcpip)->xdivs >= 0) {
						int i;
						int xdivs = (*srcpip)->xdivs;
						double *tab, *srctab;;
						/* Go ahead with the copy */
						*pip = create_interpol(
							(*srcpip)->xdivs,
							(*srcpip)->xmin,
							(*srcpip)->xmax);
						tab = (*pip)->table;
						srctab = (*srcpip)->table;
						for(i = 0 ; i <= xdivs; i++)
							tab[i] = srctab[i];
						/* all done successfully */
						return;
					}
				}
			}
		}
	}
	Error();
	printf("could not find table %s on element %s\n",
		srctabname, srcname);
}
#endif

/* Utility function for reading in the entire definition of a table */
/* This has been revised to handle very long tables, which the		**
** parser barfs at. The hack is to have multiple loadtab commands	**
** which successively fill up the same interpol. For this to		**
** work the first loadtab works as usual, and successive			**
** loadtabs use the -cont flag. The interpol struct ip is made		**
** static for this reason. No other loadtabs can intervene, or the	**
** original ip is lost.												**
*/
void LoadTab(argc,argv)
	int	argc;
	char	**argv;
{
	static Interpol *ip;
	int	xdivs;
	double	xmin,xmax;
	int	calc_mode;
	double *tab;
	static Element	*elm;
	static char tabname[50];
	static int lastentry;
	int		i,j;
	int		load_done_flag = 0;
	char	*targs[2];
	Action	action;

	/* bypass initopt because the strings are so long */
	if (argc < 2) {
		printf(
	"usage: %s element table calc_mode xdivs xmin xmax [values...]\n",
			argv[0]);
		printf("or, for continuation loadtabs: -cont [values...]\n");
		printf("or, for the last loadtab: -end [values...]\n");
		return;
	}
	if (strncmp(argv[1],"-c",2) == 0 || strncmp(argv[1],"-e",2) == 0) {
		/* having set up the interpol, lets assign table values */
		if (!ip)  {
			Error();
			printf("LoadTab: continuation requires prior 'loadtab' \n command with full specification\n");
			return;
		}
			
		tab = ip->table;
		for(i = lastentry, j = 2; j < argc && i <= ip->xdivs; i++,j++) {
			tab[i] = Atod(argv[j]);
		}
		lastentry = i;
		if (i > ip->xdivs) load_done_flag = 1;
	} else if (argc > 7) {
	/* set up the values that will be used later to access the elm */
		if (!(elm = GetElement(argv[1]))) {
			printf("could not find element %s\n",argv[1]);
			return;
		}
		strcpy(tabname ,argv[2]);
		calc_mode = atoi(argv[3]);
		xdivs = atoi(argv[4]);
		xmin = Atod(argv[5]);
		xmax = Atod(argv[6]);
		ip = create_interpol(xdivs,xmin,xmax);
		ip->calc_mode = calc_mode;
		/* having set up the interpol, lets assign table values */
		tab = ip->table;
		for(i = 0, j = 7; j < argc && i <= ip->xdivs; i++,j++) {
			tab[i] = Atod(argv[j]);
		}
		lastentry = i;
		if (i > ip->xdivs) load_done_flag = 1;
	} else {
		printf(
	"usage: %s element table calc_mode xdivs xmin xmax [values...]\n",
			argv[0]);
		printf("or, for continuation loadtabs: -cont [values...]\n");
		return;
	}
	if (load_done_flag || strncmp(argv[1],"-e",2) == 0) {
		/* Send the interpol out to the table */
		/* We need to let the elements own SET action handle the
		** assignment of the table in order to correctly update
		** dependencies on table pointers and so on.
		*/
		action.name = "SET";
		action.type = SET;
		action.argc = 2;
		action.argv = targs;
		targs[0] = tabname; /* the name of the table */
		targs[1] = "*loadtab";
		action.data = (char *)ip;
		CallActionFunc(elm,&action);
		ip = NULL;
	}
}

/* Utility function for reading in a table from an ascii file.
** Can read in multiple tables. The ascii file is assumed to have
** the entries for the respective tables in successive positions.
** Can also read in a single table based on x,y pairs. If the
** x values are not uniformly spaced it does linear interpolation/
** extrapolation to guess the y value.
*/

void FileToTab(argc,argv)
	int	argc;
	char	**argv;
{
	Interpol *ip = NULL;
	Interpol *ip2 = NULL;
	Interpol *ip3 = NULL;
	Element	*elm;
	int		i,j;
	char	*targs[2];
	Action	action;
	char	*tab1 = NULL;
	char	*tab2 = NULL;
	char	*tab3 = NULL;
	char	*autofilltab = NULL;
	int		do_xy = 0;
	int		xy_xdivs = 100;
	int		skiplines = 0;
	int		calc_mode = 0; /* default is lin_interp */
	int		status;
	FILE	*fp,*fopen();
	int		num_entries;
	int		xdivs;
	float	temp;

	/* Check the args */
	initopt(argc, argv,
		"filename element table -table2 table -table3 table -xy xdivs -skiplines number -calc_mode mode -autofill table \n");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-skiplines") == 0) {
			skiplines = atoi(optargv[1]);
		}
	    if (strcmp(G_optopt, "-calc_mode") == 0) {
			calc_mode = atoi(optargv[1]);
		}
	    if (strcmp(G_optopt, "-table2") == 0) {
			tab2 = optargv[1];
		}
	    if (strcmp(G_optopt, "-table3") == 0) {
			tab3 = optargv[1];
		}
	    if (strcmp(G_optopt, "-xy") == 0) {
			do_xy = 1;
			xy_xdivs = atoi(optargv[1]);
			if (xy_xdivs < 1) {
				Error();
				printf("%s: xdivs for xy mode must be >= 1",argv[0]);
				return;
			}
		}
	    if (strcmp(G_optopt, "-autofill") == 0) {
			autofilltab = optargv[1];
		}
	  }
	if (skiplines < 0) {
	Error();
			printf("%s: skiplines must be > 0\n",optargv[0]);
	}
	  if ((do_xy || autofilltab) && tab2) {
	    printoptusage(argc, argv);
		printf(
		"Cannot specify an extra table and the xy or autofill options together\n");
	    return;
	  }

	if (status < 0)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	if (!(fp = fopen(optargv[1],"r"))) {
		Error();
		printf("%s: could not open file %s\n",optargv[0],optargv[1]);
		return;
	}

	if (!(elm = GetElement(optargv[2]))) {
		Error();
		printf("%s: could not find element %s\n",optargv[0],optargv[2]);
		return;
	}
	tab1 = optargv[3];

	/* Skip the specified number of lines */
	for (i = 0 ; i < skiplines;  i++) {
		while(1) {
			j = fgetc(fp);
			if (j == (int)'\n')
				break;
			if (j == EOF) {
				Error();
		printf("%s: EOF encountered before skiplines=%d lines read\n",
				optargv[0],skiplines);
				return;
			}
		}
	}
	
	/* Find the number of entries here */
	for(num_entries = 0; fscanf(fp,"%f",&temp) == 1; num_entries++);

	/* Check that they are sensible */
	if (num_entries <= 0) {
		Error();
		printf("%s: Zero entries in file \n", optargv[0]);
	}
	if ((do_xy && num_entries % 2 != 0) ||
		(tab3 && num_entries % 3 != 0) ||
		(tab2 && !tab3 && num_entries % 2 != 0)) {
		Warning();
		printf("%s: # of entries %d in file does not divide evenly into number of tables\n", optargv[0],num_entries);
	}

    rewind(fp);
	/* Skip the specified number of lines */
	for (i = 0 ; i < skiplines;  i++) {
		while(1) {
			j = fgetc(fp);
			if (j == (int)'\n')
				break;
			if (j == EOF) {
				Error();
		printf("%s: EOF encountered before skiplines=%d lines read\n",
				argv[0],skiplines);
				return;
			}
		}
	}
/* Start filling out the entries for the tables */
	/* First, check if we are using the xy option */
	if (do_xy) { /* This section is inherited from the old FileToTab */
        int npts=0;
		double x,y,lastx,lasty;
		double xmin = 1e10, xmax = -1e10;
		double dx;
		double tabx;
		int tabindex;
		int j;

		xdivs = xy_xdivs;
        while (1) {
            if (fscanf(fp,"%f",&temp) < 1) break;
            if (temp<xmin) xmin=temp;
            else if (temp>xmax) xmax=temp;
            if (npts==0) x=temp;
            if (fscanf(fp,"%f",&temp) < 1) break;
            if (npts==0) y=temp;
            npts++;
        }
        rewind(fp);
		/* Skip the specified number of lines */
		for (i = 0 ; i < skiplines;  i++) {
			while(1) {
				j = fgetc(fp);
				if (j == (int)'\n')
					break;
				if (j == EOF) {
					Error();
			printf("%s: EOF encountered before skiplines=%d lines read\n",
					argv[0],skiplines);
					return;
				}
			}
		}
        if (xmax<=xmin) {
			Error();
            printf("%s : x range too small in file \n",optargv[0]);
            return;
        }
		ip = create_interpol(xdivs,xmin,xmax);
		ip->calc_mode = calc_mode;
        dx = ip->dx;
        if (fscanf(fp,"%lf",&x) < 1) ;
        if (fscanf(fp,"%lf",&y) < 1) ;
		ip->table[0] = y;
		tabx = xmin;
		tabindex = 0;
        for(i=1;i<npts;i++) {
            lastx=x;
            lasty=y;
 
            if (fscanf(fp,"%lf",&x) < 1) break;
 /*           index=(int)(0.1+(x-xmin)/dx); */
            if (fscanf(fp,"%lf",&y) < 1) break;
			if (x <= lastx) continue;
			for (; tabindex <=xdivs && tabx >= lastx && tabx < x;
				tabx += dx, tabindex++) {
                /* Find fraction of interval corresponding to pt */
                temp=(tabx-lastx)/(x-lastx);
                /* Do linear interpolation */
                ip->table[tabindex]=lasty*(1.0-temp) + y*temp;
			}
		}
		/* fill out the stuff at the end, if any */
		for (;tabindex <= xdivs; tabindex++)
            ip->table[tabindex]=y;
	/* check if we are filling up 3 tables */
	} else if (tab3) {
		float x,y,z;

		xdivs = num_entries / 3 -1 ;
		ip = create_interpol(xdivs,(float)0.0,(float)1.0);
		ip2 = create_interpol(xdivs,(float)0.0,(float)1.0);
		ip3 = create_interpol(xdivs,(float)0.0,(float)1.0);
		ip->calc_mode = ip2->calc_mode = ip3->calc_mode = calc_mode;
		for(i = 0 ; i <= xdivs; i++) {
            if (fscanf(fp,"%f",&x) < 1) break;
            if (fscanf(fp,"%f",&y) < 1) break;
            if (fscanf(fp,"%f",&z) < 1) break;
			ip->table[i] = x;
			ip2->table[i] = y;
			ip3->table[i] = z;
		}
	/* check if we are filling up 2 tables */
	} else if (tab2) {
		float x,y;

		xdivs = num_entries / 2 - 1; /* ips use xdivs inclusive */
		ip = create_interpol(xdivs,(float)0.0,(float)1.0);
		ip2 = create_interpol(xdivs,(float)0.0,(float)1.0);
		ip->calc_mode = ip2->calc_mode = calc_mode;
		for(i = 0 ; i <= xdivs; i++) {
            if (fscanf(fp,"%f",&x) < 1) break;
            if (fscanf(fp,"%f",&y) < 1) break;
			ip->table[i] = x;
			ip2->table[i] = y;
		}
	/* check if we are filling up 1 table */
	} else if (tab1) {
		float x;

		xdivs = num_entries - 1;
		ip = create_interpol(xdivs,(float)0,(float)1);
		ip->calc_mode = calc_mode;
		for(i = 0 ; i <= xdivs; i++) {
            if (fscanf(fp,"%f",&x) < 1) break;
			ip->table[i] = x;
		}
	}

	if (tab1 && autofilltab) {
		xdivs = num_entries - 1;
		ip2 = create_interpol(xdivs,(float)0,(float)1);
		ip2->calc_mode = calc_mode;
		for(i = 0 ; i <= xdivs; i++) {
			ip2->table[i] = i;
		}
		tab2 = autofilltab;  /* to make the targs[0] below happy */
	}
	fclose(fp);

	/* We need to let the elements own SET action handle the
	** assignment of the table in order to correctly update
	** dependencies on table pointers and so on.
	*/
	action.name = "SET";
	action.type = SET;
	action.argc = 2;
	action.argv = targs;
	targs[0] = tab1; /* the name of the table */
	targs[1] = "*loadtab";

	if (ip) {
		action.data = (char *)ip;
		CallActionFunc(elm,&action);
	}
	if (ip2) {
		targs[0] = tab2;
		action.data = (char *)ip2;
		CallActionFunc(elm,&action);
	}
	if (ip3) {
		targs[0] = tab3;
		action.data = (char *)ip3;
		CallActionFunc(elm,&action);
	}
}

/* Utility function for writing one or more tables to an ascii file.
** The output ascii file has the entries for the respective tables
** in successive positions. Various specialized output modes let
** one do on-the-fly conversions of the table entries:
** 
*/
void TabToFile(argc,argv)
	int argc;
	char	**argv;
{
	FILE	*fp,*fopen();
	Interpol	*ip;
	Interpol	*ip2 = NULL;
	Interpol	*ip3 = NULL;
	char	*tab1;
	char	*tab2 = NULL;
	char	*tab3 = NULL;
	Element	*elm;
	int i;
	char mode='y';
	char* file_mode = "a";
	int		status;
	int		tabsize;
	int             nentries = 0;

	initopt(argc, argv, "file-name element table -table2 table -table3 table -mode mode -nentries n -overwrite\n");

	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-mode") == 0) {
			mode = optargv[1][0];
		}
	    if (strcmp(G_optopt, "-overwrite") == 0) {
			file_mode = "w";
		}
	    if (strcmp(G_optopt, "-table2") == 0) {
			tab2 = optargv[1];
		}
	    if (strcmp(G_optopt, "-table3") == 0) {
			tab3 = optargv[1];
		}
	    if (strcmp(G_optopt, "-nentries") == 0) {
                        nentries = atoi(optargv[1]);
	    }
	  }
	if (status < 0)
	  {
	    printoptusage(argc, argv);
		printf("mode can be y, xy, alpha, beta, tau, minf index. 'y' is default\n");
	    return;
	  }

	switch(mode) {
		case 'a':
		case 't':
		case 'x':
		case 'i':
			/* these cases must have only one table */
			if (tab2 || tab3) {
	    		printoptusage(argc, argv);
				printf("in modes: alpha tau xy index ; one table must be specified\n");
				return;
			}
		break;
		case 'b':
		case 'm':
			/* these cases must have 2 tables */
			if (!tab2) {
	    		printoptusage(argc, argv);
				printf("in modes: beta minf ; two tables must be specified\n");
				return;
			}
		break;
	}

	elm = GetElement(optargv[2]);
	if (!elm) {
		printf("Element '%s' not found\n",optargv[2]);
		return;
	}
	tab1 = optargv[3];

	if (!(ip = GetTabByName(elm,tab1))) /* it has already complained */
		return;

	if (tab2) {
		if (!(ip2 = GetTabByName(elm,tab2)))
			return;
	}

	if (tab3) {
		if (!(ip3 = GetTabByName(elm,tab3)))
			return;
	}

	fp=fopen(optargv[1],file_mode);
	if (!fp) {
		printf("Could not open file %s\n",argv[1]);
		return;
	}

	switch(mode) {
		case 'a': /* print the alpha form of the table */
			/*
			for(i=0;i<=ip->xdivs;i++) {
				fprintf(fp,"%f	%f\n",ip->xmin+ip->dx*i,
					ip->table[i]);
			}
			*/
			tabsize = (nentries > 0 && (nentries < ip->xdivs)) ?
                                nentries : ip->xdivs;
                        for(i=0;i<=nentries;i++) {
                                fprintf(fp,"%f  %f\n",ip->xmin+ip->dx*i,
                                        ip->table[i]);
                        }
			break;
		case 'b':  /* beta */
			tabsize = (ip->xdivs > ip2->xdivs) ? ip2->xdivs : ip->xdivs;
			tabsize = (nentries > 0 && (nentries < tabsize)) ?
                                nentries : tabsize;
			for(i=0;i<=tabsize;i++) {
				fprintf(fp,"%f	%f\n",ip2->xmin+ip2->dx*i,
					ip2->table[i]-ip->table[i]);
			}
			break;
		case 't': /* tau */
			 tabsize = (nentries > 0 && (nentries < ip->xdivs)) ?
                                nentries : ip->xdivs;
			for(i=0;i<=tabsize;i++) {
				fprintf(fp,"%f	%f\n",ip->xmin+ip->dx*i,
					1.0/ip->table[i]);
			}
			break;
		case 'm': /* m-infinity */
			tabsize = (ip->xdivs > ip2->xdivs) ? ip2->xdivs : ip->xdivs;
			tabsize = (nentries > 0 && (nentries < tabsize)) ?
                                nentries : tabsize;
			for(i=0;i<=tabsize;i++) {
				fprintf(fp,"%f	%f\n",ip->xmin+ip->dx*i,
					ip->table[i]/ip2->table[i]);
			}
			break;
		case 'i': /* index the table */
			tabsize = (nentries > 0 && (nentries < ip->xdivs)) ?
                                nentries : ip->xdivs;
			for(i=0;i<=tabsize;i++) {
				fprintf(fp,"%d	%f\n",i,ip->table[i]);
			}
			break;
		case 'x': /* the dx based x coordinate */
			tabsize = (nentries > 0 && (nentries < ip->xdivs)) ?
                                nentries : ip->xdivs;
			/* for(i=0;i<=ip->xdivs;i++) { */
			    for(i=0;i<=tabsize;i++) {
				fprintf(fp,"%f	%f\n",ip->xmin+ip->dx*i,ip->table[i]);
			}
			break;
		case 'y':
		default :
			if (tab2 && tab3) {
				tabsize =
					(ip->xdivs > ip2->xdivs) ? ip2->xdivs : ip->xdivs;
				if (tabsize > ip3->xdivs)
					tabsize = ip3->xdivs;
				 tabsize = (nentries > 0 && (nentries < tabsize)) ?
                                        nentries : tabsize;
				for(i=0;i<=tabsize;i++) {
					fprintf(fp,"%g	%g	%g\n",
						ip->table[i],ip2->table[i],ip3->table[i]);
				}
			} else if (tab2) {
				tabsize =
					(ip->xdivs > ip2->xdivs) ? ip2->xdivs : ip->xdivs;
				tabsize = (nentries > 0 && (nentries < tabsize)) ?
                                        nentries : tabsize;
				for(i=0;i<=tabsize;i++) {
					fprintf(fp,"%g	%g\n",
						ip->table[i],ip2->table[i]);
				}
			} else {
				 tabsize = (nentries > 0 && (nentries < ip->xdivs)) ?
                                        nentries : ip->xdivs;
				/* for(i=0;i<=ip->xdivs;i++) { */
			        for(i=0;i<=tabsize;i++) {
					fprintf(fp,"%g\n",ip->table[i]);
				}
			}
		break;
	}
	fclose(fp);
}

static Interpol *GetTabByName(elm,table)
	Element *elm;
	char *table;
{
	Interpol **pip;
	short	datatype;

	if ((pip = (Interpol **)GetFieldAdr(elm,table,&datatype))) {
		if (datatype == INVALID) {
			if (!(*pip)) { /* The interpol has not been allocated */
				Error();
				printf("table %s on element %s has not been allocated\n",
					table,Pathname(elm));
			}
			return(*pip);
		} else {
			Error();
			printf("field %s on element %s is not a table\n",
				table, Pathname(elm));
			return(NULL);
		}
	} else {
		Error();
		printf("could not find table %s on element %s\n",
			table, Pathname(elm));
		return(NULL);
	}
}

#define MAX_SHOW_INTERPOL 500
/* Utility function for showing the contents of an interpol */
int ShowInterpol(action,ipname,ip,dim,nmax)
	Action	*action;
	char *ipname;
	Interpol *ip;
	short	dim;
	int		nmax;
{
	char	dumpline[500];
	char	*str;
	char	val[25];
	int		i,j,len;
	Interpol2D *ip2;
	
	if (strcmp(action->data,ipname) != 0)
		return(0);
	action->passback = "";
	if (!ip || !dim) {
		action->passback = "Interpol struct: Not allocated";
	} else if (dim==1) {	/* Interpol */
		printf("%-20s = Interpol struct: \n",ipname);
		printf("  %-10s %-10s %-5s %-5s %-5s %-10s\n",
			"allocated","calc_mode","xdivs","xmin","xmax","dx");
		printf("  %-10d %-10d %-5d %-5g %-5g %-10g\n",
			ip->allocated,ip->calc_mode,
			ip->xdivs,ip->xmin,ip->xmax,ip->dx);
		if (!ip->allocated  || !ip->table) {
			action->passback = "array in interpol not allocated\n";
		} else {
			sprintf(dumpline," [%3d] ",0);
       		str = dumpline + strlen(dumpline);
			for(i = 0; i <= ip->xdivs && i <= nmax; i++) {
		        sprintf(val,"%g",ip->table[i]);
        		len = strlen(val);
        		if (len + (str - dumpline) > 76) {
            		printf("%s\n",dumpline);
					sprintf(dumpline," [%3d] ",i);
            		str = dumpline + strlen(dumpline);
        		}
        		*str = ' ';
        		str++;
        		strcpy(str,val);
        		str += len;
			}
			printf("%s\n",dumpline);
			if (ip->xdivs > nmax)
				printf("   .... %d entries not displayed\n",
					ip->xdivs-nmax);
			action->passback = "End of interpol display";
		}
	} else if (dim==2) {	/* Interpol2D */
		ip2=(Interpol2D *)ip;
		printf("%-20s = Interpol2D struct: \n",ipname);
		printf("  %-10s %-10s %-5s %-5s %-5s %-8s %-5s %-5s %-5s %-8s\n",
			"allocated","calc_mode","xdivs","xmin","xmax","dx",
			"ydivs","ymin","ymax","dy");
		printf("  %-10d %-10d %-5d %-5g %-5g %-8g %-5d %-5g %-5g %-8g\n",
			ip2->allocated,ip2->calc_mode,
			ip2->xdivs,ip2->xmin,ip2->xmax,ip2->dx,
			ip2->ydivs,ip2->ymin,ip2->ymax,ip2->dy);
		if (!ip2->allocated  || !ip2->table) {
			action->passback = "array in interpol not allocated\n";
		} else {
			sprintf(dumpline," [%3d][%3d] ",0,0);
       		str = dumpline + strlen(dumpline);
			for(i = 0; i <= ip2->xdivs && i <= nmax; i++) {
				for(j = 0; j <= ip2->ydivs && j <= nmax; j++) {
					sprintf(val,"%g",ip2->table[i][j]);
					len = strlen(val);
					if (len + (str - dumpline) > 76) {
						printf("%s\n",dumpline);
						sprintf(dumpline," [%3d][%3d] ",i,j);
						str = dumpline + strlen(dumpline);
					}
					*str = ' ';
					str++;
					strcpy(str,val);
						str += len;
					}
			}
			printf("%s\n",dumpline);
			if (ip2->xdivs > nmax) {
				i=ip2->xdivs-nmax;
			} else {
				i=0;
			}
			if (ip2->ydivs > nmax) i=(ip2->xdivs + i) * (ip2->ydivs-nmax);
			if (i>0)  printf("   .... %d entries not displayed\n",i);
			action->passback = "End of interpol display";
		}
	} else {
		action->passback = "Interpol struct: unknown table dimension";
	}
	return(1);
}
#undef MAX_SHOW_INTERPOL

