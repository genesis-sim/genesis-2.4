# $Id: Makefile,v 1.3 2005/07/29 15:52:46 svitak Exp $
#
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:52:46  svitak
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
# Revision 1.1.1.1  2005/06/14 04:38:34  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.22  2000/07/13 18:57:41  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.21  2000/07/03 18:06:29  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.20  1999/11/29 07:50:13  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.19  1999/10/17 23:56:17  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.18  1999/10/17 22:32:04  mhucka
# Added changes from Erik De Schuter dated circa March 1999.
# The definitions of TabChannel_SAVE, TabChannel_READ, TabChannel_ptrs have
# been moved to tabfiles.c.
#
# Revision 1.17  1998/06/30 21:18:22  dhb
# Added ../hh to include dirs
# Added hh_tabchan.o to objects
#
# Revision 1.16  1997/05/29 07:53:06  dhb
# Update from Antwerp GENESIS version 21e
#
# EDS21e revison: EDS BBF-UIA 97/05/03
# Added ss to include directories
#
# EDS20i revison: EDS BBF-UIA 95/06/01
# Added olf_defs.h and interp2d.o tab2channel.o table2d.o tabcurrent.o
#
# Revision 1.15  1996/05/23 22:59:38  dhb
# t3d/e port
#
# Revision 1.14  1995/11/02  01:42:17  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.13  1995/11/01  22:33:38  venkat
# Removed-install-target-dependencies
#
# Revision 1.12  1995/02/13  22:44:09  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies for sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.11  1994/12/19  22:35:37  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.10  1994/03/21  05:37:24  dhb
# make code_g run first
#
# Revision 1.9  1994/03/20  06:33:38  dhb
# *** empty log message ***
#
# Revision 1.8  1994/03/20  06:09:09  dhb
# Fixes to C code startup changes
#
# Revision 1.7  1994/03/20  05:33:19  dhb
# Changes to support startup in C code.  Now uses code_g program
# to generate C code for startup commands from the .g file.  The
# functions file is now obsolete.
#
# Revision 1.6  1993/09/17  16:31:40  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:21:02  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:40:26  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:21:36  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:55:52  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/11/14  01:28:10  dhb
# Initial revision
#

#
# Makefile for olflib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SYS		=	../sys
SIMINCLUDE 	= 	-I. -I../sim -I../shell -I../ss -I$(SYS)
LIBRARY_NAME 	= 	olf
FUNCTIONS 	= 	olffuncs
STARTUP		=	olflib.g
STRUCTURES 	= 	olf_struct.h
EXT_HEADER	=	olf_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	olflib.$(LIBEXT)

default: $(TARGET_OBJ) 

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	\
		receptor.o \
		receptor2.o \
		disk_in.o \
		tabgate.o \
		new_interp.o \
		table.o \
		tabchannel.o \
		dd_syn.o \
		interp2d.o \
		tab2channel.o \
		table2d.o \
		tabcurrent.o \
		tabfiles.o


HEADERS = 	\
		olf_ext.h \
		olf_defs.h \
		olf_struct.h

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

$(OBJECTS) : $(HEADERS)

# make the data structure section of the symbol table

$(LIBRARY_NAME)_g@.c $(LIBRARY_NAME)_g@.h: $(STARTUP)
	- $(SYS)/code_g $(STARTUP) $(EXT_HEADER) $(LIBRARY_NAME)

# make the function list section of the symbol table

$(LIBRARY_NAME)_d@.c : $(STRUCTURES)
	- $(CPP) $(SIMINCLUDE) $(STRUCTURES) > $(TMPDIR)/$(STRUCTURES)
	- $(SYS)/code_sym $(TMPDIR)/$(STRUCTURES) $(LIBRARY_NAME) \
	  -I $(EXT_HEADER) -NI -o $(LIBRARY_NAME)_d@.c
	- rm $(TMPDIR)/$(STRUCTURES)

$(LIBRARY_NAME)_f@.c : $(FUNCTIONS)
	- $(SYS)/code_func $(FUNCTIONS) $(LIBRARY_NAME) \
	  > $(LIBRARY_NAME)_f@.c

# make the library header function
$(LIBRARY_NAME)_l@.c : $(LIBRARY_NAME)_d@.c $(LIBRARY_NAME)_g@.c $(OBJECTS)
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
