# 
# File name   : Makefile
# Description : Makefile for SPRNG library included with GENESIS
# Author(s)   : Michael Hucka <mhucka@bbb.caltech.edu>
# Organization: Bower Lab, Division of Biology, Caltech
# Language    : make
# Created     : 1999-12-02
# Revsion     : $Id: Makefile,v 1.5 2005/10/04 22:05:13 svitak Exp $
# $Source: /cvsroot/genesis-sim/genesis2/genesis/src/sprng/Makefile,v $
# 
# SEE THE END OF THIS FILE FOR A REVISION HISTORY LOG.
#
# This file is not the same as the Makefile that is supplied with the SPRNG
# package.  It has been heavily modified to adapt it for GENESIS.

LIBS 		=  	-lm
CFLAGS		= 	$(RNG_CFLAGS)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	$(RANLIB)
LIBEXT		=	o
LIB_DIR		= 	`pwd`/lib
DIRS 		= 	SRC EXAMPLES TESTS lib
SRCDIR 		= 	SRC

PLAT		= 	$(MACHINE)

# 
# Basic targets and rules.
# 

all: source

#---------------------------------------------------------------------------
source:
	(LIB=$(LIB_DIR); export LIB; cd SRC; $(MAKE) CC="$(CC)" LD="$(LD)" AR="$(AR)" RANLIB="$(RANLIB)" CPP="$(CPP)" YACC="$(YACC)" LEX="$(LEX)" LEXLIB="$(LEXLIB)" SPRNG_LIB="$(SPRNG_LIB)" LIB_DIR='$$(LIB)' TMPDIR="$(TMPDIR)" LDFLAGS="$(LDFLAGS)" OS="$(OS)" PLAT="$(PLAT)" INSTALLDIR="$(INSTALLDIR)" CFLAGS_IN="$(CFLAGS)"; cd ..)

examples: 
	(LIB=$(LIB_DIR); export LIB; cd EXAMPLES; $(MAKE) CC="$(CC)" CFLAGS_IN="$(CFLAGS)" SPRNG_LIB="$(SPRNG_LIB)" LIB_DIR='$$(LIB)' PLAT=$(PLAT); cd ..)

tests: 
	(LIB=$(LIB_DIR); export LIB; cd TESTS; $(MAKE) CC="$(CC)" CFLAGS_IN="$(CFLAGS)" SPRNG_LIB="$(SPRNG_LIB)" LIB_DIR='$$(LIB)' PLAT=$(PLAT); cd ..)

#---------------------------------------------------------------------------
clean:
	@for l in $(DIRS) ; do \
	  cd $$l ; \
	  $(MAKE) PLAT=$(PLAT) LIB_DIR='$$(LIB)' SPRNG_LIB="$(SPRNG_LIB)" clean ; \
	  cd .. ; \
        done

realclean:
	@for l in $(DIRS) ; do \
	  cd $$l ; \
	  $(MAKE) PLAT=$(PLAT) LIB_DIR='$$(LIB)' SPRNG_LIB="$(SPRNG_LIB)" realclean ; \
	  cd .. ; \
        done
	@rm -f core *~ check* time* *.data

install:
	-cp lib/lib$(SPRNG_LIB).a $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/lib$(SPRNG_LIB).a
	-cp include/*.h $(INSTALL_INCLUDE)

freeze:


#
# $Log: Makefile,v $
# Revision 1.5  2005/10/04 22:05:13  svitak
# Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
# the branch modifications should be included.
#
# Revision 1.4.2.1  2005/08/12 21:03:37  svitak
# Changed $(TARGET_OBJ) to lib$(SPRNG_LIB).a. Lib was not getting ranlib'd.
#
# Revision 1.4  2005/07/29 15:52:47  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.3  2005/07/20 19:11:07  svitak
# Include of make.$(MACHINE) commented out as it is no longer used.
#
# Revision 1.2  2005/06/17 22:02:31  svitak
# Target 'src' was renamed to source to avoid confusion with the SRC directory
# (from Cygwin).
#
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.9  2001/07/22 17:50:04  mhucka
# Changes from Chris Butson for SGI Origin 64-bit.
#
# Revision 1.8  2000/07/12 08:46:43  mhucka
# This wasn't passing down the value of CC.  Fixed.
#
# Revision 1.7  2000/07/03 19:54:10  mhucka
# Changed treatment of CFLAGS so that it's properly passed to submakefiles.
#
# Revision 1.6  2000/07/03 18:35:54  mhucka
# 1) Added LDFLAGS to LIBBUILDFLAGS.
# 2) Changed treatment of CFLAGS so that it's properly passed to submakefiles.
# 3) Renamed LIBDIR to LIB_DIR.  The value was sometimes getting overridden
#    (in some versions of make) by the value passed on the command line, which
#    in turn screwed up what was passed down by this Makefile.
#
# Revision 1.5  2000/05/25 02:58:40  mhucka
# Fixed "make install" action and "make realclean".
#
# Revision 1.4  2000/05/19 20:11:37  mhucka
# Need to pass the value of SPRNG_LIB to make clean, so that the sprng
# directory is cleaned up properly.
#
# Revision 1.3  2000/04/24 08:03:33  mhucka
# Made this work with GENESIS makefiles.
#
# Revision 1.2  2000/03/27 10:35:14  mhucka
# Started making this makefile more GENESIS-compliant.
#
#
