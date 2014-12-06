static char rcsid[] = "$Id: strsave.c,v 1.2 2005/06/20 19:31:25 svitak Exp $";

/*
** $Log: strsave.c,v $
** Revision 1.2  2005/06/20 19:31:25  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  2000/07/12 06:14:19  mhucka
** Added #include of malloc.h and string.h.
**
** Revision 1.2  1996/05/23 23:16:59  dhb
** t3d/e port
**
** Revision 1.1  1992/12/11  19:05:05  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "system_deps.h"

extern int errno;

char *strsave(cp)

char	*cp;

{	/* strsave --- Allocate mem and save copy of string */

	char	*mem;

	mem = (char *) malloc(strlen(cp)+1);
	if (mem == NULL)
	  {
	    perror("strsave");
	    sig_msg_restore_context(0, errno);
	    /* No Return */
	  }

	strcpy(mem, cp);
	return(mem);

}	/* strsave */
