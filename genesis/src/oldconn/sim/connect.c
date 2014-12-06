static char rcsid[] = "$Id: connect.c,v 1.2 2005/07/01 10:03:05 svitak Exp $";

/*
** $Log: connect.c,v $
** Revision 1.2  2005/07/01 10:03:05  svitak
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
 * Revision 1.4  1994/03/21  00:04:14  dhb
 * Changed do_region_connect() and do_connection_status() back to their
 * 1.4.x format.  New formats can be compiled by defining the
 * NEW_REGIONCONNECT_OPTIONS and NEW_CONNECTION_STATUS defines
 * respectivly.  NOTE: don't forget to change the command names in
 * simlib.g!!!!!
 *
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/02/16  19:49:30  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_add_connection (connect), do_connection_status (cstat/showconn) and
 * 	do_region_connect (region_connect) changed to use GENESIS getopt
 * 	rouines.
 *
 * 	do_connection_status split into two functions for getconn and showconn
 * 	commands.
 *
 * 	do_region_connect command arguments for probability of connection and
 * 	source and destination masks moved to command options "-probability p"
 * 	"-sourcemask xl yl xh yh", "-sourcehole xl yl xh yh", etc.
 *
 * Revision 1.1  1992/10/27  19:44:37  dhb
 * Initial revision
 *
*/

#include "simconn_ext.h"

GenesisObject *CheckConnectionObject(projection,connection_object)
Projection	*projection;
GenesisObject		*connection_object;
{
    /*
    ** does the projection already have a connection object defined?
    */
    if(projection->connection_object){
	/*
	** if it does then check against the user-specified object
	*/
	if(connection_object &&
	projection->connection_object != connection_object){
	    Error();
	    printf("projection '%s' already has connections of '%s' type\n",
	    projection->name,
	    projection->connection_object->name);
	    return(0);
	}
    } else {
	/*
	** if it is null then just assign it to the specified type 
	*/
	projection->connection_object = connection_object;
    }
    return(projection->connection_object);
}

/*
** creates a new connection structure and adds it to the src_projection
** returns the new connection
*/
Connection *AddConnection(src_projection,target,object,action_func)
Projection 	*src_projection;
Segment		*target;
GenesisObject		*object;
PFI		action_func;
{
Connection 	*new_connection;
static Action create_action = { "CREATE", CREATE, NULL, 0, NULL, NULL };

    if(src_projection == NULL){
	return(NULL);
    }	
    /*
    ** create a new connection
    */
    new_connection = (Connection *) scalloc(1, object->size);
    /*
    ** add the new connection to the beginning of the connection list
    ** this is faster than adding to the end
    */
    new_connection->next = src_projection->connection;
    src_projection->connection = new_connection;
    /*
    ** set the connection parameters
    */
    new_connection->target = target;
    /*
    ** check to see if the  object has a create action
    */
    if(action_func){
	if(!action_func(new_connection,&create_action,src_projection,target)){
	    return(NULL);
	}
    }
    return(new_connection);
}

Connection *FindConnectionToSegment(src_projection,target)
Projection *src_projection;
Segment *target;
{
Connection *conn;

    if(src_projection == NULL || target == NULL){
	return(NULL);
    }
    /*
    ** find the connection based on its destination
    */
    for(conn=src_projection->connection;conn;conn=conn->next){
	if(conn->target == target){
	    return(conn);
	}
    }
    return(NULL);
}

int DeleteConnection(src_projection,target_connection)
Projection *src_projection;
Connection *target_connection;
{
Connection *connection;
Connection *delete_connection;

    if(src_projection == NULL || target_connection == NULL)
	return(0);
    /*
    ** find the connection to be deleted
    */
    for(connection = src_projection->connection;
    (connection && !(connection->next == target_connection)
    && !(connection == target_connection));
    connection=connection->next);
    /*
    ** couldnt find any connection which matched
    */
    if(connection == NULL){
	/*
	** failed
	*/
	return(0);
    }
    /*
    ** if there is only one connection then this condition
    ** could occur
    */
    if(connection == target_connection){
	delete_connection = connection;
	/*
	** delete the connection
	*/
	src_projection->connection = delete_connection->next;
	/*
	** free the memory used
	*/
	sfree(delete_connection);
	/*
	** success
	*/
	return(1);
    } else 
    if(connection->next == target_connection){
	/*
	** otherwise remove the connection
	*/
	delete_connection = connection->next;
	/*
	** point to the connection following the deleted connection
	*/
	connection->next = delete_connection->next;
	/*
	** free the memory taken by the connection
	*/
	sfree(delete_connection);
	/*
	** success
	*/
	return(1);
    } else {
	/*
	** failed
	*/
	return(0);
    }
}


/*
** returns the ith connection from the projection
*/
Connection *GetConnection(projection,index)
Projection 	*projection;
int 		index;
{
Connection 	*connection;
int 		i;

    if(projection == NULL) return(NULL);
    for(i=0,connection=projection->connection;
    i<index && connection; i++,connection=connection->next)
	;
    if(i == index){
	return(connection);
    } else {
	return(NULL);
    }
}

void RegionConnect(projection_path,src_region,src_nregions,
	    segment_path,dst_region,dst_nregions,
	    connection_object_name,
	    pconnect,
	    relative)
char 			*projection_path;
struct mask_type 	*src_region;
int 			src_nregions;
char 			*segment_path;
struct mask_type 	*dst_region;
int 			dst_nregions;
char			*connection_object_name;
float			pconnect;
int			relative;
{
Projection 		*projection;
Segment 		*segment;
Connection 		*connection;
ElementList		*slist,*plist;
float 			srcx,srcy;
GenesisObject			*connection_object;
int			i,j;
PFI			action_func;
float 			x,y;
register struct mask_type *ptr;
int			val;

    connection_object = NULL;
    /*
    ** if the user specified a connection object then try to get it
    */
    if(connection_object_name){
	if((connection_object = GetObject(connection_object_name)) == NULL){
	    Error();
	    printf("could not find object '%s'\n",connection_object_name);
	    return;
	}
    }
    /*
    ** get the element list for the projections
    */
    plist = WildcardGetElement(projection_path,0);
    /*
    ** use a separate list for the segments
    */
    slist = WildcardGetElement(segment_path,0);
    /*
    ** go through all of the projections
    */
    for(i=0;i<plist->nelements;i++){
	projection = (Projection *)plist->element[i];
	/*
	** make sure its a projection
	*/
	if(!CheckClass(projection,PROJECTION_ELEMENT)){
	    continue;
	}
	/*
	** is it within the bounding regions
	*/
	if(!IsElementWithinRegion(projection,src_region,src_nregions)){
	    continue;
	};
	/*
	** assign and/or check the connection object of the projection
	*/
	if(!CheckConnectionObject(projection,connection_object)){
	    Error();
	    printf("must define a connection object\n");
	    return;
	}
	/*
	** get the CREATE action func for the connection
	*/
	action_func = GetActionFunc(connection_object,CREATE,NULL,NULL);
	/*
	** get the location of the projection
	*/
	srcx = projection->x;
	srcy = projection->y;
	/*
	** go through all of the children of the dst_element
	** to get the destination segments
	*/
	for(j=0;j<slist->nelements;j++){
	    segment = (Segment *)slist->element[j];
	    /*
	    ** make sure it is a segment
	    */
	    if(!CheckClass(segment,SEGMENT_ELEMENT)){
		continue;
	    }
	    /*
	    ** is it within the bounding regions
	    */
	    if(relative){
		/*
		if(!IsElementWithinRegionRel(segment,dst_region,
		dst_nregions,srcx,srcy)){
		    continue;
		}
		*/

		/*
		** if no masks are specified then assume that is 
		** within the region
		*/
		if(dst_nregions != 0){
		    /*
		    ** search the mask list from the bottom up
		    ** if the element is bounded by a mask then
		    ** it will possess that sign and no further search is 
		    ** necessary
		    */
		    x = segment->x - srcx;
		    y = segment->y - srcy;
		    ptr = dst_region + dst_nregions-1;
		    val = 0;
		    do{
			if( 
			x >= ptr->xl && 
			x <= ptr->xh &&
			y >= ptr->yl && 
			y <= ptr->yh){
			    val = (ptr->type == 1);
			    break;
			}
		    } while(ptr-- != dst_region);
		    /*
		    ** if not in the region then skip it
		    */
		    if(!val) continue;
		}
	    } else {
		if(!IsElementWithinRegion(segment,dst_region,dst_nregions)){
		    continue;
		}
	    }
	    /*
	    ** make the connection
	    */
	    if(pconnect >= 1 || urandom() <= pconnect){
		if((connection = AddConnection(projection,
		segment,connection_object,action_func)) == NULL){
		    printf("unable to make connection from '%s' to '%s'\n",
		    Pathname(projection),
		    Pathname(segment));
		} 
	    }
	}
	if (IsSilent() < 2) {
		printf(".");
		fflush(stdout);
	}
    }
	if (IsSilent() < 2)
    	printf("\n");
    /*
    ** free up the element lists
    */
    FreeElementList(slist);
    FreeElementList(plist);
}

/*
** CONNECTION COMMANDS FOR USE IN THE INTERPRETER
*/

void do_add_connection(argc,argv)
int argc;
char **argv;
{
char		*segment_name;
Projection 	*projection;
Segment 	*segment;
Connection 	*connection;
char		*connection_object_name;
char		*projection_name;
GenesisObject		*connection_object;
int		nxtarg;
short           projectionfound         = 0;
short           segmentfound            = 0;
short           connectionfound         = 0;
PFI             action_func;
 
    if(argc < 4){
        printf("usage: %s projection [to] segment [[with] connection_object]\n",
        argv[0]);
        return;
    }
    projection_name = NULL;
    segment_name = NULL;
    connection_object_name = NULL;

    /*
    ** parse the command line arguments
    */
    nxtarg = 0;
    while(++nxtarg<argc){
        if(!projectionfound){
            projection_name = argv[nxtarg];
            projectionfound = 1;
            continue;
        } else
        if(!segmentfound){
            if(strcmp(argv[nxtarg],"to")==0){
                continue;
            }
            segment_name = argv[nxtarg];
            segmentfound = 1;
            continue;
        } else
        if(!connectionfound){
            if(strcmp(argv[nxtarg],"with")==0){
                continue;
            }
            connection_object_name = argv[nxtarg];
            connectionfound = 1;
            continue;
        }
    }
    if(!projection_name || !segment_name){
        Error();
        printf("must specify source and destination pathnames\n");
        return;
    }

    /*
    ** get the source projection
    */
    if((projection=(Projection *)GetElement(projection_name))==NULL){
	Error();
	printf("cannot find projection %s\n",projection_name);
	return;
    }
    if(!CheckClass(projection,PROJECTION_ELEMENT)){
	Error();
	printf("'%s' is not a projection\n",projection_name);
	return;
    }
    /*
    ** get the destination segment
    */
    if((segment = (Segment *)GetElement(segment_name)) == NULL){
	Error();
	printf("cannot find segment %s\n",segment_name);
	return;
    }
    if(!CheckClass(segment,SEGMENT_ELEMENT)){
	Error();
	printf("'%s' is not a segment\n",segment_name);
	return;
    }
    connection_object = NULL;
    /*
    ** if the user specified a connection object then try to get it
    */
    if(connection_object_name){
	if((connection_object = GetObject(connection_object_name)) == NULL){
	    Error();
	    printf("could not find object '%s'\n",connection_object_name);
	    return;
	}
    }
    if(!CheckConnectionObject(projection,connection_object)){
	Error();
	printf("must define a connection object\n");
	return;
    }
    action_func = GetActionFunc(connection_object,CREATE,NULL,NULL);
    if((connection = AddConnection(projection,segment,
    connection_object,action_func)) == NULL){
	printf("unable to make connection from '%s' to '%s'\n",
	Pathname(projection),
	Pathname(segment));
    } else {
	/*
	** set the current variables to the newly created connection
	*/
	SetRecentConnection(connection);
    }
    OK();
}

void do_delete_connection(argc,argv)
int 	argc;
char 	**argv;
{
Projection 	*src_projection;
Segment 	*target;
Connection 	*connection;

    initopt(argc, argv, "projection-element segment-element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    src_projection = (Projection *)GetElement(optargv[1]);
    target = (Segment *)GetElement(optargv[2]);
    if((connection=FindConnectionToSegment(src_projection,target))==NULL){
	printf("cant find connection from '%s' to '%s'\n",
	Pathname(src_projection),
	Pathname(target));
	return;
    } else {
	if(!DeleteConnection(src_projection,connection)){
	    printf("unable to delete connection\n");
	    return;
	}
    }
    OK();
}

/*
** connects the elements of one tree to another
** using specified source projections and destination segments
** and the specified source and destination masks
** The destination mask is given relative to the source element location
*/
#ifdef NEW_REGIONCONNECT_OPTIONS
void do_region_connect(argc,argv)
int argc;
char **argv;
{
char 			*projection_name;
char 			*segment_name;
char			*connection_object_name;
int 			src_count,dst_count;
struct mask_type 	*src_mask, *dst_mask;
int 			i;
int 			nxtarg;
float			pconnect = 1;
int			relative = 0;
char			*type;
int			status;

    src_mask = NULL;
    dst_mask = NULL;
    src_count = 0;
    dst_count = 0;

    initopt(argc, argv, "source-path dest-path connection-object -relative -sourcemask xl yl xh yh -sourcehole xl yl xh yh -destmask xl yl xh yh -desthole xl yl xh yh -probability p");

    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-relative") == 0)
	    relative = 1;
	else if (strcmp(G_optopt, "-sourcemask") == 0)
	    src_count++;
	else if (strcmp(G_optopt, "-sourcehole") == 0)
	    src_count++;
	else if (strcmp(G_optopt, "-destmask") == 0)
	    dst_count++;
	else if (strcmp(G_optopt, "-desthole") == 0)
	    dst_count++;
	else if (strcmp(G_optopt, "-probability") == 0)
	    pconnect = Atof(optargv[1]);
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if (src_count < 1 || dst_count < 1)
      {
	Error();
	printf("Must have at least one source and one destination mask\n");
	return;
      }

    src_mask = (struct mask_type*)
		malloc(src_count*sizeof(struct mask_type));
    if (src_mask == NULL)
      {
	Error();
	perror(argv[0]);
	return;
      }

    dst_mask = (struct mask_type*)
		malloc(src_count*sizeof(struct mask_type));
    if (dst_mask == NULL)
      {
	Error();
	perror(argv[0]);
	free(src_mask);
	return;
      }

    /*
    ** set up the source and destination masks
    */
    initopt(argc, argv, "source-path dest-path connection-object -relative -sourcemask xl yl xh yh -sourcehole xl yl xh yh -destmask xl yl xh yh -desthole xl yl xh yh -probability p");

    src_count = 0;
    dst_count = 0;
    while (G_getopt(argc, argv) == 1)
      {
	struct mask_type*	mask;

	mask = NULL;
	if (strcmp(G_optopt, "-sourcemask") == 0)
	  {
	    src_mask[src_count].type = 1;
	    mask = src_mask + src_count;
	    src_count++;
	  }
	else if (strcmp(G_optopt, "-destmask") == 0)
	  {
	    dst_mask[dst_count].type = 1;
	    mask = dst_mask + dst_count;
	    dst_count++;
	  }
	else if (strcmp(G_optopt, "-sourcehole") == 0)
	  {
	    src_mask[src_count].type = -1;
	    mask = src_mask + src_count;
	    src_count++;
	  }
	else if (strcmp(G_optopt, "-desthole") == 0)
	  {
	    dst_mask[dst_count].type = -1;
	    mask = dst_mask + dst_count;
	    dst_count++;
	  }

	if (mask != NULL)
	  {
	    mask->xl = Atof(optargv[1]); 
	    mask->yl = Atof(optargv[2]); 
	    mask->xh = Atof(optargv[3]); 
	    mask->yh = Atof(optargv[4]); 
	  }
      }

    projection_name = optargv[1];
    segment_name = optargv[2];
    connection_object_name = optargv[3];

    RegionConnect(projection_name,src_mask,src_count, 
		    segment_name,dst_mask,dst_count,
		    connection_object_name,pconnect,relative);

    free(src_mask);
    free(dst_mask);
    OK();
}
#else /* NEW_REGIONCONNECT_OPTIONS */
void do_region_connect(argc,argv)
int argc;
char **argv;
{
char 			*projection_name;
char 			*segment_name;
char			*connection_object_name;
int 			src_count,dst_count;
struct mask_type 	*src_mask,*dst_mask;
int 			nxtarg;
float			pconnect = 1;
int			relative = 0;

    if(argc < 4){
	printf("usage: %s src_path [to] dst_path [[with] connection_object]\n",
	argv[0]);
	printf("       [-rel]\n");
	printf("       nsrc_masks type xl yl xh yh ...\n");
	printf("       ndst_masks type xl yl xh yh ...\n");
	printf("       [probability of connection]\n");
	return;
    }
    /*
    ** PARSE THE COMMAND LINE
    */
    projection_name = argv[1];
    connection_object_name = NULL;
    nxtarg = 1;
    if(strcmp(argv[++nxtarg],"to") == 0){
	segment_name  = argv[++nxtarg];
    } else {
	segment_name  = argv[nxtarg];
    }
    if(strcmp(argv[++nxtarg],"with") == 0){
	connection_object_name  = argv[++nxtarg];
    } else {
	connection_object_name  = argv[nxtarg];
    }
    /*
    ** the remaining arguments give the mask specifications
    */
    nxtarg++;
    if(strcmp(argv[nxtarg],"-rel") == 0){
	relative = 1;
	nxtarg++;
    }
    /*
    ** set up the src masks
    */
    src_mask = GetMaskFromArgv(&src_count,&nxtarg,argc,argv);
    /*
    src_count = atoi(argv[nxtarg++]);
    if(argc < nxtarg + src_count){
	Error();
	printf("bad src region specification\n");
	return;
    }
    src_mask = (struct mask_type *)
    malloc(src_count * sizeof(struct mask_type));
    for(i=0;i<src_count;i++){
	type = argv[nxtarg++];
	if(strcmp(type,"POS")==0){
	    src_mask[i].type = 1;
	} else 
	if(strcmp(type,"NEG")==0){
	    src_mask[i].type = -1;
	} else {
	    src_mask[i].type = atoi(type); 
	}
	src_mask[i].xl = Atof(argv[nxtarg++]); 
	src_mask[i].yl = Atof(argv[nxtarg++]); 
	src_mask[i].xh = Atof(argv[nxtarg++]); 
	src_mask[i].yh = Atof(argv[nxtarg++]); 
    }
    */
    /*
    ** set up the dst masks
    */
    dst_mask = GetMaskFromArgv(&dst_count,&nxtarg,argc,argv);
    /*
    dst_count = atoi(argv[nxtarg++]);
    if(argc < nxtarg + dst_count){
	Error();
	printf("bad dst region specification\n");
	return;
    }
    dst_mask = (struct mask_type *)
    malloc(dst_count * sizeof(struct mask_type));
    for(i=0;i<dst_count;i++){
	dst_mask[i].type = atoi(argv[nxtarg++]); 
	dst_mask[i].xl = Atof(argv[nxtarg++]); 
	dst_mask[i].yl = Atof(argv[nxtarg++]); 
	dst_mask[i].xh = Atof(argv[nxtarg++]); 
	dst_mask[i].yh = Atof(argv[nxtarg++]); 
    }
    */
    /*
    ** if there is an argument left then use it as the probability
    ** of connection
    */
    if(argc > nxtarg){
	pconnect = Atof(argv[nxtarg]);
    }
    RegionConnect(projection_name,src_mask,src_count, 
    segment_name,dst_mask,dst_count,
    connection_object_name,pconnect,relative);
    free(src_mask);
    free(dst_mask);
    OK();
}
#endif /* NEW_REGIONCONNECT_OPTIONS */

/*
** generate useful information about connection lists
*/
#ifdef NEW_CONNECTION_STATUS
do_connection_status(argc,argv)
int 	argc;
char 	**argv;
{
ElementList 	*srclist;
ElementList 	*dstlist;
Projection 	*projection;
Connection 	*conn;
char		*srcpath;
char		*dstpath;
int		anydst = 0;
float		totalweight=0;
int		i,j;
int		dstcount = 0;
int		srccount = 0;
int		conncount = 0;
float 		sxmin,symin,szmin;
float 		sxmax,symax,szmax;
float 		txmin,tymin,tzmin;
float 		txmax,tymax,tzmax;
float 		mindelay,maxdelay;
float 		minweight,maxweight;
int 		srctouched;
int		display = 1;
int		nxtarg;
int		havesrc;

    initopt(argc, argv, "source-path [dest-path]");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return;
      }

    srcpath = optargv[1];
    dstpath = optargc == 3 ? optargv[2] : NULL;

    srclist = WildcardGetElement(srcpath,1);
    if(srclist->nelements == 0){
	Error();
	printf("invalid srcpath '%s'\n",srcpath);
	FreeElementList(srclist);
	return(0);
    }

    if(dstpath){
	dstlist = WildcardGetElement(dstpath,1);
	if(dstlist->nelements == 0){
	    Error();
	    printf("invalid dstpath '%s'\n",dstpath);
	    FreeElementList(srclist);
	    FreeElementList(dstlist);
	    return(0);
	}
    } else {
	anydst = 1;
	dstlist = NULL;
    }
    ClearMarkers(RootElement());
    for(i=0;i<srclist->nelements;i++){
	projection = (Projection *)srclist->element[i];
	if(!CheckClass(projection,PROJECTION_ELEMENT)){
	    continue;
	}
	/*
	** bounding region
	*/
	if(srccount > 0){
	    sxmax = MAX(sxmax,projection->x);
	    symax = MAX(symax,projection->y);
	    szmax = MAX(szmax,projection->z);

	    sxmin = MIN(sxmin,projection->x);
	    symin = MIN(symin,projection->y);
	    szmin = MIN(szmin,projection->z);
	} else {
	    sxmax = sxmin = projection->x;
	    symax = symin = projection->y;
	    szmax = szmin = projection->z;
	}
	srctouched = 0;
	/*
	** go through all connections
	*/
	for(conn = projection->connection;conn;conn=conn->next){
	    if(!anydst){
		/*
		** check to see whether the connection dst matches
		** anything in the specified targetlist
		*/
		for(j=0;j<dstlist->nelements;j++){
		    if(conn->target == (Segment *)dstlist->element[j]){
			break;
		    }
		}
		if(j >= dstlist->nelements){
		    /*
		    ** no match
		    */
		    continue;
		}
	    }
	    srctouched = 1;
	    /*
	    ** mark it as being accessed for later tallying
	    */
	    conn->target->flags |= MARKERMASK;
	    /*
	    ** establish the parameter ranges
	    */
	    if(conncount > 0){
		/*
		** max
		*/
		if(conn->target->x > txmax) 
		    txmax = conn->target->x;
		if(conn->target->y > tymax) 
		    tymax = conn->target->y;
		if(conn->target->z > tzmax) 
		    tzmax = conn->target->z;
		if(conn->delay > maxdelay) 
		    maxdelay = conn->delay;
		if(conn->weight > maxweight) 
		    maxweight = conn->weight;

		/*
		** min
		*/
		if(conn->target->x < txmin) 
		    txmin = conn->target->x;
		if(conn->target->y < tymin) 
		    tymin = conn->target->y;
		if(conn->target->y < tzmin) 
		    tzmin = conn->target->z;
		if(conn->delay < mindelay) 
		    mindelay = conn->delay;
		if(conn->weight < minweight) 
		    minweight = conn->weight;
	    } else {
		txmin = txmax = conn->target->x;
		tymin = tymax = conn->target->y;
		tzmin = tzmax = conn->target->z;
		mindelay = maxdelay = conn->delay;
		minweight = maxweight = conn->weight;
	    }
	    /*
	    ** sum the weights
	    */
	    totalweight += conn->weight;
	    /*
	    ** count the connections
	    */
	    conncount++;
	}
	srccount += srctouched;
    }
    dstcount = CountMarkers(RootElement());
    if(display){
	/*
	** print the results
	*/
	printf("\nStatus of connections from '%s'",srcpath);
	if(!anydst){
	    printf(" to '%s'",dstpath);
	} 
	printf("\n-------------------------------------------------------\n");
	printf("%d source(s)\n",srccount);
	printf("%d destination(s)\n",dstcount);
	printf("%d connections\n",conncount);
	if(conncount > 0){
	    printf("bounding src region = ( %g , %g , %g ) ( %g , %g , %g )\n",
	    sxmin,symin,szmin,sxmax,symax,szmax);
	    printf("bounding dst region = ( %g , %g , %g ) ( %g , %g , %g )\n",
	    txmin,tymin,tzmin,txmax,tymax,tzmax);
	    printf("weight range   = ( %g to %g)\n",minweight,maxweight);
	    printf("total weight   = ( %g )\n",totalweight);
	    printf("delay range    = ( %g to %g)\n",mindelay,maxdelay);
	}
	printf("\n");
    }
    FreeElementList(srclist);
    FreeElementList(dstlist);
}


float do_get_connection_status(argc,argv)
int 	argc;
char 	**argv;
{
ElementList 	*srclist;
ElementList 	*dstlist;
Projection 	*projection;
Connection 	*conn;
char		*srcpath;
char		*dstpath;
int		anydst = 0;
float		totalweight=0;
int		i,j;
int		dstcount = 0;
int		srccount = 0;
int		conncount = 0;
float 		sxmin,symin,szmin;
float 		sxmax,symax,szmax;
float 		txmin,tymin,tzmin;
float 		txmax,tymax,tzmax;
float 		mindelay,maxdelay;
float 		minweight,maxweight;
int 		srctouched;
int		returncount = 0;
int		status;

    initopt(argc, argv, "source-path [dest-path] -count");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-count") == 0)
	    returncount = 1;
      }

    if (status < 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    srcpath = optargv[1];
    dstpath = optargc == 3 ? optargv[2] : NULL;

    srclist = WildcardGetElement(srcpath,1);
    if(srclist->nelements == 0){
	Error();
	printf("invalid srcpath '%s'\n",srcpath);
	FreeElementList(srclist);
	return(0.0);
    }

    if(dstpath){
	dstlist = WildcardGetElement(dstpath,1);
	if(dstlist->nelements == 0){
	    Error();
	    printf("invalid dstpath '%s'\n",dstpath);
	    FreeElementList(srclist);
	    FreeElementList(dstlist);
	    return(0.0);
	}
    } else {
	anydst = 1;
	dstlist = NULL;
    }
    ClearMarkers(RootElement());
    for(i=0;i<srclist->nelements;i++){
	projection = (Projection *)srclist->element[i];
	if(!CheckClass(projection,PROJECTION_ELEMENT)){
	    continue;
	}
	/*
	** bounding region
	*/
	if(srccount > 0){
	    sxmax = MAX(sxmax,projection->x);
	    symax = MAX(symax,projection->y);
	    szmax = MAX(szmax,projection->z);

	    sxmin = MIN(sxmin,projection->x);
	    symin = MIN(symin,projection->y);
	    szmin = MIN(szmin,projection->z);
	} else {
	    sxmax = sxmin = projection->x;
	    symax = symin = projection->y;
	    szmax = szmin = projection->z;
	}
	srctouched = 0;
	/*
	** go through all connections
	*/
	for(conn = projection->connection;conn;conn=conn->next){
	    if(!anydst){
		/*
		** check to see whether the connection dst matches
		** anything in the specified targetlist
		*/
		for(j=0;j<dstlist->nelements;j++){
		    if(conn->target == (Segment *)dstlist->element[j]){
			break;
		    }
		}
		if(j >= dstlist->nelements){
		    /*
		    ** no match
		    */
		    continue;
		}
	    }
	    srctouched = 1;
	    /*
	    ** mark it as being accessed for later tallying
	    */
	    conn->target->flags |= MARKERMASK;
	    /*
	    ** establish the parameter ranges
	    */
	    if(conncount > 0){
		/*
		** max
		*/
		if(conn->target->x > txmax) 
		    txmax = conn->target->x;
		if(conn->target->y > tymax) 
		    tymax = conn->target->y;
		if(conn->target->z > tzmax) 
		    tzmax = conn->target->z;
		if(conn->delay > maxdelay) 
		    maxdelay = conn->delay;
		if(conn->weight > maxweight) 
		    maxweight = conn->weight;

		/*
		** min
		*/
		if(conn->target->x < txmin) 
		    txmin = conn->target->x;
		if(conn->target->y < tymin) 
		    tymin = conn->target->y;
		if(conn->target->y < tzmin) 
		    tzmin = conn->target->z;
		if(conn->delay < mindelay) 
		    mindelay = conn->delay;
		if(conn->weight < minweight) 
		    minweight = conn->weight;
	    } else {
		txmin = txmax = conn->target->x;
		tymin = tymax = conn->target->y;
		tzmin = tzmax = conn->target->z;
		mindelay = maxdelay = conn->delay;
		minweight = maxweight = conn->weight;
	    }
	    /*
	    ** sum the weights
	    */
	    totalweight += conn->weight;
	    /*
	    ** count the connections
	    */
	    conncount++;
	}
	srccount += srctouched;
    }
    dstcount = CountMarkers(RootElement());
    FreeElementList(srclist);
    FreeElementList(dstlist);

    if (returncount)
	return (float) conncount;

    return 0.0;
}
#else /* NEW_CONNECTION_STATUS */
int do_connection_status(argc,argv)
int 	argc;
char 	**argv;
{
ElementList 	*srclist;
ElementList 	*dstlist;
Projection 	*projection;
Connection 	*conn;
char		*srcpath;
char		*dstpath;
int		anydst = 0;
float		totalweight=0;
int		i,j;
int		dstcount = 0;
int		srccount = 0;
int		conncount = 0;
float 		sxmin = 0.0,symin = 0.0,szmin = 0.0;
float 		sxmax = 0.0,symax = 0.0,szmax = 0.0;
float 		txmin = 0.0,tymin = 0.0,tzmin = 0.0;
float 		txmax = 0.0,tymax = 0.0,tzmax = 0.0;
float 		mindelay = 0.0,maxdelay = 0.0;
float 		minweight = 0.0,maxweight = 0.0;
int 		srctouched;
int		display = 1;
int		nxtarg;
int		havesrc;

    if(argc < 2){
	printf("usage: %s [-count] [srcpath] [dstpath]\n",argv[0]);
	return(0);
    }
    nxtarg = 0;
    havesrc = 0;
    srcpath = dstpath = NULL;
    while(++nxtarg < argc){
	if(strcmp(argv[nxtarg],"-count") == 0){
	    display = 0;
	    nxtarg++;
	} else 
	if(!havesrc){
	    srcpath = argv[nxtarg];
	    havesrc = 1;
	} else {
	    dstpath = argv[nxtarg];
	}
    }
    if(!srcpath){
	printf("usage: %s [-count] [srcpath] [dstpath]\n",argv[0]);
	return(0);
    }
    srclist = WildcardGetElement(srcpath,1);
    if(srclist->nelements == 0){
	Error();
	printf("invalid srcpath '%s'\n",srcpath);
	FreeElementList(srclist);
	return(0);
    }
    if(dstpath){
	dstlist = WildcardGetElement(dstpath,1);
	if(dstlist->nelements == 0){
	    Error();
	    printf("invalid dstpath '%s'\n",dstpath);
	    FreeElementList(srclist);
	    FreeElementList(dstlist);
	    return(0);
	}
    } else {
	anydst = 1;
	dstlist = NULL;
    }
    ClearMarkers(RootElement());
    for(i=0;i<srclist->nelements;i++){
	projection = (Projection *)srclist->element[i];
	if(!CheckClass(projection,PROJECTION_ELEMENT)){
	    continue;
	}
	/*
	** bounding region
	*/
	if(srccount > 0){
	    sxmax = MAX(sxmax,projection->x);
	    symax = MAX(symax,projection->y);
	    szmax = MAX(szmax,projection->z);

	    sxmin = MIN(sxmin,projection->x);
	    symin = MIN(symin,projection->y);
	    szmin = MIN(szmin,projection->z);
	} else {
	    sxmax = sxmin = projection->x;
	    symax = symin = projection->y;
	    szmax = szmin = projection->z;
	}
	srctouched = 0;
	/*
	** go through all connections
	*/
	for(conn = projection->connection;conn;conn=conn->next){
	    if(!anydst){
		/*
		** check to see whether the connection dst matches
		** anything in the specified targetlist
		*/
		for(j=0;j<dstlist->nelements;j++){
		    if(conn->target == (Segment *)dstlist->element[j]){
			break;
		    }
		}
		if(j >= dstlist->nelements){
		    /*
		    ** no match
		    */
		    continue;
		}
	    }
	    srctouched = 1;
	    /*
	    ** mark it as being accessed for later tallying
	    */
	    conn->target->flags |= MARKERMASK;
	    /*
	    ** establish the parameter ranges
	    */
	    if(conncount > 0){
		/*
		** max
		*/
		if(conn->target->x > txmax) 
		    txmax = conn->target->x;
		if(conn->target->y > tymax) 
		    tymax = conn->target->y;
		if(conn->target->z > tzmax) 
		    tzmax = conn->target->z;
		if(conn->delay > maxdelay) 
		    maxdelay = conn->delay;
		if(conn->weight > maxweight) 
		    maxweight = conn->weight;

		/*
		** min
		*/
		if(conn->target->x < txmin) 
		    txmin = conn->target->x;
		if(conn->target->y < tymin) 
		    tymin = conn->target->y;
		if(conn->target->y < tzmin) 
		    tzmin = conn->target->z;
		if(conn->delay < mindelay) 
		    mindelay = conn->delay;
		if(conn->weight < minweight) 
		    minweight = conn->weight;
	    } else {
		txmin = txmax = conn->target->x;
		tymin = tymax = conn->target->y;
		tzmin = tzmax = conn->target->z;
		mindelay = maxdelay = conn->delay;
		minweight = maxweight = conn->weight;
	    }
	    /*
	    ** sum the weights
	    */
	    totalweight += conn->weight;
	    /*
	    ** count the connections
	    */
	    conncount++;
	}
	srccount += srctouched;
    }
    dstcount = CountMarkers(RootElement());
    if(display){
	/*
	** print the results
	*/
	printf("\nStatus of connections from '%s'",srcpath);
	if(!anydst){
	    printf(" to '%s'",dstpath);
	} 
	printf("\n-------------------------------------------------------\n");
	printf("%d source(s)\n",srccount);
	printf("%d destination(s)\n",dstcount);
	printf("%d connections\n",conncount);
	if(conncount > 0){
	    printf("bounding src region = ( %g , %g , %g ) ( %g , %g , %g )\n",
	    sxmin,symin,szmin,sxmax,symax,szmax);
	    printf("bounding dst region = ( %g , %g , %g ) ( %g , %g , %g )\n",
	    txmin,tymin,tzmin,txmax,tymax,tzmax);
	    printf("weight range   = ( %g to %g)\n",minweight,maxweight);
	    printf("total weight   = ( %g )\n",totalweight);
	    printf("delay range    = ( %g to %g)\n",mindelay,maxdelay);
	}
	printf("\n");
    }
    FreeElementList(srclist);
    FreeElementList(dstlist);
    return(conncount);
}
#endif /* NEW_CONNECTTION_STATUS */
