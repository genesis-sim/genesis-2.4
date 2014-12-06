static char rcsid[] = "$Id: cell_sheet.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: cell_sheet.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.13  2001/06/29 21:32:36  mhucka
** Added extra parens inside conditionals and { } groupings to quiet certain
** compiler warnings.
**
** Revision 1.12  2000/04/19 07:59:22  mhucka
** Missed an instance of ifdef PARALLEL.
**
** Revision 1.11  2000/04/19 07:58:00  mhucka
** Fix from Dave Beeman: Replaced PARALLEL macro with OLD_PARALLEL, to make sure
** that Upi's obsolete parallel library code doesn't accidently get used, but
** preserve it in the source.  Removed the -parallel option in initopt.
**
** Revision 1.10  1997/08/08 20:16:32  dhb
** Fixed check of return value from DetachElement() which returns
** int (bool) rather than a pointer.
**
** Revision 1.9  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.8  1996/11/04 01:33:39  dhb
** Fixed undefined variable name
**
 * Revision 1.7  1996/10/14  17:52:10  dhb
 * Added support for GetTreeCount() valid_index parameter.
 *
 * Revision 1.6  1995/06/24  15:32:29  dhb
 * Removed call to connection compatability library function
 * CopyConnections().  This is called from the COPY action on
 * axons.
 *
 * Added call to CopyAction() which calls the COPY action on the
 * copied element tree.
 *
 * Added code to check the CopyElementTree() was successful.
 *
 * Had to change some uses for optargv now that we are calling
 * an ACTION, as optargv could change if a script calls another
 * command.
 *
 * Revision 1.5  1995/05/09  23:31:13  dhb
 * Removed erroneous free(pts) statement.  Pts is a locally defined
 * array and is not allocated.
 *
 * Revision 1.4  1995/01/24  22:31:32  dhb
 * Added check for prototype element existence in do_cell_sheet().
 *
 * Revision 1.3  1993/02/24  15:41:35  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * Revision 1.2  1992/10/29  19:11:19  dhb
 * Replaced explicit msgin looping with MSGLOOP macro.
 *
 * Revision 1.1  1992/10/29  19:09:49  dhb
 * Initial revision
 *
*/

/*******************************************************************
**                                                                **
**                       cell_sheet.c                             **
**                Written by U.S.Bhalla.                          **
**  Permission to use and modify this software is freely granted, **
**    provided this message and acknowledgement remain intact.    **
**                                                                **
**  The cell-creation code is derived from code written by        **
**                      Matt Wilson                               **
**                                                                **
**  I do not take any responsibility for the functionality and    **
**  use of this code, which is provided 'as is'.                  **
**                                                                **
*******************************************************************/

/*
** Cell_sheet : creates a sheet of cells based on coords specified
** by a coordinate file as a list of numbers. The cells are copies
** of prototype cells. Options allow one to reorient the cells to
** remain normal to the local plane of cells, to reposition the
** compartments of the cells to follow the curve of the plane,
** and to randomise the coordinates as the cells are made.
*/

#include <stdio.h>
#include <math.h>
#include "sim_ext.h"
#include "tools.h"

#define N_SURFACE_PTS 5000 /* A really big simulation */
#ifndef RAXIAL
#define RAXIAL 1
#endif

#ifndef SMALL_EL_SIZE
#define SMALL_EL_SIZE	500
#define MAX_EL_SIZE	5000
#endif

#ifndef dot_prd
#define dot_prd(x1,y1,z1,x2,y2,z2) (x1 * x2 + y1 * y2 + z1 * z2)
#endif

typedef struct surface_pt_type {
	float x,y,z;	/* coords of pt */
	float nx,ny,nz;	/* vector for normal */
} SurfacePt;

void find_nearest_pt(pts,npts,x,y,z,pt)
	SurfacePt	*pts;
	int			npts;
	float		x,y,z;
	SurfacePt	*pt;
{
	register int		i;
	register float	r;
	int	besti = -1;
	register float	bestr;
	register	SurfacePt	*spt;

	spt = pts;
	bestr = (spt->x - x) * (spt->x - x) +
			(spt->y - y) * (spt->y - y) +
			(spt->z - z) * (spt->z - z);

	for (i = 0, spt = pts ; i < npts ; i++, spt++) {
		r = (spt->x - x) * (spt->x - x) +
			(spt->y - y) * (spt->y - y) +
			(spt->z - z) * (spt->z - z);
		if (r < bestr) {
			bestr = r;
			besti = i;
		}
	}
	if (besti < 0)
		*pt = pts[0];
	else 
		*pt = pts[besti];
}

/*
** curve_arbor : adjusts the arbor to conform to the curvature of the
** surface  : for now, assume it is only convex.
** This algorithm assumes that the length of each compartment is less
** than the radius of curvature of the surface, otherwise the errors
** tend to add up. It also assumes that the sampling density over
** the plane results in a smaller spacing of samples than the 
** dendritic length.
** This routine does not change the positions of compartments which
** are within a cylinder of xxx from the parent elm.
*/
void curve_arbor(elm,elmpt,pts,npts)
	Element	*elm;
	SurfacePt	*elmpt,*pts;
	int		npts;
{
	SurfacePt	pt;
	ElementList	el;
	Element		*elms[MAX_EL_SIZE];
	MsgIn		*msg;
	Element		*kid;
	float		theta;
	float		c;

	el.element = elms;
	el.nelements = 0;
/* Find ref coord, where normal to elm crosses surface. */
/* Move out from elm to dends, ignoring all compartments less than
** pi/6 from normal. */
/* Find closest pt to dend. */
/* Get angle between line joining this pt and elm, and normal. */
/* Rotate elm and all kids by this angle, in plane defined by 
** normal, parent elm, and new elm. */
/* Continue recursively for all kids down this branch, now defining
** parent elm as the rotated elm, and the normal as the perp to the
** dend in the plane of the original normal. */

	/* Scan through all kids of element */
	MSGLOOP(elm, msg) {
	    case RAXIAL:
		{
			kid = msg->src;
			/* find the nearest pt in surface definition list */
			find_nearest_pt(pts,npts,kid->x,kid->y,kid->z,&pt);

			/* Find the angle between the normal to this point and
			** the original normal */
			c = dot_prd(pt.nx,pt.ny,pt.nz,
				elmpt->nx,elmpt->ny,elmpt->nz);
			if (c < 1.0)  /* to compensate for rounding errors */
				theta = acos(c);
			else
				theta = 0.0;
			/* If theta is not between 10 and 60 degrees, skip curve */
			if (theta < (PI/18.0) || theta > (PI/3.0)) {
				curve_arbor(kid,&pt,pts,npts);
				continue;
			}

			/* Sign flip since we wish to rotate DOWN, not up */
			theta = -theta;

			/* Rotate 'kid' and all its subkids by theta */
			find_all_dends(kid,1,&el);
			RotateInPlane(&el,elm->x,elm->y,elm->z,kid->x,kid->y,kid->z,
				elm->x + elmpt->nx, elm->y + elmpt->ny,
				elm->z + elmpt->nz,theta);
			/* Recurse down to repeat whole process for kid */
			curve_arbor(kid,&pt,pts,npts);
		}
		break;
	}
}

/*
** If elm is not a compartment, curve looks for somas among its children
** to do the curving algorithm from.
*/
void curve_somas(elm,elmpt,pts,npts) 
	Element	*elm;
	SurfacePt	*elmpt,*pts;
{
	ElementList	el;
	Element	*elms[MAX_EL_SIZE];
	int		i;
	
	el.nelements = 0;
	el.element = elms;
	find_somas(elm,&el);
	
	for(i = 0 ; i < el.nelements ; i++)
		curve_arbor(el.element[i],elmpt,pts,npts);
}

void do_cell_sheet(argc,argv)
int argc;
char **argv;
{
Element 	*element;
Element 	*new_element;
Element		*last_element;
Element 	*parent;
Element 	*new_parent;
Element		*elm;
ElementList	sel,el;
Element		*selms[SMALL_EL_SIZE],*elms[MAX_EL_SIZE];
FILE		*fp,*fopen();
int			i,j;
int			orient = 0, curve = 0, randomise = 0,twirl = 0;
int			use_prototype=0;
char		*coord_file_name;
float		fx = 0.0,fy = 0.0,fz = 0.0;
char		name[100];
int			index;
int			valid_index;
char		*ptr;
SurfacePt	pts[N_SURFACE_PTS];
SurfacePt	*pt;
int			npts;
float		theta = 0.0;
#ifdef OLD_PARALLEL
int			use_parallel = 0;
int			IsInPostRegion();
#endif
int		status;

	sel.nelements = 0;
	sel.element = selms;
	el.nelements = 0;
	el.element = elms;

    initopt(argc, argv, "prototype destination file-name -orient -curve -randomize x y z -twirl theta -useprototype");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if(strcmp(G_optopt, "-orient") == 0) /* orient */
	    orient = 1;
	if(strcmp(G_optopt, "-curve") == 0)
	    curve = 1;
	if(strcmp(G_optopt, "-randomize") == 0) {
	    randomise = 1;
	    fx = Atof(optargv[1]);
	    fy = Atof(optargv[2]);
	    fz = Atof(optargv[3]);
	}
	if(strcmp(G_optopt, "-twirl") == 0) {
		twirl = 1;
		theta = Atof(optargv[1]);
		theta *= PI/180.0;
	}
	if(strcmp(G_optopt, "-useprototype") == 0)
		use_prototype = 1;
#ifdef OLD_PARALLEL
	if(strcmp(G_optopt, "-parallel") == 0) {
		use_parallel = 1;
	}
#endif
      }

    if(status < 0){
	printoptusage(argc, argv);
	printf("\n");
	printf("The 'prototype' cell is copied to the 'destination' element to\n");
	printf("form a sheet of cells at positions specified in three-d\n");
	printf("by the file. The -orient option reorients the cell to\n");
	printf("be normal to the local region of the sheet, The normals are also in the file\n");
	printf("The -curve option positions the children elements\n");
	printf("of the cell to follow the curvature of the sheet\n");
	printf("The -randomize option randomizes the coordinates by the\n");
	printf("specified factors in x,y,z before orienting/positioning\n");
	printf("The -twirl option rotates each cell about the normal by\n");
	printf("a random angle from 0 to theta (in degrees)\n");
	printf("The -useprototype option uses the prototype for the\n");
	printf("last cell on the list of coords, to economize on space\n");
#ifdef OLD_PARALLEL
	printf("The -parallel option tells the routine to load coords\n");
	printf("within the zone defined by the postmaster\n");
#endif

	return;
    }

    /*
    ** open the sheet file
    */
    coord_file_name = optargv[3];
	if (!(fp = fopen(coord_file_name,"r"))) {
	    fprintf(stderr,"could not open coordinate file '%s'\n",coord_file_name);
	    return;
	}
	if (IsSilent() < 2)
	    printf("Reading in cell coordinates from file %s\n",coord_file_name);
    /*
    ** find the element used to fill the map
    */
    element = GetElement(optargv[1]);
    if (element == NULL) {
	InvalidPath(argv[0], optargv[1]);
	fclose(fp);
	return;
    }

    /*
    ** find the element to attach the elements to
    */
    parent = GetElement(optargv[2]);
    /*
    ** if no such parent exists
    */
    if(parent == NULL){
	/*
	** try to create it using the next level up on the tree as
	** the new parent for the parent
	*/
		strcpy(name,GetParentComponent(optargv[2]));
		new_parent = GetElement(name);
		/*
		** if some place to attach the parent was found then go ahead
		** and create the parent
		*/
		if(new_parent != NULL){
			strcpy(name,GetBaseComponent(optargv[2]));
			index = GetTreeCount(name, &valid_index);
			if (!valid_index){
			    Error();
			    printf("missing or bad element index in cellsheet destination path '%s'\n", name);
			    return;
			}
			if ((ptr = strchr(name,'['))) {
				*ptr = '\0';
			} 
	   		parent = Create("neutral",name,new_parent,NULL,index);
		} else {
			printf("could not create element %s\n",name);
			return;
		}
    }
	last_element = parent->child;

#ifdef OLD_PARALLEL
	if (use_parallel) {
		for(npts = 0, pt = pts ;
			fscanf(fp,"%f%f%f%f%f%f",
			&(pt->x),&(pt->y),&(pt->z),&(pt->nx),&(pt->ny),&(pt->nz))
			== 6;) {
			if (IsInPostRegion(pt->x,pt->y,pt->z)) {
				pt++;
				npts++;
			}
		}
	} else {
#else
	{
#endif
	for(npts = 0, pt = pts ;
		fscanf(fp,"%f%f%f%f%f%f",
		&(pt->x),&(pt->y),&(pt->z),&(pt->nx),&(pt->ny),&(pt->nz)) == 6;
		npts++, pt++)
		;
	}
	fclose(fp);

	for (i = 0 , pt = pts; i < npts ; pt++,i++) {
		if (use_prototype && i==npts-1) {
			if (!DetachElement(element)) {
				printf("Could not use prototype element %s\n",
					element->name);
				return;
			}
	    	new_element = element;
	    	AttachToEnd(parent,last_element,new_element);
			if (last_element == NULL) {
				new_element->index = 0;
			} else {
				new_element->index = last_element->index + 1;
			}
			last_element = new_element;
		} else {
	    	new_element = CopyElementTree(element);
		if (new_element == NULL) {
			printf("copy from prototype element %s failed\n",
				Pathname(element));
			return;
		}
	    	AttachToEnd(parent,last_element,new_element);
			if (last_element == NULL) {
				new_element->index = 0;
			} else {
				new_element->index = last_element->index + 1;
			}
			last_element = new_element;
			CopyMsgs(element,new_element);
		        if (CopyAction(element,new_element)==0){
				DeleteElement(new_element);
				return;
			}
		}

		if (randomise)
			rancoord(new_element,fx,fy,fz,1,1);

		if (twirl) {
			find_somas(new_element,&sel);
			for(j = 0 ; j < sel.nelements ; j++) {
				elm = sel.element[j];
				find_all_dends(elm,1,&el);
				RotateAboutAxis(&el, elm->x,elm->y,elm->z, 0.0,0.0,1.0,
					frandom(theta,-theta));
			}
		}

		rel_position(new_element,pt->x,pt->y,pt->z);

		if (orient) {
			find_all_kids(new_element,&el);
			rotate3d(&el,pt->x,pt->y,pt->z,
				0.0,0.0,1.0,
				pt->nx,pt->ny,pt->nz);
		}

		if (curve) {
			curve_somas(new_element,pt,pts,npts);
		}
	}
	npts = 0;
	if (IsSilent() < 2)
		printf("coordinate file %s read\n",coord_file_name);
}
