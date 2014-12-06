static char rcsid[] = "$Id: spatial.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: spatial.c,v $
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
** Revision 1.4  2000/06/12 05:07:03  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.3  1997/07/18 03:11:14  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/24 23:46:34  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:43  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"
/* 
** 3/89 Matt Wilson 
**
*/
int SpatialDistField(argc,argv)
int 	argc;
char 	**argv;
{
float 		max;
float 		sigma = 0.0;
float 		min = 0;
float 		x0,y0,z0;
float		lambda = 0.0;
ElementList	*list;
Element		*element;
char 		*path;
int		i;
char		val[80];
int		mode = -1;
char		*field;
int		status;

    initopt(argc, argv, "path field maxvalue minvalue x y z -gaussian sigma -exponential lambda");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	/*
	** check the distribution options
	*/
	if(strcmp(G_optopt,"-gaussian") == 0){
	    mode = 0;
	    sigma = -2*pow(Atof(optargv[1]),2.0);
	} else 
	if(strcmp(G_optopt,"-exponential") == 0){
	    mode = 1;
	    lambda = -Atof(optargv[1]);
	}
      }

    if(status < 0 || mode == -1){
	printoptusage(argc, argv);
	printf("\n");
	printf("One of the command options must be given.\n");
	return(0);
    }

    path = optargv[1];
    field = optargv[2];
    max = Atof(optargv[3]);
    min = Atof(optargv[4]);
    x0 = Atof(optargv[5]);
    y0 = Atof(optargv[6]);
    z0 = Atof(optargv[7]);

    list = WildcardGetElement(path,1);
    switch(mode){
    case 0:		/* gaussian */
	for(i=0;i<list->nelements;i++){
	    element = list->element[i];
	    /*
	    ** set the field according to the gaussian of the distance between
	    ** the specified coordinate and element 
	    */
	    sprintf(val,"%e", (max - min)*exp((pow(x0 - element->x,2.0) + 
	    pow(y0 - element->y,2.0))/sigma) + min);
	    SetElement(element,field,val);
	}
	break;
    case 1:		/* exponential */
	for(i=0;i<list->nelements;i++){
	    element = list->element[i];
	    sprintf(val,"%e", (max - min)*exp(sqrt(pow(x0-element->x,2.0)+ 
	    pow(y0 - element->y,2.0))/lambda) + min);
	    SetElement(element,field,val);
	}
	break;
    default:
	printf("invalid mode\n");
	FreeElementList(list);
	return(0);
	/* NOTREACHED */
	break;
    }
    FreeElementList(list);
    OK();
    return(1);
}
