static char rcsid[] = "$Id: sim_map.c,v 1.3 2005/07/01 10:03:09 svitak Exp $";

/*
** $Log: sim_map.c,v $
** Revision 1.3  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.13  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.12  1999/10/16 23:53:06  mhucka
** Added fixed from Dave Bilitch and Dave Beeman for problems involving
** numerical quantities in CreateMap.
**
** Revision 1.11  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.10  1996/10/14 17:44:40  dhb
** Added missing valid index parameter to GetTreeCount() call.
**
 * Revision 1.9  1996/09/13  01:03:00  dhb
 * Added valid_index handling to GetTreeCount().
 *
 * Revision 1.8  1996/05/16  22:13:06  dhb
 * Support for element path hash table.
 *
 * Revision 1.7  1994/10/26  17:23:36  dhb
 * Fixed index generation bug and bug in freeing previous elements when
 * the overwrite condition occurs.
 *
 * Revision 1.6  1994/10/21  00:04:30  dhb
 * Removed call to CopyCOnnections() in CreateMap().  Connections now handled
 * by CREATE/COPY action in connection compatability library.
 *
 * Added CREATE and COPY actions.  CREATE is called when we create the map
 * from objects, COPY when we copy the map from an element.
 *
 * Revision 1.5  1994/10/20  20:13:48  dhb
 * CreateMap() used to do bad things when a given map to be created
 * already existed.  Now checks for this and frees the old elements.
 * Also will correctly handle creating maps in an already existing
 * hierarchy.
 *
 * Revision 1.4  1994/03/24  01:30:49  dhb
 * Changes to allow either an element or an object to make the map of
 * element from.
 *
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/02/16  17:26:07  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_create_map (createmap) changed to use GENESIS getopt routines.
 * 	Command arguments for element position delta and origin rplaced
 * 	with options -delta dx dy and -origin x y respectively.
 *
 * Revision 1.1  1992/10/27  20:17:29  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

void do_create_map(argc,argv)
int 	argc;
char 	**argv;
{
GenesisObject	*object;
Element 	*element;
Element 	*parent;
int		from_object;
int 		nx,ny;
float 		wdx,wdy;
float 		wxmin,wymin;
Element 	*new_parent;
char 		name[100];
char		*srcname;
char		*parentname;
int		index;
int		valid_index;
char		*ptr;
int		status;

    wdx = 1.0;
    wdy = 1.0;
    wxmin = 0.0;
    wymin = 0.0;
    from_object = 0;

    initopt(argc, argv, "element-or-object dest Nx Ny -delta dx dy -origin x y -object");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-delta") == 0)
	  {
	    wdx = Atof(optargv[1]);
	    wdy = Atof(optargv[2]);
	  }
	else if (strcmp(G_optopt, "-origin") == 0)
	  {
	    wxmin = Atof(optargv[1]);
	    wymin = Atof(optargv[2]);
	  }
	else if (strcmp(G_optopt, "-object") == 0)
	  {
	    from_object = 1;
	  }
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }

    srcname = optargv[1];
    parentname = optargv[2];

    /*
    ** find the element used to fill the map and
    ** find the parent element to attach the elements to
    */

    if (from_object)
      {
	object = GetObject(srcname);
	if (object == NULL)
	  {
	    Error();
	    printf("could not find object '%s'\n", srcname);
	    return;
	  }
	element = object->defaults;
      }
    else
      {
	element = GetElement(srcname);
      }

    parent = GetElement(parentname);
    /*
    ** if no such parent exists
    */
    if(parent == NULL){
	/*
	** try to create it using the next level up on the tree as
	** the new parent for the parent
	*/
	strcpy(name,GetParentComponent(parentname));
	new_parent = GetElement(name);

	/*
	** if some place to attach the parent was found then go ahead
	** and create the parent
	*/
	if(new_parent != NULL){
	    strcpy(name,GetBaseComponent(parentname));
	    index = GetTreeCount(name, &valid_index);
	    if (!valid_index){
		Error();
		printf("missing or bad element index in createmap destination path '%s'\n", parentname);
		return;
	    }
	    if ((ptr = strchr(name,'['))) {
		*ptr = '\0';
	    }
	    parent = Create("neutral",name,new_parent,NULL,index);
	} else {
	    Error();
	    printf("could not create element %s\n",name);
	    return;
	}
    }

    /*
    ** set the map parameters
    */
    nx = atoi(optargv[3]);
    ny = atoi(optargv[4]);

    /*
    ** make the map
    */
    if(!CreateMap(parent,element,from_object,nx,ny,wdx,wdy,wxmin,wymin)){
	Error();
	printf("unable to create map\n");
	return;
    }
    OK();
}

/*
** allocate a 2 dimensional map grid
*/
Element ***CreateGrid(nx,ny)
int nx,ny;
{
Element ***grid;
int i;

	grid = (struct element_type ***)
	malloc(sizeof(struct element_type **)*ny);
	for(i=0;i<ny;i++){
		grid[i] = (struct element_type **)
		malloc(sizeof(struct element_type *)*nx);
	}
	return(grid);
}

/*
** create a 2 dimensional map of elements
*/
int CreateMap(parent,element,composite,nx,ny,dx,dy,wxmin,wymin)
Element *parent;
Element *element;
int composite;
int nx,ny;
double dx,dy;
double wxmin,wymin;
{
Element *last_element;
Element *new_element;
int warn;
int i,j,n;

    /*
    ** check for valid parameters
    */
    if(element == NULL || parent == NULL || 
    nx < 1 || ny < 1 || dx < 0 || dy < 0){
	return(0);
    }
#ifdef LATER	/* add the map grid to the elements */
    /*
    ** create the map structure
    */
	struct map_type *map;
    map = (struct map_type *)malloc(sizeof(struct map_type));
    /*
    ** fill in the map parameters
    */
    map->xmax = nx-1;
    map->ymax = ny-1;
    map->dx = dx;
    map->dy = dy;
    map->wxmin = wxmin;
    map->wymin = wymin;
    /*
    ** allocate the grid
    */
    map->grid = CreateGrid(nx,ny);
#endif

    /*
    ** check for and warn about overwrite of existing elements
    */

    warn = 0;
    n = nx * ny;
    last_element = NULL;
    new_element = parent->child;
    while (new_element != NULL){
	Element*	next_element = new_element->next;

	if (strcmp(new_element->name, element->name) == 0 &&
						new_element->index < n) {
	    ElementHashRemoveTree(new_element);

	    if (last_element == NULL)
		parent->child = next_element;
	    else
		last_element->next = next_element;

	    new_element->next = NULL;
	    FreeTree(new_element);
	    warn = 1;
	}
	else
	    last_element = new_element;

	new_element = next_element;
    }

    if (warn){
	Warning();
	printf("createmap: overwriting existing element(s)\n");
    }

    /*
    ** fill the grid
    */
    for(i=0;i<ny;i++){
	for(j=0;j<nx;j++){
	    Element*	child;

	    /*
	    ** make a copy of the element and all its sub elements
	    */
	    new_element = CopyElementTree(element);

	    if (composite)
	      {
		/*
		** go through each child and make it a component of the
		** root element of the object.
		*/
		child = new_element->child;
		while (child != NULL)
		  {
		    child->componentof = new_element;
		    child = child->next;
		  }
	      }

	    /*
	    ** set the default positions to the grid coordinates
	    */
	    PositionElement(new_element, j*dx + wxmin, i*dy + wymin,element->z);
	    /*
	    ** attach the new element to the parent
	    ** this needs to be optimized
	    */
	    AttachToEnd(parent,last_element,new_element);
	    if(i == 0 && j == 0){
		new_element->index = 0;
	    } else {
		new_element->index = last_element->index+1;
	    }
	    last_element = new_element;
	    /*
	    ** copy the msgs between elements
	    */
	    CopyMsgs(element,new_element);

	    if (composite)
		CreateAction(new_element, element, NULL);
	    else
		CopyAction(element,new_element);

            /* enter the new element(s) into the element hash table */
            ElementHashPutTree(new_element);

#ifdef LATER	/* add the map grid to the elements */
	    /*
	    ** place the element into the grid
	    */
	    map->grid[i][j] = new_element;
	    /*
	    ** attach the map to the element
	    */
	    new_element->map = map;
#endif
	}
    }
    /*
    ** return 1 to indicate success
    */
    return(1);
}
