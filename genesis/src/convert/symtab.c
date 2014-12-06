static char rcsid[] = "$Id: symtab.c,v 1.4 2005/07/20 20:01:58 svitak Exp $";

/*
** $Log: symtab.c,v $
** Revision 1.4  2005/07/20 20:01:58  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/07/01 10:03:01  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/20 19:31:23  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2003/05/29 22:09:07  gen-dbeeman
** Included <errno.h> in convert/strsave.c and symtab.c, needed for gcc 3.2
**
** Revision 1.4  2001/04/18 22:39:36  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.3  2000/07/12 06:12:11  mhucka
** Added #include of malloc.h.
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
** Revision 1.2  1993/10/12  21:52:39  dhb
** Fixed bad access to freed mem.
**
** Revision 1.1  1992/12/11  19:05:06  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "symtab.h"

#include <errno.h>

/*
** symtab.c
**
**	Symbol table routines.
*/


extern int errno;

Symtab *SymtabCreate()

{	/* SymtabCreate --- Create an empty symbol table */

	Symtab	*symtab;

	symtab = (Symtab *) malloc(sizeof(Symtab));
	if (symtab == NULL)
	  {
	    perror("SymtabCreate");
	    sig_msg_restore_context(0, errno);
	  }

	symtab->sym_entlist = NULL;
	return(symtab);

}	/* SymtabCreate */



void SymtabDestroy(symtab)

Symtab	*symtab;

{	/* SymtabDestroy --- Destroy symbol table */

	SymtabEnt	*symtabent;

	symtabent = symtab->sym_entlist;
	while (symtabent != NULL)
	  {
	    SymtabEnt	*next;

	    next = symtabent->sym_next;
	    free(symtabent->sym_ident);
	    free(symtabent);

	    symtabent = next;
	  }
	free(symtab);

}	/* SymtabDestroy */


Result *SymtabLook(symtab, sym)

Symtab		*symtab;
char		*sym;

{	/* SymtabLook --- Look for symbol table entry for sym */

	SymtabEnt	*symtabent;

	if (symtab == NULL)
	    return(NULL);

	symtabent = symtab->sym_entlist;
	while (symtabent != NULL)
	  {
	    if (strcmp(symtabent->sym_ident, sym) == 0)
		return(&symtabent->sym_val);

	    symtabent = symtabent->sym_next;
	  }
	return(NULL);

}	/* SymtabLook */



char *SymtabKey(symtab, rp)

Symtab	*symtab;
Result	*rp;

{	/* SymtabKey --- Return sym_ident for given result address */

	SymtabEnt	*symtabent;

	if (symtab == NULL)
	    return(NULL);

	symtabent = symtab->sym_entlist;
	while (symtabent != NULL)
	  {
	    if (&symtabent->sym_val == rp)
		return(symtabent->sym_ident);

	    symtabent = symtabent->sym_next;
	  }
	return(NULL);

}	/* SymtabKey */



Result *SymtabNew(symtab, sym)

Symtab		*symtab;
char		*sym;

{	/* SymtabNew --- Enter new symbol table entry */

	SymtabEnt	*symtabent;
	Result		*rp;
	extern char     *strsave();

	rp = SymtabLook(symtab, sym);
	if (rp == NULL)
	  {
	    symtabent = (SymtabEnt *) malloc(sizeof(SymtabEnt));
	    if (symtabent == NULL)
	      {
		perror("SymtabNew");
		sig_msg_restore_context(0, errno);
	      }

	    symtabent->sym_ident = (char *) strsave(sym);
	    symtabent->sym_next = symtab->sym_entlist;
	    symtab->sym_entlist = symtabent;
	    rp = &symtabent->sym_val;
	    rp->r_type = 0;
	  }

	return(rp);

}	/* SymtabNew */



#ifdef COMMENT

SymtabPush(scope)

SymtabScope	scope;

{	/* SymtabPush --- Allocate a new symbol table */

	SymtabStack	*symtab;

	symtab = (SymtabStack *) malloc(sizeof(SymtabStack));
	if (symtab == NULL)
	  {
	    perror("SymtabPush");
	    exit(1);
	  }

	symtab->sym_scope = scope;
	symtab->sym_tab = NULL;
	symtab->sym_prev = CurSymtab;
	CurSymtab = symtab;

}	/* SymtabPush */



SymtabPop()

{	/* SymtabPop --- Pop the current symbol table */

	SymtabEnt	*symtabent;

	if (CurSymtab == NULL)
	  {
	    fprintf(stderr, "SymtabPop: Symbol table stack is empty!\n");
	    return;
	  }

	symtabent = CurSymtab->sym_tab;
	while (symtabent != NULL)
	  {
	    free(symtabent->sym_ident);
	    free(symtabent);
	    symtabent = symtabent->sym_next;
	  }
	free(CurSymtab);

	CurSymtab = CurSymtab->sym_prev;

}	/* SymtabPop */
#endif
