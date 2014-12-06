static char rcsid[] = "$Id: system.c,v 1.7 2005/10/21 19:01:09 svitak Exp $";

/*
** $Log: system.c,v $
** Revision 1.7  2005/10/21 19:01:09  svitak
** Oops! Left for loop starting at 1, sposed to be 0 in ExecFork.
**
** Revision 1.6  2005/10/20 22:01:12  svitak
** Changed csh to sh for passing non-genesis commands to the system. csh may
** not be installed on cygwin and sh is generally more portable.
**
** Revision 1.5  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.4  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.3  2005/06/20 21:20:15  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.2  2005/06/20 19:28:15  svitak
** Added __APPLE__ to the list of systems which have int status.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2000/07/12 06:18:42  mhucka
** Added #include of unistd.h and string.h.
**
** Revision 1.7  1997/06/12 22:20:36  dhb
** Fixed typo in BgSystemProcess(), == replaces = in conditional
**
** Revision 1.6  1996/05/23 23:17:48  dhb
** t3d/e port
**
** Revision 1.5  1995/06/16  06:02:21  dhb
** FreeBSD compatibility.
**
** Revision 1.4  1995/01/09  19:36:15  dhb
** Additional DEC Alpha compatibility.
**
** Revision 1.3  1994/04/14  19:23:04  dhb
** Changed Solaris conditionals handling call to wait() to SYSV.
**
** Revision 1.2  1993/09/17  21:04:47  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:05:37  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

/* Alpha can use BSD wait structure, but needs to define _BSD */
#ifdef decalpha
#ifndef _BSD
#define _BSD
#endif
#endif

#include <sys/wait.h>
#include "system_deps.h"

extern int debug;

void Beep(){
    putchar('\007');
}

#ifndef CRAY
void BgSystemProcess(s)
char	*s;
{
int pid;
char	*argv[200];
char	tmpstr[200];
char	*sptr;
int		cnt;
short	string_on;

	strcpy(tmpstr,s);
	sptr = tmpstr;
	cnt = 0;
	/*
	* construct the damned argument list myself!
	*/
	while(*sptr != '\0'){
		/*
		* skip leading blanks
		*/
		string_on = 0;
		while((*sptr == ' ') || (*sptr == '\'')){
			if(*sptr == '\'') string_on = 1;
			sptr++;
		}
		/*
		* point to the beginning of the arg
		*/
		argv[cnt++] = sptr;
		/*
		* find the end of the arg
		*/
		while(((*sptr != ' ') || string_on) && (*sptr != '\0') && (*sptr != '\'')){
			sptr++;
		}
		/*
		* replace the space with a null
		*/
		if(*sptr != '\0') *sptr++ = '\0';
	}
	argv[cnt] = NULL;
	/*
	* do the fork
	*/
/* mds3 changes */
#ifdef HAVE_VFORK
      pid = vfork();
#else
      pid = fork();
#endif
	if(pid == -1){
		printf("run: fork unsucessful in BgSystemProcess()\n");
		_exit(0);
	} else 
	if(pid ==0){
		/*
		* execute the program
		*/
#ifdef i860
		printf("Trying to do execvp() in sys/system.c\n");
#else
		execvp(argv[0],argv);
#endif
/*		printf("execve failed\n"); */
		printf("execvp failed\n");
		_exit(0);
	}
}
#endif /* CRAY */

int ExecFork(argc,argv)
int	argc;
char	**argv;
{
#ifdef CRAY
  return -1;
#else
int pid;
#if defined(SYSV) || defined(__FreeBSD__) || defined(__APPLE__)
int status;
#else
union wait status;
#endif
char	*newargv[4];
char	string[1000];
int	i;

    /*
    ** prepare the argument list for the fork
    */
    newargv[0] = "sh";
    newargv[1] = "-c";
    string[0] = '\0';
    for(i=0;i<argc;i++){
	strcat(string,argv[i]);
	strcat(string," ");
    }
    newargv[2] = string;
    newargv[3] = NULL;
    /*
    ** do the fork
    */
/* mds3 changes */
#ifdef HAVE_VFORK
    pid = vfork();
#else
    pid = fork();
#endif
    if(pid == -1){
	printf("run: fork unsuccessful in ExecFork() for %s\n",string);
	_exit(0);
    } else 
    if(pid ==0){
	/*
	** pid = 0 indicates that this is the child resulting
	** from the fork so execute the program 
	** which overlays the current process and therefore
	** does not return
	*/
#ifdef i860
	printf("Trying to do execvp() in sys/system.c\n");
#else
	execvp(newargv[0],newargv);
#endif
	/*
	** if the exec fails then exit the child
	*/
	printf("unable to execute '%s'\n",argv[0]);
	_exit(0);
    }
#ifdef DEBUG
    printf("waiting for child process %d\n",pid);
#endif
    /*
    ** pid > 0 indicates successful child has been forked
    ** so wait for it to complete execution and return the status
    */
#if defined(SYSV) || defined(__FreeBSD__) || defined(__APPLE__)
    while(wait(&status) != pid);
#else
    while(wait(&status.w_status) != pid);
#endif
    if(debug){
	printf("child process %d done. Status = %d\n",
	pid,
#if defined(SYSV) || defined(__FreeBSD__) || defined(__APPLE__)
	status);
#else
	status.w_status);
#endif
    }
#if defined(SYSV) || defined(__FreeBSD__) || defined(__APPLE__)
    return(status);
#else
    return(status.w_status);
#endif

#endif /* CRAY */
}
