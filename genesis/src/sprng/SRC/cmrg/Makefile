############################################################################
#
# Then typing the command below   => results in the following being created
#      make               => Fortran and C libraries for the generator,
#                            Timing executibles.
#      make cmrg	  => Library for Combined Multiple Recursive Generator
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
############################################################################

LIBDIR = $(LIB_DIR)
CFLAGS = $(CFLAGS_IN)
SRCDIR = ..
CHKDIR = ../..

all : cmrg timesprng checksprng


#---------------------------------------------------------------------------
# Construct Library
#---------------------------------------------------------------------------
SPRNG_COMMON_DEPEND = $(SRCDIR)/fwrap_mpi.o  $(SRCDIR)/cputime.o \
		      $(SRCDIR)/makeseed.o   $(SRCDIR)/simple_mpi.o \
		      $(SRCDIR)/memory.o  $(SRCDIR)/communicate.o  $(SRCDIR)/checkid.o $(SRCDIR)/store.o

cmrg :
	(cd ..; $(MAKE) sprng_common)
	$(MAKE) $(LIBDIR)/libcmrg.a

$(LIBDIR)/libcmrg.a : cmrg.o primes.o  $(SPRNG_COMMON_DEPEND)
	$(AR) $(ARFLAGS) $(LIBDIR)/libcmrg.a cmrg.o primes.o \
		$(SPRNG_COMMON_DEPEND)
	$(RANLIB) $(LIBDIR)/libcmrg.a

cmrg.o : $(SRCDIR)/interface.h cmrg.c  $(SRCDIR)/memory.h  primes.h  $(SRCDIR)/multiply.h $(SRCDIR)/store.h $(SRCDIR)/fwrap_.h
	$(CC) -c $(CFLAGS) $(FFXN) $(INLINEOPT)  cmrg.c -I$(SRCDIR)

primes.o : primes.h primes.c
	$(CC) -c $(CFLAGS) primes.c -I$(SRCDIR)


#---------------------------------------------------------------------------
# Time the generator
#---------------------------------------------------------------------------
timesprng :
	(cd ..; $(MAKE) timesprng_exec TIMESPRNG_LIB=cmrg)


#---------------------------------------------------------------------------
# Check portability of generator
#---------------------------------------------------------------------------
checksprng :
	(cd ..; $(MAKE) checksprng_exec CHECKSPRNG_LIB=cmrg)
	cp cmrg.data $(CHKDIR)/.


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
