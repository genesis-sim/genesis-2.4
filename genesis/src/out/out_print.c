static char rcsid[] = "$Id: out_print.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_print.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:26  dhb
** Initial revision
**
*/

#include "sim_ext.h"
#include "out_struct.h"

void PrintOutput(output,action)
Output 		*output;
Action		*action;
{
struct output_data_type *data;
PFI		func;

    switch(action->type){
	case PROCESS:
	printf("output '%s'\n", Pathname(output));
	for(data = output->data;data;data=data->next){
	    func=data->data_func;
	    printf("element '%s' : value ",
	    Pathname(data->element));
	    switch(data->datatype){
		case FLOAT :
		    if(func){
			printf("%f\n",(float)(*((float *)(func(data)))));
		    } else
			printf("%f\n",(float)(*((float *)(data->dataptr))));
		    break;
		case DOUBLE :
		    if(func){
			printf("%f\n",(float)(*((double *)(func(data)))));
		    } else
			printf("%f\n",(float)(*((double *)(data->dataptr))));
		    break;
		case INT :
		    if(func){
			printf("%d\n",(int)(*((int *)(func(data)))));
		    } else
			printf("%d\n",(int)(*((int *)(data->dataptr))));
		    break;
		case SHORT :
		    if(func){
			printf("%d\n",(int)(*((short *)(func(data)))));
		    } else
			printf("%d\n",(int)(*((short *)(data->dataptr))));
		    break;
		case CHAR :
		    if(func){
			printf("%d\n",(int)(*((char *)(func(data)))));
		    } else
			printf("%d\n",(int)(*((char *)(data->dataptr))));
		    break;
		default :
		    printf("unknown type\n");
		    break;
	    }
	}
	break;
    default:
	printf("'%s' cannot handle action '%s'.\n",
	Pathname(output),
	Actionname(action));
	break;
    }
}
