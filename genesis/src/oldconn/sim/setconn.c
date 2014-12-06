static char rcsid[] = "$Id: setconn.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: setconn.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/06/29 21:17:55  mhucka
** Added extra parens inside conditionals and { } groupings to quiet certain
** compiler warnings.
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
** do_setconn
**
** FUNCTION
**
**	do_setconn implements the setconn command to set connection field
**	values.  The syntax is the same as for the old set command, except
**	that absence of the connection syntax implies setting all connections.
**
** ARGUMENTS
**
**	int argc	- Number of command args
**	char* argv[]	- Command args
**
** RETURN VALUE
**	int		- 0 on error, 1 on success
**
** AUTHOR
**	David Bilitch (this code is originally in sim_set.c)
*/


int do_setconn(argc,argv)
int 	argc;
char 	**argv;
{
Element	*element;
Projection	*projection;
Connection	*connection;
char 		*pathname;
int 		nxtarg;
int 		field_offset;
Info 		info;
char 		*field;
char 		*value;
int		set_connections;
int		start_connection;
int		end_connection;
ElementList	*list;
ElementList	*targetlist;
GenesisObject		*previous_object;
int		i;
int		j;
Action		action;
PFI		func;
int		count;
int		already_set;
void*		savopt();

    initopt(argc, argv, "[path[:connection]] field value ...");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    nxtarg = 1;
    set_connections = 0;
    action.argv = (char **)malloc((optargc+3)*sizeof(char *));
    action.type = SET;
    action.name = "SET";
    action.argc = 2;
    /*
    ** check for a pathname
    */
    if(((optargc - nxtarg)%2) != 0){
	pathname = optargv[nxtarg];
	nxtarg++;
    } else {
	pathname = ".";
    }
    /*
    ** check for a connection
    */
    targetlist = NULL;
    set_connections =
    ParseConnection(pathname,&start_connection,&end_connection,&targetlist);
    if(targetlist && targetlist->nelements < 1){
	InvalidPath(optargv[0],pathname);
	printf("empty connection target path specification\n");
	return(0);
    }

    /*
    ** If no connection spec then do all connections
    */
    if (set_connections == 0)
	set_connections = 3;

    if((list = WildcardGetElement(pathname,0)) == NULL){
	InvalidPath(optargv[0],pathname);
	return(0);
    }
    if(list->nelements < 1){
	InvalidPath(optargv[0],pathname);
	return(0);
    }
    /*
    ** loop over all the fields to be set
    */
    while(nxtarg < optargc){
	/*
	** get the field to be set
	*/
	field = optargv[nxtarg++];
	if(nxtarg == optargc){
	    TraceScript();
	    printf("missing a value for field '%s'\n",field);
	    return(0);
	}
	/*
	** and its value
	*/
	value = optargv[nxtarg++];
	previous_object = NULL;
	/*
	** loop over all the elements in the list
	*/
	for(i=0;i<list->nelements;i++){
	    /*
	    ** get the element
	    */
	    if((element = list->element[i]) == NULL) continue;

	    /*
	    ** connection
	    */
	    if(!CheckClass(element,PROJECTION_ELEMENT)){
		Error();
		printf("'%s' is not a projection.\n",Pathname(element)); 
		printf("Must specify a projection to set connections\n");
		return(0);
	    }
	    projection = (Projection *)element;
	    if(projection->connection_object == NULL){
		Error();
		printf("invalid connection\n");
		return(0);
	    }

	    /*
	    ** if there is a SET action then use it
	    **
	    ** NOTE: I think this is an error.  The projections SET action
	    **       is used to call a SET action on the connection.  I'll
	    **       leave it as is for now.  ---dhb
	    */
	    already_set = 0;
	    if ((func = GetActionFunc(projection->object,SET,NULL,NULL))) {
		action.argv[0] = field;
		action.argv[1] = value;
		/*
		** set all connections for all projections in the tree
		*/
		if(set_connections == 1){
		    if((connection = GetConnection(element,start_connection)) ==
		    NULL){
			Error();
			printf("could not find connection %d on '%s'\n",
			start_connection,
			Pathname(element));
			return(0);
		    }
		    already_set = func(connection,&action);
		} else 
		if(set_connections == 2){
		    count = 0;
		    for(connection=projection->connection;connection;
		    connection=connection->next){
			if(count >= start_connection &&
			count <= end_connection){
			    if(!(already_set = func(connection,&action))){
				break;
			    }
			}
			count++;
		    }
		} else
		if(set_connections == 3){
		    /*
		    ** all connections
		    */
		    for(connection=projection->connection;connection;
		    connection=connection->next){
			if(!(already_set = func(connection,&action))){
			    break;
			}
		    }
		} else
		if(set_connections == 4){
		    /*
		    ** target list specification
		    */
		    for(connection=projection->connection;connection;
		    connection=connection->next){
			for(j=0;j<targetlist->nelements;j++){
			    /*
			    ** if the connection is onto an element in
			    ** the target list then show it
			    */
			    if(connection->target ==
			    (Segment *)(targetlist->element[j])){
				already_set = func(connection,&action);
				break;
			    }
			}
		    }
		} 
	    }
	    /*
	    ** do the default set
	    */
	    if(!already_set){
		if((field_offset = CalculateOffset(
		projection->connection_object->type,field,&info)) == -1){
		    Error();
		    printf("%s: could find field '%s'\n",optargv[0],field);
		    return(0);
		}
		/*
		** set all connections for all projections in the tree
		*/
		if(set_connections == 1){
		    if((connection = GetConnection(element,start_connection)) ==
		    NULL){
			Error();
			printf("could not find connection %d on '%s'\n",
			start_connection,
			Pathname(element));
			return(0);
		    }
		    PutValue((char *)connection + field_offset, &info,value);
		} else 
		if(set_connections == 2){
		    count = 0;
		    for(connection=projection->connection;connection;
		    connection=connection->next){
			if(count >= start_connection &&
			count <= end_connection){
			    PutValue((char *)connection
			    +field_offset, &info,value);
			}
			count++;
		    }
		} else
		if(set_connections == 3){
		    /*
		    ** all connections
		    */
		    for(connection=projection->connection;connection;
		    connection=connection->next){
			PutValue((char *)connection +field_offset, &info,value);
		    }
		} else
		if(set_connections == 4){
		    /*
		    ** target list specification
		    */
		    for(connection=projection->connection;connection;
		    connection=connection->next){
			for(j=0;j<targetlist->nelements;j++){
			    /*
			    ** if the connection is onto an element in
			    ** the target list then show it
			    */
			    if(connection->target ==
			    (Segment *)(targetlist->element[j])){
				PutValue((char *)connection 
				+field_offset, &info,value);
				break;
			    }
			}
		    }
		}
	    }
	}
    }
    /*
    ** done with the list
    */
    if(targetlist){
	FreeElementList(targetlist);
    }
    FreeElementList(list);
    free(action.argv);
    OK();
    return(1);
}

/*
** SetConnection
**
** FUNCTION
**
**	Sets a connection field value.
**
** ARGUMENTS
**
**	Projection* projection	- Projection where connection lives
**	Connection* connection	- Connection to set
**	char* field		- Field name of connection field to set
**	char* value		- New value for the connection field
**
** RETURN VALUE
**	int	- 0 on error, 1 on success
**
** NOTES
**
**	I don't know if this is used.
**
** AUTHOR
**	Matt Wilson? (This was originally in sim_set.c)
*/


int SetConnection(projection,connection,field,value)
Projection 	*projection;
Connection 	*connection;
char 		*field;
char 		*value;
{
    if(connection != NULL){
	if(field[0] == '.') field++;
	SetField(connection,"connection_type",field,value);
    } else {
	TraceScript();
	printf("connection is undefined\n");
	return(0);
    }
    return(1);
}

