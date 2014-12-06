#define YES 1
#define NO  0
#define NULL 0
#define PARAM 0

        program test_generator

	implicit none
#include "sprng_f.h"      

        integer check_gen
        integer check_errors
        integer result, temp
        integer seed1, seed2

        result = YES

C -- Checking make_sprng_seed ...
        seed1 = make_sprng_seed()
        seed2 = make_sprng_seed()
        if( seed1 .eq. seed2 ) then
          print *, 'ERROR: make_sprng_seed does not return unique seeds'
	  result = NO
        end if

C -- Check generator with correct parameters --
        temp = check_gen()
        if(temp .ne. YES) then
          result = NO
        end if

C -- Check if generator meets specifications in handling errors --
        temp = check_errors()
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


        integer function check_gen()

	implicit none
#include "sprng_f.h"

        SPRNG_POINTER gen1, gen2, gen3, gen4
	SPRNG_POINTER gen5(1), newgen1(2), newgen2(2)
	SPRNG_POINTER tmpGen
	integer tempi1, tempi2
	real tempf1, tempf2
	real*8 tempd1, tempd2
        integer i, correct, result
        integer temp
	integer ngens, seed
	integer nsp
	integer size
        character s(MAX_PACKED_LENGTH)

	ngens = 3
	seed = 985456376
        gen1 = init_sprng(0,ngens,seed,PARAM)  
        gen2 = init_sprng(1,ngens,seed,PARAM)
        gen3 = init_sprng(2,ngens,seed,PARAM)

	result = YES
        correct = YES

C -- Check arithmetic for integer, float and double --
        do 100 i = 1,500
          tempi2 = isprng(gen1)
          read *, tempi1
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 100    continue
  
  	if(correct .eq. NO) then 
    	   print *, 'ERROR: Integer generator produces incorrect stream.'
C	else
C	   print *, 'PASSED: Integer generator passed reproducibility test.'
	endif

        correct = YES
	do 200 i = 1,500
          tempf1 = fget_rn_flt(gen1)
	  read *, tempf2
	  if ( abs(tempf2-tempf1) .ge. 1.e-6) then
	    result = NO
	    correct = NO
          end if
 200    continue
        if(correct .eq. NO) then
	   print *, 'FAILED: Float generator produces incorrect stream.'
	else
C	   print *, 'PASSED: Float generator passed reproducibility test.'
	endif

        correct = YES
        do 300 i = 1,500
	  tempd1 = fget_rn_dbl(gen1)
          read *, tempd2
	  if ( abs(tempd2-tempd1) .ge. 1.e-14) then
            result = NO
            correct = NO
          end if
 300    continue
  	if(correct .eq. NO) then 
    	   print *, 'FAILED: Double generator produces incorrect stream.'
	else
c 	   print *, 'PASSED: Double generator passed reproducibility test.'
	endif

C ---- check spawning ----

	nsp = 0
  	temp = spawn_sprng(gen2,2, newgen1(1))
	nsp = nsp + temp
	tmpGen = newgen1(2)
  	temp = spawn_sprng(tmpGen,2,newgen2(1))
	nsp = nsp + temp

	if (nsp .ne. 4) then 
	   result = NO
	   print *, 'FAILED: Generator was unable to spawn.'
	end if

        correct = YES
	tmpGen = newgen2(2)
        do 400 i = 1,50
          read *, tempi2
          tempi1 = isprng(tmpGen)
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 400    continue
  	if(correct .eq. NO) then 
    	   print *, 'FAILED: Generator incorrect after spawning.'
	   print *, 'Probably an error in spawning the generators.'
	else
C	   print *, 'PASSED: Generator spawns correctly.'
	endif  

C --  Pack and unpack generator --

  	size = pack_sprng(newgen2(2), s(1))
	if (size .eq. 0) then
	  result = NO
          print *, 'FAILED: Generator was unable to pack.'
        end if

	gen4 = unpack_sprng(s(1))
	if (gen4 .eq. 0) then
	  print *, 'Generator was unable to unpack '
	end if

  	correct = YES
        do 500 i = 1,50
          read *, tempi2
          tempi1 = isprng(gen4)
          if(tempi1 .ne. tempi2) then
            result = NO
            correct = NO
          end if
 500    continue
  
        if(correct .eq. NO) then
	   print *, 'FAILED: Incorrect after packing and unpacking.'
        else
C	   print *, 'PASSED: Generator packs and unpacks correctly.'
	end if

  	correct = YES
	temp = spawn_sprng(gen4,1,gen5(1))
	do 600 i=1, 50
	  read *, tempi2
	  tempi1 = isprng(gen5(1))
	  if(tempi1 .ne. tempi2) then
	    result = NO
	    correct = NO
	  end if
 600    continue

 	if(correct .eq. NO) then
	 print *, 'FAILED: Generator incorrect stream after pack/unpack.'
	else
C	    print *, 'PASSED: Generator packs/unpacks .'
	end if

C -- Free generators --
	nsp = free_sprng(gen1)
	nsp = free_sprng(gen2)
	nsp = free_sprng(gen3)
	if(nsp .ne.  6) then 
	  result = NO
	  print *, 'FAILED: Free returns ', nsp, 'instead of 6.'
	end if

	nsp = free_sprng(gen4)
	nsp = free_sprng(gen5(1))
	nsp = free_sprng(newgen1(1))
	nsp = free_sprng(newgen1(2))
	nsp = free_sprng(newgen2(1))
	nsp = free_sprng(newgen2(2))
	if(nsp .ne.  0) then 
	  result = NO
	  print *, 'FAILED: Free returns ', nsp, 'instead of 0.'
	end if

	check_gen = result
	return
	end
	
C -- Check if generator meets specifications in handling errors  -- 

	integer function check_errors()

	implicit none
#include "sprng_f.h"


	SPRNG_POINTER gen1, gen2(1)
	integer i
	integer tempi
	integer tempi1, tempi2
	integer correct, result
	integer seed, nsp
	character s(MAX_PACKED_LENGTH)

        result = YES
        seed = 985456376
	
	correct = YES
	
C -- checking incorrect ngens in init_sprng --
C	print *, 'Expect SPRNG WARNING: ngens <= 0.'
        gen1 = init_sprng(0,0,seed,PARAM)
        do 100 i = 1,50
          tempi2 = isprng(gen1)
          read *, tempi1
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 100    continue

        if(correct .eq. 0) then
           print *, 'FAILED: Generator does not produce expected stream'
           print *, '    ... when ngens = 0.'
        else
C           print *, 'PASSED: Generator OK when ngens = 0.'
        end if

C -- check if only one stream had been produced --
        nsp = free_sprng(gen1)
        if(nsp .ne. 0) then
           result = 0
           print *, 'FAILED: Generator produces more than 1 stream'
           print *, '     ... when ngens = 0.'
        else
C           print *, 'PASSED: Generator OK when ngens = 0.'
        endif

C -- checking invalid range for gennum --

	correct = YES
C	print *, 'Expect SPRNG ERROR: gennum not in range.'
	gen1 = init_sprng(-1,1,seed,PARAM)
	if (gen1 .ne. NULL) then
	  tempi = free_sprng(gen1)
	  result = NO
	  correct = NO
	endif

C        print *, 'Expect SPRNG ERROR: gennum not in range.'
        gen1 = init_sprng(2,1,seed,PARAM)
        if (gen1 .ne. NULL) then
          tempi = free_sprng(gen1)
          result = NO
          correct = NO
        endif

	if(correct .eq. NO) then
	  print *, 'FAILED: Generator wrong with wrong gennum.'
	else 
C	  print *, 'PASSED: Generator OK when gennum is incorrect.'
	endif


C-- Invalid parameter--
	correct = YES
C        print *, 'Expect SPRNG WARNING: Invalid parameter'
	gen1 = init_sprng(0,1,seed,2**30)
        do 200 i = 1,50
          tempi2 = isprng(gen1)
          read *, tempi1
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 200    continue

C-- Check spawn with invalid ngens --

C        print *, 'Expect SPRNG WARNING: nspawned <= 0.'
	nsp = spawn_sprng(gen1, 0, gen2(1))
	tempi = free_sprng(gen1)
        if (nsp .ne. 1) then
          result = NO
          print *, 'FAILED: Spawn returned', nsp
          print *, '     instead of 1 when nspawned = 0.'
        endif

C -- check spawned stream --
        do 300 i = 1,50
          tempi2 = isprng(gen2(1))
          read *, tempi1
          if(tempi2 .ne. tempi1) then
            result = NO
            correct = NO
          end if
 300    continue
 
	tempi = free_sprng(gen2(1))

        if(correct .eq. NO) then
          print *, 'FAILED: Incorrect when nspawned = 0.'
        else
C         print *, 'PASSED: Generator OK when nspawned = 0.'
	endif

C -- Unpack invalid string --

	do 400 i = 1, MAX_PACKED_LENGTH
	  s(i) = '0'
 400    continue
C        print *, 'Expect SPRNG ERROR: packed string invalid'
	gen1 = unpack_sprng(s)
	if (gen1 .ne. NULL) then
	  result = NO
	  print *, 'FAILED: Generator unpacks invalid string'
C 	else
C	  print *, 'PASSED: Detected invalid string while unpacking'
	endif

        check_errors = result
        return
        end





