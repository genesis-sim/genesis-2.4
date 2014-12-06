static char rcsid[] = "$Id: shell_io.c,v 1.5 2005/10/04 22:05:13 svitak Exp $";

/*
** $Log: shell_io.c,v $
** Revision 1.5  2005/10/04 22:05:13  svitak
** Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
** the branch modifications should be included.
**
** Revision 1.4  2005/09/29 21:43:55  ghood
** changed AvailableCharacters because xt3 does not have FIONREAD
**
** Revision 1.3.2.1  2005/08/13 05:20:59  svitak
** Moved extern decls of iofunc() to iofunc.h.
**
** Revision 1.3  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.2  2005/06/17 19:26:21  svitak
** Function AvailableChars: Cygwin does not implement the FIONREAD ioctl
** (aka TIOCINQ). This was solved using the same method as for T3E.
** Requires including <sys/time.h> for Cygwin.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.19  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.17  2001/04/01 19:29:43  mhucka
** Portability fixes for FreeBSD.
**
** Revision 1.16  2000/06/12 05:07:04  mhucka
** Removed nested comments; added NOTREACHED comments where appropriate.
**
** Revision 1.15  2000/03/27 10:33:07  mhucka
** Commented out statusline functionality, because it caused GENESIS to send
** control character sequences upon exit and thereby screw up the user's
** terminal.  Also added return type declarations for various things.
**
** Revision 1.14  1998/07/21 22:28:57  dhb
** Fixed version messages on ^V to print current version.
**
** Revision 1.13  1997/08/12 17:30:54  dhb
** Commented extraneous expressions at end of #else and
** #endif preprocessor directives which result in errors on
** some systems.
**
** Revision 1.12  1997/08/08  19:27:09  dhb
** Include sgtty.h rather than termio.h for Linux running glibc
** version of libc.
**
** Revision 1.11  1997/07/18 19:57:58  dhb
** Changes from PSC: T3E support
**
** Revision 1.10  1996/06/06  20:50:04  dhb
** merged in 1.8.1.1 changes.
**
** Revision 1.9  1996/05/23  23:15:45  dhb
** t3d/e port
**
** Revision 1.8.1.1  1996/06/06  20:05:49  dhb
** Paragon port.
**
** Revision 1.8  1995/04/14  18:10:50  dhb
** Call to iofunc() in nbgets() was missing its parameter.
**
** Revision 1.7  1995/02/17  21:58:05  dhb
** Added calls to ExecuteCleanups() to nbgets() to call cleanup code
** after executing jobs.
**
** Revision 1.6  1995/01/20  01:42:01  dhb
** Include of rpc/rpc.h unser IRIX 5.x (a SYSV system) causes include
** of stdlib.h which results in conflict over optopt global variable.
** Changed this include to be #ifdef i860 rather than #ifdef SYSV.
**
** Revision 1.5  1994/10/25  23:57:29  dhb
** Added support for -notty mode of non-interactive use.  The iofunc()
** function is not added as a job if -notty is in effect.  Also, no
** terminal handling is set up.
**
** The setting up of iofunc() as a job was moved from shell_job.c to
** shell_tty.c where is belongs.
**
** Revision 1.4  1994/08/31  02:58:23  dhb
** Removed non-portable putfeof() macro and the apparently defunct
** eof command function do_eof().
**
** Revision 1.3  1993/10/14  22:13:05  dhb
** Added conditional compilation of use of SIGIO based on
** !defined(DONT_USE_SIGIO).
**
** Revision 1.2  1993/09/17  17:23:15  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:04:33  dhb
** Initial revision
**
*/

/*
** Sorry, but we want to print the version number here.  Could create
** a function in sim to do it, but we'd still be dependent on sim in
** any case.
*/

/* mds3 changes */
#include <fcntl.h>
/*
** SIGIO(BSD) equivalent to SIGPOLL(SYSV)
*/
#ifdef Solaris

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/filio.h>

#else

#ifdef i860
/* I believe this is for the port to the Delta   ---dhb */
#include <rpc/rpc.h>
#endif

#ifdef CRAY
#ifdef T3E
#include <unistd.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#else
#include <sys/ioctl.h>
#endif
#else  /* CRAY */

#if defined(SYSV) && !(defined(Linux) && defined(__GLIBC__))
#include <sys/termio.h>
#else
#include <sgtty.h>
#endif
#endif /* CRAY */

#endif

#ifdef Cygwin
#include <sys/time.h>
#endif

#include <string.h>
#include <signal.h>
#include "shell_ext.h"
#include "ss_func_ext.h"
#include "sim_version.h"
#include "iofunc.h"

/*
** in order to simulate non-blocking io one of two options must be
** supported. The first option is to use non-blocking io explicitly.
** This is done by disabling the PLAINTERM option in the makefile and 
** either using the termio libraries (TERMIO flag in the makefile)
** or the sgtty libraries (no TERMIO flag in the makefile).
** This option gives the most flexible io control and enables a number
** of useful command editing functions within the simulator shell.
** If neither termio or sgtty is available 
** then the PLAINTERM flag must be set in the makefile.
** In order to implement non-blocking io under these conditions
** the function AvailableCharacters must contain a system function
** which is capable of determining the number of characters currently
** available for reading from the input stream. 
*/
#define WhiteSpace(C) ((C == ' ') || (C == '\t'))

#define MAXCHAR 5000 	/* maximum number of buffered characters */
#define MAXLINE 100 	/* maximum number of buffered lines */
#define MAXESCAPE 100 	/* maximum number of characters in the escape buffer */

extern EscapeCommand	*esc_command;
static int		escape_char_index = 0;

static char 	escape_buffer[MAXESCAPE];
static char 	input_buffer[MAXCHAR];
static int 	eol_buffer[MAXLINE];

/*
** current_loc points to the current cursor location
** in the input buffer
*/
static int 	current_loc=0;
static int 	current_line=0;
static int	start_loc = 0;
/*
** end_of_buffer points to the end of the line
** this will be the next location after the final valid character
** in the line
*/
static int 	end_of_buffer = 0;
static short 	insert_mode;
static short 	escape_mode;
static int 	current_history;

/*
** Forward declarations
*/
int endline();


/*
** return the number of characters available in the standard input
** stream buffer
*/
int AvailableCharacters()
{
int nc;
#if defined(i860) && !defined(paragon)
nc = ioctl(fileno(stdin),FIORDCHK,0);
/* printf("nc=%d\n",nc); */
#elif defined(T3E) || defined(Cygwin)
#ifdef T3E
#define AVAILABLECHARSFIELD _cnt
#else
#define AVAILABLECHARSFIELD _r
#endif
  fd_set readfds;
  struct timeval timeout;

  if (stdin->AVAILABLECHARSFIELD == 0)
    {
      FD_ZERO(&readfds);
      FD_SET(0, &readfds);
      timeout.tv_sec = 0;
      timeout.tv_usec = 0;
      if (select(1, &readfds, NULL, NULL, &timeout) > 0)
	ungetc(getc(stdin), stdin);
    }
  nc = stdin->AVAILABLECHARSFIELD;
#elif defined(XT3)
  nc = 0;  /* FIONREAD is unsupported so for now just assume no interactive
              input on XT3 */
#else
    ioctl(fileno(stdin),FIONREAD,&nc);
#endif
    return(nc);
}

/*
** return the location of the first character in the line
*/
int bol()
{
int i;
    for(i=current_loc;i>=start_loc;i--){
	/*
	** return the location following a cr if there
	** is one
	*/
	if(input_buffer[i] == '\n') return(i+1);
    }
    /*
    ** otherwise just return the 0 location
    */
    return(start_loc);
}

void backerase()
{
    printf("\b \b");
    fflush(stdout);
}

void backspace()
{
    printf("\b");
    fflush(stdout);
}

void movebol()
{
int i;
    for(i=bol();i<current_loc;i++){
	backspace();
    }
    current_loc = bol();
}

/*
** redisplay the line
*/
void refreshline()
{
int i;
    /*
    ** starting at the current location going to the end of the line
    ** print out the characters
    */
    for(i=current_loc;i<end_of_buffer;i++){
	putc(input_buffer[i],stdout);
    }
    /*
    ** clear out the rest of the characters on that line of the display
    */
    cleareol();
    /*
    ** reposition the cursor at the current location by doing 
    ** backspaces
    */
    for(i=current_loc;i<end_of_buffer;i++){
	backspace();
    }
    fflush(stdout);
}

void moveeol()
{
int i;

    for(i=current_loc;i<end_of_buffer;i++){
	putc(input_buffer[i],stdout);
    }
    current_loc = end_of_buffer;
    fflush(stdout);
}

/*
** do the delete but without doing the refresh
*/
void nrdeletechar()
{
int i;
    /*
    ** dont delete past the beginning of the line
    */
    if(--end_of_buffer < bol()) end_of_buffer = bol();
    /*
    ** how the hell can this happen?
    ** check it out later. I must have done it for some reason
    */
    if(end_of_buffer < current_loc) {
	backspace();
	current_loc = end_of_buffer;
    }
    /*
    ** delete the character by shifting the rest of the characters back
    */
    for(i=current_loc;i<end_of_buffer;i++){
	input_buffer[i] = input_buffer[i+1];
    }
}

void deletechar()
{
    /*
    ** delete the character
    */
    nrdeletechar();
    /*
    ** and redisplay the line
    */
    refreshline();
}

void insertchar(c)
char c;
{
int i;
    
    /*
    ** bump the end of line counter
    */
    end_of_buffer++;
    /*
    ** and open a space
    */
    for(i=end_of_buffer;i>current_loc;i--){
	input_buffer[i] = input_buffer[i-1];
    }
    /*
    ** insert the character
    */
    input_buffer[current_loc] = c;
    /*
    ** display the new character
    */
    putc(c,stdout);
    /*
    ** advance the current cursor
    */
    current_loc++;
    /*
    ** and redisplay the result
    */
    refreshline();
}

void redisplayline()
{
int i;
    for(i=bol();i<current_loc;i++){
	putc(input_buffer[i],stdout);
    }
    refreshline();
}

void LoadHistory()
{
    if(History(current_history) == NULL){
	current_loc = bol();
    } else {
	strcpy(input_buffer+bol(),History(current_history));
	current_loc = (int)strlen(input_buffer);
	printf("%s",input_buffer+bol());
    }
    fflush(stdout);
    end_of_buffer = current_loc;
}

void ProcessChar(ch)
char ch;
{
#ifndef PLAINTERM
    /*
    ** process it normally
    */
    switch(ch){
    case '':			/* move to beginning of line */
	movebol();
	break;
    case '':			/* move to end of line */
	moveeol();
	break;
    case '':
	putc('\n',stdout);
	redisplayline();
	break;
    case '\4':			/* delete char */
	deletechar();
	break;
    case '': 			/* destructive backspace */
	if(current_loc > bol()) {
	    backspace();
	    current_loc--;
	}
	deletechar();
	break;
    case '': 			/* nondestructive backspace */
    case '': 			/* nondestructive backspace */
	if(current_loc > bol()) {
	    backspace();
	    current_loc--;
	}
	break;
    case '': 			/* forwardspace */
	if(current_loc < end_of_buffer) {
	    putc(input_buffer[current_loc],stdout);
	    fflush(stdout);
	    current_loc++;
	}
	break;
    case '':			/* previous command line */
	movebol();
	cleareol();
	if(--current_history < 0) current_history = 0;
	LoadHistory();
	break;
    case '':			/* next command line */
	movebol();
	cleareol();
	if(++current_history > HistoryCnt()) current_history = HistoryCnt();
	LoadHistory();
	break;
#ifdef OLD
    case '':			/* process status */
	ProcessStatus();
	break;
#endif /* OLD */
    case '\t':			/* toggle insert mode */
	insert_mode = !insert_mode;
#ifdef STATUSLINE
	if(insert_mode){
	    StatusMessage("<insert>",0,0);
	} else {
	    StatusMessage("        ",0,0);
	}
#endif /* STATUSLINE */
	break;
    case '':			/* escape mode */
	escape_mode = 1;
#ifdef STATUSLINE
	StatusMessage("<ESC>",10,0);
#endif /* STATUSLINE */
	break;
    case '': 			/* erase line */
	movebol();
	cleareol();
	fflush(stdout);
	current_loc = bol();
	end_of_buffer = current_loc;
	break;
    case '': 			/* version message */
	    printf(
	    "\nGENESIS version %s\n", VERSIONSTR);
	break;
    case '':			/* erase word */
	/*
	** delete white space
	*/
	if(WhiteSpace(input_buffer[current_loc])){
	    if(current_loc > bol()) {
		backspace();
		current_loc--;
	    }
	}
	while(WhiteSpace(input_buffer[current_loc-1]) ||
	WhiteSpace(input_buffer[current_loc])
	    ){
	    nrdeletechar();
	    if(current_loc > bol()) {
		backspace();
		current_loc--;
	    } else break;
	}
	/*
	** delete non-white space
	*/
	if(!WhiteSpace(input_buffer[current_loc])){
	    nrdeletechar();
	}
	while(!WhiteSpace(input_buffer[current_loc-1])){
	    if(current_loc > bol()) {
		backspace();
		current_loc--;
		nrdeletechar();
	    } else break;
	}
	refreshline();
	fflush(stdout);
	break;
    case '':			/* truncate and end line */
	cleareol();
	putc('\n',stdout);
	fflush(stdout);
	input_buffer[current_loc] = '\n';
	current_loc++;
	end_of_buffer = current_loc;
	eol_buffer[current_line++] = end_of_buffer;
	break;
    case '\n':			/* end of line */
    case '\0':
	putc('\n',stdout);
	fflush(stdout);
	input_buffer[end_of_buffer] = '\n';
	end_of_buffer++;
	current_loc++;
	eol_buffer[current_line++] = end_of_buffer;
	break;
    default:
	if(insert_mode){
	    insertchar(ch);
	} else {
	    input_buffer[current_loc] = ch;
	    /*
	    * print them out right away
	    */
	    putc(ch,stdout);
	    current_loc++;
	}
	fflush(stdout);
	break;
    }
#else /* PLAINTERM */
    switch(ch){
    case '\r':		/* end of line */
    case '\n':
    case '\0':
	fflush(stdout);
	input_buffer[end_of_buffer] = '\n';
	end_of_buffer++;
	current_loc++;
	eol_buffer[current_line++] = end_of_buffer;
	break;
    default:
	input_buffer[current_loc++] = ch;
	break;
    }
#endif /* PLAINTERM */
    if(current_loc > end_of_buffer){
	end_of_buffer = current_loc;
    }
    if(end_of_buffer >= MAXCHAR -1){
	/*
	** input buffer full
	*/
	end_of_buffer = MAXCHAR - 1;
	input_buffer[end_of_buffer] = '\0';
	eol_buffer[current_line++] = end_of_buffer;
    }
}

void AddStringToBuffer(string)
char	*string;
{
    if(string == NULL) return;
    while(*string != '\0'){
	ProcessChar(*string++);
    }
}

void ProcessEscapeChar(ch)
char	ch;
{
int	j;
EscapeCommand *esc;

    /*
    ** add the character to the escape buffer
    */
    escape_buffer[escape_char_index] = ch;
    /*
    ** check all defined escape sequences
    */
    for(esc=esc_command;esc;esc=esc->next){
	/*
	** check the escape character at the current escape index 
	*/
	if(strncmp(esc->escseq,escape_buffer,escape_char_index+1) == 0){
	    /*
	    ** if it matches then check to see whether this is the end of
	    ** a completed escape sequence
	    */
	    if(esc->escseq[escape_char_index+1] == '\0'){
		/*
		** it is so perform the appropriate escape function
		*/
		if(esc->exec_mode){
		    /*
		    ** execute the assigned escape function
		    */
		    ExecuteFunction(esc->argc,esc->argv);
		} else {
		    /*
		    ** substitute the escape string into the stdin buffer
		    */
		    AddStringToBuffer(esc->string);
		}
		/*
		** clear the escape mode after completing an escape sequence
		** and return
		*/
		escape_char_index = 0;
		escape_mode = 0;
#ifdef STATUSLINE
		StatusMessage("     ",10,0);
#endif /* STATUSLINE */
		return;
	    }
	    /*
	    ** a matching character was found so no point in
	    ** continuing the search
	    ** but no complete sequence was found so
	    ** go to the next character in the sequence
	    */
	    escape_char_index++;
	    return;
	}
    }
    /*
    ** the search was unsuccessful
    ** so clear the escape mode and reset the index
    ** also put back the characters in the escape buffer
    */
    for(j=0;j<escape_char_index+1;j++){
	ProcessChar(escape_buffer[j]);
    }
    escape_char_index = 0;
    escape_mode = 0;
#ifdef STATUSLINE
    StatusMessage("     ",10,0);
#endif
#ifdef OLD
    switch(ch){
    case '':
	EnableStatusLine();
	break;
    case '':
	DisableStatusLine();
	break;
    case '':
	if(!StatusActive()){
	    printf("\n");
	}
	SimStatus();
	break;
    default:
	/*
	** use the character to lookup the escape string associated 
	** with it
	*/
	GetEscapeString(escape_buffer,MAXESCAPE,ch);
	/*
	** process all of the characters in the escape buffer
	*/
	for(j=0;j<MAXESCAPE;j++){
	    if((ch = escape_buffer[j]) == '\0')
		break;
	    ProcessChar(ch);
	}
	break;
    }
    /*
    ** clear the escape mode after completing an escape sequence
    */
    escape_mode = 0;
    StatusMessage("     ",10,0);
#endif /* OLD */
}

/*
** io interrupt handler (SIGIO)
*/
void
iofunc(sig)
     int	sig;
{
  char	ch;
  int	i;
  int	nc;

/* mds3 changes
** printf("Should change to non-blocking mode now...\n");
** fcntl(fileno(stdin),F_SETFL,O_NDELAY);
*/
  /*
  ** check how many characters in the buffer?
  */
#ifdef i860
  /*
  ** read the character from stdin
  */
  ch = getc(stdin);
  printf("char:%c",ch);
  if(ferror(stdin)) return;
  printf("escape_mode?\n");
  if(escape_mode){
    printf("About to ProcessEscapeChar()\n");
    ProcessEscapeChar(ch);
  } else {
    printf("About to ProcessChar()\n");
    ProcessChar(ch);
  }
  printf("About to clearerr()\n");
  clearerr(stdin);
  printf("Finished clearerr()\n");
#endif
#if (!defined(OLD)&&!defined(i860))
/*  printf("!(OLD)&&!(i860)\n"); */
  /*
  ** poll the input stream buffer to determine whether characters are
  ** available
  */
  nc = AvailableCharacters();
  for(i=0;i<nc;i++){
    /*
    ** read the character from stdin
    */
    ch = getc(stdin);
    if(escape_mode){
      ProcessEscapeChar(ch);
    } else {
      ProcessChar(ch);
    }
  }
#endif
#ifdef OLD
  while(1){
  /*
  ** read the character from stdin
  */
    ch = getc(stdin);
  if(ferror(stdin)) break;
  if(escape_mode){
    ProcessEscapeChar(ch);
  } else {
    ProcessChar(ch);
  }
}
  clearerr(stdin);
#endif /* OLD */
}

/*
** non-blocking io routine to fill the array line with characters
** currently only called with stdin as the fp
*/
char *nbgets(line,size,fp)
char *line;
int size;
FILE *fp;
{
int nc;
int eol;
char	*ptr;
char	new_line[1000];

    /*
    ** If we don't have an interactive shell, just execute the jobs and
    ** return no input.
    */
    if (!IsTtyMode()) {
	ExecuteJobs();
	ExecuteCleanups();
	return NULL;
    }

    /*
    ** turn off insert mode on each line of input
    */
    insert_mode = 0;
    /*
    ** set the current history pointer to the current command
    */
    current_history = HistoryCnt();
    /*
    ** clear the line
    */
    line[0] = '\0';
    /*
    ** enable io interrupts
    */
#ifndef PLAINTERM
    /*
    ** how many characters left in the system stdin buffer?
    */
    nc = AvailableCharacters();
    /*
    ** if there are any characters in the buffer then
    ** process them
    */
#if defined(SIGIO) && !defined(DONT_USE_SIGIO)
    if(nc){
	signal(SIGIO,SIG_DFL);
	iofunc(0);
	signal(SIGIO,iofunc);
    }
#endif /* SIGIO && !DONT_USE_SIGIO */
#endif /* PLAINTERM */
    /*
    ** wait for a complete line in the input buffer
    */
#ifdef NEW
    show_prompt();
    fflush(stdout);
#endif
    while(current_line == 0){
	/*
	** execute the simulator jobs which would include
	** the x interface and the main simulator
	*/
	ExecuteJobs();
	ExecuteCleanups();
    }
#ifdef STATUSLINE
    if(insert_mode){
	StatusMessage("        ",0,0);
    }
#endif /* STATUSLINE */
    /*
    ** after each command line reset the error count
    */
    ResetErrors();
    /*
    ** find the end of the return line
    */
    eol = endline(input_buffer,size);
    /*
    ** and copy the result into the return line
    */
    strncpy(line,input_buffer,eol);
    line[eol] = '\0'; 
    /*
    ** then shift the buffer down
    */
    BCOPY(input_buffer +eol,input_buffer,MAXCHAR-eol);
    /*
    ** move the line pointers back the appropriate amount
    */
    end_of_buffer -= eol;
    current_loc = end_of_buffer;
    /*
    ** check for history substitution
    */
    /*
    ** HISTORY RECALL
    */
    if(line[0] == '!'){
	strcpy(new_line,line);
	/*
	** append the history to the new line
	*/
	strcpy(new_line,GetHistory(line));
	/*
	** find the end of the history indicator
	*/
	if(line[1] == '!'){
	    /*
	    ** append the remainder of the line
	    */
	    strcat(new_line,line+2);
	    strcpy(line,new_line);
	} else {
	    ptr = line;
	    while(!IsWhiteSpace(*ptr) && *ptr != '\0') ptr++;
	    strcat(new_line, ptr);
	    strcpy(line,new_line);
	}
	/*
	** echo the line
	*/
	lprintf("%s",line);
    } 
    /*
    ** send back the result
    */
    if(!EmptyString(line)){
	inc_prompt();
	AddHistory(line);
	lprint_only("%s",line);
    }
    return(line);
}

/*
** find the end of a return line in the input buffer and 
** return the location of the eol
*/

int endline(s,size)
char	*s;
int	size;
{
int eol;
int i;

    if((eol = eol_buffer[0]) >= size){
	eol = size-1;
    }
#ifdef OLD
    /*
    ** munch the CR
    */
    if(eol > 0 && input_buffer[eol-1] == '\n'){
	s[eol-1] = '\0';
    } 
#endif
    current_line--;
    /*
    ** and adjust the current line pointers if there are multiple lines
    ** in the buffer
    */
    for(i=0;i<current_line;i++){
	eol_buffer[i] = eol_buffer[i+1] - eol;
    }
    return(eol);
}

char *GetLineFp(line,fp,size)
char 	*line;
FILE  *fp;
int		size;
{
int		eol;
char	*nbgets();

    if(fp == stdin){
	if(nbgets(line,size,fp) == NULL) line[0] = '\0';
    } else 
    if(fp == NULL){
	line[0] = '\0';
    } else
    if(fgets(line,size,fp) == NULL) {
	NextScript();
	line[0] = '\0';
#ifndef OLDTRACE
	return(NULL);
#endif
    }
    if((eol = ((int)strlen(line) - 1)) >= 0){
#ifndef NEW
	if(line[eol] == '\n') line[eol] = '\0';
#endif
    }
    return(line);
}

char *sgets(line,size)
char *line;
int size;
{
FILE	*CurrentScriptFp();
Script	*script;
Script	*CurrentScript();

    if(GetLineFp(line,CurrentScriptFp(),size) == NULL){
	return(NULL);
    }
    if ((script = CurrentScript()))
	if (strchr(line,'\n'))
	    script->line++;
    return(line);
}

int GetLine(line,script,size)
char 	*line;
Script  *script;
int		size;
{
int		eol;

    switch(script->type){
    case FILE_TYPE:
	GetLineFp(line, (FILE*)(script->ptr),size);
	script->line++;
	return(!feof((FILE *)(script->ptr)));
    case STR_TYPE:
	eol = 0;
	if(*script->current == '\0') return(0);
	while(*script->current != '\0' && *script->current != '\n'){
	    line[eol++] = *script->current++;
	}
	line[eol] = '\0';
	if(*script->current != '\0') script->current++;
	script->line++;
    }
    return(1);
}

int EmptyLine(s)
char *s;
{
    while(*s != '\0' && (*s == ' ' || *s == '\t' || *s == '\n')) s++;
    if(*s == '\0') return(1);
    return(0);
}

int EmptyString(string)
char *string;
{
int i;

    if(string != NULL && string[0] != '\0'){
	for(i=0;i<strlen(string);i++){
	    if(!IsWhiteSpace(string[i])) return(0);
	}
    }
    return(1);
}

