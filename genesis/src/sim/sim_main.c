static char rcsid[] = "$Id: sim_main.c,v 1.6 2005/10/16 20:48:35 svitak Exp $";

/*
** $Log: sim_main.c,v $
** Revision 1.6  2005/10/16 20:48:35  svitak
** Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
** memory instead. This was only done where directory paths might have easily
** exceeded 100 characters.
**
** Revision 1.5  2005/08/12 01:01:05  ghood
** Added InitPar/FinalizePar calls so that MPI_Init/MPI_Finalize can be called at appropriate times in PGENESIS.
**
** Revision 1.4  2005/06/27 22:23:52  svitak
** Quick change to copyright banner.
**
** Revision 1.3  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.2  2005/06/26 08:24:21  svitak
** Interpreter was being called with -1 as a parameter but no parameters
** are declared... -1 removed.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.30  2003/03/28 20:43:33  gen-dbeeman
** *** empty log message ***
**
** Revision 1.29  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.28  2001/03/17 20:33:36  mhucka
** Updated copyright year.
**
** Revision 1.27  2000/09/08 23:21:10  mhucka
** Fiddled with the formatting of the startup banner.
**
** Revision 1.26  2000/06/19 06:09:02  mhucka
** The Script structure and the handling of scripts had this bogosity in which
** FILE pointers were being cast to char * and back again.  Why on earth
** didn't the authors just keep a separate pointer in the Script structure for
** FILE pointers, instead of shoehorning the pointers into the .ptr char *
** field?  Anyway, for better robustness and portability, I reimplemented this
** part using two separate pointers.
**
** Revision 1.25  2000/05/19 18:43:21  mhucka
** Reformatted the welcome message slightly.
**
** Revision 1.24  2000/05/01 18:07:29  mhucka
** It doesn't need to do a sleep(2) at startup.  Bilitch speculates on the
** reason why it was there: "My guess is that its a short pause to allow
** the user to see the copyright message before the startup continues.  The
** delay was likely necessary back in the days when each library would echo
** its name as it loaded making the copyright scroll off the screen (albeit
** slowly)."
**
** Revision 1.23  2000/04/20 07:27:08  mhucka
** Updated copyright date.
**
** Revision 1.22  2000/03/27 10:34:44  mhucka
** Commented out statusline functionality, because it caused GENESIS to
** print garbage control characters upon exit, screwing up the user's
** terminal.  Also added return type declaractions for various things.
**
** Revision 1.21  1999/11/28 07:49:23  mhucka
** Updated copyright dates.
**
** Revision 1.20  1997/08/08 19:33:23  dhb
** Update copyright message to include 1997.
**
** Revision 1.19  1996/11/02 00:34:16  dhb
** Updated copyright year
**
 * Revision 1.18  1996/09/18  18:32:02  dhb
 * Mod to check for simrc file in execdir rather than cwd where we
 * ran genesis from when execdir is given.
 *
 * Revision 1.17  1996/07/29  23:10:21  dhb
 * It is now a fatal error to be missing a .simrc file.  User is
 * instructed to copy one from the startup directory.
 *
 * Added escaping of double quotes in startup banner info on help.
 *
 * Revision 1.16  1996/07/19  23:15:59  dhb
 * Added message to startup banner to use help command for GENESIS
 * help.
 *
 * Revision 1.15  1995/08/02  21:40:55  dhb
 * Added -nox genesis command line option.
 *
 * Revision 1.14  1995/07/14  00:13:02  dhb
 * The .simrc file and command line specified script execution can now
 * be interrupted.  Errors in .simrc now result in entering interactive
 * mode unless we're in batch mode.
 *
 * Revision 1.13  1995/05/31  19:25:38  dhb
 * Added -defaultcolor command line option to force explicit use of
 * default colortable.
 *
 * Revision 1.12  1995/04/28  21:04:38  dhb
 * Added -execdir and -nice options and removed the old -load
 * option.  Beefed up the error checking on options which take
 * an additional argument.  Also added -notty along with the
 * new options to the usage message.
 *
 * Revision 1.11  1995/04/05  01:27:26  dhb
 * Updated copyright banner.
 *
 * Revision 1.10  1995/04/01  17:38:28  dhb
 * Removed include of sim_header.h ... already included by sim_defs.h
 * via sim.h.
 *
 * Revision 1.9  1995/04/01  17:11:37  dhb
 * Fixed bug in SetScript() call from main() when called for the
 * simrc.
 *
 * Change handling of line buffering under -batch option for SYSV
 * systems as setlinebuf() is a BSDism.  Left setlinebuf() for BSD
 * even though setvbuf() seems to be on all the systems we have.
 *
 * Revision 1.8  1995/01/24  21:06:46  dhb
 * Added setting of line buffering on stdout when -batch option
 * is given.  This synchonizes stdout and stderr output.
 *
 * Revision 1.7  1994/10/25  23:51:31  dhb
 * Added -notty option to genesis command line options.  Will avoid all
 * tty related setup code and use of iofunc to poll for tty input.  All
 * calls to get input from stdin will return no input.
 *
 * Revision 1.6  1993/10/15  18:29:31  dhb
 * Reordered include statements to make Solaris patches work.
 *
 * Revision 1.5  1993/09/17  18:26:07  dhb
 * Now includes "system_deps.h".
 * This is part of the Solaris compatability changes.  system_deps.h
 * defines a setjmp macro to replace the setjmp call with a call to
 * sigsetjmp().
 *
 * Revision 1.4  1993/08/12  20:48:39  dhb
 * Changed startup banner to use VERSIONSTR to print the GENESIS version.
 *
 * Revision 1.3  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.2  1993/07/16  23:43:16  dhb
 * Version change to 1.4.1
 *
 * Revision 1.1  1992/10/27  20:15:31  dhb
 * Initial revision
 *
*/

#ifdef sgi
#include <malloc.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include "sim.h"
#include "shell_func_ext.h"
#include "ss_func_ext.h"
#include "system_deps.h"
#include "par_ext.h"

/* Modified Feb 1992 by Upi Bhalla: added silent flag to command line */

char	**global_envp;
jmp_buf main_context;


/* GENESIS command line option variables */

int	GENOPT_UseDefaultColor = 0;
int	GENOPT_NoX = 0;


void DisplayHeader()
{
printf("==========================================================================\n");
printf("                               G E N E S I S\n");
printf("                           Release Version: %s\n\n", VERSIONSTR);
printf("\n");
printf("GENESIS is made available under the GNU General Public License,\n");
printf("GNU Library General Public License, or by the originating institution \n");
printf("with the permission of the authors.\n");
printf("\n");
printf("Type \"help\" for help with GENESIS.\n");
printf("==========================================================================\n\n");
}

void usage(s)
char *s;
{
#ifdef STATUSLINE
    printf("usage : %s [-usage][-nosimrc][-altsimrc filename][-notty][-nosig][-execdir dirname][-nice nice-level][-nox][-defaultcolor][-batch][-status][script arg1 ...]\n",s);
#else
    printf("usage : %s [-usage][-nosimrc][-altsimrc filename][-notty][-nosig][-execdir dirname][-nice nice-level][-nox][-defaultcolor][-batch][script arg1 ...]\n",s);
#endif
}

int main (argc, argv,envp)
int     argc;
char   *argv[];
char	**envp;
{
extern int LOAD_LIBRARIES();     
short     nxtarg;
FILE	*pfile;
char	*pname;
char	*execdir;
int	file_arg = 0;
short catch = 1;
char *home;
char string[256];
char *getenv();
#ifdef STATUSLINE
int status_line=0;
#endif
int use_simrc;
char simrc_name[256];

#ifdef sgi
    mallopt(M_KEEP, 1);
#endif

    InitPar(&argc, &argv, &envp);

    nxtarg = 0;
    use_simrc = 1;
	strcpy(simrc_name,".simrc");
    global_envp = envp;
    pname = NULL;
    execdir = NULL;
    while(++nxtarg < argc){
	if(arg_is("-defout")){
	    freopen(argv[++nxtarg],"w",stdout);
	} else 
	if(arg_is("-nosig")){
	    catch = 0;
	} else 
	/* Upi Modification Feb 1992 
	** Level 0,1 : Same effects as before
	** Level 2   : Silences the startup stuff, on all nodes.
	** Level 3   : (for parallel machines) All but node 0 are silent
	*/
	if(arg_is("-silent")){
	    nxtarg++;
	    SetSilent(atoi(argv[nxtarg]));
	} else 
	if(arg_is("-batch")){
	    SetBatchMode(1);
	    catch = 0;
#ifdef SYSV
	    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
#else
	    setlinebuf(stdout);
#endif
	} else 
	if(arg_is("-notty")){
	    SetTtyMode(0);
	} else 
#ifdef STATUSLINE
	if(arg_is("-status")){
	    status_line = 1;
	} else 
#endif /* STATUSLINE */
	if(arg_is("-nosimrc")){
	    use_simrc = 0;
	} else 
	if(arg_is("-altsimrc")){
	    nxtarg++;
	    if (nxtarg < argc){
		strcpy(simrc_name,argv[nxtarg]);
	    } else {
		fprintf(stderr, "-altsimrc option missing a filename\n");
	    }
	} else 
	if(arg_is("-execdir")){
	    nxtarg++;
	    if (nxtarg < argc){
		execdir = argv[nxtarg];
	    } else {
		fprintf(stderr, "-execdir option missing dirname\n");
	    }
	} else 
	if(arg_is("-nice")){
	    nxtarg++;
	    if (nxtarg < argc){
		int	nice_level;

		nice_level = atoi(argv[nxtarg]);
		if (nice(nice_level) == -1)
		    perror("-nice");
	    } else {
		fprintf(stderr, "-nice option missing nice_level\n");
	    }
	} else
	if(arg_is("-nox")){
	    GENOPT_NoX = 1;
	} else
	if(arg_is("-defaultcolor")){
	    GENOPT_UseDefaultColor = 1;
	} else
	if(arg_is("-usage")){
	    usage(argv[0]);
	    return(0);
	} else {
	    pname = argv[nxtarg];			
	    file_arg = nxtarg;
	    break;
	}
    }

    /*
    ** Try to find a simrc if we're using one.
    ** It'll be read below after initialization.
    */

    pfile = NULL;
    if(use_simrc){
	/*
	** first try and find it locally in execdir if given else in
	** current working directory
	*/
	if (execdir != NULL)
	    sprintf(string, "%s/%s", execdir, simrc_name);
	else
	    strcpy(string,simrc_name);

	if((pfile=fopen(string,"r")) == NULL){
	    /*
	    ** else try and find it in the home directory
	    */
	    if ((home = getenv("HOME"))) {
		sprintf(string,"%s/%s",home,simrc_name);
		pfile=fopen(string,"r");
	    }
	}
	if(pfile == NULL){
	    fprintf(stderr, "Cannot find a simrc file in the execdir/working or\n");
	    fprintf(stderr, "home directories.  Copy one from startup/.simrc\n");
	    fprintf(stderr, "in the GENESIS installation directory and try\n");
	    fprintf(stderr, "again or see the README in the same location.\n");
	    return(1);
	}
    }

    /* Moved down - Upi Feb 92 */
    if (IsSilent()<1) {
    	printf("Starting Genesis\n");
    	fflush(stdout);
    }

    /*
    ** get input from stdin
    */
    if(!IsBatchMode()){
	if (IsSilent() < 2)
	    DisplayHeader();
    }

    /*
    ** initialize the command interpreter
    */
    SetDefaultPrompt("genesis #!");
	set_float_format("%0.10g");
	sim_set_float_format("%0.10g");

    if(!SetupInterp(argv[0])){
    	if (IsSilent() < 2) {
	    printf("Unable to find the simulator. ");
	    printf("Check your PATH environment variable\n");
	}
	return(0);
    };

    /*
    ** If an alternate execution directory is specified change to
    ** the given directory.
    */

    if (execdir != NULL)
	if (chdir(execdir) == -1)
	    perror("-execdir");

    /*
    ** initialization for the base simulator
    */
    SimStartup();

    /*
    ** load the symbolic information for the rest of the libraries
    */
    LOAD_LIBRARIES();

    terminal_setup();

    if(catch){
	SetSignals();
    }

    /*
    ** try reading from the startup file
    */
	if(pfile){
	    char*	rc_argv[2];

	    if(IsSilent()<1) printf("%-20s%s\n","Startup script:",string);
	    /*
	    * get input from .simrc
	    */
	    /*
	    AddScript(NULL, pfile, 0, NULL, FILE_TYPE);
	    */

	    /*
	    ** set the return point for context jumps during startup script.
	    ** if startup script fails we exit.
	    */
	    if(setjmp(main_context) == 0){
		ParseInit();
		rc_argv[0] = string; /* simrc script name w/path */
		rc_argv[1] = NULL;
		SetScript(NULL, pfile, 1, rc_argv, FILE_TYPE);
		Interpreter();
	    } else {
		/*
		** coming back from a longjump
		*/
		ClearScriptStack();
		tset();
		printf("Error running startup script!\n");
		if(catch){
		    SetSignals();
		}
		pname = NULL; /* don't run command line script */
	    }
	}

    /*
    ** if a script has been specified on the command line
    ** then read it in
    ** using the remaining command line args as args to the file
    */
    if(pname != NULL) {
	if((pfile=SearchForScript(pname,"r")) == NULL){
	    Error();
    	    if (IsSilent() < 2) 
	   	 printf("can't open parameter file %s\n",pname);
	} else {
	    if(IsSilent()<1) printf("%-20s%s\n","Simulation Script:",pname);
	    /*
	    ** try reading from the parameter file
	    */
	    /*
	    AddScript(NULL, pfile, argc - file_arg, argv + file_arg, FILE_TYPE);
	    */

	    /*
	    ** set the return point for context jumps during command
	    ** line script.  If an error causes a longjmp() we let the
	    ** user continue.  This gives him a chance to investigate
	    ** the problem if he is interactive.
	    */
	    if(setjmp(main_context) == 0){
		ParseInit();
		SetScript(NULL, pfile, argc - file_arg, argv + file_arg,
			  FILE_TYPE);
		Interpreter();
	    } else {
		ClearScriptStack();
		RemoveSimulationJob();
		tset();
		if(catch){
		    SetSignals();
		}
	    }
	}
    }

    /*
    ** if it is batch mode then dont bother going into
    ** interactive mode
    */
    if(!IsBatchMode()){
	SetScript(NULL, stdin, 0, NULL, FILE_TYPE);
#ifdef STATUSLINE
	if(status_line){
	    EnableStatusLine();
	}
#endif /* STATUSLINE */
	/*
	** put the terminal into non-blocking custom io mode
	terminal_setup();
	*/
	/*
	** set the return point for context jumps
	*/
	if(setjmp(main_context) != 0){
	    /*
	    ** coming back from a longjump
	    */
	    ClearScriptStack();
	    RemoveSimulationJob();
	    tset();
	}

	ParseInit();

	if(catch){
	    SetSignals();
	}
	/*
	** go to the interactive interpreter level
	*/
    }
    EnableHistory(1);
    AlternatePrompt(NULL);
    Interpreter();

    FinalizePar();

    return 0;
}
