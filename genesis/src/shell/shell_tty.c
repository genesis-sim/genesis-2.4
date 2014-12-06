static char rcsid[] =
    "$Id: shell_tty.c,v 1.6 2005/10/24 06:31:22 svitak Exp $";

/*
** $Log: shell_tty.c,v $
**
** 2014/09/09 dbeeman
** Added patch from Gilles Detillieux to cure problem on some Linux systems
** that resulted in no CR provided with LF after quitting GENESIS.
**
** Revision 1.6  2005/10/24 06:31:22  svitak
** Fixed includes for Solaris.
**
** Revision 1.5  2005/10/04 22:05:13  svitak
** Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
** the branch modifications should be included.
**
** Revision 1.4.2.1  2005/08/13 05:20:59  svitak
** Moved extern decls of iofunc() to iofunc.h.
**
** Revision 1.4  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.3  2005/06/18 19:18:36  svitak
** Added __APPLE__ to list of architectures needed to include files correctly.
**
** Revision 1.2  2005/06/17 19:28:24  svitak
** Replaced a call to FIONREAD by a call to AvailableCharacters, which
** will do the proper job depending on the machine and system used.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.18  2001/06/29 21:26:40  mhucka
** Can't have whitespace before a #clause on some old compilers.
**
** Revision 1.17  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.16  2001/04/01 19:29:43  mhucka
** Portability fixes for FreeBSD.
**
** Revision 1.15  2000/10/09 23:01:21  mhucka
** Portability firxes for DEC Alpha.
**
** Revision 1.14  2000/07/02 22:50:03  mhucka
** An "#endif STATUSLINE" caused some compilers to complain it's non-ANSI.
**
** Revision 1.13  2000/07/01 03:58:29  mhucka
** 1) Fixed problem with tty modes getting screwed up when genesis exits.
** 2) UP, BC and PC should not be defined here, they should be extern's.
**
** Revision 1.11  2000/03/27 10:33:07  mhucka
** Commented out statusline functionality, because it caused GENESIS to send
** control character sequences upon exit and thereby screw up the user's
** terminal.  Also added return type declarations for various things.
**
** Revision 1.10  2000/02/26 01:41:33  mhucka
** No functional changes.  I only ran the code through GNU indent 2.2.5 with
** the following options:
**   -bap -br -brs -bs -c40 -cd40 -ce -cs -d0 -di1 -i4 -l79 -lc79-lp
**   -nbad -ncdb -nfc1 -nip -npcs -nsc -psl -sc -ss
** I did this because I have to make changes to disable the status-line
** code, and I decided I may as well clean up the formatting a little bit
** while I'm at it.
**
** Revision 1.9  1999/10/17 04:54:34  mhucka
** Minor Linux patch regarding the inclusion of sys/filio.h.
**
** Revision 1.8  1997/08/12 17:30:54  dhb
** Commented extraneous expressions at end of #else and
** #endif preprocessor directives which result in errors on
** some systems.
**
** Revision 1.7  1996/05/23  23:15:45  dhb
** t3d/e port
**
** Revision 1.6  1995/06/16  05:59:42  dhb
** FreeBSD compatibility.
**
** Revision 1.5  1995/01/09  19:37:03  dhb
** DEC Alpha compatibility changes.
**
** Revision 1.4  1994/10/25  23:57:29  dhb
** Added support for -notty mode of non-interactive use.  The iofunc()
** function is not added as a job if -notty is in effect.  Also, no
** terminal handling is set up.
**
** The setting up of iofunc() as a job was moved from shell_job.c to
** shell_tty.c where is belongs.
**
** Revision 1.3  1993/10/14  22:31:25  dhb
** Added conditional compile of SIGIO code based on DONT_USE_SIGIO define.
**
** Revision 1.2  1993/09/17  17:56:46  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:04:49  dhb
** Initial revision
**
*/

/* mds3 changes
** SIGIO (BSD) equivalent to SIGPOLL (SYSV)
*/
#ifdef i860
#include<rpc/rpc.h>
#endif
#ifdef decalpha
#include <sys/param.h>
#endif
#include "shell_ext.h"
#include <signal.h>
#include <fcntl.h>
#if !defined(mips) && !defined(decalpha) && !defined(linux)
#ifndef SYSV
#include <sys/filio.h>
#endif
#endif

#ifndef PLAINTERM

#if defined(SYSV) && !defined(TERMIO)
#define TERMIO
#endif

#ifdef Solaris
#include <sys/file.h>
#include <sys/filio.h>
#include <signal.h>
#endif

#ifdef CRAY
#include <sys/file.h>
#include <sys/ioctl.h>
#endif

#ifdef Linux
#include <sys/ioctl.h>
#endif

/*
** structures for saving tty parameters
*/
#ifdef TERMIO

#if defined(sgi) || defined(__FreeBSD__) || defined(Linux)
#include <termios.h>
#include <curses.h>
#include <term.h>
#elif defined(__APPLE__)
#include <termios.h>
#include <curses.h>
#elif defined(Solaris)
#include <curses.h>
#include <term.h>
#else
#include <termio.h>
#endif

#if defined(__FreeBSD__) || defined(__APPLE__) || defined(Linux)
struct termios old_tty_stdin, old_tty_stdout, new_tty_stdin, new_tty_stdout;
#else
struct termio old_tty_stdin, old_tty_stdout, new_tty_stdin, new_tty_stdout;
#endif

#else /* TERMIO */
#include <sgtty.h>
struct sgttyb old_tty_stdin, old_tty_stdout, new_tty_stdin, new_tty_stdout;
#endif /* TERMIO */
static char area[1024];

/*
** terminal capabilities
*/
char *tgetstr();
#endif /* PLAINTERM */
#include "iofunc.h"

static char *ce;		       /* clear eol */
static char *so;		       /* begin standout mode */
static char *se;		       /* end standout mode */
static char *cm;		       /* cursor motion */
static int li;			       /* number of lines */
static int co;			       /* number of columns */
static char *cs;		       /* change scrolling region */
static char *rs;		       /* reset to sane mode */
static char *sc;		       /* save cursor position */
static char *rc;		       /* restore cursor position */

extern char PC;
extern char *BC;
extern char *UP;

#ifdef STATUSLINE
static int status_active;
static int status_offset = 2;
#endif /* STATUSLINE */

static int ttymode = 1;

void
SetTtyMode(newvalue)
int newvalue;
{
    ttymode = newvalue;
}

int
IsTtyMode()
{
    return ttymode;
}

void cleareol()
{
    if (ce != NULL) {
	printf("%s", ce);
    }
    fflush(stdout);
}

/* 2000-02-25 <mhucka@bbb.caltech.edu>
 * The status line code hasn't been used in a long time, and under
 * some platforms, the escape codes sent by DisableStatusLine cause
 * problems when GENESIS exits.  However, I'm loath to remove it
 * completely in case someone wants to get it working again in the
 * future.  So I've placed all the calls inside #ifdef STATUSLINE.
 */
#ifdef STATUSLINE
int StatusActive()
{
    return (status_active);
}
#endif

void SaveCursorPosition()
{
    if (sc) {
	printf("%s", sc);
	fflush(stdout);
    }
}

void RestoreCursorPosition()
{
    if (rc) {
	printf("%s", rc);
	fflush(stdout);
    }
}

void SaneTerm()
{
    if (rs) {
	printf("%s", rs);
	fflush(stdout);
    }
}

void TermParse(cs, parameter)
char *cs;
int *parameter;
{
    char *ptr;
    int current_parm = 0;
    unsigned char ch;

    for (ptr = cs; *ptr != '\0'; ptr++) {
	if (*ptr == '\0')
	    break;
	switch (*ptr) {
	case '%':
	    ptr++;
	    switch (*ptr) {
	    case 'd':
		printf("%d", parameter[current_parm++]);
		break;
	    case '2':
		printf("%2d", parameter[current_parm++]);
		break;
	    case '3':
		printf("%3d", parameter[current_parm++]);
		break;
	    case 'i':
		parameter[current_parm]++;
		break;
	    case '+':
		ptr++;
		ch = (unsigned char) (parameter[current_parm++]) + *ptr;
		printf("%c", ch);
		break;
	    case '.':
		ch = parameter[current_parm++];
		printf("%c", ch);
		break;
	    case '%':
		printf("%c", '%');
		break;
	    default:
		printf("%c", *ptr);
		break;
	    }
	    break;
	default:
	    printf("%c", *ptr);
	    break;
	}
    }
    fflush(stdout);
}

int TermGoto(line, col)
int line;
int col;
{
    int parameter[2];

    if (cm) {
	parameter[0] = line;
	parameter[1] = col;
	TermParse(cm, parameter);
	return (1);
    } else {
	return (0);
    }
}

#ifdef STATUSLINE
int
StatusMessage(s, col, line)
char *s;
int col;
int line;
{
    int inverse;

    if (status_active) {
	if (line == 0) {
	    inverse = 1;
	} else {
	    inverse = 0;
	}
	SaveCursorPosition();
	TermGoto(li - status_offset + line, col);
	if (inverse)
	    Standout(TRUE);
	printf("%s", s);
	if (inverse)
	    Standout(FALSE);
	RestoreCursorPosition();
	fflush(stdout);
	return (1);
    } else {
	return (0);
    }
}

void StatusLine(line, inverse)
int line;
int inverse;
{
    int i;

    if (status_active) {
	SaveCursorPosition();
	if (TermGoto(li - status_offset + line, 1)) {
	    if (inverse)
		Standout(TRUE);
	    for (i = 0; i < co; i++) {
		printf(" ");
	    }
	    if (inverse)
		Standout(FALSE);
	}
	RestoreCursorPosition();
    }
}
#endif /* STATUSLINE */

int
ScrollRegion(bottom, top)
int bottom;
int top;
{
    int parameter[2];

    if (cs) {
	parameter[0] = bottom;
	parameter[1] = top;
	/*
	 * ** parse the escape sequence
	 */
	TermParse(cs, parameter);
	return (1);
    } else {
	return (0);
    }
}

void Standout(val)
int val;
{
    switch (val) {
    case 0:			       /* standout off */
	if (se) {
	    printf("%s", se);
	}
	break;
    case 1:			       /* standout on */
	if (so) {
	    printf("%s", so);
	}
	break;
    }
}

#ifdef PLAINTERM

/*
** for vanilla terminal handling just define empty functions for
*/
void genesis_tty()
{
}

void restore_tty()
{
}

void normal_tty()
{
}

void terminal_setup()
{
    /*
     * ** mds3 changes
     * printf("Should be in non-blocking mode now...\n");
     * fcntl(fileno(stdin),F_SETFL,O_NDELAY);
     */
#ifndef i860
    /* mds3 changes */
    /* Temporarily comment this out until we sort out the problems with
     * ** terminal handling (makes Genesis only run from script files for now)
     */
    if (ttymode)
	AddJob(iofunc, 1);
#endif
}

void
tset()
{
}

#else /* PLAINTERM */
/*
** give back the original terminal state
*/
void normal_tty()
{
    if (!ttymode)
	return;

    /*
     * ignore io interrupts
     */
#if defined(SIGIO) && !defined(DONT_USE_SIGIO)
#ifdef BSD
    signal(SIGIO, SIG_DFL);
#else /* BSD */
    signal(SIGPOLL, SIG_DFL);
#endif /* BSD */
#endif /* SIGIO && !DONT_USE_SIGIO */
    /*
     * reset the terminal parameters to their original state
     */
#ifdef TERMIO
#if defined(__FreeBSD__) || defined(__APPLE__) || defined(Linux)
    tcsetattr(fileno(stdin), TCSANOW, &old_tty_stdin);
    tcsetattr(fileno(stdout), TCSANOW, &old_tty_stdout);
#else
    ioctl(fileno(stdin), TCSETA, &old_tty_stdin);
    ioctl(fileno(stdout), TCSETA, &old_tty_stdout);
#endif
#else /* TERMIO */
    ioctl(fileno(stdin), TIOCSETP, &old_tty_stdin);
    ioctl(fileno(stdout), TIOCSETP, &old_tty_stdout);
#endif /* TERMIO */
}

void restore_tty()
{
    if (!ttymode)
	return;

    normal_tty();
#ifdef STATUSLINE
    DisableStatusLine();
#endif /* STATUSLINE */
    if (IsSilent() < 2)
	printf("\n");		       /* scroll an additional line */
}

void genesis_tty()
{
    tset();
}


/*
** prepare the terminal settings used in thee simulator command line io
*/
void terminal_setup()
{
    if (!ttymode)
	return;

    /* 
     * save the old terminal parameters
     */
#ifdef TERMIO
#if defined(__FreeBSD__) || defined(__APPLE__) || defined(Linux)
    tcgetattr(fileno(stdin), &old_tty_stdin);
    tcgetattr(fileno(stdout), &old_tty_stdout);
#else
    ioctl(fileno(stdin), TCGETA, &old_tty_stdin);
    ioctl(fileno(stdout), TCGETA, &old_tty_stdout);
#endif
#else
    ioctl(fileno(stdin), TIOCGETP, &old_tty_stdin);
    ioctl(fileno(stdout), TIOCGETP, &old_tty_stdout);
#endif
    /*
     * set the new terminal parameters
     */
    tset();

    /*
     * If we don't have SIGIO or don't want to use it, then set up a
     * job to poll for input.
     */

#if !defined(SIGIO) || defined(DONT_USE_SIGIO)
#ifndef i860
    /* mds3 changes */
    /* Temporarily comment this out until we sort out the problems with
     * terminal handling (makes Genesis only run from script files for now)
     */
    AddJob(iofunc, 1);
#endif
#endif

}
#endif /* PLAINTERM */

#ifdef STATUSLINE
void EnableScrollRegion()
{
    /*
     * in order to do the status line, the terminal must be capable
     * of saving and restoring cursor locations as well as region scrolling
     */
    if (sc && rc && ScrollRegion(0, li - status_offset)) {
	status_active = 1;
    } else {
	status_active = 0;
    }
}

void DisableStatusLine()
{
    ScrollRegion(0, li);
    StatusLine(0, 0);
    status_active = 0;
    if (IsSilent() < 2)
	TermGoto(li - 1, 0);	       /* go to the bottom of the screen */
}

void EnableStatusLine()
{
    printf("\n\n\n");
    EnableScrollRegion();
    StatusLine(0, 1);
    TermGoto(li - status_offset - 1, 0);
}
#endif /* STATUSLINE */

#ifndef PLAINTERM
char *
termstr(s, area)
char *s;
char **area;
{
    char *value;

    value = tgetstr(s, area);	       /* get the terminal capability string */
    if (value) {
	/*
	 * skip the speed component
	 */
	while ((*value <= '9' && *value >= '0') || *value == '*')
	    value++;
    }
    return (value);
}

/*
** terminal setup must be called prior to doing the first tset
** or bad things will happen
*/
void
tset()
{
    char *term;
    char *getenv();
    char bp[1024];
    char *areaptr;
    int nc;
    int i;
    char buffer[1000];

    if (!ttymode)
	return;

    /*
     * get any leftover characters from the previous terminal io
     * state
     */
    nc = AvailableCharacters();
    if (nc) {
	for (i = 0; i < nc; i++) {
	    buffer[i] = getc(stdin);
	}
    }
    /*
     * load the termcap entry
     */
    term = getenv("TERM");
    if (term == NULL) {
	printf
	    ("TERM variable is undefined, therefore terminal type is unknown.\n");
	printf("Assuming vt100.\n");
	term = "vt100";
    }
    if (tgetent(bp, term) == -1) {
	printf("Could not find termcap/terminfo entry for %s\n", term);
    }
    /*
     * get some term capabilities
     * like backspace and eol
     */
    areaptr = area;

    ce = termstr("ce", &areaptr);      /* clear eol */
    so = termstr("so", &areaptr);      /* begin standout mode */
    se = termstr("se", &areaptr);      /* end standout mode */
    cm = termstr("cm", &areaptr);      /* cursor motion */
    co = tgetnum("co");		       /* number of columns */
    li = tgetnum("li");		       /* number of lines */
    cs = termstr("cs", &areaptr);      /* change scrolling region */
    rs = termstr("rs", &areaptr);      /* reset to sane mode */
    UP = termstr("UP", &areaptr);      /* move cursor up n lines */
    sc = termstr("sc", &areaptr);      /* save cursor position */
    rc = termstr("rc", &areaptr);      /* restore cursor position */

    /*
     * copy the old terminal parameters and
     * bcopy(&old_tty,&new_tty,sizeof(struct sgttyb));
     * prepare to set the new terminal parameters
     */
    new_tty_stdin = old_tty_stdin;
#ifdef TERMIO
    /*
     * enable single character input
     */
    new_tty_stdin.c_lflag &= ~(ECHO | ICANON);
    new_tty_stdin.c_oflag |= ONLCR;
    /* on some systems the c_cc array is too small so test before setting: */
    if (sizeof(new_tty_stdin) >
           ((void *)&new_tty_stdin.c_cc[VEOF] - (void *)&new_tty_stdin))
        new_tty_stdin.c_cc[VEOF] = 1;
    if (sizeof(new_tty_stdin) >
           ((void *)&new_tty_stdin.c_cc[VEOL] - (void *)&new_tty_stdin))
        new_tty_stdin.c_cc[VEOL] = 1;
    /*
     * set the new parameters
     */
#if defined(__FreeBSD__) || defined(__APPLE__) || defined(Linux)
    tcsetattr(fileno(stdin), TCSANOW, &new_tty_stdin);
#else
    ioctl(fileno(stdin), TCSETA, &new_tty_stdin);
#endif
#if defined(SIGIO) && !defined(DONT_USE_SIGIO)
    /* 
     * enable the sending of SIGIO when input becomes available 
     */
    fcntl(fileno(stdin), F_SETFL, FASYNC);
    /*
     * put back any leftover characters from the previous terminal io
     * state
     */
    signal(SIGIO, SIG_DFL);
    /*
     * enable io interrupts
     */
    signal(SIGIO, iofunc);
#endif /* SIGIO && !DONT_USE_SIGIO */
#ifndef SYSV
    if (nc) {
	for (i = 0; i < nc; i++) {
	    ioctl(fileno(stdin), TIOCSTI, buffer + i);
	}
    }
#endif
#else /* TERMIO */
    /*
     * enable single character input
     */
    new_tty_stdin.sg_flags |= CBREAK;
    /*
     * dont echo characters as they are typed
     */
    new_tty_stdin.sg_flags &= ~ECHO;
    /*
     * set the new parameters
     */
    ioctl(fileno(stdin), TIOCSETP, &new_tty_stdin);
#if defined(SIGIO) && !defined(DONT_USE_SIGIO)
    /* 
     * enable the sending of SIGIO when input becomes available 
     */
    fcntl(fileno(stdin), F_SETFL, FASYNC);
    /*
     * put back any leftover characters from the previous terminal io
     * state
     */
    signal(SIGIO, SIG_DFL);
    /*
     * enable io interrupts
     */
    signal(SIGIO, iofunc);
#endif /* SIGIO && !DONT_USE_SIGIO */
    if (nc) {
	for (i = 0; i < nc; i++) {
	    ioctl(fileno(stdin), TIOCSTI, buffer + i);
	}
    }
#endif /* TERMIO */

    /* 
     * attach the interrupt handlers to the various signals 
     */
    /*
     * signal(SIGQUIT,quitfunc);
     * signal(SIGINT,intfunc);
     */
}

#endif /* PLAINTERM */

#ifdef STATUSLINE
int
do_status_message(argc, argv)
int argc;
char **argv;
{
    if (argc < 4) {
	printf("usage: %s string col line\n", argv[0]);
	return (0);
    }
    return (StatusMessage(argv[1], atoi(argv[2]), atoi(argv[3])));
}
#endif /* STATUSLINE */
