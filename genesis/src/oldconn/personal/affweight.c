static char rcsid[] = "$Id: affweight.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: affweight.c,v $
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
** Revision 1.1  1992/12/11 19:03:36  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/* 9/88 Matt Wilson */
int AffWeight(argc,argv)
int argc;
char **argv;
{
Projection *projection;
Connection *connection;
float	x_dst,y_dst;
float	tx;
float	tan_theta;
float	d;
float	angle;
int	i;
ElementList	*list;
char 	*path;
float 	scale;
float	mrate;
float	crate;
float	minval;

    if(argc < 6){
	Error();
	printf("usage: %s path angle scale mainrate collrate [minval]\n",
	argv[0]);
	return 0;
    }
    path = argv[1];
    angle = Atof(argv[2]);
    tan_theta = tan(2*M_PI*angle/360);
    /* 
    ** get the max weight
    */
    scale = Atof(argv[3]);
    /* 
    ** get the lambda of the collaterals
    */
    mrate = Atof(argv[4]);
    crate = Atof(argv[5]);
    if(argc > 6){
	minval = Atof(argv[6]);
    } else {
	minval = 0;
    }

    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	projection = (Projection *)list->element[i];
	for(connection=projection->connection;connection;
	connection=connection->next){
	    x_dst = connection->target->x;
	    y_dst = connection->target->y;
	    d = MIN(x_dst, y_dst/tan_theta);
	    /*
	    ** calculate the weight distribution according to the 
	    ** distance along the main fiber tract
	    */
	    tx = (scale -minval)*exp(mrate*(x_dst - d))+minval;
	    /*
	    ** calculate the weight distribution according to the 
	    ** distance along the angled collateral
	    */
	    connection->weight = (tx -minval)*
	    exp(crate*sqrt(y_dst*y_dst + d*d))+minval;
	}
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return 1;
}

