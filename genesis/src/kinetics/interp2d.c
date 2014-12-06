/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
static char rcsid[] = "$Id: interp2d.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";
 
/* $Log: interp2d.c,v $
/* Revision 1.1.1.1  2005/06/14 04:38:34  svitak
/* Import from snapshot of CalTech CVS tree of June 8, 2005
/*
/* Revision 1.2  2000/06/19 06:14:30  mhucka
/* The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
/* structures are doubles.  Functions that read these values from input files
/* (e.g., for implementing tabcreate actions) need to treat them as doubles,
/* not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
/* compiler), treating them as float results in roundoff errors.
/*
/* Revision 1.1  1997/07/24 17:49:40  dhb
/* Initial revision
/*
 * Revision 1.1  1994/06/13  22:55:39  bhalla
 * Initial revision
 * */

#include "kin_ext.h"
#define DEFAULT_XDIVS 3000
#define DEFAULT_XMIN ((double) -0.1)
#define DEFAULT_XMAX ((double) 0.05)
#define SETUP_ALPHA 0
#define SETUP_TAU 1

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

#ifndef NO_INTERP
#define NO_INTERP 0
#define LIN_INTERP 1
#define FIXED 2
#endif



/*
** The table consists of xdivs + 1 points. Table[0] and Table[xdivs]
** are used as truncation values.
*/

double Tab2DInterp(ip,x,y)
	Interpol2D *ip;
	double x,y;
{
	register double t,u;
	register int ilo,jlo;
	register double **table;

	switch(ip->calc_mode) {
		case NO_INTERP : /* Assumes that the grid-points are sampled
			** finely enough, and the xman and yminshifted by dx/2,dy/2
			** so that the truncation becomes equiv to a rounding. */
			if (ip->table) {
				ilo = ((x - ip->xmin)*ip->invdx);
				jlo = ((y - ip->ymin)*ip->invdy);

			/* Check that ilo,jlo lie within limits */
				if (ilo<0)
					ilo = 0;
				else if (ilo>ip->xdivs)
					ilo = ip->xdivs;

				if (jlo<0)
					jlo = 0;
				else if (jlo>ip->ydivs)
					jlo = ip->ydivs;
				return(ip->table[ilo][jlo]);

			} else {
				return(0.0);
			}
			break;
		case LIN_INTERP : /* Num rec C 1988 pg 105 Press et al. */
			table = ip->table;
			if(table) {
				ilo = t = (x - ip->xmin)*ip->invdx;
				jlo = u = (y - ip->ymin)*ip->invdy;

			/* Check that ilo,jlo lie within limits */
				if (ilo<0) {
					ilo = 0;
					t = 0.0;
				} else if (ilo>=ip->xdivs) {
					ilo = ip->xdivs-1 ;
					t = 1.0;
				} else {
					t -= (double)ilo;
				}
				if (jlo<0) {
					jlo = 0;
					u = 0.0;
				} else if (jlo>=ip->ydivs) {
					jlo = ip->ydivs-1 ;
					u = 1.0;
				} else {
					u -= (double)jlo;
				}

				return(
					(1.0-t)	*	(1.0-u)	*	table[ilo][jlo]		+
					t		*	(1.0-u)	*	table[ilo+1][jlo]	+
					t		*	u		*	table[ilo+1][jlo+1]	+
					(1.0-t)	*	u		*	table[ilo][jlo+1]	);
					
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
			break;
		case FIXED :
			return(ip->oz);
			break;
	}
}

Interpol2D *create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax)
	int xdivs;
	double xmin,xmax;
	int ydivs;
	double ymin,ymax;
{
	Interpol2D *ip;
	int i;

	ip = (Interpol2D *) calloc(1,sizeof(Interpol2D));

	ip->xdivs = xdivs;
	ip->xmin = xmin;
	ip->xmax = xmax;
	ip->table = (double **)calloc(xdivs + 1,sizeof(double*));
	ip->ydivs = ydivs;
	ip->ymin = ymin;
	ip->ymax = ymax;
	for(i=0;i<=xdivs;i++)
		ip->table[i] = (double *)calloc(ydivs + 1,sizeof(double));
	ip->allocated = 1;
	ip->calc_mode = LIN_INTERP;
	ip->sx = ip->sy = ip->sz = 1.0;
	ip->ox = ip->oy = ip->oz = 0.0;

	ip->dx = (ip->xmax - ip->xmin) / (double)(ip->xdivs);
	ip->invdx=1.0/ip->dx;

	ip->dy = (ip->ymax - ip->ymin) / (double)(ip->ydivs);
	ip->invdy=1.0/ip->dy;

	return(ip);
}

/*
** Special function for tweaking table :
** scale_table(interpol,sx,sy,ox,oy)
** where sx, sy are magnification factors on X and Y, and ox,oy are
** offsets. The new values are then installed in the interpol struct
*/

scale_table2d(ip,field,value)
	Interpol2D	*ip;
	char		*field;
	char		*value;
{
	double	**table;
	int		i,j;
	int		xdivs,ydivs;
	double	osz,ooz;
	double	sx,sy,sz,ox,oy,oz;

	sx = ip->sx; sy = ip->sy; sz = ip->sz;
	ox = ip->ox; oy = ip->oy; oz = ip->oz;

	if (strcmp(field,"sx") == 0)
		sscanf(value,"%lf",&sx);
	else if (strcmp(field,"sy") == 0)
		sscanf(value,"%lf",&sy);
	else if (strcmp(field,"sz") == 0)
		sscanf(value,"%lf",&sz);
	else if (strcmp(field,"ox") == 0)
		sscanf(value,"%lf",&ox);
	else if (strcmp(field,"oy") == 0)
		sscanf(value,"%lf",&oy);
	else if (strcmp(field,"oz") == 0)
		sscanf(value,"%lf",&oz);
	else 
		return;

	xdivs = ip->xdivs;
	ydivs = ip->ydivs;
	table = ip->table;
	osz = ip->sz;
	ooz = ip->oz;
	if (oz != ooz || osz != sz) {
		osz = sz / ip->sz;
		for (i = 0 ; i <= xdivs ; i++) {
			for (j = 0 ; j <= ydivs ; j++)
				table[i][j] = oz + (table[i][j] - ooz) * osz;
		}
	}
	ip->xmin = ox + (ip->xmin - ip->ox) * sx / ip->sx;
	ip->xmax = ox + (ip->xmax - ip->ox) * sx / ip->sx;
    ip->dx = (ip->xmax - ip->xmin) / (double)(ip->xdivs);
    ip->invdx = 1.0/ip->dx;

	ip->ymin = oy + (ip->ymin - ip->oy) * sy / ip->sy;
	ip->ymax = oy + (ip->ymax - ip->oy) * sy / ip->sy;
    ip->dy = (ip->ymax - ip->ymin) / (double)(ip->ydivs);
    ip->invdy = 1.0/ip->dy;

	ip->ox = ox;
	ip->oy = oy;
	ip->oz = oz;
	ip->sx = sx;
	ip->sy = sy;
	ip->sz = sz;
}

/*
** Special function for finding min/max values in table, between
** lox and hix.
** get_table_min(table,lox,hix)
** get_table_max(table,lox,hix)
**
** Special function for finding roots (x value for given y)
*/


free_interpol2d(ip)
	Interpol2D	*ip;
{
	int i;

	if (ip->allocated) {
		for(i=0;i<=ip->xdivs;i++)
			free(ip->table[i]);
		free(ip->table);
	}
	free(ip);
}

/* This needs to be similar to the DiskIn element */
FileToTab2D(argc,argv)
	int argc;
	char	**argv;
{
}

/* This needs to be related to the DiskOut element */
TabToFile2D(argc,argv)
	int argc;
	char	**argv;
{
}
