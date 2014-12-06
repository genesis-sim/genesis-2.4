############################################################################
#
# Typing the command below   => results in the following being created
#      make               => Fortran and C libraries for the generator,
#                            Timing executibles.
#      make mlfg	  => Library for the Multiplicative LFG
#
# Object files created during the compilation process can be deleted finally
# by typing
#       make clean
############################################################################

LIBDIR = $(LIB_DIR)
CFLAGS = $(CFLAGS_IN)
SRCDIR = ..
CHKDIR = ../..

all : mlfg timesprng checksprng

#---------------------------------------------------------------------------
# Construct Library
#---------------------------------------------------------------------------
SPRNG_COMMON_DEPEND = $(SRCDIR)/fwrap_mpi.o  $(SRCDIR)/cputime.o \
		      $(SRCDIR)/makeseed.o   $(SRCDIR)/simple_mpi.o \
		      $(SRCDIR)/memory.o  $(SRCDIR)/communicate.o  $(SRCDIR)/checkid.o $(SRCDIR)/store.o

mlfg :
	(cd ..; $(MAKE) sprng_common)
	$(MAKE) $(LIBDIR)/libmlfg.a

$(LIBDIR)/libmlfg.a: mlfg.o  $(SPRNG_COMMON_DEPEND)
	$(AR) $(ARFLAGS) $(LIBDIR)/libmlfg.a mlfg.o $(SPRNG_COMMON_DEPEND)
	$(RANLIB) $(LIBDIR)/libmlfg.a

mlfg.o : $(SRCDIR)/interface.h mlfg.c int64.h  $(SRCDIR)/memory.h $(SRCDIR)/store.h $(SRCDIR)/fwrap_.h
	$(CC) -c $(CFLAGS)  $(FFXN) $(INLINEOPT) mlfg.c -I$(SRCDIR)


#---------------------------------------------------------------------------
# Time the generator
#---------------------------------------------------------------------------
timesprng :
	(cd ..; $(MAKE) timesprng_exec TIMESPRNG_LIB=mlfg)


#---------------------------------------------------------------------------
# Check portability of generator
#---------------------------------------------------------------------------
checksprng :
	(cd ..; $(MAKE) checksprng_exec CHECKSPRNG_LIB=mlfg)
	cp mlfg.data $(CHKDIR)/.


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
