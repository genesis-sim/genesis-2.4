############################################################################
# This is a make file compiles the test executables
#
# We have set variables giving information on the archiver, the C compiler 
# and the FORTRAN compiler for certain machines in files make.xxx, where xxx 
# is the machine name, set in ../make.CHOICES. 
#
# Typing the command below    => results in the following being created
#      make                        => All the sequential and parallel tests.
#      make xxx                    => Test 'xxx.rng' for each generator 'rng'.
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
#
# Object files and the test executables created can be deleted finally
# by typing
#       make realclean
#
# Set 'LIB_DIR' to the directory where the SPRNG libraries are. 
# Set 'INCDIR' to the directory where the SPRNG include files are. 
# We have already set them to the correct locations, unless whoever installed
# the libraries changed them.
############################################################################
LIB_DIR = ../$(LIB_REL_DIR)
INCDIR = ../include
SRCDIR = ../SRC
TESTLIB = sprngtest
#GMPLIB = -lgmp #uncomment for pmlcg

#############################################################################
# Random number libraries

# lcg, lfg, or lcg64 for 48 bit, Fibonacci, and 64 bit LCG cmrg for
# the combined multiple recursive genrator, pmlcg for the prime
# modulus LCG, and mlfg for the multiplicative lagged Fibonacci
# generator respectively.

LIBLIST = lfg

# To create a new test 'xxx', place the source code in file 'xxx.c`
# Then append 'xxx' to TESTLIST below. This assumes that 'xxx.c'
# depends on 'tests.h' and 'util.h' alone. If there are more dependencies,
# then explicitly write the commands for that. 'xxx.yyy' files are created
# for each random number library 'libyyy.a' in LIBLIST.

TESTLIST = maxt collisions equidist serial gap perm runs coupon poker  sum random_walk wolff metropolis
#
# ____________________________________________________________________
# The user should normally not need to change anything below this line
# _____________________________________________________________________
##############################################################################

INCLUDEDIR = -I$(INCDIR)

all : $(LIB_DIR)/lib$(TESTLIB).a $(TESTLIST) 

$(LIB_DIR)/lib$(TESTLIB).a: init_tests.o util.o \
                           chisquare.o stirling.o communicate.o 
	$(AR) $(ARFLAGS) $(LIB_DIR)/lib$(TESTLIB).a  \
                init_tests.o util.o chisquare.o stirling.o communicate.o 
	$(RANLIB) $(LIB_DIR)/lib$(TESTLIB).a

init_tests.o : init_tests.c
	$(CC) -c $(CFLAGS) $(INCLUDEDIR) $(DEF) init_tests.c

util.o : util.c
	$(CC) -c $(CFLAGS) $(INCLUDEDIR) $(DEF) util.c

stirling.o : stirling.c util.h
	$(CC) -c $(CFLAGS) $(INCLUDEDIR) $(DEF) stirling.c

chisquare.o : chisquare.c util.h
	$(CC) -c $(CFLAGS) $(INCLUDEDIR) $(DEF) chisquare.c

communicate.o : communicate.c
	$(CC) -c  $(CFLAGS)  $(INCLUDEDIR) $(DEF) communicate.c

$(BASE).o : $(BASE).c tests.h util.h
	$(CC) -c $(CFLAGS) $(INCLUDEDIR) $(DEF) $(BASE).c

$(TESTLIST) : 
	@for l in $(LIBLIST) ; do \
	  $(MAKE) LIB=$$l BASE=$@ DEF=$(DEF) PLAT=$(PLAT) \
                  $@.$$l ; \
        done

$(BASE).$(LIB) : $(BASE).o $(LIB_DIR)/lib$(TESTLIB).a $(LIB_DIR)/lib$(LIB).a
	$(CLD) $(CLDFLAGS) -o $@ $(BASE).o -L$(LIB_DIR) -l$(TESTLIB) -l$(LIB) $(GMPLIB) -lm $(MPIDIR) $(MPILIB)

#######################################################################
#  FFT test for parallel RNGs
#

fft : 
	@for l in $(LIBLIST) ; do \
	  $(MAKE) LIB=$$l DEF=$(DEF) PLAT=$(PLAT) \
                  $@.$$l ; \
        done

fft.o : fft.c $(INCDIR)/sprng.h
	$(CC) -c $(CFLAGS) $(INCLUDEDIR) $(DEF) $(CHK) -I$(INCDIR) fft.c 

fft.$(LIB) : fft.o $(LIB_DIR)/lib$(TESTLIB).a $(LIB_DIR)/lib$(LIB).a
	$(CLD) $(CLDFLAGS) $(INCLUDEDIR) $(DEF) $(CHK) -I$(INCDIR) -o $@ fft.o -L$(LIB_DIR) -l$(LIB) -lcomplib.sgimath -lftn $(GMPLIB)


#########################################################################

clean :
	@rm -f *.o

realclean :
	@rm -f *.o $(LIB_DIR)/lib$(TESTLIB).a
	@for l in $(LIBLIST) ; do \
	  rm -f *.$$l ; \
        done
	rm -f *~ core a.out
.SUFFIXES : 
