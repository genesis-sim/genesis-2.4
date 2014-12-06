static char rcsid[] = "$Id: scaleweight.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: scaleweight.c,v $
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
** Revision 1.1  1992/12/11  19:03:41  dhb
** Initial revision
**
*/

#include "per_ext.h"

/* 4/89 Matt Wilson */
int ScaleWeight(argc,argv)
int argc;
char **argv;
{
Projection 	*projection;
ElementList	*list;
char 		*path;
int		i;
register Connection *connection;
register float scale;

    initopt(argc, argv, "path scale");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    path = optargv[1];
    scale = Atof(optargv[2]);

    list = WildcardGetElement(path,1);
    for(i=0;i<list->nelements;i++){
	projection = (Projection *)list->element[i];
	/*
	** scale the weight 
	*/
	for(connection=projection->connection;connection;
	connection=connection->next){
	    connection->weight *= scale;
	}
	printf(".");
	fflush(stdout);
    }
    printf("\n");
    FreeElementList(list);
    OK();
    return(1);
}

