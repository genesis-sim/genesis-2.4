#ifdef GCMALLOC
    #include <stdlib.h>
    #define malloc(x) gcmalloc(x)
    #define calloc(x,y) gccalloc(x,y)
    #define free(x) gcfree(x)
    #define realloc(x,y) gcrealloc(x,y)
    extern void * gcmalloc(size_t size);
    extern void gcfree(void *ptr);
    extern void * gccalloc(size_t nmemb, size_t size);
    extern void * gcrealloc(void *ptr, size_t size);
#endif
