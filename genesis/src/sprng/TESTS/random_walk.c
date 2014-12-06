/* A two-dimensional random walk test based on the code of I. Vattulainen, 
   et al  */

#include <stdio.h>
#include "tests.h"
#include <math.h>

#define sgn(a) ((a<0)?0:1)

#ifndef ANSI_ARGS
#ifdef __STDC__
#define ANSI_ARGS(args) args
#else
#define ANSI_ARGS(args) ()
#endif
#endif

void random_walk ANSI_ARGS((int walk_length, double *quadrant));


#ifdef __STDC__
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
  long ntests, walk_length, i;
  double result, expected, chisum, quadrant[4];
  
  quadrant[0] = quadrant[1] = quadrant[2] = quadrant[3] = 0;
  
  if(argc != N_STREAM_PARAM + 2)
  {
    fprintf(stderr,"USAGE: %s (... %d arguments)\n",argv[0], N_STREAM_PARAM+1);
    exit(1);
  }
  
  ntests = init_tests(argc,argv);
  
  walk_length = atoi(argv[N_STREAM_PARAM+1]);
  
  for(i=0; i<ntests; i++)
  {
    random_walk(walk_length, quadrant);
    next_stream();
  }
  
#if defined(SPRNG_MPI)
  reduce_sum(quadrant,4);  
#endif
  
  if(proc_rank == 0)
  {    
    chisum = 0.0;
    expected = NTESTS/4.0;
    
    for(i=0; i<4; i++)
    {
      chisum += (quadrant[i]-expected)*(quadrant[i]-expected)/expected;
    }
    
    result = chipercent(chisum, 3);
    printf("Chi square value = %f, percent = %f\n", chisum, result*100);
  }
  

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

}



#ifdef __STDC__
void random_walk(int walk_length, double *quadrant)
#else
void random_walk(walk_length, quadrant)
int walk_length;
double *quadrant;
#endif
{
  int x, y, i, j;
  double rn;
  
  x = y = 0;
  
  for(i=0; i<walk_length; i++)
  {
    rn = get_rn();
    
    if(rn > 0.75)
      x++;
    else if(rn > 0.5)
      x--;
    else if(rn > 0.25)
      y++;
    else
      y--;
  }

  if(x == 0 && y == 0)
  {
    quadrant[0] += 0.25;
    quadrant[1] += 0.25;
    quadrant[2] += 0.25;
    quadrant[3] += 0.25;
  }
  else if(x == 0)
  {
    quadrant[sgn(y)] += 0.5;
    quadrant[2+sgn(y)] += 0.5;
  }
  else if(y == 0)
  {
    quadrant[2*sgn(x)]   += 0.5;
    quadrant[2*sgn(x)+1] += 0.5;
  }
  else
    quadrant[2*sgn(x)+sgn(y)] += 1.0;
}
