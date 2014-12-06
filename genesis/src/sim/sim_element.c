static char rcsid[] = "$Id: sim_element.c,v 1.2 2005/06/27 19:00:59 svitak Exp $";

/*
** $Log: sim_element.c,v $
** Revision 1.2  2005/06/27 19:00:59  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1993/06/29 18:53:23  dhb
** Extended objects (incomplete)
**
 * Revision 1.3  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.2  1993/02/23  19:10:32  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_exists changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:00:15  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

Element *GetChildElement(element,name,count)
Element *element;
char	*name;
int	count;
{
Element *child;

    for(child=element->child;child;child=child->next){
	if(VISIBLE(child) && (name == NULL || strcmp(child->name,name) ==0)){ 
#ifdef OLD
	    if(--count < 0)
		return(child);
#else
	    if(child->index == count)
		return(child);
#endif
	}
    }
    return(NULL);
}

int IsElementWithinRegion(element,mask,nmasks)
Element 	*element;
struct mask_type *mask;
int nmasks;
{
register float x,y;
register struct mask_type *ptr;

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
    ptr = mask + nmasks-1;
    do{
	if( 
	x >= ptr->xl && 
	x <= ptr->xh &&
	y >= ptr->yl && 
	y <= ptr->yh){
	    return(ptr->type == 1);
	}
    } while(ptr-- != mask);
    return(0);
}

int IsElementWithinRegionRel(element,mask,nmasks,relx,rely)
Element		*element;
struct mask_type *mask;
int nmasks;
float relx,rely;
{
register float x,y;
register struct mask_type *ptr;

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
    ptr = mask + nmasks-1;
    do{
	if( 
	x >= ptr->xl && 
	x <= ptr->xh &&
	y >= ptr->yl && 
	y <= ptr->yh){
	    return(ptr->type == 1);
	}
    } while(ptr-- != mask);
    return(0);
}

int do_exists(argc,argv)
int	argc;
char **argv;
{
Element *element;
char	*do_getelementenv();

    initopt(argc, argv, "element [field]");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if((element = GetElement(optargv[1])) == NULL){
	return(0);
    }

    if(argc == 3)
	return ElmFieldValue(element, optargv[2]) != NULL;
    else
	return(1); /* element exists */
}
