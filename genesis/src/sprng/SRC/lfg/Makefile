############################################################################
#
# Typing the command below   => results in the following being created
#      make               => Fortran and C libraries for the generator,
#                            Timing executibles.
#      make lfg		  => Library for the Lagged Fibonacci Generator
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
############################################################################

LIBDIR = $(LIB_DIR)
CFLAGS = $(CFLAGS_IN)
SRCDIR = ..
CHKDIR = ../..

all : lfg timesprng checksprng

#---------------------------------------------------------------------------
# Construct Library
#---------------------------------------------------------------------------
SPRNG_COMMON_DEPEND = $(SRCDIR)/fwrap_mpi.o  $(SRCDIR)/cputime.o \
		      $(SRCDIR)/makeseed.o   $(SRCDIR)/simple_mpi.o \
		      $(SRCDIR)/memory.o  $(SRCDIR)/communicate.o  $(SRCDIR)/checkid.o $(SRCDIR)/store.o

lfg :
	(cd ..; $(MAKE) sprng_common)
	$(MAKE) CC="$(CC)" CFLAGS_IN="$(CFLAGS)" SPRNG_LIB="$(SPRNG_LIB)" LIB_DIR='$$(LIB)' PLAT=$(PLAT) $(LIBDIR)/liblfg.a

$(LIBDIR)/liblfg.a: lfg.o  $(SPRNG_COMMON_DEPEND)
	$(AR) $(ARFLAGS) $(LIBDIR)/liblfg.a lfg.o $(SPRNG_COMMON_DEPEND)
	$(RANLIB) $(LIBDIR)/liblfg.a

lfg.o : $(SRCDIR)/interface.h lfg.c  $(SRCDIR)/multiply.h  $(SRCDIR)/memory.h $(SRCDIR)/store.h $(SRCDIR)/fwrap_.h
	$(CC) -c $(CFLAGS)  $(FFXN) $(INLINEOPT) lfg.c -I$(SRCDIR)


#---------------------------------------------------------------------------
# Time the generator
#---------------------------------------------------------------------------
timesprng :
	(cd ..; $(MAKE) timesprng_exec TIMESPRNG_LIB=lfg)


#---------------------------------------------------------------------------
# Check portability of generator
#---------------------------------------------------------------------------
checksprng :
	(cd ..; $(MAKE) checksprng_exec CHECKSPRNG_LIB=lfg)
	cp lfg.data $(CHKDIR)/.


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
