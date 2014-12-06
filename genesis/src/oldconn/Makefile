# $Id: Makefile,v 1.3 2005/07/29 15:52:46 svitak Exp $
#
# GENESIS/XODUS : network simulator
#

#
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:46  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:50  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.6  2000/07/03 18:06:27  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.5  1996/05/23 23:02:05  dhb
# t3d/e port
#
# Revision 1.4  1995/07/21  20:50:51  venkat
# Changed the name of SUBDIR macro to SUBDIRS. With certain Linux configurations
# the make was setting the enviroment from the top level make file definition of
# SUBDIR and this was causing loopbacks and out of virtual memory errors
#
# Revision 1.3  1995/03/28  21:44:32  dhb
# Removed extraneous message "making connection compatability library"
#
# Revision 1.2  1995/01/05  01:43:01  dhb
# Fixed redundant (but not benign) quoting of CFLAGS values.
#
# Revision 1.1  1994/12/12  17:44:12  dhb
# Initial revision
#
#

#
# these four variables may have to be modified depending on the
# installation of the basic system files
#


LIBEXT	=	o
LIBBUILD =	$(LD)
LIBBUILDFLAGS =	$(LDFLAGS) -r -o
LIBORDER =	echo no need to order

SUBDIRS	= \
		./axon \
		./synapse \
		./personal \
		./sim \
		./tools


###################################################################
# nothing beyond this point should have to be modified by the user
###################################################################
#==========================================================================
default: connlibs

connlibs:
	@(for i in $(SUBDIRS); do echo cd $$i;  cd $$i; make MACHINE=$(MACHINE) INSTALLDIR=$(INSTALLDIR)  CC="$(CC)" LIBBUILD="$(LIBBUILD)" LIBBUILDFLAGS="$(LIBBUILDFLAGS)" LIBEXT="$(LIBEXT)" LIBORDER="$(LIBORDER)" RANLIB="$(RANLIB)" CPP="$(CPP)" TOPFLAGS="$(CFLAGS_IN)"; cd ..;done)

clean:
	@(for i in $(SUBDIRS) ; do echo cd $$i; cd $$i; make clean; cd ..;done)

install:
	@(for i in $(SUBDIRS) ; do echo cd $$i; cd $$i; make MACHINE=$(MACHINE) INSTALLDIR=$(INSTALLDIR) LIBORDER="$(LIBORDER)" LIBEXT="$(LIBEXT)" install; cd ..;done)
