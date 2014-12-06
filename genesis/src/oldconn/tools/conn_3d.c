static char rcsid[] = "$Id: conn_3d.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: conn_3d.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:16:59  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  2000/07/03 18:18:21  mhucka
** Removed extra function declaration that was annoying the compiler.
**
** Revision 1.4  1999/08/22 04:42:14  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.3  1997/07/18 03:08:45  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1995/07/08 19:09:31  dhb
** Made VolumeConnect() static to avoid link errors with newconn
** function of the same name.
**
 * Revision 1.1  1994/09/23  16:15:46  dhb
 * Initial revision
 *
 * Revision 1.7  1994/08/08  22:11:20  dhb
 * Changes from Upi.
 *
 * Also fixed bug in initopt() string in do_gen_3d_msg().
 *
 * Revision 1.7  1994/06/13  22:38:35  bhalla
 * Replaced the missing options for  specifying a hole in the volume
 *
 * Revision 1.2  1994/05/26  13:55:12  bhalla
 * reintroduced fixes that had been lost in the Gen2 version.
 *
 * Revision 1.1  1994/05/09  20:40:08  bhalla
 * Initial revision
 *
 * Revision 1.6  1994/03/21  02:14:59  dhb
 * Changed volume_connect() options back to 1.4.x style.  The new options
 * may be compiled if the NEW_VOLUMECONNECT_OPTIONS define is defined.
 *
 * Also changed call to GetActionFunc() which was missing extra args
 * which were added for extended objects.
 *
 * Revision 1.5  1993/07/08  20:44:00  dhb
 * Fixed bug in gen_3d_msg(): changed type of slot_type to short!
 *
 * Revision 1.4  1993/03/18  17:21:20  dhb
 * Element field protection.
 *
 * Revision 1.3  1993/02/24  15:41:35  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * Revision 1.2  1992/10/29  19:35:29  dhb
 * Replaced explicit msgin looping to use MSGLOOP macro.
 * Modified message creation code to account for removal of slot names
 *   from slots.
 *
 * Revision 1.1  1992/10/29  19:21:45  dhb
 * Initial revision
 *
*/

/*******************************************************************
**                                                                **
**                          conn_3d.c                             **
**                Written by U.S.Bhalla.                          **
**  Permission to use and modify this software is freely granted, **
**    provided this message and acknowledgement remain intact.    **
**                                                                **
**	Parts of this code are derived from code written by           **
**                     Matt Wilson                                **
**                                                                **
**  I do not take any responsibility for the functionality and    **
**  use of this code, which is provided 'as is'.                  **
**                                                                **
*******************************************************************/

#ifndef EPSILON
#define EPSILON 1e-60
#endif

#include <stdio.h>
#include <math.h>

#include "simconn_ext.h"
#include "toolconn_ext.h"


struct vol_mask_type {
	short	type;
	short	box;
	float	cx,cy,cz;
	float	rx,ry,rz;
};

struct plane_mask_type {
	short	type;
	float	cx,cy,cz;
	float	nx,ny,nz;
	float	r,h;
};

struct vol_mask_type *GetVolMaskFromArgv();
extern double ExtFieldMessageData();

static void VolumeConnect(projection_path,src_region,src_nregions,
	    segment_path,dst_region,dst_nregions,
	    connection_object_name,
	    pconnect,
	    relative)
char 			*projection_path;
struct vol_mask_type 	*src_region;
int 			src_nregions;
char 			*segment_path;
struct vol_mask_type 	*dst_region;
int 			dst_nregions;
char			*connection_object_name;
float			pconnect;
int			relative;
{
Projection 		*projection;
Segment 		*segment;
Connection 		*connection;
ElementList		*slist,*plist;
float 			srcx,srcy,srcz;
GenesisObject			*connection_object;
int			i,j;
PFI			action_func;

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
	if(!IsElementWithinVolume(projection,src_region,src_nregions)){
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
	srcz = projection->z;
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
			if(!IsElementWithinVolumeRel(segment,dst_region,
			dst_nregions,srcx,srcy,srcz)){
		    	continue;
			}
	    } else {
		if(!IsElementWithinVolume(segment,dst_region,dst_nregions)){
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
    printf("\n");
    /*
    ** free up the element lists
    */
    FreeElementList(slist);
    FreeElementList(plist);
}


/*
** This routine derived from Matt Wilson's code, in particular,
** from the do_region_connect function.
**
** connects the elements of one tree to another
** using specified source projections and destination segments
** and the specified source and destination masks
** The destination mask is given relative to the source element location
*/
#ifdef NEW_VOLUMECONNECT_OPTIONS
do_volume_connect(argc,argv)
int argc;
char **argv;
{
char                    *projection_name;
char                    *segment_name;
char                    *connection_object_name;
int                     src_count,dst_count;
struct vol_mask_type	*src_mask,*dst_mask;
int                     i;
int                     nxtarg;
float                   pconnect = 1;
int                     relative = 0;
char                    *type;
int			box;
int			status;

    box = 0;
    src_count = 0;
    dst_count = 0;

    initopt(argc, argv, "source-path dest-path connection-object -relative -box -sourcemask cx cy cz rx ry rz -sourcehole cx cy cz rx ry rz -destmask cx cy cz rx ry rz -desthole cx cy cz rx ry rz -probability p");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-sourcemask") == 0)
	    src_count++;
	else if (strcmp(G_optopt, "-destmask") == 0)
	    dst_count++;
	else if (strcmp(G_optopt, "-sourcehole") == 0)
	    src_count++;
	else if (strcmp(G_optopt, "-desthole") == 0)
	    dst_count++;
	else if (strcmp(G_optopt, "-probability") == 0)
	    pconnect = Atof(optargv[1]);
	else if (strcmp(G_optopt, "-relative") == 0)
	    relative = 1;
	else if (strcmp(G_optopt, "-box") == 0)
	    box = 1;
      }

    if(status < 0){
	printoptusage(argc, argv);
	printf("\n");
        printf("The default mode is to use an ellipse centered at cx,cy,cz\n");
        printf("and with axes rx,ry,rz\n");
        return;
    }

    /*
    ** allocate memory for the source and destination mask.  Then make a
    ** second pass through the options filling in the mask information.
    */

    src_mask = (struct vol_mask_type *)
			malloc(src_count*sizeof(struct vol_mask_type));
    dst_mask = (struct vol_mask_type *)
			malloc(dst_count*sizeof(struct vol_mask_type));

    src_count = 0;
    dst_count = 0;

    initopt(argc, argv, "source-path dest-path connection-object -relative -box -sourcemask cx cy cz rx ry rz -sourcehole cx cy cz rx ry rz -destmask cx cy cz rx ry rz -desthole cx cy cz rx ry rz -probability p");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	struct vol_mask_type*	mask;

	mask = NULL;
	if (strcmp(G_optopt, "-sourcemask") == 0)
	  {
	    mask = src_mask + src_count;
	    mask->type = 1;
	    mask->box = box;
	    src_count++;
	  }
	else if (strcmp(G_optopt, "-destmask") == 0)
	  {
	    mask = dst_mask + dst_count;
	    mask->type = 1;
	    mask->box = box;
	    dst_count++;
	  }
	else if (strcmp(G_optopt, "-sourcehole") == 0)
	  {
	    mask = src_mask + src_count;
	    mask->type = -1;
	    mask->box = box;
	    src_count++;
	  }
	else if (strcmp(G_optopt, "-desthole") == 0)
	  {
	    mask = dst_mask + dst_count;
	    mask->type = -1;
	    mask->box = box;
	    dst_count++;
	  }

	if (mask != NULL)
	  {
	    mask->cx = Atof(optargv[1]);
	    mask->cy = Atof(optargv[2]);
	    mask->cz = Atof(optargv[3]);
	    mask->rx = Atof(optargv[4]);
	    mask->ry = Atof(optargv[5]);
	    mask->rz = Atof(optargv[6]);
	  }
      }

    projection_name = optargv[1];
    segment_name  = optargv[2];
    connection_object_name  = optargv[3];

    VolumeConnect(projection_name,src_mask,src_count, 
	segment_name,dst_mask,dst_count,
	connection_object_name,pconnect,relative);

    free(src_mask);
    free(dst_mask);
    OK();
}
#else /* NEW_VOLUMECONNECT_OPTIONS */
void do_volume_connect(argc,argv)
int argc;
char **argv;
{
char                        *projection_name;
char                        *segment_name;
char                        *connection_object_name;
int                         src_count,dst_count;
struct vol_mask_type        *src_mask,*dst_mask;
int                         nxtarg;
float                       pconnect = 1;
int                         relative = 0;
int	   		    box = 0;

    if(argc < 4){
        printf("usage: %s src_path [to] dst_path [[with] connection_object]\n",
        argv[0]);
        printf("       [-rel] [-box]\n");
        printf("       nsrc_masks type cx cy cz rx ry rz ...\n");
        printf("       ndst_masks type cx cy cz rx ry rz ...\n");
        printf("       [probability of connection]\n");
        printf("The default mode is to use an ellipse centered at cx,cy,cz\n");
        printf("and with axes rx,ry,rz\n");
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
        if(strcmp(argv[nxtarg],"-box") == 0) {
                box = 1;        
                nxtarg++;
        }
    /*
    ** set up the src masks
    */
    src_mask = GetVolMaskFromArgv(&src_count,&nxtarg,argc,argv,box);
    /*
    ** set up the dst masks
    */
    dst_mask = GetVolMaskFromArgv(&dst_count,&nxtarg,argc,argv,box);
    /*
    ** if there is an argument left then use it as the probability
    ** of connection
    */
    if(argc > nxtarg){
        pconnect = Atof(argv[nxtarg]);
    }
    VolumeConnect(projection_name,src_mask,src_count, 
    segment_name,dst_mask,dst_count,
    connection_object_name,pconnect,relative);
    free(src_mask);
    free(dst_mask);
    OK();
}
#endif /* NEW_VOLUMECONNECT_OPTIONS */


struct vol_mask_type *GetVolMaskFromArgv(count,nxtarg,argc,argv,box)
int *count;
int *nxtarg;
int argc;
char **argv;
int	box;
{
int i;
struct vol_mask_type *mask;
int argcount;

    argcount = *nxtarg;
    *count = atoi(argv[argcount++]);
    if(argc < argcount + *count){
        Error();
        printf("bad region specification\n");
        printf("format: n +-1 cx1 cy1 cz1 rx1 ry1 rz1 ... +-1 cxn cyn czn rxn ryn rzn\n");
        return(NULL);
    }
    mask = (struct vol_mask_type *)
    malloc((*count) * sizeof(struct vol_mask_type));
    for(i=0;i<(*count);i++){
        mask[i].type = atoi(argv[argcount++]); 
        mask[i].box = box; 
        mask[i].cx = Atof(argv[argcount++]); 
        mask[i].cy = Atof(argv[argcount++]); 
        mask[i].cz = Atof(argv[argcount++]); 
        mask[i].rx = Atof(argv[argcount++]); 
        mask[i].ry = Atof(argv[argcount++]); 
        mask[i].rz = Atof(argv[argcount++]); 
    }
    *nxtarg = argcount;
    return(mask);
}


int IsElementWithinVolume(element,mask,nmasks)
Element         *element;
struct vol_mask_type *mask;
int nmasks;
{
register float x,y,z;
float		rx,ry,rz;
register struct vol_mask_type *ptr;

    /*
    ** if no masks are specified then assume that is within the region
    */
    if(nmasks == 0) return(1);
    /*
    ** search the mask list from the bottom up
    ** if the element is bounded by a mask then
    ** it will possess that sign and no further search is 
    ** necessary
    */
    x = element->x;
    y = element->y;
    z = element->z;
    ptr = mask + nmasks-1;
    do{
		if (ptr->box) {
        	if ( 
        		x >= (ptr->cx - ptr->rx) && 
        		x <= (ptr->cx + ptr->rx) && 
        		y >= (ptr->cy - ptr->ry) &&
        		y <= (ptr->cy + ptr->ry) &&
        		z >= (ptr->cz - ptr->rz) &&
        		z <= (ptr->cz + ptr->rz)) {
            		return(ptr->type == 1);
        	}
		} else { /* Elliptical volume */
			if (ptr->rx < EPSILON ||
				ptr->ry < EPSILON ||
				ptr->rz < EPSILON) {
				return(0);
			}
			rx = (x - ptr->cx) / ptr->rx;
			ry = (y - ptr->cy) / ptr->ry;
			rz = (z - ptr->cz) / ptr->rz;
			if ((rx * rx + ry * ry + rz * rz) <= 1) {
            		return(ptr->type == 1);
			}
		}
    } while(ptr-- != mask);
    return(0);
}

int IsElementWithinVolumeRel(element,mask,nmasks,relx,rely,relz)
Element                *element;
struct vol_mask_type *mask;
int nmasks;
float relx,rely,relz;
{
register float x,y,z;
float		rx,ry,rz;
register struct vol_mask_type *ptr;

    /*
    ** if no masks are specified then assume that is within the region
    */
    if(nmasks == 0) return(1);
    /*
    ** search the mask list from the bottom up
    ** if the element is bounded by a mask then
    ** it will possess that sign and no further search is 
    ** necessary
    */
    x = element->x - relx;
    y = element->y - rely;
    z = element->z - relz;
    ptr = mask + nmasks-1;
    do{
		if (ptr->box) {
        	if ( 
        		x >= (ptr->cx - ptr->rx) && 
        		x <= (ptr->cx + ptr->rx) && 
        		y >= (ptr->cy - ptr->ry) &&
        		y <= (ptr->cy + ptr->ry) &&
        		z >= (ptr->cz - ptr->rz) &&
        		z <= (ptr->cz + ptr->rz)) {
            		return(ptr->type == 1);
        	}
		} else { /* Elliptical volume */
			if (ptr->rx < EPSILON ||
				ptr->ry < EPSILON ||
				ptr->rz < EPSILON) {
				return(0);
			}
			rx = (x - ptr->cx) / ptr->rx;
			ry = (y - ptr->cy) / ptr->ry;
			rz = (z - ptr->cz) / ptr->rz;
			if ((rx * rx + ry * ry + rz * rz) <= 1) {
            		return(ptr->type == 1);
			}
		}
    } while(ptr-- != mask);
    return(0);
}
