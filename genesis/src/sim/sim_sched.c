static char rcsid[] = "$Id: sim_sched.c,v 1.2 2005/06/27 19:01:09 svitak Exp $";

/*
** $Log: sim_sched.c,v $
** Revision 1.2  2005/06/27 19:01:09  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/02/17 22:40:53  dhb
** 1.4 to 2.0 command argument changes.
**
** 	do_add_schedule (addschedule), do_list_tasks (sched), do_clear_schedule
** 	(clearschedule) and Reschedule (resched) not changed.  All but
** 	do_add_schedule take not arguments.  do_add_schedule takes arguments
** 	specific to the task-function given, which do_add_schedule has no way
** 	of knowing about.
**
** Fixed missing return value in do_add_schedule.
**
** Fixed incorrect data type for rcsid variable.
**
 * Revision 1.1  1992/10/27  20:27:17  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#define MAXTASKS		100

Schedule 	schedule[MAXTASKS];
Schedule 	*working_schedule[MAXTASKS];

static int	max_tasks = 0;
static int	working_tasks = 0;

Schedule *GetSchedule()
{
    return(schedule);
}

int GetMaxTasks()
{
    return(max_tasks);
}


int WorkingTasks()
{
    return(working_tasks);
}

Schedule *GetWorkingTask(i)
int i;
{
    if(i < working_tasks){
	return(working_schedule[i]);
    } else {
	return(NULL);
    }
}


void InitSchedule()
{
    max_tasks = 0;
    working_tasks = 0;
    BZERO(schedule,MAXTASKS*sizeof(Schedule));
}

void ExecuteTasks()
{
int		i;

    /*
    ** check the clocks to see which are active on this cycle
    */
    CheckClocks();
    /*
    ** go through the scheduling table 
    */
    for(i=0;i<working_tasks;i++){
	working_schedule[i]->function(working_schedule[i]);
    }
}

void ClearWorkingSchedule()
{
    working_tasks = 0;
}

void do_clear_schedule()
{
    max_tasks = 0;
    working_tasks = 0;
}

int do_add_schedule(argc,argv)
int	argc;
char	**argv;
{
char		*taskname;
char		buildname[100];
PFI		taskfunc;
PFI		buildfunc;
Schedule	*task;
int		i;

    if(argc < 2){
	printf("usage: %s task_function [args ...]\n",argv[0]);
	return(0);
    }
    /*
    ** read in the task specifications and construct the table
    */
    task = schedule + max_tasks;
    taskname = argv[1];
    /*
    ** look for the BUILD function for the task
    */
    sprintf(buildname,"BUILD_%s",taskname);
    if ((buildfunc = GetFuncAddress(buildname))) {
	buildfunc(argc-1,argv+1,task);
    } else {
	/*
	** if none then just use the arguments as they are
	** if there were previous values then free them
	*/
	if(task->argc){
	    for(i=0;i<task->argc;i++){
		free(task->argv[i]);
	    }
	    free(task->argv);
	}
	task->argc = argc-1;
	task->argv = CopyArgv(argc-1,argv+1);
    }
    if((taskfunc = GetFuncAddress(taskname)) == NULL){
	printf("could not find task function '%s'\n",taskname);
	return(0);
    }
    task->function = taskfunc;
    /*
    ** make a copy of the line for listing purposes
    ** if there were previous values then free them
    */
    if(task->spec_argc > 0){
	for(i=0;i<task->spec_argc;i++){
	    free(task->spec_argv[i]);
	}
	free(task->spec_argv);
    }
    task->spec_argc = argc-1;
    task->spec_argv = CopyArgv(argc-1,argv+1);
    max_tasks++;
    return(1);
}

void Reschedule()
{
char		*taskname;
char		buildname[100];
PFI		buildfunc;
Schedule	*task;
int		i;

    /*
    ** read in the task specifications and construct the table
    */
    working_tasks = 0;
    for(i=0;i<max_tasks;i++){
	task = schedule + i;
	taskname = task->spec_argv[0];
	/*
	** look for the BUILD function for the task
	*/
	sprintf(buildname,"BUILD_%s",taskname);
	if ((buildfunc = GetFuncAddress(buildname))) {
	    if(buildfunc(task->spec_argc,task->spec_argv,task)){
		working_schedule[working_tasks++] = task;
	    }
	}
    }
}

void do_list_tasks()
{
int		i;
int		j;
Schedule	*task;

    /*
    ** go through the scheduling table 
    */
    printf("\nWORKING SIMULATION SCHEDULE\n\n");
    for(i=0;i<working_tasks;i++){
	printf("[%d] ",i+1);
	task = GetWorkingTask(i);
	for(j=0;j<task->spec_argc;j++){
	    printf("%s\t",task->spec_argv[j]);
	}
	printf("\n");
    }
    printf("\n");
}
