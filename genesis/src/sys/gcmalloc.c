#ifdef GCMALLOC
#include <stdio.h>
#include "gc.h"
#include "gcmalloc.h"

void* gcmalloc(b)
size_t b;
{
	void*	mem;

	mem = GC_MALLOC(b > 0 ? b : 1);
	if (mem == NULL)
	    write(2, "gc_malloc returned NULL\n", 24);

	return mem;
}

void* gccalloc(b,n)

size_t	b;
size_t	n;

{
	void* mem;

	mem = GC_MALLOC(b*n > 0 ? b*n : 1);
	if (mem == NULL)
	    write(2, "gc_malloc returned NULL\n", 24);

	return mem;
}

void gcfree(p)

void* p;

{
}

void* gcrealloc(p, b)

void*	p;
size_t	b;
{
	void*	newp;

	newp = GC_REALLOC(p,b > 0 ? b : 1);
	if (newp == NULL)
	    write(2, "gc_malloc returned NULL\n", 24);

        return newp;
}
#endif
