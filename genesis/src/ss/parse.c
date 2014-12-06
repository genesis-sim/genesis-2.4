static char rcsid[] = "$Id: parse.c,v 1.3 2005/06/20 19:31:25 svitak Exp $";

/*
** $Log: parse.c,v $
** Revision 1.3  2005/06/20 19:31:25  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.2  2005/06/18 19:23:36  svitak
** Changed return type of PTFree to void.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2003/05/29 22:34:11  gen-dbeeman
** Replaced C++ style comments for K&R compatiblility
**
** Revision 1.7  2003/03/28 21:02:32  gen-dbeeman
** Changes from Hugo Cornelis to enable debugging commands gctrace and gftrace
**
** Revision 1.6  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/09/21 19:41:10  mhucka
** Added function declarations where appropriate.
**
** Revision 1.3  2000/07/12 06:14:04  mhucka
** Added #include of malloc.h.
**
** Revision 1.2  1996/05/23 23:16:59  dhb
** t3d/e port
**
** Revision 1.1  1992/12/11  19:05:04  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "parse.h"
#include "system_deps.h"

/*
** parse.c
**
**	Routines involved in parse tree construction.
*/

extern int errno;

ParseNode *PTNew(type, data, left, right)

int		type;
ResultValue	data;
ParseNode	*left;
ParseNode	*right;

{	/* PTNew --- Make a new parse tree node and initialze fields */

        extern SIGTYPE sig_msg_restore_context();
	ParseNode	*pn;

	pn = (ParseNode *) malloc(sizeof(ParseNode));
	if (pn == NULL)
	  {
	    perror("PTNew");
	    sig_msg_restore_context(0, errno);
	    /* No Return */
	  }

	pn->pn_val.r_type = type;
	pn->pn_val.r = data;
	pn->pn_left = left;
	pn->pn_right = right;

	/* erase parse node info */

	pn->pcInfo = NULL;

	return(pn);

}	/* PTNew */


void PTFree(pn)

ParseNode	*pn;

{	/* PTFree --- Free an entire parse tree given root */


	if (pn != NULL)
	  {
	    PTFree(pn->pn_left);
	    PTFree(pn->pn_right);

	    /* free parse node info */

	    if (pn->pcInfo)
		free(pn->pcInfo);

	    free(pn);
	  }

}	/* PTFree */
