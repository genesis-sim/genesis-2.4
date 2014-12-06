static char rcsid[] = "$Id: popen.c,v 1.2 2005/06/25 22:04:40 svitak Exp $";

/*
** $Log: popen.c,v $
** Revision 1.2  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2000/09/21 19:43:46  mhucka
** Don't define TRUE/FALSE here, just include header.h
**
** Revision 1.6  2000/07/12 06:17:07  mhucka
** Added #include of unistd.h and removed unused variable in G_pclose().
**
** Revision 1.5  1996/05/23 23:17:48  dhb
** t3d/e port
**
** Revision 1.4  1995/04/14  19:12:08  dhb
** Changed popen() and pclose() to G_popen() and G_pclose() to avoid
** conflicts with standard library version.
**
** Revision 1.3  1995/03/24  22:04:40  dhb
** Fixed typo in Linux specific changes.
**
** Revision 1.2  1995/02/22  19:11:10  dhb
** Linux support.
**
** Revision 1.1  1992/12/11  19:05:32  dhb
** Initial revision
**
*/

/*
** popen code written by Dave Bilitch
*/

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

extern int	errno;

#define	MAX_PFILES	10

typedef struct _pfile
  {
    FILE	*pf_fs;
    int		pf_pid;
  } PFILE;

static PFILE	PFiles[MAX_PFILES];

/*
** G_popen
**
**	Local version of popen which allows for multiple simultaneous
**	popen calls.
*/

FILE	*G_popen(cmd, mode)

char	*cmd;
char	*mode;

{	/* G_popen --- Open pipe to/from command */

	FILE	*fs;
	int	pid;
	int	p[2];
	int	slot;

#if defined(CRAY)
	return (popen(cmd,mode));
#else
	if (*mode != 'w' && *mode != 'r')
	  {
	    errno = EINVAL;
	    return(NULL);
	  }

	/*
	** Find an empty slot in PFiles
	*/

	for (slot = 0; slot < MAX_PFILES; slot++)
	    if (PFiles[slot].pf_fs == NULL)
		break;

	if (slot >= MAX_PFILES)
	  {
	    errno = ENOSPC;
	    return(NULL);
	  }

	if (pipe(p) == -1)
	    return(NULL);

	fflush(stdout);
/* mds3 changes */
#ifdef HAVE_VFORK
      pid = vfork();
#else
      pid = fork();
#endif
	if (pid == -1)
	  {
	    close(p[0]);
	    close(p[1]);
	    return(NULL);
	  }

	if (pid == 0)
	  {
	    if (*mode == 'w')
	      {
		close(p[1]);
		close(0);
		dup(p[0]);
		close(p[0]);
	      }
	    else
	      {
		close(p[0]);
		close(1);
		dup(p[1]);
		close(p[1]);
	      }

#ifdef i860
	    printf("Trying to do execl() in sys/popen.c\n");
#else
	    execl("/bin/sh", "sh", "-c", cmd, NULL);
#endif
	    _exit(1);
	  }

	if (*mode == 'w')
	  {
	    close(p[0]);
	    fs = (FILE *) fdopen(p[1], mode);
	  }
	else
	  {
	    close(p[1]);
	    fs = (FILE *) fdopen(p[0], mode);
	  }

	PFiles[slot].pf_fs = fs;
	PFiles[slot].pf_pid = pid;

	return(fs);

#endif /* CRAY */

}	/* G_popen */



void G_pclose(fs)

FILE	*fs;

{	/* G_pclose --- Close the pipe to the specified program, wait on proc */

	int	pid;
	int	slot;
	int     WaitOn();

#if defined(CRAY)
	pclose(fs);
#else
	for (slot = 0; slot < MAX_PFILES; slot++)
	    if (PFiles[slot].pf_fs == fs)
		break;

	if (slot >= MAX_PFILES)
	    return;

	fclose(fs);
	PFiles[slot].pf_fs = NULL;

	pid = WaitOn(PFiles[slot].pf_pid, NULL); 
	PFiles[slot].pf_pid = 0;

#endif /* CRAY */

}	/* G_pclose */
