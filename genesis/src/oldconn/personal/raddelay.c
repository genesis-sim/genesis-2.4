static char rcsid[] = "$Id: raddelay.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: raddelay.c,v $
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
** Revision 1.1  1992/12/11  19:03:40  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/* 9/88 Matt Wilson */
int RadialDelay(argc,argv)
int argc;
char **argv;
{
float scale = 0.0;
float lower = 0.0,upper = 0.0;
float mean = 0.0,sd,var = 0.0;
float x,y;
int mode;
Connection *connection;
Projection *projection;
ElementList	*list;
char *path;
int	i;
int	status;

    mode = -1;

    initopt(argc, argv, "path [scale] -uniform low high -gaussian mean sd lower upper -exponential low high");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	/*
	** check the distribution options
	*/
	if(strcmp(G_optopt,"-uniform") == 0){
	    mode = 1;
	    lower = Atof(optargv[1]);
	    upper = Atof(optargv[2]);
	} else if(strcmp(G_optopt,"-gaussian") == 0){
	    mode = 2;
	    mean = Atof(optargv[1]);
	    sd = Atof(optargv[2]);
	    lower = Atof(optargv[3]);
	    upper = Atof(optargv[4]);
	    var = sd*sd;
	} else if(strcmp(G_optopt,"-exponential") == 0){
	    mode = 3;
	    lower = Atof(optargv[1]);
	    upper = Atof(optargv[2]);
	}
      }

    if(status < 0 || optargc > 3 || (optargc == 2 && mode == -1) ||
				    (optargc == 3 && mode != -1)){
	printoptusage(argc, argv);
	printf("\n");
	printf("Either scale or one of the command options must be given.\n");
	return(0);
    }

    path = optargv[1];

    if (optargc == 3)
      {
	mode = 0;
	scale = Atof(optargv[2]);
      }

    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	switch(mode){
	case 0:			/* constant */
	    break;
	case 1:			/* uniform */
	    scale = frandom(lower,upper);
	    break;
	case 2:			/* gaussian */
	    do {
		scale = rangauss(mean,var);
	    } while((scale < lower) || (scale > upper));
	    break;
	case 3:			/* exponential */
	    scale = -log(frandom(lower,upper));
	    break;
	}
	if(scale <= 0) scale = 0;
	projection = (Projection *)list->element[i];
	x = projection->x;
	y = projection->y;
	for(connection=projection->connection;connection;
	connection=connection->next){
	    /*
	    ** calculate delay based on radial distance multiplied
	    ** by inverse velocity
	    */
	    connection->delay =
	    sqrt( sqr(x - connection->target->x) +
		sqr(y - connection->target->y)) * scale;
	}
	printf("."); fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return(1);
}

