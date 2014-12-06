static char rcsid[] = "$Id: getconn.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: getconn.c,v $
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

/*
** GetConnectionField
**
** FUNCTION
**
**	Return the value of a connection field.
**
** ARGUMENTS
**	Projection* projection	- Projection where connection lives
**	Connection* connection	- Connection to get field value from
**	char* field		- Field name of connection field to get
**
** RETURN VALUE
**	char*			- "???" on error, field value on success
**
** NOTES
**
**	The return value is a copy of the field in malloced memory.
**
** AUTHOR
**	Matt Wilson? (This code originally in sim_get.c)
*/


char *GetConnectionField(projection,connection,field)
Projection *projection;
Connection *connection;
char	*field;
{
char *str;

    if((str = FieldValue(connection,projection->connection_object,field)) == NULL){
	Error();
	printf("could not find field '%s'\n",field);
	str = "???";
    }
    return(CopyString(str));
}


/*
** do_getconn
**
** FUNCTION
**
**	do_getconn implements the getconn command which returns the value
**	of fields in connections.  getconn takes the same arguments is the
**	old set syntax.
**
** ARGUMENTS
**
**	int argc	- Number of command arguments
**	char* argv[]	- Command arguments
**
** RETURN VALUE
**	char*		- NULL on error, field value on success, "???" if
**			  no such field
**
** NOTES
**
**	Returns the field value in malloced memory (allocated in 
**	GetConnectionField()).
**
** AUTHOR
**	David Bilitch
*/


char *do_getconn(argc,argv)
     int argc;
     char **argv;
{
  char 		*pathname;
  char 		*value = NULL;
  char 		*field;
  Element 	*element;
  int		get_connections = 0;
  ElementList	*targetlist;
  int		start_connection,end_connection;
  Projection	*projection;
  Connection	*connection;

  initopt(argc, argv, "path:connection field");
  if (G_getopt(argc, argv) != 0 || optargc > 3)
    {
      printoptusage(argc, argv);
      return(NULL);
    }

    pathname = optargv[1];
    field = optargv[2];

    /*
    ** check for a connection
    */
    targetlist = NULL;
    get_connections = 
    ParseConnection(pathname,&start_connection,&end_connection,&targetlist);
    if (get_connections < 1) {
	Error();
	printf("Must give a connection specification\n");
	return(NULL);
    }
    if(targetlist && targetlist->nelements < 1){
	Error();
	printf("empty connection target path specification\n");
	return(NULL);
    }
    if(targetlist && targetlist->nelements > 1){
	Error();
	printf("cannot get multiple fields\n");
	return(NULL);
    }
    if((element = GetElement(pathname)) ==NULL){
	Error();
	printf("could not find '%s'\n",pathname);
	return(NULL);
    }
    /*
    ** connection
    */
    if(!CheckClass(element,PROJECTION_ELEMENT)){
	Error();
	printf("'%s' is not a projection.\n",Pathname(element)); 
	printf("Must specify a projection to set connections\n");
	return(NULL);
    }
    projection = (Projection *)element;
    if(projection->connection_object == NULL){
	Error();
	printf("invalid connection from '%s'\n",Pathname(projection));
	return(NULL);
    }
    if(get_connections == 1){
	if((connection = 
	GetConnection(projection,start_connection)) == NULL){
	    Error();
	    printf("unable to find connection %d on '%s'\n",
	    start_connection,
	    Pathname(projection));
	    return(NULL);
	}
	value = GetConnectionField(projection,connection,field);
    } else 
    if(get_connections == 2){
	Error();
	printf("cannot get multiple fields\n");
	return(NULL);
    } else
    if(get_connections == 3){
	Error();
	printf("cannot get multiple fields\n");
	return(NULL);
    } else
    if(get_connections == 4){
	for(connection=projection->connection;connection;
	connection=connection->next){
	    if(connection->target ==
	    (Segment *)(targetlist->element[0])){
		value = GetConnectionField(projection, connection,field);
		break;
	    }
	}
    }
    if(targetlist){
	FreeElementList(targetlist);
    }
    return(value);
}
