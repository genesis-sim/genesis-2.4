#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "info.h"

#define LINEAR_SEARCH_LIMIT 5000
#define MOST 18

typedef struct DATATYPE1 {
  long power;
  MP_INT *valid;
  struct MP_ARRAY_TYPE denom;
  struct MP_ARRAY_TYPE the_primes;
  struct MP_ARRAY_TYPE kill_us;
  MP_INT *prim;
  MP_RAT *magic;
} REL_PRIME_TABLE;


void free_mp_array(struct MP_ARRAY_TYPE *array)
{
  long i;
  
  for(i=0; i<array->size; i++)
    mpz_clear(&(array->list[i]));
  
  free(array->list);
  
  return;
}


/* This function merges two arrays */
extern struct MP_ARRAY_TYPE join(first, second)
     struct MP_ARRAY_TYPE first, second;
{
  struct MP_ARRAY_TYPE result;
  long count1 = 0; 
  long count2 = 0;
  MP_INT absfirst, abssecond;

  mpz_init(&absfirst);
  mpz_init(&abssecond);
  result.size = first.size + second.size;
  result.list = malloc(result.size * sizeof(MP_INT));
  if ((count1 < first.size) && (count2 < second.size)) {
    mpz_abs(&absfirst, &(first.list[count1]));
    mpz_abs(&abssecond, &(second.list[count2]));
  }
  while ((count1 < first.size) && (count2 < second.size)) {
    if (mpz_cmp(&absfirst, &abssecond) > 0) {
      mpz_init_set(&(result.list[count1+count2]),&(second.list[count2]));
      ++count2;
      if (count2 < second.size) 
	mpz_abs(&abssecond, &(second.list[count2]));
    }
    else {
      mpz_init_set(&(result.list[count1+count2]),&(first.list[count1]));
      ++count1;
      if (count1 < first.size)
	mpz_abs(&absfirst, &(first.list[count1]));
    }
  }
  mpz_clear(&absfirst);
  mpz_clear(&abssecond);
  for (;count1<first.size;count1++) 
    mpz_init_set(&(result.list[count1+count2]),&(first.list[count1]));
  for (;count2<second.size;count2++) 
    mpz_init_set(&(result.list[count1+count2]),&(second.list[count2]));
  return(result);
}

/* This function multiplies all elements by -1 */
extern struct MP_ARRAY_TYPE minus(list, size)
     MP_INT *list;
     long size;
{
  long count;
  struct MP_ARRAY_TYPE result;

  result.size = size;
  result.list = malloc(size * sizeof(MP_INT));
  for (count=0;count<size;count++) {
    mpz_init(&(result.list[count]));
    mpz_neg(&(result.list[count]), &(list[count]));
  }
  return(result);
}

/* This function multiplies and reduces an array */
extern struct MP_ARRAY_TYPE mult_reduce(number, set, limit)
     MP_INT *number, *limit;
     struct MP_ARRAY_TYPE set;
{
  struct MP_ARRAY_TYPE result;
  long count, count2;
  MP_INT plussed/*, temp*/;
  
#if 0
  mpz_neg(number,number);	/* multiply by -number, except with 1 */
  mpz_init(&temp);
  mpz_abs(&temp,number);
#endif
  mpz_init(&plussed);
  
  result.list = malloc(set.size * sizeof(MP_INT));
  count2 = 0;
  if (set.size > 0) {
    mpz_init(&(result.list[count2]));
    for (count=0;count<set.size;count++) 
    {
      /*if(mpz_cmp_ui(&(set.list[count]),1U) != 0)*/
	mpz_mul(&(result.list[count2]), &(set.list[count]), number);
	/*else
	mpz_mul(&(result.list[count2]), &(set.list[count]), &temp);*/
      
      mpz_abs(&plussed, &(result.list[count2]));
      if (mpz_cmp(&plussed, limit) < 0) {
	++count2;
	if (count2<set.size)
	  mpz_init(&(result.list[count2]));
      }
    }
  }
  result.size = count2;
  mpz_clear(&plussed);
  /*mpz_clear(&temp);*/
  return(result);
}

/* This makes a list of denominators out of a list of primes */
extern struct MP_ARRAY_TYPE find_denom(the_primes, maxsize, limit)
     MP_INT *the_primes, *limit;
     long maxsize;

{
  struct MP_ARRAY_TYPE newones, oldones, answer;
  int count;

  if (maxsize > 1)
    oldones = find_denom(&(the_primes[1]), maxsize-1, limit);
  else {
    oldones.size = 1;
    oldones.list = malloc(sizeof(MP_INT));
    mpz_init_set_str(oldones.list, "1", 10);
  }
  newones = mult_reduce(the_primes, oldones, limit);
  answer = join(oldones, newones);
    
  free_mp_array(&newones);
  free_mp_array(&oldones);
  
  return(answer);
}

/* Evaluates Mu function, given denominators */
extern void mu_eval(result, x, denominators)
     MP_INT *result;
     MP_INT *x;
     struct MP_ARRAY_TYPE denominators;
{
  MP_INT val;
  long count = 0;

  mpz_set_str(result, "0", 10);
  mpz_init_set_str(&val, "1", 10);
  while ((count < denominators.size) && (mpz_cmp_ui(&val, 0) != 0)) {
    mpz_div(&val, x, &(denominators.list[count]));
    mpz_add(result, result, &val);
    count++;
  }
  mpz_clear(&val);
}

/* Checks if x == 0 mod y, returns true if so */
extern short divisible(x, y)
     MP_INT *x, *y;
{
  MP_INT temp;
  int sign;

  mpz_init(&temp);
  mpz_mod(&temp, x, y);
  sign = mpz_cmp_si(&temp, 0);
  mpz_clear(&temp);
  if (sign == 0) {
    return(1);
  }
  else {
    return(0);
  }
}

/* Checks if any primes divide a number */
extern short any_divide(number, the_primes, no_of_primes)
     MP_INT *number, *the_primes; 
     long no_of_primes;
{
  if (no_of_primes == 1) {
    return(divisible(number, &(the_primes[0])));
  }
  else {
    if (divisible(number, &(the_primes[no_of_primes-1]))) {
      return(1);
    }
    else {
      return(any_divide(number, the_primes, no_of_primes - 1));
    }
  }
}

extern void incr(x, num)
     MP_INT *x;
     long num;
{
  mpz_add_ui(x, x, num);
}

extern void decr(x, num)
     MP_INT *x;
     long num;
{
  mpz_sub_ui(x, x, num);
}

/* Uses a linear search to find inverse value given guess, mu(guess) */
extern MP_INT linear_find(y, current_mu, guess, the_primes)
     MP_INT *y, *current_mu, *guess; 
     struct MP_ARRAY_TYPE the_primes; 
{  
  while (mpz_cmp(current_mu, y) < 0) {
    incr(guess, 1L);
    if (any_divide(guess, the_primes.list, the_primes.size)) {
    }
    else 
      incr(current_mu, 1L);
  }
  while (mpz_cmp(current_mu, y) > 0) {
    if (any_divide(guess, the_primes.list, the_primes.size))
      decr(guess, 1L);
    else {
      decr(guess, 1L);
      decr(current_mu, 1L);
    }
  }

  while (any_divide(guess, the_primes.list, the_primes.size)) 
    decr(guess, 1L);

  return(*guess);  
}

/* This guesses at mu_inverse */
extern MP_INT guess_mu_inverse(y, magic)
     MP_INT *y;
     MP_RAT *magic;
{
  MP_INT temp, guess;
  mpz_init(&temp);
  mpz_init(&guess);

  mpq_get_num(&temp, magic);
  mpz_mul(&guess, y, &temp);
  mpq_get_den(&temp, magic);
  mpz_div(&guess, &guess, &temp);
  mpz_clear(&temp);
  
  return(guess);
}   


/* Efficient caluculation of mu */
extern void Mu(result, x, left_to_kill, the_final_denom)
     MP_INT *x, *result;
     struct MP_ARRAY_TYPE left_to_kill, the_final_denom;
{
  MP_INT subout, first, second;
  struct MP_ARRAY_TYPE new_kill;

  if (left_to_kill.size <= 0) {
    mu_eval(result, x, the_final_denom);
  }
  else {
    mpz_init(&subout);
    mpz_init(&first);
    mpz_init(&second); 
    mpz_div(&subout, x, &(left_to_kill.list[0]));
    new_kill.size = left_to_kill.size - 1;
    new_kill.list = &(left_to_kill.list[1]);
    Mu(&first, x, new_kill, the_final_denom);
    Mu(&second, &subout, new_kill, the_final_denom);
    mpz_sub(result, &first, &second);
    mpz_clear(&first);
    mpz_clear(&second);
    mpz_clear(&subout);
  }
}

/* finds the desired result, given y, the primes to be killed, the denominators,
   the full set of initial primes, and the density of relative primes */
extern MP_INT find_M(guess, y, left_to_kill, denom, initial_primes, magic)
     MP_INT *guess, *y;
     struct MP_ARRAY_TYPE denom, initial_primes, left_to_kill;
     MP_RAT *magic;
{
  MP_INT temp, temp2, current_mu;

  mpz_init(&temp);
  mpz_init(&temp2);
  mpz_init(&current_mu);
#if 0
  Mu(&current_mu, guess, left_to_kill, denom); 

  mpz_sub(&temp, y, &current_mu);
  mpz_abs(&temp2, &temp);

  while (mpz_cmp_ui(&temp2, LINEAR_SEARCH_LIMIT) > 0) {
    temp2 = guess_mu_inverse(&temp, magic);
    mpz_add(guess, guess, &temp2);

    Mu(&current_mu, guess, left_to_kill, denom);

    mpz_sub(&temp, y, &current_mu);
    mpz_abs(&temp2, &temp);
  }
  mpz_clear(&temp);
  mpz_clear(&temp2);
#else
  mpz_set_str(guess, "17", 10);
  mpz_set_str(&current_mu, "1", 10);
#endif
  return(linear_find(y, &current_mu, guess, initial_primes));
} 

/* setup. Note: 'int param' must be another argument if modulii other than 2^61-1 will be used */
extern void init_rel_prime(data, maxval)
REL_PRIME_TABLE *data;
MP_INT *maxval;
{
/* modified by cmdavis */
  MP_INT limit, num, denom;
  struct MP_ARRAY_TYPE the_primes;
	
  /* initialize 'data.power' */ 
  (*data).power = POWER_N;

  /* initialize 'data.valid' */
  (*data).valid = malloc(sizeof(MP_INT));
  mpz_init_set((*data).valid, maxval);

  /* initialize 'data.prim'  */
  (*data).prim = malloc(sizeof(MP_INT));
  mpz_init_set_ui((*data).prim, PRIM);

  /* initialize 'data.magic' (density of relative primes) */
  (*data).magic = malloc(sizeof(MP_RAT)); mpq_init((*data).magic);
  mpz_init_set_str(&num,MAGIC_NUM,10);
  mpz_init_set_str(&denom,MAGIC_DEN,10);
  mpq_set_num((*data).magic,&num); mpq_set_den((*data).magic,&denom);

  /* initialize 'data.the_primes[]' */
  (*data).the_primes = init_factors();

  /* initialize 'data.kill_us[]' */	
  (*data).kill_us.size = (*data).the_primes.size - MOST;
  if ((*data).kill_us.size < 0)           
    (*data).kill_us.size = 0;
  (*data).kill_us.list = (*data).the_primes.list;
 
  /* initialize 'data.denom[]' */ 
  the_primes = minus(&((*data).the_primes.list[(*data).kill_us.size]),
		     (*data).the_primes.size - (*data).kill_us.size); 
  /* define maximum value for which setup is valid */
  mpz_init(&limit);
  limit = guess_mu_inverse(maxval, (*data).magic);
  mpz_mul_ui(&limit, &limit, 2);
  (*data).denom = find_denom(the_primes.list, the_primes.size, &limit);

  mpz_clear(&limit);
  mpz_clear(&num);
  mpz_clear(&denom);
  free_mp_array(&the_primes);
}



void free_rel_prime(REL_PRIME_TABLE *data)
{
  mpz_clear(data->valid);
  mpz_clear(data->prim);
  free(data->magic);
  free_mp_array(&(data->the_primes));
  free_mp_array(&(data->denom));
}



/* find nth number relatively prime, given data */
extern int rel_prime(result, number, data)
     MP_INT *result, *number;
     REL_PRIME_TABLE data;
{
  MP_INT guess;
  int i;
  REL_PRIME_TABLE newdata;
  mpz_init(result); 
  mpz_init(&guess);

  if (mpz_cmp(data.valid, number) >= 0) {
    mpq_get_num(&guess, data.magic);
    guess = guess_mu_inverse(number, data.magic);
    *result = find_M(&guess, number, 
		     data.kill_us, data.denom, 
		     data.the_primes, data.magic);
  }
  else {
    init_rel_prime(&newdata, number);
    rel_prime(result, number, newdata);
    free_rel_prime(&newdata);
  }

  /* Do NOT clear guess, since result uses its component array */
  
  return 0;
}

/* find nth primitive element, given data */
extern void prim_elt(result, number, data)
     MP_INT *result, *number;
     REL_PRIME_TABLE data;
{
  MP_INT x, pow,temp;
  mpz_init_set_str(&x, "2", 10);
  mpz_pow_ui(&x, &x, data.power);
  mpz_sub_ui(&x, &x, 1);

  mpz_init_set(&temp,number);
  mpz_add_ui(&temp,&temp,5UL); /* first few streams are bad; so offset by 5 streams */
  
  
  rel_prime(&pow, &temp, data);
  
  mpz_powm(result, data.prim, &pow, &x);

  mpz_clear(&pow);
  mpz_clear(&temp);
}

