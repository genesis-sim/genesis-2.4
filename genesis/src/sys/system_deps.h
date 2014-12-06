/*
** $Id: system_deps.h,v 1.2 2005/06/20 20:29:14 svitak Exp $
** $Log: system_deps.h,v $
** Revision 1.2  2005/06/20 20:29:14  svitak
** Added includes for defs of bcopy, memcpy, etc. This centralizes the
** portability issues with string.h and strings.h.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2000/10/09 23:02:50  mhucka
** Removed definitions of M_E and M_PI and #included math.h instead.
**
** Revision 1.7  1999/12/31 08:33:16  mhucka
** Added SIGTYPE macro for declaring signal types.
**
** Revision 1.6  1997/07/18 03:14:53  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5  1995/04/14 18:03:28  dhb
** Added check for getopt macro already defined.
**
** Revision 1.4  1994/04/14  19:26:27  dhb
** DEC Alpha compatability
**
** Revision 1.3  1993/10/15  18:26:37  dhb
** Fixed typo in Solaris patches.
**
** Revision 1.2  1993/09/17  16:25:14  dhb
** Solaris compatibility
**
** Revision 1.1  1992/12/11  19:05:42  dhb
** Initial revision
**
*/

/*
** Global include file for system dependent functions
** (c) Michael D. Speight 1991
*/

/*
** Following includes for bcopy, memcpy, etc.
*/
#include <string.h>
#include <strings.h>

#ifdef Solaris
#define	jmp_buf		sigjmp_buf
#define	setjmp(buf)	sigsetjmp(buf,1)
#define	longjmp		siglongjmp
#endif

/*
 * Define the return type of signals for different OSes.
 * The usual type for ANSI C systems is 'void', and for pre-ANSI
 * systems it is 'int'.
 */
#ifdef __STDC__
#define SIGTYPE void
#else
#define SIGTYPE int
#endif

extern SIGTYPE sig_msg_restore_context();
extern SIGTYPE restore_context();
extern SIGTYPE save_context();


/*
 * bcopy/bzero/bcmp cause no end of compatibility problems.
 * Here we try to work around them by defining appropriate macros.
 */
#ifdef SYSV
#define BCMP(s1, s2, length)  memcmp(s1, s2, length)
#define BCOPY(s1, s2, length) memcpy(s2, s1, length)
#define BZERO(s1, length) memset(s1, 0, length)
#else
#ifdef decalpha
#define BCMP(s1, s2, length)  bcmp((const char *)(s1), (const char *)(s2), length)
#define BCOPY(s1, s2, length) bcopy((const char *)(s1), (char *)(s2), length)
#define BZERO(s1, length)     bzero((char *)(s1), length)
#else
#define BCMP(s1, s2, length)  bcmp(s1, s2, length)
#define BCOPY(s1, s2, length) bcopy(s1, s2, length)
#define BZERO(s1, length)     bzero(s1, length)
#endif
#endif

#ifdef BSD
#define HAVE_VFORK
#endif

#ifdef LONGWORDS
#define WORD long
#define ADDR long
#else
#define WORD int
#define ADDR int
#endif

#define WORDSIZE sizeof(WORD)
#define ADDRSIZE sizeof(ADDR)
