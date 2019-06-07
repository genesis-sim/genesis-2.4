#ifndef _sim_gcmalloc_h
#define _sim_gcmalloc_h
void * gcmalloc(size_t size);
void gcfree(void *ptr);
void * gccalloc(size_t nmemb, size_t size);
void * gcrealloc(void *ptr, size_t size);
#endif
