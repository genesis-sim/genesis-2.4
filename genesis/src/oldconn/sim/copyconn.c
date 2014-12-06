#include "simconn_ext.h"

static char rcsid[] = "$Id: copyconn.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** copy the connections within a tree
**
** $Log: copyconn.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/18 22:39:37  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.2  1994/10/20 22:16:06  dhb
** Moved from sim library to oldconn/sim.
**
*/

void CopyConnections(orig,copy)
Element 	*orig;
Element	*copy;
{
struct equiv_ptype {
	Projection	*orig;
	Projection	*copy;
} *equiv_ptable;
struct equiv_stype {
	Segment		*orig;
	Segment		*copy;
} *equiv_stable;
int 		pcount=0;
int 		scount=0;
int 		i,j;
int 		psize;
int 		ssize;
ElementStack 	*stk0,*stk1;
Element 	*orig_ptr;
Element 	*copy_ptr;
Connection 	*connection;
Connection 	*new_connection;
Segment 	*orig_target;
Segment 	*copy_target;
Projection 	*orig_projection;
Projection 	*copy_projection;
char *CreateObject();

    if((psize = CountChildren(2,orig,PROJECTION_ELEMENT) + 1) == 1) return;
    if((ssize = CountChildren(2,orig,SEGMENT_ELEMENT) + 1) == 1) return;

    equiv_ptable = (struct equiv_ptype *)
	malloc(psize * sizeof(struct equiv_ptype));
    equiv_stable = (struct equiv_stype *)
	malloc(ssize * sizeof(struct equiv_stype));
    /*
    ** the trees should be identical or strange things could happen
    */
    stk0 = NewPutElementStack(orig);
    stk1 = NewPutElementStack(copy);
    /*
    ** check the first element 
    */
    if(CheckClass(orig,SEGMENT_ELEMENT)){
	equiv_stable[0].orig = (Segment *)orig;
	equiv_stable[0].copy = (Segment *)copy;
	scount = 1;
    }
    if(CheckClass(orig,PROJECTION_ELEMENT)){
	equiv_ptable[0].orig = (Projection *)orig;
	equiv_ptable[0].copy = (Projection *)copy;
	pcount = 1;
    }
    /*
    ** construct the element equivalence table
    ** by adding all projections and segments in the tree
    */
    while((orig_ptr = NewFastNextElement(2,stk0)) != NULL){
	copy_ptr = NewFastNextElement(2,stk1);
	if(CheckClass(orig_ptr,SEGMENT_ELEMENT)){
	    equiv_stable[scount].orig = (Segment *)orig_ptr;
	    equiv_stable[scount].copy = (Segment *)copy_ptr;
	    scount++;
	} else
	if(CheckClass(orig_ptr,PROJECTION_ELEMENT)){
	    equiv_ptable[pcount].orig = (Projection *)orig_ptr;
	    equiv_ptable[pcount].copy = (Projection *)copy_ptr;
	    pcount++;
	} 
    }
    NewFreeElementStack(stk0);
    NewFreeElementStack(stk1);
    /*
    ** go through all of the projections in the table and map the connections
    */
    for(i=0;i<pcount;i++){
	/*
	** get the next original projection
	*/
	orig_projection = equiv_ptable[i].orig;
	/*
	** start the copy without any connections 
	*/
	copy_projection = equiv_ptable[i].copy;
	copy_projection->connection = NULL;
	/*
	** and go through all of the original connections
	*/
	for(connection=orig_projection->connection;connection;
	connection=connection->next){
	    /*
	    ** get the destination segment
	    */
	    orig_target = connection->target;
	    /*
	    ** get the equivalent destination segment from the copy
	    ** by searching the equivalence table
	    */
	    for(j=0;j<scount;j++){
		if(equiv_stable[j].orig == orig_target)
		    break;
	    }
	    if(j >= scount){
		/*
		** could not find the destination segment within the 
		** table indicating a non-local connection
		** so forget it
		*/
		break;
	    }
	    /*
	    ** get the equivalent copy destination element  
	    */
	    copy_target = equiv_stable[j].copy;
	    /*
	    ** add the connection
	    */
	    new_connection = 
	    (Connection *)CreateObject(orig_projection->connection_object);
	    /*
	    ** copy the connection information from the original
	    */
	    BCOPY(connection,new_connection, 
	    orig_projection->connection_object->size);
	    /*
	    ** set the copy pointers
	    */
	    new_connection->target = copy_target;
	    new_connection->next = copy_projection->connection;
	    copy_projection->connection = new_connection;
	}
    }
    free(equiv_ptable);
    free(equiv_stable);
}
