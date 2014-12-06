# $Id: Makefile,v 1.3 2005/07/29 15:52:46 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:46  svitak
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
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.21  2001/04/25 17:16:58  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.20  2000/07/13 18:55:54  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.19  2000/07/03 18:06:25  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.18  1999/12/01 08:32:45  mhucka
# Looks like it's no longer necessary to disable optimization in this
# directory.  Some of the other changes in the 2.2 work (probably
# Greg Hood's fixes for the T3E) fixed this problem too.
#
# Revision 1.17  1999/11/29 07:50:11  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.16  1999/10/17 23:56:16  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.15  1999/08/22 04:42:13  mhucka
# Various fixes, mostly for Red Hat Linux 6.0
#
# Revision 1.14  1996/05/23 22:58:49  dhb
# t3d/e port
#
# Revision 1.13  1995/11/02  01:41:59  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.12  1995/11/01  22:33:14  venkat
# Removed-install-target-dependencies
#
# Revision 1.11  1995/02/13  21:45:57  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.10  1994/12/19  22:28:48  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.9  1994/03/21  05:35:50  dhb
# make codeg run first
#
# Revision 1.8  1994/03/20  06:32:39  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:14:30  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:30:40  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:19:23  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:38:19  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:20:29  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:54:19  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:03:04  dhb
# Initial revision
#

#
# Makefile for hhlib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
SIM 		= 	../sim
SS 		= 	../ss
SYS 		= 	../sys
INTERP 		= 	../shell
SIMINCLUDE 	= 	-I. -I$(SYS) -I$(SS) -I$(SIM) -I$(INTERP)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBRARY_NAME 	= 	hh
FUNCTIONS 	= 	hhfuncs
STARTUP		=	hhlib.g
STRUCTURES 	= 	hh_struct.h
EXT_HEADER 	= 	hh_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	hhlib.$(LIBEXT)

default : $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS = 	\
		channel.o \
		evaluate.o \
		gate.o \
		rate.o \
		update.o \
		vchannel.o

HEADERS = 	\
		hh_struct.h

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
