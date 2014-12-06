static char rcsid[] = "$Id: sim_grid.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_grid.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:31:57  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:07:22  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

do_create_map(argc,argv)
int argc;
char **argv;
{
Element 	*element;
Element 	*parent;
int 		nx,ny;
float 		wdx,wdy;
float 		wxmin,wymin;
Element 	*new_parent;

    if(argc < 5){
	printf("usage: %s element parent Nx Ny [wdx wdy][wxmin wymin]\n",
	argv[0]);
	return;
    }
    /*
    ** find the element used to fill the map
    */
    element = GetElement(argv[1]);
    /*
    ** find the element to attach the elements to
    */
    parent = GetElement(argv[2]);
    /*
    ** if no such parent exists
    */
    if(parent == NULL){
	/*
	** try to create it using the next level up on the tree as
	** the new parent for the parent
	*/
	new_parent = GetElement(GetParentComponent(argv[2]));
	/*
	** if some place to attach the parent was found then go ahead
	** and create the parent
	*/
	if(new_parent != NULL){
	    parent = CreateElement();
	    parent->name = GetBaseComponent(argv[2]);
	    Attach(new_parent,parent);
	}
    }
    /*
    ** set the map parameters
    */
    nx = atoi(argv[3]);
    ny = atoi(argv[4]);
    if(argc > 5){
	wdx = Atof(argv[5]);
	wdy = Atof(argv[6]);
    } else {
	wdx = 1.0;
	wdy = 1.0;
    }
    if(argc > 7){
	wxmin = Atof(argv[7]);
	wymin = Atof(argv[8]);
    } else {
	wxmin = 0.0;
	wymin = 0.0;
    }
    /*
    ** make the map
    */
    if(!CreateMap(parent,element,nx,ny,wdx,wdy,wxmin,wymin)){
	printf("unable to create map\n");
    }
}

/*
** allocate a 2 dimensional map grid of pointers to elements
*/
Element ***CreateGrid(nx,ny)
int 	nx;
int	ny;
{
Element 	***grid;
int 		i;

    grid = (Element ***)
    malloc(sizeof(Element **)*ny);
    for(i=0;i<ny;i++){
	grid[i] = (Element **)malloc(sizeof(Element *)*nx);
    }
    return(grid);
}

/*
** create a 2 dimensional map of elements
*/
CreateMap(parent,element,nx,ny,dx,dy,wxmin,wymin)
Element	*parent;
Element	*element;
int 		nx,ny;
float 		dx,dy;
float 		wxmin,wymin;
{
Element 	*new_element;
struct map_type *map;
int 		i,j;

    /*
    ** check for valid parameters
    */
    if(element == NULL || parent == NULL || 
    nx < 1 || ny < 1 || dx < 0 || dy < 0){
	return(0);
    }
    /*
    ** create the map structure
    */
    map = (struct map_type *)malloc(sizeof(struct map_type));
    /*
    ** fill in the map parameters
    */
    map->xmax = nx-1;
    map->ymax = ny-1;
    map->dx = 	dx;
    map->dy = 	dy;
    map->wxmin = wxmin;
    map->wymin = wymin;
    /*
    ** allocate the grid
    */
    map->grid = CreateGrid(nx,ny);
    /*
    ** fill the grid
    */
    for(i=0;i<ny;i++){
	for(j=0;j<nx;j++){
	    /*
	    ** make a copy of the element and all its sub elements
	    */
	    new_element = CopyElementTree(element);
	    /*
	    ** set the relative coordinates
	    */
	    new_element->rel_x = j*dx;
	    new_element->rel_y = i*dy;
	    /*
	    ** set the default positions to the grid coordinates
	    */
	    new_element->x = j*dx;
	    new_element->y = i*dy;
	    /*
	    ** attach the new element to the parent
	    */
	    Attach(parent,new_element);
	    /*
	    ** place the element into the grid
	    */
	    map->grid[i][j] = new_element;
	    /*
	    ** attach the map to the element
	    */
	    new_element->map = map;
	}
    }
    /*
    ** return 1 to indicate success
    */
    return(1);
}
