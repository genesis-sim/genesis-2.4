#define YES 1
#define NO  0
#define NULL 0
#define PARAM 0
#define SIMPLE_SPRNG

        program test_simple_generator

	implicit none
#include "sprng_f.h"      

        integer check_simple_gen
        integer check_simple_errors
        integer result, temp
        integer seed1, seed2

        result = YES
        seed1 = make_sprng_seed()
        seed2 = make_sprng_seed()

        if( seed1 .eq. seed2 ) then
          print *, 'ERROR: make_sprng_seed does not return unique seeds'
	  result = NO
        end if

        temp = check_simple_gen()
        if(temp .ne. YES) then
          result = NO
        end if

        temp = check_simple_errors()
        if(temp .ne. YES) then
          result = NO
        end if

	print *, '  '
        if(result .eq. YES) then
          print *, 'Result:  PASSED'
        else
          print *, 'Result:  FAILED'
        end if
	print *, '  '

        end


        integer function check_simple_gen()

	implicit none
#include "sprng_f.h"

        SPRNG_POINTER gen
	integer tempi1, tempi2, seed, size, dblmult
	real tempf1
	real*8 tempd1
        integer i, correct, result, fltmult
        character s(MAX_PACKED_LENGTH)

	correct = YES
	result = YES
	seed = 985456376
	fltmult = 2**20
	dblmult = 2**30

        gen = init_sprng(seed,PARAM)  
	if (gen .eq. NULL) then
	  result = NO
	  print *, ' FAILED: SPRNG unable to initialize the generator.'
	endif

C -- check default generator

        do 100 i = 1,200
          tempi2 = isprng()
          read *, tempi1
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 100    continue
  
  	if(correct .eq. NO) then 
    	   print *, 'ERROR: Integer generator incorrect.'
	else
c 	   print *, 'PASSED: Integer generator passed reproducibility test.'
	endif

        correct = YES
	do 200 i = 1,50
          tempf1 = fget_rn_flt_sim()
	  tempd1 = tempf1
	  read *, tempi1
	  tempi1 = tempi1 / (2**11)
	  tempi2 = tempd1 * fltmult
	  
	  if ( abs(tempi1-tempi2) .gt. 1) then
	    result = NO
	    correct = NO
          end if
 200    continue
        if(correct .eq. NO) then
	   print *, 'ERROR: Float generator incorrect.'
	else
c 	   print *, 'PASSED: Float generator passed reproducibility test.'
	endif

        correct = YES
        do 300 i = 1,50
	  tempd1 = sprng()
          read *, tempi2
	  tempi1 = tempd1 * dblmult
	  if ( abs(tempi2/2-tempi1) .gt. 1) then
            result = NO
            correct = NO
          end if
 300    continue
  	if(correct .eq. NO) then 
    	   print *, 'ERROR: Double generator incorrect stream.'
	else
c  	   print *, 'PASSED: Double generator passed reproducibility test.'
	endif

C --  Pack and unpack generator --

  	size = pack_sprng(s(1))
	if (size .eq. 0) then
	  result = NO
          print *, 'FAILED: SPRNG was unable to pack the generator.'
        end if
        do 400 i = 1,100
          tempi1 = isprng()
 400    continue

	gen = unpack_sprng(s(1))
	if (gen .eq. NULL) then 
	  result = NO
	  print *, 'FAILED: SPRNG was unable to unpack the generator.'
	endif

  	correct = YES
        do 500 i = 1,100
          read *, tempi2
          tempi1 = isprng()
          if(tempi1 .ne. tempi2) then
            result = NO
            correct = NO
          end if
 500    continue
  
        if(correct .eq. NO) then
	   print *, 'ERROR: Incorrect stream produced after pack/unpack.'
        else
c 	   print *, 'PASSED: Generator packs and unpacks correctly.'
	end if

	check_simple_gen = result
	return
	end
	
	integer function check_simple_errors()

C  -- Check if generator meets specifications in handling errors --

	implicit none
#include "sprng_f.h"


	SPRNG_POINTER gen1
	integer i, tempi1, tempi2, correct, result, seed 
	character s(MAX_PACKED_LENGTH)

        result = YES
        seed = 985456376
	
	correct = YES
	
        do 100 i = 1, MAX_PACKED_LENGTH
           s(i) = '0'
 100    continue
c	print *, ' Expect SPRNG ERROR: packed string invalid.'
	
	gen1 = unpack_sprng(s(1))
	if (gen1 .ne. NULL) then
	  print *, 'FAILED: Generator unpacks invalid string.'
	else 
c	  print *, 'PASSED: Generator detected invalid string while unpacking.'
	endif

	correct = YES
        do 200 i = 1,50
          tempi2 = isprng()
          read *, tempi1
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 200    continue

        if(correct .eq. 0) then
           print *, 'FAILED: Original stream not maintained '
           print *, '    ... when unpacked stream is invalid.'
        else
c            print *, 'PASSED: Generator OK with invalid unpacked stream.'
        end if

        check_simple_errors = result
        return
        end





