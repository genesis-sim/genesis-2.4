#
# GENESIS/XODUS : network simulator
#

#
# these four variables may have to be modified depending on the
# installation of the basic system files
#


CFLAGS	=	$(CFLAGS_IN) -DVERSION2 $(XINCLUDE) -D$(MACHINE)

LIBEXT	=	o
LIBBUILD =	$(LD)
LIBBUILDFLAGS =	$(LDFLAGS) -r -o
LIBORDER =	echo no need to order

XOLIB   = \
	./Xo/libXo.a \
	./Widg/libWidg.a \
	./Draw/libDraw.a
DISPLIB =	$(INSTALLDIR)/src/Xodus

XODUS_DIR	= \
		./_xo \
		./_widg \
		./_draw

WIDGET_DIR	= \
		./Xo \
		./Widg \
		./Draw


###################################################################
# nothing beyond this point should have to be modified by the user
###################################################################

LIBS 	= 	-ll -lm 

#==========================================================================
default: displibs xolibs

xolibs:
	@(echo making the Xodus widget library; for i in $(WIDGET_DIR); do echo cd $$i;  cd $$i; make MACHINE=$(MACHINE) INSTALLDIR=$(INSTALLDIR)  CC="$(CC)" AR="$(AR)" RANLIB="$(RANLIB)" CPP="$(CPP)" TOPFLAGS="$(CFLAGS)"; cd ..;done)

displibs:
	@(for i in $(XODUS_DIR); do echo cd $$i; cd $$i; make MACHINE=$(MACHINE) INSTALLDIR=$(INSTALLDIR)  CC="$(CC)" LIBEXT="$(LIBEXT)" LIBBUILD="$(LIBBUILD)" LIBBUILDFLAGS="$(LIBBUILDFLAGS)" LIBORDER="$(LIBORDER)" TMPDIR="$(TMPDIR)" CPP="$(CPP)" TOPFLAGS="$(CFLAGS)"; cd ..;done)

clean:
	@(for i in $(XODUS_DIR) $(WIDGET_DIR) ; do echo cd $$i; cd $$i; make clean; cd ..;done)

install:
	@(for i in $(XODUS_DIR) $(WIDGET_DIR) ; do echo cd $$i; cd $$i; make MACHINE=$(MACHINE) INSTALLDIR=$(INSTALLDIR) LIBORDER="$(LIBORDER)" LIBEXT="$(LIBEXT)" RANLIB="$(RANLIB)" install; cd ..;done)
