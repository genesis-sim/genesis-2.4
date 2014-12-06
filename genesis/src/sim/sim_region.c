static char rcsid[] = "$Id: sim_region.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_region.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:25:14  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"



FreeRegionMask(region_mask)
struct region_mask_type	*region_mask;
{
int	xmin,xmax;
int	i;

    xmin = region_mask->xmin;
    xmax = region_mask->xmax;

    /*
    * free space from the region mask
    */
    for(i=0;i<xmax-xmin+1;i++){
	    free(region_mask->map[i]);
    }
    free(region_mask);
}

struct region_mask_type *GetRegionMask(count,region)
short	count;
struct region_type *region;
{
	struct	region_mask_type *region_mask;
	char	**map;
	int		xmin,xmax,ymin,ymax;
	int		xsize,ysize;
	int		x,y;
	short	i;
	short	type;

	/*
	* determine the maximum bounds of the region mask
	*/
	xmax = ymax = -HUGE;
	xmin = ymin = HUGE;
	for(i=0;i<count;i++){
		xmax = MAX(region[i].xh, xmax);
		ymax = MAX(region[i].yh, ymax);
		xmin = MIN(region[i].xl, xmin);
		ymin = MIN(region[i].yl, ymin);
	}
	xsize = xmax-xmin+1;
	ysize = ymax-ymin+1;
	/*
	* allocate space for the region mask
	*/
	map = (char **) malloc(xsize*sizeof(char*));
	for(i=0;i<xsize;i++){
		map[i] = (char *)calloc(ysize,sizeof(char));
	}
	/*
	* fill the region mask
	*/
	for(i=0;i<count;i++){
		type = (region[i].type > 0);
		for(x=region[i].xl;x<=region[i].xh;x++){
			for(y=region[i].yl;y<=region[i].yh;y++){
				map[x-xmin][y-ymin] = type;
			}
		}
	}

	region_mask = (struct region_mask_type*)malloc(sizeof(struct region_mask_type));
	region_mask->xmin = xmin;
	region_mask->xmax = xmax;
	region_mask->ymin = ymin;
	region_mask->ymax = ymax;
	region_mask->map = map;
	return(region_mask);
}

GetRegionDefinition(projection,region,nregions)
short	projection;
struct region_type *region;
short	nregions;
{
short	i;

	for(i=0;i<nregions;i++){
		define_region(&region[i],
		(short)GetVarNumData(array("region",projection),i*5+1),
		(short)GetVarNumData(array("region",projection),i*5+2),
		(short)GetVarNumData(array("region",projection),i*5+3),
		(short)GetVarNumData(array("region",projection),i*5+4),
		(short)GetVarNumData(array("region",projection),i*5+5));
	}
}

