%p 5000
%a 5000
%o 7500

%{
static char lexrcsid[] = "$Id: script.l,v 1.2 2006/02/15 18:47:00 svitak Exp $";

/*
** $Log: script.l,v $
** Revision 1.2  2006/02/15 18:47:00  svitak
** Changes to allow parsing of scripts which use linefeeds containing \r, or
** carriage return.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.15  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.13  1999/04/24 01:42:18  mhucka
** The code for handling comments failed to work properly; it did not deal with
** oddball comments within C-style comments in .g files.  Replaced with new code
** taken from an ANSI C lex definition of the C language itself.
**
** Revision 2.1.1.1  1999/03/17 07:53:52  mhucka
** Import of GENESIS 2.1 release from CD-ROM
**
** Revision 1.12  1997/05/29 09:06:05  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS20l 1995/01/25  eds
 * Added MachTen compatibility
 *
 * Revision 1.11  1995/05/01  18:24:06  dhb
 * ifdef irix changed to ifdef sgi
 *
 * Revision 1.10  1995/04/29  01:23:00  dhb
 * Replaced lexer implementation specific code.  File is now flex and
 * bison compatible.
 *
 * Revision 1.9  1995/04/15  02:23:11  dhb
 * Empty update to force reprocessing by lex.
 *
 * Revision 1.8  1995/02/22  19:14:05  dhb
 * Linux support.
 *
 * Revision 1.7  1994/12/02  00:05:45  dhb
 * Added elif to if then else construct.
 *
 * Added ** operator to do pow().
 *
 * Fixed error in C style comments uncovered by new ** operator.
 *
 * Revision 1.6  1994/10/26  00:02:13  dhb
 * nextchar() was not reinitializing internal state properly (the cp
 * variable) when called to flush input.
 *
 * Revision 1.5  1994/09/29  20:49:36  dhb
 * C comment syntax in RCS log causing compile problems
 *
 * Revision 1.4  1994/09/23  16:38:38  dhb
 * Change to C style comment rule to fix lexing of comments on a command
 * line as a LITERAL.
 *
 * Revision 1.3  1994/04/01  19:02:00  dhb
 * nextchar() was missing extern declaration of CurrentScriptFp().
 *
 * Revision 1.2  1994/03/23  02:41:25  dhb
 * Changes to allow nextchar() to return 0 on end of file.  This allows
 * the parser to reduce the top level rule and exit the parser.  The code_g
 * program uses this capability.  GENESIS will never reach eof so no change
 * there.
 *
 * Revision 1.1  1992/12/11  21:18:51  dhb
 * Initial revision
 *
*/

#include <string.h>
#include "shell_func_ext.h"

#ifdef sgi
extern void exit();
#endif

#ifdef FLEX_SCANNER

static size_t GEN_moreinput();

#define YY_INPUT(buf,result,n)	result = GEN_moreinput(buf, n)

#else

#undef input
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):nextchar(0))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)

    /* YY_START - return current BEGIN state as value usable with BEGIN */
#define	YY_START	(yybgin-yysvec - 1)
#define	YY_NULL		0

#endif

extern int yyerror();

/* Forward declarations */
static void Ccomment();
static void Pushyybgin();
static void Popyybgin();

#ifdef LEXTEST
#    define	lextoken(tok)	printf("%s ==> %s\n", yytext, "tok")
#    define	lexchar(ch)	printf("'%c'\n", ch)
#else
#  ifdef YACCTEST
#    define	lextoken(tok)	{ printf("%s ==> %s\n", yytext, "tok"); return(tok); }
#    define	lexchar(ch)	{ printf("'%c'\n", ch); return(ch); }
#  else
#    define	lextoken(tok)	return(tok)
#    define	lexchar(ch)	return(ch)
#  endif
#endif

#define	BGINSTKSIZE	100

static int	yybginstk[BGINSTKSIZE];
static int	yybginidx = 0;
static int	continuation = 0;
%}

S	("+"|"-")?
E	((e|E){S}[0-9]+)

/*
** C comment handling taken from ANSI C Lex specification published by
** Jeff Lee and Jutta Degener <jutta@pobox.com>.
*/

%Start	LIT FUNCLIT CCOMMENT CPPCOMMENT

%{
/* I use this area to hold things that I might want back */

#ifdef COMMENT
<CPPCOMMENT>[^\n]*	Popyybgin();
"//"			Pushyybgin(CPPCOMMENT);

\\\n			continuation = 0;

<CCOMMENT>[^*\n]*	;
<CCOMMENT>"*"		;
<CCOMMENT>"\n"		;
<CCOMMENT>"*/"		Popyybgin();
"/*"/[^\n]*		Pushyybgin(CCOMMENT);
#endif
%}

%%

"/*"                    Ccomment();

"//".*			;

<LIT,FUNCLIT>\n		|
<LIT,FUNCLIT>\r		|
<LIT,FUNCLIT>"//"\r	|
<LIT,FUNCLIT>"//"\n	{
			  if (!continuation)
			      lexchar('\n');
			  else
			    {
			      continuation = 0;
			      lexchar(WHITESPACE);
			    }
			}

\r			|
\n			|
"//"\r		|
"//"\n			{
			  if (!continuation)
			      lexchar('\n');

			  continuation = 0;
			}

\\.*			continuation = 1;

\"[^\n"]*	|
\"[^\r"]*	{
			  char	ch;

			  ch = (char)input();
			  if (ch == '\n')
			    {
			      yyerror
			      ("Missing closing quote in string constant");
			      /* No Return */
			    }

			  if (ch == '\r')
			    {
			      yyerror
			      ("Missing closing quote in string constant");
			      /* No Return */
			    }

			  if (yytext[yyleng-1] == '\\')
			    {
			      yytext[yyleng-1] = '"';
			      yyless(yyleng-1);
			      yymore();
			    }
			  else
			    {
			      yylval.str = (char *) strsave(yytext+1);
			      lextoken(STRCONST);
			    }
			};

"$"[0-9]+		{
			  sscanf(yytext+1, " %d ", &yylval.iconst);
			  lextoken(DOLLARARG);
			}

<FUNCLIT>[^ \t\r\n{}(),"$\\]+ { 
			  yylval.str = (char *) strsave(yytext);
			  lextoken(LITERAL);
			};

<LIT>[^ \t\r\n{}"$;\\]+      { 
			  yylval.str = (char *) strsave(yytext);
			  lextoken(LITERAL);
			};

<FUNCLIT,LIT>[ \t]+	lextoken(WHITESPACE);
[ \t]+			;

include		lextoken(INCLUDE);

"*ENDSCRIPT*"	lextoken(ENDSCRIPT);

break		lextoken(BREAK);

return		lextoken(RETURN);

while		lextoken(WHILE);

if		lextoken(IF);

else		lextoken(ELSE);

elif		lextoken(ELIF);

for		lextoken(FOR);

foreach		lextoken(FOREACH);

end		lextoken(END);

function	lextoken(FUNCTION);

extern		lextoken(EXTERN);

int		lextoken(INT);

float		lextoken(FLOAT);

str		lextoken(STR);

"<"		lextoken(LT);
"<="		lextoken(LE);
">"		lextoken(GT);
">="		lextoken(GE);
"=="		lextoken(EQ);
"!="		lextoken(NE);

"||"		lextoken(OR);
"&&"		lextoken(AND);

"**"		lextoken(POW);

[_A-Za-z][_A-Za-z0-9]*	{
			  Result	*rp;

			  if (!ReturnIdents &&
			      ((rp=SymtabLook(LocalSymbols, yytext)) != NULL ||
			       (rp=SymtabLook(&GlobalSymbols, yytext)) != NULL))
			    {
			      yylval.str = (char *) rp;
			      if (rp->r_type == FUNCTION)
				  { lextoken(FUNCREF); }
			      else
				  lextoken(VARREF);
			    }
			  else
			    {
			      yylval.str = (char *) strsave(yytext);
			      lextoken(IDENT);
			    }
			};

[0-9]+			{
			  sscanf(yytext, " %d ", &yylval.iconst);
			  lextoken(INTCONST);
			}

[0-9]+{E}		|
[0-9]+\.[0-9]*{E}?	|
[0-9]*\.[0-9]+{E}?	{
			  sscanf(yytext, " %lf ", &yylval.fconst);
			  lextoken(FLOATCONST);
			}

"{"			{ Pushyybgin(0); lexchar('{'); };
"}"			{ Popyybgin(); lexchar('}'); };

.			lexchar(yytext[0]);
%%

/*
** Routine to handle C comments.
*/

static void Ccomment()
{
        char c, c1;

loop:
        while ((c = (char)input()) != '*' && c != 0)
                ;

        if ((c1 = (char)input()) != '/' && c != 0)
	{
		unput(c1);
                goto loop;
	}
}

/*
** Lexer routines to push and pop the lexer begin state
*/

static void Pushyybgin(start)

int	start;

{	/* Pushyybgin --- Push to another lexer begin state */

	if (yybginidx == BGINSTKSIZE)
	    yyerror("Pushyybgin: lexer begin state stack overflow");
	    /* No Return */

	yybginstk[yybginidx++] = YY_START;
	BEGIN start;

}	/* Pushyybgin */



static void Popyybgin()

{	/* Popyybgin --- Pop to previous lexer begin state */

	if (yybginidx == 0)
	    yyerror("Popyybgin: lexer begin state stack underflow");
	    /* No Return */

	BEGIN yybginstk[--yybginidx];

}	/* Popyybgin */



/*
** Local routine to get input characters
**
** For FLEX_SCANNER we don't need to do this buffering since
** flex does it for us.  We still provide nextchar() as a method
** for flushing the lexer input.  The *END SCRIPT* stuff will be
** handled in the YY_INPUT macro.
*/

static size_t GEN_moreinput(buf, n)

char*	buf;
int	n;

{
	extern FILE* CurrentScriptFp();

	char*	cp;
	size_t	result;

	do
	  {
	    cp = (char *) sgets(buf, n);
	  }
       while (cp != NULL && *cp == '\0');

	if (cp == NULL)
	  {
	    if (feof(CurrentScriptFp()))
		result = YY_NULL; /* END OF INPUT */
	    else
	      {
		sprintf(buf, "*ENDSCRIPT*\n");
		result = strlen(buf);
	      }
	  }
	else
	    result = strlen(buf);

	return result;

}

#ifdef FLEX_SCANNER

int nextchar(flush)

int	flush;

{
	if (flush)
	  {
	    yybginidx = 0;
	    BEGIN 0;
	    yyrestart(yyin); /* flushes flex input buffer */
	  }

	return 0;
}

#else
int nextchar(flush)

int	flush;

{	/* nextchar --- Return next char to lexer */

	static char	buf[100] = "";
	static char	*cp = buf;

	if (flush)
	  {
	    yybginidx = 0;
	    BEGIN 0;
	    yysptr = yysbuf;
	    cp = buf;
	    *cp = '\0';
	    return 0;
	  }

	if (cp == NULL)
	    return 0;

	if (*cp != '\0')
	    return *cp++;

	if (GEN_moreinput(buf, sizeof(buf)) == 0)
	  {
	    cp = NULL;
	    return 0;
	  }

	cp = buf;
	return(*cp++);

}	/* nextchar */
#endif

/*
** For systems that don't have a lex library, we define some of the support
** routines.
*/

/* mds3 changes */
#if defined(i860)
yyless(x)
{
extern char yytext[];
register char *lastch, *ptr;
extern int yyleng;
extern int yyprevious;
lastch = yytext+yyleng;
if (x>=0 && x <= yyleng)
        ptr = x + yytext;
else
        ptr = x;
while (lastch > ptr)
        yyunput(*--lastch);
*lastch = 0;
if (ptr >yytext)
        yyprevious = *--lastch;
yyleng = ptr-yytext;
}
#endif
 
#if defined(i860) || defined(machten)
yywrap()
{
return 1;
}
#endif
/* end of mds3 changes */
