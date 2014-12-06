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
# Revision 1.21  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.20  2000/07/03 18:06:24  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.19  1999/11/29 07:50:09  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.18  1999/10/17 23:56:15  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.17  1999/10/17 22:13:23  mhucka
# New version from Erik De Schutter, dated circa March 1999.
#
# EDS22b revison 97/11/14: EDS BBF-UIA 97/11/14
# Added hillpump object.
#
# Revision 1.15  1997/05/28 23:32:13  dhb
# Changes for Antwerp GENESIS 21e
#
# EDS21a revison 96/04/06: EDS BBF-UIA 96/04/06
# Added dif2buffer object.
#
# EDS20k revison 95/09/27: EDS BBF-UIA 95/09/27
# Added concpool object.
#
# EDS20i revison 95/06/02: EDS BBF-UIA 95/06/02
# This is a COMPLETE replacement of the C-code.
# Except for the OBJECTS and HEADERS lists, this file has not changed.
#
# Revision 1.14  1996/05/23 22:58:08  dhb
# T3D/E port
#
# Revision 1.13  1995/11/02  01:42:08  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.12  1995/11/01  22:33:29  venkat
# Removed-install-target-dependencies
#
# Revision 1.11  1995/02/13  21:25:17  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to files removed by clean.
#
# Revision 1.10  1994/12/19  22:27:06  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.9  1994/03/21  05:34:14  dhb
# make code_g run first
#
# Revision 1.8  1994/03/20  06:31:55  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:19:08  dhb
# Changes for C code Startups
#
# Revision 1.6  1993/09/17  16:29:32  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:17:38  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:35:00  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:17:03  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:53:44  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:02:45  dhb
# Initial revision
#

#
# Makefile for conclib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SYS		=	../sys
SS		=	../ss
SIMINCLUDE 	= 	-I. -I../sim -I../shell -I$(SS) -I$(SYS)
LIBRARY_NAME 	= 	concen
FUNCTIONS 	= 	concfuncs
STARTUP		=	conclib.g
STRUCTURES 	= 	conc_struct.h
EXT_HEADER	=	conc_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	conclib.$(LIBEXT)

default: $(TARGET_OBJ) 


OBJECTS =	\
        difshell.o \
        taupump.o \
        fixbuffer.o \
        difbuffer.o \
        mmpump.o \
	fura2.o \
	concpool.o \
        dif2buffer.o \
	hillpump.o

HEADERS = 	\
		conc_ext.h \
		conc_defs.h \
		conc_struct.h

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

$(OBJECTS) : $(HEADERS)

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES)
	- $(CPP) $(SIMINCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(SYS)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

# make the function list section of the symbol table

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function
$(LIBRARY_NAME)_l@.c : $(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_d@.c $(OBJECTS)
	 - $(SYS)/code_lib $(LIBRARY_NAME) -o $(LIBRARY_NAME)_l@.c



SYMBOLTAB = $(LIBRARY_NAME)_d@.o $(LIBRARY_NAME)_g@.o $(LIBRARY_NAME)_l@.o

$(TARGET_OBJ): $(SYMBOLTAB) $(OBJECTS)
	$(LIBBUILD) $(LIBBUILDFLAGS) $(TARGET_OBJ) $(OBJECTS) $(SYMBOLTAB)
	$(LIBORDER) $(TARGET_OBJ)

install:
	-cp $(TARGET_OBJ) $(INSTALL_LIB)
	-$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	-cp *.h $(INSTALL_INCLUDE)

clean:
	-rm -f  *.a *.o *@.[ch]

