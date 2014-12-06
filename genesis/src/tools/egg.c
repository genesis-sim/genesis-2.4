static char rcsid[] = "$Id: egg.c,v 1.4 2005/10/21 21:16:24 svitak Exp $";

/*
** $Log: egg.c,v $
** Revision 1.4  2005/10/21 21:16:24  svitak
** Using PI from math.h.
**
** Revision 1.3  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 18:11:12  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:05:52  dhb
** Initial revision
**
*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

#ifndef M_PI
#define M_PI 3.1415927
#endif

float newpt();

void do_egg(argc,argv)
	int	argc;
	char	**argv;
{
	FILE	*fp,*fopen();
	float	x,y,z,d,cx,cy,cz,cr;
	float	theta,phi;
	float	nx,ny,nz;
	float	temp;
	int		i = 0;
	int 	normal;
	float	fudge;
	int	status;

	normal = 0;

	initopt(argc, argv, "file-name x y z d -normal");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-normal") == 0)
		normal = 1;
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		printf("\n");
		printf("This routine generates coords for an egg with specified dimensions x,y,z\n");
		printf("d specifies the spacing.\n");
		printf("the -normal option decides if a normal should also be calculated\n");	
		return;
	}

	x = Atof(optargv[2])/2.0;
	y = Atof(optargv[3])/2.0;
	z = Atof(optargv[4])/2.0;
	d = Atof(optargv[5]);
	fudge = sqrt(x * y);

	fp = fopen(optargv[1],"w");
	if (fp == NULL)
	  {
	    perror(optargv[1]);
	    return;
	  }

	for(cz = z, cr = 0.0, phi = 0.0; phi < M_PI ;
		phi += newpt(&cz,&cr,z,fudge,d)) {
		for(cx = cr * x/fudge, cy = 0.0, theta = 0.0; theta < (M_PI * 2);
			theta += newpt(&cx,&cy,cr * x/fudge,cr * y/fudge,d)) {
			i++;
			if (normal) {
				nx = cx /(x * x);
				ny = cy /(y * y);
				nz = cz /(z * z);
				temp = sqrt(nx * nx + ny * ny + nz * nz);
				nx /= temp;
				ny /= temp;
				nz /= temp;
				fprintf(fp,"%-10.6e	%-10.6e	%-10.6e	%-10.6e	%-10.6e	%-10.6e\n",cx,cy,cz,nx,ny,nz);
			} else {
				fprintf(fp,"%-20.6e	%-20.6e	%-20.6e\n",cx,cy,cz);
			}
		}
	}
	printf ("%d coordinates were found\n",i);
	fclose(fp);
}

/* Given coords x,y, and the required dist d, gets dtheta by assuming
** a circle, using the new theta to calculate new coords x,y. dtheta
** is calculated agin for this position, and the mean taken as the
** correct dtheta, which is used to calculated the returned coords.
*/
float newpt(nx,ny,X,Y,d)
	float *nx,*ny;
	float X,Y,d;
{
	float dt;
	float len;
	float x,y;
	float tx,ty;
	float s,c;
	float ds,dc,ns,nc;

	if ((X * M_PI) < d || (Y * M_PI) < d)
		return(2 * M_PI);

	x = *nx;
	y = *ny;
	len = sqrt(x * x + y * y);
	dt = d / len;

	s = y/Y;
	c = x/X;
	ds = sin(dt);
	dc = cos(dt);
	ns = s * dc + ds * c;
	nc = c * dc - s * ds;

	tx = X * nc;
	ty = Y * ns;
	len = sqrt(tx * tx + ty * ty);

	/* Average of two dthetas */
	dt = (dt + d/len)/2.0;

	ds = sin(dt);
	dc = cos(dt);
	ns = s * dc + ds * c;
	nc = c * dc - s * ds;

	*nx = X * nc;
	*ny = Y * ns;

	return(dt);
}
