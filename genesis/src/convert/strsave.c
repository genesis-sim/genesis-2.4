static char rcsid[] = "$Id: strsave.c,v 1.2 2005/06/20 19:31:22 svitak Exp $";

/*
** $Log: strsave.c,v $
** Revision 1.2  2005/06/20 19:31:22  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2003/05/29 22:09:06  gen-dbeeman
** Included <errno.h> in convert/strsave.c and symtab.c, needed for gcc 3.2
**
** Revision 1.3  2000/07/12 06:11:55  mhucka
** Added #include of string.h and malloc.h.
**
** Revision 1.2  1997/08/08 19:28:43  dhb
** Added include of stdlib.h for malloc() return type
**
** Revision 1.1  1995/01/13 01:09:48  dhb
** Initial revision
**
 **
 ** This file is from GENESIS 1.4.1
 **
** Revision 1.1  1992/12/11  19:05:05  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

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

char *CopyString(str)

char* str;

{
	return strsave(str);
}
