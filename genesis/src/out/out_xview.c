static char rcsid[] = "$Id: out_xview.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: out_xview.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:28  dhb
** Initial revision
**
*/

#include "sim_ext.h"
#include "out_struct.h"

typedef struct coord_type {
    float	x,y,z;
} XYZCoord;

ResetXview(output)
struct oldxview_type *output;
{
struct output_data_type *data;
int ndata;
int i;
XYZCoord *coords;
    /*
    ** how many points?
    */
    ndata = CountOutputData(output->data);
    /*
    ** if things have changed then do the setup
    */
    if(ndata != output->ndata){
	/*
	** update the ndata count
	*/
	output->ndata = ndata;
	/*
	** allocate the coord array
	*/
	coords = (XYZCoord *) calloc(ndata,sizeof(XYZCoord));
	/*
	** fill the coord array
	*/
	for(i=0,data = output->data;data;data=data->next,i++){
	    coords[i].x = data->x;
	    coords[i].y = data->y;	
	}
	/*
	** if the color is set to background then
	** set the default color to 1
	*/
	if(output->color == 0)
	    output->color = 1;
	/*
	** send the coords to the xview widget
	x_view_coords(output->name,coords,ndata);
	*/
    }
}

XviewOutput(output,action)
struct oldxview_type *output;
Action		*action;
{
struct output_data_type *data;
PFI	func;
float y;
float x;
char *itoa();
int i;
int plotnum;
float *xview_data;

    switch(action->type){
    case RESET:
	ResetXview(output);
	break;
    case PROCESS:
	if(output->ndata == 0){
	    ResetXview(output);
	}
	xview_data = (float *) 
	calloc(output->ndata,sizeof(float));
	/*
	** fill in the data array
	*/
	for(i=0,data = output->data;data;data=data->next,i++){
	    switch(data->datatype){
	    case FLOAT :
		y = (float)(*((float *)(data->dataptr)));
		break;
	    case DOUBLE :
		y = (float)(*((double *)(data->dataptr)));
		break;
	    case INT :
		y = (float)(*((int *)(data->dataptr)));
		break;
	    case SHORT :
		y = (float)(*((short *)(data->dataptr)));
		break;
	    case CHAR :
		y = (float)(*((char *)(data->dataptr)));
		break;
	    default :
		printf("unknown type\n");
		break;
	    }
	    xview_data[i] = y;
	}
	/*
	** send out the data to be displayed

	x_view_data(output->name,
	itoa(output->plotnum),
	xview_data,
	output->ndata,
	output->display_mode,
	output->color
	);
	*/
	break;
    default:
	printf("'%s' cannot handle action '%s'.\n",
	Pathname(output),
	Actionname(action));
	break;
    }
}

