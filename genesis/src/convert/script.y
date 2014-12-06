%{
static char parsercsid[] = "$Id: script.y,v 1.3 2005/08/08 13:10:23 svitak Exp $";

/*
** $Log: script.y,v $
** Revision 1.3  2005/08/08 13:10:23  svitak
** Removed #ifdef around #include y.tab.h.
**
** Revision 1.2  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2003/03/28 21:07:05  gen-dbeeman
** Fix from Mike Vanier Jan 2003 to fix problem with bison ver. 1.75
**
** Revision 1.8  2001/04/18 22:39:36  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.7  1997/06/12 22:50:55  dhb
** Added include of string.h for strlen() declaration needed by
** included lexer code.
**
** Revision 1.6  1996/07/15 23:19:00  venkat
** Added preprocessor macros to deal with the order of the include lex.yy.c
** statement and the definitions of the macros used in it, for the DEC/alpha
**
 * Revision 1.5  1995/06/16  05:57:12  dhb
 * FreeBSD compatibility.
 *
 * Revision 1.4  1995/04/29  01:18:16  dhb
 * Moved include of lex.yy.c back to original location.  Previous
 * problem is fixed by extern void exit() in script.l.
 *
 * Fixed bug in call to SymtabLook() call where a Symtab rather than
 * a Symtab* was passed.
 *
 * Revision 1.3  1995/04/15  02:26:28  dhb
 * Moved include of lex.yy.c after the parse rules to work around SGI
 * yacc misplacement of include statements.
 *
 * Revision 1.2  1995/04/14  01:03:20  dhb
 * Null change to force patch and reprocessing by yacc.
 *
 * Revision 1.1  1995/01/13  01:09:48  dhb
 * Initial revision
 *
 * Revision 1.1  1992/12/11  21:19:07  dhb
 **
 ** This file is from GENESIS 1.4.1
 **
 * Initial revision
 *
*/

#define YYDEBUG	1

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include "parse.h"
#include "symtab.h"

/*
** General declarations.
*/
extern char *strsave();

/*
** Parser routines which return something other than int.
*/

extern ParseNode *vardef();
extern char *TokenStr();

extern char* Combine();
extern char* ArgListToStr();
extern char** AllocateArgList();

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

%nonassoc LT LE GT GE EQ NE
%left '!'
%left OR AND
%left '+' '-' '&' '|' '^'
%left '*' '/' '%' '~'
%left UMINUS

%token WHILE IF ELSE FOR FOREACH END INCLUDE ENDSCRIPT BREAK
%token INT FLOAT STR RETURN WHITESPACE FUNCTION
%token <str> INTCONST DOLLARARG
%token <str> FLOATCONST
%token <str> STRCONST LITERAL IDENT VARREF FUNCREF EXTERN

  /*
  ** The following tokens are defined only to be used as unique parse tree
  ** node types.  Real tokens are used when applicable.
  */

%token SL COMMAND ARGUMENT ARGLIST LOCREF ICAST FCAST SCAST

%type <str> script statement statement_list while_stmnt for_stmnt
%type <str> foreach_stmnt if_stmnt else_clause assign_stmnt cmd_stmnt
%type <str> arg_list func_call_list arg_component_list arg_component
%type <str> func_def decl_stmnt return_stmnt null_stmnt 
%type <str> func_args func_arg_list func_hdr include_stmnt func_call
%type <str> break_stmnt decl_ident decl_list opt_arg_list endscript_marker
%type <str> ext_func stmnt_terminator
%type <str> funcref cmd_name include_hdr opt_node init decl_type
%type <expr> expr
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
		  statement stmnt_terminator
		  {
		    Output($3, NULL);
		  }
		;

stmnt_terminator : '\n'
		  {
		    $$ = "\n";
		  }
		 | ';'
		  {
		    $$ = "; ";
		  }
		 ;

statement	: while_stmnt
		| for_stmnt
		| foreach_stmnt
		| if_stmnt
		| assign_stmnt
		  {
		    OutputIndentation();
		    Output($1, NULL);
		  }
		| include_stmnt
		| endscript_marker
		| cmd_stmnt
		  {
		    OutputIndentation();
		    OutputArgList($1);
		  }
		| func_call
		  {
		    OutputIndentation();
		    OutputArgList($1);
		  }
		| func_def
		| ext_func
		| decl_stmnt
		| break_stmnt
		| return_stmnt
		| null_stmnt
		;

endscript_marker : ENDSCRIPT
		  {
		    yyaccept();
		  }
		;

while_stmnt	: WHILE '(' expr ')'
		  {
		    OutputIndentation();
		    Output("while (", $3.s, ")", NULL);
		  }
		  statement_list END
		  {
		    OutputIndentation();
		    Output("end", NULL);
		  }
		;

for_stmnt	: FOR '(' assign_stmnt ';' expr ';' assign_stmnt ')'
		  {
		    OutputIndentation();
		    Output("for (", $3, "; ", $5.s, "; ", $7, ")", NULL);
		  }
		      statement_list
 		  END
		  {
		    OutputIndentation();
		    Output("end", NULL);
		  }
		;

foreach_stmnt	: FOREACH VARREF '('
		    {
			BEGIN FUNCLIT;
			OutputIndentation();
			Output("foreach ", $2, " (", NULL);
		    }
		  ws arg_list ws
		    {
			BEGIN 0;
			OutputArgList($6);
			Output(")", NULL);
		    }
 		  ')' statement_list END
		  {
		    OutputIndentation();
		    Output("end", NULL);
		  }
		;

if_stmnt	: IF '(' expr ')'
		  {
		    OutputIndentation();
		    Output("if (", $3.s, ")", NULL);
		  }
		    statement_list else_clause END
		  {
		    OutputIndentation();
		    Output("end", NULL);
		  }
		;

else_clause	: /* Nothing */
		  {
 		    $$ = NULL;
 		  }
		| ELSE
		  { OutputIndentation(); Output("else", NULL); }
		  statement_list
		  { $$ = NULL; }
		;

assign_stmnt	: VARREF '=' expr
		  {
		    $$ = Combine($1, " = ", $3.s, NULL);
		  }
		;

include_hdr	: INCLUDE
		  {
		    Pushyybgin(LIT);
		  }
		  ws LITERAL
		  {
		    OutputIndentation();
		    Output("include ", $4, " ", NULL);
		  }
		;

include_stmnt	: include_hdr WHITESPACE arg_list ws
		  {
		    Popyybgin();
		    OutputArgList($3);
		  }
		| include_hdr ws
		  {
		    Popyybgin();
		  }
		;

opt_node	: /* Nothing */
		  {
		    $$ = (char*) strsave("");
		  }
		| LITERAL
		  {
		    $$ = $1;
		  }
		;

cmd_name	: IDENT
		  {
		    Pushyybgin(FUNCLIT);
		  }
		  opt_node
		  {
		    char	cmd[200];

		    sprintf(cmd, "%s%s", $1, $3);
		    free($1);
		    free($3);
		    $$ = (char*) strsave(cmd);
		  }
		;

cmd_stmnt	: cmd_name
		  {
		    BEGIN LIT;
		  }
		  opt_arg_list ws
		  {
		    char*	cmdname;
		    char**	argv;
		    int		i;

		    Popyybgin();
		    cmdname = $1;
		    argv = (char**) $3;
		    switch (MapCommand(&cmdname, argv, NULL))
		      {
		      case -1:
			fprintf(stderr, "Error mapping command: %s", cmdname);
			for (i = 0; argv[i] != NULL; i++)
			    fprintf(stderr, " %s", argv[i]);
			fprintf(stderr, "\n");
			InsertItemInArgList(argv, 0, "/* Could not map:", 100);
			InsertItemInArgList(argv, 1, cmdname, 100);
			AddItemToArgList(argv, "*/", 100);
			break;
		      case 0:
			argv[0] = NULL; /* don't output anything */
			break;
		      default:
			InsertItemInArgList(argv, 0, cmdname, 100);
			break;
		      }
		    $$ = (char*) argv;
		  }
		| cmd_name '(' optwslist func_call_list optwslist ')'
		  {
		    char*	cmdname;
		    char**	argv;
		    int		i;

		    Popyybgin();
		    cmdname = $1;
		    argv = (char**) $4;
		    switch (MapCommand(&cmdname, argv, NULL))
		      {
		      case -1:
			fprintf(stderr, "Error mapping command: %s", cmdname);
			for (i = 0; argv[i] != NULL; i++)
			    fprintf(stderr, " %s", argv[i]);
			fprintf(stderr, "\n");
			InsertItemInArgList(argv, 0, "/* Could not map:", 100);
			InsertItemInArgList(argv, 1, cmdname, 100);
			AddItemToArgList(argv, "*/", 100);
			break;
		      case 0:
			argv[0] = NULL; /* don't output anything */
			break;
		      default:
			InsertItemInArgList(argv, 0, cmdname, 100);
			break;
		      }
		    $$ = (char*) argv;
		  }
		;

funcref		: FUNCREF
		  {
		    Pushyybgin(FUNCLIT);
		    $$ = $1;
		  }
		;

func_call	: funcref '(' optwslist func_call_list optwslist ')'
		  {
		    char*	cmdname;
		    char**	argv;

		    Popyybgin();
		    cmdname = $1;
		    argv = (char**) $4;
		    InsertItemInArgList(argv, 0, cmdname, 100);
		    $$ = (char*) argv;
		  }
		| funcref
		  {
		    BEGIN LIT;
		  }
		  opt_arg_list ws
		  {
		    char*	cmdname;
		    char**	argv;

		    Popyybgin();
		    cmdname = $1;
		    argv = (char**) $3;
		    InsertItemInArgList(argv, 0, cmdname, 100);
		    $$ = (char*) argv;
		  }
		;

opt_arg_list	: /* Nothing */
		  {
		    char**	argv;

		    argv = AllocateArgList(100);
		    argv[0] = NULL;
		    $$ = (char*) argv;
		  }
		| opt_arg_list wslist arg_component_list
		  {
		    char**	argv;

		    argv = (char**) $1;
		    AddItemToArgList(argv, $3, 100);

		    $$ = $1;
		  }
		;

arg_list	: arg_component_list
		  {
		    char**	argv;

		    argv = AllocateArgList(100);
		    argv[0] = $1;
		    argv[1] = NULL;
		    $$ = (char*) argv;
		  }
		| arg_list wslist arg_component_list
		  {
		    char**	argv;

		    argv = (char**) $1;
		    AddItemToArgList(argv, $3, 100);

		    $$ = $1;
		  }
		;

optwslist	: /* NOTHING */
		| wslist
		;

wslist		: WHITESPACE
		| wslist WHITESPACE
		;

func_call_list	: arg_component_list
		  {
		    char**	argv;

		    argv = AllocateArgList(100);
		    argv[0] = $1;
		    argv[1] = NULL;
		    $$ = (char*) argv;
		  }
		| func_call_list optwslist ',' optwslist arg_component_list
		  {
		    char**	argv;

		    argv = (char**) $1;
		    AddItemToArgList(argv, $5, 100);

		    $$ = $1;
		  }
		;

ws		: /* NOTHING */
		| WHITESPACE
		;

arg_component_list	: arg_component
			  {
			    $$ = $1;
			  }
			| arg_component_list arg_component
			  {
			    $$ = Combine($1, $2, NULL);
			  }
			;

arg_component	: LITERAL
		  {
		    $$ = $1;
		  }
		| STRCONST
		  {
		    $$ = Combine("\"", $1, "\"", NULL);
		  }
		| DOLLARARG
		  {
		    $$ = $1;
		  }
		| '{'
		  {
		    Pushyybgin(0);
		  }
		  expr
		  {
		    Popyybgin();
		  }
		  '}'
		  {
		    /*
		    ** Often the expression is just a call to a command
		    ** or function, in which case we get the command
		    ** already wrapped in a {} pair.  Avoid the additional
		    ** curley braces.
		    */

		    if (AlreadyBracketed($3.s))
			$$ = $3.s;
		    else
			$$ = Combine("{", $3.s, "}", NULL);
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

		    OutputIndentation();
		    Output("extern ", $2, NULL);
		  }
		| EXTERN FUNCREF
		  {
		    OutputIndentation();
		    Output("extern ", $2, NULL);
		  }
		;

func_hdr	: FUNCTION IDENT
		  {
		    ParseNode	*funcpn;
		    ResultValue	v;
		    Result	*rp;
		    char	*script;

		    if (InFunctionDefinition)
		      {
			fprintf(stderr, "Function definition within another function or\n");
			fprintf(stderr, "within a control structure (FUNCTION %s).\n", $2);
			fprintf(stderr, "conversion failed\n");
			exit(1);
		      }

		    InFunctionDefinition++;
		    NextLocal = 0;
		    rp = SymtabNew(&GlobalSymbols, $2);
		    if (rp->r_type != 0 && rp->r_type != FUNCTION)
			fprintf(stderr, "WARNING: function name '%s' is redefining a variable!\n", $2);

		    rp->r_type = FUNCTION;

		    LocalSymbols = SymtabCreate();
		    $$ = $2;
		  }
		| FUNCTION FUNCREF
		  {
		    ParseNode	*funcpn;
		    ResultValue	v;
		    Result	*rp;
		    char	*script;

		    rp = (Result *) $2;
		    if (InFunctionDefinition)
		      {
			fprintf(stderr, "Function definition within another function or\n");
			fprintf(stderr, "within a control structure (FUNCTION %s).\n", $2);
			fprintf(stderr, "conversion failed\n");
			exit(1);
		      }

		    InFunctionDefinition++;
		    NextLocal = 0;
		    LocalSymbols = SymtabCreate();

		    $$ = $2;
		  }
		;

func_def	: func_hdr 
		  {
		    OutputIndentation();
		    Output("function ", $1, NULL);
		    ReturnIdents = 1;
		  }
		  func_args
		  {
		    ReturnIdents = 0;
		  }
		  statement_list END
		  {
		    OutputIndentation();
		    Output("end", NULL);

		    InFunctionDefinition--;
		    SymtabDestroy(LocalSymbols);
		    LocalSymbols = NULL;
		  }
		;

func_args	: /* NOTHING */
		  { $$ = NULL; }
		| '(' 
		  {
		    Output("(", NULL);
		  }
		  func_arg_list
		  ')'
		  { 
		    Output(")", NULL);
		  }
		;

func_arg_list	: IDENT
		  {
		    Output($1, NULL);
		    vardef($1, STR, SCAST, NULL);
		  }
		| func_arg_list ',' IDENT
		  {
		    Output(", ", $3, NULL);
		    vardef($3, STR, SCAST, NULL);
		  }
		;

decl_type	: INT
		  {
		    ReturnIdents = 1;
		    DefType = INT;
		    DefCast = ICAST;
		    $$ = "int";
		  }
		| FLOAT
		  {
		    ReturnIdents = 1;
		    DefType = FLOAT;
		    DefCast = FCAST;
		    $$ = "float";
		  }
		| STR
		  {
		    ReturnIdents = 1;
		    DefType = STR;
		    DefCast = SCAST;
		    $$ = "str";
		  }
		;

decl_stmnt	: decl_type decl_list
		  {
		    OutputIndentation();
		    Output($1, " ", $2, NULL);
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
		    $$ = Combine($1, ", ", $4, NULL);
		  }
		;

decl_ident	: IDENT
		  {
		    ReturnIdents = 0;
		  }
		  init
		  {
		    vardef($1, DefType, DefCast, NULL);
		    if ($3 != NULL)
			$$ = Combine($1, " = ", $3, NULL);
		    else
			$$ = $1;
		  }
		;

init		: /* NOTHING */
		  {
		    $$ = NULL;
		  }
		| '=' expr
		  {
		    $$ = $2.s;
		  }
		;

break_stmnt	: BREAK
		  {
		    OutputIndentation();
		    Output("break", NULL);
		  }
		;

return_stmnt	: RETURN expr
		  {
		    OutputIndentation();
		    Output("return ", $2.s, NULL);
		  }
		| RETURN
		  {
		    OutputIndentation();
		    Output("return", NULL);
		  }
		;

null_stmnt	: /* Nothing */
		  { $$ = NULL; }
		;

expr		: expr '|' expr
		  { $$.t = INT; $$.s = Combine($1.s, "|", $3.s, NULL); }
		| expr '&' expr
		  { $$.t = INT; $$.s = Combine($1.s, "&", $3.s, NULL); }
		| expr '^' expr
		  { $$.t = INT; $$.s = Combine($1.s, "^", $3.s, NULL); }
		| '~' expr
		  { $$.t = INT; $$.s = Combine("~", $2.s, NULL); }

		| expr '+' expr
		  {
		    if ($1.t == STR && $3.t == STR)
		      {
			$$.s = Combine($1.s, " @ ", $3.s, NULL);
			$$.t = STR;
		      }
		    else
		      {
			$$.s = Combine($1.s, " + ", $3.s, NULL);
			$$.t = INT;
		      }
		  }
		| expr '-' expr
		  { $$.t = INT; $$.s = Combine($1.s, " - ", $3.s, NULL); }
		| expr '*' expr
		  { $$.t = INT; $$.s = Combine($1.s, "*", $3.s, NULL); }
		| expr '/' expr
		  { $$.t = INT; $$.s = Combine($1.s, "/", $3.s, NULL); }
		| expr '%' expr
		  { $$.t = INT; $$.s = Combine($1.s, "%", $3.s, NULL); }
		| '-' expr %prec UMINUS
		  { $$.t = INT; $$.s = Combine("-", $2.s, NULL); }

		| expr OR expr
		  { $$.t = INT; $$.s = Combine($1.s, " || ", $3.s, NULL); }
		| expr AND expr
		  { $$.t = INT; $$.s = Combine($1.s, " && ", $3.s, NULL); }
		| '!' expr
		  { $$.t = INT; $$.s = Combine("!", $2.s, NULL); }

		| expr LT expr
		  { $$.t = INT; $$.s = Combine($1.s, " < ", $3.s, NULL); }
		| expr LE expr
		  { $$.t = INT; $$.s = Combine($1.s, " <= ", $3.s, NULL); }
		| expr GT expr
		  { $$.t = INT; $$.s = Combine($1.s, " > ", $3.s, NULL); }
		| expr GE expr
		  { $$.t = INT; $$.s = Combine($1.s, " >= ", $3.s, NULL); }
		| expr EQ expr
		  { $$.t = INT; $$.s = Combine($1.s, " == ", $3.s, NULL); }
		| expr NE expr
		  { $$.t = INT; $$.s = Combine($1.s, " != ", $3.s, NULL); }

		| IDENT '('
		    {
			BEGIN FUNCLIT;
		    }
 		  optwslist func_call_list optwslist
		    {
			BEGIN 0;
		    }
 		  ')'
		  {
		    char*	cmdname;
		    char**	argv;
		    char*	argstr;
		    int		retstr;
		    int		i;

		    cmdname = $1;
		    argv = (char**) $5;
		    switch (MapCommand(&cmdname, argv, &retstr))
		      {
		      case -1:
			argstr = ArgListToStr(argv);
			fprintf(stderr, "Error mapping command: %s %s",
			    cmdname, argstr);
			for (i = 0; argv[i] != NULL; i++)
			    fprintf(stderr, " %s", argv[i]);
			fprintf(stderr, "\n");
			$$.t = INT;
			$$.s = Combine("/* Could not map: ", cmdname, " ",
							argstr, " */", NULL);
			break;
		      case 0:
			fprintf(stderr, "Possible replacement of a command in an expression\n");
			fprintf(stderr, "with multiple commands.  Look for \"probable error\"\n");
			fprintf(stderr, "in converted script file.\n");
			$$.t = INT;
			$$.s = "/* probable error */"; /* don't output anything */
			break;
		      default:
			$$.t = retstr ? STR : INT;
			$$.s = Combine("{", cmdname, " ", ArgListToStr(argv),
								    "}", NULL);
			break;
		      }
		  }

		| FUNCREF '('
		    {
			BEGIN FUNCLIT;
		    }
 		  optwslist func_call_list optwslist
		    {
			BEGIN 0;
		    }
 		  ')'
		  {
		    $$.t = INT;
		    $$.s = Combine("{", $1, " ", ArgListToStr($5), "}", NULL);
		  }

		| VARREF
		  { 
		    Result*	rp;

		    rp = SymtabLook(LocalSymbols, $1);
		    if (rp == NULL)
			rp = SymtabLook(&GlobalSymbols, $1);

		    $$.t = rp->r_type;
		    $$.s = $1;
 		  }

		| FUNCREF
		  { 
		    $$.t = INT;
		    $$.s = Combine("{", $1, "}", NULL);
 		  }

		| IDENT
		  { 
		    char*	cmdname;
		    char*	argv[1];
		    int		retstr;
		    int		i;

		    cmdname = $1;
		    argv[0] = NULL;
		    switch (MapCommand(&cmdname, argv, &retstr))
		      {
		      case -1:
			fprintf(stderr, "Error mapping command: %s", cmdname);
			fprintf(stderr, "\n");
			$$.t = INT;
			$$.s = Combine("/* Could not map: ", cmdname,
							    " */", NULL);
			break;
		      case 0:
			fprintf(stderr, "Possible replacement of a command in an expression\n");
			fprintf(stderr, "with multiple commands.  Look for \"probable error\"\n");
			fprintf(stderr, "in converted script file.\n");
			$$.t = INT;
			$$.s = "/* probable error */"; /* don't output anything */
			break;
		      default:
			$$.t = retstr ? STR : INT;
			$$.s = Combine("{", cmdname, "}", NULL);
			break;
		      }
 		  }

		| FLOATCONST
		  { 
		    $$.t = INT;
		    $$.s = $1;
 		  }
		| INTCONST
		  { 
		    $$.t = INT;
		    $$.s = $1;
 		  }
		| STRCONST
		  { 
		    $$.t = STR;
		    $$.s = Combine("\"", $1, "\"", NULL);
 		  }

		| DOLLARARG
		  {
		    $$.t = STR;
		    $$.s = $1;
		  }

		| '{' expr '}'
		  {
		    $$.t = $2.t;
		    $$.s = Combine("(", $2.s, ")", NULL);
		  }


		| '(' expr ')'
		  {
		    $$.t = $2.t;
		    $$.s = Combine("(", $2.s, ")", NULL);
		  }
		;

%%

#if defined(decalpha) && !defined(bison)
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
char	*init;

{	/* vardef --- Define a variable */

	ParseNode	*pn;
	Result		*rp, r;
	ResultValue	v, slv;

	if (InFunctionDefinition && LocalSymbols != NULL)
	  {
	    rp = SymtabNew(LocalSymbols, ident);
	        rp->r_type = type;
		rp->r.r_loc.l_type = type;
		rp->r.r_loc.l_offs = NextLocal++;
	  }
	else
	  {
	    rp = SymtabNew(&GlobalSymbols, ident);

	    rp->r_type = type;
	  }

}	/* vardef */


ParseInit()

{    /* ParseInit --- Initialize parser variables */

        InFunctionDefinition = 0;
	Compiling = 0;
	BreakAllowed = 0;
	LocalSymbols = NULL;
	nextchar(1);	/* Flush lexer input */

}    /* ParseInit */


int NestedLevel()

{    /* NestedLevel --- Return TRUE if in func_def or control structure */

        return(InFunctionDefinition || Compiling);

}    /* NestedLevel */

int AlreadyBracketed(str)

char*	str;

{	/* AlreadyBracketed --- Return TRUE if the string has valid curley
				braces */

	if (*str == '{')
	  {
	    int	braceCount;

	    braceCount = 1;
	    for (str++; braceCount > 0 && *str != '\0'; str++)
	      {
		if (*str == '{')
		    braceCount++;
		else if (*str == '}')
		    braceCount--;
	      }

	    return braceCount == 0 && *str == '\0';
	  }
	else
	    return 0;

}	/* AlreadyBracketed */
