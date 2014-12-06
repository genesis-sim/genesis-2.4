/*
** $Id: symtab.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: symtab.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1995/01/13 01:09:48  dhb
** Initial revision
**
 **
 ** This file is from GENESIS 1.4.1
 **
** Revision 1.1  1992/12/11  19:05:11  dhb
** Initial revision
**
*/

typedef struct _symtabent
  {
    char		*sym_ident;
    Result		sym_val;
    struct _symtabent	*sym_next;
  } SymtabEnt;

/*
** Eventually Symtab will have more fields
*/

typedef struct _symtab
  {
    SymtabEnt	*sym_entlist;
  } Symtab;

extern Symtab *SymtabCreate();
extern Result *SymtabLook();
extern Result *SymtabNew();
extern char   *SymtabKey();

#ifdef COMMENT

typedef enum _symtabscope
  {
    SymAnyScope,	/* Used to look in current scope then next global */
    SymLocScope,	/* Used to define local or look in current scope */
    SymGlobalScope	/* Used to define global scope */
  } SymtabScope;

typedef struct _symtabent
  {
    char		*sym_ident;
    Result		sym_val;
    struct _symtabent	*sym_next;
  } SymtabEnt;

typedef struct _symtabstack
  {
    SymtabScope		sym_scope;
    SymtabEnt		*sym_tab;
    struct _symtabstack	*sym_prev;
  } SymtabStack;
#endif
