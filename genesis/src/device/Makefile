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
# Revision 1.1.1.1  2005/06/14 04:38:28  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.22  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.21  2000/07/13 18:55:32  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.20  2000/07/03 18:06:25  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.19  1999/11/29 07:50:10  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.18  1999/10/17 23:56:16  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.17  1998/08/28 15:27:50  dhb
# Removed sumin.o from OBJECTS.  Use new calculator object.
#
# Revision 1.16  1998/06/30 22:07:45  dhb
# EDS 22d merge
#
# Revision 1.15  1998/04/01 00:04:06  dhb
# Incorporated spike analysis library by Dieter Jaeger
#
# Revision 1.14.1.1  1998/06/30 22:04:57  dhb
# EDS 22d merge
#
# EDS22d revison: EDS BBF-UIA 98/06/16-98/06/16
#  Added calculator
#
# Revision 1.14  1996/05/23 22:58:33  dhb
# T3D/E port
#
# Revision 1.13  1995/11/02  01:42:11  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.12  1995/11/01  22:33:32  venkat
# Removed-install-target-dependencies
#
# Revision 1.11  1995/02/13  21:38:46  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.10  1994/12/19  22:28:14  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.9  1994/03/21  05:33:18  dhb
# make code_g run first
#
# Revision 1.8  1994/03/20  06:32:22  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:16:17  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:30:09  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:18:42  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:37:30  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:19:34  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:54:05  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:02:55  dhb
# Initial revision
#

#
# Makefile for devlib.o
#

CFLAGS 		= $(CFLAGS_IN) -D$(MACHINE)
SIM 		= ../sim
SS 		= ../ss
SYS 		= ../sys
INTERP 		= ../shell
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SIMINCLUDE 	= -I. -I$(SYS) -I$(SS) -I$(SIM) -I$(INTERP)

LIBRARY_NAME 	= devices
FUNCTIONS 	= devfuncs
STARTUP		= devlib.g
STRUCTURES 	= dev_struct.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= devlib.$(LIBEXT)
EXT_HEADER	= dev_ext.h

default : $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS = 	\
		acxc.o \
		diffamp.o \
		efield.o \
		event_tofile.o \
		frequency.o \
		new_frequency.o \
		funcgen.o \
		nernst.o \
		interspike.o \
		peristim.o \
		new_peristim.o \
		thresh.o  \
		timetable.o \
		calculator.o

HEADERS = 	\
		dev_struct.h

$(OBJECTS) : $(HEADERS) 

# ============================================
# everything below here should maintain itself
# ============================================

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

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

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS)

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)
