static char rcsid[] = "$Id: out_delete.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_delete.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:06:15  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/26 18:03:19  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:22  dhb
** Initial revision
**
*/

#include "out_ext.h"

int DeleteOutput(output)
Output *output;
{

    if(output == NULL){
	return(0);
    }
    /*
    ** free the output data structures
    */
    DeleteOutputData(output);
    /*
    ** free the output
    */
    free(output);
    return(1);
}

int DeleteOutputData(output)
Output *output;
{
struct output_data_type *next;
struct output_data_type *data;

    if(output == NULL){
	return(0);
    }
    /*
    ** free the output data structures
    */
    data=output->data;
    while(data){
	next=data->next;
	free(data);
	data = next;
    }
    output->data = NULL;
    return(1);
}

void do_delete_output_data(argc,argv)
int argc;
char **argv;
{
struct output_type *output;

    initopt(argc, argv, "output-element");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if((output = (Output *)GetElement(optargv[1])) == NULL){
	printf("cant find output '%s'\n",optargv[1]);
	return;
    } else 
    if(!DeleteOutputData(output)){
	printf("unable to delete output data for '%s'\n",optargv[1]);
	return;
    }

    OK();
}

