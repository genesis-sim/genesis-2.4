# $Id: Makefile,v 1.3 2005/07/29 15:52:47 svitak Exp $
#
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:47  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:51  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.1.1.1  2005/06/14 04:38:28  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.9  2001/07/22 17:52:49  mhucka
# Missing LIBORDER caused trouble on SGIs.
#
# Revision 1.8  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.7  2001/04/18 22:39:37  mhucka
# Miscellaneous portability fixes, mainly for SGI IRIX.
#
# Revision 1.6  2000/07/03 18:27:52  mhucka
# Added CVS id header.
#
# Revision 1.5  2000/07/03 18:27:32  mhucka
# Made this use the variable LD instead of invoking `ld' directly.
#

#
# Makefile for `param' library.
# Originally from Michael Vanier <mvanier@bbb.caltech.edu>
# 

#
# Compiling options; note that these can be overridden
# if this Makefile is called from another Makefile.
#

CFLAGS          = $(CFLAGS_IN) -D$(OS) -D$(MACHINE)

INSTALL_LIB 	= $(INSTALLDIR)/lib
INSTALL_INCLUDE	= $(INSTALLDIR)/include

LIBRARY_NAME    = param
STARTUP         = paramlib.g
STRUCTURES      = param_struct.h
EXT_HEADER      = param_ext.h
TARGET_OBJ      = paramlib.o
LIBORDER	= @echo no lib ordering needed for

OBJECTS = \
          paramtableGA.o   \
          paramtableSS.o   \
          paramtableSA.o   \
          paramtableBF.o   \
          paramtableCG.o   \
          param_utils.o    \
          gen2spk.o        \
          spkcmp.o         \
          shapematch.o


#
# GENESIS_ROOT is the genesis src directory.
# GENESIS_LIB is where code_{g,sym,lib} live; here it's the sys directory.
# GENESIS_INCLUDE is where all the needed include files for this library live.
#

GENESIS_ROOT    = ..
GENESIS_LIB     = $(GENESIS_ROOT)/sys
GENESIS_INCLUDE = -I. \
                  -I$(GENESIS_ROOT)/sys   \
                  -I$(GENESIS_ROOT)/ss    \
                  -I$(GENESIS_ROOT)/sim   \
                  -I$(GENESIS_ROOT)/shell \
                  -I$(GENESIS_ROOT)/olf


HEADERS = $(STRUCTURES)

default : $(TARGET_OBJ)

$(OBJECTS) : $(HEADERS) 

.c.o:
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) $< -c 

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(GENESIS_LIB)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

$(LIBRARY_NAME)_g@.o: $(LIBRARY_NAME)_g@.c 
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) $< -c 

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES) $(GENESIS_LIB)/code_sym
	- $(CPP) $(GENESIS_INCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(GENESIS_LIB)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

$(LIBRARY_NAME)_d@.o : $(LIBRARY_NAME)_d@.c
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) $< -c 

# make the library header function

$(LIBRARY_NAME)_l@.c: $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c \
                      $(FSYMBOLTAB_C) $(GENESIS_LIB)/code_lib $(OBJECTS)
	- $(GENESIS_LIB)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c

$(LIBRARY_NAME)_l@.o: $(LIBRARY_NAME)_l@.c
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) $< -c 


SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o \
            $(FSYMBOLTAB) $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
	$(LD) $(LDFLAGS) -r -o $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

clean:
	-rm -f *.o; rm -rf *@.[ch]
