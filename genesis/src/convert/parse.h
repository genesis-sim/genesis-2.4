/*
** $Id: parse.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: parse.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2003/03/28 21:07:05  gen-dbeeman
** Fix from Mike Vanier Jan 2003 to fix problem with bison ver. 1.75
**
** Revision 1.1  1995/01/13 01:09:48  dhb
** Initial revision
**
**
** This file is from GENESIS 1.4.1
**
** Revision 1.1  1992/12/11  19:05:08  dhb
** Initial revision
**
*/

/*
** Parsing related type and constant definitions
*/

#include "result.h"

typedef struct _pn
  {
    Result	pn_val;
    struct _pn	*pn_left;
    struct _pn	*pn_right;
  } ParseNode;

typedef struct _expr_t
  {
    int		t;
    char*	s;
  } expr_t;

typedef union _yyvals
  {
    int		iconst;
    double	fconst;
    char	*str;
    ParseNode	*pn;
    expr_t	expr;
  } YYSTYPE;

/* added by Mike Vanier Jan 2003 to fix problem with bison ver. 1.75
 * According to Mike: There was a type error involving the YYSTYPE expression
 * and some ifdef's which had the combined effect of making the compiler think
 * that YYSTYPE was an int even though it is defined here.  I think that
 * giving an explicit type for YYSTYPE is the Wrong Thing to do; you should
 * always #define it since bison generates lines like "#ifdef YYSTYPE".  My
 * fix was simple: I just added this line after the above lines:
 *
 * #define YYSTYPE YYSTYPE
 *
 * Yes, this actually works.  Apparently, #define uses a separate namespace or
 * something than the C compiler itself, so the "typedef union" statement
 * above doesn't actually define YYSTYPE from the point of view of the
 * preprocessor.  This is ugly, and I'm sure there is a better way to do
 * things, but it works.
 */

#define YYSTYPE YYSTYPE

extern ParseNode *PTNew();
extern Result PTEval();
