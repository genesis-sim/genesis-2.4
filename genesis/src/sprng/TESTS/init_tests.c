#ifdef SPRNG_MPI
#include <mpi.h>
#endif
#include <stdio.h>
/*#define READ_FROM_STDIN*/   /* read random numbers from stdin */
#ifndef READ_FROM_STDIN
#include "sprng.h"
#endif
#include "util.h"

#ifndef ANSI_ARGS
#ifdef __STDC__
#define ANSI_ARGS(args) args
#else
#define ANSI_ARGS(args) ()
#endif
#endif

#define OFFSET 0		/* set offset to 'k' to start tests
				   from the k th stream onwards */

long NTESTS = 0;
int proc_rank=0, init_nprocs=1;
int nsubsequences=0;

static int init_seed, init_param, init_total;
static int current_subsequence;
static long current_group, first_group, init_ngroups;
static int **gens, current_gen, n_combine, skip;

long init_streams ANSI_ARGS((int argc, char *argv[]));
void next_stream ANSI_ARGS((void));
double get_rn ANSI_ARGS((void));


#ifdef READ_FROM_STDIN		/* read random numbers from stdin */
int *init_sprng(int a, int b, int c, int d)
{
  return NULL;
}

int free_sprng(int *a)
{
  return 0;
}

double sprng(int *a)
{
  double rn;
  scanf("%lf", &rn);
  return rn;
}
#endif

#ifdef __STDC__
long init_tests(int argc, char *argv[])
#else
long init_tests(argc, argv)
int argc;
char *argv[];
#endif
{
  long n;

#ifdef SPRNG_MPI
  MPI_Init(&argc, &argv);
#endif

  if(argc < 6+1)
  {
    fprintf(stderr,"Usage: %s n_sets ncombine seed param nsubsequences skip test_arguments\n",
	    argv[0]);
    exit(-1);
  }
  
  if(atoi(argv[2]) <= 0)
  {
    fprintf(stderr,"Error: Second command line argument should be greater than 0\n");
    exit(-1);
  }
  
  if(atoi(argv[5]) <= 0)
  {
    fprintf(stderr,"Error: Fifth command line argument should be greater than 0\n");
    exit(-1);
  }
  
  n = init_streams(argc, argv);
  
  return n;
}


#ifdef __STDC__
long init_streams(int argc, char *argv[])
#else
long init_streams(argc, argv)
int argc;
char *argv[];
#endif
{
  int seed, param, n, i, j;
  int myid = 0, nprocs = 1;
  long k;
  
  n = atoi(argv[1]);
  n_combine = atoi(argv[2]);
  seed = atoi(argv[3]);
  param = atoi(argv[4]);
  nsubsequences = atoi(argv[5]);
  skip = atoi(argv[6]);
  
#ifdef SPRNG_MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  proc_rank = myid;
  init_nprocs = nprocs;
#endif

   if(proc_rank == 0)
   {
     for(i=0; i<argc; i++)
       printf("%s  ", argv[i]);
     putchar('\n');
   }
   
  first_group = current_group = n*myid/nprocs;
  init_seed = seed;
  init_param = param;
  init_total = n;
  NTESTS = n*nsubsequences;
  current_gen = 0;
  current_subsequence = 0;
  
  gens = (int **) mymalloc(n_combine*sizeof(int *));
  for(i=0; i<n_combine; i++)
    gens[i] = init_sprng(n_combine*current_group+i+OFFSET,n_combine*n+OFFSET,seed,init_param);
  
  init_ngroups = n*(myid+1)/nprocs - n*myid/nprocs;
  
  return init_ngroups*nsubsequences;
}

#ifdef __STDC__
void next_stream(void)
#else
void next_stream()
#endif
{
  int i;
  double temp;
  
  current_subsequence = (current_subsequence + 1)%nsubsequences;
  
  if(current_subsequence > 0)
    for(i=0; i<skip; i++)
    {
      temp = get_rn();
    }
  else
  {
    current_group++;
    current_gen = 0;
  
    for(i=0; i<n_combine; i++)
      free_sprng(gens[i]);

    if(current_group > first_group && current_group < first_group+init_ngroups)
      for(i=0; i<n_combine; i++)
	gens[i] = init_sprng(n_combine*current_group+i+OFFSET,n_combine*init_total+OFFSET,
			     init_seed, init_param);
    else if(current_group > first_group && current_group >
	    first_group+init_ngroups)
      printf("ERROR: current_pair = %ld not in allowed range [%d,%ld]\n",
	     current_group,0,init_ngroups-1 ); 
    
  }
  
}

#ifdef __STDC__
double get_rn(void)
#else
double get_rn()
#endif
{
  double temp;
  
  temp = sprng(gens[current_gen]);
  current_gen = (current_gen+1)%n_combine;
  
  return temp;
}


#if 0
#ifdef __STDC__
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
  int n, i, j, length;

  n = init_tests(argc,argv);
  length = atoi(argv[7]);
  
  for(i=0; i<n; i++)
  {
    for(j=0; j<length; j++) 
    { 
      printf("(%d, %d, %d). current_gen = %d, current_group = %d", 
	     proc_rank, i, j, current_gen, current_group);
      printf(" rn = %f\n", get_rn()); 
    } 

    next_stream(); 
  } 

#ifdef SPRNG_MPI 
  MPI_Finalize();
#endif
}
#endif
