static char rcsid[] = "$Id: symtab.c,v 1.3 2005/06/25 22:04:34 svitak Exp $";

/*
** $Log: symtab.c,v $
** Revision 1.3  2005/06/25 22:04:34  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.2  2005/06/20 19:31:25  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  2000/09/21 19:41:10  mhucka
** Added function declarations where appropriate.
**
** Revision 1.4  2000/07/12 06:14:31  mhucka
** Added #include of malloc.h.
**
** Revision 1.3  1996/05/23 23:16:59  dhb
** t3d/e port
**
** Revision 1.2  1993/10/12  21:52:39  dhb
** Fixed bad access to freed mem.
**
** Revision 1.1  1992/12/11  19:05:06  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "parse.h"
#include "symtab.h"
#include "ss_func_ext.h"
#include "system_deps.h"

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
