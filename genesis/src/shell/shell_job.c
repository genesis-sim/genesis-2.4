static char rcsid[] = "$Id: shell_job.c,v 1.3 2005/07/01 10:03:08 svitak Exp $";

/*
** $Log: shell_job.c,v $
** Revision 1.3  2005/07/01 10:03:08  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.10  2001/04/01 19:30:47  mhucka
** Small cleanup fixes involving variable inits and printf args.
**
** Revision 1.9  2000/09/21 19:38:49  mhucka
** Added long specifiers where needed in printf format strings.
**
** Revision 1.8  2000/06/12 04:50:16  mhucka
** Rearranged #include statements, and added a typecast in do_list_jobs().
**
** Revision 1.7  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.6  1995/02/17 22:05:56  dhb
** Added AddCleanup(), RemoveCleanup() and ExecuteCleanups() to provide
** callback ability for clean up code.
**
** Fixed error in return value of AddJob() to correctly return the job
** index for a reallocated job slot.
**
** Revision 1.5  1994/10/25  23:57:29  dhb
** Added support for -notty mode of non-interactive use.  The iofunc()
** function is not added as a job if -notty is in effect.  Also, no
** terminal handling is set up.
**
** The setting up of iofunc() as a job was moved from shell_job.c to
** shell_tty.c where is belongs.
**
** Revision 1.4  1993/10/14  22:52:04  dhb
** Added a check on AvailableCharacters() to determine if we should call
** Sleep() in ExecuteJobs().  Improves interactive response time.
**
** Revision 1.3  1993/10/14  22:20:41  dhb
** Added use of DONT_USE_SIGIO define to conditionally compile adding
** the iofunc job when SIGIO is not used.
**
** Revision 1.2  1993/02/23  22:40:40  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:34  dhb
** Initial revision
**
*/

/* mds3 changes */
/*
** SIGIO(BSD) equivalent to SIGPOLL(SYSV)
*/
#ifdef SYSV
#include <sys/param.h>
#endif

#include "shell_ext.h"
#include <sys/time.h>
#include <signal.h>

#define MAXJOBS			100

void Usleep(usec)
int usec;
{
struct timeval	timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = usec;
    select(32,NULL,NULL,NULL,&timeout);
}

static Job 	job[MAXJOBS];
static int	max_jobs = 0;
static int	grab_cpu = 0;

static Job 	cleanup[MAXJOBS];
static int	max_cleanups = 0;

void Sleep()
{
#ifdef MASSCOMP
    /*
    ** sleep for .1 second
    ** WARNING may affect priority sensitive functions
    */
    astpause(0,100);
#else
    /*
    ** sleep for .1 sec
    */
/* mds3 changes */
#ifdef i860
#else
    Usleep(100000);
#endif
#endif
}

void InitJobs()
{
    max_jobs = 0;
    BZERO(job,MAXJOBS*sizeof(Job));
    max_cleanups = 0;
    BZERO(cleanup,MAXJOBS*sizeof(Job));
}

void ExecuteCleanups()
{
  int	i;

    /*
    ** scan through all the cleanup jobs
    */
  for(i=0;i<max_cleanups;i++){
	/*
	** execute the cleanup job function
	*/
    if(cleanup[i].function != NULL){
      cleanup[i].function();
    }
  }
}

int AddCleanup(func)
PFI	func;
{
int i;

    if(func == NULL) return(-1);
    /*
    ** find the first open cleanup job slot
    */
    for(i=0;i<max_cleanups;i++){
	if(cleanup[i].function == NULL) break;
    }
    if(i < MAXJOBS){
	cleanup[i].function = func;
	cleanup[i].priority = 0;
	if(i >= max_cleanups) max_cleanups++;
    }
    return(i);
}

void RemoveCleanup(i)
int	i;
{
    /*
    ** remove it
    */
    if(i < max_cleanups && i >= 0 ){
	BZERO(cleanup+i,sizeof(Job));
	if(i == max_cleanups-1) max_cleanups--;
    }
}

void ExecuteJobs()
{
  int	i;

    /*
    ** scan through all the jobs
    */
  for(i=0;i<max_jobs;i++){
	/*
	** execute the job function
	*/
    if(job[i].function && ((job[i].count++ % (job[i].priority + 1)) == 0)){
      job[i].function();
    }
  }
    /*
    ** if there is no job with maximum priority then
    ** sleep to avoid pounding on the cpu
    */
    if(!grab_cpu && AvailableCharacters() == 0) Sleep();
}

int AddJob(func,priority)
PFI	func;
int	priority;
{
int i;

    if(func == NULL) return(-1);
    if(priority < 0) priority = 0;
    /*
    ** find the first open job slot
    */
    for(i=0;i<max_jobs;i++){
	if(job[i].function == NULL) break;
    }
    if(i < MAXJOBS){
	job[i].function = func;
	job[i].priority = priority;
	if(i >= max_jobs) max_jobs++;
    }
    if(priority == 0) grab_cpu = 1;
    return(i);
}

void RemoveJob(i)
int	i;
{
int j;

    /*
    ** remove it
    */
    if(i < max_jobs && i >= 0 ){
	BZERO(job+i,sizeof(Job));
	if(i == max_jobs-1) max_jobs--;
    }
    /*
    ** recheck the peak priorities
    */
    grab_cpu = 0;
    for(j=0;j<max_jobs;j++){
	if(job[j].function != NULL && job[j].priority == 0) grab_cpu = 1;
    }
}

void do_add_job(argc,argv)
int	argc;
char	**argv;
{
int priority = 0;

    initopt(argc, argv, "function [priority]");
    if (G_getopt(argc, argv) != 0 || optargc > 3)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc == 3){
	priority = atoi(optargv[2]);
    }

    if(AddJob(GetFuncAddress(optargv[1]),priority) == -1){
	printf("%s unable to add job %s\n",optargv[0],optargv[1]);
    }
}

void do_remove_job(argc,argv)
int	argc;
char	**argv;
{
    initopt(argc, argv, "job-number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    RemoveJob(atoi(optargv[1]));
}

void do_change_priority(argc,argv)
int	argc;
char	**argv;
{
int	j,n,priority;

    initopt(argc, argv, "job-number priority");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    n = atoi(optargv[1]);
    priority = atoi(optargv[2]);
    if(n < max_jobs){
	printf("old priority = %d. new priority = %d\n",
	job[n].priority,
	priority);
	job[n].priority = priority;
	/*
	** reevaluate the peak priority
	*/
	grab_cpu = 0;
	for(j=0;j<max_jobs;j++){
	    if(job[j].priority == 0) grab_cpu = 1;
	}
    } else {
	printf("no such job [%d]\n",n);
    }
}

void do_list_jobs()
{
int	i;

    /*
    ** scan through all the jobs
    */
    printf("\nSimulator Jobs\n\n");
    if(max_jobs == 0){
	printf("----- none ----\n");
    }
    for(i=0;i<max_jobs;i++){
	/*
	** print the function name
	*/
	if(job[i].function){
	    printf("[%d] %-20s priority %d\n",
	    i, GetFuncName(job[i].function), job[i].priority);
	}
    }
    if(grab_cpu){
	printf("\ncpu ACTIVE\n");
    } else {
	printf("\ncpu SLEEPING\n");
    }
    printf("\n");
}

