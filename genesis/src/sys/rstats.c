static char rcsid[] = "$Id: rstats.c,v 1.3 2005/10/24 06:30:30 svitak Exp $";

/*
** $Log: rstats.c,v $
** Revision 1.3  2005/10/24 06:30:30  svitak
** Included <unistd.h> for Solaris.
**
** Revision 1.2  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2001/07/22 17:51:47  mhucka
** Backed out stupid change I made in 1.8 to 1.9.
**
** Revision 1.10  2001/06/29 21:30:38  mhucka
** Fixed type compatbility between printf format string and args.
**
** Revision 1.9  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2000/07/12 06:18:07  mhucka
** Soem of the format directives in rstats() should have been long int instead
** of just plain int.
**
** Revision 1.7  1999/08/22 04:42:15  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.6  1997/07/18 20:05:56  dhb
** Changes from PSC: support for CRAY in rtd()
**
** Revision 1.5  1996/05/23  23:17:48  dhb
** t3d/e port
**
** Revision 1.4  1995/06/16  06:02:21  dhb
** FreeBSD compatibility.
**
** Revision 1.3  1993/10/15  22:21:56  dhb
** Fixed bugs which left open file descriptors after use.
**
** Revision 1.2  1993/09/17  20:38:59  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:05:34  dhb
** Initial revision
**
*/

#include <sys/time.h>
#ifdef __FreeBSD__
#include <sys/types.h>
#endif
#include <sys/resource.h>
#include <stdio.h>

#ifdef Solaris /* C King 8/93 */
#include <unistd.h>	/* getpid, ioctl, close */
#include <fcntl.h>
#include <sys/procfs.h>

#else

/* 1999-07-16 <mhucka@bbb.caltech.edu> 
 * Avoid redefinition warnings in Red Hat Linux 6.0.
 */
#ifndef RUSAGE_SELF
#define RUSAGE_SELF	0
#define RUSAGE_CHILDREN -1
#endif

#ifndef CRAY
struct rusage  rbuff;
#endif

/* mds3 changes */
#ifdef i860
#define getrusage(a,b) null_proc(a,b)
null_proc(who, rusage)
     int who;
     struct rusage *rusage;
{
}
#endif

#ifdef __hpux
/*
** Thanks to Bill Broadley (broadley@neurocog.lrdc.pitt.edu) for this
** bit of code.
*/

#include <syscall.h>

int getrusage(who, rusage)

int		who;
struct rusage	*rusage;

{
	return (syscall(SYS_getrusage, who, rusage));
}
#endif

#endif /* Solaris */

void rstats (fp) 
FILE *fp;
{
#ifndef CRAY
#ifdef Solaris
/* Thanks to Jordan Hayes for this bit. CKing 9/93 */
        int             proc;
        char            buf[100];
        prusage_t       r;

        (void)sprintf(buf, "/proc/%d", getpid());
        if ((proc = open(buf, O_RDONLY, 0)) < 0) {
                fprintf(stderr, "open('%s'):", buf);
                perror("");
        } else {
	    if (ioctl(proc, PIOCUSAGE, (void *)&r) < 0)
                perror("ioctl(PIOCUSAGE)");
	    close(proc);
	}

      fprintf (fp,"user time used\t\t= %ld sec  %ld nsec\n", r.pr_utime.tv_sec, r.pr_utime.tv_nsec);
      fprintf (fp,"system time used\t= %ld sec  %ld nsec\n", r.pr_stime.tv_sec, r.pr_stime.tv_nsec);
      fprintf (fp,"page reclaims\t\t= %d\n", r.pr_minf);
      fprintf (fp,"page faults\t\t= %d\n", r.pr_majf);
      fprintf (fp,"swaps\t\t\t= %d\n", r.pr_nswap);
      fprintf (fp,"block inputs\t\t= %d\n", r.pr_inblk);
      fprintf (fp,"block outputs\t\t= %d\n", r.pr_oublk);
      fprintf (fp,"messages sent\t\t= %d\n", r.pr_msnd);
      fprintf (fp,"messages received\t= %d\n", r.pr_mrcv);
      fprintf (fp,"signals received\t= %d\n", r.pr_sigs);
      fprintf (fp,"voluntary context switches\t= %d\n", r.pr_vctx);
      fprintf (fp,"involuntary context switches\t= %d\n", r.pr_ictx);
      fprintf (fp,"system calls\t\t= %d\n",r.pr_sysc);
      fprintf (fp,"chars read and written\t= %d\n", r.pr_ioch);
#else
    getrusage (RUSAGE_SELF, &rbuff);
    fprintf (fp,"user time used\t\t= %ld sec  %ld usec\n", (long)rbuff.ru_utime.tv_sec, rbuff.ru_utime.tv_usec);
    fprintf (fp,"system time used\t= %ld sec  %ld usec\n", (long)rbuff.ru_stime.tv_sec, rbuff.ru_stime.tv_usec);
    fprintf (fp,"max set size in Kbytes \t= %ld\n", rbuff.ru_maxrss);
    fprintf (fp,"shared memory size\t= %ld\n", rbuff.ru_ixrss);
    fprintf (fp,"unshared data size\t= %ld\n", rbuff.ru_ixrss);
    fprintf (fp,"shared stack size\t= %ld\n", rbuff.ru_ixrss);
    fprintf (fp,"page reclaims\t\t= %ld\n", rbuff.ru_minflt);
    fprintf (fp,"page faults\t\t= %ld\n", rbuff.ru_majflt);
    fprintf (fp,"swaps\t\t\t= %ld\n", rbuff.ru_nswap);
    fprintf (fp,"block inputs\t\t= %ld\n", rbuff.ru_inblock);
    fprintf (fp,"block outputs\t\t= %ld\n", rbuff.ru_oublock);
    fprintf (fp,"voluntary switches\t= %ld\n", rbuff.ru_nvcsw);
    fprintf (fp,"involuntary switches\t= %ld\n", rbuff.ru_nivcsw);
#endif
#endif
}

void rtime (fp) 
FILE *fp;
{
#ifndef CRAY
#ifdef Solaris
/* Thanks to Jordan Hayes for this bit. CKing 9/93 */
	int             proc;
	char            buf[100];
	prusage_t       r;

	(void)sprintf(buf, "/proc/%d", getpid());
	if ((proc = open(buf, O_RDONLY, 0)) < 0) {
	        fprintf(stderr, "open('%s'):", buf);
	        perror("");
        } else {
	    if (ioctl(proc, PIOCUSAGE, (void *)&r) < 0)
	        perror("ioctl(PIOCUSAGE)");
	    close(proc);
	}
	
      fprintf (fp,"user time used\t\t= %d sec  %ld nsec\n",
				    r.pr_utime.tv_sec, r.pr_utime.tv_nsec);
      fprintf (fp,"system time used\t= %d sec  %ld nsec\n",
				    r.pr_stime.tv_sec, r.pr_stime.tv_nsec);
#else
    getrusage (RUSAGE_SELF, &rbuff);
    fprintf (fp,"user time used\t\t= %ld sec  %ld usec\n", (long)rbuff.ru_utime.tv_sec, rbuff.ru_utime.tv_usec);
    fprintf (fp,"system time used\t= %ld sec  %ld usec\n", (long)rbuff.ru_stime.tv_sec, rbuff.ru_stime.tv_usec);
#endif
#endif
}


void rtd(utime,stime) 
double	*utime,*stime;
{
#ifdef i860
/*
  sec = mclock()/1000;
*/
  double dclock();
  *utime = dclock();
  *stime = 0;
/* printf("dclock(): %lf\n",utime); */
#elif defined(Solaris)
/* Thanks to Jordan Hayes for this bit. CKing 9/93 */
	int             proc;
	char            buf[100];
	prusage_t       r;

	(void)sprintf(buf, "/proc/%d", getpid());
	if ((proc = open(buf, O_RDONLY, 0)) < 0) {
	        fprintf(stderr, "open('%s'):", buf);
	        perror("");
        } else {
	    if (ioctl(proc, PIOCUSAGE, (void *)&r) < 0)
	        perror("ioctl(PIOCUSAGE)");
	    close(proc);
	}
	
	*utime = (double)(r.pr_utime.tv_sec) + (r.pr_utime.tv_nsec)/((double)1.0e9);
	*stime = (double)(r.pr_stime.tv_sec) + (r.pr_stime.tv_nsec)/((double)1.0e9);
#elif defined(CRAY)
    /* added by Greg Hood, PSC */
    *utime = rtclock() / ((double) CLK_TCK);
    *stime = 0.0;
#else
    getrusage (RUSAGE_SELF, &rbuff);
    *utime = (double)(rbuff.ru_utime.tv_sec) + (rbuff.ru_utime.tv_usec)/((double)1.0e6);
    *stime = (double)(rbuff.ru_stime.tv_sec) + (rbuff.ru_stime.tv_usec)/((double)1.0e6);
#endif
}

double	rtd_t() 
{
#ifndef CRAY
#ifdef Solaris
/* Thanks to Jordan Hayes for this bit. CKing 9/93 */
	int             proc;
	char            buf[100];
	prusage_t       r;

	(void)sprintf(buf, "/proc/%d", getpid());
	if ((proc = open(buf, O_RDONLY, 0)) < 0) {
	        fprintf(stderr, "open('%s'):", buf);
	        perror("");
        } else {
	    if (ioctl(proc, PIOCUSAGE, (void *)&r) < 0)
	        perror("ioctl(PIOCUSAGE)");
	    close(proc);
	}

	return((r.pr_utime.tv_sec) + (r.pr_utime.tv_nsec)/1.0e9
	+ (r.pr_stime.tv_sec) + (r.pr_stime.tv_nsec)/1.0e9);
#else
      getrusage (RUSAGE_SELF, &rbuff);
      return((rbuff.ru_utime.tv_sec) + (rbuff.ru_utime.tv_usec)/1.0e6
	     + (rbuff.ru_stime.tv_sec) + (rbuff.ru_stime.tv_usec)/1.0e6);
#endif
#endif
}

void rtl(utime_s,utime_u,stime_s,stime_u) 
long	*utime_s,*utime_u,*stime_s,*stime_u;
{
#ifndef CRAY
#ifdef Solaris
/* Thanks to Jordan Hayes for this bit. CKing 9/93 */
	int             proc;
	char            buf[100];
	prusage_t       r;

	(void)sprintf(buf, "/proc/%d", getpid());
	if ((proc = open(buf, O_RDONLY, 0)) < 0) {
		fprintf(stderr, "open('%s'):", buf);
		perror("");
        } else {
	    if (ioctl(proc, PIOCUSAGE, (void *)&r) < 0)
		perror("ioctl(PIOCUSAGE)");
	    close(proc);
	}

	*utime_s = r.pr_utime.tv_sec;
	*utime_u = (r.pr_utime.tv_nsec)/1.0e3;
	*stime_s = r.pr_stime.tv_sec;
	*stime_u = (r.pr_stime.tv_nsec)/1.0e3;
#else
    getrusage (RUSAGE_SELF, &rbuff);
	*utime_s = rbuff.ru_utime.tv_sec;
	*utime_u = rbuff.ru_utime.tv_usec;
	*stime_s = rbuff.ru_stime.tv_sec;
	*stime_u = rbuff.ru_stime.tv_usec;
#endif
#endif
}
