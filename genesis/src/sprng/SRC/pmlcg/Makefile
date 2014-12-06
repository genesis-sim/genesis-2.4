############################################################################
#
# Then typing the command below   => results in the following being created
#      make               => Fortran and C libraries for the generator,
#                            Timing executibles.
#      make pmlcg	  => Library for the Prime Modulus LCG
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
############################################################################

LIBDIR = $(LIB_DIR)
CFLAGS = $(CFLAGS_IN)
SRCDIR = ..
CHKDIR = ../..

all : pmlcg timesprng checksprng


#---------------------------------------------------------------------------
# Construct Library
#---------------------------------------------------------------------------
SPRNG_COMMON_DEPEND = $(SRCDIR)/fwrap_mpi.o  $(SRCDIR)/cputime.o \
		      $(SRCDIR)/makeseed.o   $(SRCDIR)/simple_mpi.o \
		      $(SRCDIR)/memory.o  $(SRCDIR)/communicate.o  $(SRCDIR)/checkid.o $(SRCDIR)/store.o

pmlcg :
	(cd ..; $(MAKE) sprng_common)
	$(MAKE) $(LIBDIR)/libpmlcg.a

$(LIBDIR)/libpmlcg.a : pmlcg.o   $(SPRNG_COMMON_DEPEND)
	$(AR) $(ARFLAGS) $(LIBDIR)/libpmlcg.a pmlcg.o  \
		$(SPRNG_COMMON_DEPEND)
	$(RANLIB) $(LIBDIR)/libpmlcg.a

pmlcg.o : $(SRCDIR)/interface.h pmlcg.c  $(SRCDIR)/memory.h  basic.h info.h gmp.h longlong.h $(SRCDIR)/store.h $(SRCDIR)/fwrap_.h
	$(CC) -c $(CFLAGS)  $(FFXN) $(INLINEOPT) pmlcg.c -I$(SRCDIR)


#---------------------------------------------------------------------------
# Time the generator
#---------------------------------------------------------------------------
timesprng :
	(cd ..; $(MAKE) timesprng_exec TIMESPRNG_LIB=pmlcg GMPLIB=-lgmp)


#---------------------------------------------------------------------------
# Check portability of generator
#---------------------------------------------------------------------------
checksprng :
	(cd ..; $(MAKE) checksprng_exec CHECKSPRNG_LIB=pmlcg GMPLIB=-lgmp)
	cp pmlcg.data $(CHKDIR)/.


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
