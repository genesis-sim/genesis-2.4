/*
** $Id: shell_defs.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
** $Log: shell_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  2000/04/24 07:55:04  mhucka
** Added extern declarations for debug and progname.
**
** Revision 1.1  1992/12/11 19:04:51  dhb
** Initial revision
**
*/

#ifndef SHELL_DEFS_H
#define SHELL_DEFS_H

#define		MAXVARS	2000
#ifndef TRUE
#define		TRUE		1
#define		FALSE		0
#endif

#define		I_TYPE 0
#define		F_TYPE 1
#define		S_TYPE 2

#define         FILE_TYPE       0
#define         STR_TYPE        1

extern int debug;
extern char *progname;

#define Debug(name)	(name + debug)

#endif /* SHELL_DEFS_H */
