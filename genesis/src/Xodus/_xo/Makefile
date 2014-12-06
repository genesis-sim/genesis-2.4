# $Id: Makefile,v 1.3 2005/07/29 15:53:07 svitak Exp $ #
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:53:07  svitak
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
# Revision 1.1  2005/06/17 21:23:52  svitak
# This file was relocated from a directory with the same name minus the
# leading underscore. This was done to allow comiling on case-insensitive
# file systems. Makefile was changed to reflect the relocations.
#
# Revision 1.1.1.1  2005/06/14 04:38:33  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.16  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.15  2000/07/03 18:19:03  mhucka
# Removed RCS directives.
#
# Revision 1.14  2000/05/29 02:12:11  mhucka
# 1) SYS setting caused problems for make install, so used a relative path
#    instead of basing the path on the value of $INSTALL.
# 2) Cleaned up the surrounding block of definitions for better
#    readability.
#
# Revision 1.13  1999/11/29 07:50:09  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.12  1998/01/19 03:07:11  venkat
# Hardcoded inclusion of diskio interface headers avoided by adding it
# in the -I option for CC.
#
# Revision 1.11  1996/06/18 06:50:15  dhb
# Removed redundant CC and CPP defined which caused recursive
# macro definition errors on some systems.
#
# Revision 1.10  1996/05/23  23:06:23  dhb
# t3d/e port
#
# Revision 1.9  1996/05/17  19:44:09  venkat
# Removed the includes from the installation directory for the INCLUDE macro.
#
# Revision 1.8  1995/11/02  01:43:06  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.7  1995/02/13  21:13:21  dhb
# Removed checkout of RCSRELEASE versions.
#
# Revision 1.6  1995/02/13  21:01:23  dhb
# Removed dependencies on sys/code_... programs.
#
# Added @.h files to clean target.
#
# Revision 1.5  1994/12/19  22:25:43  dhb
# Added rules for RCS subdirectoy.
#
# Revision 1.4  1994/12/06  02:09:49  dhb
# Added install target.
#
# Revision 1.3  1994/12/06  00:37:16  dhb
# Nov 8 1994 changes from Upi Bhalla
#
# Revision 1.2.1.4  1994/04/11  21:42:10  dhb
# Proper include directories not present in RCS checkout actions
#
# Revision 1.2.1.3  1994/04/11  21:17:00  dhb
# Needed tabs on RCS checkout actions
#
# Revision 1.2.1.2  1994/04/11  21:11:38  dhb
# Added RCS checkout rules
#
# Revision 1.2.1.1  1994/04/11  17:49:03  dhb
# Update by Dave Bilitch
# 	code_g related changes
# #
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
# GENESIS 
#	set this to the directory in which the simulator was installed
# LIBRARY_NAME
#	set this to the desired name of the library 
# FUNCTIONS
#	set this to the name of the file containing the list of
#	public function names in the library
# STARTUP
#	set this to the name of the startup script which defines the
#	commands and objects for this library
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

LIBRARY_NAME 	= 	xo
FUNCTIONS 	= 	xofuncs
STARTUP		=	xolib.g
STRUCTURES 	=	xo_struct.h
EXT_HEADER	= 	xo_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	-r -o
LIBORDER	=	echo no need to reorder
LIBEXT		=	o
TARGET_OBJ	=	xolib.$(LIBEXT)

OBJECTS = \
	xo_cmds.o \
	xo_cvt.o \
	xo_callb.o \
	xo_hash.o

# ============================================
# everything below here should maintain itself
# ============================================

# If you want to use the debug option (which will cost you in both
# speed and memory), use the alternate CFLAGS = -g.
# Otherwise use "-O" to optimize.

CFLAGS		= $(TOPFLAGS) -D$(MACHINE)
LIBS 		= -lm
SYS		= ../../sys
INCLUDE		= -I. -I.. -I../../sim -I../../ss -I../../shell -I../../sys -I../../diskio/interface -I../../diskio/interface/netcdf -I../../diskio/interface/FMT1
HEADERS 	= $(STRUCTURES)

default : $(TARGET_OBJ)

$(OBJECTS) : $(HEADERS) 

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $*.c
	rm -f $*.c

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -c 

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	$(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME) -noobjects -cstartup xinit

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES)
	- $(CPP) $(INCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(SYS)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function

$(LIBRARY_NAME)_l@.c: $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(OBJECTS)
	- $(SYS)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c

SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

clean:
	-rm -f *.a *.o; rm -f *@.[ch]

install:
	cp $(TARGET_OBJ) $(INSTALLDIR)/lib
	$(LIBORDER) $(INSTALLDIR)/lib/$(TARGET_OBJ)
	cp *.h $(INSTALLDIR)/include
