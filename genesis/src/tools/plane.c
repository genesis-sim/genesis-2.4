static char rcsid[] = "$Id: plane.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: plane.c,v $
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
** Revision 1.1  1992/12/11  19:06:04  dhb
** Initial revision
**
*/

#include <math.h>
#include <stdio.h>
#include <string.h>	/* strcmp */
#include "header.h"

#define PI 3.1415927
/* mds3 changes */
/*
** #define MAXLONG                2147483641L
** #ifdef i860
** #define frandom(l,h)    (((float)rand()/MAXLONG)*((h)-(l))+(l))
** #else
** #define frandom(l,h)    (((float)random()/MAXLONG)*((h)-(l))+(l))
** #endif
*/

float newpt();

void do_plane(argc,argv)
	int	argc;
	char	**argv;
{
	FILE	*fp,*fopen();
	float	x,y,dx,dy;
	int		i = 0;
	int 	normal = 0;
	int 	ellipse = 0;
	int 	hex = 0;
	int		center = 0;
	float	cx = 0,cy = 0;
	int		jitter = 0;
	float	jx = 0,jy = 0;
	float	tx,ty,px,py;
	int	status;

	initopt(argc, argv, "file-name x y dx dy -center cx cy -jitter jx jy -normal -ellipse -hexagonal");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt,"-normal") == 0) {
		    normal = 1;
	    }
	    if (strcmp(G_optopt,"-ellipse") == 0) {
		    ellipse = 1;
	    }
	    if (strcmp(G_optopt,"-hexagonal") == 0) {
		    hex = 1;
		    printf("Hexagonal mode not yet implemented\n");
	    }
	    if (strcmp(G_optopt,"-center") == 0) {
		    center = 1; i++;
		    cx = Atof(optargv[1]);
		    cy = Atof(optargv[2]);
	    }
	    if (strcmp(G_optopt,"-jitter") == 0) {
		    jitter = 1; i++;
		    jx = Atof(optargv[1]);
		    jy = Atof(optargv[2]);
	    }
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		printf("\n");
		printf("This routine generates coords for a plane with specified dimensions x,y\n");
		printf("dx,dy specify the spacing.\n");
		printf("cx,cy specify the center of the plane : default is (0,0).\n");
		printf("jx,jy specify the jitter as a fraction of the spacing.\n");
		printf("the -normal option decides if a normal should also be calculated\n");	
		printf("the -ellipse option puts the cells in an ellipse rather than a rectangle\n");	
		printf("the -hexagonal option puts the cells in a hex array\n");	
		return;
	}

	x = Atof(optargv[2])/2.0;
	y = Atof(optargv[3])/2.0;
	dx = Atof(optargv[4]);
	dy = Atof(optargv[5]);


	fp = fopen(optargv[1],"w");
	if (fp == NULL)
	  {
	    perror(optargv[1]);
	    return;
	  }

	i = 0;
	for (px = -x ; px < x ; px += dx) {
		for (py = -y; py < y; py += dy) {
			tx = px; ty = py;
			if (ellipse) {
				if ((tx*tx/(x*x) + ty*ty/(y*y)) > 1.0)
					continue;
			}
			i++;
			if (center) {
				tx += cx;
				ty += cy;
			}
			if (jitter) {
				tx += dx * jx * frandom(-1.0,1.0);
				ty += dy * jy * frandom(-1.0,1.0);
			}
			if (normal) {
				fprintf(fp,"%-10.6e	%-10.6e	%-10.6e	%-10.6e	%-10.6e	%-10.6e\n",tx,ty,0.0,0.0,0.0,1.0);
			} else {
				fprintf(fp,"%-20.6e	%-20.6e	%-20.6e\n",tx,ty,0.0);
			}
		}
	}
	fclose(fp);
	printf ("%d coordinates were found\n",i);
}
