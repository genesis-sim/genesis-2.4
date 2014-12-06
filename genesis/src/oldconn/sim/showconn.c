static char rcsid[] = "$Id: showconn.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: showconn.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1997/07/18 03:10:04  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.1  1994/09/23 16:12:46  dhb
** Initial revision
**
*/

#include "simconn_ext.h"

#define field_is(S) (all || strcmp(field,"S") == 0 )

/*
** ShowConnectionField
**
** FUNCTION
**
**	Show one or more connectino fields.  If the field is * then all
**	connection fields will be shown.  If field is ** then the connection
**	object is shown followed by all fields.  If field is *** then just
**	the connection object description is shown.
**
** ARGUMENTS
**
**	Projection* projection	- Projection where connection lives
**	Connection* connection	- Connection with field(s) to show
**	char* field		- Name of connection field to display
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	Matt Wilson? (Originally resided in sim_show.c)
*/


void ShowConnectionField(projection,connection,field)
Projection *projection;
Connection *connection;
char	*field;
{

    if(!projection->connection_object) return;
    if(strcmp(field,"***") == 0){
	printf("connection to '%s'\n",Pathname(connection->target));
	ShowObject(projection->connection_object);
    } else
    if(strcmp(field,"**") == 0){
	ShowObject(projection->connection_object);
	printf("-----------------------------------------------------\n");
	DisplayFields(connection,projection->connection_object,
	connection,projection->connection_object->type,NULL,0);
    } else 
    if(strcmp(field,"*") == 0){
	DisplayFields(connection,projection->connection_object,
	connection,projection->connection_object->type,NULL,0);
    } else {
	if(projection->connection_object){
	    DisplayField(connection,projection->connection_object,field);
	} else {
	    FieldFormat(field,"???");
	}
    }
}

/*
** do_showconn
**
** FUNCTION
**
**	do_showconn implements the showconn command which shows connection
**	fields in projection elements.  The syntax for showconn is the same
**	as for the 1.x show command.  If the connection does not appear then
**	all connections are shown.
**
** ARGUMENTS
**
**	int argc	- Number of command arguments
**	char* argv[]	- Command arguments
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch (Modified from sim_show.c do_show())
*/

void do_showconn(argc,argv)
int	argc;
char	**argv;
{
Element 	*element = NULL;
int 		nxtarg;
char 		*field;
int		startarg;
ElementList	*list;
ElementList	*targetlist;
int		multiple;
int		j;
int		ecount;
int		start_connection;
int		end_connection;
Projection	*projection;
Connection 	*connection;
int		show_connections = 0;
char		*pathname;
int		count;
int		status;

    initopt(argc, argv, "[path[:connections]] [field] ...");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	/* no command options so should never get here */
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if (optargc < 2)
	pathname = Pathname(WorkingElement());
    else
	pathname = optargv[1];

    /*
    ** check for a connection
    */
    targetlist = NULL;
    show_connections = 
    ParseConnection(pathname,&start_connection,&end_connection,&targetlist);
    if(targetlist && targetlist->nelements < 1){
	printf("empty connection target path specification\n");
	return;
    }

    /*
    ** a missing connection spec implies show all connections.
    */
    if (show_connections < 1)
	show_connections = 3;

    nxtarg = 1;
    list = WildcardGetElement(pathname,0);

    /*
    ** if there are more than 2 arguments then assume that there is
    ** a path specification. Try opening it as an element.
    */
    if((ecount = list->nelements) > 0){
	multiple = 1;
	nxtarg++;
    } else {
	element = WorkingElement();

	ecount = 1;
	multiple = 0;
    }
    startarg = nxtarg;

    while(ecount>0){
	if(multiple){
	    element = list->element[list->nelements - ecount];
	}
	ecount--;
	nxtarg = startarg;
	if(show_connections != 4){
	    printf("\n[ %s ]\n",Pathname(element));
	}

	while(nxtarg < optargc){
	    field = optargv[nxtarg++];
	    /*
	    ** connection
	    */
	    if(!CheckClass(element,PROJECTION_ELEMENT)){
		Error();
		printf("'%s' is not a projection.\n",Pathname(element)); 
		printf("Must specify a projection to show connections\n");
		break;
	    }
	    projection = (Projection *)element;
	    if(projection->connection_object == NULL){
		Error();
		printf("invalid connection\n");
		break;
	    }
	    /*
	    ** show all connections for all projections in the tree
	    */
	    if(show_connections == 1){
		if((connection = 
		GetConnection(projection,start_connection)) == NULL){
		    Error();
		    printf("unable to find connection %d on '%s'\n",
		    start_connection,
		    Pathname(projection));
		    break;
		}
		ShowConnectionField(projection,connection,field);
	    } else 
	    if(show_connections == 2){
		count = 0;
		for(connection=projection->connection;connection;
		connection=connection->next){
		    if(count >= start_connection &&
		    count <= end_connection){
			ShowConnectionField(projection,connection,field);
		    }
		    count++;
		}
	    } else
	    if(show_connections == 3){
		for(connection=projection->connection;connection;
		connection=connection->next){
		    ShowConnectionField(projection,connection,field);
		}
	    } else
	    if(show_connections == 4){
		for(connection=projection->connection;connection;
		connection=connection->next){
		    for(j=0;j<targetlist->nelements;j++){
			/*
			** if the connection is onto an element in
			** the target list then show it
			*/
			if(connection->target ==
			(Segment *)(targetlist->element[j])){
			    printf("\nfrom [ %s ]\n",Pathname(element));
			    ShowConnectionField(projection,
			    connection,field);
			    break;
			}
		    }
		}
	    }
	}
    }
    if(targetlist){
	FreeElementList(targetlist);
    }
    FreeElementList(list);
}


