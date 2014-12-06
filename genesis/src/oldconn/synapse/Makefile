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
# Revision 1.18  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.17  2000/07/13 18:57:12  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.16  2000/07/03 18:06:28  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.15  1999/11/29 07:50:12  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.14  1996/07/16 22:22:04  dhb
# TARGET_OBJ wasn't using LIBEXT for target file extension.
#
# Revision 1.13  1996/05/23  23:05:42  dhb
# t3d/e port
#
# Revision 1.12  1995/11/02  01:42:53  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.11  1995/11/01  22:34:21  venkat
# Removed-install-target-dependencies
#
# Revision 1.10  1995/02/13  22:36:01  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files
#
# Revision 1.9  1994/12/19  22:33:45  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.8  1994/09/16  23:00:52  dhb
# Added a .. to the include directories path.  This directory is another
# level deeper now.
#
# Revision 1.7  1994/03/21  05:53:14  dhb
# Changes for C code startups.
#
# Revision 1.6  1993/09/17  19:52:18  dhb
# Changed CPP macro to use CPP from the top level Makefile.
#
# Revision 1.5  1993/07/17  00:33:25  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:56:00  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:32:26  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:59:51  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:05:24  dhb
# Initial revision
#

#
# make synapselib.o
#
LIBS 		=  	-lm -ltermcap
CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
SIM 		= 	../../sim
SS 		= 	../../ss
SYS 		= 	../../sys
INTERP 		= 	../../shell
SIMINCLUDE 	= 	-I. -I$(SIM) -I$(SS) -I$(SYS) -I$(INTERP) -I../../include
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBRARY_NAME 	= 	synapse
FUNCTIONS 	= 	synfuncs
STARTUP		=	synlib.g
STRUCTURES 	= 	syn_struct.h
EXT_HEADER	=	syn_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	echo no need to order
LIBEXT		=	o
TARGET_OBJ	=	synlib.$(LIBEXT)

default: $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS = \
		simple.o \
		synapseA.o \
		synapseB.o \
		synshow.o

HEADERS = 	\
		syn_struct.h 

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

.c.s:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -S

# $(OBJECTS) : $(HEADERS)

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
