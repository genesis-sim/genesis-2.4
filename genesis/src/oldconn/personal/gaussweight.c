static char rcsid[] = "$Id: gaussweight.c,v 1.2 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: gaussweight.c,v $
** Revision 1.2  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:39  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/* 
** 9/88 Matt Wilson 
**
** y = (scale - min)*exp( -1/2 * ((x-x1)^2 + (y-y1)^2)/sigma^2) + min
*/
int GaussianWeight(argc,argv)
int argc;
char **argv;
{
float sigma;
float maxval;
float minval;
float x,y;
Connection *connection;
Projection *projection;
ElementList	*list;
char *path;
int	i;

    if(argc < 4){
	printf("usage: %s path sigma maxval minval\n",argv[0]);
	return(0);
    }
    path = argv[1];
    sigma = -2*pow(Atof(argv[2]),2.0);
    maxval = Atof(argv[3]);
    minval = Atof(argv[4]);
    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	projection = (Projection *)list->element[i];
	x = projection->x;
	y = projection->y;
	/*
	** set the weight according to the gaussian of the distance between
	** the source and dst 
	*/
	for(connection=projection->connection;connection;
	connection=connection->next){
	connection->weight =
	    (maxval - minval)*exp(
		(pow(x - connection->target->x,2.0) +
		pow(y - connection->target->y,2.0)
		)/sigma) + minval;
	}
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return(1);
}

