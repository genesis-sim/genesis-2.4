static char rcsid[] = "$Id: pc_init.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: pc_init.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:07  dhb
** Initial revision
**
*/

#include "seg_ext.h"

#ifdef LATER
    if(GetVarNumData("edge_compensate",0)){
	short	projection;
	short	i;
	short	dst_seg;
	struct	projectionway_type *projection_ptr;
	struct	population_type *dst_pop_ptr;
	struct	connection_type *connection_ptr;
	int		cellnum,src_cell;
	int		nentries;
	float	factor;
	static	already_done = 0;

	if(!already_done){
	    already_done = 1;
	    for(projection = 0;projection<nprojections;projection++){
		if(Projectionway[projection].src_pop == GetVarNumData("fbinhib",0)){
		    projection_ptr = &(Projectionway[projection]);
		    dst_seg = projection_ptr->dst_seg;
		    dst_pop_ptr = &(Population[projection_ptr->dst_pop]);
		    cellnum = Population[projection_ptr->src_pop].cellnum;
		    factor = GetVarNumData("neighbors",0)*2 +1;
		    factor *= factor;
		    for(src_cell = 0;src_cell < cellnum;src_cell++){
			nentries = projection_ptr->connection_nentries[src_cell];
			for(i=0;i<nentries;i++){
			    connection_ptr = &(projection_ptr->connection[src_cell][i]);
			    connection_ptr->weight *=
			    factor/dst_pop_ptr->element[connection_ptr->n].segment[dst_seg].nsynapses;
			}
		    }
		}
	    }
	}
    }
#endif
