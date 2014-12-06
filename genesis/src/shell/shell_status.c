static char rcsid[] = "$Id: shell_status.c,v 1.4 2005/12/16 13:16:59 svitak Exp $";

/*
** $Log: shell_status.c,v $
** Revision 1.4  2005/12/16 13:16:59  svitak
** Removed or relocated "header.h" to keep it from mangling definitions in
** math.h on OSX.
**
** Revision 1.3  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.2  2005/06/20 19:22:59  svitak
** Commented out uneeded headers.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2001/04/01 19:30:47  mhucka
** Small cleanup fixes involving variable inits and printf args.
**
** Revision 1.3  2000/09/21 19:37:47  mhucka
** Changed declaration of clock variables to use time_t.
**
** Revision 1.2  2000/03/27 10:33:07  mhucka
** Commented out statusline functionality, because it caused GENESIS to send
** control character sequences upon exit and thereby screw up the user's
** terminal.  Also added return type declarations for various things.
**
** Revision 1.1  1992/12/11 19:04:46  dhb
** Initial revision
**
*/

#include <time.h>
#include "shell_ext.h"
#include "sim_func_ext.h"	/* memusage() */

static struct tm start_tmval;

void RecordStartTime()
{
    time_t clock;

    time(&clock);
    BCOPY(localtime(&clock),&start_tmval,sizeof(struct tm));
}

void ProcessStatus()
{
double stime,utime;
int umin,smin;
float usec,ssec;
char string[200];
time_t clock;
struct tm tmval;
int totalsec;
int esec,emin,ehour,eday;
int asec,amin;

    rtd(&utime,&stime);
    umin = utime/60;
    usec = utime - umin*60;
    smin = stime/60;
    ssec = stime - smin*60;

    time(&clock);
    BCOPY(localtime(&clock),&tmval,sizeof(struct tm));
    esec = tmval.tm_sec - start_tmval.tm_sec;
    emin = tmval.tm_min - start_tmval.tm_min;
    ehour = tmval.tm_hour - start_tmval.tm_hour;
    eday = tmval.tm_mday - start_tmval.tm_mday;
    if(eday < 0) eday = 1;

    totalsec = esec + emin*60 + ehour*3600 + eday*86400;
    amin = totalsec/60;
    asec = totalsec - amin*60;


    sprintf(string,
#ifdef LONGWORDS
"process status:  %d:%-5.2f user  %d:%-5.2f sys   %d:%02d real  %6.2ld Mbytes",
#else
"process status:  %d:%-5.2f user  %d:%-5.2f sys   %d:%02d real  %6.2d Mbytes",
#endif
    umin, usec,
    smin,ssec,
    amin,asec,
    memusage()/(size_t)1000000);
#ifdef STATUSLINE
    if (!StatusMessage(string, 0, 1)) {
	printf("\n%s\n",string);
    }
#else
    printf("\n%s\n",string);
#endif /* STATUSLINE */
}

