############################################################################
#
# Typing the command below   => results in the following being created
#      make               => Fortran and C libraries for the generator,
#                            Timing executibles.
#      make lcg		  => Library for the Linear Congruential Generator
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
############################################################################

LIBDIR = $(LIB_DIR)
CFLAGS = $(CFLAGS_IN)
SRCDIR = ..
CHKDIR = ../..

all : lcg timesprng checksprng


#---------------------------------------------------------------------------
# Construct Library
#---------------------------------------------------------------------------
SPRNG_COMMON_DEPEND = $(SRCDIR)/fwrap_mpi.o  $(SRCDIR)/cputime.o \
		      $(SRCDIR)/makeseed.o  $(SRCDIR)/simple_mpi.o \
		      $(SRCDIR)/memory.o  $(SRCDIR)/communicate.o  $(SRCDIR)/checkid.o $(SRCDIR)/store.o

lcg :
	(cd ..; $(MAKE) CHECKSPRNG_LIB=lcg sprng_common)
	$(MAKE) LIBDIR="$(LIBDIR)" $(LIBDIR)/liblcg.a

$(LIBDIR)/liblcg.a : lcg.o primes.o  $(SPRNG_COMMON_DEPEND)
	$(AR) $(ARFLAGS) $(LIBDIR)/liblcg.a lcg.o primes.o \
		$(SPRNG_COMMON_DEPEND)
	$(RANLIB) $(LIBDIR)/liblcg.a

lcg.o : $(SRCDIR)/interface.h lcg.c  $(SRCDIR)/memory.h  primes.h  $(SRCDIR)/multiply.h $(SRCDIR)/store.h $(SRCDIR)/fwrap_.h
	$(CC) -c $(CFLAGS)  $(FFXN) $(INLINEOPT) lcg.c -I$(SRCDIR)

primes.o : primes.h primes.c
	$(CC) -c $(CFLAGS) primes.c -I$(SRCDIR)


#---------------------------------------------------------------------------
# Time the generator
#---------------------------------------------------------------------------
timesprng :
	(cd ..; $(MAKE) timesprng_exec TIMESPRNG_LIB=lcg)


#---------------------------------------------------------------------------
# Check portability of generator
#---------------------------------------------------------------------------
checksprng :
	(cd ..; $(MAKE) checksprng_exec CHECKSPRNG_LIB=lcg)
	cp lcg.data $(CHKDIR)/.


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
