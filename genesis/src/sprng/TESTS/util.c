#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifdef __STDC__
void *_mymalloc(size_t size, int line, char *message)
#else
void *_mymalloc(size, line, message)
size_t size;
int line;
char *message;
#endif
{
  char *temp;

  if(size == 0)
    return NULL;

  temp = (char *) malloc(size);
  
  
  if(temp == NULL)
    {
      printf("\nmemory allocation failure in file: %s at line number: %d\n", message, line);
      exit(-1);
    }

  return (void *) temp;
}
