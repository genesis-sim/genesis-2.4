static char rcsid[] = "$Id: shell_error.c,v 1.3 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_error.c,v $
** Revision 1.3  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.2  2005/06/20 21:20:14  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/23 22:40:40  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:27  dhb
** Initial revision
**
*/

#include "shell_ext.h"
#include "sim_func_ext.h"
#include "ss_func_ext.h"

/* Modified by Upi Bhalla Feb 1992 : Added parallel options to
** silent stuff */

static int warning_cnt = 0;
static int error_cnt = 0;
#ifdef i860
static int silent = 2;
#else
static int silent = 0;
#endif
static int max_warnings = 20;
static int max_errors = 10;

/* Upi Modification Feb 1992
** Level 0,1 : Same effects as before
** Level 2   : Silences the startup stuff, on all nodes.
** Level 3   : (for parallel machines) All but node 0 are silent
*/

void SetSilent(x) 
int x; 
{
#ifdef i860
	if (x==3 && do_realmynode(0,0)==0)
		silent=0;
	else
    	silent = x; 
#else
   	silent = x; 
#endif
}


int IsSilent()
{
    return((silent==0) ? InControlStructure() : silent);
}

int do_silent(argc,argv)
int	argc;
char	**argv;
{

    initopt(argc, argv, "[level]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return -1;
      }

    if(optargc == 2){
	silent =atoi(optargv[1]);
    }

    return(silent);
}


void MaxWarnings(i)
int i;
{
    max_warnings = i;
}

void MaxErrors(i)
int i;
{
    max_errors = i;
}

void do_set_maxwarnings(argc,argv)
int argc;
char **argv;
{

    initopt(argc, argv, "[#]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc == 2){
	max_warnings = atoi(optargv[1]);
    }
    if(!silent)
	printf("max warnings = %d\n",max_warnings);
}

void do_set_maxerrors(argc,argv)
int argc;
char **argv;
{

    initopt(argc, argv, "[#]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc == 2){
	max_errors = atoi(optargv[1]);
    }
    if(!silent)
	printf("max errors = %d\n",max_errors);
}

void ResetErrors()
{
    /*
    ** ResetErrors is called from various places in the simulator as well
    ** as being user callable from the shell.  For now, we won't add getopt
    ** code.
    */

    warning_cnt = 0;
    error_cnt = 0;
}

void Warning()
{
    if(!silent){
	TraceScript();
	printf("* Warning - ");
    }
    warning_cnt++;
    if(warning_cnt > max_warnings){
	printf("Exceeded the maximum number of warnings (%d)\n",
	max_warnings);
	warning_cnt = 0;
	restore_context();
    }
}

void Error()
{
    TraceScript();
    printf("** Error - ");
    error_cnt++;
    if(error_cnt > max_errors){
	printf("Exceeded the maximum number of errors (%d)\n",
	max_errors);
	error_cnt = 0;
	restore_context();
    }
}

void DisplayErrors()
{
    printf("\n");
    printf("%d Warnings\n",warning_cnt);
    printf("%d Errors\n",error_cnt);
    printf("\n");
}

void yyerror(s)
char *s;
{
    Error();
    printf("%s\n",s);
    restore_context();
}
