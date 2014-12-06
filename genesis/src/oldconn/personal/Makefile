# $Id: Makefile,v 1.3 2005/07/29 15:53:05 svitak Exp $
# $Log: Makefile,v $
# Revision 1.3  2005/07/29 15:53:05  svitak
# Changed INSTALL to INSTALLDIR to avoid confusion with commonly used INSTALL
# executable.
#
# Eliminated use of COPT in favor of CFLAGS_IN.
#
# Eliminated unused DEC_HACK.
#
# Revision 1.2  2005/07/20 19:07:50  svitak
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
# Revision 1.22  2000/07/13 18:56:43  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.21  2000/07/03 18:06:27  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.20  1999/11/29 07:50:12  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.19  1998/07/21 23:36:00  dhb
# Moved randfield.c to tools.
#
# Revision 1.18  1996/07/16 22:21:01  dhb
# TARGET_OBJ wasn't using LIBEXT for target file extension.
#
# Revision 1.17  1996/05/23  23:02:39  dhb
# t3d/e port
#
# Revision 1.16  1995/11/02  01:42:50  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.15  1995/11/01  22:34:18  venkat
# Removed-install-target-dependencies
#
# Revision 1.14  1995/02/13  21:56:50  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.13  1995/01/25  02:32:44  dhb
# Added to install target to install tools.g script to startup directory.
#
# Revision 1.12  1994/12/19  22:32:28  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.11  1994/09/16  23:00:03  dhb
# Added a .. to the include directories path.  This directory is another
# level deeper now.
#
# Revision 1.10  1994/03/21  05:38:54  dhb
# make code_g run first
#
# Revision 1.9  1994/03/20  07:15:12  dhb
# *** empty log message ***
#
# Revision 1.8  1994/03/20  06:35:10  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:03:37  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:32:45  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:22:16  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:42:22  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:22:33  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:56:30  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:03:46  dhb
# Initial revision
#

#
# Makefile for perlib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
SIM 		= 	../../sim
SYS 		= 	../../sys
INTERP 		= 	../../shell
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SIMINCLUDE 	= 	-I. -I$(SYS) -I$(SIM) -I$(INTERP)
LIBRARY_NAME 	= 	personal
FUNCTIONS 	= 	perfuncs
STARTUP		=	perlib.g
STRUCTURES 	= 	per_struct.h
EXT_HEADER	=	per_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no need to order
LIBEXT		=	o
TARGET_OBJ	=	perlib.$(LIBEXT)

default : $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS = 	\
		adjsyn.o \
		affdelay.o \
		affweight.o \
		expsum.o \
		expweight.o \
		gaussweight.o \
		raddelay.o \
		scaleweight.o \
		spatial.o

HEADERS = 	\
		per.h 		\
		per_defs.h	\
		per_struct.h	\
		per_ext.h	

# $(OBJECTS) : $(HEADERS) 

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
	-cp tools.g $(INSTALL_STARTUP)

freeze:
	rcsclean
	rcsfreeze $(VERSION)
	co Makefile *.h,v *.g,v $(FUNCTIONS)

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)
