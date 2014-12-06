#ifdef i860
int system(string)
     char * string;
{
  printf(string);
  return(1);
}
#endif
/*
** #ifdef NO-X
** axis_graph_add_pts(){}
** #endif
*/

#if defined(SYSV) && !defined(Linux) && !defined(aix) && !defined(__FreeBSD__) && !defined(__APPLE__) && !defined(Cygwin)
#include <string.h>
char *index(s, c)
char *s;
char c;
{
      return(strchr(s,(int)c));
}

char *rindex(s, c)
char *s;
char c;
{
      return(strrchr(s,(int)c));
}

void *BCOPY(src, dest, len)
void *src;
void *dest;
size_t len;
{
      return(memmove(dest, src, len));
}
#endif

#if defined( BSD ) || defined( aix )
/* UNUSED */
static void dummy()
{
  /* This is to prevent the compiler from complaining about an empty file. */
  /* UNUSED */
  int x;
}
#endif
