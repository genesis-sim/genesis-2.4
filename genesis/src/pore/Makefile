# $Id: Makefile,v 1.3 2005/07/29 15:52:47 svitak Exp $
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
# Revision 1.17  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.16  2000/07/13 18:58:03  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.15  2000/07/03 18:06:30  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.14  1999/11/29 07:50:16  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.13  1999/10/17 23:56:18  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.12  1996/05/23 23:00:22  dhb
# t3d/e port
#
# Revision 1.11  1995/11/02  01:42:25  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.10  1995/11/01  22:33:44  venkat
# Removed-install-target-dependencies
#
# Revision 1.9  1995/02/13  22:50:48  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.8  1994/12/19  22:37:27  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.7  1994/03/21  05:49:33  dhb
# Changes for C code startups.
#
# Revision 1.6  1993/09/17  16:33:24  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:25:49  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:42:54  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:26:54  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:56:43  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:03:54  dhb
# Initial revision
#

#
# Makefile for porelib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SYS		=	../sys
SIMINCLUDE 	= 	-I. -I../ss -I../sim -I../shell -I$(SYS) 
LIBRARY_NAME 	= 	pore
FUNCTIONS 	= 	porefuncs
STARTUP		=	porelib.g
STRUCTURES 	= 	pore_struct.h
EXT_HEADER	=	pore_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	porelib.$(LIBEXT)

default: $(TARGET_OBJ) 

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	\
		Napore.o \
		Kpore.o \
		prim.o


HEADERS = 	\
		pore_ext.h \
		pore_struct.h

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

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS)
	cd examples/ConClamp; rcsfreeze $(VERSION)
	cd examples/UnClamp; rcsfreeze $(VERSION)
	cd examples/VClamp; rcsfreeze $(VERSION)

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)
