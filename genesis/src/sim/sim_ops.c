static char rcsid[] = "$Id: sim_ops.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_ops.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  1998/01/08 23:51:11  dhb
** Changes to support additional random number generator.
**
** Revision 1.4  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3  1995/03/27 19:14:52  dhb
** Added range check for clock number in do_getclock() (getclock command).
**
 * Revision 1.2  1993/02/12  23:17:02  dhb
 * 1.4 to 2.0 command srg changes:
 * 	do_date (date) changed to only do getdate command
 * 	do_srandom (randomseed) changed to take optional seed arg
 * 		provides time based seed if seed not provided
 * 	do_getclock (getclock) same as 1.4 using getopt routines
 * 	do_ps (cpu) unchanged
 *
 * Revision 1.1  1992/10/27  20:22:44  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

float do_getclock(argc,argv)
int argc;
char **argv;
{

    int	clockno;

    initopt(argc, argv, "clock-number");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0.0;
      }

    if (sscanf(optargv[1], "%d", &clockno) != 1 ||
		clockno < 0 || clockno >= NCLOCKS)
      {
	Error();
	printf("%s: bad clock number\n", argv[0]);
	return 0.0;
      }

    return ClockValue(clockno);
}

int do_srandom(argc,argv)
int		argc;
char	**argv;
{
time_t	seed;

    seed = 0;
    initopt(argc, argv, "[seed]");
    if (G_getopt(argc, argv) < 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    if (optargc == 2)
	seed = (time_t)atoi(argv[1]);
    else
	time(&seed);

    G_SEEDRNG(seed);
    return (int)seed;
}

void do_ps() 
{ 
    rtime(stdout); 
}

char *do_date(argc,argv) 
int argc;
char **argv;
{ 
char string[100];
char *ptr;

    strcpy(string,date());
    if ((ptr = strchr(string,'\n'))) {
	*ptr = '\0';
    }
    return(CopyString(string));
}
