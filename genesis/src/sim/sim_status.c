static char rcsid[] = "$Id: sim_status.c,v 1.2 2005/06/27 19:01:11 svitak Exp $";

/*
** $Log: sim_status.c,v $
** Revision 1.2  2005/06/27 19:01:11  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  2000/03/27 10:34:44  mhucka
** Commented out statusline functionality, because it caused GENESIS to
** print garbage control characters upon exit, screwing up the user's
** terminal.  Also added return type declaractions for various things.
**
** Revision 1.5  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1993/03/10 23:11:35  dhb
** Extended element fields
**
 * Revision 1.3  1993/02/15  21:26:34  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_status	broken into two functions do_getstatus and
 * 			do_showstatus.  Each function uses the GENESIS
 * 			getopt routines.
 *
 * Revision 1.2  1992/12/18  21:12:23  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  20:32:10  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define NCLASSES	100
static int classcount;

typedef struct status_type {
    char	*name;
    int		count;
    int		size;
} Status;

Status class_status[NCLASSES];
Status object_status[NCLASSES];

Status *AddObjectStatus(name)
char	*name;
{
Status *status;

    if(classcount < NCLASSES){
	status = object_status+classcount;
	status->name = name;
	status->size = 0;
	status->count = 0;
	classcount++;
	return(status);
    } else {
	return(NULL);
    }
}

Status *GetObjectStatus(name)
char *name;
{
int i;

    for(i=0;i<classcount;i++){
	if(object_status[i].name && (strcmp(object_status[i].name,name) == 0)){
	    return(object_status + i);
	}
    }
    return(NULL);
}

void PrintStatus(name,count,size)
char *name;
int count;
int size;
{
	printf("%10d %-15s %10d bytes (%6.2f Mbytes)\n",
	count,name,size,size/1.0e6);
}

void element_count(root)
Element	*root;
{
char 		*class;
Element 	*element;
Buffer		*buffer;
Projection 	*projection;
Connection 	*connection;
int 		csize;
short 		stk;
Status		*status;
int		total_size;
int		total_count;
int		i;

    if(root == NULL) return;
    stk = PutElementStack(root);
    /*
    ** clean out all of the status values
    */
    classcount = 0;
    while((element = NextElement(0,0,stk)) != NULL){
	if(element->object == NULL) continue;
	class = element->object->name;
	/*
	** look for the status structure of the element class
	*/
	if((status = GetObjectStatus(class)) == NULL){
	    /*
	    ** if not found then make one
	    */
	    if((status = AddObjectStatus(class)) == NULL){
		printf("status table full\n");
		continue;
	    }
	}
	/*
	** update the statistics of the class
	*/
	(status->count)++;
	status->size += Size(element);

	/*
	** add memory usage for extended fields
	*/
	status->size += MemUsageExtFields(element);

	/*
	** handle the messages
	*/
	if((status = GetObjectStatus("messages")) == NULL){
	    /*
	    ** if not found then make one
	    */
	    if((status = AddObjectStatus("messages")) == NULL){
		printf("status table full\n");
		continue;
	    }
	}

	/*
	** add stats for msg out
	*/

	status->count += element->nmsgout;
	status->size += element->nmsgout*sizeof(MsgOut);

	/*
	** add stats for msg in
	*/

	status->size += MsgInMemReq(element);

	/*
	** deal with the special cases
	*/
	if(CheckClass(element,PROJECTION_ELEMENT)){
	    projection = (Projection *)element;
	    if(projection->connection_object){
		if((status=
		GetObjectStatus(projection->connection_object->name))==NULL){
		    if((status=
		    AddObjectStatus(projection->connection_object->name))==NULL){
			printf("status table full\n");
			continue;
		    }
		}
		csize = projection->connection_object->size;
	    } else {
		csize = 0;
	    }
	    for(connection=projection->connection;connection;
	    connection=connection->next){
		(status->count)++;
		status->size += csize;
	    }
	} else
	if(CheckClass(element,BUFFER_ELEMENT)){
	    buffer = (Buffer *)element;
	    if((status=GetObjectStatus("events"))==NULL){
		if((status = AddObjectStatus("events")) == NULL){
		    printf("status table full\n");
		    continue;
		}
	    }
	    status->count += buffer->size;
	    status->size += buffer->size*buffer->event_size;
	}
    }
    FreeElementStack(stk);
    /*
    ** go through all of the classes and print the results
    */
    printf("'%s' element count:\n",Pathname(root));
    total_size = 0;
    total_count = 0;
    for(i=0;i<classcount;i++){
	status = object_status+i;
	PrintStatus(status->name,status->count,status->size);
	total_size += status->size;
	total_count += status->count;
    }
    printf("----------------------------------------------------------\n");
    PrintStatus("",total_count,total_size);
    printf("\n");
#ifdef LONGWORDS
    printf("total memory usage : %ld bytes   ( %6.2f Mbytes )\n",
#else
    printf("total memory usage : %d bytes   ( %6.2f Mbytes )\n",
#endif
    memusage(),
    (double)memusage()/1000000.0);
}

float do_getstatus(argc,argv)
int argc;
char **argv;
{
int	status;

    initopt(argc, argv, "-time -step -memory");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if(strcmp(G_optopt,"-time") == 0){
	    return((float) SimulationTime());
	}
	if(strcmp(G_optopt,"-step") == 0){
	    return((float) GetCurrentStep());
	}
	if(strcmp(G_optopt,"-memory") == 0){
	    return((float) memusage());
	}
      }

    printoptusage(argc, argv);
    return(0.0);
}


void do_showstatus(argc,argv)
int argc;
char **argv;
{
Element *element;
int	status;

    initopt(argc, argv, "-process -element [element-name]");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if(strcmp(G_optopt,"-process") == 0){
	    ProcessStatus();
	    return;
	}
	if(strcmp(G_optopt,"-element") == 0){
	    if(optargc > 1){
		element = GetElement(optargv[1]);
	    } else {
		element = RootElement();
	    }
	    element_count(element);
	    return;
	}
    }

    if (status < 0)
	printoptusage(argc, argv);
    else
	SimStatus();
}

void SimStatus()
{
char string[100];

    sprintf(string,"current simulation time = %f ; step = %d; dt = %e         ",
    SimulationTime(),
    GetCurrentStep(),
    ClockValue(0));
#ifdef STATUSLINE
    if (!StatusMessage(string, 0, 1)) {
        printf("%s\n", string);
    }
#else
    printf("%s\n", string);
#endif /* STATUSLINE */
}
