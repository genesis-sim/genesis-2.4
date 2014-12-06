# $Id: Makefile,v 1.3 2005/07/29 15:52:45 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:45  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:48  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.1.1.1  2005/06/14 04:38:29  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.15  1999/11/29 07:50:10  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.14  1999/10/16 21:23:47  mhucka
# Venkat had a version of the diskio code in his home directory that he
# appears to have been working on, and that did not make it into the
# source tree that I had originally used as the starting point of my
# GENESIS 2.2.2 DR reorganization effort.  In order to merge Venkat's
# changes into the distribution, I'm reverting to his version, then will
# update his version to incorporate recent changes by Upi for netCDF 3.4.
#
# Revision 1.12  1998/01/15 02:15:48  venkat
# Explicit inclusion of the interface and file format subdirs in the -I
# option to CC for comipiling the object files
#
# Revision 1.11  1998/01/14 23:33:41  venkat
# Passed the DISKIOSUBDIR_IN macro in the install target command line
# down to the lowerlevel directory.
#
# Revision 1.10  1998/01/14  00:23:47  venkat
# Changes to accomodate the change in directory structure where the
# abstract interface is seperated from the individual file format
# interfaces and libraries.
#
# Revision 1.9  1997/08/12  19:27:35  dhb
# Removed unset NCDFDIR from the call to the interface makefile.  Was
# returning non-zero status causing AIX make to abort.
#
# Revision 1.8  1997/08/08 00:16:19  dhb
# Added define for MACHINE to CFLAGS
#
# Revision 1.7  1997/08/03 19:43:44  dhb
# Commented potentially recursive macros
#
# Revision 1.6  1997/08/01 21:35:23  dhb
# Fixed comment leader to use Makefiel comment char
#
# Revision 1.5  1997/07/29 18:26:20  venkat
# Added RCS Log and Id headers

#####################################################################
# The macros passed down from the machine-specific top-level Makefile
#####################################################################

# Since almost all of these macros are set directly on the make
# command line, we shouldn't need to set them here and should not
# as a macro like this can result in recursive macro errors with
# some makes


CFLAGS		=	$(CFLAGS_IN)
LIBRARY_NAME 	= 	diskio
STARTUP		=	diskiolib.g
STRUCTURES 	= 	diskio_struct.h
EXT_HEADER	= 	diskio_ext.h
TARGET_OBJ	=	diskiolib.o

OBJECTS = gen_diskio.o gen_variable.o gen_metadata.o diskio_utilfunc.o copyrights.o
INTERFACE_OBJECTS = interface/fflib.o


# ============================================
# everything below here should maintain itself
# ============================================

SHELL		=	/bin/sh

# GENINST is passed in from the Usermake makefile.  If you're not
# using Usermake, either edit your top level makefile to pass it
# in or hard code the GENESIS 2.0 installation directory here.

GENESIS_INCLUDE	= 	-I. -I../sim -I../sys -I../ss -I../shell 
HEADERS 	=  	$(STRUCTURES)
INCDIRS		=  	-I. -I..	

default : $(TARGET_OBJ)

$(OBJECTS) : $(HEADERS) 


$(INTERFACE_OBJECTS) : interfacelib

interfacelib:
	@(for subdir in interface; do echo cd $$subdir; cd $$subdir; make DISKIOSUBDIR="$(DISKIOSUBDIR)" CC="$(CC)" CFLAGS_IN="$(CFLAGS)" CPP="$(CPP)" LD="$(LD)" LDFLAGS="$(LDFLAGS)"; cd ..;done)

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- ../sys/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME) \
	-cstartup copyright_fileformats

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES) ../sys/code_sym
	- $(CPP) $(GENESIS_INCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- ../sys/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS) ../lib/code_func
	- ../sys/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function

$(LIBRARY_NAME)_l@.c: $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(FSYMBOLTAB_C) ../sys/code_lib $(OBJECTS)
	- ../sys/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c

SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(FSYMBOLTAB) $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(OBJECTS) $(INTERFACE_OBJECTS) $(SYMBOLTAB) 
	$(LD) $(LDFLAGS) -r -o $(TARGET_OBJ) $(OBJECTS) $(INTERFACE_OBJECTS) $(SYMBOLTAB)

clean:
	@(for i in interface; do echo cd $$i; cd $$i; make DISKIOSUBDIR="$(DISKIOSUBDIR)" clean; cd ..;done)
	-rm -f *.o *@.[ch]

install:
	@(for subdir in interface; do echo cd $$subdir; cd $$subdir; make INSTALLDIR="$(INSTALLDIR)" DISKIOSUBDIR="$(DISKIOSUBDIR)" RANLIB="$(RANLIB)" install; cd ..;done)
	-cp diskiolib.o $(INSTALLDIR)/lib
	-cp *.h $(INSTALLDIR)/include



# =========================================

#	Suffix rules

# =========================================

.c.o:
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) -Iinterface -Iinterface/netcdf -Iinterface/FMT1 $< -c
