# $Id: Makefile,v 1.5 2005/08/24 03:10:19 ghood Exp $
# ============================================
# Modified to live in the GENESIS distribution directory heirarchy
# and to remove some modules from compilation (see below).  ---dhb
#
# $Log: Makefile,v $
# Revision 1.5  2005/08/24 03:10:19  ghood
# Removed the -I prefix to XINCLUDE
#
# Revision 1.4  2005/07/29 15:52:46  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.3  2005/07/20 19:07:49  svitak
# Changed $(CPP) lines to redirect output. Some architectures did not have
# the form of $(CPP) where the output is specified as an argument.
#
# Removed redundant 'CPP = $(CPP)' line.
#
# Made sure $(TMPDIR) was used instead of /tmp.
#
# Revision 1.2  2005/06/20 21:08:30  svitak
# Changed sprng() calls to G_RNG(). This is so the kinetics code can still
# be compiled even if sprng cannot.
#
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.12  2003/05/29 22:26:40  gen-dbeeman
# New version of kinetics library from Upi Bhalla, April 2003
#
# Revision 1.11  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.10  2001/04/18 22:39:37  mhucka
# Miscellaneous portability fixes, mainly for SGI IRIX.
#
# Revision 1.9  2000/07/03 18:06:26  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.8  1999/12/31 08:27:29  mhucka
# Added CFLAGS definition.
#
# Revision 1.7  1999/11/29 07:50:11  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.6  1999/10/17 23:56:17  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.5  1998/07/15 06:45:33  dhb
# Upi update
#
# Revision 1.4  1997/08/04 19:13:01  dhb
# Added LIBBUILD et al to support cross compile environments
# Added INSTALL_... macros to fix install problem
#
# Revision 1.3  1997/07/31 22:44:20  dhb
# Added install target
#
# Revision 1.2  1997/07/24 18:26:26  dhb
# Library modified to live in the GENESIS distribution source heirarchy
# and to exclude the 2D interpolation and table modules which have been
# integrated into the distribution elsewhere.
#
# Also excluded ksolve object as it no longer works (per the author).
#
# ============================================
#
# Makefile for creating a new user library
#
# It is recommended that each library have its own makefile and
# its own subdirectory for clarity of organization.
# This subdirectory should contain all of the object files
# needed to compile the library as well as all header files,
# support files, and the script file for loading the library (e.g. userlib.g).
#
# The user should specify the values of the following makefile variables :
#
# LIBRARY_NAME
#	set this to the desired name of the library 
# FUNCTIONS
#	set this to the name of the file containing the list of
#	public function names in the library
#	NOTE: The FUNCTIONS file is not necessary in GENESIS 2.0
#             as public function names are added automatically when
#             referenced in the library startup script.  Functions
#             not referenced by an addfunc or object startup command
#             may be declared as public functions with the hashfunc
#             startup command.
#
#             If you are converting a 1.4 library to 2.0 you may use
#             the FUNCTIONS file by following the instructions below.
# STRUCTURES
#	set this to the name of the file containing the structure definitions
#	for objects in the library
# EXT_HEADER
#	set this to the name of the header file containing the appropriate
#	include references needed to compile the library files
# TARGET_OBJ
#	set this to the name of the library object which will be compiled
#	This is normally just the LIBRARY_NAME with 'lib.o' appended to it
# OBJECTS
#	list the user object files (with the .o extension) which will be 
#	a part of the library
#
# ============================================

CFLAGS		= 	$(CFLAGS_IN) -D$(OS) -D$(MACHINE) $(XINCLUDE)
LIBRARY_NAME 	= 	kin
STARTUP		=	kinlib.g
STRUCTURES 	= 	kin_struct.h
EXT_HEADER	= 	kin_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	=	kinlib.$(LIBEXT)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup

#
# The folowing modules have been removed here for varying reasons.
#
# Modified versions of these now live in the olf library.  NOTE: the
# 2D table object name changed in the olf version.
#
#        interp2d.o \
#        table2d.o \
#
# This module doesn't work as supplied and has been disabled at the
# suggestion of the author.
#
#        ksolve.o

OBJECTS = \
        reac.o \
        pool.o \
        enz.o \
        concchan.o \
        msg_clone.o \
		file_info.o \
		text.o \
		gsolve.o

# If really you want to use a FUNCTIONS file you can by setting it
# here and uncommenting the FSYMBOLTAB and FSYMBOLTAB_C lines.  In
# all likelyhood you will not need the FUNCTIONS file at all.

FUNCTIONS 	= 	kinfuncs
FSYMBOLTAB	=	$(LIBRARY_NAME)_f@.o
FSYMBOLTAB_C	=	$(LIBRARY_NAME)_f@.c

# ============================================
# everything below here should maintain itself
# ============================================

# GENINST is passed in from the Usermake makefile.  If you're not
# using Usermake, either edit your top level makefile to pass it
# in or hard code the GENESIS 2.0 installation directory here.

GENESIS		= 	..
GENESIS_LIB	= 	../sys
GENESIS_INCLUDE	= 	-I. -I../ss -I../sys -I../shell -I../sim -I../sprng/include -I../Xodus
HEADERS 	=  	$(STRUCTURES)

default : $(TARGET_OBJ)

$(OBJECTS) : $(HEADERS) 

.c.o:
	$(CC) $(CFLAGS) $(GENESIS_INCLUDE) $< -c 

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(GENESIS_LIB)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME) \
	-cstartup copyleft_kin

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES) $(GENESIS_LIB)/code_sym
	- $(CPP) $(GENESIS_INCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(GENESIS_LIB)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS) $(GENESIS_LIB)/code_func
	- $(GENESIS_LIB)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function

$(LIBRARY_NAME)_l@.c: $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(FSYMBOLTAB_C) $(GENESIS_LIB)/code_lib $(OBJECTS)
	- $(GENESIS_LIB)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c

SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(FSYMBOLTAB) $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

clean:
	-rm -f *.o; rm -rf *@.[ch]
