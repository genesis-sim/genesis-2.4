%p 5000
%a 5000
%o 7500

%{
static char lexrcsid[] = "$Id: script.l,v 1.3 2005/07/01 10:03:01 svitak Exp $";

/*
** $Log: script.l,v $
** Revision 1.3  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:34  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/18 22:39:36  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.6  1999/10/17 04:53:03  mhucka
** Added forward declarations for Pushyybgin(), Popyybgin().
**
** Revision 1.5  1999/08/22 04:42:13  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.4  1995/05/01 18:25:08  dhb
** changes ifdef irix to ifdef sgi
**
 * Revision 1.3  1995/04/29  01:19:50  dhb
 * Replaced lexer implementation specific code and made lexer
 * work with flex and bison.
 *
 * Revision 1.2  1995/04/14  01:03:20  dhb
 * Null change to force patch and reprocessing by lex.
 *
 * Revision 1.1  1995/01/13  01:09:48  dhb
 * Initial revision
 *
**
** This file is from GENESIS 1.4.1
**
 * Revision 1.1  1992/12/11  21:18:51  dhb
 * Initial revision
 *
*/

#include "output.h"

#ifdef sgi
extern void exit();
#endif

#ifdef FLEX_SCANNER

static int GEN_moreinput();

#define YY_INPUT(buf,result,n)	result = GEN_moreinput(buf, n)

#else

#undef input
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):nextchar(0))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)

    /* YY_START - return current BEGIN state as value usable with BEGIN */
#define	YY_START	(yybgin-yysvec - 1)
#define	YY_NULL		0

#endif

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
#    define	lextoken(tok)	{inCommand = 1; return(tok);}
#    define	lexchar(ch)	{(ch == '\n' && !continuation) ? inCommand = 0: 0; return(ch); }
#  endif
#endif

#define	BGINSTKSIZE	100

static int	yybginstk[BGINSTKSIZE];
static int	yybginidx = 0;
static int	continuation = 0;
static int	inCommand = 0;
int		Indentation = 0;

/* Forward declarations */
static void Pushyybgin();
static void Popyybgin();
%}

S	("+"|"-")?
E	((e|E){S}[0-9]+)

%Start	LIT FUNCLIT CCOMMENT CPPCOMMENT

%{
/* I use this area to hold things that I might want back */

#ifdef COMMENT
<CPPCOMMENT>[^\n]*	Popyybgin();
"//"			Pushyybgin(CPPCOMMENT);

\\\n			continuation = 0;
#endif
%}

%%

<CCOMMENT>[^*\n]*	Output(yytext, NULL);
<CCOMMENT>"*"		Output("*", NULL);
<CCOMMENT>"\n"		Output("\n", NULL);
<CCOMMENT>"*/"		{
			  if (inCommand)
			    Output("*/", "\n", NULL);
			  else
			    Output("*/", NULL);
			  Popyybgin();
			};
"/*"			{
			  OutputIndentation();
			  Output("/*", NULL);
			  Pushyybgin(CCOMMENT);
			};

"//".*			{
			  OutputIndentation();
			  if (inCommand)
			    Output(yytext, "\n", NULL);
			  else
			    Output(yytext, NULL);
			};

<LIT,FUNCLIT>\n		|
<LIT,FUNCLIT>"//"\n	{
			  if (!continuation)
			    {
			      Indentation = 0;
			      lexchar('\n');
			    }
			  else
			    {
			      continuation = 0;
			      lexchar(WHITESPACE);
			    }
			}

\n			|
"//"\n			{
			  if (!continuation)
			    {
			      Indentation = 0;
			      lexchar('\n');
			    }

			  continuation = 0;
			}

\\.*			{
			  /* extern char*	strchr(); */
			  char*		comment;

			  comment = strchr(yytext, '/');
			  if (comment != NULL && comment[1] == '/')
			    {
			      OutputIndentation();
			      Output(comment, "\n", NULL);
			    }

			  continuation = 1;
			}

\"[^\n"]*		{
			  char	ch;

			  ch = input();
			  if (ch == '\n')
			    {
			      fprintf(stderr, 
				  "Missing closing quote in string constant\n");
			      fprintf(stderr, "conversion failed\n");
			      exit(1);
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
			  yylval.str = (char *) strsave(yytext);
			  lextoken(DOLLARARG);
			}

<FUNCLIT>[^ \t\n{}(),"$\\]+ { 
			  yylval.str = (char *) strsave(yytext);
			  lextoken(LITERAL);
			};

<LIT>[^ \t\n{}"$;\\]+      { 
			  yylval.str = (char *) strsave(yytext);
			  lextoken(LITERAL);
			};

<FUNCLIT,LIT>[ \t]+	lextoken(WHITESPACE);
[ \t]+			{
			  if (Indentation == 0)
			    {
			      char*	cp;

			      SetOutputIndentation(yytext);
			      Indentation = 1;
			      /*
			      cp = yytext;
			      for (cp = yytext; *cp != '\0'; cp++)
				if (*cp == '\t')
				    Indentation += 8 - Indentation%8;
				else
				    Indentation++;
			      */
			    }
			};

include		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(INCLUDE);
		};

"*ENDSCRIPT*"	lextoken(ENDSCRIPT);

break		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(BREAK);
		};

return		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(RETURN);
		};

while		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(WHILE);
		};

if		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(IF);
		};

else		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(ELSE);
		};

for		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(FOR);
		};

foreach		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(FOREACH);
		};

end		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(END);
		};

function	{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(FUNCTION);
		};

extern		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(EXTERN);
		};

int		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(INT);
		};

float		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(FLOAT);
		};

str		{
		  if (!Indentation)
		      SetOutputIndentation("");
		  Indentation = 1;
		  lextoken(STR);
		};

"<"		lextoken(LT);
"<="		lextoken(LE);
">"		lextoken(GT);
">="		lextoken(GE);
"=="		lextoken(EQ);
"!="		lextoken(NE);

"||"		lextoken(OR);
"&&"		lextoken(AND);

[_A-Za-z][_A-Za-z0-9]*	{
			  Result	*rp;

			  if (!Indentation)
			      SetOutputIndentation("");
			  Indentation = 1;

			  yylval.str = (char *) strsave(yytext);
			  if (!ReturnIdents &&
			      ((rp=SymtabLook(LocalSymbols, yytext)) != NULL ||
			       (rp=SymtabLook(&GlobalSymbols, yytext)) != NULL))
			    {
			      if (rp->r_type == FUNCTION)
				  { lextoken(FUNCREF); }
			      else
				  lextoken(VARREF);
			    }
			  else
			      lextoken(IDENT);
			};

[0-9]+			{
			  yylval.str = (char *) strsave(yytext);
			  lextoken(INTCONST);
			}

[0-9]+{E}		|
[0-9]+\.[0-9]*{E}?	|
[0-9]*\.[0-9]+{E}?	{
			  yylval.str = (char *) strsave(yytext);
			  lextoken(FLOATCONST);
			}

"{"			{ Pushyybgin(0); lexchar('{'); };
"}"			{ Popyybgin(); lexchar('}'); };

.			{
			  if (yytext[0] == '\n')
			    Indentation = 0;
			  lexchar(yytext[0])
			};
%%

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



static FILE*	CurFs = NULL;

void InputFs(fs)

FILE*	fs;

{
	CurFs = fs;
}


/*
** Local routine to get input characters
**
** For FLEX_SCANNER we don't need to do this buffering since
** flex does it for us.  We still provide nextchar() as a method
** for flushing the lexer input.  The *END SCRIPT* stuff will be
** handled in the YY_INPUT macro.
*/

static int GEN_moreinput(buf, n)

char*	buf;
int	n;

{
	char*	cp;
	int	result;

        if (CurFs == NULL)
            CurFs = stdin;

	do
	  {
	    cp = (char *) fgets(buf, n, CurFs);
	  }
       while (cp != NULL && *cp == '\0');

	if (cp == NULL)
	  {
	    result = YY_NULL; /* END OF INPUT */
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

{	/* nextchar --- Return next char to lexer */

	static char	buf[100] = "";
	static char	*cp = buf;

        if (CurFs == NULL)
            CurFs = stdin;

	if (flush)
	  {
	    yybginidx = 0;
	    BEGIN 0;
	    yysptr = yysbuf;
	    cp = buf;
	    *cp = '\0';
	    return;
	  }

	while (cp != NULL && *cp == '\0')
	    cp = (char *) fgets(buf, sizeof(buf), CurFs);

	if (cp == NULL)
	  {
	    /*
	    sprintf(buf, "*ENDSCRIPT*\n");
	    cp = buf;
	    */
	    return EOF;
	  }

	return(*cp++);

}	/* nextchar */
#endif

/*
** For systems that don't have a lex library, we define some of the support
** routines.
*/

/* mds3 changes */
#ifdef i860
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

yywrap()
{
return 1;
}
#endif
/* end of mds3 changes */
