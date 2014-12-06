static char rcsid[] = "$Id: sim_step.c,v 1.2 2005/06/27 19:01:12 svitak Exp $";

/*
** $Log: sim_step.c,v $
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2001/07/22 17:51:47  mhucka
** Backed out stupid change I made in 1.8 to 1.9.
**
** Revision 1.10  2001/06/29 21:29:35  mhucka
** Fixed type compatbility between printf format string and args.
**
** Revision 1.9  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2000/09/21 19:38:58  mhucka
** Added long specifiers where needed in printf format strings.
**
** Revision 1.7  2000/09/07 05:17:40  mhucka
** Moved definition of simulation_time from sim.h to here.
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
** Revision 1.4  1994/04/11 17:27:57  dhb
** Fixed do_simulate() incorrect call to Atof() duting option processing.
**
 * Revision 1.3  1993/02/12  23:22:58  dhb
 * changed do_simulate (step) to use the command arg for number of steps
 * or time to simulate (for the -time option).  -time no longer take an
 * argument.
 *
 * Revision 1.2  1993/02/12  22:38:52  dhb
 * 1.4 to 2.0 arguments for:
 * 	do_simulate (step command)
 *
 * Removed do_bgsimulate (bgstep) since do_simulate has the -background
 * option.
 *
 * Revision 1.1  1992/10/27  20:32:22  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <sys/time.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

static int 	nsteps;
static int 	stepcount;
static int 	verbose;
static float	start_time;
static float	duration;
static double 	utime1,stime1;
static int	job = -1;
static int	current_step = 0;
static int 	break_flag = 0;

double simulation_time;

void SetBreakFlag() { break_flag = 1; }
int GetCurrentStep(){ return(current_step); }


void StepStatus()
{
     static char string[100];

     sprintf(string,"time = %f ; step = %d          ",
	     simulation_time,
	     current_step);
     /*
     ** try printing to the status line
     */
     if (IsSilent() < 2) {
#ifdef STATUSLINE
         if (!StatusMessage(string, 30, 0)) {
	     printf("%s\n", string);
         }
#else
       printf("%s\n", string);
#endif /* STATUSLINE */
     }
}

void SetCurrentStep(val)
int val;
{
    current_step = val;
}

void StepSimulation()
{
    ExecuteTasks();
    stepcount++;
    current_step++;
    nsteps--;
    simulation_time += ClockValue(0);
    if(verbose && IsSilent()<1){
	StepStatus();
    }
    if(break_flag || nsteps <= 0){
	/*
	** when completed
	*/
	RemoveJob(job);
	job = -1;
	ReportStatus();
    }
}

void TStepSimulation()
{
    ExecuteTasks();
    stepcount++;
    current_step++;
    simulation_time += ClockValue(0);
    if(verbose && IsSilent()<1){
	StepStatus();
    }
    if(break_flag || simulation_time - start_time >= duration){
	/*
	** when completed
	*/
	RemoveJob(job);
	job = -1;
	ReportStatus();
    }
}

void RemoveSimulationJob()
{
    RemoveJob(job);
    job = -1;
}

void ActivateStep(mode,start,dur,n,v)
int	mode;
float	start;
float	dur;
int	n;
int	v;
{
    /* 
    ** reset the break flag 
    */
    break_flag = FALSE;
    stepcount=0;

    start_time = start;
    nsteps = n;
    verbose = v;
    duration = dur;

/* mds3 changes */
/*
**    printf("Entered ActivateStep()\n");
**    fflush (stdout);
*/
    rtd(&utime1,&stime1);

    switch(mode){
    case 0:
	job = AddJob(StepSimulation,0);
	break;
    case 1:
	job = AddJob(TStepSimulation,0);
	break;
    }
/*
**    printf("Leaving ActivateStep()\n");
**    fflush (stdout);
*/
}

void ReportStatus()
{
double utime2,stime2;

    rtd(&utime2,&stime2);
    if(IsSilent()<1){
	StepStatus();
	printf("completed %d steps in %f cpu seconds\n",
	stepcount,
	utime2-utime1);
    }
}



/*
** advance the simulation
*/

int do_simulate(argc,argv)
int	argc;
char	**argv;
{
int		verbose = 0;
int		mode = 0;
float		duration = simulation_time;
int		nsteps = 1;
int		bg = 0;
int		status;

    if(job >= 0){
	printf("simulation is already in progress [%d]\n",job);
	return(0);
    }

    initopt(argc, argv, "[steps-or-time] -time -verbose -background");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-time") == 0)
	    mode = 1;
	else if (strcmp(G_optopt, "-verbose") == 0)
	    verbose = 1;
	else if (strcmp(G_optopt, "-background") == 0)
	    bg = 1;
      }

    if (status < 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return 0;
      }

    if (optargc == 2)
      {
	if (mode == 0)
	    nsteps = atoi(optargv[1]);
	else if (mode == 1)
	    duration = Atof(optargv[1]);
      }

    ActivateStep(mode,simulation_time,duration,nsteps,verbose);
    if(!bg){
	/*
	** do the simulation on the spot
	*/
	while(job != -1){
	    ExecuteJobs();
	}
    }
    return(1);
}
