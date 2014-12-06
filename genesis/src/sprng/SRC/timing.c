#include <stdio.h>
#include <stdlib.h>
#include "cputime.h"
#include "interface.h"

#define TIMING_TRIAL_SIZE 1000000
#define PARAM 0

int main()
{
  int i;
  int *gen;
  double temp1, temp2, temp3, temp4;
  double temp_mult = TIMING_TRIAL_SIZE/1.0e6;
  
  gen = init_rng(0,1,0,PARAM);
  /*print_rng(gen);*/
  
  temp1 = cputime();

  for(i=0; i<TIMING_TRIAL_SIZE; i++)
    get_rn_int(gen);
  
  temp2 = cputime();
  

  for(i=0; i<TIMING_TRIAL_SIZE; i++)
    get_rn_flt(gen);
  
  temp3 = cputime();
  

  for(i=0; i<TIMING_TRIAL_SIZE; i++)
    get_rn_dbl(gen);
  
  temp4 = cputime();
  
  if(temp2-temp1 < 1.0e-15 || temp3-temp2 < 1.0e-15 ||  temp4-temp3 < 1.0e-15)
  {
    printf("Timing Information not available/not accurate enough.\n\t...Exiting\n");
    exit(1);
  }
  
  /* The next line is just used to ensure that the optimization does not remove the call to the RNG. Nothing is really printed.             */
  fprintf(stderr,"Last random number generated %f\n", get_rn_dbl(gen));

  printf("\nUser + System time Information (Note: MRS = Million Random Numbers Per Second)\n");
  printf("\tInteger generator:\t Time = %7.3f seconds => %8.4f MRS\n", 
	 temp2-temp1, temp_mult/(temp2-temp1));
  printf("\tFloat generator:\t Time = %7.3f seconds => %8.4f MRS\n", 
	 temp3-temp2, temp_mult/(temp3-temp2));
  printf("\tDouble generator:\t Time = %7.3f seconds => %8.4f MRS\n", 
	 temp4-temp3, temp_mult/(temp4-temp3));
  putchar('\n');
  
}

