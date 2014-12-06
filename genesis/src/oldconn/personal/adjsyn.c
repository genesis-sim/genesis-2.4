static char rcsid[] = "$Id: adjsyn.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: adjsyn.c,v $
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
** Revision 1.1  1992/12/11  19:03:35  dhb
** Initial revision
**
*/

#include <math.h>
#include "per_ext.h"

/*
** this routine should not be interrupted as it temporarily modifies
** the xy coords of some elements and must finish in order to 
** restore their value
*/
void do_NormalizeSynapses(argc,argv)
int 	argc;
char 	**argv;
{
char 		*src;
char		*dst;
ElementList	*srclist;
ElementList	*dstlist;
Projection	*src_element;
int		i;
Connection	*conn;
float		*savex;
float		*savey;
int		mode;
float		lower = 0.0,upper = 0.0;
float		mean = 0.0,sd;
float		scale = 0.0;
float		var = 0.0;
int		status;

    mode = -1;

    initopt(argc, argv, "source-path target-path -scale s -uniform low high -gaussian mean sd -exponential low high");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	/*
	** check the distribution options
	*/
	if(strcmp(G_optopt,"-scale") == 0){
	    mode = 0;
	    scale = Atof(optargv[1]);
	} else
	if(strcmp(G_optopt,"-uniform") == 0){
	    mode = 1;
	    lower = Atof(optargv[1]);
	    upper = Atof(optargv[2]);
	} else
	if(strcmp(G_optopt,"-gaussian") == 0){
	    mode = 2;
	    mean = Atof(optargv[1]);
	    sd = Atof(optargv[2]);
	    var = sd*sd;
	} else 
	if(strcmp(G_optopt,"-exponential") == 0){
	    mode = 3;
	    lower = Atof(optargv[1]);
	    upper = Atof(optargv[2]);
	}
      }

    if(status < 0 || mode == -1){
	printoptusage(argc, argv);
	printf("\n");
	printf("One of the command options must be given.\n");
	return;
    }

    src = optargv[1];
    dst = optargv[2];

    srclist = WildcardGetElement(src,1);
    dstlist = WildcardGetElement(dst,1);
    savex = (float *)malloc(sizeof(float)*dstlist->nelements);
    savey = (float *)malloc(sizeof(float)*dstlist->nelements);
    ClearMarkers(RootElement());
    for(i=0;i<dstlist->nelements;i++){
	/*
	** use the x field for the synapse count
	*/
	savex[i] = dstlist->element[i]->x;
	/*
	** use the y field for the normalization factor
	*/
	savey[i] = dstlist->element[i]->y;
	/*
	** clear the synapse count
	*/
	dstlist->element[i]->x = 0;
	/*
	** set the marker used to indicate valid targets
	*/
	dstlist->element[i]->flags |= MARKERMASK;
	/*
	** calculate the desired normalization factor
	*/
	switch(mode){
	case 0:			/* constant */
	    break;
	case 1:			/* uniform */
	    scale = frandom(lower,upper);
	    break;
	case 2:			/* gaussian */
	    scale = rangauss(mean,var);
	    break;
	case 3:			/* exponential */
	    scale = -log(frandom(lower,upper));
	    break;
	}
	if(scale < 0) scale = 0;
	dstlist->element[i]->y = scale;
    }
    /*
    ** first pass to calculate total enervation
    */
    for(i=0;i<srclist->nelements;i++){
	src_element = (Projection *)srclist->element[i];
	if(!CheckClass(src_element,PROJECTION_ELEMENT)){
	    Error();
	    printf("'%s' is not a projection\n",Pathname(src_element));
	    FreeElementList(srclist);
	    FreeElementList(dstlist);
	    return;
	}
	/*
	** for each dst element go through all of the connections
	** to it and keep track of the total synaptic weight
	*/
	for(conn=src_element->connection;conn;conn=conn->next){
	    /*
	    ** check the connection target against the destination
	    ** elements
	    */
	    if(conn->target->flags & MARKERMASK){
		conn->target->x += conn->weight;
	    }
	}
    }
    /*
    ** second pass to apply normalization
    */
    for(i=0;i<srclist->nelements;i++){
	src_element = (Projection *)srclist->element[i];
	for(conn=src_element->connection;conn;conn=conn->next){
	    /*
	    ** check the connection target against the destination
	    ** elements
	    */
	    if((conn->target->flags & MARKERMASK) && (conn->target->x > 0)){
		conn->weight = conn->weight*conn->target->y/conn->target->x;
	    }
	}
    }
    /*
    ** restore the x,y fields to their original value
    */
    for(i=0;i<dstlist->nelements;i++){
	dstlist->element[i]->x = savex[i];
	dstlist->element[i]->y = savey[i];
    }
    free(savex);
    free(savey);
    FreeElementList(srclist);
    FreeElementList(dstlist);
}
