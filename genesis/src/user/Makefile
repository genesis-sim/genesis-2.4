# $Id: Makefile,v 1.3 2005/07/29 15:52:48 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:48  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:52  svitak
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
# Revision 1.19  2001/04/25 17:17:04  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.18  2000/07/13 18:59:25  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.17  2000/07/03 18:06:32  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.16  1999/11/29 07:50:23  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.15  1999/10/17 23:56:19  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.14  1996/06/18 07:01:27  dhb
# t3d/e port.
#
# Revision 1.13  1995/11/02  01:42:38  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.12  1995/11/01  22:34:05  venkat
# Removed-install-target-dependencies
#
# Revision 1.11  1995/02/13  23:04:05  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.10  1994/12/19  22:39:26  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.9  1994/03/21  05:54:20  dhb
# make code_g run first
#
# Revision 1.8  1994/03/20  06:37:52  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  05:45:21  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  21:06:57  dhb
# Changed CPP macro to use CPP from the top level Makefile.
#
# Revision 1.5  1993/07/17  00:38:21  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  16:19:49  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:35:20  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  20:00:44  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:06:26  dhb
# Initial revision
#

#
# Makefile for userlib.o
#
# It is recommended that each library have its own makefile and
# its own subdirectory for clarity of organization.
#

LIBS 		=  -lm
CFLAGS 		= $(CFLAGS_IN) -D$(MACHINE)
SIM			= ../sim
SS			= ../ss
SYS			= ../sys
INTERP		= ../shell
INSTALL_LIB		= $(INSTALLDIR)/lib
INSTALL_INCLUDE	= $(INSTALLDIR)/include
INSTALL_STARTUP	= $(INSTALLDIR)/startup
SIMINCLUDE 	= 	-I. -I$(SYS) -I$(SS) -I$(SIM) -I$(INTERP)
LIBRARY_NAME 	= user
FUNCTIONS 	= userfuncs
STARTUP		= userlib.g
STRUCTURES 	= user_struct.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= userlib.$(LIBEXT)
EXT_HEADER	= user_ext.h

default : $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	pulsegen.o \
		PID.o


HEADERS = 	user_struct.h

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
