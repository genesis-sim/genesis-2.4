static char rcsid[] = "$Id: expweight.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: expweight.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:05  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:11:14  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 23:46:34  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:38  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/* 9/88 Matt Wilson */
int ExpWeight(argc,argv)
int argc;
char **argv;
{
float 		maxval;
float 		minval;
float 		x,y;
Projection 	*projection;
ElementList	*list;
char 		*path;
int		i;
register Connection *connection;
register float scale;
register float rate;

    initopt(argc, argv, "path rate maxvalue minvalue");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    path = optargv[1];
    rate = Atof(optargv[2]);
    maxval = Atof(optargv[3]);
    minval = Atof(optargv[4]);

    scale = maxval-minval;
    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	projection = (Projection *)list->element[i];
	x = projection->x;
	y = projection->y;
	/*
	** set the weight according to the radial distance between
	** the source and dst and the velocity
	*/
	for(connection=projection->connection;connection;
	connection=connection->next){
	connection->weight =
		scale*exp(rate*sqrt(
		sqr(x - connection->target->x) +
		sqr(y - connection->target->y)
		)) + minval;
	}
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return(1);
}

/* 9/88 Matt Wilson */
int AbsExpWeight(argc,argv)
int argc;
char **argv;
{
float rate;
float maxval;
float minval;
float scale;
Connection *connection;
ElementList	*list;
char *path;
int	i;

    initopt(argc, argv, "path rate maxvalue minvalue");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    path = optargv[1];
    rate = Atof(optargv[2]);
    maxval = Atof(optargv[3]);
    minval = Atof(optargv[4]);

    scale = maxval-minval;
    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	/*
	** set the weight according to the radial distance between
	** the source and dst and the velocity
	*/
	for(connection= ((Projection *)(list->element[i]))->connection;
	connection;
	connection=connection->next){
	connection->weight =
	    scale*exp(rate*sqrt(
		sqr(connection->target->x) +
		sqr(connection->target->y)
		)) + minval;
	}
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return(1);
}
