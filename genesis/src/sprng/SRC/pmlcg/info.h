

#define MAXVAL "100000000000000000000000" /*maximum # of independent streams*/

struct MP_ARRAY_TYPE 
{
  long size;
  MP_INT *list;
};

#define  OP_SIZE    2
#define  RNGBITS       3
#define  SHIFT      29
#define  MASK       0X1FFFFFFF
#define  MAGIC_NUM  "153722867280912930"
#define  MAGIC_DEN  "27097804800000000"
#define  PRIM       37
#define  POWER_N    61

/* Values pertain to this particular parameter: 2^61-1 as modulus*/
struct MP_ARRAY_TYPE init_factors()
{
  struct MP_ARRAY_TYPE factors;

  factors.size = 12;
    factors.list = malloc(12 * sizeof(MP_INT));
	
  mpz_init_set_str(&(factors.list[0]), "2", 10);
  mpz_init_set_str(&(factors.list[1]), "3", 10);
  mpz_init_set_str(&(factors.list[2]), "5", 10);
  mpz_init_set_str(&(factors.list[3]), "7", 10);
  mpz_init_set_str(&(factors.list[4]), "11", 10);
  mpz_init_set_str(&(factors.list[5]), "13", 10);
  mpz_init_set_str(&(factors.list[6]), "31", 10);
  mpz_init_set_str(&(factors.list[7]), "41", 10);
  mpz_init_set_str(&(factors.list[8]), "61", 10);
  mpz_init_set_str(&(factors.list[9]), "151", 10);
  mpz_init_set_str(&(factors.list[10]),"331", 10);
  mpz_init_set_str(&(factors.list[11]),"1321", 10);

  return (factors);
}

