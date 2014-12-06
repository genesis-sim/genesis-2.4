static char rcsid[] = "$Id: sim_position.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_position.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3  1993/06/29 18:53:23  dhb
** Extended objects (incomplete)
**
 * Revision 1.2  1993/02/16  17:52:46  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_position (position) changed to use GENESIS getopt routines.
 *
 * Added OK(); for successful completion of do_position.
 *
 * Fixed bug in do_position.  Was checking for "-" in the z component to
 * decide to ignore the z value.  Changed this to "I" as for x and y.
 *
 * Revision 1.1  1992/10/27  20:23:45  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

void do_position(argc,argv)
int argc;
char **argv;
{
Element *element;
float	x,y,z;

    initopt(argc, argv, "element x y z");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	fprintf(stderr,
	    "\t['I' to ignore, 'R' for relative (e.g. %s name R1.5 I 3).]\n",
	    optargv[0]);
	return;
      }

    if((element = GetElement(optargv[1])) == NULL){
	printf("unable to find element '%s'\n",argv[1]);
	return;
    }
    if(strcmp(optargv[2],"I") == 0){
	x = element->x;
    } else {
	if(optargv[2][0] == 'R'){
	    x = Atof(optargv[2]+1) + element->x;
	} else {
	    x = Atof(optargv[2]);
	}
    }
    if(strcmp(optargv[3],"I") == 0){
	y = element->y;
    } else {
	if(optargv[3][0] == 'R'){
	    y = Atof(optargv[3]+1) + element->y;
	} else {
	    y = Atof(optargv[3]);
	}
    }
    if(strcmp(optargv[4],"I") == 0){
	z = element->z;
    } else {
	if(optargv[4][0] == 'R'){
	    z = Atof(optargv[4]+1) + element->z;
	} else {
	    z = Atof(optargv[4]);
	}
    }
    if(!PositionElement(element, x,y,z)){
	printf("unable to position element '%s'\n",optargv[1]);
    }
    else
	OK();
}

int PositionElement(parent,x,y,z)
Element 	*parent;
float 		x,y,z;
{
Element 	*child;
ElementStack 	*stk;
float		dx,dy,dz;

    if(parent == NULL) return(0);
    /*
    ** position the parent element
    */
    dx = x - parent->x;
    dy = y - parent->y;
    dz = z - parent->z;
    parent->x = x;
    parent->y = y;
    parent->z = z;
    /*
    ** propagate the coordinates down the tree
    */
    stk = NewPutElementStack(parent);
    while((child = NewFastNextElement(2,stk)) != NULL){
	child->x += dx;
	child->y += dy;
	child->z += dz;
    }
    NewFreeElementStack(stk);
    return(1);
}
