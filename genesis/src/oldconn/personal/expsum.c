static char rcsid[] = "$Id: expsum.c,v 1.2 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: expsum.c,v $
** Revision 1.2  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1999/12/27 10:49:22  mhucka
** Added fix to ExpSum from Mike Vanier.
**
** Revision 1.3  1997/07/18 03:11:14  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 23:46:34  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:37  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/* 3/89 Matt Wilson */
float ExpSum(argc,argv)
int argc;
char **argv;
{
float 		maxval;
float 		rate;
float 		minval;
float 		x,y;
ElementList	*list;
Element		*element;
char 		*path;
int		i;
float		px,py;
float		sum;

    initopt(argc, argv, "path rate maxvalue minvalue x y");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    path = optargv[1];
    rate = Atof(optargv[2]);
    maxval = Atof(optargv[3]);
    minval = Atof(optargv[4]);
    px = Atof(optargv[5]);
    py = Atof(optargv[6]);

    list = WildcardGetElement(path,1);
    sum = 0;
    for(i=0;i<list->nelements;i++){
	element = list->element[i];
	x = element->x;
	y = element->y;
	/*
	** set the weight according to the radial distance between
	** the source and dst and the velocity
	*/
	sum += (maxval - minval) *
	  exp(rate * sqrt((x - px) * (x - px) + (y - py) * (y - py))) + minval;
    }
    FreeElementList(list);
    return(sum);
}
