############################################################################
#
# Typing the command below   => results in the following being created
#      make               => Fortran and C libraries for the generator,
#                            Timing executibles.
#      make lcg64	  => Library for 64 bit Linear Congruential Generator
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
############################################################################

LIBDIR = $(LIB_DIR)
CFLAGS = $(CFLAGS_IN)
SRCDIR = ..
CHKDIR = ../..

all : lcg64 timesprng checksprng


#---------------------------------------------------------------------------
# Construct Library
#---------------------------------------------------------------------------
SPRNG_COMMON_DEPEND = $(SRCDIR)/fwrap_mpi.o  $(SRCDIR)/cputime.o \
		      $(SRCDIR)/makeseed.o  $(SRCDIR)/simple_mpi.o \
		      $(SRCDIR)/memory.o  $(SRCDIR)/communicate.o  $(SRCDIR)/checkid.o $(SRCDIR)/store.o

lcg64 :
	(cd ..; $(MAKE) sprng_common)
	$(MAKE) $(LIBDIR)/liblcg64.a

$(LIBDIR)/liblcg64.a : lcg64.o primes.o  $(SPRNG_COMMON_DEPEND)
	$(AR) $(ARFLAGS) $(LIBDIR)/liblcg64.a lcg64.o primes.o \
		$(SPRNG_COMMON_DEPEND)
	$(RANLIB) $(LIBDIR)/liblcg64.a

lcg64.o : $(SRCDIR)/interface.h lcg64.c  $(SRCDIR)/memory.h  primes.h  $(SRCDIR)/multiply.h $(SRCDIR)/store.h $(SRCDIR)/fwrap_.h
	$(CC) -c $(CFLAGS) $(FFXN) $(INLINEOPT) -I$(SRCDIR) lcg64.c 

primes.o : primes.h primes.c
	$(CC) -c $(CFLAGS) primes.c -I$(SRCDIR)


#---------------------------------------------------------------------------
# Time the generator
#---------------------------------------------------------------------------
timesprng :
	(cd ..; $(MAKE) timesprng_exec LIB_DIR=../lib TIMESPRNG_LIB=lcg64)


#---------------------------------------------------------------------------
# Check portability of generator
#---------------------------------------------------------------------------
checksprng :
	(cd ..; $(MAKE) checksprng_exec LIB_DIR=../lib CHECKSPRNG_LIB=lcg64)
	cp lcg64.data $(CHKDIR)/.


#---------------------------------------------------------------------------
.F.f : 
	@if [ -f $*.i ] ; then \
	rm $*.i ;\
	fi
	$(CPP) $*.F
	@if [ -f $*.i ] ; then \
	mv $*.i  $*.f ;\
	fi

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
clean :
	rm -f *.o *.i

realclean :
	rm -f *.o *.f *~ *.i core a.out

.SUFFIXES : 

.SUFFIXES : .f .F
