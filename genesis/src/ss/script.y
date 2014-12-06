%{
static char parsercsid[] = "$Id: script.y,v 1.5 2006/02/15 18:47:00 svitak Exp $";

/*
** $Log: script.y,v $
** Revision 1.5  2006/02/15 18:47:00  svitak
** Changes to allow parsing of scripts which use linefeeds containing \r, or
** carriage return.
**
** Revision 1.4  2005/08/08 13:10:21  svitak
** Removed #ifdef around #include y.tab.h.
**
** Revision 1.3  2005/06/27 19:01:13  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.2  2005/06/25 22:04:34  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.24  2003/03/28 21:03:58  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
** Fix from Mike Vanier Jan 2003 to fix problem with bison ver. 1.75
**
** Revision 1.23  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.21  1997/07/24 00:32:16  dhb
** Additional T3E mods from PSC
**
** Revision 1.20  1997/06/12 22:49:35  dhb
** Added include of string.h for strlen() declaration needed by
** included lexer code.
**
** Revision 1.19  1996/06/26 18:17:55  venkat
** Added preprocessor macros to deal with the order of the include lex.yy.c
** statement and the definitions of the macros used in it on the DEC/alpha.
**
 * Revision 1.18  1995/06/16  06:01:10  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.17  1995/06/14  20:34:25  dhb
 * Fix to allow variable names to appear as first part of a command
 * with trailing arg_component_list.
 *
 * Revision 1.16  1995/05/26  17:37:40  dhb
 * Changed lexer start state used for parsing opt_node syntax from
 * FUNCLIT to LIT which allows commas and parenthesis in the LITERALs
 * there.  The node syntax allows comma separated node specs.
 *
 * Revision 1.15  1995/05/08  22:34:16  dhb
 * Removed non-terminal typing for cmdname_expr (e.g. %type statement)
 *
 * Revision 1.14  1995/05/08  22:31:51  dhb
 * Changed optional node syntax appended to command and function calls
 * to be an optional arg_component_list.  This will allow just about
 * anything following an initial command for function name.
 *
 * Revision 1.13  1995/04/29  01:21:59  dhb
 * Moved include of lex.yy.c back to original location.  SGI specific
 * problem is fixed by extern void exit() in script.l.
 *
 * Revision 1.12  1995/04/24  22:36:15  dhb
 * Call to SymtabKey() in func_call production code was passing
 * a Symtab rather than a Symtab*.
 *
 * Revision 1.11  1995/04/15  02:23:11  dhb
 * Moved include of lex.yy.c after parse rules.  SGI lex generates calls
 * to exit() but SGI yacc doesn't include unistd.h until it starts doing
 * the rules.  Since unistd.h defines void exit() and cc interprets the
 * calls in the lexer as int exit() cc complains we are redefining the
 * return type.
 *
 * Revision 1.10  1994/12/15  21:52:36  dhb
 * Added support for adding @node to SLI script function calls.  Will allow
 * calling script function on other nodes on parallel systems.
 *
 * Revision 1.9  1994/12/02  00:05:45  dhb
 * Added elif to if then else construct.
 *
 * Added ** operator to do pow().
 *
 * Revision 1.8  1994/10/18  17:32:10  dhb
 * Changed PTEval() call to PTCall() which frees the evaluated result.
 *
 * Revision 1.7  1994/10/14  17:22:05  dhb
 * Another memory leak of ScriptInfo patched.
 *
 * Revision 1.6  1994/10/12  17:14:21  dhb
 * Removed dead code.
 *
 * Patched various memory leaks.
 *
 * Removed foreach restriction that the loop variable be a string.
 *
 * Revision 1.5  1994/08/31  03:03:23  dhb
 * Allow additional whitespace in foreach argument list.  This allows
 * for line continuations within the arglist including just after ( and
 * before ).
 *
 * Revision 1.4  1994/06/03  21:09:56  dhb
 * Fixed precedence problem where a == b && c == d was a syntax error.
 *
 * Revision 1.3  1994/04/16  17:19:55  dhb
 * Fixed operator precedence problem.  Made ! have higher prec than
 * || and &&.
 *
 * Revision 1.2  1993/03/01  17:39:01  dhb
 * 1.4 to 2.0 script language changes.
 *
 * 	Removed support for `function call' format of command invokation from
 * 	both the command line and expressions.  Commands are invoked in
 * 	expressions and command arguments by delimiting them with curly braces.
 *
 * 	Added @ operator for string concatenation.
 *
 * Revision 1.1  1992/12/11  21:19:07  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <string.h>
#include "parse.h"
#include "symtab.h"
#include "ss_func_ext.h"

/*
** Parser routines which return something other than int.
*/

extern ParseNode *vardef();
extern char *TokenStr();
extern char *MakeScriptInfo();	/* Actually a generic pointer */

/*
** Parser global variables
*/

extern jmp_buf	BreakBuf;	/* Used to break out of a loop */
extern jmp_buf	ReturnBuf;	/* Used to return out of a script */

static int	DefType;	/* Remembers type in a var decl */
static int	DefCast;	/* Remembers cast in a var decl */
static int	BreakAllowed = 0; /* In a loop control structure */
static int	ReturnIdents = 0; /* 1 ==> lexer doesn't classify IDENTs */
static int	Compiling = 0;	/* Make a statement list rather than execute */
static int	InFunctionDefinition = 0;
static int	NextLocal;	/* Next local symbol offset */
static int	ArgMatch;	/* Matches argument number to name in func */
Symtab		GlobalSymbols;	/* Symbols defined outside a function */
static Symtab	*LocalSymbols = NULL;	/* Symbols local to a function */
static ResultValue RV;			/* Dummy ReturnValue for PTNew */

%}

%left OR AND
%nonassoc LT LE GT GE EQ NE
%left '!'
%left '+' '-' '&' '|' '^' '@'
%left '*' '/' '%' '~'
%left POW
%left UMINUS

%token WHILE IF ELSE ELIF FOR FOREACH END INCLUDE ENDSCRIPT BREAK
%token INT FLOAT STR RETURN WHITESPACE FUNCTION
%token <iconst> INTCONST DOLLARARG
%token <fconst> FLOATCONST
%token <str> STRCONST LITERAL IDENT VARREF FUNCREF EXTERN

  /*
  ** The following tokens are defined only to be used as unique parse tree
  ** node types.  Real tokens are used when applicable.
  */

%token SL COMMAND EXPRCALL ARGUMENT ARGLIST LOCREF ICAST FCAST SCAST

%type <pn> script statement statement_list while_stmnt for_stmnt
%type <pn> foreach_stmnt if_stmnt else_clause assign_stmnt cmd_stmnt
%type <pn> arg_list arg_component_list arg_component
%type <pn> func_def decl_stmnt init return_stmnt null_stmnt expr
%type <pn> func_args func_arg_list func_hdr include_stmnt func_call
%type <pn> break_stmnt decl_ident decl_list opt_arg_list endscript_marker
%type <pn> ext_func cmd_name opt_node ac_func_cmd_expr
/*
%type <pn> cmdname_expr
*/
%type <str> funcref include_hdr

%{
#if !defined(decalpha) || defined(bison) || defined(T3E)
/*
** Start of lexical analyzer.  LEX source is in "script.l".
*/

extern YYSTYPE	yylval;

#include "y.tab.h"
#include "lex.yy.c"

#endif
%}

%%

script		: statement_list
		;

statement_list	: /* NOTHING */
		  { 
		    $$ = NULL;
 		  }
		| statement_list
		  {
		    $<str>$ = (char *) MakeScriptInfo();
		    SetLine($<str>$);
		  }
		  statement stmnt_terminator
		  {
		    ResultValue	v;

		    if (InFunctionDefinition || Compiling)
			if ($3 != NULL)
			  {
			    v.r_str = $<str>2;
			    $$ = PTNew(SL, v, $1, $3);
			  }
			else
			  {
			    FreeScriptInfo($<str>2);
			    $$ = $1;
			  }
		    else
		      {
		        /* execute statement */
		        if (setjmp(BreakBuf) == 0) {
			    if (setjmp(ReturnBuf) == 0)
				PTCall($3);
			    else
				EndScript();
		        }
			FreeScriptInfo($<str>2);
			FreePTValues($3);
			PTFree($3);
		      }
		  }
		;

stmnt_terminator : '\n'
		 | '\r'
		 | ';'
		 ;

statement	: while_stmnt
		| for_stmnt
		| foreach_stmnt
		| if_stmnt
		| assign_stmnt
		| include_stmnt
		| endscript_marker
		| cmd_stmnt
		| func_call
		| func_def
		| ext_func
		| decl_stmnt
		| break_stmnt
		| return_stmnt
		| null_stmnt
		;

endscript_marker : ENDSCRIPT
		  {
		    /*
		    ** When the end of a script is encountered, the simulator
		    ** sgets routine returns NULL.  The nextchar routine in the
		    ** lexer returns a special character '\200' which is lexed
		    ** as ENDSCRIPT.  We need this when we include a script
		    ** in a function or control structure so that the script
		    ** local variable storage is allocated and deallocated.
		    */

		    if (Compiling || InFunctionDefinition)
		      {
			$$ = PTNew(ENDSCRIPT, RV, NULL, NULL);
		      }
		    else
			$$ = NULL;
		  }
		;

while_stmnt	: WHILE '(' expr ')'
		  {
		    Compiling++;
		    BreakAllowed++;
		    $<str>$ = (char *) MakeScriptInfo();
		  }
		  statement_list END
		  {
		    ResultValue	v;

		    v.r_str = $<str>5;
		    $$ = PTNew(WHILE, v, $3, $6);
		    Compiling--;
		    BreakAllowed--;
		  }
		;

for_stmnt	: FOR '(' assign_stmnt ';' expr ';' assign_stmnt ')'
		    {
		      Compiling++;
		      BreakAllowed++;
		      $<str>$ = (char *) MakeScriptInfo();
		    }
		      statement_list
 		  END
		  {
		    ResultValue	v;
		    ParseNode	*forbody, *whilepart;

		    v.r_str = (char *) MakeScriptInfo();
		    forbody = PTNew(SL, v, $10, $7);
		    v.r_str = $<str>9;
		    whilepart = PTNew(FOR, v, $5, forbody);
		    $$ = PTNew(SL, v, $3, whilepart);
		    Compiling--;
		    BreakAllowed--;
		  }
		;

foreach_stmnt	: FOREACH VARREF '('
		    {
			BEGIN FUNCLIT;
			Compiling++;
			BreakAllowed++;
		    }
		  optwslist arg_list optwslist
		    {
			BEGIN 0;
		    }
 		  ')' statement_list END
		  {
		    Result	*rp;
		    ResultValue	v;

		    rp = (Result *) $2;
		    v.r_str = (char *) rp;
		    $$ = PTNew(FOREACH, v, $6, $10);
		    Compiling--;
		    BreakAllowed--;
		  }
		;

if_stmnt	: IF '(' expr ')'
		  {
		    Compiling++;
		    $<str>$ = (char *) MakeScriptInfo();
		  }
		    statement_list else_clause END
		  {
		    ResultValue	v;
		    ParseNode	*stmntlists;

		    stmntlists = PTNew(0, v, $6, $7);
		    v.r_str = $<str>5;
		    $$ = PTNew(IF, v, $3, stmntlists);
		    Compiling--;
		  }
		;

else_clause	: /* Nothing */
		  {
 		    $$ = NULL;
 		  }
		| ELSE statement_list
		  { $$ = $2; }
		| ELIF '(' expr ')'
		  {
		    Compiling++;
		    $<str>$ = (char *) MakeScriptInfo();
		  }
		    statement_list else_clause
		  {
		    ResultValue	v;
		    ParseNode	*stmntlists;

		    stmntlists = PTNew(0, v, $6, $7);
		    v.r_str = $<str>5;
		    $$ = PTNew(IF, v, $3, stmntlists);
		    Compiling--;
		  }
		;

assign_stmnt	: VARREF '=' expr
		  {
		    ResultValue	v;
		    Result	*rp;

		    $$ = NULL;
		    rp = (Result *) $1;
			  {
			    v.r_str = (char *) rp;
		            $$ = PTNew('=', v, $3, NULL);
			  }
		  }
		;

include_hdr	: INCLUDE
		  {
		    Pushyybgin(LIT);
		  }
		  ws
		  {
		    $$ = NULL;
		  }
		;

include_stmnt	: include_hdr LITERAL WHITESPACE arg_list ws
		  {
		    ResultValue	v;
		    int		argc;
		    char	*argv[100];
		    char	argbuf[1000];

		    Popyybgin();
		    sprintf(argbuf, "%s", $2);
		    argc = 1;
		    argv[0] = argbuf;
		    do_cmd_args($4, &argc, argv);
		    argv[argc] = NULL;

		    if (!IncludeScript(argc, argv))
		      {
			sprintf(argbuf, "Script '%s' not found", $2);
			FreePTValues($4);
			PTFree($4);
			free($2);
			yyerror(argbuf);
		      }

		    if (Compiling || InFunctionDefinition)
		      {
			v.r_str = $2;
			$$ = PTNew(INCLUDE, v, $4, NULL);
		      }
		    else
		      {
			FreePTValues($4);
			PTFree($4);
			free($2);
			$$ = NULL;
		      }
		  }
		| include_hdr LITERAL ws
		  {
		    ResultValue	v;
		    int		argc;
		    char	*argv[100];
		    char	argbuf[1000];

		    Popyybgin();
		    sprintf(argbuf, "%s", $2);
		    argc = 1;
		    argv[0] = argbuf;
		    argv[argc] = NULL;

		    if (!IncludeScript(argc, argv))
		      {
			sprintf(argbuf, "Script '%s' not found", $2);
			free($2);
			yyerror(argbuf);
		      }

		    if (Compiling || InFunctionDefinition)
		      {
			v.r_str = $2;
			$$ = PTNew(INCLUDE, v, NULL, NULL);
		      }
		    else
		      {
			free($2);
			$$ = NULL;
		      }
		  }
		;

opt_node	: /* Nothing */
		  {
		    $$ = (ParseNode*) NULL;
		  }
		| arg_component_list
		  {
		    $$ = $1;
		  }
		;

/*
** The COMMAND, FUNCTION and EXPRCALL node created here are later modified
** by the cmd_stmnt code to add the argument list to the left child and
** will change the node to an INCLUDE if there is no opt_node part and
** the node is not an EXPRCALL and it can find the named include file.
*/

/*
** Can't generate a parser to handle an expression as a command name
** since we it can't determine if {expr} in a command argument is a
** command or just an expression.

cmdname_expr	: '{' expr '}'
		  {
		    Pushyybgin(LIT);
		    $$ = $2;
		  }
		;
cmdname_expr	: '{'
		  {
		    Pushyybgin(0);
		  }
		  ac_func_cmd_expr
		  {
		    Popyybgin();
		  }
		  '}'
		  {
		    Pushyybgin(LIT);
		    $$ = $3;
		  }
		;
*/

cmd_name	: IDENT
		  {
		    Pushyybgin(LIT);
		  }
		  opt_node
		  {
		    ResultValue	v;

		    v.r_str = $1;
		    if ($3 == NULL)
			$$ = PTNew(COMMAND, v, NULL, NULL);
		    else
			$$ = PTNew(FUNCTION, v, NULL, $3);
		  }
		| VARREF 
		  {
		    Pushyybgin(LIT);
		  }
		  arg_component_list
		  {
		    ResultValue	v;
		    char*	varname;

		    varname = NULL;
		    if (LocalSymbols != NULL)
			varname = SymtabKey(LocalSymbols, $1);
		    if (varname == NULL)
			varname = SymtabKey(&GlobalSymbols, $1);
		    v.r_str = (char*) strsave(varname);

		    $$ = PTNew(FUNCTION, v, NULL, $3);
		  }
/*
** See above comments
		| cmdname_expr
		  opt_node
		  {
		    ResultValue	v;

		    v.r_str = (char*) $1;
		    $$ = PTNew(EXPRCALL, v, NULL, $2);
		  }
*/
		;

cmd_stmnt	: cmd_name
		  {
		    BEGIN LIT;
		  }
		  opt_arg_list ws
		  {
		    int		argc;
		    char	*argv[100];
		    char	argbuf[1000];

		    $$ = $1;
		    $1->pn_left = $3;
		    Popyybgin();
		    if ($1->pn_val.r_type != EXPRCALL && $1->pn_right == NULL &&
				!IsCommand($1->pn_val.r.r_str))
		      {
			if (IsInclude($1->pn_val.r.r_str))
			  {
			    sprintf(argbuf, "%s", $1->pn_val.r.r_str);
			    argc = 1;
			    argv[0] = argbuf;
			    do_cmd_args($3, &argc, argv);
			    argv[argc] = NULL;
			    IncludeScript(argc, argv);

			    if (Compiling || InFunctionDefinition)
			      {
				$1->pn_val.r_type = INCLUDE;
			      }
			    else
			      {
				FreePTValues($1);
				PTFree($1);
				$$ = NULL;
			      }
			  }
		      }
		  }
		;

funcref		: FUNCREF
		  {
		    Pushyybgin(LIT);
		    $$ = $1;
		  }
		;

func_call	: funcref opt_node
		  {
		    BEGIN LIT;
		  }
		  opt_arg_list ws
		  {
		    ResultValue	v;
		    Result	*rp;

		    Popyybgin();
		    rp = (Result *) $1;
		    if ($2 == NULL)
			$$ = PTNew(FUNCTION, rp->r, $4, NULL);
		    else
		      {
			v.r_str = (char*) strsave(SymtabKey(&GlobalSymbols, rp));
			$$ = PTNew(FUNCTION, v, $4, $2);
		      }
		  }
		;

opt_arg_list	: /* Nothing */
		  {
		    $$ = NULL;
		  }
		| opt_arg_list wslist arg_component_list
		  {
		    $$ = PTNew(ARGLIST, RV, $1, $3);
		  }
		;

arg_list	: arg_component_list
		  {
		    $$ = PTNew(ARGLIST, RV, NULL, $1);
		  }
		| arg_list wslist arg_component_list
		  {
		    $$ = PTNew(ARGLIST, RV, $1, $3);
		  }
		;

optwslist	: /* NOTHING */
		| wslist
		;

wslist		: WHITESPACE
		| wslist WHITESPACE
		;

ws		: /* NOTHING */
		| WHITESPACE
		;

arg_component_list	: arg_component
			  {
			    ResultValue	v;

			    $$ = PTNew(ARGUMENT, v, NULL, $1);
			  }
			| arg_component_list arg_component
			  {
			    ResultValue	v;

			    $$ = PTNew(ARGUMENT, v, $1, $2);
			  }
			;

arg_component	: LITERAL
		  {
		    ResultValue	v;

		    v.r_str = $1;
		    $$ = PTNew(LITERAL, v, NULL, NULL);
		  }
		| STRCONST
		  {
		    ResultValue	v;

		    v.r_str = $1;
		    $$ = PTNew(LITERAL, v, NULL, NULL);
		  }
		| DOLLARARG
		  {
		    ResultValue	v;

		    v.r_int = $1;
		    $$ = PTNew(DOLLARARG, v, NULL, NULL);
		  }
		| '{'
		  {
		    Pushyybgin(0);
		  }
		  ac_func_cmd_expr
		  {
		    Popyybgin();
		  }
		  '}'
		  {
		    $$ = $3;
		  }
		;

ac_func_cmd_expr : func_call
		  {
		    $$ = $1;
		  }
		| cmd_stmnt
		  {
		    $$ = $1;
		  }
		| expr
		  {
		    if ($1->pn_val.r_type == STRCONST)
			$1->pn_val.r_type = LITERAL;

		    $$ = $1;
		  }
		;

ext_func	: EXTERN IDENT
		  {
		    ParseNode	*funcpn;
		    ResultValue	v;
		    Result	*rp;

		    rp = SymtabNew(&GlobalSymbols, $2);
		    if (rp->r_type != 0 && rp->r_type != FUNCTION)
			fprintf(stderr, "WARNING: function name '%s' is redefining a variable!\n", $2);

		    rp->r_type = FUNCTION;

		    v.r_str = (char *) NULL;
		    funcpn = PTNew(SL, v, NULL, NULL);
		    rp->r.r_str = (char *) funcpn;

		    free($2);
		    $$ = NULL;
		  }
		| EXTERN FUNCREF
		  {
		    $$ = NULL;
		  }
		;

func_hdr	: FUNCTION IDENT
		  {
		    ParseNode	*funcpn;
		    ResultValue	v;
		    Result	*rp;

		    if (InFunctionDefinition)
		      {
			fprintf(stderr, "Function definition within another function or\n");
			fprintf(stderr, "within a control structure (FUNCTION %s).\n", $2);
			yyerror("");
			/* No Return */
		      }

		    InFunctionDefinition++;
		    NextLocal = 0;
		    rp = SymtabNew(&GlobalSymbols, $2);
		    if (rp->r_type != 0 && rp->r_type != FUNCTION)
			fprintf(stderr, "WARNING: function name '%s' is redefining a variable!\n", $2);

		    rp->r_type = FUNCTION;

		    LocalSymbols = SymtabCreate();
		    v.r_str = (char *) LocalSymbols;
		    funcpn = PTNew(SL, v, NULL, NULL);

		    /*- copy function name as parse node info */

		    funcpn->pcInfo = strsave($2);

		    rp->r.r_str = (char *) funcpn;

		    free($2);
		    $$ = funcpn;
		  }
		| FUNCTION FUNCREF
		  {
		    ParseNode	*funcpn;
		    Result	*rp;

		    rp = (Result *) $2;
		    if (InFunctionDefinition)
		      {
			fprintf(stderr, "Function definition within another function or\n");
			fprintf(stderr, "within a control structure (FUNCTION %s).\n", $2);
			yyerror("");
			/* No Return */
		      }

		    /*
		    ** Free old function parse tree and symtab
		    */

		    funcpn = (ParseNode *) rp->r.r_str;
		    if (funcpn->pn_val.r.r_str != NULL)
			SymtabDestroy(funcpn->pn_val.r.r_str);
		    FreePTValues(funcpn->pn_left);
		    PTFree(funcpn->pn_left);
		    FreePTValues(funcpn->pn_right);
		    PTFree(funcpn->pn_right);

		    InFunctionDefinition++;
		    NextLocal = 0;
		    LocalSymbols = SymtabCreate();
		    funcpn->pn_val.r.r_str = (char *) LocalSymbols;

		    /*- copy the function name as parse node info */

		    if (!funcpn->pcInfo)
		    {
			funcpn->pcInfo = strsave(yytext);
		    }
		    else
		    {
			/*! this should not happen */

			int i = 0;
		    }

		    $$ = funcpn;
		  }
		;

func_def	: func_hdr 
		  {
		    ReturnIdents = 1;
		  }
		  func_args
		  {
		    ReturnIdents = 0;
		  }
		  statement_list END
		  {
		    InFunctionDefinition--;

		    $1->pn_left = $3;
		    $1->pn_right = $5;

		    LocalSymbols = NULL;
		    $$ = NULL;
		  }
		;

func_args	: /* NOTHING */
		  { $$ = NULL; }
		| '(' func_arg_list ')'
		  { $$ = $2; }
		;

func_arg_list	: IDENT
		  {
		    ResultValue	v;
		    ParseNode	*init;

		    ArgMatch = 1;
		    v.r_int = ArgMatch++;
		    init = PTNew(DOLLARARG, v, NULL, NULL);
		    $$ = vardef($1, STR, SCAST, init);
		    free($1);
		  }
		| func_arg_list ',' IDENT
		  {
		    ResultValue	v;
		    ParseNode	*init;

		    v.r_int = ArgMatch++;
		    init = PTNew(DOLLARARG, v, NULL, NULL);
		    v.r_str = (char *) MakeScriptInfo();
		    $$ = PTNew(SL, v, $1, vardef($3, STR, SCAST, init));
		    free($3);
		  }
		;

decl_type	: INT
		  {
		    ReturnIdents = 1;
		    DefType = INT;
		    DefCast = ICAST;
		  }
		| FLOAT
		  {
		    ReturnIdents = 1;
		    DefType = FLOAT;
		    DefCast = FCAST;
		  }
		| STR
		  {
		    ReturnIdents = 1;
		    DefType = STR;
		    DefCast = SCAST;
		  }
		;

decl_stmnt	: decl_type decl_list
		  {
		    $$ = $2;
		  }
		;

decl_list	: decl_ident
		| decl_list ','
		  {
		    ReturnIdents = 1;
		  }
		  decl_ident
		  {
		    ResultValue	v;

		    v.r_str = (char *) MakeScriptInfo();
		    $$ = PTNew(SL, v, $1, $4);
		  }
		;

decl_ident	: IDENT
		  {
		    ReturnIdents = 0;
		  }
		  init
		  {
		    $$ = vardef($1, DefType, DefCast, $3);
		    free($1);
		  }
		;

init		: /* NOTHING */
		  { $$ = NULL; }
		| '=' expr
		  { $$ = $2; }
		;

break_stmnt	: BREAK
		  {
		    ResultValue	v;

		    if (BreakAllowed)
			$$ = PTNew(BREAK, v, NULL, NULL);
		    else
			yyerror("BREAK found outside of a loop");
			/* No Return */
		  }
		;

return_stmnt	: RETURN expr
		  {
		    ResultValue	v;

		    $$ = PTNew(RETURN, v, $2, NULL);
		  }
		| RETURN
		  {
		    ResultValue	v;

		    $$ = PTNew(RETURN, v, NULL, NULL);
		  }
		;

null_stmnt	: /* Nothing */
		  { $$ = NULL; }
		;

expr		: expr '|' expr
		  { $$ = PTNew('|', RV, $1, $3); }
		| expr '&' expr
		  { $$ = PTNew('&', RV, $1, $3); }
		| expr '^' expr
		  { $$ = PTNew('^', RV, $1, $3); }
		| '~' expr
		  { $$ = PTNew('~', RV, $2, NULL); }

		| expr '@' expr
		  { $$ = PTNew('@', RV, $1, $3); }

		| expr '+' expr
		  { $$ = PTNew('+', RV, $1, $3); }
		| expr '-' expr
		  { $$ = PTNew('-', RV, $1, $3); }
		| expr '*' expr
		  { $$ = PTNew('*', RV, $1, $3); }
		| expr '/' expr
		  { $$ = PTNew('/', RV, $1, $3); }
		| expr '%' expr
		  { $$ = PTNew('%', RV, $1, $3); }
		| expr POW expr
		  { $$ = PTNew(POW, RV, $1, $3); }
		| '-' expr %prec UMINUS
		  { $$ = PTNew(UMINUS, RV, $2, NULL); }

		| expr OR expr
		  { $$ = PTNew(OR, RV, $1, $3); }
		| expr AND expr
		  { $$ = PTNew(AND, RV, $1, $3); }
		| '!' expr
		  { $$ = PTNew('!', RV, $2, NULL); }

		| expr LT expr
		  { $$ = PTNew(LT, RV, $1, $3); }
		| expr LE expr
		  { $$ = PTNew(LE, RV, $1, $3); }
		| expr GT expr
		  { $$ = PTNew(GT, RV, $1, $3); }
		| expr GE expr
		  { $$ = PTNew(GE, RV, $1, $3); }
		| expr EQ expr
		  { $$ = PTNew(EQ, RV, $1, $3); }
		| expr NE expr
		  { $$ = PTNew(NE, RV, $1, $3); }

		| VARREF
		  { 
		    Result	*rp;
		    ResultValue	v;

		    /*
		    ** Variable reference
		    */

		    rp = (Result *) $1;
		      {
			if (rp->r_type == FUNCTION || rp->r_type == LOCREF)
			    v = rp->r;
			else /* Global Variable */
			    v.r_str = (char *) rp;

		        $$ = PTNew(rp->r_type, v, NULL, NULL);
		      }
 		  }

		| FLOATCONST
		  { 
		    ResultValue	v;

		    v.r_float = $1;
		    $$ = PTNew(FLOATCONST, v, NULL, NULL);
 		  }
		| INTCONST
		  { 
		    ResultValue	v;

		    v.r_int = $1;
		    $$ = PTNew(INTCONST, v, NULL, NULL);
 		  }
		| STRCONST
		  { 
		    ResultValue	v;

		    v.r_str = $1;
		    $$ = PTNew(STRCONST, v, NULL, NULL);
 		  }

		| DOLLARARG
		  {
		    ResultValue	v;

		    v.r_int = $1;
		    $$ = PTNew(DOLLARARG, v, NULL, NULL);
		  }

		| '{' expr '}'
		  { $$ = $2; }


		| '{' cmd_stmnt '}'
		  { $$ = $2; }


		| '{' func_call '}'
		  { $$ = $2; }


		| '(' expr ')'
		  { $$ = $2; }
		;

%%

#if defined(decalpha) && !defined(bison) && !defined(T3E)
/*
** Start of lexical analyzer.  LEX source is in "script.l".
*/

extern YYSTYPE	yylval;

#if defined(__FreeBSD__)
#include "y.tab.h"
#endif

#include "lex.yy.c"
#endif

/*
** TokenStr
**
**	Return the token string for the given token.
*/

char *TokenStr(token)

int	token;

{	/* TokenStr --- Return token string for token */

	static char	buf[100];

	switch (token)
	  {

	  case LT: return("<");
	  case LE: return("<=");
	  case GT: return(">");
	  case GE: return(">=");
	  case EQ: return("==");
	  case NE: return("!=");

	  case OR: return("||");
	  case AND: return("&&");

#define	RET(tok)	case tok: return("tok")

	  RET(UMINUS);

	  RET(WHILE);
	  RET(IF);
	  RET(ELSE);
	  RET(FOR);
	  RET(FOREACH);
	  RET(END);
	  RET(INCLUDE);
	  RET(BREAK);
	  RET(INT);
	  RET(FLOAT);
	  RET(STR);
	  RET(RETURN);
	  RET(WHITESPACE);
	  RET(FUNCTION);
	  RET(INTCONST);
	  RET(DOLLARARG);
	  RET(FLOATCONST);
	  RET(STRCONST);
	  RET(LITERAL);
	  RET(IDENT);
	  RET(VARREF);
	  RET(FUNCREF);
	  RET(SL);
	  RET(COMMAND);
	  RET(ARGUMENT);
	  RET(ARGLIST);
	  RET(LOCREF);
	  RET(ICAST);
	  RET(FCAST);
	  RET(SCAST);

	  }

	if (token < 128)
	    if (token < ' ')
		sprintf(buf, "^%c", token+'@');
	    else
		sprintf(buf, "%c", token);
	else
	    sprintf(buf, "%d", token);

	return(buf);

}	/* TokenStr */


ParseNode *vardef(ident, type, castop, init)

char		*ident;
int		type;
int		castop;
ParseNode	*init;

{	/* vardef --- Define a variable */

	ParseNode	*pn;
	Result		*rp;
	ResultValue	v, slv;

	if (InFunctionDefinition && LocalSymbols != NULL)
	  {
	    rp = SymtabNew(LocalSymbols, ident);
	    if (rp->r_type == 0)
	      {
	        rp->r_type = LOCREF;
		rp->r.r_loc.l_type = type;
		rp->r.r_loc.l_offs = NextLocal++;
	      }

	    v.r_str = (char *) rp;
	    pn = PTNew(castop, v, NULL, NULL);
	    if (init)
	      {
		slv.r_str = (char *) MakeScriptInfo();
		pn = PTNew(SL, slv, pn, PTNew('=', v, init, NULL));
	      }
	  }
	else
	  {
	    rp = SymtabNew(&GlobalSymbols, ident);
	    switch(type)
	      {

	      case INT:
	        if (rp->r_type == 0)
	            rp->r.r_int = 0;
	        else
		    CastToInt(rp);
	        break;

	      case FLOAT:
	        if (rp->r_type == 0)
	            rp->r.r_float = 0.0;
	        else
		    CastToFloat(rp);
	        break;

	      case STR:
	        if (rp->r_type == 0)
	            rp->r.r_str = (char *) strsave("");
	        else
		    CastToStr(rp);
	        break;

	      }

	    rp->r_type = type;
	    v.r_str = (char *) rp;
	    if (init)
	        pn = PTNew('=', v, init, NULL);
	    else
	        pn = NULL;
	  }

	return(pn);

}	/* vardef */


void ParseInit()

{    /* ParseInit --- Initialize parser variables */

        InFunctionDefinition = 0;
	Compiling = 0;
	BreakAllowed = 0;
	LocalSymbols = NULL;
	nextchar(1);	/* Flush lexer input */
	PTInit();	/* Reinit parse tree evaluation */

}    /* ParseInit */


int NestedLevel()

{    /* NestedLevel --- Return TRUE if in func_def or control structure */

        return(InFunctionDefinition || Compiling);

}    /* NestedLevel */
