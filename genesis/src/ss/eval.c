static char rcsid[] = "$Id: eval.c,v 1.7 2006/02/15 18:50:12 svitak Exp $";

/*
** $Log: eval.c,v $
** Revision 1.7  2006/02/15 18:50:12  svitak
** Changes to make asc_file objects use float_format.
**
** Revision 1.6  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.5  2005/06/25 21:25:45  svitak
** printoptusage() is used everywhere. Created a new file for it's declaration
** and included it where appropriate.
**
** Revision 1.4  2005/06/24 21:07:31  svitak
** Fixed type of ExecuteFunction (void back to int).
**
** Revision 1.3  2005/06/20 21:20:15  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.2  2005/06/20 19:31:24  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.42  2003/05/29 22:34:11  gen-dbeeman
** Replaced C++ style comments for K&R compatiblility
**
** Revision 1.41  2003/03/28 20:59:43  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
**
** Revision 1.40  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.38  2000/10/09 23:01:40  mhucka
** Portability fixes.
**
** Revision 1.37  2000/09/21 19:42:37  mhucka
** 1) Wrapped definition of TRUE/FALSE in #ifndef/endif.
** 2) Added function declarations where appropriate.
** 3) Added {} in some if/then clauses to get gcc to shut up.
**
** Revision 1.36  2000/09/07 05:22:20  mhucka
** Modified behavior so that shell commands are not interpreted unless they
** are the first thing on the command line.  This means that things like
** spelling errors like typing "rm" instead of "Rm" in a command such as
** 'eval {rm * ca}' will not cause shell commands to be executed unexpectedly.
**
** Revision 1.35  2000/07/12 06:13:44  mhucka
** Added #include of malloc.h.
**
** Revision 1.34  2000/06/12 04:55:04  mhucka
** Added a number of typecasts to make the compilers happy.
**
** Revision 1.33  2000/01/28 19:36:30  mhucka
** Fix from MCV: increase buffer size in do_cmd_args from 1000 to 10000.
**
** Revision 1.32  1999/12/02 09:43:29  mhucka
** Added improvement to error message printing by M. Vanier
**
** Revision 1.31  1998/08/28 15:36:28  dhb
** Fixed return value for do_getglobal() under usage error
** condition.
**
** Revision 1.30  1998/04/08 16:12:32  dhb
** Added commands addglobal, setglobal and getglobal so that scripts
** may manipulate global script variables within script functions and
** in server EXEC calls.
**
** Revision 1.29  1997/07/18 21:46:11  dhb
** Fixed lax checking of returned arglist values from commands; now
** check for NULL arglists and yyerror() in this case.
**
** Revision 1.28  1997/07/18 03:01:02  dhb
** FIx for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.27  1996/07/01 20:09:06  dhb
** Added ExecuteStrFunction() which works just like ExecuteFunction()
** but returns command or function result as a string in static
** storage.
**
** Revision 1.26  1996/05/23  23:16:59  dhb
** t3d/e port
**
** Revision 1.25  1995/11/03  23:38:28  dhb
** Fixed bug in handling of ScriptInfo which was being freed at
** end of statement in parser while the CurLocals still referenced
** the script name.  ScriptInfo's now have a reference count which
** prevents the premature freeing of ScriptInfo's.
**
** Revision 1.24  1995/06/28  17:22:03  dhb
** Added a check on the returned value from function or command
** execution in ExecuteFunction().  If the value is a NULL string
** then avoid the CastToInt() error message and return 0.  This
** condition occurs for any callback issuing a command which can
** return a NULL string.
**
** Revision 1.23  1995/06/16  06:01:10  dhb
** FreeBSD compatibility.
**
** Revision 1.22  1995/05/08  22:36:31  dhb
** Support for adding arg_component_list parse tree value to end of
** a function or command name for execution.
**
** Revision 1.21  1995/04/29  01:25:00  dhb
** Moved include of y.tab.h after parse.h as bison uses the
** YYSTYPE definition from parse.h in y.tab.h.
**
** Revision 1.20  1995/04/14  18:08:09  dhb
** externs for float Atof() were incorrectly typed as returning double.
**
** Revision 1.19  1995/01/24  21:43:05  dhb
** get_glob_val() and GetScriptStr() will now return NULL if the given
** global variable does not exist.
**
** set_script_float() would dump core if given an unknown global
** variable name.  Now checks that the variable exists.
**
** CreateScript...() routines now check that the global is created
** properly.
**
** Revision 1.18  1995/01/24  18:40:45  dhb
** The monadic ~ operator was being evaluated in the same case with
** the diadic bitwise logical operators.  Was causing problem casting
** the right parse node which was NULL.
**
** Revision 1.17  1994/12/02  00:05:45  dhb
** Added elif to if then else construct.
**
** Added ** operator to do pow().
**
** Revision 1.16  1994/10/18  19:38:31  dhb
** PTEval() was returning Results which were already used.  Then PTCall()
** would free memory we are still using.  Modified PTEval() to not return
** a valid Result in cases where the result is used.
**
** Revision 1.15  1994/10/18  18:07:03  dhb
** Needed forward declaration of 'void PTCall()'.  Why'd I have to be
** so formal?
**
** Revision 1.14  1994/10/18  17:56:47  dhb
** Added handling of arglists returned from functions called as part
** of a command argument.
**
** Revision 1.13  1994/10/18  17:28:25  dhb
** Added PTCall() which calls PTEval() to evaluate a parse tree and
** frees the result value returned.
**
** Changed all calls to PTEval() which don't handle the return value
** tp PTCall().
**
** Revision 1.12  1994/10/12  17:20:31  dhb
** Added FreeScriptInfo() and FreePTValues() functions to free script info
** and other parse tree values.
**
** Changes to foreach processing to allow non-string loop variables.
**
** Other memory leak patches.
**
** Added cast to int for conditional expressions (if, while, for).
**
** Revision 1.11  1994/04/01  19:04:31  dhb
** Added missing extern declaration for Atof() in SetScriptStr().
**
** Revision 1.10  1994/03/19  23:12:07  dhb
** Added complete set of functions to get and set global script variables.
**
** Added CompileScriptVars() which generates C code to define the current
** global script variables and initialize them to current values.
**
** Added functions to create script variables.
**
** Revision 1.9  1993/10/12  23:15:59  dhb
** Modified do_func() and ExecuteFunction() to call PTEval() explicitly
** for left and right parse trees.  The FUNCTION node is typed as an
** SL (statement list) but the pn_val is a symbol table instead of
** script file information as in a normal SL.
**
** Revision 1.8  1993/10/05  20:18:36  dhb
** Fixed mistake in application of memory free fix.
**
** Revision 1.7  1993/09/17  18:59:06  dhb
** Fixed accesses to memory which had already been freed.
**
** Revision 1.6  1993/06/29  19:05:35  dhb
** Speed hacks to PushLocalVars/PopLocalVars
**
** String variables cast to float for arithmetic ops
**
** Revision 1.5  1993/03/19  23:50:05  dhb
** Changed ExecuteFunction() to return the result value as an int.
**
** Revision 1.4  1993/03/08  18:39:02  dhb
** Fixed a bug in SLI changes for 2.0.  Handling of SLI operators is cleaner.
**
** Revision 1.3  1993/03/01  17:35:45  dhb
** 1.4 to 2.0 script language changes.
**
** 	Changed strings concatination operator to @.  + is no longer overloaded
** 	based on argument type.
**
** 	All string operands to numerical operators are cast to integer.
**
** 	All operands to bitwise and logical operators are cast to integer.
**
** Revision 1.2  1993/02/23  23:55:24  dhb
** 1.4 to 2.0 command argument changes.
**
** 	do_listglobals changed to use GENESIS getopt routines.
**
** Revision 1.1  1992/12/11  19:05:01  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <setjmp.h>
#include <math.h>
#include <string.h>
#include <strings.h>	/* bcopy */
#include "parse.h"
#include "y.tab.h"
#include "symtab.h"
/* mds3 changes */
#include "shell_func_ext.h"
#include "ss_func_ext.h"
#include "system_deps.h"
#include "printoptusage.h"

extern char*	G_optopt;
extern int	optargc;
extern char**	optargv;
extern int      yyerror();


#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif


static int bGFTrace = FALSE;

static int iTraceIndent = 2;

static char float_format[10];

extern char *strsave();
extern char *CopyString();
extern int   initopt();
extern int   G_getopt();
extern SIGTYPE sig_msg_restore_context();

typedef struct _script_info
  {
    int		si_lineno;
    char	*si_script;
    int		si_refcnt;
  } ScriptInfo;

/*
** eval.c
**
**	Parse tree evaluation module.  This source file uses defines
**	in the yacc program.  For this reason eval.c should be included
**	in the program section of the yacc source file.
*/

extern int errno;
extern char* TokenStr();
extern Symtab GlobalSymbols;

typedef struct _localvars
  {
    int			lv_nargs;
    int			lv_nlocals;
    Result		*lv_vars;
    struct _localvars	*lv_prev;
    char		*lv_function;
    ScriptInfo		*lv_si;
  } LocalVars;
LocalVars	*CurLocals = NULL;


/*
** The following globals are used to implement the RETURN statement.
** When a function or a command is executed, setjmp is called to
** mark the start of function execution.  When RETURN is executed
** the return value (if present) is placed in ReturnResult and we
** longjmp back to the start of function node.
*/

jmp_buf		BreakBuf;
jmp_buf		ReturnBuf;
Result		ReturnResult;
static int	InLoop = 0;
static ScriptInfo ScriptLoc = { 0, "<stdin>", 1 };

/*
** Forward declarations
*/
char* get_glob_val();


int GetFunctionTraceLevel(void)
{
    return(bGFTrace);
}

void SetFunctionTraceLevel(int iLevel)
{
    bGFTrace = !!iLevel;
}

ScriptInfo *MakeScriptInfo()

{	/* MakeScriptInfo --- Return current script anem and line number */

	ScriptInfo *si;
	extern int yerror();

	si = (ScriptInfo *) malloc(sizeof(ScriptInfo));
	if (si == NULL)
	  {
	    yyerror("MakeScriptInfo: out of mem");
	    /* No Return */
	  }

	si->si_lineno = CurrentScriptLine();
	si->si_script = CurrentScriptName();
	if (si->si_script == NULL)
	    si->si_script = "<stdin>";

	si->si_script = (char *) strsave(si->si_script);

	si->si_refcnt = 1;
	return(si);

}	/* MakeScriptInfo */


void RefScriptInfo(si)

ScriptInfo*	si;

{	/* RefScriptInfo --- Count a reference to this ScriptInfo */

	if (si != NULL)
	    si->si_refcnt++;

}	/* RefScriptInfo */


void FreeScriptInfo(si)

ScriptInfo	*si;

{	/* FreeScriptInfo --- Free script info structure */

	if (si != NULL)
	  {
	    si->si_refcnt--;
	    if (si->si_refcnt <= 0)
	      {
		free(si->si_script);
		free(si);
	      }
	  }

}	/* FreeScriptInfo */


void FreePTValues(pn)

ParseNode*	pn;

{	/* FreePTValues --- traverse parse tree freeing pn->pn_val in SLs */

	if (pn == NULL)
	    return;

	switch (pn->pn_val.r_type)
	  {

	  /* Parse nodes with ScriptInfo in the value */
	  case SL:
	  case WHILE: /* FOR shares its ScriptInfo with the init SL */
	  case IF:
	    FreeScriptInfo((ScriptInfo *) pn->pn_val.r.r_str);
	    break;

	  /* Parse nodes with allocated strings in the value */
	  case INCLUDE:
	  case COMMAND:
	  case LITERAL:
	  case STRCONST:
	    free(pn->pn_val.r.r_str);
	    break;

	  case FUNCTION:
	    if (pn->pn_right != NULL)
		free(pn->pn_val.r.r_str);
	    break;

	  case EXPRCALL:
	    PTFree((ParseNode*) pn->pn_val.r.r_str);
	    break;
	  }

	FreePTValues(pn->pn_left);
	FreePTValues(pn->pn_right);

}	/* FreePTValues */


/*
** The following functions are used by the simulator to retrieve
** the result type values to use in returning command values.
*/

int ArgListType()

{	/* ArgListType --- Return ARGLIST result type */

	return(ARGLIST);

}	/* ArgListType */



int IntType()

{	/* IntType --- Return INT result type */

	return(INT);

}	/* IntType */



int FloatType()

{	/* FloatType --- Return FLOAT result type */

    return(FLOAT);

}	/* FloatType */



int StrType()

{	/* StrType --- Return STR result type */

	return(STR);

}	/* StrType */



static int listmatches(ident, wild)

char	*ident;
char	*wild;

{	/* listmatches --- Return TRUE if the ident matches the SHELL wild */

	while (*wild != '\0')
	  {
	    switch (*wild)
	      {

	      case '?':
		if (*ident++ == '\0')
		    return(FALSE);
		break;

	      case '*':
		if (wild[1] == '?')
		  {
		    if (*ident++ == '\0')
			return(FALSE);
		    wild[1] = '*';
		  }
		else
		    while (*ident != wild[1] && *ident != '\0')
			ident++;
		break;

	      case '[':
		wild++;
		while (*wild != *ident)
		  {
		    if (*wild == ']' || *wild == '\0')
			return(FALSE);
		    wild++;
		  }

		while (*wild != ']')
		    wild++;
		ident++;
		break;

	      default:
		if (*ident++ != *wild)
		    return(FALSE);
		break;

	      }
	    wild++;
	  }

	return(*ident == '\0');

}	/* listmatches */



void do_listglobals(argc, argv)

int	argc;
char	*argv[];

{	/* do_listglobals --- List global symbols and their types */

	SymtabEnt	*se;
	int		first;
	int		listfunctions;
	int		listvariables;
	int		arg;
	int		status;

	listfunctions = TRUE;
	listvariables = TRUE;

	initopt(argc, argv, "[global-symbols] -functions -variables");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-functions") == 0)
	      {
		listvariables = FALSE;
		listfunctions = TRUE;
	      }
	    else if (strcmp(G_optopt, "-variables") == 0)
	      {
		listvariables = TRUE;
		listfunctions = FALSE;
	      }
	  }

	if (status < 0)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	first = TRUE;
	for (arg = 1; first || arg < optargc; arg++)
	  {
	    se = GlobalSymbols.sym_entlist;
	    while (se != NULL)
	      {
		if (arg == optargc || listmatches(se->sym_ident, optargv[arg]))
		    switch (se->sym_val.r_type)
		      {

		      case INT:
			if (listvariables)
			    printf("int\t\t%s = %d\n", se->sym_ident, se->sym_val.r.r_int);
			break;

		      case FLOAT:
			if (listvariables)
			    printf("float\t\t%s = %0.15g\n", se->sym_ident,
							se->sym_val.r.r_float);
			break;

		      case STR:
			if (listvariables)
			    printf("str\t\t%s = \"%s\"\n", se->sym_ident,
							se->sym_val.r.r_str);
			break;

		      case FUNCTION:
			if (listfunctions)
			    printf("function\t%s\n", se->sym_ident);
			break;

		      }

		se = se->sym_next;
	      }

	    first = FALSE;
	  }

}	/* do_listglobals */



/*
** InControlStructure
**
**	Return TRUE if we are executing code in a loop.
*/

int InControlStructure()

{	/* InControlStructure --- Return TRUE if looping */

	return(InLoop != 0);

}	/* InControlStructure */



void PushLocalVars(argc, argv, symtab)

int	argc;
char	*argv[];
Symtab	*symtab;

{	/* PushLocalVars --- Allocate a new frame of local variables */

	LocalVars	*localvars;
	Result		*vars;
	int		nlocals;
	int		i;

	localvars = (LocalVars *) calloc(1,sizeof(LocalVars));
	if (localvars == NULL)
	  {
	    perror("PushLocalVars");
	    sig_msg_restore_context(0, errno);
	  }
	localvars->lv_prev = CurLocals;
	CurLocals = localvars;

	if (symtab == NULL || symtab->sym_entlist == NULL ||
	    symtab->sym_entlist->sym_val.r_type != LOCREF)
	    nlocals = 0;
	else
	    nlocals = symtab->sym_entlist->sym_val.r.r_loc.l_offs + 1;
	nlocals += argc;

	localvars->lv_vars = (Result *) malloc(nlocals*sizeof(Result));
	if (localvars->lv_vars == NULL)
	  {
	    perror("PushLocalVars");
	    sig_msg_restore_context(0, errno);
	  }

	localvars->lv_nargs = argc;
	localvars->lv_nlocals = nlocals;
	for (i = 0, vars = localvars->lv_vars; i < nlocals; i++, vars++)
	  {
	    vars->r_type = STR;
	    if (i < argc)
	        /*vars->r.r_str = (char *) strsave(argv[i]);*/
	        vars->r.r_str = argv[i];
	    else
	        vars->r.r_str = strsave("0");
	  }

}	/* PushLocalVars */



void PopLocalVars()

{	/* PopLocalVars --- Deallocate local variable storage */

	LocalVars  *localvars;

	if (CurLocals == NULL) {
		if (IsSilent() < 0)
	    	fprintf(stderr, "PopLocalVars: CurLocals == NULL\n");
	} else {
	    Result	*vars;
	    int		i;

	    for (i = CurLocals->lv_nargs, vars = CurLocals->lv_vars + i;
		 i < CurLocals->lv_nlocals;
		 i++, vars++)
		if (vars->r_type == STR)
		    free(vars->r.r_str);

	    localvars = CurLocals;
	    CurLocals = CurLocals->lv_prev;
	    free(localvars->lv_vars);
	    FreeScriptInfo(localvars->lv_si);
	    free(localvars);
	  }

}	/* PopLocalVars */



/*
** PTArgv
**
**	Return the arguments to a function or script by argument number
*/

char **PTArgv(arg)

int	arg;

{	/* PTArgv --- Return argument to a function or script */

	char	**argv;
	int	i;

	if (CurLocals == NULL)
	  {
	    argv = (char **) malloc(sizeof(char *));
	    if (argv == NULL)
		yyerror("PTArgv: Out of mem");
		/* No Return */

	    *argv = NULL;
	    return(argv);
	  }

	switch(arg)
	  {

	  case -1:	/* Return all args */
	    argv = (char **) malloc(sizeof(char *)*CurLocals->lv_nargs);
	    if (argv == NULL)
		yyerror("PTArgv: Out of mem");
		/* No Return */

	    for (i = 1; i < CurLocals->lv_nargs; i++)
		argv[i-1] = (char *) strsave(CurLocals->lv_vars[i].r.r_str);
	    argv[i-1] = NULL;
	    break;

	  case 0:	/* script or function name */
	    argv = (char **) malloc(sizeof(char *)*2);
	    if (argv == NULL)
		yyerror("PTArgv: Out of mem");
		/* No Return */

	    if (CurLocals->lv_si != NULL)
		if (CurLocals->lv_function == NULL)
		    argv[0] = (char *) strsave(CurLocals->lv_si->si_script);
		else
		    argv[0] = (char *) strsave(CurLocals->lv_function);
	    else
		argv[0] = strsave("");
	    argv[1] = NULL;
	    break;

	  default:	/* Specific argument */
	    argv = (char **) malloc(sizeof(char *)*2);
	    if (argv == NULL)
		yyerror("PTArgv: Out of mem");
		/* No Return */

	    if (arg < 0 || arg >= CurLocals->lv_nargs)
		argv[0] = (char *) strsave("");
	    else
		argv[0] = (char *) strsave(CurLocals->lv_vars[arg].r.r_str);
	    argv[1] = NULL;
	    break;

	  }

	return(argv);

}	/* PTArgv */



/*
** PTArgc
**
**	Return the number of arguments passed to a function or script.
*/

int PTArgc()

{	/* PTArgc --- Return number of args */

	if (CurLocals == NULL)
	    return(0);
	else
	    return(CurLocals->lv_nargs-1);

}	/* PTArgc */



void TraceScript()

{	/* TraceScript --- Provide backtrace of script and function calls */

	LocalVars	*lv;

	if (CurLocals == NULL)
	    return;

	lv = CurLocals;
	while (lv != NULL)
	  {
	    if (lv->lv_si) {
		if (lv->lv_function != NULL)
		    printf("function %s in <%s> line %d", lv->lv_function,
				lv->lv_si->si_script, lv->lv_si->si_lineno);
		else
		    printf("<%s> line %d", lv->lv_si->si_script,
						    lv->lv_si->si_lineno);
	    }

	    lv = lv->lv_prev;
	    if (lv != NULL)
              {
		/* printf(" <- "); */
                /* MCV change; this looks better: */
		printf("\n... ");
              }
	  }
	printf("\n");

}	/* TraceScript */



void SetLine(si)

ScriptInfo	*si;

{	/* SetLine --- Set line number in the current local scope */

	if (CurLocals != NULL)
	  {
	    /* order of the following two statements could be important
	    ** as it might be that si == CurLocals->lv_si.
	    */

	    RefScriptInfo(si);
	    FreeScriptInfo(CurLocals->lv_si);

	    CurLocals->lv_si = si;
	  }
	ScriptLoc = *si;

}	/* SetLine */



static Result *locref(offset)

int		offset;

{	/* locref --- Reference local variable */

	if (CurLocals == NULL)
	    yyerror("locref: NULL CurLocals");

	return(CurLocals->lv_vars + CurLocals->lv_nargs + offset);

}	/* locref */


static Result dollarref(offset)

int		offset;

{	/* dollarref --- Reference function argument */

	Result	r;

	if (CurLocals != NULL && offset < CurLocals->lv_nargs)
	    return(CurLocals->lv_vars[offset]);

	r.r_type = STR;
	r.r.r_str = (char *) strsave("");
	return(r);

}	/* dollarref */



void CastToInt(rp)

Result	*rp;

{	/* CastToInt --- Cast value to integer */

	char	**argv;
	char	*mem;
	char    buf[1000];
	int	i;

	switch(rp->r_type)
	  {

	  case ARGLIST:
	    argv = (char **) rp->r.r_str;
	    if (argv == NULL)
		yyerror("CastToInt: NULL Arglist\n");

	    mem = argv[0];
	    if (mem == NULL)
		yyerror("CastToInt: Empty Arglist\n");

	    if (sscanf(mem, "%d", &rp->r.r_int) != 1)
	      {
		Error();
		printf("CastToInt: Error casting '%s', using 0\n", mem);
		rp->r.r_int = 0;
	      }

	    for (i = 0; argv[i] != NULL; i++)
		free(argv[i]);
	    free(argv);
	    break;
	      
	  case STR:
	    mem = rp->r.r_str;
	    if (mem == NULL)
		yyerror("CastToInt: NULL string");
		/* No Return */
	    if (sscanf(rp->r.r_str, "%d", &rp->r.r_int) != 1)
	      {
		Error();
		printf("CastToInt: Error casting '%s', using 0\n",
					rp->r.r_str);
		rp->r.r_int = 0;
	      }
	    free(mem);
	    break;

	  case INT:
	    break;

	  case FLOAT:
	    rp->r.r_int = (int) rp->r.r_float;
	    break;

	  default:
	    sprintf(buf, "CastToInt: bad result type %s", TokenStr(rp->r_type));
	    yyerror(buf);
	    break;

	  }
	rp->r_type = INT;

}	/* CastToInt */



void CastToFloat(rp)

Result	*rp;

{	/* CastToFloat --- Cast value to float */

	char	**argv;
	char	*mem;
	char    buf[1000];
	int	i;

	switch(rp->r_type)
	  {

	  case ARGLIST:
	    argv = (char **) rp->r.r_str;
	    if (argv == NULL)
		yyerror("CastToFloat: NULL Arglist\n");

	    mem = argv[0];
	    if (mem == NULL)
		yyerror("CastToFloat: Empty Arglist\n");

	    if (sscanf(mem, "%lf", &rp->r.r_float) != 1)
	      {
		Error();
		printf("CastToFloat: Error casting '%s', using 0.0\n", mem);
		rp->r.r_float = 0.0;
	      }

	    for (i = 0; argv[i] != NULL; i++)
		free(argv[i]);
	    free(argv);
	    break;
	      
	  case STR:
	    mem = rp->r.r_str;
	    if (mem == NULL)
		yyerror("CastToFloat: NULL string");
		/* No Return */
	    if (sscanf(rp->r.r_str, "%lf", &rp->r.r_float) != 1)
	      {
		Error();
		printf("CastToFloat: Error casting '%s', using 0.0\n",
					rp->r.r_str);
		rp->r.r_float = 0.0;
	      }
	    free(mem);
	    break;

	  case INT:
	    rp->r.r_float = (double) rp->r.r_int;
	    break;

	  case FLOAT:
	    break;

	  default:
	    sprintf(buf,"CastToFloat: bad result type %s",TokenStr(rp->r_type));
	    yyerror(buf);
	    break;

	  }
	rp->r_type = FLOAT;

}	/* CastToFloat */



void CastToStr(rp)

Result	*rp;

{	/* CastToStr --- Cast value to string */

	char	**argv;
	char	buf[100];
	char	*mem;
	int	i;

	switch(rp->r_type)
	  {

	  case ARGLIST:
	    argv = (char **) rp->r.r_str;
	    if (argv == NULL)
		yyerror("CastToInt: NULL Arglist\n");

	    mem = argv[0];
	    if (mem == NULL)
		yyerror("CastToStr: Empty Arglist\n");

	    rp->r.r_str = mem;
	    rp->r_type = STR;

	    for (i = 1; argv[i] != NULL; i++)
		free(argv[i]);
	    free(argv);
	    return;
	      
	  case STR:
	    if (rp->r.r_str == NULL)
		yyerror("CastToStr: NULL string");
		/* No Return */
	    return;

	  case INT:
	    sprintf(buf, "%d", rp->r.r_int);
	    break;

	  case FLOAT:
		/* sprintf(buf, "%0.20g", rp->r.r_float); */
	    sprintf(buf, float_format, rp->r.r_float);
	    break;

	  default:
	    sprintf(buf, "CastToStr: bad result type %s", TokenStr(rp->r_type));
	    yyerror(buf);
	    break;

	  }

	mem = (char *) malloc(strlen(buf)+1);
	if (mem == NULL)
	  {
	    perror("CastToStr");
	    sig_msg_restore_context(0, errno);
	  }

	strcpy(mem, buf);
	rp->r.r_str = mem;
	rp->r_type = STR;

}	/* CastToStr */



#define	DOOP(swop,rtyp,rfld,oper) case swop: \
				  r.r_type = rtyp; \
				  r.r.rfld = arg1 oper arg2; \
				  break

static Result intop(arg1, op, arg2)

int	arg1;
int	op;
int	arg2;

{	/* intop --- Do integer operation */

	Result	r;
	char    buf[1000];

	switch (op)
	  {

	  case UMINUS:
	    r.r_type = INT;
	    r.r.r_int = -arg1;
	    break;

	  DOOP('*',INT,r_int,*);
	  DOOP('/',INT,r_int,/);
	  DOOP('%',INT,r_int,%);
	  DOOP('+',INT,r_int,+);
	  DOOP('-',INT,r_int,-);

	  case '~':
	    r.r_type = INT;
	    r.r.r_int = ~arg1;
	    break;

	  DOOP('|',INT,r_int,|);
	  DOOP('&',INT,r_int,&);
	  DOOP('^',INT,r_int,^);

	  DOOP(AND,INT,r_int,&&);
	  DOOP(OR,INT,r_int,||);

	  DOOP(LT,INT,r_int,<);
	  DOOP(LE,INT,r_int,<=);
	  DOOP(GT,INT,r_int,>);
	  DOOP(GE,INT,r_int,>=);
	  DOOP(EQ,INT,r_int,==);
	  DOOP(NE,INT,r_int,!=);
	  
	  case '!':
	    r.r_type = INT;
	    r.r.r_int = !arg1;
	    break;

	  default:
	    sprintf(buf, "intop: Unknown or illegal operator %s", TokenStr(op));
	    yyerror(buf);
	    break;;

	  }

	return(r);

}	/* intop */



static Result floatop(arg1, op, arg2)

double	arg1;
int	op;
double	arg2;

{	/* intop --- Do integer operation */

	Result	r;
	char    buf[1000];

	switch (op)
	  {

	  case UMINUS:
	    r.r_type = FLOAT;
	    r.r.r_float = -arg1;
	    break;

	  DOOP('*',FLOAT,r_float,*);
	  DOOP('/',FLOAT,r_float,/);
	  DOOP('+',FLOAT,r_float,+);
	  DOOP('-',FLOAT,r_float,-);
	  DOOP(LT,INT,r_int,<);
	  DOOP(LE,INT,r_int,<=);
	  DOOP(GT,INT,r_int,>);
	  DOOP(GE,INT,r_int,>=);
	  DOOP(EQ,INT,r_int,==);
	  DOOP(NE,INT,r_int,!=);

	  default:
	    sprintf(buf,"floatop: Unknown or illegal operator %s",TokenStr(op));
	    yyerror(buf);
	    break;;

	  }

	return(r);

}	/* floatop */



static Result strop(arg1, op, arg2)

char	*arg1;
int	op;
char	*arg2;

{	/* strop --- Do string operation */

	Result	r;
	char    buf[1000];

	r.r_type = INT;
	switch (op)
	  {

	  case '@':	/* Concatination */
	    r.r.r_str = (char *) malloc(strlen(arg1) + strlen(arg2) + 1);
	    if (r.r.r_str == NULL)
	      {
		perror("strop(@)");
		sig_msg_restore_context(0, errno);
		/* No return */
	      }

	    sprintf(r.r.r_str, "%s%s", arg1, arg2);
	    r.r_type = STR;
	    break;

	  case EQ:
	    r.r.r_int = strcmp(arg1, arg2) == 0;
	    break;

	  case NE:
	    r.r.r_int = strcmp(arg1, arg2) != 0;
	    break;

	  case GT:
	    r.r.r_int = strcmp(arg1, arg2) > 0;
	    break;

	  case GE:
	    r.r.r_int = strcmp(arg1, arg2) >= 0;
	    break;

	  case LT:
	    r.r.r_int = strcmp(arg1, arg2) < 0;
	    break;

	  case LE:
	    r.r.r_int = strcmp(arg1, arg2) <= 0;
	    break;

	  default:
	    sprintf(buf, "strop: Unknown or illegal operator %s", TokenStr(op));
	    yyerror(buf);
	    /* No Return */

	  }

	free(arg1);
	free(arg2);
	return(r);

}	/* strop */


void promote(arg1, arg2)

Result	*arg1;
Result	*arg2;

{	/* promote --- Promote one or other arg if necessary */

	char    buf[1000];
	char	*tmp;

	if ((arg1->r_type == STR && arg1->r.r_str == NULL) ||
	    (arg2->r_type == STR && arg2->r.r_str == NULL))
	    yyerror("promote: NULL string");
	    /* No Return */

	if (arg1->r_type == ARGLIST)
	    CastToStr(arg1);

	if (arg2->r_type == ARGLIST)
	    CastToStr(arg2);

	if (arg2->r_type == 0 || arg1->r_type == arg2->r_type)
	    return;

	switch (arg1->r_type)
	  {

	  case STR:
	    switch (arg2->r_type)
	      {

	      case INT:
		tmp = arg1->r.r_str;
		arg1->r_type = INT;
		if (sscanf(arg1->r.r_str, "%d", &arg1->r.r_int) != 1)
		  {
		    Error();
		    printf("Error promoting '%s' to INT, using 0\n",
						arg1->r.r_str);
		    arg1->r.r_int = 0;
		  }
		free(tmp);
		break;

	      case FLOAT:
		tmp = arg1->r.r_str;
		arg1->r_type = FLOAT;
		if (sscanf(arg1->r.r_str, "%lf", &arg1->r.r_float) != 1)
		  {
		    Error();
		    printf("Error promoting '%s' to FLOAT, using 0\n",
						arg1->r.r_str);
		    arg1->r.r_float = 0.0;
		  }
		free(tmp);
		break;

	      default:
	        sprintf(buf,"promote: Unknown argument type %s",
				TokenStr(arg2->r_type));
		yyerror(buf);
	        /* No Return */

	      }
	    break;

	  case INT:
	    switch (arg2->r_type)
	      {

	      case STR:
		tmp = arg2->r.r_str;
		arg2->r_type = INT;
		if (sscanf(arg2->r.r_str, "%d", &arg2->r.r_int) != 1)
		  {
		    Error();
		    printf("Error promoting '%s' to INT, using 0\n",
						arg2->r.r_str);
		    arg2->r.r_int = 0;
		  }
		free(tmp);
		break;

	      case FLOAT:
		arg1->r_type = FLOAT;
		arg1->r.r_float = (double) arg1->r.r_int;
		break;

	      default:
	        sprintf(buf,"promote: Unknown argument type %s",
				TokenStr(arg2->r_type));
		yyerror(buf);
	        /* No Return */

	      }
	    break;

	  case FLOAT:
	    switch (arg2->r_type)
	      {

	      case STR:
		tmp = arg2->r.r_str;
		arg2->r_type = FLOAT;
		if (sscanf(arg2->r.r_str, "%lf", &arg2->r.r_float) != 1)
		  {
		    Error();
		    printf("Error promoting '%s' to FLOAT, using 0\n",
						arg2->r.r_str);
		    arg2->r.r_float = 0.0;
		  }
		free(tmp);
		break;

	      case INT:
		arg2->r_type = FLOAT;
		arg2->r.r_float = (double) arg2->r.r_int;
		break;

	      default:
	        sprintf(buf,"promote: Unknown argument type %s",
				TokenStr(arg2->r_type));
		yyerror(buf);
	        /* No Return */

	      }
	    break;

	  default:
	    sprintf(buf,"promote: Unknown argument type %s",
			TokenStr(arg1->r_type));
	    yyerror(buf);
	    /* No Return */

	  }

}	/* promote */


static Result do_cmd();

char *do_cmd_args(arg, argc, argv)

ParseNode	*arg;
int		*argc;
char		*argv[];

{	/* do_cmd_args --- Handle arguments to a command */

	Result	r;
	char	**cargv;
	char	*argcp, *arg1, *arg2, *result;

	/*
	** Format argument list for command execution.
	*/

	if (arg == NULL)
	    return(NULL);
 
	switch (arg->pn_val.r_type)
	  {

	  case COMMAND:
	    r = do_cmd(arg->pn_val.r.r_str, arg->pn_left, FALSE);
	    switch (r.r_type)
	      {

	      case INT:
		{
		char buf[100];
		sprintf(buf, "%d", r.r.r_int);
		return(strsave(buf));
		}

	      case FLOAT:
		{
		char buf[100];
		/* sprintf(buf, "%0.20g", r.r.r_float); */
		sprintf(buf, float_format, r.r.r_float);
		return(strsave(buf));
		}

	      case STR:
		if (r.r.r_str == NULL)
		    yyerror("do_cmd_args: NULL string");
		    /* No Return */

		return(r.r.r_str);

	      case ARGLIST:
		cargv = (char **) r.r.r_str;
		if (cargv == NULL)
		    yyerror("do_cmd_args: NULL Arglist");
		    /* no return */

		while (*cargv != NULL)
		  {
		    if (argv == NULL)
			free(*cargv++);
		    else
		      {
			argv[*argc] = *cargv++;
			*argc += 1;
		      }
		  }
		free(r.r.r_str);
		return(NULL);

	      }

	  case ARGLIST:
	    if (argv != NULL)
	      {
		do_cmd_args(arg->pn_left, argc, argv);
		argcp = do_cmd_args(arg->pn_right, argc, argv);
		if (argcp != NULL)
		  {
		    argv[*argc] = argcp;
		    *argc += 1;
		  }
	      }
	    break;

	  case ARGUMENT:
	    arg1 = do_cmd_args(arg->pn_left, argc, argv);
	    arg2 = do_cmd_args(arg->pn_right, argc, argv);

	    if (arg1 == NULL) return(arg2);
	    if (arg2 == NULL) return(arg1);

	    result = (char *) malloc(strlen(arg1) + strlen(arg2) + 1);
	    if (result == NULL)
	      {
		perror("do_cmd_args");
		yyerror("Out of mem");
		/* No Return */
	      }

	    sprintf(result, "%s%s", arg1, arg2);
	    free(arg1);
	    free(arg2);

	    return(result);

	  case LITERAL:
	    return(strsave(arg->pn_val.r.r_str));

	  default:	/* expr or DOLLARARG */
	    r = PTEval(arg);
	    switch (r.r_type)
	      {

	      case INT:
		{
		char buf[100];
	        sprintf(buf, "%d", r.r.r_int);
		return(strsave(buf));
		}

	      case FLOAT:
		{
		char buf[100];
	        /* sprintf(buf, "%0.20g", r.r.r_float); */
	    	sprintf(buf, float_format, r.r.r_float);
		return(strsave(buf));
		}

	      case STR:
		return(r.r.r_str);

	      case ARGLIST:
		cargv = (char **) r.r.r_str;
		if (cargv == NULL)
		    yyerror("do_cmd_args: NULL Arglist");
		    /* no return */

		while (*cargv != NULL)
		  {
		    if (argv == NULL)
			free(*cargv++);
		    else
		      {
			argv[*argc] = *cargv++;
			*argc += 1;
		      }
		  }
		free(r.r.r_str);
		return(NULL);

	      default:
		{
		char *err = "do_cmd_args: Unknown return value type from PTEval: ";
		char *buf = (char *)malloc(strlen(err) + strlen(TokenStr(r.r_type) + 1));
		strcpy(buf, err);
		strcat(buf, TokenStr(r.r_type));
		yyerror(buf);
		/* No Return */
		}

	      }
	    break;

	  }

	return(NULL);

}	/* do_cmd_args */



static Result do_cmd(cmdname, args, do_autoshell)
char*		cmdname;
ParseNode	*args;
short		do_autoshell;
{	/* do_cmd --- Call command and return result */

	int	i;
	int	argc;
	int	saveinloop;
	char	*argv[1000];
	char	argbuf[1000];
	void	TurnOnAutoshell(), TurnOffAutoshell();
	jmp_buf	save;
	Result	r;
	extern Result ExecuteCommand();

	sprintf(argbuf, "%s", cmdname);
	argc = 1;
	argv[0] = argbuf;
	SetAutoshell(FALSE);
	do_cmd_args(args, &argc, argv);
	argv[argc] = NULL;

	BCOPY(ReturnBuf, save, sizeof(jmp_buf));
	saveinloop = InLoop;
	if (setjmp(ReturnBuf) == 0)
	  {
	    SetAutoshell(do_autoshell);
	    r = ExecuteCommand(argc, argv);
	  }
	else
	    r = ReturnResult;
	InLoop = saveinloop;
	BCOPY(save, ReturnBuf, sizeof(jmp_buf));

	for (i = 1; i < argc; i++)
	    free(argv[i]);

	return(r);

}	/* do_cmd */



void do_include_args(script, args)

char		*script;
ParseNode	*args;

{	/* do_include_args --- Set up localvars for included script */

	int	i;
	int	argc;
	int	saveinloop;
	char	*argv[1000];
	char	argbuf[1000];
	jmp_buf	save;

	sprintf(argbuf, "%s", script);
	argc = 1;
	argv[0] = argbuf;
	do_cmd_args(args, &argc, argv);
	argv[argc] = NULL;

	BCOPY(ReturnBuf, save, sizeof(jmp_buf));
	saveinloop = InLoop;
	if (setjmp(ReturnBuf) == 0)
	  {
	    PushLocalVars(argc, argv, (Symtab *)NULL);
	  }
	InLoop = saveinloop;
	BCOPY(save, ReturnBuf, sizeof(jmp_buf));

	for (i = 1; i < argc; i++)
	    free(argv[i]);

}	/* do_include_args */



/*
** ExecuteFunction
**
**	The simulator calls ExecuteFunction to execute a function or a
**	simulator command.  If the function doesn't exist then call
**	ExecuteCommand.
*/

int ExecuteFunction(argc, argv)

int	argc;
char	*argv[];

{	/* ExecuteFunction --- Given argc/argv lookup function and execute */

	jmp_buf		save;
	int		saveinloop;
	Result		*rp;
	Result		r;
	ParseNode	*func;
	extern Result ExecuteCommand();

	if (argc < 1)
	    yyerror("ExecuteFunction: argc < 1!");
	    /* No Return */

	rp = SymtabLook(&GlobalSymbols, argv[0]);
	if (rp == NULL || rp->r_type != FUNCTION)
	    r = ExecuteCommand(argc, argv);
	else
	  {
	    /* trace */

	    if (bGFTrace)
	      {
		int i;

		printf
		  ("%-*.*s",
		   iTraceIndent,
		   iTraceIndent,
		   "                                        ");
		printf("Function %s(",argv[0] ? argv[0] : "Undefined");
		for (i = 1 ; i < argc ; i++)
		  printf("{%s}%s",argv[i],i + 1 == argc ? "" : ",");
		printf(")\n");

		iTraceIndent += 2;
	      }

	    func = (ParseNode *) rp->r.r_str;
	    PushLocalVars(argc, argv, (Symtab *)func->pn_val.r.r_str);

	    BCOPY(ReturnBuf, save, sizeof(jmp_buf));
	    saveinloop = InLoop;
	    InLoop++;
	    if (setjmp(ReturnBuf) == 0)
	      {
		PTCall(func->pn_left);
		PTCall(func->pn_right);
		r.r_type = INT;
		r.r.r_int = 0;
	      }
	    else
		r = ReturnResult;
	    InLoop = saveinloop;
	    BCOPY(save, ReturnBuf, sizeof(jmp_buf));

	    PopLocalVars();

	    /* end trace */

	    if (bGFTrace)
	    {
		iTraceIndent -= 2;

		printf
		    ("%-*.*s",
		     iTraceIndent,
		     iTraceIndent,
		     "                                        ");
		printf("End %s()\n",argv[0] ? argv[0] : "Undefined");
	    }
	  }

	/* avoid CastToInt error messages on NULL strings */
	if (r.r_type == STR && r.r.r_str == NULL)
	    return 0;

	CastToInt(&r);
	return r.r.r_int;

}	/* ExecuteFunction */



/*
** ExecuteStrFunction
**
**	The simulator calls ExecuteStrFunction to execute a function
**	or a simulator command.  If the function doesn't exist then
**	call ExecuteCommand.  Returns the function or command result
**	as a string in static storage.  Returned string values longer
**	than the static storage will be truncated.
*/

char* ExecuteStrFunction(argc, argv)

int	argc;
char	*argv[];

{	/* ExecuteStrFunction --- Given argc/argv lookup function and execute */

	/* result of function or command is saved here */
	static char	result[500];

	jmp_buf		save;
	int		saveinloop;
	Result		*rp;
	Result		r;
	ParseNode	*func;
	extern Result ExecuteCommand();

	if (argc < 1)
	    yyerror("ExecuteStrFunction: argc < 1!");
	    /* No Return */

	result[0] = '\0';

	rp = SymtabLook(&GlobalSymbols, argv[0]);
	if (rp == NULL || rp->r_type != FUNCTION)
	    r = ExecuteCommand(argc, argv);
	else
	  {
	    /* trace */

	    if (bGFTrace)
	      {
		int i;

		printf
		  ("%-*.*s",
		   iTraceIndent,
		   iTraceIndent,
		   "                                        ");
		printf("Function %s(",argv[0] ? argv[0] : "Undefined");
		for (i = 1 ; i < argc ; i++)
		  printf("{%s}%s",argv[i],i + 1 == argc ? "" : ",");
		printf(")\n");

		iTraceIndent += 2;
	      }

	    func = (ParseNode *) rp->r.r_str;
	    PushLocalVars(argc, argv, (Symtab *) func->pn_val.r.r_str);

	    BCOPY(ReturnBuf, save, sizeof(jmp_buf));
	    saveinloop = InLoop;
	    InLoop++;
	    if (setjmp(ReturnBuf) == 0)
	      {
		PTCall(func->pn_left);
		PTCall(func->pn_right);
		r.r_type = INT;
		r.r.r_int = 0;
	      }
	    else
		r = ReturnResult;
	    InLoop = saveinloop;
	    BCOPY(save, ReturnBuf, sizeof(jmp_buf));

	    PopLocalVars();

	    /* end trace */

	    if (bGFTrace)
	      {
		iTraceIndent -= 2;

		printf
		  ("%-*.*s",
		   iTraceIndent,
		   iTraceIndent,
		   "                                        ");
		printf("End %s()\n",argv[0] ? argv[0] : "Undefined");
	      }
	  }

	/* avoid NULL strings */
	if (r.r_type == STR && r.r.r_str == NULL)
	    return result;

	CastToStr(&r);
	strncpy(result, r.r.r_str, sizeof(result));
	result[sizeof(result)-1] = '\0';
	free(r.r.r_str);

	return result;

}	/* ExecuteStrFunction */



static Result do_func(func, args)

ParseNode	*func;
ParseNode	*args;

{	/* do_func --- Execute function */

	int	i;
	int	argc;
	char	*argv[1000];
	char	argbuf[2];
	jmp_buf	save, savebrk;
	int	saveinloop;
	Result	r;

	if (func->pn_val.r.r_str == NULL)    /* extern function not defined */
	  {
	    r.r_type = INT;
	    r.r.r_int = 0;
	    return(r);
	  }

	argbuf[0] = '\0';
	argv[0] = argbuf;

	/* fill in function name if available */

	if (func->pcInfo)
	    argv[0] = func->pcInfo;

	argc = 1;
	do_cmd_args(args, &argc, argv);
	PushLocalVars(argc, argv, (Symtab *) func->pn_val.r.r_str);

	/* trace */

	if (bGFTrace)
	{
	    printf
		("%-*.*s",
		 iTraceIndent,
		 iTraceIndent,
		 "                                        ");
	    printf("Function %s(",func->pcInfo ? func->pcInfo : "Undefined");
	    for (i = 1 ; i < argc ; i++)
		printf("{%s}%s",argv[i],i + 1 == argc ? "" : ",");
	    printf(")\n");

	    iTraceIndent += 2;
	}

	BCOPY(ReturnBuf, save, sizeof(jmp_buf));
	BCOPY(BreakBuf, savebrk, sizeof(jmp_buf));
	saveinloop = InLoop;
	InLoop++;
	if (setjmp(ReturnBuf) == 0)
	  {
	    PTCall(func->pn_left);
	    PTCall(func->pn_right);
	    r.r_type = INT;
	    r.r.r_int = 0;
	  }
	else
	    r = ReturnResult;
	InLoop = saveinloop;
	BCOPY(save, ReturnBuf, sizeof(jmp_buf));
	BCOPY(savebrk, BreakBuf, sizeof(jmp_buf));

	for (i = 1; i < argc; i++)
	    free(argv[i]);

	PopLocalVars();

	/* end trace */

	if (bGFTrace)
	{
	    iTraceIndent -= 2;

	    printf
		("%-*.*s",
		 iTraceIndent,
		 iTraceIndent,
		 "                                        ");
	    printf("End %s()\n",func->pcInfo ? func->pcInfo : "Undefined");
	}

	return(r);

}	/* do_func */


Result do_funcwithnode(cmdleader, args, argcomplist)

char*		cmdleader;
ParseNode*	args;
ParseNode*	argcomplist;

{
	Result	r;
	Result*	rp;
	char*	fullcmd;
	char*	cmdtrailer;

	r.r_type = INT;
	r.r.r_int = 0;

	cmdtrailer = do_cmd_args(argcomplist, (int *)NULL, (char **)NULL);
	if (cmdtrailer == NULL)
	    fullcmd = cmdleader;
	else
	  {
	    fullcmd = (char*) malloc(strlen(cmdleader)+strlen(cmdtrailer)+1);
	    if (fullcmd == NULL)
	      {
		perror("malloc");
		return r;
	      }

	    sprintf(fullcmd, "%s%s", cmdleader, cmdtrailer);
	    free(cmdtrailer);
	  }

	rp = SymtabLook(&GlobalSymbols, fullcmd);
	if (rp == NULL || rp->r_type != FUNCTION)
	    r = do_cmd(fullcmd, args, FALSE);
	else
	    r = do_func((ParseNode *) rp->r.r_str, args);

	if (fullcmd != cmdleader)
	    free(fullcmd);

	return r;

}

void do_assignment(dst, src)

Result*	dst;
Result*	src;

{
	switch (dst->r_type)
	  {

	  case INT:
	    CastToInt(src);
	    break;

	  case FLOAT:
	    CastToFloat(src);
	    break;

	  case STR:
	    CastToStr(src);
	    free(dst->r.r_str);
	    break;

	  }

	dst->r = src->r;
}


Result do_foreach_arg(arg, body, rp)

ParseNode	*arg;
ParseNode	*body;
Result		*rp;

{	/* do_foreach_arg --- execute foreach body for one argument */

	Result	r;
	char	**cargv;
	Result	arg1, arg2;

	/*
	** Format argument list for command execution.
	*/

	r.r_type = 0;
	if (arg == NULL)
	    return r;
 
	switch (arg->pn_val.r_type)
	  {

	  case ARGLIST:
	    do_foreach_arg(arg->pn_left, body, rp);
	    r = do_foreach_arg(arg->pn_right, body, rp);
	    if (r.r_type == ARGLIST)
	      {
		cargv = (char **) r.r.r_str;
		if (cargv == NULL)
		    yyerror("do_foreach_arg: NULL Arglist");
		    /* no return */

		while (*cargv != NULL)
		  {
		    Result	argvr;

		    argvr.r_type = STR;
		    argvr.r.r_str = *cargv++;
		    do_assignment(rp, &argvr);
		    PTCall(body);
		  }

		free(r.r.r_str);
	      }
	    else if (r.r_type != 0)
	      {
		do_assignment(rp, &r);
		PTCall(body);
	      }

	    r.r_type = 0;
	    break;

	  case ARGUMENT:
	    arg1 = do_foreach_arg(arg->pn_left, body, rp);
	    arg2 = do_foreach_arg(arg->pn_right, body, rp);

	    if (arg1.r_type == 0) return(arg2);
	    if (arg2.r_type == 0) return(arg1);

	    CastToStr(&arg1);
	    CastToStr(&arg2);
	    r = strop(arg1.r.r_str, '@', arg2.r.r_str);
	    break;

	  case LITERAL:
	    r.r_type = STR;
	    r.r.r_str = strsave(arg->pn_val.r.r_str);
	    break;

	  default:	/* expr or DOLLARARG */
	    r = PTEval(arg);
	    break;

	  }

	return r;

}	/* do_foreach_arg */



static void do_foreach(arg, body, rp)

ParseNode	*arg;
ParseNode	*body;
Result		*rp;

{	/* do_foreach --- Execute the body for current argument */

	if (arg == NULL)
	    return;

	switch(arg->pn_val.r_type)
	  {

	  case ARGLIST:
	    do_foreach_arg(arg, body, rp);
	    break;

	  case ARGUMENT:
	    do_foreach_arg(arg, body, rp);
	    break;

	  default:
	    yyerror("do_foreach: bad parse node type");
	    /* No Return */

	  }

}	/* do_foreach */



Result PTEval(pn)

ParseNode	*pn;

{	/* PTEval --- Evaluate the current node of the parse tree */

	Result	arg1, arg2, r, *rp;
	jmp_buf	save;
	char    buf[1000];
	char*	cmd;

	r.r_type = 0;
	if (pn == NULL)
	    return(r);

	switch (pn->pn_val.r_type)
	  {

	  case '@':
	    arg1 = PTEval(pn->pn_left);
	    arg2 = PTEval(pn->pn_right);
	    CastToStr(&arg1);
	    CastToStr(&arg2);
	    return(strop(arg1.r.r_str, pn->pn_val.r_type, arg2.r.r_str));

	  case '!': case '~':
	    arg1 = PTEval(pn->pn_left);
	    CastToInt(&arg1);
	    return(intop(arg1.r.r_int, pn->pn_val.r_type, 0));

	  case '&': case '|': case '^':
	  case OR: case AND:
	    arg1 = PTEval(pn->pn_left);
	    arg2 = PTEval(pn->pn_right);
	    CastToInt(&arg1);
	    CastToInt(&arg2);
	    return(intop(arg1.r.r_int, pn->pn_val.r_type, arg2.r.r_int));

	  case UMINUS:
	    arg1 = PTEval(pn->pn_left);
	    if (arg1.r_type == STR)
		CastToFloat(&arg1);

	    switch (arg1.r_type)
	      {

	      case INT:
		return(intop(arg1.r.r_int, pn->pn_val.r_type, 0));

	      case FLOAT:
		return(floatop(arg1.r.r_float, pn->pn_val.r_type, 0.0));

	      }
	    break;

	  case '*': case '/': case '%': case '+': case '-':
	    arg1 = PTEval(pn->pn_left);
	    arg2 = PTEval(pn->pn_right);

	    if (arg1.r_type == STR)
		CastToFloat(&arg1);
	    if (arg2.r_type == STR)
		CastToFloat(&arg2);

	    promote(&arg1, &arg2);
	    switch (arg1.r_type)
	      {

	      case INT:
		return(intop(arg1.r.r_int, pn->pn_val.r_type, arg2.r.r_int));

	      case FLOAT:
		return(floatop(arg1.r.r_float, pn->pn_val.r_type, arg2.r.r_float));

	      }
	    break;

	  case POW:
	    arg1 = PTEval(pn->pn_left);
	    arg2 = PTEval(pn->pn_right);

	    CastToFloat(&arg1);
	    CastToFloat(&arg2);
	    r.r_type = FLOAT;
	    r.r.r_float = pow(arg1.r.r_float, arg2.r.r_float);
	    break;

	  case LT: case LE: case GT: case GE: case EQ: case NE:
	    arg1 = PTEval(pn->pn_left);
	    arg2 = PTEval(pn->pn_right);
	    promote(&arg1, &arg2);
	    switch (arg1.r_type)
	      {

	      case INT:
		return(intop(arg1.r.r_int, pn->pn_val.r_type, arg2.r.r_int));

	      case FLOAT:
		return(floatop(arg1.r.r_float, pn->pn_val.r_type, arg2.r.r_float));

	      case STR:
		return(strop(arg1.r.r_str, pn->pn_val.r_type, arg2.r.r_str));

	      }
	    break;

	  case ICAST:
	    rp = (Result *) pn->pn_val.r.r_str;
	    if (rp->r_type == LOCREF)
	        rp = locref(rp->r.r_loc.l_offs);
	    CastToInt(rp);
	    break;

	  case FCAST:
	    rp = (Result *) pn->pn_val.r.r_str;
	    if (rp->r_type == LOCREF)
	        rp = locref(rp->r.r_loc.l_offs);
	    CastToFloat(rp);
	    break;

	  case SCAST:
	    rp = (Result *) pn->pn_val.r.r_str;
	    if (rp->r_type == LOCREF)
	        rp = locref(rp->r.r_loc.l_offs);
	    CastToStr(rp);
	    break;

	  case INTCONST:
	    r = pn->pn_val;
	    r.r_type = INT;
	    break;

	  case FLOATCONST:
	    r = pn->pn_val;
	    r.r_type = FLOAT;
	    break;

	  case STRCONST:
	    r.r.r_str = (char *) strsave(pn->pn_val.r.r_str);
	    r.r_type = STR;
	    break;

	  case INT:	/* Reference to a global integer variable */
	  case FLOAT:	/* Reference to a global float variable */
	    r = * (Result *)(pn->pn_val.r.r_str);
	    break;

	  case STR:	/* Reference to a global string variable */
	    r = * (Result *)(pn->pn_val.r.r_str);
	    if (r.r_type == STR)
		r.r.r_str = (char *) strsave(r.r.r_str);
	    break;

	  case LOCREF:	/* Reference to a local variable */
	    rp = locref(pn->pn_val.r.r_loc.l_offs);
	    r = *rp;
	    if (r.r_type == STR)
		r.r.r_str = (char *) strsave(r.r.r_str);
	    break;

	  case DOLLARARG:
	    r = dollarref(pn->pn_val.r.r_int);
	    r.r.r_str = (char *) strsave(r.r.r_str);
	    break;

	  case '=':
	      {
		r = PTEval(pn->pn_left);
		rp = (Result *) pn->pn_val.r.r_str;
		if (rp->r_type == LOCREF)
		    rp = locref(rp->r.r_loc.l_offs);

		do_assignment(rp, &r);
		r.r_type = 0;
	      }
	    break;

	  case FUNCTION:
	    if (pn->pn_right == NULL)
		r = do_func((ParseNode *) pn->pn_val.r.r_str, pn->pn_left);
	    else
	      {
		r = do_funcwithnode(pn->pn_val.r.r_str, pn->pn_left, pn->pn_right);
	      }
	    break;

	  case EXPRCALL:
	    r = PTEval((ParseNode*) pn->pn_val.r.r_str);
	    CastToStr(&r);
	    cmd = r.r.r_str;
	    r = do_funcwithnode(cmd, pn->pn_left, pn->pn_right);
	    free(cmd);
	    break;

	  case BREAK:
	    longjmp(BreakBuf, 1);
	    /* No Return */

	  case RETURN:
	    if (pn->pn_left == NULL)
	      {
		ReturnResult.r_type = INT;
		ReturnResult.r.r_int = 0;
	      }
	    else
		ReturnResult = PTEval(pn->pn_left);
	    longjmp(ReturnBuf, 1);
	    /* No Return */

	  case COMMAND:
	    r = do_cmd(pn->pn_val.r.r_str, pn->pn_left, TRUE);
	    break;

	  case INCLUDE:
	    ScriptLoc.si_lineno = 0;
	    ScriptLoc.si_script = pn->pn_val.r.r_str;
	    do_include_args(pn->pn_val.r.r_str, pn->pn_left);
	    break;

	  case ENDSCRIPT:
	    PopLocalVars();
	    break;

	  case SL:
	    PTCall(pn->pn_left);
	    SetLine((ScriptInfo *) pn->pn_val.r.r_str);
	    PTCall(pn->pn_right);
	    break;

	  case IF:
	    SetLine((ScriptInfo *) pn->pn_val.r.r_str);
	    r = PTEval(pn->pn_left);
	    CastToInt(&r);
	    if (r.r.r_int)
	        PTCall(pn->pn_right->pn_left);
	    else
	        PTCall(pn->pn_right->pn_right);
	    r.r_type = 0;
	    break;

	  case FOREACH:
	    rp = (Result *) pn->pn_val.r.r_str;
	    if (rp->r_type == LOCREF)
		rp = locref(rp->r.r_loc.l_offs);

	    BCOPY(BreakBuf, save, sizeof(jmp_buf));
	    InLoop++;
	    if (setjmp(BreakBuf) == 0)
		do_foreach(pn->pn_left, pn->pn_right, rp);
	    InLoop--;
	    BCOPY(save, BreakBuf, sizeof(jmp_buf));
	    break;

	  case WHILE:
	  case FOR:
	    SetLine((ScriptInfo *) pn->pn_val.r.r_str);
	    r = PTEval(pn->pn_left);
	    CastToInt(&r);
	    BCOPY(BreakBuf, save, sizeof(jmp_buf));
	    InLoop++;
	    if (setjmp(BreakBuf) == 0)
		while (r.r.r_int)
		  {
		    PTCall(pn->pn_right);
		    SetLine((ScriptInfo *) pn->pn_val.r.r_str);
		    r = PTEval(pn->pn_left);
		    CastToInt(&r);
		  }
	    InLoop--;
	    BCOPY(save, BreakBuf, sizeof(jmp_buf));
	    r.r_type = 0;
	    break;

	  default:
	    sprintf(buf, "PTEval: unknown parse tree node %s",
						TokenStr(pn->pn_val.r_type));
	    yyerror(buf);
	    /* No Return */

	  }

	return(r);

}	/* PTEval */


void PTCall(pn)

ParseNode*	pn;

{	/* PTCall --- Evaluate parse tree and free any result returned */

	Result	r;

	r = PTEval(pn);
	switch (r.r_type)
	  {

	  case STR:
	    if (r.r.r_str != NULL)
		free(r.r.r_str);
	    break;

	  case ARGLIST:
	    if (r.r.r_str != NULL)
	      {
		char**	argv;
		int	arg;

		argv = (char**) r.r.r_str;
		arg = 0;
		while (argv[arg] != NULL)
		    free(argv[arg++]);
		free(argv);
	      }
	    break;

	  }

}	/* PTCall */



void PTInit()

{	/* PTInit --- Initialize parse tree evaluation variables */

	InLoop = 0;
    	while (CurLocals != NULL)
	    PopLocalVars();

}	/* PTInit */


/*
** Hack to enable access to this static variable from other dircectories
*/
char * get_float_format()
{
	return float_format;	/* returns pointer to static! */
}

void set_float_format(format)
char	*format;
{
	strcpy(float_format,format);
}

/*
** Hack to get returned function value as a string for functions outside
** the scope of the 'ReturnResult' globals.
*/
char *ConvertBuf()
{
	static char	ret[400];

    ret[0] = '\0';
    switch (ReturnResult.r_type) {
        case INT :
            sprintf(ret,"%d",ReturnResult.r.r_int);
            break;
        case FLOAT :
            sprintf(ret,"%f",ReturnResult.r.r_float);
            break;
        case STR :
            sprintf(ret,"%s",ReturnResult.r.r_str);
            break;
    }
	return(ret);
}


/*
** Mon Jun 29 18:08:13 PDT 1992 [DHB]
**
** Moved from tools/script_hack.c.  Hooks for accessing script vairables
** from within the C code.  Should probably define and documents a consistent
** set of these routines.
*/

double GetScriptDouble(name)

char*	name;

{	/* GetScriptDouble --- Return value of a script variable as a double */

	Result*	rp;

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp)
	  { 
	    switch (rp->r_type)
	      {

	      case FLOAT:
		return rp->r.r_float;

	      case INT:
		return rp->r.r_int;

	      case STR:
		return Atof(rp->r.r_str);
	      }
	  }

	fprintf(stderr, "GetScriptDouble: script variable '%s' not found\n",
									name);
	return 0.0;

}	/* GetScriptDouble */



void SetScriptDouble(name, value)

char*	name;
double	value;

{	/* SetScriptDouble --- Set a script variable to the given value */

	Result*	rp;
	char	buf[100];

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp != NULL)
	  {
	    switch (rp->r_type)
	      {
	      case FLOAT:
		rp->r.r_float = value;
		break;

	      case INT:
		rp->r.r_int = (int) value;
		break;

	      case STR:
		sprintf(buf, "%g", value);
		if (rp->r.r_str != NULL)
		    free(rp->r.r_str);
		rp->r.r_str = CopyString(buf);
		break;
	      }
	  }
	else
	    fprintf(stderr, "SetScriptDouble: script variable '%s' not found\n",
									name);
}	/* SetScriptDouble */



int GetScriptInt(name)

char*	name;

{	/* GetScriptInt --- Return value of a script variable as an int */

	Result*	rp;

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp)
	  { 
	    switch (rp->r_type)
	      {

	      case FLOAT:
		return rp->r.r_float;

	      case INT:
		return rp->r.r_int;

	      case STR:
		return atoi(rp->r.r_str);
	      }
	  }

	fprintf(stderr, "GetScriptInt: script variable '%s' not found\n",
									name);
	return 0;

}	/* GetScriptInt */



void SetScriptInt(name, value)

char*	name;
int	value;

{	/* SetScriptInt --- Set a script variable to the given value */

	Result*	rp;
	char	buf[100];

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp != NULL)
	  {
	    switch (rp->r_type)
	      {
	      case FLOAT:
		rp->r.r_float = (double) value;
		break;

	      case INT:
		rp->r.r_int = value;
		break;

	      case STR:
		sprintf(buf, "%d", value);
		if (rp->r.r_str != NULL)
		    free(rp->r.r_str);
		rp->r.r_str = CopyString(buf);
		break;
	      }
	  }
	else
	    fprintf(stderr, "SetScriptInt: script variable '%s' not found\n",
									name);
}	/* SetScriptInt */



char* GetScriptStr(name)

char*	name;

{	/* GetScriptStr --- Return value of a script variable as a string */

	return get_glob_val(name);

}	/* GetScriptStr */



void SetScriptStr(name, value)

char*	name;
char*	value;

{	/* SetScriptStr --- Set a script variable to the given value */

	Result*	rp;

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp != NULL)
	  {
	    switch (rp->r_type)
	      {
	      case FLOAT:
		rp->r.r_float = Atof(value);
		break;

	      case INT:
		rp->r.r_int = atoi(value);
		break;

	      case STR:
		if (rp->r.r_str != NULL)
		    free(rp->r.r_str);
		rp->r.r_str = CopyString(value);
		break;
	      }
	  }
	else
	    fprintf(stderr, "SetScriptStr: script variable '%s' not found\n",
									name);
}	/* SetScriptStr */


void CreateScriptFloat(name)

char*	name;

{	/* CreateScriptFloat --- Create a script float with the given name */

	Result*	rp;

	rp = SymtabNew(&GlobalSymbols, name);
	if (rp == NULL)
	    return;

	if (rp->r_type != 0)
	    CastToFloat(rp);
	else
	  {
	    rp->r_type = FLOAT;
	    rp->r.r_float = 0.0;
	  }

}	/* CreateScriptFloat */


void CreateScriptInt(name)

char*	name;

{	/* CreateScriptInt --- Create a script int with the given name */

	Result*	rp;

	rp = SymtabNew(&GlobalSymbols, name);
	if (rp == NULL)
	    return;

	if (rp->r_type != 0)
	    CastToInt(rp);
	else
	  {
	    rp->r_type = INT;
	    rp->r.r_int = 0;
	  }

}	/* CreateScriptInt */


void CreateScriptString(name)

char*	name;

{	/* CreateScriptString --- Create a script string with the given name */

	Result*	rp;

	rp = SymtabNew(&GlobalSymbols, name);
	if (rp == NULL)
	    return;

	if (rp->r_type != 0)
	    CastToStr(rp);
	else
	  {
	    rp->r_type = STR;
	    rp->r.r_str = CopyString("");
	  }

}	/* CreateScriptFloat */



float get_script_float(name)
	char	*name;
{
	Result *rp;

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp) { 
		if (rp->r_type == FLOAT) 
			return((float)(rp->r.r_float));
	}
	fprintf(stderr,"Could not find global float '%s'\n",name);
	return(0.0);
}


void set_script_float(name,value) 
	char	*name;
	float	value;
{
	Result *rp;

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp != NULL && rp->r_type == FLOAT) {
		rp->r.r_float = value;
	} else {
		fprintf(stderr,"Could not find global '%s'\n",name);
	}
}


char *get_glob_val(name)
	char	*name;
{
	Result *rp;
	static char val[100];

	rp = SymtabLook(&GlobalSymbols,name);
	if (rp == NULL)
	    return NULL;

	val[0]='\0';
		switch(rp->r_type) {
			case FLOAT :
				sprintf(val,"%f",rp->r.r_float);
				break;
			case INT :
				sprintf(val,"%d",rp->r.r_int);
				break;
			case STR :
				if (strlen(rp->r.r_str)<99)
					sprintf(val,"%s",rp->r.r_str);
				break;
			default :
				break;
		}
	val[99]='\0';
	return(val);
}


/*
** GENESIS command interface to global script variables
*/

int do_addglobal(argc, argv)

int	argc;
char*	argv[];

{
	char*	type;
	char*	name;
	char*	value;

	initopt(argc, argv, "type name [value]");
	if (G_getopt(argc, argv) != 0 || optargc < 3 || 4 < optargc)
	  {
	    printoptusage(argc, argv);
	    printf("	type must be one of int, float or str.\n");
	    return 0;
	  }

	type = optargv[1];
	name = optargv[2];
	if (optargc == 4)
	    value = optargv[3];
	else
	    value = NULL;

	if (strcmp(type, "str") == 0)
	    CreateScriptString(name);
	else if (strcmp(type, "float") == 0)
	    CreateScriptFloat(name);
	else if (strcmp(type, "int") == 0)
	    CreateScriptInt(name);
	else
	  {
	    printoptusage(argc, argv);
	    printf("	type must be one of int, float or str.\n");
	    return 0;
	  }

	if (value != NULL)
	    SetScriptStr(name, value);

	return 1;
}


int do_setglobal(argc, argv)

int	argc;
char*	argv[];

{
	char*	name;
	char*	value;

	initopt(argc, argv, "name value");

	name = optargv[1];
	value = optargv[2];
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return 0;
	  }

	if (GetScriptStr(name) == NULL)
	  {
	    fprintf(stderr, "%s: script variable '%s' not found\n",
		argv[0], name);

	    return 0;
	  }

	SetScriptStr(name, value);
	return 1;
}


char* do_getglobal(argc, argv)

int	argc;
char*	argv[];

{
	char*	name;
	char*	value;

	initopt(argc, argv, "name");

	name = optargv[1];
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return NULL;
	  }

	value = GetScriptStr(name);
	if (value == NULL)
	  {
	    fprintf(stderr, "%s: Script variable '%s' not found\n",
		argv[0], name);
	    value = "";
	  }

	return CopyString(value);
}


void CompileScriptVars(fs, leader)

FILE*	fs;
char*	leader;

{	/* CompileScriptVars --- Generate C code to define and set script
				 variables currently defined as global
				 variables */

	SymtabEnt	*se;

	se = GlobalSymbols.sym_entlist;
	while (se != NULL)
	  {
	    switch (se->sym_val.r_type)
	      {

	      case INT:
		    fprintf(fs, "%sCreateScriptInt(\"%s\");\n", leader,
								se->sym_ident);
		    fprintf(fs, "%sSetScriptInt(\"%s\", %d);\n", leader,
					se->sym_ident, se->sym_val.r.r_int);
		break;

	      case FLOAT:
		    fprintf(fs, "%sCreateScriptFloat(\"%s\");\n", leader,
								se->sym_ident);
		    fprintf(fs, "%sSetScriptDouble(\"%s\", %f);\n", leader,
					se->sym_ident, se->sym_val.r.r_float);
		break;

	      case STR:
		    fprintf(fs, "%sCreateScriptString(\"%s\");\n", leader,
								se->sym_ident);
		    fprintf(fs, "%sSetScriptStr(\"%s\", \"%s\");\n", leader,
					se->sym_ident, se->sym_val.r.r_str);
		break;

	      }

	    se = se->sym_next;
	  }

}	/* CompileScriptVars */
