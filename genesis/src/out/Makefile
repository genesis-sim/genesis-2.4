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
# Revision 1.20  2001/04/25 17:16:59  mhucka
# Misc. small changes to improve portability and address compiler warnings.
#
# Revision 1.19  2000/07/13 18:57:53  mhucka
# Removed RCS directives, because we don't use RCS anymore.
#
# Revision 1.18  2000/07/03 18:06:29  mhucka
# Added LDFLAGS to LIBBUILDFLAGS.
#
# Revision 1.17  1999/11/29 07:50:14  mhucka
# Removed needless subshell () wrappers and performed other related cleanup.
#
# Revision 1.16  1999/10/17 23:56:17  mhucka
# Fixed silly annoying duplicate messages about "no lib ordering needed"
# during compilation.  It just required putting an '@' sign in front of
# the echo command for that message.
#
# Revision 1.15  1997/05/29 08:42:32  dhb
# Update from Antwerp GENESIS version 21e
#
# Version EDS21b, 96/08/02 */
# Added spikehistory
#
# Version EDS17, Erik De Schutter, Caltech 1991 */
# Added res_ascfile
#
# Revision 1.14  1996/05/23 23:00:06  dhb
# t3d/e port
#
# Revision 1.13  1995/11/02  01:42:21  venkat
# Replaced-/tmp-with-TMPDIR-macro
#
# Revision 1.12  1995/11/01  22:33:41  venkat
# Removed-install-target-dependencies
#
# Revision 1.11  1995/02/13  22:47:20  dhb
# Removed checkout of RCSRELEASE versions.
# Removed dependencies on sys_code_... files.
# Added @.h files to those removed by clean.
#
# Revision 1.10  1994/12/19  22:36:08  dhb
# Added rules for RCS subdirectory.
#
# Revision 1.9  1994/03/21  05:38:27  dhb
# make code_g run first
#
# Revision 1.8  1994/03/20  06:34:03  dhb
# *** empty log message ***
#
# Revision 1.7  1994/03/20  06:06:48  dhb
# Changes for C code startups
#
# Revision 1.6  1993/09/17  16:32:10  dhb
# Use CPP macro defined from top level Makefile.
#
# Revision 1.5  1993/07/17  00:21:47  dhb
# Separate clean and rcsclean targets
#
# Revision 1.4  1993/07/01  15:41:16  dhb
# Added RCSRELEASE stuff
#
# Revision 1.3  1993/02/12  17:22:06  dhb
# added freeze target to create rcs configurations
# added rcsclean to clean target
# both targets ensure .h .g Makefile and funcs files exist
#
# Revision 1.2  1992/12/11  19:56:15  dhb
# Added RCS check out rules
#
# Revision 1.1  1992/12/11  19:03:32  dhb
# Initial revision
#

#
# make outlib.o
#
LIBS 		=  	-lm -ltermcap
CFLAGS 		= 	$(CFLAGS_IN) -D$(MACHINE)
SIM 		= 	../sim
SS 		= 	../ss
SYS 		= 	../sys
INTERP 		= 	../shell
SIMINCLUDE 	= 	-I. -I$(SIM) -I$(SS) -I$(SYS) -I$(INTERP)
INSTALL_LIB 	= 	$(INSTALLDIR)/lib
INSTALL_INCLUDE	= 	$(INSTALLDIR)/include
INSTALL_STARTUP	= 	$(INSTALLDIR)/startup
LIBRARY_NAME 	= 	output
FUNCTIONS 	= 	outfuncs
STARTUP		=	outlib.g
STRUCTURES 	= 	out_struct.h
EXT_HEADER	=	out_ext.h
LIBBUILD	=	$(LD)
LIBBUILDFLAGS	=	$(LDFLAGS) -r -o
LIBORDER	=	@echo no lib ordering needed for
LIBEXT		=	o
TARGET_OBJ	=	outlib.$(LIBEXT)

default: $(TARGET_OBJ)

.SUFFIXES: .c,v .c .h,v .h

.h,v.h:
	co $*.h

.c,v.o:
	co $*.c
	$(CC) -c $(CFLAGS) $(SIMINCLUDE) $*.c
	rm -f $*.c

WIDG_OBJECTS = \
		out_graph2.o \
		out_plot.o \
		out_xview.o
#WIDG_OBJECTS = 

OUT_OBJECTS = \
		out_ascfile.o \
		out_ascii.o \
		out_add.o \
		out_delete.o \
		out_datasize.o \
		out_file.o \
		out_file2.o \
		out_print.o \
		out_script.o \
		out_view.o \
		res_ascfile.o \
		spike_history.o

# OBJECTS = $(WIDG_OBJECTS) $(OUT_OBJECTS)
OBJECTS = $(OUT_OBJECTS)

HEADERS = 	\
		out_defs.h \
		out_struct.h 

.c.o:
	$(CC) $(CFLAGS) $(SIMINCLUDE) $< -c 

$(OUT_OBJECTS) : $(HEADERS)

# $(WIDG_OBJECTS) : $(HEADERS)

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
