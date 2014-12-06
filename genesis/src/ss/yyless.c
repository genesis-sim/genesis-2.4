static char rcsid[] = "$Id: yyless.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $";

/*
** $Log: yyless.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1996/06/28 23:05:32  dhb
** Changed ifdef for Intel gamma and delta to be more specific
** (e.g. use gamma and delta rather than i860 define).
**
** Revision 1.2  1996/05/23  23:16:59  dhb
** t3d/e port
**
** Revision 1.1  1992/12/11  19:05:07  dhb
** Initial revision
**
*/

/* If the lex library doesn't define these ... */
#if defined(gamma) || defined(delta) || defined(CRAY)
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
        ptr = (char *) x;
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
