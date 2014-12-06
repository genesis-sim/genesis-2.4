/*
** $Id: header.h,v 1.4 2005/06/27 19:01:15 svitak Exp $
** $Log: header.h,v $
** Revision 1.4  2005/06/27 19:01:15  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.3  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.2  2005/06/25 21:25:45  svitak
** printoptusage() is used everywhere. Created a new file for it's declaration
** and included it where appropriate.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.10  2000/06/19 06:10:33  mhucka
** Added #ifndef HEADER_H protection.
**
** Revision 1.9  2000/06/12 04:55:43  mhucka
** MAX and MIN shouldn't be defined if they already are.
**
** Revision 1.8  1998/01/08 23:49:59  dhb
** Changed to support additional random number generator.
**
** Revision 1.7  1997/08/08 19:25:19  dhb
** Added include of stdlib.h
**
** Revision 1.6  1997/07/18 03:14:53  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5  1995/04/14 18:03:28  dhb
** Added extern for G_popen().
**
** Added check for getopt macro already defined.
**
** Revision 1.4  1995/04/13  00:17:53  dhb
** Added macros under GETOPT_PROBLEM ifdef to redefine the GENESIS
** getopt() function and optopt variable to G_getopt() and G_optopt.
** Since virtually all the genesis code includes header.h this should
** cover it.
**
** Revision 1.3  1994/04/16  20:33:43  dhb
** Added include of sys/types.h which is needed since we
** use the caddr_t type now.
**
** Revision 1.2  1993/02/12  19:57:06  dhb
** Added extern statements for GENESIS getopt routines and variables
**
** Revision 1.1  1992/12/11  19:05:40  dhb
** Initial revision
**
*/

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "printoptusage.h"

#define INVALID		0
#define CHAR		1
#define SHORT		2
#define INT		3
#define FLOAT		4
#define DOUBLE		5
#define FUNC		6
#define FFUNC		7
#define STRING		8

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#define next_arg_is(s)	(strcmp(argv[++nxtarg],s) == 0)
#define arg_is(s)	(strcmp(argv[nxtarg],s) == 0)
#define iarg()	(atoi(argv[++nxtarg]))
#define farg()	(Atof(argv[++nxtarg]))
#define arg_starts_with(c)	(argv[nxtarg][0] == c)

/* mds3 changes */
/*
** long    random ();
** #define IHUGE 0x7fffffff
** #define MAXLONG		    2147483641L
*/
#ifdef i860
/*
** #define frandom(l,h)    (((float)rand()/MAXLONG)*((h)-(l))+(l))
** #define urandom()		((float)rand()/MAXLONG)
*/
extern float G_RNG();
extern void G_SEEDRNG();
#define frandom(l,h) (G_RNG()*((h)-(l))+(l))
#define urandom() G_RNG()
#else
/*
** #define frandom(l,h)    (((float)rand()/MAXLONG)*((h)-(l))+(l))
** #define urandom()		((float)rand()/MAXLONG)
*/
extern float G_RNG();
extern void G_SEEDRNG();
#define frandom(l,h) (G_RNG(0)*((h)-(l))+(l))
#define urandom() G_RNG(0)
#endif
#define falloc(n,t)   (t*)malloc((unsigned)((n)*sizeof(t)))
/* mds3 changes */
#if !defined( MAX ) && !defined( MIN )
#define MAX(x,y)	(((x) > (y)) ? (x) : (y))
#define MIN(x,y)	(((x) < (y)) ? (x) : (y))
#endif
#define round(x)	((int)(0.5+(x)))
#define sqr(x)		((x)*(x))

#ifndef PFI_DEFINED
#define PFI_DEFINED
typedef int	(*PFI)();
#endif
typedef float	(*PFF)();
typedef double	(*PFD)();
typedef char	*(*PFC)();

extern float  Atof();

/*
** externs for GENESIS getopt routines
*/

extern int	optargc;
extern char**	optargv;
extern char*	G_optopt;

extern int      initopt();
extern int      G_getopt();

/*
** Genesis version of popen routines
*/

FILE* G_popen();
int G_pclose();

/*
** Other externs
*/

extern int ExecFork();
extern int rtd();

extern void SetSignals();
extern int rtime();
extern void restoropt();

#endif
