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
# Revision 1.1.1.1  2005/06/14 04:38:33  svitak
# Import from snapshot of CalTech CVS tree of June 8, 2005
#
# Revision 1.23  2000/07/13 18:59:17  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.22  2000/07/03 18:06:31  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.21  1999/11/29 07:50:22  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.20  1999/10/17 23:56:19  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.19  1998/07/22 00:06:09  dhb
# Moved randfield.c from oldconn personal library
#
# Revision 1.18  1998/06/30 23:34:41  dhb
# EDS 22d merge
#
# EDS22c: revision 97/12/02: added device and hines to lib path
#
# Revision 1.17  1996/05/23 23:01:49  dhb
# t3d/e port
#
# Revision 1.16  1995/11/02  01:42:35  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.15  1995/11/01  22:34:02  venkat
# Removed-install-target-dependencies
#
# Revision 1.14  1995/02/13  23:01:51  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys/code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.13  1994/12/19  22:38:55  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.12  1994/08/08  22:24:00  dhb
# Changes from Upi.  I changed nmin2.o to nmin.o in OBJECTS.
#
# Revision 1.12  1994/06/13  22:47:12  bhalla
# Added lftab, rmsmatch and nmin2 files
#
# Revision 1.11  1994/05/31  22:35:11  dhb
# Added -I../newconn to include directories for new synapse and spike
# objects.
#
# Revision 1.10  1994/03/21  05:53:51  dhb
# make code_g run first
#
# Revision 1.9  1994/03/20  06:37:19  dhb
# *** empty log message ***
#
# Revision 1.8  1994/03/20  05:54:54  dhb
# Typo
#
# Revision 1.7  1994/03/20  05:47:38  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  21:05:58  dhb
# Changed CPP macro to use CPP from the top level Makefile.
#
# Revision 1.5  1993/07/17  00:37:43  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:58:47  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:34:23  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  20:00:24  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:06:20  dhb
# Initial revision
#

#
# Makefile for toollib.o
#

CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
SYS			= ../sys
SIMINCLUDE 	= 	-I. -I../sys -I../shell -I../sim -I../ss -I../segment -I../hh -I../buffer -I../olf  -I../concen -I../newconn -I../hines -I../device
LIBRARY_NAME 	= 	tools
FUNCTIONS 	= 	toolfuncs
STARTUP		=	toollib.g
STRUCTURES 	= 	tool_struct.h
EXT_HEADER	=	tool_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	= 	toollib.$(LIBEXT)

default: $(TARGET_OBJ) 

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

OBJECTS =	\
		NIH_parser.o \
		brent.o \
		cell_parms.o \
		cell_sheet.o \
		conn_3d.o \
		coord_geom.o \
		covsrt.o \
		curvefit3.o \
		egg.o \
		emrqmin.o \
		gaussj.o \
		lftab.o \
		lump_p.o \
		mrqcof.o \
		mrqmin.o \
		new_parser.o \
		nrutil.o \
		plane.o \
		rall_calc.o \
		rand_comp.o \
		randfield.o \
		readplot.o \
		spline.o \
		traverse.o \
		zbrent.o

HEADERS = 	\
		tools.h 

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

clean:
	-rm -f *.a *.o *@.[ch]

rcsclean: clean
	- rcsclean
	co Makefile *.h,v *.g,v $(FUNCTIONS)
