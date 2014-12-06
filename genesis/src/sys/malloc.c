static char rcsid[] = "$Id: malloc.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: malloc.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.1  1992/12/11 19:05:31  dhb
** Initial revision
**
*/

#include <stdio.h>

extern char* gc_malloc();
extern void gc_free();

char* malloc(b)

int b;

{
	char*	mem;

	mem = gc_malloc(b > 0 ? b : 1);
	if (mem == NULL)
	    write(2, "gc_malloc returned NULL\n", 24);

	return mem;
}

char* calloc(b,n)

int	b;
int	n;

{
	char* mem;

	mem = gc_malloc(b*n > 0 ? b*n : 1);
	if (mem == NULL)
	    write(2, "gc_malloc returned NULL\n", 24);

	return mem;
}

void free(p)

char* p;

{
	/*gc_free(p);*/
}

char* realloc(p, b)

char*	p;
int	b;

{
	char*	newp;

	if (b <= 0)
	    b = 1;

	newp = gc_malloc(b);
	if (newp != NULL)
	  {
	    BCOPY(p, newp, b);
	    /*gc_free(p);*/
	  }
	else
	    write(2, "gc_malloc returned NULL\n", 24);

	return newp;
}
