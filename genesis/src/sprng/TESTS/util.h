#ifndef _util_h
#define _util_h

#include <stddef.h>

#define mymalloc(a) (_mymalloc((a), __LINE__, __FILE__))

#ifndef ANSI_ARGS
#ifdef __STDC__
#define ANSI_ARGS(args) args
#else
#define ANSI_ARGS(args) ()
#endif
#endif

void *_mymalloc ANSI_ARGS((size_t size, int line, char *message));

#endif
