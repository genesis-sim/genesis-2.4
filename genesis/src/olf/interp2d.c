static char rcsid[] = "$Id: interp2d.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/*
** This file has been modified from its original form as indicated below.
** The original version lives in the kinetics library, though it is not
** compiled into genesis.  ---dhb
*/

/* Version EDS20k 95/09/29 */

/*
** $Log: interp2d.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2000/06/19 06:14:30  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.4  2000/06/12 05:07:03  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.3  2000/03/27 10:29:57  mhucka
** Fix from Hugo Cornelis: include errno.h.
**
** Revision 1.2  1997/07/29 14:40:56  dhb
** Added copyright notice referencing LGPL.
**
** Revision 1.1  1997/05/29 08:21:37  dhb
** Initial revision
**
 * EDS20k revison: EDS BBF-UIA 95/09/29
 * Changed procedures so that if xdivs==0 behavior reverts to one-
 *  dimensional table
 *
 * EDS20j revison: EDS BBF-UIA 95/08/11
 * Corrected limit checking for LIN_INTERP: failed if t==-0.2 (then
 *  ilo was 0)
 *
*/

/* Code written by Upi Bhalla */

#include "olf_ext.h"
#include "olf_defs.h"

#include <errno.h>

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
				jlo = ((y - ip->ymin)*ip->invdy);
			/* Check that ilo,jlo lie within limits */
				if (jlo<0)
					jlo = 0;
				else if (jlo>ip->ydivs)
					jlo = ip->ydivs;

				if (ip->xdivs) {   /* 2-dimensional */
					ilo = ((x - ip->xmin)*ip->invdx);
					if (ilo<0)
						ilo = 0;
					else if (ilo>ip->xdivs)
						ilo = ip->xdivs;
					return(ip->table[ilo][jlo]);
				} else {	/* 1-dimensional */
					return(ip->table[0][jlo]);
				}
			} else {
				return(0.0);
			}
			/* NOTREACHED */
			break;
		case LIN_INTERP : /* Num rec C 1988 pg 105 Press et al. */
			table = ip->table;
			if(table) {
				jlo = u = (y - ip->ymin)*ip->invdy;
			/* Check that ilo,jlo lie within limits */
				if (u<0.0) {
					jlo = 0;
					u = 0.0;
				} else if (u>=ip->ydivs) {
					jlo = ip->ydivs-1 ;
					u = 1.0;
				} else {
					u -= (double)jlo;
				}
				if (ip->xdivs) {   /* 2-dimensional */
					ilo = t = (x - ip->xmin)*ip->invdx;
					if (t<0.0) {
						ilo = 0;
						t = 0.0;
					} else if (t>=ip->xdivs) {
						ilo = ip->xdivs-1 ;
						t = 1.0;
					} else {
						t -= (double)ilo;
					}
					return(
						(1.0-t)	*	(1.0-u)	*	table[ilo][jlo]		+
						t		*	(1.0-u)	*	table[ilo+1][jlo]	+
						t		*	u		*	table[ilo+1][jlo+1]	+
						(1.0-t)	*	u		*	table[ilo][jlo+1]	);
 				} else {    /* 1-dimensional */
					t=ip->table[0][jlo];
					return(u*(ip->table[0][jlo+1]-t)+t);
				}
			} else {
				return(0.0);
			}
			/* NOTREACHED */
			break;
		case FIXED :
			return(ip->oz);
			/* NOTREACHED */
			break;
	}
	return(0.0);
}

Interpol2D *create_interpol2d(xdivs,xmin,xmax,ydivs,ymin,ymax)
	int xdivs;
	double xmin,xmax;
	int ydivs;
	double ymin,ymax;
{
	Interpol2D *ip;
	int i;

	if ((xdivs<0)||(ydivs<=0)) {
		fprintf(stderr,"Error while creating table2d: zero x or y dimension\n");
		return(NULL);
	}

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

	if (xdivs) {	/* 2-dimensional */
		ip->dx = (ip->xmax - ip->xmin) / (double)(ip->xdivs);
		ip->invdx=1.0/ip->dx;
	}

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

void scale_table2d(ip,field,value)
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
	if (xdivs) {	/* 2-dimensional */
		ip->dx = (ip->xmax - ip->xmin) / (double)(ip->xdivs);
		ip->invdx = 1.0/ip->dx;
	}

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


void free_interpol2d(ip)
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
void FileToTab2D(argc,argv)
	int argc;
	char	**argv;
{
}

/* This needs to be related to the DiskOut element */
void TabToFile2D(argc,argv)
	int argc;
	char	**argv;
{
}
