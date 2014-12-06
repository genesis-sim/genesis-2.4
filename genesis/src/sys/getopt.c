static char rcsid[] = "$Id: getopt.c,v 1.6 2005/07/07 19:12:35 svitak Exp $";

/*
** $Log: getopt.c,v $
** Revision 1.6  2005/07/07 19:12:35  svitak
** Removed calls to TraceScript() because 'convert' uses G_getopt but
** including all the libraries/files necessary for TraceScript (which
** accesses globals defined in ss/eval.c) eventually causes linkage
** conflicts with lex and yacc generated functions.
**
** Revision 1.5  2005/07/01 10:03:09  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.4  2005/06/25 21:25:45  svitak
** printoptusage() is used everywhere. Created a new file for it's declaration
** and included it where appropriate.
**
** Revision 1.2  2005/06/20 21:20:15  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.11  2000/09/21 19:45:36  mhucka
** Added parens around assignments inside if/then conditionals to please gcc.
**
** Revision 1.10  2000/06/12 04:55:56  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.9  1997/07/18 03:14:53  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.8  1995/04/13 00:17:53  dhb
** Added include of header.h so that defining GETOPT_PROBLEM will
** rename getopt() and optopt to G_getopt() and G_optopt.
**
 * Revision 1.7  1994/09/02  18:23:30  dhb
 * Added special check to getopt() for -help and -usage options to commands.
 * If an option does not match a command option but does match -help or
 * -usage, then getopt() returns error codes -5 and -6 respectively without
 * issuing any error meesage.  The command will print its usual usage message.
 *
 * This also allows the command to override the special handling by providing
 * its own -help or -usage options.
 *
 * Revision 1.6  1993/06/29  19:08:59  dhb
 * Added savopt() and restoropt() routines to save state of options
 * processing.  Allows for nested use of getopt routines.
 *
 * Revision 1.5  1993/04/23  23:02:39  dhb
 * 1.4 mods were horrendously bogus.  Reimplemented.
 *
 * Revision 1.4  1993/03/19  20:59:38  dhb
 * Modified initopt to get optargv and optargc to refer to the command
 * arguments.  This allows processing of command arguments before command
 * options.
 *
 * Revision 1.3  1993/02/12  22:33:12  dhb
 * Added printoptusage() function
 * Added code to consider args starting with '[' as optional.  This is done
 * by not counting the arg as a static arg and setting the additional args
 * flag.  This leaves it up to the caller to check for correct number of args
 * for non-static args.  Note: args to options also work this way.
 *
 * Revision 1.2  1993/02/12  18:16:04  dhb
 * Various bug fixes: OPTLIKE checks and off by one errors in arg scanning
 *
 * Revision 1.1  1993/02/04  22:52:02  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <string.h>
#include "header.h"

#define OPTLIKE(arg) \
	(arg[0] == '-' && 'a' <= arg[1] && arg[1] <= 'z')

int	G_optind;
int	G_opterror;
char*	G_optopt;
int	optargc;
char**	optargv;

typedef struct _opt
  {
    char*	name;
    int		staticargs;
    int		additionalargs;
    struct _opt	*next;
  } Opt;

typedef struct _cmd
  {
    char*	mem;
    char*	optstring;
    int		staticargs;
    int		additionalargs;
    Opt*	options;
  } Cmd;

static Cmd optcmd;

static void uninitopt()

{	/* uninitopt --- Free memory from previous options processing */

	if (optcmd.mem != NULL)
	  {
	    free(optcmd.mem);
	    optcmd.mem = NULL;
	  }

	while (optcmd.options != NULL)
	  {
	    Opt*	opt;

	    opt = optcmd.options;
	    optcmd.options = opt->next;
	    free(opt);
	  }

}	/* uninitopt */



int initopt(argc, argv, optstring)

int	argc;
char*	argv[];
char*	optstring;

{	/* initopt --- Digest optstring and prepare to scan options */

	char*	mem;
	char*	arg;
	char*	CopyString();
	int	argcount;

	uninitopt();

	mem = CopyString(optstring);
	if (mem == NULL)
	  {
	    perror("initopt");
	    return -1;
	  }

	optcmd.optstring = optstring;
	optcmd.mem = mem;
	optcmd.options = NULL;
	optcmd.staticargs = 0;
	optcmd.additionalargs = 0;

	for (argcount = 1; argcount < argc; argcount++)
	  {
	    if (OPTLIKE(argv[argcount]))
		break;
	  }

	optargv = argv;
	optargc = argcount;

	arg = strtok(mem, " \t\n");
	while (arg != NULL)
	  {
	    if (OPTLIKE(arg))
	      {
		Opt*	opt;

		opt = (Opt *) malloc(sizeof(Opt));
		if (opt == NULL)
		  {
		    perror("initopt");
		    uninitopt();
		    return -1;
		  }

		opt->name = arg;
		opt->staticargs = 0;
		opt->additionalargs = 0;
		opt->next = optcmd.options;
		optcmd.options = opt;

		arg = strtok(NULL, " \t\n");
		while (arg != NULL)
		  {
		    if (OPTLIKE(arg))
			break;

		    if (strcmp(arg, "...") == 0 || arg[0] == '[')
			opt->additionalargs = 1;
		    else
			opt->staticargs++;

		    arg = strtok(NULL, " \t\n");
		  }
	      }
	    else
	      {
		if (strcmp(arg, "...") == 0 || arg[0] == '[')
		    optcmd.additionalargs = 1;
		else
		    optcmd.staticargs++;

		arg = strtok(NULL, " \t\n");
	      }
	  }

	G_optind = 1;
	while (G_optind < argc && !OPTLIKE(argv[G_optind]))
	    G_optind++;
	G_opterror = 0;

	return 0;

}	/* initopt */


/*
** G_getopt
**
**	Gets the next option and associated arguments from argv.  Return
**	values are:
**
**		-1	Unknown or ambiguous option
**		-2	Incorrect number of option arguments
**		-3	Incorrect number of command arguments
**		-4	G_getopt called before initopt or after all options
**			  have been processed
**		-5	G_getopt found -help and does not match an option
**		-6	G_getopt found -usage and does not match an option
**		 0	End of command options, optargv contains the
**			  command arguments
**		 1	Valid option and arguments, optargv contains the
**			  option arguments, G_optopt contains the full option
**			  name
*/

int G_getopt(argc, argv)

int	argc;
char**	argv;

{	/* G_getopt --- Get next option with associated args from argv */
    
	Opt*	found;
	Opt*	opt;
	size_t	len;

	if (optcmd.mem == NULL)
	  {
	    fprintf(stderr, "G_getopt: called before initopt or after all arguments have been processed (this is a bug)\n");
	    fprintf(stderr, "        command == '%s'\n", argv[0]);
	    return -4;
	  }

	if (G_optind >= argc)
	  {
	    uninitopt();

	    optargv = argv;
	    optargc = 1;
	    while (optargc < argc && !OPTLIKE(argv[optargc]))
		optargc++;

	    if (optargc-1 < optcmd.staticargs)
	      {
		fprintf(stderr, "%s: Too few command arguments\n", argv[0]);
		return -2;
	      }

	    if (optargc-1 > optcmd.staticargs && !optcmd.additionalargs)
	      {
		fprintf(stderr, "%s: Too many command arguments\n", argv[0]);
		return -2;
	      }

	    return 0;
	  }

	if (!OPTLIKE(argv[G_optind]))
	  {
	    fprintf(stderr, "%s: Expecting a command option, found '%s'\n",
			argv[0], argv[G_optind]);
	    return -1;
	  }

	len = strlen(argv[G_optind]);
	found = NULL;
	for (opt = optcmd.options; opt != NULL; opt = opt->next)
	  {
	    if (strncmp(opt->name, argv[G_optind], len) == 0) {
		if (found != NULL)
		  {
		    fprintf(stderr, "%s: Ambiguous command option '%s'\n",
			    argv[0], argv[G_optind]);
		    return -1;
		  }
		else
		    found = opt;
	    }
	  }

	if (found == NULL)
	  {
	    if (strncmp("-help", argv[G_optind], len) == 0)
		return -5;

	    if (strncmp("-usage", argv[G_optind], len) == 0)
		return -6;

	    fprintf(stderr, "%s: Unknown command option '%s'\n",
		    argv[0], argv[G_optind]);
	    return -1;
	  }

	G_optopt = found->name;
	optargv = argv+G_optind;

	optargc = 0;
	for (G_optind++; G_optind < argc && !OPTLIKE(argv[G_optind]); G_optind++)
	    optargc++;

	if (optargc < found->staticargs)
	  {
	    fprintf(stderr, "%s: Too few arguments to command option '%s'\n",
		    argv[0], G_optopt);
	    return -2;
	  }

	if (optargc > found->staticargs && !found->additionalargs)
	  {
	    fprintf(stderr, "%s: Too many arguments to command option '%s'\n",
		    argv[0], G_optopt);
	    return -2;
	  }

	optargc++; /* for the command option itself */

	return 1;

}	/* G_getopt */

void printoptusage(argc, argv)

int	argc;
char**	argv;

{	/* printoptusage --- Print a usage statement */

	if (optcmd.optstring != NULL)
	    fprintf(stderr, "usage: %s %s\n", argv[0], optcmd.optstring);

}	/* printoptusage */

typedef struct
  {
    int		optind;
    int		opterror;
    char*	optopt;
    int		optargc;
    char**	optargv;
    Cmd		cmd;
  } SaveOpts;

void *savopt()
{
        SaveOpts*	so;

	so = (SaveOpts*) malloc(sizeof(SaveOpts));
	if (so != NULL)
	  {
	    so->optind = G_optind;
	    so->opterror = G_opterror;
	    so->optopt = G_optopt;
	    so->optargc = optargc;
	    so->optargv = optargv;
	    so->cmd = optcmd;
	  }

	optcmd.mem = NULL;
	optcmd.options = NULL;

	return (void*) so;
}

void restoropt(so)

SaveOpts* so;

{
	if (so != NULL)
	  {
	    G_optind = so->optind;
	    G_opterror = so->opterror;
	    G_optopt = so->optopt;
	    optargc = so->optargc;
	    optargv = so->optargv;
	    optcmd = so->cmd;

	    free(so);
	  }
}
