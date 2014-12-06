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
# Revision 1.20  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.19  2000/07/13 18:55:16  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.18  2000/07/03 18:06:24  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.17  1999/11/29 07:50:09  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.16  1999/10/17 23:56:09  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.15  1996/05/23 22:57:50  dhb
# T3D/E port
#
# Revision 1.14  1995/11/02  01:42:05  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.13  1995/11/01  22:33:26  venkat
# Removed-install-target-dependencies
#
# Revision 1.12  1995/02/13  21:08:51  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h to files removed by clean.
#
# Revision 1.11  1994/12/19  22:26:37  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.10  1994/09/20  18:33:16  dhb
# Added pc_input.o to objects list (moved from segment library).
#
# Revision 1.9  1994/03/21  05:32:30  dhb
# make code_g run first
#
# Revision 1.8  1994/03/20  06:31:27  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:20:45  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:28:37  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:16:51  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:33:38  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:13:12  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:53:03  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:02:37  dhb
# Initial revision
#

#
# Makefile for userlib.o
#

LIBS 		=  	-lm
CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
SIM 		= 	../sim
SS 		= 	../ss
SYS 		= 	../sys
INTERP 		= 	../shell
SIMINCLUDE 	= 	-I. -I$(SYS) -I$(SS) -I$(SIM) -I$(INTERP)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBRARY_NAME 	= 	buffer
FUNCTIONS 	= 	buffuncs
STARTUP		=	buflib.g
STRUCTURES 	= 	buf_struct.h
EXT_HEADER	=	buf_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	=	buflib.$(LIBEXT)

default : $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS = 	\
		graded.o \
		linear.o \
		pc_input.o \
		periodic.o \
		playback.o \
		randombuf.o \
		sigmoid.o \
		spike.o \
		statebuf.o 

HEADERS = 	\
		buf.h 		\
		buf_defs.h	\
		buf_struct.h	\
		buf_ext.h	

$(OBJECTS) : $(HEADERS) 

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
	cp $(TARGET_OBJ) $(INSTALL_LIB)
	$(LIBORDER) $(INSTALL_LIB)/$(TARGET_OBJ)
	cp *.h $(INSTALL_INCLUDE)

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS)

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)
